#include <z80e/devices/speed.h>

#include <z80e/ti/asic.h>

static unsigned char __speed_read(device_t device) {
	asic_t asic = device->data;
	switch (asic->clock_rate) {
	case 6000000: return 0;
	case 15000000: return 1;
	}
	// TODO: set overclock
	return 0;
}

static void __speed_write(device_t device, unsigned char value) {
	asic_t asic = device->data;
	if (value == 0) {
		asic_set_clock_rate(asic, 6000000);
	} else if (value == 1) {
		asic_set_clock_rate(asic, 15000000);
	}
	// TODO: set overclock
}

void port_speed(device_t device, asic_t asic) {
	device->data = asic;
	device->read = __speed_read;
	device->write = __speed_write;
}
