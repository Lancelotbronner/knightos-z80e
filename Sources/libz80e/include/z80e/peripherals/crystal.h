//
//  peripherals/crystal.h
//  z80e
//
//  Created by Christophe Bronner on 2024-05-12.
//

#pragma once

#include <z80e/types.h>

//MARK: - Port Configuration

/// Configures a port to be the frequency of a crystal timer.
/// - Parameters:
///   - peripheral: The peripheral to configure.
void port_crystal_frequency(peripheral_t peripheral);

/// Configures a port to be the loop of a crystal timer.
/// - Parameters:
///   - peripheral: The peripheral to configure.
void port_crystal_loop(peripheral_t peripheral);

/// Configures a port to be the count of a crystal timer.
/// - Parameters:
///   - peripheral: The peripheral to configure.
void port_crystal_count(peripheral_t peripheral);
