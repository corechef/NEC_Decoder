#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#define BITS_PER_NEC_PACKET 32

#define CLOCKS_PER_MICROSECOND (F_CPU / 1000000L)
#define TIMER_PRESCALER (64L)

#define TICKS_GIVEN_MICROSECS(x) ((x * CLOCKS_PER_MICROSECOND) / TIMER_PRESCALER)
#define RESET_TICK_LOWBOUND TICKS_GIVEN_MICROSECS(11300L)
#define RESET_TICK_UPPERBOUND TICKS_GIVEN_MICROSECS(11400L)
#define HEADER_TICK_LOWBOUND TICKS_GIVEN_MICROSECS(13550L)
#define HEADER_TICK_UPPERBOUND TICKS_GIVEN_MICROSECS(13750L)
#define ZERO_BIT_TICK_LOWBOUND TICKS_GIVEN_MICROSECS(1100L)
#define ZERO_BIT_TICK_UPPERBOUND TICKS_GIVEN_MICROSECS(1220L)
#define ONE_BIT_TICK_LOWBOUND TICKS_GIVEN_MICROSECS(2180L)
#define ONE_BIT_TICK_UPPERBOUND TICKS_GIVEN_MICROSECS(2280L)

enum nec_decoder_state_t
{
    NEC_DECODER_IDLE,
    NEC_DECODER_EXPECTING_HEADER,
    NEC_DECODER_DECODING,
    NEC_DECODER_STATE_MAX
};

enum nec_decoder_state_t state = NEC_DECODER_IDLE;
uint8_t received_bit_count = 0;
uint16_t current_time = 0;
uint32_t command = 0;

bool is_state_idle(void)
{
    return NEC_DECODER_IDLE == state;
}

bool is_state_expecting_header(void)
{
    return NEC_DECODER_EXPECTING_HEADER == state;
}

bool is_state_decoding(void)
{
    return NEC_DECODER_DECODING == state;
}

bool is_last_decoding_done(void)
{
    return received_bit_count == BITS_PER_NEC_PACKET;
}

void start_timer(void)
{
    // init 16-bit timer with prescaler 64
    TCCR1B = _BV(CS11) | _BV(CS10);
}

void setup_interrupt_pin(void)
{
    // Setup interrupt for falling edge for INT0 pin (PD2)
    EICRA = _BV(ISC01);
}

void enable_external_interrupt(void)
{
    // Enable interrupt for the INT0 pin (PD2)
    EIMSK = _BV(INT0);
}

void transition_to_expecting_header(void)
{
    state = NEC_DECODER_EXPECTING_HEADER;
}

void transition_to_decoding(void)
{
    state = NEC_DECODER_DECODING;
}

void transition_to_idle_state(void)
{
    state = NEC_DECODER_IDLE;
    received_bit_count = 0;
    command = 0;
}

void decode_bit_0(void)
{
    // decoded a 0
    command *= 2;
    // increment decode counter
    received_bit_count++;
}

void decode_bit_1(void)
{
    // decoded a 1
    command *= 2;
    command += 1;
    // increment decode counter
    received_bit_count++;
}

void record_time_and_reset_timer(void)
{
    current_time = TCNT1;
    TCNT1 = 0;
}

bool got_reset_signal(void)
{
    return (RESET_TICK_LOWBOUND < current_time && current_time < RESET_TICK_UPPERBOUND);
}

bool got_header_signal(void)
{
    return (HEADER_TICK_LOWBOUND < current_time && current_time < HEADER_TICK_UPPERBOUND);
}

bool got_zero_bit_signal(void)
{
    return (ZERO_BIT_TICK_LOWBOUND < current_time && current_time < ZERO_BIT_TICK_UPPERBOUND);
}

bool got_one_bit_signal(void)
{
    return (ONE_BIT_TICK_LOWBOUND < current_time && current_time < ONE_BIT_TICK_UPPERBOUND);
}

uint32_t get_command(void)
{
    return command;
}