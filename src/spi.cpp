#include "spi.h"

#define SPI_DDR DDRB
#define SPI_PORT PORTB
/** @brief digital pin 53 (D53)*/
#define SPI_SS_PIN_NUMBER PB0
/** @brief digital pin 52 (D52) */
#define SPI_SCK_PIN_NUMBER PB1
/** @brief digital pin 51 (D51) */
#define SPI_MOSI_PIN_NUMBER PB2
/** @brief digital pin 50 (D50)*/
#define SPI_MISO_PIN_NUMBER PB3

/** @brief Slave Select */
#define SPI_SS_PIN_MASK (1 << SPI_SS_PIN_NUMBER)
#define SPI_SCK_PIN_MASK (1 << SPI_SCK_PIN_NUMBER)
#define SPI_MOSI_PIN_MASK (1 << SPI_MOSI_PIN_NUMBER)
#define SPI_MISO_PIN_MASK (1 << SPI_MISO_PIN_NUMBER)

#define SPI_WAIT_TRANSFER() while (!(SPSR & (1 << SPIF)))

void init_spi(void) {
	SPI_DDR = (SPI_MOSI_PIN_MASK | SPI_SCK_PIN_MASK | SPI_SS_PIN_MASK);
	SPI_PORT |= SPI_SS_PIN_MASK;
	// Enable SPI 
	SPCR = (1 << MSTR) | (1 << SPR0);
	SPCR = (1 << SPE);
	// Clear SPSR
	volatile uint8_t tmp __attribute__((unused)) = SPSR;
}

void spi_write_byte(uint8_t data, uint8_t addr) {
	SPI_PORT &= ~SPI_SS_PIN_MASK;
	SPDR = addr;
	SPI_WAIT_TRANSFER();
	volatile uint8_t status __attribute__((unused)) = SPSR;
	SPDR = data;
	SPI_WAIT_TRANSFER();
	status = SPSR;
	SPI_PORT |= SPI_SS_PIN_MASK;
}