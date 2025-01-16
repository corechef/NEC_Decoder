#include <avr/io.h>

enum nec_decoder_state_t
{
    NEC_DECODER_IDLE,
    NEC_DECODER_HEADER,
    NEC_DECODER_DECODING,
    NEC_DECODER_STATE_MAX
};

enum nec_decoder_state_t state = 0;

int main(void)
{
    while (1) {}
}