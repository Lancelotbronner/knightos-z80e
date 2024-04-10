//
//  device.c
//  z80e
//
//  Created by Christophe Bronner on 2024-03-18.
//

#include <z80e/device.h>

unsigned char device_read(struct z80_device device) {
	if (device.read)
		return device.read(device.data);
	return 0x00;
}

void device_write(struct z80_device device, unsigned char value) {
	if (device.write)
		device.write(device.data, value);
}
