//
//  peripherals/t6a04.h
//  z80e
//
//  Created by Christophe Bronner on 2024-09-17.
//

#pragma once

#include <z80e/types.h>

//MARK: - Port Configuration

/// Configures a port to control the provided T6A04 LCD display.
/// - Parameters:
///   - peripheral: The peripheral to configure.
///   - asic: The LCD to interact with.
void port_t6a04_status(peripheral_t peripheral, lcd_t6a04_t lcd);

/// Configures a port to transfer data with the provided T6A04 LCD display.
/// - Parameters:
///   - peripheral: The peripheral to configure.
///   - asic: The LCD to interact with.
void port_t6a04_data(peripheral_t peripheral, lcd_t6a04_t lcd);
