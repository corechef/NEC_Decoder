#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "state.h"
#include "stdlib.h"

// Start with  NEC_DECODER_IDLE,
// on first interrupt, record time and transition to
// NEC_DECODER_HEADER,

int main(void)
{
    init_uart();

    start_timer();

    setup_interrupt_pin();

    enable_external_interrupt();

    // enable global interrupts
    sei();

    while (1) {
    }
}


ISR(INT0_vect)
{
    record_time_and_reset_timer();

    if (is_state_idle())
    {
        transition_to_expecting_header();
        write_usart('i');
    }
    else if (is_state_expecting_header())
    {
        if (got_reset_signal())
        {
            transition_to_idle_state();
            write_usart('r');
        }
        else if (got_header_signal())
        {
            transition_to_decoding();
            write_usart('h');
        }
        else
        {
            transition_to_idle_state();
            write_usart('e');
        }
    }
    else if (is_state_decoding())
    {
        if (got_zero_bit_signal())
        {
            decode_bit_0();
            write_usart('0');
        }
        else if (got_one_bit_signal())
        {
            decode_bit_1();
            write_usart('1');
        }
        else 
        {
            transition_to_idle_state();
            write_usart('e');
        }

        if (is_last_decoding_done())
        {
            // run your code here
            char number[8];
            ultoa(get_command(), number, 16);
            write_usart('c');
            write_usart(number[0]);
            write_usart(number[1]);
            write_usart(number[2]);
            write_usart(number[3]);
            write_usart(number[4]);
            write_usart(number[5]);
            write_usart(number[6]);
            write_usart(number[7]);
            write_usart('h');
            write_usart('\n');

            transition_to_idle_state();
        }
    }
}