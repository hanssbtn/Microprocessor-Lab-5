#include "timer.h"

void init_timer1(void) {
	TCCR1B = 1 << WGM12;
	TCCR1A = 0;
}

void delay_ms(uint16_t t_ms) {
	// set delay
	OCR1A = t_ms << 4;
	TCNT1 = 0;
	// prescaler 1024
	TCCR1B |= (1 << CS12) | (1 << CS10);
	while (!(TIFR1 & (1 << OCF1A)));
	TIFR1 |= 1 << OCF1A;
	TCCR1B = 1 << WGM12;
}