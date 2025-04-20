#include "i2c.h"
#include <avr/io.h>
#define I2C_WAIT_TRANSFER() while(!(TWCR & (1 << TWINT)))

#define I2C_CMD_START      ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN))
#define I2C_CMD_STOP       ((1 << TWINT) | (1 << TWSTO) | (1 << TWEN)) // No Interrupt for STOP
#define I2C_CMD_TRANSMIT   ((1 << TWINT) | (1 << TWEN))
#define I2C_CMD_RECEIVE_ACK ((1 << TWINT) | (1 << TWEA) | (1 << TWEN))
#define I2C_CMD_RECEIVE_NACK ((1 << TWINT) | (1 << TWEN)) // No TWEA for NACK
#define I2C_CMD_CLEAR_TWINT ((1 << TWINT) | (1 << TWEN)) // Used sometimes just to clear flag

#ifdef Arduino_h
void get_status_name(uint8_t status) {
	DEBUG_PRINT("Status: 0x");
	DEBUG_PRINT(status, HEX);
	switch (status) {
		// ----- General Master States -----
		case I2C_START:           // 0x08
			DEBUG_PRINT(" (START condition transmitted)\n");
			break;
		case I2C_REP_START:       // 0x10
			DEBUG_PRINT(" (Repeated START condition transmitted)\n");
			break;

		// ----- Master Transmitter (MT) States -----
		case I2C_MT_SLA_ACK:      // 0x18
			DEBUG_PRINT(" (MT: SLA+W transmitted, ACK received)\n");
			break;
		case I2C_MT_SLA_NACK:     // 0x20
			DEBUG_PRINT(" (MT: SLA+W transmitted, NACK received (Error: Device not responding?))\n");
			break;
		case I2C_MT_DATA_ACK:     // 0x28
			DEBUG_PRINT(" (MT: Data byte transmitted, ACK received)\n");
			break;
		case I2C_MT_DATA_NACK:    // 0x30
			DEBUG_PRINT(" (MT: Data byte transmitted, NACK received (Error: Device rejected data?))\n");
			break;
		case I2C_ARB_LOST:     // 0x38 (Also used in MR mode)
			DEBUG_PRINT(" (MT/MR: Arbitration lost (Error: Another master took control?))\n");
			break;

		// ----- Master Receiver (MR) States -----
		// Note: I2C_MR_ARB_LOST is the same value as I2C_MT_ARB_LOST (0x38)
		case I2C_MR_SLA_ACK:      // 0x40
			DEBUG_PRINT(" (MR: SLA+R transmitted, ACK received)\n");
			break;
		case I2C_MR_SLA_NACK:     // 0x48
			DEBUG_PRINT(" (MR: SLA+R transmitted, NACK received (Error: Device not responding?))\n");
			break;
		case I2C_MR_DATA_ACK:     // 0x50
			DEBUG_PRINT(" (MR: Data byte received, ACK returned (More data coming))\n");
			break;
		case I2C_MR_DATA_NACK:    // 0x58
			DEBUG_PRINT(" (MR: Data byte received, NACK returned (Last byte received))\n");
			break;

		// ----- Slave Receiver (SR) States -----
		case I2C_SR_SLA_ACK:      // 0x60
			DEBUG_PRINT(" (SR: Own SLA+W received, ACK returned)\n");
			break;
		case I2C_SR_ARB_LOST_SLA_ACK: // 0x68
			DEBUG_PRINT(" (SR: Arbitration lost in SLA+RW as Master; Own SLA+W received, ACK returned)\n");
			break;
		case I2C_SR_GCALL_ACK:    // 0x70
			DEBUG_PRINT(" (SR: General call address received, ACK returned)\n");
			break;
		case I2C_SR_ARB_LOST_GCALL_ACK: // 0x78
			DEBUG_PRINT(" (SR: Arbitration lost in SLA+RW as Master; General call received, ACK returned)\n");
			break;
		case I2C_SR_DATA_ACK:     // 0x80
			DEBUG_PRINT(" (SR: Data byte received (for own SLA+W), ACK returned)\n");
			break;
		case I2C_SR_DATA_NACK:    // 0x88
			DEBUG_PRINT(" (SR: Data byte received (for own SLA+W), NACK returned (Error condition?))\n");
			break;
		case I2C_SR_GCALL_DATA_ACK: // 0x90
			DEBUG_PRINT(" (SR: Data byte received (for general call), ACK returned)\n");
			break;
		case I2C_SR_GCALL_DATA_NACK: // 0x98
			DEBUG_PRINT(" (SR: Data byte received (for general call), NACK returned (Error condition?))\n");
			break;
		case I2C_SR_STOP_REP_START: // 0xA0
			DEBUG_PRINT(" (SR: STOP or Repeated START received while still addressed as slave)\n");
			break;

		// ----- Slave Transmitter (ST) States -----
		case I2C_ST_SLA_ACK:      // 0xA8
			DEBUG_PRINT(" (ST: Own SLA+R received, ACK returned)\n");
			break;
		case I2C_ST_ARB_LOST_SLA_ACK: // 0xB0
			DEBUG_PRINT(" (ST: Arbitration lost in SLA+RW as Master; Own SLA+R received, ACK returned)\n");
			break;
		case I2C_ST_DATA_ACK:     // 0xB8
			DEBUG_PRINT(" (ST: Data byte transmitted, ACK received (Master expects more data))\n");
			break;
		case I2C_ST_DATA_NACK:    // 0xC0
			DEBUG_PRINT(" (ST: Data byte transmitted, NACK received (Master received last byte or error))\n");
			break;
		case 0xC8:    // 0xC8
			DEBUG_PRINT(" (ST: Last data byte transmitted (TWEA=0), ACK received)\n"); // Note: Specification says NACK (0xC0) is expected here.
			break;

		// ----- Custom States -----
		
		case 0x1: 
			DEBUG_PRINT(" (I2C STOP successful)\n");
			break;
		case 0xE8:
			DEBUG_PRINT(" (I2C STOP Timeout occurred_\n");
			break;

		// ----- Miscellaneous States -----
		case I2C_NO_INFO:         // 0xF8
			DEBUG_PRINT(" (No relevant state information available; TWINT=0)\n");
			break;
		case I2C_BUS_ERROR:       // 0x00
			DEBUG_PRINT(" (Bus error due to illegal START or STOP condition)\n");
			break;
		default:
			DEBUG_PRINT(" (Unknown)\n");
			break;
	}
}
#endif

#include <stdint.h>
#include <avr/io.h>
const uint8_t __start(uint8_t SLA, bool read) {
	TWCR = I2C_CMD_START;
	I2C_WAIT_TRANSFER();
	if (I2C_STATUS != I2C_START && I2C_STATUS != I2C_REP_START) {
		DEBUG_PRINT("Failed to send start signal (got ");
		DEBUG_PRINT(I2C_STATUS);
		DEBUG_PRINT(")\n");
		return I2C_STATUS;
	}
	
	TWDR = (SLA << 1) | read;
	TWCR = I2C_CMD_TRANSMIT;
	I2C_WAIT_TRANSFER();
	uint8_t status = I2C_STATUS;
	uint8_t expected_ack = read ? I2C_MR_SLA_ACK : I2C_MT_SLA_ACK;
    if (status != expected_ack) {
        DEBUG_PRINT("Failed SLA+RW ACK (expected 0x"); DEBUG_PRINT(expected_ack, HEX);
        DEBUG_PRINT(", got 0x"); DEBUG_PRINT(status, HEX); DEBUG_PRINTLN(")");
        // Optional: Send STOP on error?
        StopI2C_Trans();
        return status; // Return error status
    }

	return 1;
}

void InitI2C(void) {
	PRR0 &= ~(1 << PRTWI); // wake up I2C module on AT2560 power management register
	DDRD &= ~((1 << PD0) | (1 << PD1));
	PORTD |= ((1 << PD0) | (1 << PD1));
	TWSR = (TWSR & ~(1 << TWPS1)) | (1 << TWPS0);
	TWBR = 0xC6;
}

const uint8_t StartI2C_Trans(unsigned char SLA) {
	return __start(SLA, 0);
}

const uint8_t StopI2C_Trans(void) {
	TWCR = I2C_CMD_STOP;
	uint16_t counter = UINT16_MAX;
	while (TWCR & (1 << TWSTO) && --counter) {}
	return counter != 0 ? 1 : 0xE8;
}

const uint8_t Write(unsigned char data) {
	TWDR = data;
	TWCR = I2C_CMD_TRANSMIT;
	I2C_WAIT_TRANSFER();
	if (I2C_STATUS != I2C_MT_DATA_ACK) {
		DEBUG_PRINT("Failed to send data (got ");
		DEBUG_PRINT(I2C_STATUS);
		DEBUG_PRINT(")\n");
		StopI2C_Trans();
		return I2C_STATUS;
	}
	return true;
}

const uint8_t Read_from(unsigned char SLA, unsigned char MEMADDRESS) {
	uint8_t ret = StartI2C_Trans(SLA);
	if (ret & ~1) {
		return ret;
	}
	ret = Write(MEMADDRESS);
	if (ret & ~1) {
		return ret;
	}
	ret = __start(SLA, 1);
	if (ret & ~1) {
		return ret;
	}
	
	TWCR = I2C_CMD_RECEIVE_NACK;
	I2C_WAIT_TRANSFER();
	if (I2C_STATUS != I2C_MR_DATA_NACK) {
		DEBUG_PRINT("Failed to send NACK (got ");
		DEBUG_PRINT(I2C_STATUS, HEX);
		DEBUG_PRINT(")\n");
		return I2C_STATUS;
	}
	delay_ms(1);
	return StopI2C_Trans();
}

unsigned char Read_data() {
	return TWDR;
}