#include "pwm.h"

void init_piezo_alarm_pin(void) {
	DDRE |= (1 << DDE3);
	PORTE &= (1 << PORTE);
}

void init_timer3(void) {
    // Mode 7: Fast PWM, 10-bit (TOP = 0x3FF)
    // Non-inverting mode on OC3A (Clear OC3A on Compare Match, set OC3A at 0)
    TCCR3A = (1 << COM3A1) | (1 << WGM31) | (1 << WGM30);
    // WGM32 for Mode 7, No prescaling (CS30 == 1)
    TCCR3B = (1 << WGM32) | (1 << CS30);
	// Turn off all digital input registers to limit power consumption
	DIDR0 = 0xFF;
}

#define ADC_MAX 0x3FF // 1023 for 10-bit ADC
#define ADC_CENTER (ADC_MAX / 2) // 511
#define PWM_MAX 0x3FF // 1023 for 10-bit Fast PWM
#define DEAD_ZONE 5 // Adjust sensitivity around center (e.g., 512 +/- 20)

void change_duty_cycle(int16_t analog_magnitude) {
    int16_t duty_cycle = 0;

    // Counter-clockwise
    if (analog_magnitude < (ADC_CENTER - DEAD_ZONE)) {
        // Calculate speed: Map [0, ADC_CENTER - DEAD_ZONE - 1] to [PWM_MAX, 0]
        duty_cycle = (PWM_MAX * (int32_t)(ADC_CENTER - DEAD_ZONE - analog_magnitude)) / (ADC_CENTER - DEAD_ZONE);        
        // Set direction pins to move counter-clockwise (PH4 == 1, PH3 == 0)
        PORTH = (1 << PH4);

    // Clockwise
    } else if (analog_magnitude > (ADC_CENTER + DEAD_ZONE)) {
        // Calculate speed: Map [ADC_CENTER + DEAD_ZONE + 1, ADC_MAX] to [0, PWM_MAX]
        duty_cycle = (PWM_MAX * (int32_t)(analog_magnitude - (ADC_CENTER + DEAD_ZONE))) / (ADC_MAX - (ADC_CENTER + DEAD_ZONE));
        // Set direction pins to move clockwise (PH4 == 0, PH3 == 1)
        PORTH = (1 << PH3);
	// Stop
    } else {
        // Use bitwise operations to set direction pins low (brake).
        PORTH = 0;
    }

    // Set the PWM duty cycle register for Timer3 Output Compare A (OC3A = PE3)
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