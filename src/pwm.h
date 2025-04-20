#ifndef PWM_H__
#define PWM_H__

#include "common.h"

void init_piezo_alarm_pin(void);
void init_timer3(void);
void change_duty_cycle(int16_t analog_magnitude);
void enable_piezo_alarm(void);
void disable_piezo_alarm(void);

#endif // PWM_H__