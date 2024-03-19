//
//  Header.h
//  
//
//  Created by Christophe Bronner on 2024-03-18.
//

#pragma once

#include <z80e/types.h>

struct z80_device {
	void * _Nullable data;
	read_t read;
	write_t write;
};

unsigned char device_read(const struct z80_device device);
void device_write(const struct z80_device device, const unsigned char value);
