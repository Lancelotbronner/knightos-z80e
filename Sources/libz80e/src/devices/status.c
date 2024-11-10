#include <z80e/devices/status.h>

#include <z80e/ti/asic.h>

static uint8_t __status_read(device_t device) {
	asic_t asic = device->data;
	uint8_t value = 0x00;

	if (asic->battery.remove_check) {
		if (asic->battery.state != BATTERIES_REMOVED)
			value |= 0x01;
	} else if (asic->battery.state == BATTERIES_GOOD)
		value |= 0x01;

	value |= asic->device != TI73 ? 0x02 : 0x00;

	if (asic->mmu.flash_unlocked)
		value |= 0x04;

	if (asic->device != TI73 && asic->device != TI83p)
		value |= 0x80;

	if (asic->device != TI73 && asic->device != TI83p)
		value |= 0x40; // link assist available

	if (asic->device == TI84p || asic->device == TI84pSE || asic->device == TI84pCSE)
		value |= 0x20;

	return value;
}

static void __status_write(device_t device, uint8_t value) {
	asic_t asic = device->data;
	if (asic->device == TI83p || asic->device == TI73)
		return;
	asic->interrupts.interrupted.link_activity &= value;
	__asic_interrupt_update(asic);
}

void port_status(device_t device, asic_t asic) {
	device->data = asic;
	device->read = __status_read;
	device->write = __status_write;
}
