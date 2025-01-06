//
//  peripheral.h
//  z80e
//
//  Created by Christophe Bronner on 2024-03-18.
//

#pragma once

#include <z80e/types.h>

struct peripheral {
	void * _Nullable data;
	read_t read;
	write_t write;
};

unsigned char peripheral_read(const peripheral_t _Nonnull peripheral);

void peripheral_write(const peripheral_t _Nonnull peripheral, unsigned char value);

//MARK: - Port Configuration

/// Configures a peripheral to mirror to another peripheral.
/// - Parameters:
///   - peripheral: The peripheral to configure.
///   - other: The peripheral to mirror.
void port_mirror(peripheral_t _Nonnull peripheral, const peripheral_t _Nonnull other);

/// Configures a peripheral to be considered unimplemented.
/// - Parameters:
///  - peripheral: The peripheral to configure.
///  - port: The port of the peripheral for logging purposes.
void port_unimplemented(peripheral_t _Nonnull peripheral, unsigned char port);

/// Configures a peripheral to ignore reads and/or writes.
/// - Parameters:
///   - peripheral: The peripheral to configure.
///   - read: Whether to ignore reads.
///   - write: Whether to ignore writes.
void port_null(peripheral_t _Nonnull peripheral, bool read, bool write);
