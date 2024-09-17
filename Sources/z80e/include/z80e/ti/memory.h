#pragma once

#include <z80e/debugging/hooks.h>
#include <z80e/ti/ti.h>

#include <stdint.h>

typedef struct {
	uint16_t ram_pages;
	uint16_t flash_pages;
} ti_mmu_settings_t;

typedef struct {
	uint8_t page;
	int flash;
} ti_mmu_bank_state_t;

typedef struct {
	uint32_t address;
	uint32_t address_mask;
	uint8_t value;
	uint8_t value_mask;
} flash_write_t;

struct ti_mmu {
	ti_mmu_settings_t settings;
	ti_mmu_bank_state_t banks[4];
	uint8_t *ram;
	uint8_t *flash;
	int flash_unlocked;
	int flash_write_index;
	flash_write_t flash_writes[6];

	struct {
		struct hooks_memory memory_read;
		struct hooks_memory memory_write;
	} hook;
};

uint8_t ti_read_byte(void *memory, uint16_t address);
void ti_write_byte(void *memory, uint16_t address, uint8_t value);
void mmu_force_write(void *memory, uint16_t address, uint8_t value);

void ti_mmu_init(ti_mmu_t mmu, ti_device_type device_type);
void ti_mmu_deinit(ti_mmu_t mmu);
