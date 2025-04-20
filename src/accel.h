#ifndef ACCEL_H__
#define ACCEL_H__

#include "common.h"
#include "i2c.h"

#define MPU6050_I2C_ADDRESS_DEFAULT 0x68
#define MPU6050_I2C_ADDRESS_ALTERNATIVE 0x69
#define MPU6050_I2C_ADDRESS MPU6050_I2C_ADDRESS_ALTERNATIVE

// MPU-6050 Register Defines (Commonly Read Registers)

// --- Configuration Registers (R/W - Often Read to Verify Settings) ---
#define MPU6050_REG_SMPLRT_DIV       0x19  // Sample Rate Divider (R/W)
                                          // Output Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
#define MPU6050_REG_CONFIG           0x1A  // Configuration (R/W)
                                          // - FS_SYNC: Frame Synchronization config
                                          // - DLPF_CFG: Digital Low Pass Filter configuration
#define MPU6050_REG_GYRO_CONFIG      0x1B  // Gyroscope Configuration (R/W)
                                          // - FS_SEL: Gyro Full Scale Range (+/- 250, 500, 1000, 2000 dps)
                                          // - Self-test bits
#define MPU6050_REG_ACCEL_CONFIG     0x1C  // Accelerometer Configuration (R/W)
                                          // - AFS_SEL: Accel Full Scale Range (+/- 2, 4, 8, 16g)
                                          // - Self-test bits
#define MPU6050_REG_FIFO_EN          0x23  // FIFO Enable (R/W)
                                          // - Which sensor data is loaded into the FIFO buffer

// --- Interrupt Status/Configuration Registers (R/W or R - Read for Status) ---
#define MPU6050_REG_INT_PIN_CFG      0x37  // INT Pin / Bypass Enable Configuration (R/W)
                                          // - Interrupt pin logic level, open-drain/push-pull, latching, etc.
#define MPU6050_REG_INT_ENABLE       0x38  // Interrupt Enable (R/W)
                                          // - Which interrupt sources are enabled (e.g., data ready, FIFO overflow)
#define MPU6050_REG_INT_STATUS       0x3A  // Interrupt Status (R)
                                          // - Flags indicating which interrupt events have occurred (Read to clear)

// --- Sensor Data Registers (R - Read-Only Data Output) ---
// Note: These are 16-bit values, read as High Byte then Low Byte
#define MPU6050_REG_ACCEL_XOUT_H     0x3B  // Accelerometer Measurements X-Axis High Byte (R)
#define MPU6050_REG_ACCEL_XOUT_L     0x3C  // Accelerometer Measurements X-Axis Low Byte (R)
#define MPU6050_REG_ACCEL_YOUT_H     0x3D  // Accelerometer Measurements Y-Axis High Byte (R)
#define MPU6050_REG_ACCEL_YOUT_L     0x3E  // Accelerometer Measurements Y-Axis Low Byte (R)
#define MPU6050_REG_ACCEL_ZOUT_H     0x3F  // Accelerometer Measurements Z-Axis High Byte (R)
#define MPU6050_REG_ACCEL_ZOUT_L     0x40  // Accelerometer Measurements Z-Axis Low Byte (R)

#define MPU6050_REG_TEMP_OUT_H       0x41  // Temperature Measurement High Byte (R)
#define MPU6050_REG_TEMP_OUT_L       0x42  // Temperature Measurement Low Byte (R)
                                          // Formula: Temp Celsius = (TEMP_OUT / 340) + 36.53

#define MPU6050_REG_GYRO_XOUT_H      0x43  // Gyroscope Measurements X-Axis High Byte (R)
#define MPU6050_REG_GYRO_XOUT_L      0x44  // Gyroscope Measurements X-Axis Low Byte (R)
#define MPU6050_REG_GYRO_YOUT_H      0x45  // Gyroscope Measurements Y-Axis High Byte (R)
#define MPU6050_REG_GYRO_YOUT_L      0x46  // Gyroscope Measurements Y-Axis Low Byte (R)
#define MPU6050_REG_GYRO_ZOUT_H      0x47  // Gyroscope Measurements Z-Axis High Byte (R)
#define MPU6050_REG_GYRO_ZOUT_L      0x48  // Gyroscope Measurements Z-Axis Low Byte (R)

// --- FIFO Buffer Registers (R/W or R - Read for Status/Data) ---
#define MPU6050_REG_USER_CTRL        0x6A  // User Control (R/W)
                                          // - FIFO_EN: Enable FIFO buffer
                                          // - I2C_MST_EN: Enable Auxiliary I2C Master mode
                                          // - FIFO_RESET: Reset FIFO buffer
                                          // - I2C_MST_RESET: Reset Auxiliary I2C Master module
                                          // - SIG_COND_RESET: Reset sensor signal paths
#define MPU6050_REG_FIFO_COUNTH      0x72  // FIFO Measurement Count High Byte (R)
#define MPU6050_REG_FIFO_COUNTL      0x73  // FIFO Measurement Count Low Byte (R)
                                          // Total bytes in FIFO = (FIFO_COUNTH << 8) | FIFO_COUNTL
#define MPU6050_REG_FIFO_R_W         0x74  // FIFO Read Write (R/W)
                                          // - Read from this register to get data from the FIFO buffer
                                          // - Write to this register *if* using the FIFO for slave sensor writes (less common)

// --- Power Management & Device ID (R/W or R) ---
#define MPU6050_REG_PWR_MGMT_1       0x6B  // Power Management 1 (R/W)
                                          // - DEVICE_RESET: Trigger full chip reset
                                          // - SLEEP: Enable low-power sleep mode
                                          // - CYCLE: Enable cyclic measurement mode
                                          // - TEMP_DIS: Disable temperature sensor
                                          // - CLKSEL: Select clock source
#define MPU6050_REG_PWR_MGMT_2       0x6C  // Power Management 2 (R/W)
                                          // - LP_WAKE_CTRL: Wake-up frequency in low-power mode
                                          // - STBY_XA, STBY_YA, STBY_ZA: Place Accel axes into standby
                                          // - STBY_XG, STBY_YG, STBY_ZG: Place Gyro axes into standby

#define MPU6050_REG_WHO_AM_I         0x75  // Who Am I (R)
                                          // - Contains the device ID (Should read 0x68)
                                          // - Used to verify communication with the MPU-6050

#endif // ACCEL_H__