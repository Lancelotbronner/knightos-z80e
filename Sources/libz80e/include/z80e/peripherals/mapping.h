//
//  peripherals/mapping.h
//  z80e
//
//  Created by Christophe Bronner on 2024-04-10.
//

#pragma once

#include <z80e/types.h>

struct mapping_device {
	asic_t asic;

	/// The page of bank A
	unsigned char a;
	/// The page of bank B
	unsigned char b;

	struct {
		bool mode : 1;
		/// Whether bank A points to flash memory
		bool flashA : 1;
		/// Whether bank B points to flash memory
		bool flashB : 1;
		/// The current RAM bank page
		unsigned char page : 3;
	};
};

void mapping_init(mapping_device_t mapping, asic_t asic);

void mapping_reload(mapping_device_t mapping);

//MARK: - Port Configuration

void port_mapping_status(peripheral_t peripheral, mapping_device_t mapping);
void port_mapping_paging(peripheral_t peripheral, mapping_device_t mapping);
void port_mapping_bankA(peripheral_t peripheral, mapping_device_t mapping);
void port_mapping_bankB(peripheral_t peripheral, mapping_device_t mapping);
