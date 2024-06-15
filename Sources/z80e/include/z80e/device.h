//
//  Header.h
//  
//
//  Created by Christophe Bronner on 2024-03-18.
//

#pragma once

#include <z80e/types.h>

#include <stdbool.h>

struct device {
	void * _Nullable data;
	read_t read;
	write_t write;
};

unsigned char device_read(const device_t _Nonnull device);

void device_write(const device_t _Nonnull device, unsigned char value);

//MARK: - Device Configuration

/// Configures a device to mirror to another device.
/// - Parameters:
///   - device: The device to configure.
///   - other: The device to mirror.
void device_mirror(device_t _Nonnull device, const device_t _Nonnull other);

/// Configures a device to be considered unimplemented.
/// - Parameters:
///  - device: The device to configure.
///  - port: The port of the device for logging purposes.
void device_unimplemented(device_t _Nonnull device, unsigned char port);

/// Configures a device to ignore reads and/or writes.
/// - Parameters:
///   - device: The device to configure.
///   - read: Whether to ignore reads.
///   - write: Whether to ignore writes.
void device_null(device_t _Nonnull device, bool read, bool write);
