#pragma once

#include <z80e/types.h>

struct mapping_device {
	asic_t asic;
	struct {
		unsigned char mode : 1;
		/// Whether bank A points to flash memory
		unsigned char flashA : 1;
		/// Whether bank B points to flash memory
		unsigned char flashB : 1;
		/// The current RAM bank page
		unsigned char page : 3;
	};
	/// The page of bank A
	unsigned char a;
	/// The page of bank B
	unsigned char b;
};

void mapping_init(mapping_device_t mapping, asic_t asic);

void mapping_reload(mapping_device_t mapping);

//MARK: - Device Configuration

void device_mapping_status(device_t device, mapping_device_t mapping);
void device_mapping_paging(device_t device, mapping_device_t mapping);
void device_mapping_bankA(device_t device, mapping_device_t mapping);
void device_mapping_bankB(device_t device, mapping_device_t mapping);
