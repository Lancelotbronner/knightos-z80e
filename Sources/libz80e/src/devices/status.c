#include <z80e/peripherals/status.h>

#include <z80e/ti/asic.h>

static uint8_t __status_read(peripheral_t peripheral) {
	asic_t asic = peripheral->data;
	uint8_t value = 0x00;

	if (asic->battery.remove_check) {
		if (asic->battery.state != BATTERIES_REMOVED)
			value |= 0x01;
	} else if (asic->battery.state == BATTERIES_GOOD)
		value |= 0x01;

	value |= asic->peripheral != TI73 ? 0x02 : 0x00;

	if (asic->mmu.flash_unlocked)
		value |= 0x04;

	if (asic->peripheral != TI73 && asic->peripheral != TI83p)
		value |= 0x80;

	if (asic->peripheral != TI73 && asic->peripheral != TI83p)
		value |= 0x40; // link assist available

	if (asic->peripheral == TI84p || asic->peripheral == TI84pSE || asic->peripheral == TI84pCSE)
		value |= 0x20;

	return value;
}

static void __status_write(peripheral_t peripheral, uint8_t value) {
	asic_t asic = peripheral->data;
	if (asic->peripheral == TI83p || asic->peripheral == TI73)
		return;
	asic->interrupts.interrupted.link_activity &= value;
	__asic_interrupt_update(asic);
}

void port_status(peripheral_t peripheral, asic_t asic) {
	peripheral->data = asic;
	peripheral->read = __status_read;
	peripheral->write = __status_write;
}
