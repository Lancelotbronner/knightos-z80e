//
//  devices/t6a04.h
//  z80e
//
//  Created by Christophe Bronner on 2024-09-17.
//

#pragma once

#include <z80e/types.h>

//MARK: - Port Configuration

/// Configures a port to control the provided T6A04 LCD display.
/// - Parameters:
///   - device: The device to configure.
///   - asic: The LCD to interact with.
void port_t6a04_status(device_t device, lcd_t6a04_t lcd);

/// Configures a port to transfer data with the provided T6A04 LCD display.
/// - Parameters:
///   - device: The device to configure.
///   - asic: The LCD to interact with.
void port_t6a04_data(device_t device, lcd_t6a04_t lcd);
