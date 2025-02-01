#include <stdbool.h>
#include <stdint.h>

bool is_state_idle(void);

bool is_state_expecting_header(void);

bool is_state_decoding(void);

bool is_last_decoding_done(void);

void init_timer(void);

void start_timer(void);

void setup_interrupt_pin(void);

void enable_external_interrupt(void);

void transition_to_expecting_header(void);

void transition_to_idle_state(void);

void transition_to_decoding(void);

void decode_bit_0(void);

void decode_bit_1(void);

void record_time_and_reset_timer(void);

bool got_reset_signal(void);

bool got_header_signal(void);

bool got_zero_bit_signal(void);

bool got_one_bit_signal(void);

uint32_t get_command(void);