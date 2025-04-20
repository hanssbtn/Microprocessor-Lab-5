#ifndef I2C_H__
#define I2C_H__

#include "common.h"
#include "timer.h"

#ifdef Arduino_h
void get_status_name(uint8_t status);
#else
#define get_status_name(status) (void)status;
#endif // Arduino_h

#define I2C_STATUS_BIT_MASK 0xF8
#define I2C_STATUS (TWSR & I2C_STATUS_BIT_MASK)

// General status codes
#define I2C_START                   0x08
#define I2C_REP_START               0x10
#define I2C_ARB_LOST                0x38
// Master Transmitter Mode Status Codes
#define I2C_MT_SLA_ACK              0x18
#define I2C_MT_SLA_NACK             0x20
#define I2C_MT_DATA_ACK             0x28
#define I2C_MT_DATA_NACK            0x30
// Master Receiver Mode Status Codes
#define I2C_MR_SLA_ACK              0x40
#define I2C_MR_SLA_NACK             0x48
#define I2C_MR_DATA_ACK             0x50
#define I2C_MR_DATA_NACK            0x58
// Slave Transmitter Mode Status Codes (Not implemented in this master-only library)
#define I2C_ST_SLA_ACK              0xA8
#define I2C_ST_ARB_LOST_SLA_ACK     0xB0
#define I2C_ST_DATA_ACK             0xB8
#define I2C_ST_DATA_NACK            0xC0
#define I2C_ST_LAST_DATA_ACK        0xC8
// Slave Receiver Mode Status Codes (Not implemented in this master-only library)
#define I2C_SR_SLA_ACK              0x60
#define I2C_SR_ARB_LOST_SLA_ACK     0x68
#define I2C_SR_GCALL_ACK            0x70
#define I2C_SR_ARB_LOST_GCALL_ACK   0x78
#define I2C_SR_DATA_ACK             0x80
#define I2C_SR_DATA_NACK            0x88
#define I2C_SR_GCALL_DATA_ACK       0x90
#define I2C_SR_GCALL_DATA_NACK      0x98
#define I2C_SR_STOP_REP_START       0xA0
// Miscellaneous Status Codes
#define I2C_BUS_ERROR               0x00
#define I2C_NO_INFO                 0xF8

// Function definitions
void InitI2C(void);
const uint8_t StartI2C_Trans(unsigned char SLA);
const uint8_t StopI2C_Trans(void);
const uint8_t Write(unsigned char data);
const uint8_t Read_from(unsigned char SLA, unsigned char MEMADDRESS);
unsigned char Read_data();

#ifdef Arduino_h

void get_status_name(uint8_t status);

#else

#define get_status_name(status) (void)status;

#endif


#endif // I2C_H__
