#include <serial.h>
#include <ioport.h>

#include "../inc/serial.h"
#include "../inc/ioport.h"

#define SERIAL_PORT 0x3f8   /* serial port */

void serial_setup(void) {
    out8(SERIAL_PORT + 1, 0x00);    // Disable all interrupts
    out8(SERIAL_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    out8(SERIAL_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    out8(SERIAL_PORT + 1, 0x00);    //                  (hi byte)
    out8(SERIAL_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
}

int is_transmit_done() {
    return in8(SERIAL_PORT + 5) & 0x20; // if 5th bit is unset, return zero
}

void serial_putchar(int c) {

    while (!is_transmit_done());

    out8(SERIAL_PORT,c);

}

void print_string(const char* string_to_print) {

    for (unsigned int i=0; string_to_print[i] != '\0'; i++){
        serial_putchar(string_to_print[i]);
    }
}

void serial_write(const char* string_to_print, uint32_t length) {

    for (unsigned int i=0; i < length ; ++i){
        serial_putchar(string_to_print[i]);
    }
}

