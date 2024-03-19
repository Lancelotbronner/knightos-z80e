//
//  device.c
//  
//
//  Created by Christophe Bronner on 2024-03-18.
//

#include <z80e/core/device.h>

unsigned char device_read(struct z80_device device) {
	return device.read(device.data);
}

void device_write(struct z80_device device, unsigned char value) {
	device.write(device.data, value);
}
