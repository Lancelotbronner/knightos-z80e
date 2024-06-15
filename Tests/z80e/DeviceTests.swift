import XCTest
import XCTz80e
import z80e

final class DeviceTests: XCTestCaseTI83p {

	//MARK: - Keyboard

	func test_keyboard() {
		var keyboard = keyboard_device()
		keyboard_init(&keyboard)

		var device = device()
		device_keyboard(&device, &keyboard)

		keyboard_press(&keyboard, 0)
		device_write(&device, 0xFE)
		var value = device_read(&device)
		XCTAssertEqual(value, 0xFE)

		keyboard_press(&keyboard, 1);
		value = device_read(&device)
		XCTAssertEqual(value, 0xFC)

		keyboard_press(&keyboard, 0x14);
		value = device_read(&device)
		XCTAssertEqual(value, 0xFC)

		device_write(&device, 0xFC)
		value = device_read(&device)
		XCTAssertEqual(value, 0xEC)

		keyboard_release(&keyboard, 0x14);
		value = device_read(&device)
		XCTAssertEqual(value, 0xFC)
	}

	//MARK: - Memory Mapping

//	func test_memorymapping_others() {
//		asic_t *asic = asic_init(TI84p, NULL);
//		memory_mapping_state_t *state = asic->cpu->devices[0x04].device;
//
//
//		state->ram_bank_page = 1;
//		state->bank_a_page = 0;
//		state->bank_a_flash = 0;
//
//		reload_mapping(state);
//		ti_write_byte(asic->mmu, 0x4000, 0x12);
//
//		if (asic->mmu->ram[0] != 0x12) {
//			asic_free(asic);
//			return 1;
//		}
//
//		asic->mmu->ram[0x4000] = 0x34;
//		if (ti_read_byte(asic->mmu, 0xC000) != 0x34) {
//			asic_free(asic);
//			return 2;
//		}
//
//		state->map_mode = 1;
//		state->bank_a_page = 0;
//		state->bank_a_flash = 0;
//		state->bank_b_page = 0;
//		state->bank_b_flash = 0;
//		reload_mapping(state);
//
//		if (ti_read_byte(asic->mmu, 0x4000) != 0x12) {
//			asic_free(asic);
//			return 3;
//		}
//
//		if (ti_read_byte(asic->mmu, 0x8000) != 0x34) {
//			asic_free(asic);
//			return 4;
//		}
//
//		if (ti_read_byte(asic->mmu, 0xC000) != 0x12) {
//			asic_free(asic);
//			return 5;
//		}
//		return 0;
//	}

	//MARK: - Memory Mapping (83p)

//	func test_memorymapping_83p() {
//		asic_t *asic = asic_init(TI83p, NULL);
//		memory_mapping_state_t *state = asic->cpu->devices[0x04].device;
//
//		state->bank_a_page = 0;
//		state->bank_a_flash = 0;
//
//		reload_mapping(state);
//		ti_write_byte(asic->mmu, 0x4000, 0x12);
//
//		if (asic->mmu->ram[0] != 0x12) {
//			asic_free(asic);
//			return 1;
//		}
//
//		if (ti_read_byte(asic->mmu, 0xC000) != 0x12) {
//			asic_free(asic);
//			return 2;
//		}
//
//		state->map_mode = 1;
//		state->bank_a_page = 1;
//		state->bank_a_flash = 0;
//		state->bank_b_page = 0;
//		state->bank_b_flash = 0;
//		reload_mapping(state);
//
//		if (ti_read_byte(asic->mmu, 0x4000) != 0x12) {
//			asic_free(asic);
//			return 3;
//		}
//
//		asic->mmu->ram[0x4000] = 0x34;
//		if (ti_read_byte(asic->mmu, 0x8000) != 0x34) {
//			asic_free(asic);
//			return 4;
//		}
//
//		if (ti_read_byte(asic->mmu, 0xC000) != 0x12) {
//			asic_free(asic);
//			return 5;
//		}
//		return 0;
//	}

	//MARK: - Status

	func test_battery() {
		var device = device()
		device_status(&device, _asic)

		asic.battery = BATTERIES_GOOD
		asic.battery_remove_check = 0
		let value = device_read(&device) & 0x1
		XCTAssertEqual(value, 0x1)
	}

	func test_flash() {
		var device = device()
		device_status(&device, _asic)

		asic.mmu.flash_unlocked = 1
		var value = device_read(&device) & 0x4
		XCTAssertEqual(value, 0x4)

		asic.mmu.flash_unlocked = 0;
		value = device_read(&device) & 0x4
		XCTAssertEqual(value, 0)
	}

	//MARK: - Link Port

	func test_link_port() {
		var device = get(device: 0x00)

		var value = device_read(&device);
		XCTAssertEqual(value, 0)

		device_write(&device, 0x01)
		value = device_read(&device)
		XCTAssertEqual(value, 0x11)
	}

	//MARK: - Link Assist RX

//	func test_link_assist_rx() {
//		asic_t *asic = asic_init(TI83pSE, NULL);
//		struct device link_assist_rx_read = asic->cpu->devices[0x0A];
//		struct device link_assist_status = asic->cpu->devices[0x09];
//		link_state_t *state = link_assist_rx_read.device;
//
//		if (!link_recv_byte(asic, 0xBE)) {
//			asic_free(asic);
//			return 1;
//		}
//		if (link_recv_byte(asic, 0xEF)) {
//			asic_free(asic);
//			return 2;
//		}
//
//		uint8_t status = link_assist_status.read(state);
//		if (status != state->assist.status.u8 ||
//				!state->assist.status.rx_ready ||
//				!state->assist.status.int_rx_ready) {
//			return 3;
//		}
//
//		uint8_t val = link_assist_rx_read.read(state);
//		if (val != 0xBE) {
//			return 4;
//		}
//
//		status = link_assist_status.read(state);
//		if (status != state->assist.status.u8 ||
//				state->assist.status.rx_ready ||
//				state->assist.status.int_rx_ready) {
//			return 5;
//		}
//		return 0;
//	}

	//MARK: - Link Assist TX

//	func test_link_assist_tx() {
//		asic_t *asic = asic_init(TI83pSE, NULL);
//		struct device link_assist_tx_read = asic->cpu->devices[0x0D];
//		struct device link_assist_status = asic->cpu->devices[0x09];
//		link_state_t *state = link_assist_tx_read.device;
//
//		if (link_read_tx_buffer(asic) != EOF) {
//			asic_free(asic);
//			return 1;
//		}
//
//		uint8_t status = link_assist_status.read(state);
//		if (status != state->assist.status.u8 ||
//				!state->assist.status.tx_ready ||
//				!state->assist.status.int_tx_ready) {
//			return 2;
//		}
//
//		link_assist_tx_read.write(state, 0xDE);
//
//		status = link_assist_status.read(state);
//		if (status != state->assist.status.u8 ||
//				state->assist.status.tx_ready ||
//				state->assist.status.int_tx_ready) {
//			return 3;
//		}
//
//		if (link_read_tx_buffer(asic) != 0xDE) {
//			return 4;
//		}
//
//		status = link_assist_status.read(state);
//		if (status != state->assist.status.u8 ||
//				!state->assist.status.tx_ready ||
//				!state->assist.status.int_tx_ready) {
//			return 5;
//		}
//		return 0;
//	}

}
