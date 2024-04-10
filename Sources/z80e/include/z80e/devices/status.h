#pragma once

#include <z80e/types.h>

typedef struct status_device *status_device_t;

struct status_device {
	asic_t *asic;
};

/// Configures a device to return the status of the provided chip.
/// - Parameters:
///   - device: The device to configure.
///   - asic: The chip to monitor.
void device_status(device_t device, asic_t *asic);
