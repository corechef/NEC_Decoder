#ifndef UART_H
#define UART_H

#include <stdint.h>

void init_uart(void);
uint8_t read_usart();
void write_usart(uint8_t data);

#endif // UART_H