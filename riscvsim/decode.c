#include <stdint.h>
#include "decode.h"

// you should implement all of the functions prototyped in decode.h
// note: we will go over these in detail in class
// be sure to take good notes!

int8_t get_funct3(int32_t inst){
    return (inst>>12) & 0x07;
}
int8_t get_rd(int32_t inst){
   return (inst>>7) & 0x1f;
}
int8_t get_rs1(int32_t inst){
   return (inst>>15) & 0x1f;
}
int8_t get_rs2(int32_t inst){
    return (inst>>20) & 0x1f;
}

int32_t get_imm_i(int32_t inst){
    return inst>>20;
}

// int32_t get_imm_s(int32_t inst){
//     return  inst & 0x1f;
// }
int8_t get_funct7(int32_t inst){
    return (inst>>25);
}

int32_t get_imm_u(int32_t inst){
    return inst & 0xfffff000;

}
int32_t get_imm_b(int32_t inst){
    return ((inst>>20)  & 0xfffff000)|
           ((inst<<4 )  & 0x00000800)|
           ((inst>>20)  & 0x000007e0)|
           ((inst>>7 )  & 0x0000001e)|
           ((inst       & 0x00000000));

}
int32_t get_imm_j(int32_t inst){
    return ((inst>>12)  & 0xfff00000)|
           ((inst    )  & 0x000ff000)|
           ((inst>>9 )  & 0x00000800)|
           ((inst>>21)  & 0x000007e0)|
           ((inst>>20)  & 0x0000001e)|
           ((inst       & 0x00000000));

}

int32_t get_imm_s(int32_t inst){
    return ((inst>>7 ) & 0x0000001f)|
           ((inst>>20) & 0x00000fe0);
}
