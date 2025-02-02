#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "state.h"

int main(void)
{
    init_uart();

    init_timer();

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
            // redo the last command
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
            if (check_complement())
            {
                // your code executing command here
                execute_something();
            }
            else
            {
                write_usart('m');
            }

            transition_to_idle_state();
        }
    }
}

ISR(TIMER1_OVF_vect)
{
    transition_to_idle_state();
}