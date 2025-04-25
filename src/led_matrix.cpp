#include "led_matrix.h"

void init_led_matrix(void) {
	spi_write_byte(0x00, 0x0F); // turn off display
	spi_write_byte(0x01, 0x0C); // disable shutdown mode
	spi_write_byte(0x07, 0x0B); // scan 8 digits
	spi_write_byte(0x00, 0x09); // no decode
	spi_write_byte(0x01, 0x0A); // set intensity
	for (int i = 1; i <= 8; ++i) {
		spi_write_byte(0x00, i);
	}
}