#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "loadstore.h"
#include "riscv.h"

// load a byte (8 bits) from the simulated address space
int8_t lb(struct address_space *mem, int32_t address) {
    if (address < mem->memory_start || address >= mem->memory_end) {
        printf("Segmentation fault in lb: address is %d\n", address);
        exit(1);
    }
    return mem->memory[address - mem->memory_start];
}

// load a half word (16 bits) from the simulated address space
// in little-endian format
// address must be a multiple of 2
int16_t lh(struct address_space *mem, int32_t address) {
    if ((address & 1) != 0) {
        printf("Bus error in lh: address is %d\n", address);
        exit(1);
    }
    if (address < mem->memory_start || address+1 >= mem->memory_end) {
        printf("Segmentation fault in lh: address is %d\n", address);
        exit(1);
    }
    int16_t result = mem->memory[address - mem->memory_start] & 0xff;
    result |= ((int16_t) mem->memory[address+1 - mem->memory_start]) << 8;
    return result;
}

// load a word (32 bits) from the simulated address space
// in little-endian format
// address must be a multiple of 4
int32_t lw(struct address_space *mem, int32_t address) {
    if ((address & 3) != 0) {
        printf("Bus error in lw: address is %d\n", address);
        exit(1);
    }
    if (address < mem->memory_start || address+3 >= mem->memory_end) {
        printf("Segmentation fault in lw: address is %d\n", address);
        exit(1);
    }
    int32_t result = mem->memory[address - mem->memory_start] & 0xff;
    result |= ((int32_t) mem->memory[address+1 - mem->memory_start] & 0xff) << 8;
    result |= ((int32_t) mem->memory[address+2 - mem->memory_start] & 0xff) << 16;
    result |= ((int32_t) mem->memory[address+3 - mem->memory_start]) << 24;
    return result;
}

// store a byte (8 bits) to the simulated address space
void sb(struct address_space *mem, int32_t address, int8_t value) {
    if (address < mem->memory_start || address >= mem->memory_end) {
        printf("Segmentation fault in sb: address is %d\n", address);
        exit(1);
    }
    mem->memory[address - mem->memory_start] = value;
}

// store a half word (16 bits) to the simulated address space
// in little-endian format
// address must be a multiple of 2
void sh(struct address_space *mem, int32_t address, int16_t value) {
    if ((address & 1) != 0) {
        printf("Bus error in sh: address is %d\n", address);
        exit(1);
    }
    if (address < mem->memory_start || address+1 >= mem->memory_end) {
        printf("Segmentation fault in sh: address is %d\n", address);
        exit(1);
    }
    int8_t byte = value & 0xff;
    mem->memory[address   - mem->memory_start] = byte;
    byte = (value>>8) & 0xff;
    mem->memory[address+1 - mem->memory_start] = byte;
}

// store a half word (16 bits) to the simulated address space
// in little-endian format
// address must be a multiple of 2
void sw(struct address_space *mem, int32_t address, int32_t value) {
    if ((address & 3) != 0) {
        printf("Bus error in sw: address is %d\n", address);
        exit(1);
    }
    if (address < mem->memory_start || address+3 >= mem->memory_end) {
        printf("Segmentation fault in sw: address is %d\n", address);
        exit(1);
    }
    int8_t byte = value & 0xff;
    mem->memory[address   - mem->memory_start] = byte;
    byte = (value>>8) & 0xff;
    mem->memory[address+1 - mem->memory_start] = byte;
    byte = (value>>16) & 0xff;
    mem->memory[address+2 - mem->memory_start] = byte;
    byte = (value>>24) & 0xff;
    mem->memory[address+3 - mem->memory_start] = byte;
}
