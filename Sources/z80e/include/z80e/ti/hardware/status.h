#pragma once

#include <z80e/ti/types.h>

z80iodevice_t init_status(asic_t *asic);
void free_status(z80iodevice_t status);
