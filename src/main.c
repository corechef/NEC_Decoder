#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>

enum nec_decoder_state_t
{
    NEC_DECODER_IDLE,
    NEC_DECODER_EXPECTING_HEADER,
    NEC_DECODER_DECODING,
    NEC_DECODER_STATE_MAX
};

enum nec_decoder_state_t state = NEC_DECODER_IDLE;
int count = 0;
uint16_t prev_time = 0;
uint16_t current_time = 0;
uint32_t command = 0;

// Start with  NEC_DECODER_IDLE,
// on first interrupt, record time and transition to
// NEC_DECODER_HEADER,

int main(void)
{
    // init_uart()
    init_uart();

    // init 16-bit timer with prescaler 64
    TCCR1B = _BV(CS11) | _BV(CS10);

    // Setup interrupt for falling edge for INT0 pin (PD2)
    EICRA = _BV(ISC01);

    // Enable interrupt for the INT0 pin (PD2)
    EIMSK = _BV(INT0);

    // enable global interrupts
    sei();

    while (1) {
    }
}


ISR(INT0_vect)
{
    current_time = TCNT1;
    TCNT1 = 0;
    if (state == NEC_DECODER_IDLE)
    {
        write_usart('i');
        write_usart('\r');
        write_usart('\n');
        state = NEC_DECODER_EXPECTING_HEADER;
    }
    else if (state == NEC_DECODER_EXPECTING_HEADER)
    {
        if (2700 < current_time && current_time < 2900)
        {
            // take action on repeat
            write_usart('r');
            // then go idle, reset state
            state = NEC_DECODER_IDLE;
            count = 0;
            command = 0;
        }
        else if (3300 < current_time && current_time < 3500)
        {
            state = NEC_DECODER_DECODING;
            // Let next states take action by actually decoding the command
            write_usart('d');
        }
        else {
            // Failure, reset state
            write_usart('f');
            write_usart('a');
            state = NEC_DECODER_IDLE;
            count = 0;
            command = 0;
        }
    }
    else if (state == NEC_DECODER_DECODING)
    {
        if (270 < current_time && current_time < 300)
        {
            // decoded a 0
            command *= 2;
            // increment decode counter
            count++;
            write_usart('0');
        }
        else if (540 < current_time && current_time < 580)
        {
            // decoded a 1
            command *= 2;
            command += 1;
            // increment decode counter
            count++;
            write_usart('1');
        }
        else 
        {
            // failure, reset state
            state = NEC_DECODER_IDLE;
            count = 0;
            command = 0;
            write_usart('f');
            write_usart('b');
        }

        if (count == 32)
        {
            // command read successfully
            // execute command
            // reset state
            state = NEC_DECODER_IDLE;
            count = 0;
            command = 0;
            write_usart('s');
            write_usart('\r');
            write_usart('\n');
        }
    }
}