//
//  peripherals/status.h
//  z80e
//
//  Created by Christophe Bronner on 2024-04-10.
//

#pragma once

#include <z80e/types.h>

//MARK: - Port Configuration

/// Configures a port to return the status of the provided chip.
/// - Parameters:
///   - peripheral: The peripheral to configure.
///   - asic: The chip to monitor.
void port_status(peripheral_t peripheral, asic_t asic);
