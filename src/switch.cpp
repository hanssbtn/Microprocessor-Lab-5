#include "switch.h"

void init_switch_pin(void) {
	DDRJ &= ~(1 << DDJ0);
	PORTJ |= (1 << PJ0);
	PCICR = (1 << PCIE1);
	PCMSK1 = (1 << PCINT9);
}