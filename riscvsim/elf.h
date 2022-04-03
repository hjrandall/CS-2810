#pragma once

#include <stdint.h>
#include "riscv.h"

struct elf_header {
    uint8_t e_ident_magic[4];   // 0x7f 'E' 'L' 'F'
    uint8_t e_ident_class;      // 2 for 64-bit
    uint8_t e_ident_data;       // 1 for little endian
    uint8_t e_ident_version;    // 1 for current version
    uint8_t e_ident_osabi;      // 0x00 for System V
    uint8_t e_ident_abiversion; // ignored
    uint8_t e_ident_pad[7];     // padding

    uint8_t e_type[2];          // 0x02 for executable
    uint8_t e_machine[2];       // 0xf3 for RISC-V
    uint8_t e_version[4];       // 1 for original version of elf
    uint8_t e_entry[4];         // entry point PC

    uint8_t e_phoff[4];         // program header offset (0x40)
    uint8_t e_shoff[4];         // section header offset

    uint8_t e_flags[4];         // target architecture specific
    uint8_t e_ehsize[2];        // size of this header (64 bytes)
    uint8_t e_phentsize[2];     // size of a program header entry
    uint8_t e_phnum[2];         // number of program header entries
    uint8_t e_shentsize[2];     // size of a section header entry
    uint8_t e_shnum[2];         // number of section header entries
    uint8_t e_shstrndx[2];      // index of section header with section names
};

struct elf_program_header {
    uint8_t p_type[4];          // 1 == loadable segment
    uint8_t p_offset[4];        // offset of the segment in the file image
    uint8_t p_vaddr[4];         // virtual address of the segment in memory
    uint8_t p_paddr[4];         // physical address (when relevant)
    uint8_t p_filesz[4];        // size in the file image (may be 0)
    uint8_t p_memsz[4];         // size in memory (may be 0)
    uint8_t p_flags[4];
    uint8_t p_align[4];
};

struct address_space *load_elf(char *filename);
