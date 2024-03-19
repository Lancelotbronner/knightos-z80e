#include <z80e/ti/hardware/status.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <z80e/ti/asic.h>
#include <z80e/core/cpu.h>
#include <z80e/ti/hardware/interrupts.h>

void status_init(status_t status, asic_t *asic) {
	status->asic = asic;
}

//MARK: - Memory Management

status_t status_new(asic_t *asic) {
	status_t status = malloc(sizeof(status_t));
	status_init(status, asic);
	return status;
}

void status_delete(status_t status) {
	free(status);
}

//MARK: - Device Management

uint8_t status_read(status_t status) {
	uint8_t value = 0x00;
	if (status->asic->battery_remove_check) {
		if (status->asic->battery != BATTERIES_REMOVED)
			value |= 0x01;
	} else if (status->asic->battery == BATTERIES_GOOD)
		value |= 0x01;

	value |= status->asic->device != TI73 ? 0x02 : 0x00;

	if (status->asic->mmu.flash_unlocked)
		value |= 0x04;

	if (status->asic->device != TI73 && status->asic->device != TI83p)
		value |= 0x80;

	if (status->asic->device != TI73 && status->asic->device != TI83p)
		value |= 0x40; // link assist available

	if (status->asic->device == TI84p || status->asic->device == TI84pSE || status->asic->device == TI84pCSE)
		value |= 0x20;

	return value;
}

void status_write(status_t status, uint8_t value) {
	if (status->asic->device == TI83p || status->asic->device == TI73)
		return;
	write_acknowledged_interrupts(status->asic->interrupts, value);
}

struct z80_device status_device(status_t status) {
	return (struct z80_device){ status, (read_t)status_read, (write_t)status_write };
}
