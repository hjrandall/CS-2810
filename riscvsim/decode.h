#pragma once

#include <stdint.h>

int8_t get_funct3(int32_t inst);
int8_t get_rd(int32_t inst);
int8_t get_rs1(int32_t inst);
int8_t get_rs2(int32_t inst);
int32_t get_imm_i(int32_t inst);
int32_t get_imm_s(int32_t inst);
int32_t get_imm_b(int32_t inst);
int32_t get_imm_u(int32_t inst);
int32_t get_imm_j(int32_t inst);
int8_t get_funct7(int32_t inst);
