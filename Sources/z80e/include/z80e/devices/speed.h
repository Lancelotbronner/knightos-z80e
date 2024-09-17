//
//  devices/speed.h
//  z80e
//
//  Created by Christophe Bronner on 2024-04-11.
//

#pragma once

#include <z80e/types.h>

//MARK: - Port Configuration

/// Configures a port to return the speed of the provided chip.
/// - Parameters:
///   - device: The device to configure.
///   - asic: The chip to monitor.
void port_speed(device_t device, asic_t asic);
