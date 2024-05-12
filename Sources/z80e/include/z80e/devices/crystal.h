#pragma once

#include <z80e/types.h>

//MARK: - Device Configuration

/// Configures a device to be the frequency of a crystal timer.
/// - Parameters:
///   - device: The device to configure.
void device_crystal_frequency(device_t device);

/// Configures a device to be the loop of a crystal timer.
/// - Parameters:
///   - device: The device to configure.
void device_crystal_loop(device_t device);

/// Configures a device to be the count of a crystal timer.
/// - Parameters:
///   - device: The device to configure.
void device_crystal_count(device_t device);
