//
//  peripherals/interrupts.h
//  z80e
//
//  Created by Christophe Bronner on 2024-09-12.
//

#pragma once

#include <z80e/types.h>

//MARK: - Port Configuration

/// Configures a port to control the interrupt mask.
/// - Parameters:
///   - peripheral: The peripheral to configure.
///   - keyboard: The chip to control.
void port_interrupt_mask(peripheral_t peripheral, asic_t asic);
