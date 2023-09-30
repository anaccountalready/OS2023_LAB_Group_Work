#include <stdio.h>
#include "shell.h"
#define MIN_int32 0x80000000
#define MAX_int32  0x7fffffff
typedef struct inst_I{
uint32_t rs;
uint32_t rt;
uint32_t imm;
}I_inst;
typedef struct inst_J
{
    uint32_t target;
}J_inst;
typedef struct inst_R
{
    uint32_t rs;
    uint32_t rt;
    uint32_t rd;
    uint32_t shamt;
    uint32_t funct;

}R_inst;
I_inst i_inst;
J_inst j_inst;
R_inst r_inst;

inline void decode_I(uint32_t inst){
    i_inst.rs= (inst >> 21)& 0x01f;
    i_inst.rt = (inst >> 16)& 0x01f;
    i_inst.imm = inst & 0x0ffff;
}
inline void decode_J(uint32_t inst){
    j_inst.target = (inst << 6) >> 6;

}
inline void decode_R(uint32_t inst){
    r_inst.funct = inst & 0x03f;
    r_inst.shamt = (inst>>6) & 0x01f;
    r_inst.rd = (inst >> 11) & 0x01f;
    r_inst.rt = (inst >> 16) & 0x01f;
    r_inst.rs = (inst >> 21) & 0x01f;

}
inline int32_t complement(uint32_t extend_num,uint32_t sign){
    if(sign == 1)
    return (0x80000000 | ((((~((extend_num<< 1 ) >>1))+1)<<1)>>1));
    else 
    return extend_num;
} 
inline uint32_t signed_extend(uint32_t imm){
    uint32_t sign = (imm >> 15)&1;
    if(sign == 1){imm = (((sign << 16)-1)<<16) | imm;}
    return imm;
}
//ALU_MODULE
typedef struct overflow{
uint32_t sum;//存低32位
uint32_t sum_sign;//是否溢出
}alu;
alu ALU_INST;
void add(uint32_t x,uint32_t y){
    
    int64_t x1 = complement(x,x>>31);
   int64_t y1 = complement(y,y>>31);
   int32_t sum = (x1 +y1) & 0xffffffff;
   ALU_INST.sum = sum;
   if((((x1 + y1)>>32)&1) == (((x1 + y1)>>31)&1)){ALU_INST.sum_sign = 0;}
   else{
    ALU_INST.sum_sign = 1;
   }
    }

    
    

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
   uint32_t inst = mem_read_32(CURRENT_STATE.PC);
   printf("The now PC is: 0x%08x\n" ,CURRENT_STATE.PC);
   printf("The inst now is: 0x%08x\n" ,inst);
   uint32_t op = inst >> 26;
   //SYSCALL
  /*if(CURRENT_STATE.REGS[2] == 10){
    RUN_BIT = FALSE;

  }*/
   //R-inst
   if(op == 0){
    decode_R(inst);
    if (r_inst.funct == 0b001100){
         RUN_BIT = FALSE;

    }
    //AND
    if ((r_inst.funct == 0b100100)&& (r_inst.shamt == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        NEXT_STATE.REGS[r_inst.rd] = CURRENT_STATE.REGS[r_inst.rs] & CURRENT_STATE.REGS[r_inst.rt];
        
        
        
    }
    //OR
    if ((r_inst.funct == 0b100101)&& (r_inst.shamt == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        NEXT_STATE.REGS[r_inst.rd] = CURRENT_STATE.REGS[r_inst.rs] | CURRENT_STATE.REGS[r_inst.rt];
       
        
    }
    //XOR
    if ((r_inst.funct == 0b100110)&& (r_inst.shamt == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        NEXT_STATE.REGS[r_inst.rd] = CURRENT_STATE.REGS[r_inst.rs] ^ CURRENT_STATE.REGS[r_inst.rt];
       
        
        
    }
    //NOR
    if ((r_inst.funct == 0b100111)&& (r_inst.shamt == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        NEXT_STATE.REGS[r_inst.rd] = ~(CURRENT_STATE.REGS[r_inst.rs] | CURRENT_STATE.REGS[r_inst.rt]);
        
        
        
    }
    //SLT
    if((r_inst.funct == 0b101010) && (r_inst.shamt == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        uint32_t rs_sign = CURRENT_STATE.REGS[r_inst.rs]>>31;
        uint32_t rt_sign = CURRENT_STATE.REGS[r_inst.rt]>>31;
        int32_t rs = complement(CURRENT_STATE.REGS[r_inst.rs],rs_sign);
        int32_t rt =complement(CURRENT_STATE.REGS[r_inst.rt],rt_sign);
        if(rs < rt){NEXT_STATE.REGS[r_inst.rd] = 1;}
        else {NEXT_STATE.REGS[r_inst.rd] = 0;}
    
        

    }
    //SLTU
    if((r_inst.funct == 0b101011) && (r_inst.shamt == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        if( CURRENT_STATE.REGS[r_inst.rs]<  CURRENT_STATE.REGS[r_inst.rt]){NEXT_STATE.REGS[r_inst.rd] = 1;}
        else {NEXT_STATE.REGS[r_inst.rd] = 0;}
        

    }
    //MULT
    if((r_inst.funct == 0b011000) && (r_inst.shamt == 0) && (r_inst.rd == 0 )){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        uint32_t  rs = CURRENT_STATE.REGS[r_inst.rs]&0x7fffffff;
        uint32_t  rt = CURRENT_STATE.REGS[r_inst.rt]&0x7fffffff;
        int64_t sign=1;
        if(((rs>>31)&1)==1 || (((rt>>31)&1)==1)){
            sign = -1;
        }
        int64_t mul = rs * rt * sign;
         NEXT_STATE.LO = mul & 0xffffffff ;
         NEXT_STATE.HI = (mul >> 32)&0xffffffff;
        
        
    }
    //MULTU
    if((r_inst.funct == 0b011001) && (r_inst.shamt == 0) && (r_inst.rd == 0 )){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       uint64_t tem = CURRENT_STATE.REGS[r_inst.rs] * CURRENT_STATE.REGS[r_inst.rt];
        NEXT_STATE.LO = tem & 0xffffffff;
        NEXT_STATE.HI = (tem >> 32)&0xffffffff;
        

    }
    //DIV
    if((r_inst.funct == 0b011011) && (r_inst.shamt == 0) && (r_inst.rd == 0 )){
       if(CURRENT_STATE.REGS[r_inst.rt] == 0){
        printf("error, divisor is zero");
       }
       else{
        int32_t rs =complement(CURRENT_STATE.REGS[r_inst.rs],CURRENT_STATE.REGS[r_inst.rs]>>31);
        int32_t rt =complement(CURRENT_STATE.REGS[r_inst.rt],(CURRENT_STATE.REGS[r_inst.rt]>>31));
       NEXT_STATE.LO = rs / rt;
       NEXT_STATE.HI = rs % rt;
       printf("div_result:  0x%08x %08x\n",CURRENT_STATE.HI , CURRENT_STATE.LO);
       }
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    }
    //DIVU
    if((r_inst.funct == 0b011011) && (r_inst.shamt == 0) && (r_inst.rd == 0 )){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
       if(CURRENT_STATE.REGS[r_inst.rt] == 0){
        printf("error, divisor is zero");
       }
       else{
       NEXT_STATE.LO = CURRENT_STATE.REGS[r_inst.rs] / CURRENT_STATE.REGS[r_inst.rt];
       NEXT_STATE.HI = CURRENT_STATE.REGS[r_inst.rs] % CURRENT_STATE.REGS[r_inst.rt];
       }
       

    }
    //MFHI
    if((r_inst.funct == 0b010000) && (r_inst.shamt == 0) && (r_inst.rs == 0 ) && (r_inst.rt == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
         NEXT_STATE.REGS[r_inst.rd] = CURRENT_STATE.HI;
        

    }
    //MTLO
    if((r_inst.funct == 0b010011) && (r_inst.shamt == 0 ) && (r_inst.rt == 0)&&(r_inst.rd == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        NEXT_STATE.LO = CURRENT_STATE.REGS[r_inst.rs];
        
    }
    //SLL
    if((r_inst.funct == 0) && (r_inst.rs == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        NEXT_STATE.REGS[r_inst.rd] = (CURRENT_STATE.REGS[r_inst.rt] << (r_inst.shamt));
       
    }
    //SRL
    if((r_inst.funct == 0b000010) && (r_inst.rs == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        NEXT_STATE.REGS[r_inst.rd] = CURRENT_STATE.REGS[r_inst.rt] >> (r_inst.shamt);
       
    }
    //SRA
    if((r_inst.funct == 0b000011) && (r_inst.rs == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        uint32_t sign = CURRENT_STATE.REGS[r_inst.rt] >> 31;
        NEXT_STATE.REGS[r_inst.rd] = CURRENT_STATE.REGS[r_inst.rt] >> (r_inst.shamt);
        if(sign == 1){
            NEXT_STATE.REGS[r_inst.rd] = (((sign << r_inst.shamt)-1) << (32 - r_inst.shamt)) + (CURRENT_STATE.REGS[r_inst.rt] >> (r_inst.shamt));
        }
        
    }
    //SLLV
    if((r_inst.funct == 0b000100) && (r_inst.shamt == 0)){
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        NEXT_STATE.REGS[r_inst.rd] = CURRENT_STATE.REGS[r_inst.rt] << (CURRENT_STATE.REGS[r_inst.rs] & 0x1f);
    }
    //SRLV
    if((r_inst.funct==0b000110) && (r_inst.shamt == 0)){
         NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        NEXT_STATE.REGS[r_inst.rd] = CURRENT_STATE.REGS[r_inst.rt] >> (CURRENT_STATE.REGS[r_inst.rs] & 0x1f);
       
    }
    //SRAV
    if((r_inst.funct==0b000111) && (r_inst.shamt == 0)){
         NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        uint32_t sign = (CURRENT_STATE.REGS[r_inst.rt] >> 31)&1;
       NEXT_STATE.REGS[r_inst.rd] = CURRENT_STATE.REGS[r_inst.rt] >> (CURRENT_STATE.REGS[r_inst.rs] & 0x1f);
        if(sign == 1){NEXT_STATE.REGS[r_inst.rd] =((sign << ((CURRENT_STATE.REGS[r_inst.rs] & 0x1f)))-1) <<(32 - (CURRENT_STATE.REGS[r_inst.rs] & 0x1f))|(CURRENT_STATE.REGS[r_inst.rt] >> (CURRENT_STATE.REGS[r_inst.rs] & 0x1f));}
        
    }
    //JR
    if((r_inst.funct == 0b001000) && (((inst >> 6) & 0x07fff)==0)){
        NEXT_STATE.PC = CURRENT_STATE.REGS[r_inst.rs];
        if(CURRENT_STATE.REGS[r_inst.rs] & 0x03 !=0){
            printf("JR address word-aligned error,so pc+4");
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        }
    }
    //JALR
    if((r_inst.funct == 0b001001) && (r_inst.rt==0) && (r_inst.shamt == 0)){
        NEXT_STATE.PC = CURRENT_STATE.REGS[r_inst.rs];
        NEXT_STATE.REGS[31] = CURRENT_STATE.PC +4;
        if(CURRENT_STATE.REGS[r_inst.rs] & 0x03 !=0){
            printf("JR address word-aligned error,so pc+4");
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        }
    }
    //ADD
    if((r_inst.funct == 0b100000) && (r_inst.shamt == 0)){
         NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        add(CURRENT_STATE.REGS[r_inst.rt],CURRENT_STATE.REGS[r_inst.rs]);
        
            NEXT_STATE.REGS[r_inst.rd] = ALU_INST.sum ;
        if(ALU_INST.sum_sign){printf("add_inst_overflow\n");}
       
       
    }
    //ADDU
    if((r_inst.funct == 0b100001) && (r_inst.shamt == 0)){
       NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        add(CURRENT_STATE.REGS[r_inst.rs],CURRENT_STATE.REGS[r_inst.rt]); 
       NEXT_STATE.REGS[r_inst.rd] = ALU_INST.sum;
       
       
      
    }
    //SUB
    if((r_inst.funct == 0b100010) && (r_inst.shamt == 0)){
         NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        uint32_t rs_sign = CURRENT_STATE.REGS[r_inst.rs] >> 31;
        uint32_t rt_sign = CURRENT_STATE.REGS[r_inst.rt] >> 31;
        uint32_t rt = CURRENT_STATE.REGS[r_inst.rt];
        if(((rt>>31)&1)==1){rt = rt & 0x7fffffff;}
        else {rt = 0x80000000 | rt;}
        add(CURRENT_STATE.REGS[r_inst.rs],rt);
        NEXT_STATE.REGS[r_inst.rd] = ALU_INST.sum ;
        if(ALU_INST.sum_sign){printf("add_inst_overflow\n");} 
       
    }
    //SUBU
    if((r_inst.funct == 0b100011) && (r_inst.shamt == 0)){
         NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        NEXT_STATE.REGS[r_inst.rd] = CURRENT_STATE.REGS[r_inst.rs] - CURRENT_STATE.REGS[r_inst.rt]; 
        
       
    }
   }
   //ANDI
   if(op == 0b001100){
    decode_I(inst);
     NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    NEXT_STATE.REGS[i_inst.rt] = (CURRENT_STATE.REGS[i_inst.rs] & i_inst.imm);
    
   }
   //ORI
   if(op == 0b001101){
    decode_I(inst);
     NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    NEXT_STATE.REGS[i_inst.rt]=(CURRENT_STATE.REGS[i_inst.rs] | i_inst.imm);
    
   }
   //LUI
   if(op == 0b001111 && (((inst >> 21)& 0x01f) == 0)){
    decode_I(inst);
     NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        NEXT_STATE.REGS[i_inst.rt] = i_inst.imm << 16;
       
   }
   //LB
   if(op == 0b100000){
    decode_I(inst);
     NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    uint32_t signed_offset = signed_extend(i_inst.imm);
    uint32_t data =  mem_read_32(CURRENT_STATE.REGS[i_inst.rs]+signed_offset);
   
    NEXT_STATE.REGS[i_inst.rt] = data & 0x0ff;
     uint32_t sign = data >> 7;

     if(sign == 1) {NEXT_STATE.REGS[i_inst.rt] = (((sign<< 24) -1)<<8) | (data & 0x0ff);}
   }
   //LH
    if(op == 0b100001){
        decode_I(inst);
         NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        uint32_t signed_offset = signed_extend(i_inst.imm);
        uint32_t data =  mem_read_32(CURRENT_STATE.REGS[i_inst.rs]+signed_offset);
        NEXT_STATE.REGS[i_inst.rt] = data & 0x0ffff;
        uint32_t sign = data >> 15;

        if(sign == 1) {NEXT_STATE.REGS[i_inst.rt] = (((sign<< 16) -1)<<16) | (data & 0x0ffff);}
        
    }
    //LW,也会有异常
    if(op == 0b100011){
        decode_I(inst);
         NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        uint32_t signed_offset = signed_extend(i_inst.imm);
        uint32_t data =  mem_read_32(CURRENT_STATE.REGS[i_inst.rs]+signed_offset);
        NEXT_STATE.REGS[i_inst.rt] = data ;


    }
    //LBU
    if(op == 0b100100){
        decode_I(inst);
         NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        uint32_t signed_offset = signed_extend(i_inst.imm);
        uint32_t data =  mem_read_32(CURRENT_STATE.REGS[i_inst.rs]+signed_offset);
        NEXT_STATE.REGS[i_inst.rt] = data & 0x0ff;
        
    }
    //LHU
    if(op == 0b100101){
        decode_I(inst);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4; 
        uint32_t signed_offset = signed_extend(i_inst.imm);
        uint32_t data =  mem_read_32(CURRENT_STATE.REGS[i_inst.rs]+signed_offset);
        NEXT_STATE.REGS[i_inst.rt] = data & 0x0ffff;
        
    }
    //SB
    if(op == 0b101000){
        decode_I(inst);
        uint32_t signed_offset = signed_extend(i_inst.imm);
        mem_write_32(CURRENT_STATE.REGS[i_inst.rs]+signed_offset, (CURRENT_STATE.REGS[i_inst.rt] & 0x0ff));
        NEXT_STATE.PC = CURRENT_STATE.PC + 4; 
    }
    //SH
    if(op == 0b101001){
        decode_I(inst);
        uint32_t signed_offset = signed_extend(i_inst.imm);
        if(((CURRENT_STATE.REGS[i_inst.rs]+signed_offset)&0x01)!=0)printf("throwout address error");
       else
        mem_write_32(CURRENT_STATE.REGS[i_inst.rs]+signed_offset, (CURRENT_STATE.REGS[i_inst.rt] & 0x0ffff));
        NEXT_STATE.PC = CURRENT_STATE.PC + 4; 
    }
    //SW
    if(op == 0b101011){
        decode_I(inst);
        uint32_t signed_offset = signed_extend(i_inst.imm);
        if(((CURRENT_STATE.REGS[i_inst.rs]+signed_offset)&0x03)!=0)printf("throwout address error");
       else
        mem_write_32(CURRENT_STATE.REGS[i_inst.rs]+signed_offset, CURRENT_STATE.REGS[i_inst.rt]);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4; 
    }

   //XORI
   if(op == 0b001110){
    decode_I(inst);
    NEXT_STATE.PC = CURRENT_STATE.PC +4;
    NEXT_STATE.REGS[i_inst.rt] =(CURRENT_STATE.REGS[i_inst.rs] ^ i_inst.imm);
   }
   //ADDIU
   if (op == 0b001001){
    decode_I(inst);

    uint32_t result = CURRENT_STATE.REGS[i_inst.rs] + i_inst.imm;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    uint32_t r = signed_extend(i_inst.imm);
    add(CURRENT_STATE.REGS[i_inst.rs],r);
    NEXT_STATE.REGS[i_inst.rt] = complement(ALU_INST.sum,(ALU_INST.sum>>31)&1);
   }
    //ADDI
    if(op == 0b001000){
    decode_I(inst);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4; 
    uint32_t r = signed_extend(i_inst.imm);
    add(CURRENT_STATE.REGS[i_inst.rs],r);
    NEXT_STATE.REGS[i_inst.rt] = ALU_INST.sum;
    if(ALU_INST.sum_sign){printf("addi_inst_overflow\n");}
   
    }
    //SLTI
    if(op == 0b001010){
        decode_I(inst);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4; 
        uint32_t sign = i_inst.imm >> 15;
        uint32_t sign_extend_imm = i_inst.imm ;
        uint32_t sign_imm = sign_extend_imm;
        if(sign == 1){
            sign_extend_imm = (((sign << 16)-1) << 16) | i_inst.imm; 
            sign_imm =complement(sign_extend_imm,1);
        }
        uint32_t sign_reg = CURRENT_STATE.REGS[i_inst.rs] >> 31;
        uint32_t reg = CURRENT_STATE.REGS[i_inst.rs];
        if(sign_reg == 1){reg = complement(CURRENT_STATE.REGS[i_inst.rs],1);}
        if(reg < sign_imm){
            NEXT_STATE.REGS[i_inst.rt] = 0x01;
        }
        else {NEXT_STATE.REGS[i_inst.rt] = 0;}
        
       
    }
    //SLTIU
    if(op == 0b001011){
        decode_I(inst);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4; 
        uint32_t sign = i_inst.imm >> 15;
        uint32_t sign_extend_imm = i_inst.imm ;
        uint32_t sign_imm = sign_extend_imm;
        if(sign == 1){ sign_extend_imm = (((sign << 16)-1) << 16) | i_inst.imm; }
        uint32_t reg = CURRENT_STATE.REGS[i_inst.rs];
        if(reg < sign_imm){
            NEXT_STATE.REGS[i_inst.rt] = 0x01;
        }
        else {NEXT_STATE.REGS[i_inst.rt] = 0;}
        
    }    

    //J
    if(op == 0b000010){
       decode_J(inst); 
        uint32_t offset_addr = j_inst.target << 2;
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) + offset_addr;
        
    }
    //JAL
    if(op == 0b000011){
         decode_J(inst);
        uint32_t offset_addr = j_inst.target << 2;
        NEXT_STATE.REGS[31] = CURRENT_STATE.PC +4;
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) + offset_addr;
       
    }
    //BEQ
    if(op == 0b000100){
        decode_I(inst);
        uint32_t offset = i_inst.imm;
        uint32_t sign = offset >> 15;
        uint32_t sign_extend_offset = offset << 2;
        if(sign == 1){sign_extend_offset = (((sign << 14)-1) << 18 | (offset << 2));}
        if(CURRENT_STATE.REGS[i_inst.rs] == CURRENT_STATE.REGS[i_inst.rt]){
            NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend_offset;

        }
        else NEXT_STATE.PC = CURRENT_STATE.PC +4;
       
        
    }
    //BNE
    if(op == 0b000101){
         decode_I(inst);
        uint32_t offset = i_inst.imm;
        uint32_t sign = offset >> 15;
        uint32_t sign_extend_offset = offset << 2;
        if(sign == 1){sign_extend_offset = (((sign << 14)-1) << 18 | (offset << 2));}
        if(CURRENT_STATE.REGS[i_inst.rs] != CURRENT_STATE.REGS[i_inst.rt]){
             NEXT_STATE.PC = CURRENT_STATE.PC  + sign_extend_offset;

        }
        else NEXT_STATE.PC = CURRENT_STATE.PC +4;
        
        
    }
    //BLEZ
    if(op == 0b000110 && ((inst >> 16)& 0x01f)==0){
        decode_I(inst);
        uint32_t offset = i_inst.imm;
        uint32_t sign = offset >> 15;
        uint32_t sign_extend_offset = offset << 2;
        if(sign == 1){sign_extend_offset = (((sign << 14)-1) << 18 | (offset << 2));}
        if((CURRENT_STATE.REGS[i_inst.rs] >> 31)==1 || (CURRENT_STATE.REGS[i_inst.rs]==0)){
             NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend_offset;
        }
        else NEXT_STATE.PC = CURRENT_STATE.PC +4;
        
        
    }
    //BGTZ
    if(op == 0b000111 && ((inst >> 16)& 0x01f)==0){
        decode_I(inst);
        uint32_t offset = i_inst.imm;
        uint32_t sign = offset >> 15;
        uint32_t sign_extend_offset = offset << 2;
        if(sign == 1){sign_extend_offset = (((sign << 14)-1) << 18 | (offset << 2));}
        if(complement(CURRENT_STATE.REGS[i_inst.rs],CURRENT_STATE.REGS[i_inst.rs]>>31)>0)
        {NEXT_STATE.PC = CURRENT_STATE.PC  + sign_extend_offset;
        }
        else {NEXT_STATE.PC = CURRENT_STATE.PC +4;}
       
        
    }
     //BLTZ
     if(op == 0b000001 && ((inst >> 16)& 0x01f)==0){
        decode_I(inst);
        uint32_t offset = i_inst.imm;
        uint32_t sign = (offset >> 15)&1;
        uint32_t sign_extend_offset = offset << 2;
        if(sign == 1){sign_extend_offset = ((sign << 14)-1) << 18 | (offset << 2);}
        if((CURRENT_STATE.REGS[i_inst.rs] >> 31)==1){
             NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend_offset;

        }
        else {NEXT_STATE.PC = CURRENT_STATE.PC +4;}
        
        
    }
    //BGEZ
    if(op == 0b000001 && ((inst >> 16)& 0x01f)==1){
        decode_I(inst);
        uint32_t offset = i_inst.imm;
        uint32_t sign = offset >> 15;
        uint32_t sign_extend_offset = offset << 2;
        if(sign == 1){sign_extend_offset = ((sign << 14)-1) << 18 | (offset << 2);}
        if((CURRENT_STATE.REGS[i_inst.rs] >> 31)==0){
           NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend_offset;

        }
        else NEXT_STATE.PC = CURRENT_STATE.PC +4;
       
    }
    //BLTZAL
    if(op == 0b000001 && ((inst >> 16) & 0x01f)==16){
        decode_I(inst);
        uint32_t offset = i_inst.imm;
        uint32_t sign = offset >> 15;
        uint32_t sign_extend_offset = offset << 2;
        if(sign == 1){sign_extend_offset = ((sign << 14)-1) << 18 | (offset << 2);}
        CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
        if((CURRENT_STATE.REGS[i_inst.rs] >> 31)==1){
           NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend_offset;

        }
        else NEXT_STATE.PC = CURRENT_STATE.PC +4;
    }
    //BGEZAL
    if(op == 0b000001 && ((inst >> 16) & 0x01f)==17){
        decode_I(inst);
        uint32_t offset = i_inst.imm;
        uint32_t sign = offset >> 15;
        uint32_t sign_extend_offset = offset << 2;
        if(sign == 1){sign_extend_offset = ((sign << 14)-1) << 18 | (offset << 2);}
        CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
        if(((CURRENT_STATE.REGS[i_inst.rs] >> 31) & 1)==0){
          NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend_offset;

        }
        else {NEXT_STATE.PC = CURRENT_STATE.PC +4;}
    }
 return;
}
