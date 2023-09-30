# 实验报告
## 'process_instruction()'设计

### 取值
得到当前pc（`CURRENT_STATE.PC`）

从当前PC的内存中取出指令,获得op(三种指令的共有部分):

```
uint32_t inst = mem_read_32(CURRENT_STATE.PC);

uint32_t op = inst >> 26;
```

### 译码

对当前指令进行与之对应的译码操作`decode_?(uint32_t inst)`

```
I_inst i_inst;//I型指令

J_inst j_inst;//J型指令

R_inst r_inst;//R型指令

```

### 具体到特定指令进行执行

对于`add，sub`等的加减计算，另写了`add()`函数对补码进行相应计算

其他的处理函数：

`int32_t complement(uint32_t extend_num,uint32_t sign)`计算补码

`uint32_t signed_extend(uint32_t imm)`符号扩展

其余指令按照相应方法进行处理

### 改变PC

对于每条指令，跳转指令(`NEXT_STATE.PC = 跳转计算的地址;`)除外，让`NEXT_STATE.PC = CURRENT_STATE.PC + 4;`

没有对寄存器的值进行非所给指令的其他自动改变
