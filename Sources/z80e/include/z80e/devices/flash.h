//
//  devices/flash.h
//  z80e
//
//  Created by Christophe Bronner on 2024-04-10.
//

#pragma once

#include <z80e/types.h>

//MARK: - Port Configuration

/// Configures a port to control the flash memory of the specified chip.
/// - Parameters:
///   - device: The device to configure.
///   - asic: The chip to control.
///   - size: The size of the flash chip.
void port_flash_control(device_t device, asic_t asic);

/// Configures a port to control the flash exclusion of the specified chip.
/// - Parameters:
///   - device: The device to configure.
///   - asic: The chip to control.
void port_flash_exclusion(device_t device, asic_t asic);

/// Configures a port to control the flash size of the specified chip.
/// - Parameters:
///   - device: The device to configure.
///   - size: The size of the flash chip.
void port_flash_size(device_t device);
