#pragma once

#include <stdint.h>
#include <stdbool.h>

struct cpu {
    int32_t x[32];
    int32_t pc;
};

struct address_space {
    int8_t *memory;
    int32_t memory_start;
    int32_t memory_end;
    int32_t pc_start;
};

bool execute(struct address_space *mem, struct cpu *cpu, int32_t instruction);

bool execute_lui(struct cpu *cpu, int8_t rd, int32_t imm);
bool execute_auipc(struct cpu *cpu, int8_t rd, int32_t imm);
bool execute_jal(struct cpu *cpu, int8_t rd, int32_t imm);
bool execute_jalr(struct cpu *cpu, int8_t funct3, int8_t rd, int8_t rs1, int32_t imm);
bool execute_branches(struct cpu *cpu, int8_t funct3, int8_t rs1, int8_t rs2, int32_t imm);
bool execute_load(struct address_space *mem, struct cpu *cpu, int8_t funct3, int8_t rd, int8_t rs1, int32_t imm);
bool execute_store(struct address_space *mem, struct cpu *cpu, int8_t funct3, int8_t rs1, int8_t rs2, int32_t imm);
bool execute_alu_imm(struct cpu *cpu, int8_t funct3, int8_t rd, int8_t rs1, int32_t imm);
bool execute_alu(struct cpu *cpu, int8_t funct3, int8_t rd, int8_t rs1, int8_t rs2, int8_t funct7);
bool execute_ecall(struct address_space *mem, struct cpu *cpu);
