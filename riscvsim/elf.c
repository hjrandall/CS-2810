#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "riscv.h"
#include "elf.h"

static uint16_t read_uint16_t(uint8_t *raw) {
    return
        (((uint16_t) raw[0])) |
        (((uint16_t) raw[1]) << 8);
}

static uint32_t read_uint32_t(uint8_t *raw) {
    return
        (((uint32_t) raw[0])) |
        (((uint32_t) raw[1]) << 8) |
        (((uint32_t) raw[2]) << 16) |
        (((uint32_t) raw[3]) << 24);
}

#define failif(cond, msg) if (cond) { fprintf(stderr, msg); exit(1); }
#define failif_errno(cond, msg) if (cond) { perror(msg); exit(1); }

// Read an executable file into memory
// for details of the file format, see:
// https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
struct address_space *load_elf(char *filename) {
    // open the file
    int fd = open(filename, O_RDONLY);
    failif_errno(fd < 0, "opening file");

    // read the elf header
    assert(sizeof(struct elf_header) == 0x34);
    struct elf_header header;
    ssize_t read_res = read(fd, &header, sizeof(header));
    failif_errno(read_res < 0, "reading elf header");
    failif(read_res != sizeof(struct elf_header), "failed to read full elf header\n");

    // validate the header
    failif(header.e_ident_magic[0] != 0x7f ||
            header.e_ident_magic[1] != 'E' ||
            header.e_ident_magic[2] != 'L' ||
            header.e_ident_magic[3] != 'F',
            "file is not ELF format\n");
    failif(header.e_ident_class != 1, "file is not 32 bit\n");
    failif(header.e_ident_data != 1, "file is not little endian\n");
    failif(header.e_ident_version != 1, "file is not a version 1 format\n");
    failif(header.e_ident_osabi != 0, "file is not System V ABI\n");
    failif(read_uint16_t(header.e_type) != 2, "file is not an executable\n");
    failif(read_uint16_t(header.e_machine) != 0xf3, "file is not RISC-V\n");
    failif(read_uint32_t(header.e_version) != 1, "file is not elf version 1\n");
    uint32_t entry_pc = read_uint32_t(header.e_entry);
    failif(read_uint16_t(header.e_ehsize) != sizeof(struct elf_header),
        "file header is not expected size\n");
    uint32_t phoff = read_uint32_t(header.e_phoff);
    uint16_t phentsize = read_uint16_t(header.e_phentsize);
    failif(phentsize != sizeof(struct elf_program_header), "program header size mismatch\n");
    uint16_t phnum = read_uint16_t(header.e_phnum);

    // create the address space struct
    struct address_space *as = malloc(sizeof(struct address_space));
    failif(!as, "malloc failed\n");
    as->memory = 0;
    as->memory_start = 0;
    as->memory_end = 0;
    as->pc_start = entry_pc;

    // read program headers into memory
    assert(sizeof(struct elf_program_header) == 0x20);
    struct elf_program_header *program_headers = malloc(sizeof(struct elf_program_header) * phnum);
    failif(!program_headers, "malloc failed\n");
    off_t seek_res = lseek(fd, phoff, SEEK_SET);
    failif_errno(seek_res < 0, "seeking program headers");
    failif((uint32_t) seek_res != phoff,
        "unexpected offset when looking for program headers\n");
    read_res = read(fd, program_headers, phentsize * phnum);
    failif_errno(read_res < 0, "reading program headers");
    failif(read_res != phentsize * phnum,
        "read wrong number of bytes in program headers\n");

    // first pass--find how big the VM footprint is
    for (uint32_t i = 0; i < phnum; i++) {
        if (read_uint32_t(program_headers[i].p_type) != 1)
            continue;
        int32_t start = (int32_t) read_uint32_t(program_headers[i].p_vaddr);
        int32_t end = start + (int32_t) read_uint32_t(program_headers[i].p_memsz);
        if (as->memory_end == 0 || start < as->memory_start)
            as->memory_start = start;
        if (end > as->memory_end)
            as->memory_end = end;
    }

    // round to page boundaries at both ends
    as->memory_start &= ~4095;
    as->memory_end = (as->memory_end + 4095) & ~4095;
    failif(as->memory_end <= as->memory_start, "program has no loadable content\n");

    // allocate a page of stack
    as->memory_end += 4096;

    // printf("allocated %dk address space from %d to %d including 4k stack\n",
    //     (as->memory_end - as->memory_start) / 1024,
    //     as->memory_start, as->memory_end);

    // allocate program memory
    as->memory = malloc(as->memory_end - as->memory_start);
    failif(!as->memory, "malloc failed\n");
    memset(as->memory, 0, as->memory_end - as->memory_start);

    // second pass--load the segments
    for (uint32_t i = 0; i < phnum; i++) {
        //printf("program header %d\n", i+1);
        if (read_uint32_t(program_headers[i].p_type) != 1) {
            //printf("    skipping non-loadable segment\n");
            continue;
        }
        uint32_t f_start = read_uint32_t(program_headers[i].p_offset);
        uint32_t f_size = read_uint32_t(program_headers[i].p_filesz);
        //printf("    loading segment from offset %d with size %d\n", f_start, f_size);
        uint32_t v_start = read_uint32_t(program_headers[i].p_vaddr);
        uint32_t v_size = read_uint32_t(program_headers[i].p_memsz);
        (void) v_size;
        //printf("    into virtual address %d with size %d\n", v_start, v_size);

        if (!f_size) {
            //printf("    (nothing to load... skipping to next segment)\n");
            continue;
        }
        off_t seek_res = lseek(fd, f_start, SEEK_SET);
        failif_errno(seek_res < 0, "seeking program segment");
        failif((uint32_t) seek_res != f_start,
            "unexpected offset when looking for program segment\n");
        read_res = read(fd, as->memory + (v_start - as->memory_start), f_size);
        failif_errno(read_res < 0, "reading program segment");
        failif((uint32_t) read_res != f_size,
            "read wrong number of bytes in program segment\n");
    }

    // done with program headers and done with executable file
    free(program_headers);
    close(fd);

    return as;
}
