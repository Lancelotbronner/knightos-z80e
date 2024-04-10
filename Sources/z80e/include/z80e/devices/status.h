#pragma once

#include <z80e/types.h>

//TODO: Once the asic is passed to device functions, simplify status to just a configuration function `device_status(device_t)`

struct status {
	asic_t *asic;
};

void status_init(status_t status, asic_t *asic);

status_t status_new(asic_t *asic);
void status_delete(const status_t status);

//MARK: - Device Management

unsigned char status_read(const status_t status);
void status_write(const status_t status, unsigned char value);

struct z80_device status_device(const status_t status);
