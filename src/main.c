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

extern uint64_t table[]; // обработчики

static struct idt_entry idt_table[33];

#define PORT 0x3f8   /* serial port */

void init_serial() {
    out8(PORT + 1, 0x00);    // Disable all interrupts
    out8(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    out8(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    out8(PORT + 1, 0x00);    //                  (hi byte)
    out8(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
}

int is_transmit_done() {
    return in8(PORT + 5) & 0x20; // if 5th bit is unset, return zero
}

void write_to_serial(char a) {
    while (!is_transmit_done());

    out8(PORT,a);
}

void print_string(char* string_to_print) {

    init_serial();

    for (unsigned int i=0; string_to_print[i] != '\0'; i++){
        write_to_serial(string_to_print[i]);
    }
}

// todo добавить аргумент - frame
void c_handler() {
    char* ploho = "vse ochen ploho\n\0";
    print_string(ploho);
}
void c_handler2() {
    char* ploho = "vse ochen horosho\n\0";
    print_string(ploho);
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

    uint32_t master_command = 0x20;
    uint32_t master_data = 0x21;

    uint8_t word = 0b11001000; //not using second controller
    uint8_t first_byte = 32;
    uint8_t second_byte = 0b00100000;
    uint8_t third_byte = 0b10000000;
    uint8_t master_mask = 0b01111111;
    uint8_t undirected_eoi = 0b00000100;

    out8(master_command, word);
    out8(master_data, first_byte);
    out8(master_data, second_byte);
    out8(master_data, third_byte);

    // теперь замаскируем ненужные ноги
    out8(master_data, master_mask);

}

void main(void)
{
	qemu_gdb_hang();

    init_idt();

    // на нулевой ноге мастера ждем прерывание и отобразим его в 32 запись idt
    init_interrupt_controller();


    //int test0 = 0;
    //int test = 2 / test0;

    //long test2 = INT64_MIN / -1;
    cause_interrupt();

    char* helloworld = "hello, world!\0";
    print_string(helloworld);


	//while (1);
}
