#ifndef __DESC_H__
#define __DESC_H__

#include <stdint.h>

struct idt_entry {
    uint16_t handler_last_quarter;
    uint16_t seg_sel;//Kernel_cs
    uint8_t always0; //IST-zeros + undefined-zeros
    uint8_t flags; // TYPE, undefined-zeros, DPL, P
    //TYPE:0xf - trap gate, 0xe - interrupt gate
    //один нулевой ненужный бит
    //DPL:00
    //P:1
    // example: for divide by zero: - trap gate 10001111
    // interrupt gate: 10001110
    uint16_t handler_second_quarter;
    uint32_t handler_first_half;
    uint32_t always0too;
} __attribute__((packed));

struct desc_table_ptr {
	uint16_t size;
	uint64_t addr;
} __attribute__((packed));

static inline void read_idtr(struct desc_table_ptr *ptr)
{
	__asm__ ("sidt %0" : "=m"(*ptr));
}

static inline void write_idtr(const struct desc_table_ptr *ptr)
{
	__asm__ ("lidt %0" : : "m"(*ptr));
}

static inline void read_gdtr(struct desc_table_ptr *ptr)
{
	__asm__ ("sgdt %0" : "=m"(*ptr));
}

static inline void write_gdtr(const struct desc_table_ptr *ptr)
{
	__asm__ ("lgdt %0" : : "m"(*ptr));
}

#endif /*__DESC_H__*/
