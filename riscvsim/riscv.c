#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include "decode.h"
#include "loadstore.h"
#include "riscv.h"

// ABI register names
//
// x0: zero
// x1: ra
// x2: sp
// x3: gp
// x4: tp
// x5-x7: t0-t2
// x8: s0/fp (we use s0)
// x9: s1
// x10-17: a0-7
// x18-27: s2-11
// x28-31: t3-6
static char *r[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
};

// a specialized version of printf that pads output to 32 characters
__attribute__((format(printf, 1, 2)))
static void printf32(const char *format, ...) {
    char buffer[32];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    int len = strlen(buffer);
    assert(len < 32);
    printf("%-32s", buffer);
}

// Execute a single instruction
// return false if the program should terminate, true if it should keep going
bool execute(struct address_space *mem, struct cpu *cpu, int32_t inst) {
    bool okay = false;

    // print the address and the raw instruction bits
    printf("%6d: %08x    ", cpu->pc, inst);

    // divide the instructions up according to the main instruction opcode,
    // then call a specialized function for each opcode to do the rest
    int8_t opcode = inst & 0x7f;
    switch (opcode) {
    case 0x3:

        okay =execute_load(mem,cpu, get_funct3(inst), get_rd(inst), get_rs1(inst), get_imm_i(inst));
        cpu->pc+=4;
        break;


    //add i
    case 0x13:

            okay=execute_alu_imm(cpu,get_funct3(inst),get_rd(inst),get_rs1(inst),get_imm_i(inst));
            cpu->pc+=4;
            break;
    //auipc
    case 0x17:

            okay=execute_auipc(cpu,get_rd(inst),get_imm_u(inst));
            cpu->pc+=4;
            break;

    case 0x23:
        okay=execute_store(mem, cpu, get_funct3(inst), get_rs1(inst), get_rs2(inst), get_imm_s(inst));
        cpu->pc+=4;
        break;
    //add
    case 0x33:
            okay=execute_alu(cpu, get_funct3(inst), get_rd(inst), get_rs1(inst), get_rs2(inst), get_funct7(inst));
            cpu->pc+=4;
            break;

    //lui
    case 0x37:
        okay=execute_lui(cpu, get_rd(inst), get_imm_u(inst));
        cpu->pc+=4;
        break;

    //branches
    case 0x63:
        okay = execute_branches(cpu, get_funct3(inst), get_rs1(inst), get_rs2(inst), get_imm_b(inst));
        break;
    //jalr
    case 0x67:
        okay= execute_jalr(cpu, get_funct3(inst), get_rd(inst), get_rs1(inst), get_imm_i(inst));
        break;
    //ebreak
    case 0x73:
        if (inst == 0x00000073) {
            okay = execute_ecall(mem, cpu);
            cpu->pc += 4;

            // if ecall was an exit syscall we quit gracefully
            return okay;
        } else if (inst == 0x00100073) {
            // handle ebreak here--print the instruction and exit
            printf("ebreak\n");
            return false;
        }
        break;

    //jal
    case 0x6f:
        okay= execute_jal(cpu, get_rd(inst),get_imm_j(inst));
        break;
    }

    if (!okay)
        printf("unknown instruction\n");
    return okay;
}
bool execute_store(struct address_space *mem, struct cpu *cpu, int8_t funct3, int8_t rs1, int8_t rs2, int32_t imm){
    switch(funct3){
        //sb
        case 0:
            if (imm!=0){
                printf32("sb      %s, %d(%s)", r[rs2], imm,r[rs1]);
            }
            else{
                printf32("sb      %s, (%s)", r[rs2], r[rs1]);
            }

            sb(mem,cpu->x[rs1]+imm,(int8_t)cpu->x[rs2]);
            printf("[%d] <- %d\n", cpu->x[rs1]+imm,(int8_t)cpu->x[rs2]);
            return true;
        //sh
        case 1:
            if (imm!=0){
                printf32("sh      %s, %d(%s)", r[rs2],imm, r[rs1]);
            }
            else{
                printf32("sh      %s, (%s)", r[rs2], r[rs1]);
            }
            sh(mem,cpu->x[rs1]+imm,(int16_t)cpu->x[rs2]);
            printf("[%d] <- %d\n", cpu->x[rs1]+imm,(int16_t)cpu->x[rs2]);
            return true;

        //sw
        case 2:
            if (imm!=0){
                printf32("sw      %s, %d(%s)", r[rs2],imm, r[rs1]);
            }
            else{
                printf32("sw      %s, (%s)", r[rs2], r[rs1]);
            }
            sw(mem,cpu->x[rs1]+imm,(int32_t)cpu->x[rs2]);
            printf("[%d] <- %d\n", cpu->x[rs1]+imm,(int32_t)cpu->x[rs2]);
            return true;

    }
    return false;
}
bool execute_load(struct address_space *mem, struct cpu *cpu, int8_t funct3, int8_t rd, int8_t rs1, int32_t imm){
    switch(funct3){
        //lb
        case 0:
            if (imm==0){
                printf32("lb      %s, (%s)", r[rd], r[rs1]);
            }
            else{
                printf32("lb      %s, %d(%s)", r[rd],imm, r[rs1]);
            }
            cpu->x[rd]=(lb(mem,cpu->x[rs1]+imm));
            printf("%s <- %d [%d]\n", r[rd],cpu->x[rd],cpu->x[rs1]+imm);
            return true;

        //lh
        case 1:
            if (imm==0){
                printf32("lh      %s, (%s)", r[rd], r[rs1]);
            }
            else{
                printf32("lh      %s, %d(%s)", r[rd],imm, r[rs1]);
            }
            cpu->x[rd]=(lh(mem,cpu->x[rs1]+imm));
            printf("%s <- %d [%d]\n", r[rd],cpu->x[rd],cpu->x[rs1]+imm);
            return true;

        //lw
        case 2:
            if (imm==0){
                printf32("lw      %s, (%s)", r[rd], r[rs1]);
            }
            else{
                printf32("lw      %s, %d(%s)", r[rd],imm, r[rs1]);
            }
            cpu->x[rd]=(lw(mem,cpu->x[rs1]+imm));
            printf("%s <- %d [%d]\n", r[rd],cpu->x[rd],cpu->x[rs1]+imm);
            return true;

        //lbu
        case 4:
            if (imm==0){
                printf32("lbu     %s, (%s)", r[rd], r[rs1]);
            }
            else{
                printf32("lbu     %s, %d(%s)", r[rd],imm, r[rs1]);
            }
            cpu->x[rd]=((uint32_t)(lb(mem,cpu->x[rs1]+imm)));
            printf("%s <- %d [%d]\n", r[rd],cpu->x[rd],cpu->x[rs1]+imm);
            return true;

        //lhu
        case 5:
            if (imm==0){
                printf32("lhu     %s, (%s)", r[rd], r[rs1]);
            }
            else{
                printf32("lhu     %s, %d(%s)", r[rd],imm, r[rs1]);
            }
            cpu->x[rd]=((uint32_t)(lh(mem,cpu->x[rs1]+imm)));
            printf("%s <- %d [%d]\n", r[rd],cpu->x[rd],cpu->x[rs1]+imm);
            return true;


    }
    return false;
}

bool execute_lui(struct cpu *cpu, int8_t rd, int32_t imm){
    printf32("lui     %s, %u", r[rd], imm);
    cpu->x[rd]=imm;
    printf("%s <- %d\n", r[rd],cpu->x[rd]);
    return true;

}
bool execute_auipc(struct cpu *cpu, int8_t rd, int32_t imm){
    printf32("auipc   %s, %u", r[rd], imm);
    cpu->x[rd]=(imm+(cpu->pc));
    printf("%s <- %d\n", r[rd],cpu->x[rd]);
    return true;

}

bool execute_jalr(struct cpu *cpu, int8_t funct3, int8_t rd, int8_t rs1, int32_t imm){
    (void)funct3;
    cpu->x[rd]=cpu->pc+4;
    cpu->pc=cpu->x[rs1]+imm;
    if ((cpu->pc & 3) != 0) {
    printf("Bus error in branch: pc is %d\n", cpu->pc);
    exit(1);
    }
    else if (rd==0 && rs1==1){
        //ret
        cpu->pc=cpu->x[rs1];
        printf32("ret     ");
        printf("pc <- %u\n", cpu->pc);
    }
    else if (rd==0 && imm==0){
        //jr
        printf32("jr      %s",r[rs1]);
        printf("pc <- %u\n", cpu->pc);
    }
    else if (rd==1 && imm==0){
        //j
        printf32("jalr    %s",r[rs1]);
        printf("%s <- %u, pc <- %u\n",r[rd],cpu->x[rd],cpu->pc);
    }
    else if (imm==0){
        //j
        printf32("jalr    %s, (%s)",r[rd],r[rs1]);
        printf("%s <- %u, pc <- %u\n",r[rd],cpu->x[rd],cpu->pc);

    }
    else{
        printf32("jalr    %s, %d(%s)",r[rd],imm,r[rs1]);
        printf("%s <- %u, pc <- %u\n",r[rd],cpu->x[rd],cpu->pc);

    }
    return true;
}

bool execute_jal(struct cpu *cpu, int8_t rd, int32_t imm){
    cpu->x[rd]=cpu->pc+4;
    cpu->pc=cpu->pc+imm;
    if ((cpu->pc & 3) != 0) {
    printf("Bus error in branch: pc is %d\n", cpu->pc);
    exit(1);
    }
    else if (rd==0){
        //j
        printf32("j       %d",imm);
        printf("pc <- %u\n", cpu->pc);
    }
    else if (rd==1){
        //jal

        printf32("jal     %d",imm);
        printf("%s <- %u, pc <- %u\n",r[rd],cpu->x[rd],cpu->pc);
    }
    else{
        printf32("jal     %s, %d",r[rd],imm);
        printf("%s <- %u, pc <- %u\n",r[rd],cpu->x[rd],cpu->pc);

    }
    return true;
}
bool execute_branches(struct cpu *cpu, int8_t funct3, int8_t rs1, int8_t rs2, int32_t imm){
    switch (funct3){
    //beq
    case 0:
        if (cpu->x[rs1]==cpu->x[rs2]){
            cpu->pc+=imm;
            if ((cpu->pc & 3) != 0) {
            printf("Bus error in branch: pc is %d\n", cpu->pc);
            exit(1);
            }
            else if(rs2==0){
                printf32("beqz    %s, %d",r[rs1],imm);
                printf("pc <- %u\n", cpu->pc);
            }
            else{
                printf32("beq     %s, %s, %d",r[rs1],r[rs2],imm);
                printf("pc <- %u\n", cpu->pc);
            }
        }
        else{
            cpu->pc+=4;
            if(rs2==0){
                printf32("beqz    %s, %d",r[rs1],imm);
                printf("--\n");
            }
            else{
                printf32("beq     %s, %s, %d",r[rs1],r[rs2],imm);
                printf("--\n");
            }
        }
        return true;
    //bne
    case 1:
        if (cpu->x[rs1]!=cpu->x[rs2]){
            cpu->pc+=imm;
            if ((cpu->pc & 3) != 0) {
                printf("Bus error in branch: pc is %d\n", cpu->pc);
                exit(1);
            }
            else if(rs2==0){
                printf32("bnez    %s, %d",r[rs1],imm);
                printf("pc <- %u\n", cpu->pc);
            }
            else{
                printf32("bne     %s, %s, %d",r[rs1],r[rs2],imm);
                printf("pc <- %u\n", cpu->pc);
            }
        }
        else{
            cpu->pc+=4;
            if(rs2==0){
                printf32("bnez    %s, %d",r[rs1],imm);
                printf("--\n");
            }
            else{
                printf32("bne     %s, %s, %d",r[rs1],r[rs2],imm);
                printf("--\n");
            }
        }
        return true;

    //blt
    case 4:
        if (cpu->x[rs1] < cpu->x[rs2]){
            cpu->pc+=imm;
            if ((cpu->pc & 3) != 0) {
                printf("Bus error in branch: pc is %d\n", cpu->pc);
                exit(1);
            }
            else if(rs2==0){
                printf32("bltz    %s, %d",r[rs1],imm);
                printf("pc <- %u\n", cpu->pc);
            }
            else if(rs1==0){
                printf32("bgtz    %s, %d",r[rs2],imm);
                printf("pc <- %u\n", cpu->pc);
            }
            else{
                printf32("blt     %s, %s, %d",r[rs1],r[rs2],imm);
                printf("pc <- %u\n", cpu->pc);
            }
        }
        else{
            cpu->pc+=4;
            if(rs2==0){
                printf32("bltz    %s, %d",r[rs1],imm);
                printf("--\n");
            }
            else if(rs1==0){
                printf32("bgtz    %s, %d",r[rs2],imm);
                printf("--\n");
            }
            else{
                printf32("blt     %s, %s, %d",r[rs1],r[rs2],imm);
                printf("--\n");
            }
        }
        return true;
    //bge
    case 5:
        if (cpu->x[rs1] >= cpu->x[rs2]){
            cpu->pc+=imm;
            if ((cpu->pc & 3) != 0) {
                printf("Bus error in branch: pc is %d\n", cpu->pc);
                exit(1);
            }
            else if(rs2==0){
                printf32("bgez    %s, %d",r[rs1],imm);
                printf("pc <- %u\n", cpu->pc);
            }
            else if(rs1==0){
                printf32("blez    %s, %d",r[rs2],imm);
                printf("pc <- %u\n", cpu->pc);
            }
            else{
                printf32("bge     %s, %s, %d",r[rs1],r[rs2],imm);
                printf("pc <- %u\n", cpu->pc);
            }
        }
        else{
            cpu->pc+=4;
            if(rs2==0){
                printf32("bgez    %s, %d",r[rs1],imm);
                printf("--\n");
            }
            else if(rs1==0){
                printf32("blez    %s, %d",r[rs2],imm);
                printf("--\n");
            }
            else{
                printf32("bge     %s, %s, %d",r[rs1],r[rs2],imm);
                printf("--\n");
            }
        }
        return true;
    //bltu
    case 6:
        if ((uint32_t)cpu->x[rs1] < (uint32_t)cpu->x[rs2]){
            cpu->pc+=imm;
            if ((cpu->pc & 3) != 0) {
                printf("Bus error in branch: pc is %d\n", cpu->pc);
                exit(1);
            }
            else{
                printf32("bltu    %s, %s, %d",r[rs1],r[rs2],imm);
                printf("pc <- %u\n", cpu->pc);
            }
        }
        else{
            cpu->pc+=4;
            printf32("bltu    %s, %s, %d",r[rs1],r[rs2],imm);
            printf("--\n");
        }

        return true;
    //bgeu
    case 7:
        if ((uint32_t)cpu->x[rs1] >= (uint32_t)cpu->x[rs2]){
            cpu->pc+=imm;
            if ((cpu->pc & 3) != 0) {
                printf("Bus error in branch: pc is %d\n", cpu->pc);
                exit(1);
            }
            else{
                printf32("bgeu    %s, %s, %d",r[rs1],r[rs2],imm);
                printf("pc <- %u\n", cpu->pc);
            }
        }
        else{
            cpu->pc+=4;
            printf32("bgeu    %s, %s, %d",r[rs1],r[rs2],imm);
            printf("--\n");
        }

        return true;

    }
    printf("unknown instruction\n");
    return false ;





}
bool execute_alu(struct cpu *cpu, int8_t funct3, int8_t rd, int8_t rs1, int8_t rs2, int8_t funct7){
    switch (funct3){
    //add sub
    case 0:
        if (funct7==1){
            printf32("mul     %s, %s, %s",r[rd],r[rs1],r[rs2]);
            cpu->x[rd]=cpu->x[rs1]*cpu->x[rs2];
        }
        else if (funct7==0){
            //add
            printf32("add     %s, %s, %s",r[rd],r[rs1],r[rs2]);
            cpu->x[rd]=cpu->x[rs1]+cpu->x[rs2];
        }
        else{
            //sub
            if (rs1==0){
                printf32("neg     %s, %s",r[rd],r[rs2]);
            }
            else{
                printf32("sub     %s, %s, %s",r[rd],r[rs1],r[rs2]);
            }
            cpu->x[rd]=cpu->x[rs1]-cpu->x[rs2];
        }
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;
    //sll
    case 1:
        if (funct7==1){
            printf32("mulh    %s, %s, %s",r[rd],r[rs1],r[rs2]);
            int64_t num=(int64_t)cpu->x[rs1]*(int64_t)cpu->x[rs2];
            cpu->x[rd]=num>>32;
        }
        else{
            printf32("sll     %s, %s, %s",r[rd],r[rs1],r[rs2]);
            
            cpu->x[rd]=cpu->x[rs1]<<((cpu->x[rs2])& 0x1f);
        }
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;

    //slt
    case 2:
        if (funct7==1){
            printf32("mulhsu  %s, %s, %s",r[rd],r[rs1],r[rs2]);
            int64_t num=(int64_t)cpu->x[rs1]*(uint64_t)cpu->x[rs2];
            cpu->x[rd]=num>>32;
            printf("%s <- %d\n", r[rd],cpu->x[rd]);
            return true;
        }
        else if ( rs1==0){
            printf32("sgtz    %s, %s",r[rd],r[rs2]);
        }
        else if (rs2==0){
            printf32("sltz    %s, %s",r[rd],r[rs1]);
        }
        else{
            printf32("slt     %s, %s, %s",r[rd],r[rs1],r[rs2]);
        }
        if ((cpu->x[rs1])<(cpu->x[rs2])){
            cpu->x[rd]=1;
        }
        else{
            cpu->x[rd]=0;
        }
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;
    //sltu
    case 3:
        if (funct7==1){
            printf32("mulhu   %s, %s, %s",r[rd],r[rs1],r[rs2]);
            uint64_t num=(uint64_t)cpu->x[rs1]*(uint64_t)cpu->x[rs2];
            cpu->x[rd]=num>>32;
            printf("%s <- %d\n", r[rd],cpu->x[rd]);
            return true;
        }
        else if (rs1==0){
            printf32("snez    %s, %s",r[rd],r[rs2]);
        }
        else{
            printf32("sltu    %s, %s, %s",r[rd],r[rs1],r[rs2]);
        }
        if (((uint32_t)cpu->x[rs1]) < ((uint32_t)cpu->x[rs2])){
                cpu->x[rd]=1;
        }
        else{
            cpu->x[rd]=0;
        }


        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;

    //xor
    case 4:
        if (funct7==1){
            printf32("div     %s, %s, %s",r[rd],r[rs1],r[rs2]);
            cpu->x[rd]=cpu->x[rs1]/cpu->x[rs2];
        }
        else{
            printf32("xor     %s, %s, %s",r[rd],r[rs1],r[rs2]);
            cpu->x[rd]=cpu->x[rs1]^cpu->x[rs2];
        }
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;
    //srl,sra
    case 5:
        if (funct7==1){
            printf32("divu    %s, %s, %s",r[rd],r[rs1],r[rs2]);
            cpu->x[rd]=(uint32_t)cpu->x[rs1]/(uint32_t)cpu->x[rs2];
        }
        else if (funct7==0){
            //srl
            printf32("srl     %s, %s, %s",r[rd],r[rs1],r[rs2]);
            // cpu->x[rs2]=((uint32_t)cpu->x[rs2]);
            // cpu->x[rs2]=cpu->x[rs2] & 0x1f;
            cpu->x[rd]=((uint32_t)cpu->x[rs1])>>(((uint32_t)cpu->x[rs2])& 0x1f);
            // cpu->x[rd]=cpu->x[rs1];

        }
        else{
            //sra
            printf32("sra     %s, %s, %s",r[rd],r[rs1],r[rs2]);
            // cpu->x[rs2]=cpu->x[rs2] & 0x1f;
            cpu->x[rd]=cpu->x[rs1]>>(cpu->x[rs2]& 0x1f);
            // cpu->x[rd]=cpu->x[rs1];
        }
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;

    //or
    case 6:
        if (funct7==1){
            printf32("rem     %s, %s, %s",r[rd],r[rs1],r[rs2]);
            cpu->x[rd]=cpu->x[rs1]%cpu->x[rs2];
        }
        else{
            printf32("or      %s, %s, %s",r[rd],r[rs1],r[rs2]);
            cpu->x[rd]=cpu->x[rs1]|cpu->x[rs2];
        }
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;

    //and
    case 7:
        if (funct7==1){
            printf32("remu    %s, %s, %s",r[rd],r[rs1],r[rs2]);
            cpu->x[rd]=(uint32_t)cpu->x[rs1]%(uint32_t)cpu->x[rs2];
        }
        else{
            printf32("and     %s, %s, %s",r[rd],r[rs1],r[rs2]);
            cpu->x[rd]=cpu->x[rs1]&cpu->x[rs2];
        }
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;

    }

    printf("unknown instruction\n");
    return false ;
}
bool execute_alu_imm(struct cpu *cpu, int8_t funct3, int8_t rd, int8_t rs1, int32_t imm){
    switch (funct3){
    // addi
    case 0:
        if (rd==0 && rs1==0 && imm==0){
            printf("nop\n");
        }
        else if (rs1==0){
            printf32("li      %s, %d", r[rd], imm);
        }
        else if (imm ==0){
            printf32("mv      %s, %s", r[rd],r[rs1]);
        }
        else {
            printf32("addi    %s, %s, %d",r[rd],r[rs1],imm);
        }
        cpu->x[rd] = cpu->x[rs1]+imm;
        if (rd!= 0 || rs1!=0 || imm !=0 ){
            printf("%s <- %d\n", r[rd],cpu->x[rd]);
        }
        return true;

    //slli
    case 1:
        printf32("slli    %s, %s, %d",r[rd],r[rs1],imm);
        cpu->x[rd]=cpu->x[rs1]<<imm;
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;
    //slti
    case 2:
        printf32("slti    %s, %s, %d", r[rd], r[rs1], imm);
        if (cpu->x[rs1]<imm){
            cpu->x[rd] = 1;
        }

        else{
            cpu->x[rd] = 0;
        }
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;

    //sltiu
    case 3:
        printf32("sltiu   %s, %s, %d", r[rd], r[rs1], imm);
        if ((uint32_t)(cpu->x[rs1])<(uint32_t)imm){
            cpu->x[rd] = 1;
        }

        else{
            cpu->x[rd] = 0;
        }
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;
    // xori
    case 4:
        if (imm==-1){
            printf32("not     %s, %s",r[rd],r[rs1]);
        }
        else{
            printf32("xori    %s, %s, %d",r[rd],r[rs1],imm);
        }
        cpu->x[rd]=(cpu->x[rs1]^imm);
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;

    //srli,srai
    case 5:
        if (get_funct7(imm)==0){
            //srli
            // cpu->x[rs1]=(uint32_t) cpu->x[rs1];
            printf32("srli    %s, %s, %d",r[rd],r[rs1],imm);
            cpu->x[rd]=(uint32_t)(cpu->x[rs1])>> imm;

        }
        else{
            //srai
            printf32("srai    %s, %s, %d",r[rd],r[rs1],imm);
            cpu->x[rd]=cpu->x[rs1]>> (get_imm_s(imm));
        }
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;

    // ori
    case 6:
        printf32("ori     %s, %s, %d",r[rd],r[rs1],imm);
        cpu->x[rd]=cpu->x[rs1]|imm;
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;

    // andi
    case 7:
        printf32("andi    %s, %s, %d",r[rd],r[rs1],imm);
        cpu->x[rd]=cpu->x[rs1]&imm;
        printf("%s <- %d\n", r[rd],cpu->x[rd]);
        return true;

    }
    printf("unknown instruction\n");
    return false ;
}

// execute the ecall instruction, which normally invokes a system call
// we implement three system calls by proxying the call to the host OS:
//
// 63: read(fd, buffer, len)
// 64: write(fd, buffer, len)
// 93: exit(status)

bool execute_ecall(struct address_space *mem, struct cpu *cpu) {
    printf32("ecall");

    if (cpu->x[17] == 63) {
        // read system call
        printf("read(%d, %d, %d)\n", cpu->x[10], cpu->x[11], cpu->x[12]);
        if (cpu->x[11] < mem->memory_start ||
            cpu->x[11] + cpu->x[12] >= mem->memory_end) {
            printf("Invalid memory range in read system call: %d to %d\n",
                cpu->x[11], cpu->x[11] + cpu->x[12]);
            exit(1);
        }
        fflush(stdout);
        int32_t res = read(cpu->x[10],
                    mem->memory + (cpu->x[11] - mem->memory_start),
                    cpu->x[12]);
        printf("%52s%s <- %d\n", "", r[10], res);
        cpu->x[10] = res;
        return true;
    }

    if (cpu->x[17] == 64) {
        // write system call
        printf("write(%d, %d, %d)\n", cpu->x[10], cpu->x[11], cpu->x[12]);
        if (cpu->x[11] < mem->memory_start ||
            cpu->x[11] + cpu->x[12] >= mem->memory_end) {
            printf("Invalid memory range in write system call: %d to %d\n",
                cpu->x[11], cpu->x[11] + cpu->x[12]);
            exit(1);
        }
        fflush(stdout);
        int32_t res = write(cpu->x[10],
                    mem->memory + (cpu->x[11] - mem->memory_start),
                    cpu->x[12]);
        printf("%52s%s <- %d\n", "", r[10], res);
        cpu->x[10] = res;
        return true;
    }

    if (cpu->x[17] == 93) {
        // exit system call
        printf("exit(%d)\n", cpu->x[10]);
        fflush(stdout);
        return false;
    }

    // unimplemented
    printf("unsupported syscall %d\n", cpu->x[17]);
    fflush(stdout);
    return false;
}
