#include "common.h"
#include "i2c.h"
#include "switch.h"
#include "accel.h"
#include "timer.h"
#include "spi.h"
#include "pwm.h"
#include <avr/pgmspace.h>
#include "math.h"

#define atan2d(x, y) (atan2((x), (y))*RAD_TO_DEG)

#define BEFORE_PRESS 0
#define PRESSED 1
#define RELEASED 2
#define AFTER_RELEASE 3

static const uint8_t HAPPY[8] PROGMEM = {
	0x00, 0x66, 0x66, 0x00, 0x81, 0x42, 0x3C, 0x00
}; 
static const uint8_t SAD[8] PROGMEM = {
	0x00, 0x66, 0x66, 0x00, 0x3C, 0x42, 0x81, 0x00
}; 

volatile uint8_t switch_state = BEFORE_PRESS;
volatile uint8_t count = 0;

int16_t main(void) {
	DEBUG_INIT(9600);
	init_timer1();
	init_timer3();
	init_switch_pin();
	init_piezo_alarm_pin();
	init_spi();
	sei();
	InitI2C();
	delay_ms(50);

	// Write to accelerometer's power configuration register
	get_status_name(StartI2C_Trans(MPU6050_I2C_ADDRESS));
	get_status_name(Write(MPU6050_REG_PWR_MGMT_1));
	get_status_name(Write(0));
	get_status_name(StopI2C_Trans());
	
	// Write to accelerometer's acceleration configuration register
	get_status_name(StartI2C_Trans(MPU6050_I2C_ADDRESS));
	get_status_name(Write(MPU6050_REG_ACCEL_CONFIG));
	get_status_name(Write(0));
	get_status_name(StopI2C_Trans());
	int16_t x = 0, y = 0, z = 0;
	double x_n = 0, y_n = 0, z_n = 0;
	DEBUG_PRINT("=====================================\n\n");

	delay_ms(100);
	while (1) {

		switch (switch_state) {
			case PRESSED: {
				switch_state = RELEASED;
			} break;
			case AFTER_RELEASE: {
				switch_state = BEFORE_PRESS;
				count++;
				disable_piezo_alarm();
				enable_piezo_alarm();
			} break;
			default: break;
		}

		get_status_name(Read_from(MPU6050_I2C_ADDRESS, MPU6050_REG_ACCEL_XOUT_H));
		x = (Read_data()) << 8;
		
		get_status_name(Read_from(MPU6050_I2C_ADDRESS, MPU6050_REG_ACCEL_XOUT_L));
		x |= Read_data();
		get_status_name(Read_from(MPU6050_I2C_ADDRESS, MPU6050_REG_ACCEL_YOUT_H));
		y = (Read_data()) << 8;
		
		get_status_name(Read_from(MPU6050_I2C_ADDRESS, MPU6050_REG_ACCEL_YOUT_L));
		y |= Read_data();
		get_status_name(Read_from(MPU6050_I2C_ADDRESS, MPU6050_REG_ACCEL_ZOUT_H));
		z = (Read_data()) << 8;
		
		get_status_name(Read_from(MPU6050_I2C_ADDRESS, MPU6050_REG_ACCEL_ZOUT_L));
		z |= Read_data();
		
		DEBUG_PRINT("x: ");
		DEBUG_PRINT(x);
		DEBUG_PRINT(", y: ");
		DEBUG_PRINT(y);
		DEBUG_PRINT(", z: ");
		DEBUG_PRINTLN(z);

		x_n = x/16384.0;		
		y_n = y/16384.0;		
		z_n = z/16384.0;		

		double roll = atan2d(y_n, sqrt(x_n * x_n + z_n * z_n));
		double pitch = atan2d(x_n, sqrt(y_n * y_n + z_n * z_n)) - 0.12;
		DEBUG_PRINT("Roll: ");
		DEBUG_PRINT(roll);
		DEBUG_PRINT(", Pitch: ");
		DEBUG_PRINTLN(pitch);
		DEBUG_PRINT("=====================================\n\n");
		
		DEBUG_PRINT("count: ");
		DEBUG_PRINTLN(count);
		DEBUG_PRINT("=====================================\n\n");

		bool bruh_moment = roll <= 45.0 || pitch <= 45.0;
		if (bruh_moment) {
			DEBUG_PRINT("BRUH\n");
			change_duty_cycle(1000);
			for (int i = 1; i < 9; ++i) spi_write_byte(pgm_read_byte(&SAD[i - 1]), i);
		} else {
			DEBUG_PRINT("MOGGED\n");
			change_duty_cycle(0);
			for (int i = 1; i < 9; ++i) spi_write_byte(pgm_read_byte(&HAPPY[i - 1]), i);
		}
		DEBUG_PRINT(("=====================================\n\n"));
		delay_ms(100);
	}
	return 0;
}

ISR(PCINT1_vect) {
	switch (switch_state) {
		case BEFORE_PRESS: {
			switch_state = PRESSED;
		} break;
		case RELEASED: {
			switch_state = AFTER_RELEASE;
		} break;
		default: break;
	}
}