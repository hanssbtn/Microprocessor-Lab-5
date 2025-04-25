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
    // Set MOSI, SCK, and SS as outputs
    SPI_DDR |= (SPI_MOSI_PIN_MASK | SPI_SCK_PIN_MASK | SPI_SS_PIN_MASK);
    
    // Set SS high (inactive) initially
    SPI_PORT |= SPI_SS_PIN_MASK;
    
    // Enable SPI, Master mode, set clock rate
    // SPE: SPI Enable
    // MSTR: Master Mode
    // SPR1: Clock Rate Select (divides by 16)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
}

void spi_write_byte(uint8_t data, uint8_t addr) {
    // Select slave device (active low)
    SPI_PORT &= ~SPI_SS_PIN_MASK;
    SPDR = addr;   // SPDR is the SPI Data Register - load data to send
    SPI_WAIT_TRANSFER(); // Wait until SPIF flag is set (transmission complete)
    SPDR = data;
    SPI_WAIT_TRANSFER(); // Wait until SPIF flag is set
    
    // Deselect slave device
    SPI_PORT |= SPI_SS_PIN_MASK;
}