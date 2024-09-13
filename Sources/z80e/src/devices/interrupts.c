//
//  devices/interrupts.c
//  z80e
//
//  Created by Christophe Bronner on 2024-09-12.
//

#include <z80e/devices/interrupts.h>

#include <z80e/ti/asic.h>
#include <z80e/device.h>

//MARK: - Interrupt Mask

static unsigned char __mask_read(const device_t device) {
	asic_t asic = device->data;
	return asic->interrupts.enabled.flags;
}

static void __mask_write(const device_t device, unsigned char value) {
	asic_t asic = device->data;
	asic->interrupts.enabled.flags = value;
	asic->interrupts.interrupted.flags &= asic->interrupts.enabled.flags;
	__asic_interrupt_update(asic);
}

void device_interrupt_mask(device_t device, asic_t asic) {
	device->data = asic;
	device->read = __mask_read;
	device->write = __mask_write;
}
