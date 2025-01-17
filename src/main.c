#include <avr/io.h>

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
    // Setup interrupt for falling edge
    // Enable interrupt for the pin
    // enable global interrupts
    while (1) {}
}