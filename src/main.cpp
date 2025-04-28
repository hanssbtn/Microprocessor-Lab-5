#include "common.h" 
#include "i2c.h"
#include "switch.h"
#include "accel.h"
#include "timer.h"
#include "spi.h"
#include "pwm.h"
#include "led_matrix.h"
#include "buzzer.h"
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
bool enable = false;

int16_t main(void) {
    DEBUG_INIT(9600);
    
    init_timer1();
    init_timer3();
    init_switch_pin();
    init_piezo_alarm_pin(); 
    init_spi();   
    init_led_matrix();
    sei();
    delay_ms(50);
    InitI2C();
    
    get_status_name(StartI2C_Trans(MPU6050_I2C_ADDRESS));
    get_status_name(Write(MPU6050_REG_PWR_MGMT_1));
    get_status_name(Write(0));
    get_status_name(StopI2C_Trans());
    
    // Verify the WHO_AM_I register to check if the accelerometer is connected correctly
    get_status_name(Read_from(MPU6050_I2C_ADDRESS, MPU6050_REG_WHO_AM_I));
    if (Read_data() != MPU6050_I2C_ADDRESS_DEFAULT) {
  		DEBUG_PRINTLN("BRUH");
        while (1);
    }
    
    // Configure the accelerometer's acceleration sensitivity (set to default values)
    get_status_name(StartI2C_Trans(MPU6050_I2C_ADDRESS));
    get_status_name(Write(MPU6050_REG_ACCEL_CONFIG));
    get_status_name(Write(0));  // Set to default sensitivity
    get_status_name(StopI2C_Trans());
    
    // Variables to store accelerometer data
    int16_t x = 0, y = 0, z = 0;  // Raw accelerometer readings
    double x_n = 0, y_n = 0, z_n = 0;  // Normalized accelerometer values (in g)
    
    DEBUG_PRINT("=====================================\n\n");
    
    delay_ms(100);  // Delay for stabilizing the system
    
    while (1) {  // Main loop that runs forever

        // Check the switch state and update accordingly
        switch (switch_state) {
            case PRESSED: {
                switch_state = RELEASED;  // After press, change to released state
            } break;
            case AFTER_RELEASE: {
                switch_state = BEFORE_PRESS;  // After release, reset the state
                enable = false;  // Disable buzzer and LED patterns
                change_duty_cycle(0);  // Turn off buzzer
                disable_piezo_alarm();  // Disable piezo alarm
            } break;
            default: break;  // No change in other cases
        }

        // Read accelerometer data for X, Y, and Z axes
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
        
        // Print the raw accelerometer data
        DEBUG_PRINT("x: ");
        DEBUG_PRINT(x);
        DEBUG_PRINT(", y: ");
        DEBUG_PRINT(y);
        DEBUG_PRINT(", z: ");
        DEBUG_PRINTLN(z);

        // Normalize accelerometer data to g values (range: -1 to +1)
        x_n = x / 16384.0;  // Convert to normalized value (divide by sensitivity factor)
        y_n = y / 16384.0;
        z_n = z / 16384.0;

        // Calculate the roll and pitch angles using trigonometry (atan2d for 2D arctangent)
		double yaw = atan2d(z_n, sqrt(x_n * x_n + y_n * y_n));
        double roll = atan2d(y_n, sqrt(x_n * x_n + z_n * z_n));
        double pitch = atan2d(x_n, sqrt(y_n * y_n + z_n * z_n)) - 0.12;  // Adjust pitch by -0.12 for calibration
        
        DEBUG_PRINT("Yaw: ");
        DEBUG_PRINT(yaw);
        DEBUG_PRINT(", Roll: ");
        DEBUG_PRINT(roll);
        DEBUG_PRINT(", Pitch: ");
        DEBUG_PRINTLN(pitch);
        DEBUG_PRINT("=====================================\n\n");

        // Trigger a "bruh moment" if the roll or pitch exceeds 45 degrees (i.e., when tilted significantly)
        bool bruh_moment = roll >= 45.0 || pitch >= 45.0 || (yaw <= 45.0 && yaw >= -45.0) || pitch <= -45.0 || roll <= -45.0;
        
        if (bruh_moment) {
            enable_piezo_alarm();  // Enable the buzzer
            enable = true;  // Set the flag to indicate that buzzer and LED pattern should be shown
			for (int i = 0; i < 8; ++i) spi_write_byte(pgm_read_byte(&SAD[i]), i + 1);  // Display sad face
        } else {
			for (int i = 0; i < 8; ++i) spi_write_byte(pgm_read_byte(&HAPPY[i]), i + 1);  // Display happy face
		}

        // If enabled, show the "Sad" face pattern on the LED matrix and play the chirping sound
        if (enable) {
            DEBUG_PRINT("BRUH\n");
            play_chirping_sound();  // Play the buzzer sound
        } else {
            DEBUG_PRINT("SIGMA\n");
        }

        DEBUG_PRINT("=====================================\n\n");
        delay_ms(100);  // Delay before the next loop iteration
    }

    return 0;  // End of main function
}

// Interrupt Service Routine (ISR) for switch (button) press event
ISR(PCINT1_vect) {
    switch (switch_state) {
        case BEFORE_PRESS: {
            switch_state = PRESSED;  // Switch state to PRESSED when button is pressed
        } break;
        case RELEASED: {
            switch_state = AFTER_RELEASE;  // Switch state to AFTER_RELEASE when button is released
        } break;
        default: break;  // Do nothing in other cases
    }
}