#pragma once

#include <z80e/types.h>

struct status {
	asic_t *asic;
};

void status_init(status_t status, asic_t *asic);

status_t status_new(asic_t *asic);
void status_delete(const status_t status);

//MARK: - Device Management

unsigned char status_read(status_t status);
void status_write(status_t status, unsigned char value);

struct z80_device status_device(const status_t status);
