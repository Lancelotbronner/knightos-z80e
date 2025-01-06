#include <z80e/peripherals/speed.h>

#include <z80e/ti/asic.h>

static unsigned char __speed_read(peripheral_t peripheral) {
	asic_t asic = peripheral->data;
	switch (asic->clock_rate) {
	case 6000000: return 0;
	case 15000000: return 1;
	}
	// TODO: set overclock
	return 0;
}

static void __speed_write(peripheral_t peripheral, unsigned char value) {
	asic_t asic = peripheral->data;
	if (value == 0) {
		asic_set_clock_rate(asic, 6000000);
	} else if (value == 1) {
		asic_set_clock_rate(asic, 15000000);
	}
	// TODO: set overclock
}

void port_speed(peripheral_t peripheral, asic_t asic) {
	peripheral->data = asic;
	peripheral->read = __speed_read;
	peripheral->write = __speed_write;
}
