#include "buzzer.h"

#define FREQUENCY 700
void play_chirping_sound(void) {
	change_duty_cycle(FREQUENCY);
	delay_ms(100);
	change_duty_cycle(FREQUENCY / 2);
	delay_ms(100);
}