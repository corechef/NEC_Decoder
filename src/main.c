#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

enum nec_decoder_state_t
{
    NEC_DECODER_IDLE,
    NEC_DECODER_HEADER,
    NEC_DECODER_DECODING,
    NEC_DECODER_STATE_MAX
};

enum nec_decoder_state_t state = 0;

// Start with  NEC_DECODER_IDLE,
// on first interrupt, record time and transition to
// NEC_DECODER_HEADER,

int main(void)
{
    // init_uart()
    init_uart();
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
    static int count = 0;
    write_usart('a' + ((count++) % 10));
}