#include <stdint.h>

#ifndef SRC_SERIAL_H
#define SRC_SERIAL_H

void serial_setup(void);
void serial_putchar(int c);
void serial_write(const char *buf, uint32_t size);
void print_string(const char *);

#endif //SRC_SERIAL_H
