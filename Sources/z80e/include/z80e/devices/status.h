#pragma once

#include <z80e/types.h>

//MARK: - Device Configuration

/// Configures a device to return the status of the provided chip.
/// - Parameters:
///   - device: The device to configure.
///   - asic: The chip to monitor.
void device_status(device_t device, asic_t asic);
