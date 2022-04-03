#pragma once

#include <stdint.h>
#include "riscv.h"

int8_t lb(struct address_space *mem, int32_t address);
int16_t lh(struct address_space *mem, int32_t address);
int32_t lw(struct address_space *mem, int32_t address);
void sb(struct address_space *mem, int32_t address, int8_t value);
void sh(struct address_space *mem, int32_t address, int16_t value);
void sw(struct address_space *mem, int32_t address, int32_t value);
