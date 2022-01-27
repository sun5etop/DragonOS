/**
 * @file gate.h
 * @author longjin
 * @brief 门定义
 * @date 2022-01-24
 * 
 */

#pragma once

//描述符表的结构体
struct desc_struct
{
    unsigned char x[8];
};

//门的结构体
struct gate_struct
{
    unsigned char x[16];
};

extern struct desc_struct GDT_Table[]; //GDT_Table是head.S中的GDT_Table
extern struct gate_struct IDT_Table[]; //IDT_Table是head.S中的IDT_Table
extern unsigned int TSS64_Table[26];


/**
 * @brief 初始化中段描述符表内的门描述符（每个16B）
 * @param gate_selector_addr IDT表项的地址
 * @param attr P、DPL、TYPE的属性
 * @param ist 中断栈表号
 * @param code_addr 指向中断服务程序的指针的地址
 */
void set_gate(ul *gate_selector_addr, ul attr, unsigned char ist, ul *code_addr)
{
    ul __d0=0, __d1=0;
    ul tmp_code_addr = *code_addr;
    __d0 = attr << 40; //设置P、DPL、Type

    __d0 |= ((ul)(ist) << 32); // 设置ist

    __d0 |= 8 << 16; //设置段选择子为0x1000

    __d0 |= (0xffff & tmp_code_addr); //设置段内偏移的[15:00]

    tmp_code_addr >>= 16;
    __d0 |= (0xffff & tmp_code_addr) << 48; // 设置段内偏移[31:16]

    tmp_code_addr >>= 16;

    __d1 = (0xffffffff & tmp_code_addr); //设置段内偏移[63:32]


    *gate_selector_addr = __d0;
    *(gate_selector_addr + 1) = __d1;
}

/**
 * @brief 加载任务状态段寄存器
 * @param n TSS基地址在GDT中的第几项
 * 左移3位的原因是GDT每项占8字节
 */
#define load_TR(n)                                      \
    do                                                  \
    {                                                   \
        __asm__ __volatile__("ltr %%ax" ::"a"(n << 3)); \
    } while (0)

/**
 * @brief 设置中断门
 * 
 * @param n 中断号
 * @param ist ist
 * @param addr 服务程序的地址
 */
void set_intr_gate(unsigned int n, unsigned char ist, void *addr)
{
    set_gate(IDT_Table + n, 0x8E, ist, &addr); // p=1，DPL=0, type=E
}

/**
 * @brief 设置64位，DPL=0的陷阱门
 * 
 * @param n 中断号
 * @param ist ist
 * @param addr 服务程序的地址
 */
void set_trap_gate(unsigned int n, unsigned char ist, void *addr)
{
    set_gate(IDT_Table + n, 0x8F, ist, &addr); // p=1，DPL=0, type=F
}

/**
 * @brief 设置64位，DPL=3的陷阱门
 * 
 * @param n 中断号
 * @param ist ist
 * @param addr 服务程序的地址
 */
void set_system_trap_gate(unsigned int n, unsigned char ist, void *addr)
{
    set_gate(IDT_Table + n, 0xEF, ist, &addr); // p=1，DPL=3, type=F
}

/**
 * @brief 初始化TSS表的内容
 * 
 */
void set_TSS64(ul rsp0, ul rsp1, ul rsp2, ul ist1, ul ist2, ul ist3, ul ist4, ul ist5, ul ist6, ul ist7)
{
    *(ul *)(TSS64_Table + 1) = rsp0;
    *(ul *)(TSS64_Table + 3) = rsp1;
    *(ul *)(TSS64_Table + 5) = rsp2;

    *(ul *)(TSS64_Table + 9) = ist1;
    *(ul *)(TSS64_Table + 11) = ist2;
    *(ul *)(TSS64_Table + 13) = ist3;
    *(ul *)(TSS64_Table + 15) = ist4;
    *(ul *)(TSS64_Table + 17) = ist5;
    *(ul *)(TSS64_Table + 19) = ist6;
    *(ul *)(TSS64_Table + 21) = ist7;
}