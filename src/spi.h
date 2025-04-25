#ifndef SPI_H__
#define SPI_H__

#include "common.h"
#include "timer.h"

void init_spi(void);
void spi_write_byte(uint8_t data, uint8_t addr);

#endif // SPI_H__