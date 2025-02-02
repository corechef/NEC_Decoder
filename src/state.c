#include "uart.h"
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

#define BUTTON_0 0x00FF9867
#define BUTTON_1 0x00FFA25D
#define BUTTON_2 0x00FF629D
#define BUTTON_3 0x00FFE21D
#define BUTTON_4 0x00FF22DD
#define BUTTON_5 0x00FF02FD
#define BUTTON_6 0x00FFC23D
#define BUTTON_7 0x00FFE01F
#define BUTTON_8 0x00FFA857
#define BUTTON_9 0x00FF906F
#define BUTTON_HASHTAG 0x00FFB04F
#define BUTTON_STAR 0x00FF6897
#define BUTTON_UP 0x00FF18E7
#define BUTTON_RIGHT 0x00FF5AA5
#define BUTTON_LEFT 0x00FF10EF
#define BUTTON_DOWN 0x00FF4AB5
#define BUTTON_OK 0x00FF38C7

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

void init_timer(void)
{
    TCCR1A |= _BV(WGM10) | _BV(WGM11);
    TCCR1B |= _BV(WGM12) | _BV(WGM13);
    OCR1A = HEADER_TICK_UPPERBOUND;
    TIMSK1 = _BV(TOIE1);
}

void start_timer(void)
{
    // init 16-bit timer with prescaler 64
    TCCR1B |= _BV(CS11) | _BV(CS10);
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

bool check_complement(void)
{
    uint8_t byte1 = (command >> 24) & 0xFF;
    uint8_t byte2 = (command >> 16) & 0xFF;
    uint8_t byte3 = (command >> 8) & 0xFF;
    uint8_t byte4 = (command >> 0) & 0xFF;

    bool first_pair = (byte1 == (uint8_t)(~byte2));
    bool second_pair = (byte3 == (uint8_t)(~byte4));

    return first_pair && second_pair;
}

void execute_something(void)
{
    if (command == BUTTON_1)
    {
        write_usart('I');
    } else if (command == BUTTON_2) {
        write_usart('2');
    } else if (command == BUTTON_3) {
        write_usart('3');
    } else if (command == BUTTON_4) {
        write_usart('4');
    } else if (command == BUTTON_5) {
        write_usart('5');
    } else if (command == BUTTON_6) {
        write_usart('6');
    } else if (command == BUTTON_7) {
        write_usart('7');
    } else if (command == BUTTON_8) {
        write_usart('8');
    } else if (command == BUTTON_9) {
        write_usart('9');
    } else if (command == BUTTON_0) {
        write_usart('O');
    } else if (command == BUTTON_HASHTAG) {
        write_usart('#');
    } else if (command == BUTTON_STAR) {
        write_usart('*');
    } else if (command == BUTTON_UP) {
        write_usart('^');
    } else if (command == BUTTON_RIGHT) {
        write_usart('>');
    } else if (command == BUTTON_LEFT) {
        write_usart('<');
    } else if (command == BUTTON_DOWN) {
        write_usart('v');
    } else if (command == BUTTON_OK) {
        write_usart('k');
    }
}