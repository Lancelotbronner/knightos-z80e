#pragma once

#include <z80e/types.h>

//MARK: - Device Configuration

/// Configures a device to return the speed of the provided chip.
/// - Parameters:
///   - device: The device to configure.
///   - asic: The chip to monitor.
void device_speed(device_t device, asic_t asic);
