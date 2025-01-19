#include <avr/io.h>
#include <avr/wdt.h>
#include "uart.h"

void init_uart(void)
{
  UCSR0A = _BV(U2X0);
  UCSR0B = (_BV(RXEN0) | _BV(TXEN0));
  UCSR0C = (_BV(UCSZ01) | _BV(UCSZ00));
  UBRR0L = 0x10;
}

uint8_t read_usart()
{
  loop_until_bit_is_set(UCSR0A, RXC0);

  if (bit_is_clear(UCSR0A, FE0))
  {
    wdt_reset();
  }

  return UDR0;
}

void write_usart(uint8_t data)
{
  loop_until_bit_is_set(UCSR0A, UDRE0);

  UDR0 = data;
}

