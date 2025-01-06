//
//  peripherals/interrupts.c
//  z80e
//
//  Created by Christophe Bronner on 2024-09-12.
//

#include <z80e/peripherals/interrupts.h>

#include <z80e/ti/asic.h>
#include <z80e/peripheral.h>

//MARK: - Interrupt Mask

static unsigned char __mask_read(const peripheral_t peripheral) {
	asic_t asic = peripheral->data;
	return asic->interrupts.enabled.flags;
}

static void __mask_write(const peripheral_t peripheral, unsigned char value) {
	asic_t asic = peripheral->data;
	asic->interrupts.enabled.flags = value;
	asic->interrupts.interrupted.flags &= asic->interrupts.enabled.flags;
	__asic_interrupt_update(asic);
}

void port_interrupt_mask(peripheral_t peripheral, asic_t asic) {
	peripheral->data = asic;
	peripheral->read = __mask_read;
	peripheral->write = __mask_write;
}
