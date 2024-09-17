//
//  devices/link.h
//  z80e
//
//  Created by Christophe Bronner on 2024-09-12.
//

#pragma once

#include <z80e/types.h>

struct link_device {
	asic_t asic;

	union {
		unsigned char mask;
		struct {
			bool rx : 1;
			bool tx : 1;
			bool error : 1;
			unsigned char : 4;
			bool disabled : 1;
		};
	} interrupts;

	struct {
		unsigned char rx_buffer;
		unsigned char tx_buffer;
		union {
			struct {
				bool int_rx_ready : 1;
				bool int_tx_ready : 1;
				bool int_error : 1;
				bool rx_active : 1;
				bool rx_ready : 1;
				bool tx_ready : 1;
				bool error : 1;
				bool tx_active : 1;
			};
			unsigned char u8;
		} status;
	} assist;

	union {
		struct {
			unsigned char : 4;
			bool la_ready : 1;
		};
		struct {
			bool tip : 1;
			bool ring : 1;
		} them;
		struct {
			unsigned char : 2;
			bool tip : 1;
			bool ring : 1;
		} us;
	};

};

void init_link_ports(asic_t asic);
/**
 * Receives a byte via link assist.
 */
bool link_recv_byte(asic_t asic, unsigned char val);
/**
 * Reads a byte from the tx buffer and sets the link assist state to ready to send another byte.
 */
int link_read_tx_buffer(asic_t asic);

bool link_recv_ready(asic_t asic);
