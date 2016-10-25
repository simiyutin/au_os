static void qemu_gdb_hang(void)
{
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}

#include <desc.h>
#include <ints.h>
#include <ioport.h>
#include <memory.h>
//todo for clion
#include "../inc/desc.h"
#include "../inc/ioport.h"
#include "../inc/memory.h"
#include "../inc/ints.h"
#include "../inc/multiboot.h"
#include <stdlib.h>
#include <stdio.h>


// ports
#define SERIAL_PORT 0x3f8   /* serial port */
#define MASTER_COMMAND 0x20
#define MASTER_DATA 0x21
#define SLAVE_COMMAND 0xA0
#define SLAVE_DATA 0xA1
#define PIT_COMMAND 0x43
#define PIT_DATA 0x40

/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

//handlers
extern uint64_t table[];

static struct idt_entry idt_table[33];


void init_serial() {
    out8(SERIAL_PORT + 1, 0x00);    // Disable all interrupts
    out8(SERIAL_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    out8(SERIAL_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    out8(SERIAL_PORT + 1, 0x00);    //                  (hi byte)
    out8(SERIAL_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
}

int is_transmit_done() {
    return in8(SERIAL_PORT + 5) & 0x20; // if 5th bit is unset, return zero
}

void write_to_serial(char a) {

    while (!is_transmit_done());

    out8(SERIAL_PORT,a);

}

void print_string(char* string_to_print) {

    init_serial();

    for (unsigned int i=0; string_to_print[i] != '\0'; i++){
        write_to_serial(string_to_print[i]);
    }
}


void c_handler() {
    char* ploho = "vse ochen ploho\n\0";
    print_string(ploho);
}

void c_handler2() {
    char* horosho = "vse ochen horosho\n\0";
    print_string(horosho);

    uint8_t undirected_eoi = 0b00100000;
    out8(MASTER_COMMAND, undirected_eoi);
}

void init_idt() {

    for (int i = 0; i < 33; ++i) {

        uint64_t handler_address = (uint64_t) table[i];
        uint8_t trap_gate_flags = 0b10001111;
        uint16_t handler_last_quarter = (uint16_t)(handler_address & 0xffff);
        uint16_t handler_second_quarter = (uint16_t)((handler_address & 0xffff0000)>>16);
        uint32_t handler_first_half = (uint32_t)((handler_address & 0xffffffff00000000)>>32);

        idt_table[i].handler_last_quarter = handler_last_quarter;
        idt_table[i].seg_sel = KERNEL_CS;
        idt_table[i].flags = trap_gate_flags;
        idt_table[i].handler_second_quarter = handler_second_quarter;
        idt_table[i].handler_first_half = handler_first_half;

    }

    struct desc_table_ptr ptr = {sizeof(idt_table) - 1, (uint64_t) &idt_table};
    write_idtr(&ptr);
}

void init_interrupt_controller() {

    uint8_t command = 0b00010001;
    uint8_t first_byte_idt_entry_for_master = 32;
    uint8_t first_idt_entry_for_slave = 40;
    uint8_t second_byte = 0b00000100;
    uint8_t second_byte_slave = 2;
    uint8_t third_byte = 0b00000001;

    out8(MASTER_COMMAND, command);
    out8(SLAVE_COMMAND, command);

    out8(MASTER_DATA, first_byte_idt_entry_for_master);
    out8(SLAVE_DATA, first_idt_entry_for_slave);

    out8(MASTER_DATA, second_byte);
    out8(SLAVE_DATA, second_byte_slave);

    out8(MASTER_DATA, third_byte);
    out8(SLAVE_DATA, third_byte);

    out8(MASTER_DATA, 0xff);
    out8(SLAVE_DATA, 0xff);

}

void start_pit_interruptions() {

    uint8_t word = 0b00110100;
    out8(PIT_COMMAND, word);

    out8(PIT_DATA, 0xff);
    out8(PIT_DATA, 0xff);

}

void main(uint32_t magic, struct multiboot_info* boot_info) {

    if (magic == 0x2BADB002) {
        print_string("magic 0x2BADB002 is provided!");
    }

//    /* Are mmap_* valid? */
//    if (CHECK_FLAG (boot_info->flags, 6))
//    {
//        multiboot_memory_map_t *mmap;
//
//        printf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
//                (unsigned) boot_info->mmap_addr, (unsigned) boot_info->mmap_length);
//        for (mmap = (multiboot_memory_map_t *) boot_info->mmap_addr;
//             (unsigned long) mmap < boot_info->mmap_addr + boot_info->mmap_length;
//             mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
//                                                + mmap->size + sizeof (mmap->size)))
//            printf (" size = 0x%x, base_addr = 0x%x%x,"
//                            " length = 0x%x%x, type = 0x%x\n",
//                    (unsigned) mmap->size,
//                    mmap->addr >> 32,
//                    mmap->addr & 0xffffffff,
//                    mmap->len >> 32,
//                    mmap->len & 0xffffffff,
//                    (unsigned) mmap->type);
//    }

	qemu_gdb_hang();

    init_idt();

    // на нулевой ноге мастера ждем прерывание и отобразим его в 32 запись idt
    init_interrupt_controller();

    // разрешаем процессору принимать masked interruptions
    enable_ints();

    start_pit_interruptions();
    out8(MASTER_DATA, 0b11111110);

    while (1);
}
