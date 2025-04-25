#include "pwm.h"

void init_piezo_alarm_pin(void) {
	DDRE |= (1 << DDE3);
	PORTE &= (1 << PORTE);
}

void init_timer3(void) {
    // Mode 7: Fast PWM, 10-bit (TOP = 0x3FF)
    // Non-inverting mode on OC3A (Clear OC3A on Compare Match, set OC3A at 0)
    TCCR3A = (1 << WGM31) | (1 << WGM30);
    // WGM32 for Mode 7, No prescaling (CS30 == 1)
    TCCR3B = (1 << WGM32) | (1 << CS30);
	// Turn off all digital input registers to limit power consumption
	DIDR0 = 0xFF;
}

void change_duty_cycle(int16_t duty_cycle) {
    OCR3A = duty_cycle;
}


void enable_piezo_alarm(void) {
	// Enable Timer3 PWM Output 3
    TCCR3A |= (1 << COM3A1); 
}

void disable_piezo_alarm(void) {
    // Disconnect Timer3 PWM output 3 from pin PE3 by disconnecting OC3A
    TCCR3A &= ~(1 << COM3A1); 

    // Turn off motor
	OCR3A = 0;  // Set duty cycle to 0
    PORTE = 0;  // Force PE3 low
}