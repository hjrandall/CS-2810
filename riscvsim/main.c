#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "elf.h"
#include "loadstore.h"
#include "riscv.h"

int main(int argc, char **argv) {
    char *filename;

    // default to "riscvprogram" as the executable file,
    // or let the user specify a name on the command line
    if (argc == 1) {
        filename = "riscvprogram";
    } else if (argc == 2) {
        filename = argv[1];
    } else {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // load the program from disk and form the
    // simulated address space
    struct address_space *address_space = load_elf(filename);
    if (!address_space) {
        return 1;
    }

    // set the initial cpu state
    struct cpu *cpu = malloc(sizeof(struct cpu));
    assert(cpu != 0);
    memset(cpu, 0, sizeof(struct cpu));
    cpu->pc = address_space->pc_start;
    cpu->x[2] = address_space->memory_end;
    uint64_t instruction_counter = 0;

    // main loop
    bool keep_going = true;
    while (keep_going) {
        // reset the zero register after every instruction
        cpu->x[0] = 0;

        // fetch the next instruction
        int32_t instruction = lw(address_space, cpu->pc);

        // execute
        keep_going = execute(address_space, cpu, instruction);

        instruction_counter++;
    }

    printf("executed %lu instructions\n", instruction_counter);
    free(address_space->memory);
    free(address_space);
    free(cpu);
    return 0;
}
