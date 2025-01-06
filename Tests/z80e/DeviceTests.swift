import XCTest
import XCTz80e
import z80e

//TODO: Switch to Swift Testing

final class DeviceTests: XCTestCaseTI83p {

	//MARK: - Keyboard

	func test_keyboard() {
		var keyboard = keyboard_device()
		keyboard_init(&keyboard)

		var peripheral = peripheral()
		port_keyboard(&peripheral, &keyboard)

		keyboard_press(&keyboard, 0)
		peripheral_write(&peripheral, 0xFE)
		var value = peripheral_read(&peripheral)
		XCTAssertEqual(value, 0xFE)

		keyboard_press(&keyboard, 1);
		value = peripheral_read(&peripheral)
		XCTAssertEqual(value, 0xFC)

		keyboard_press(&keyboard, 0x14);
		value = peripheral_read(&peripheral)
		XCTAssertEqual(value, 0xFC)

		peripheral_write(&peripheral, 0xFC)
		value = peripheral_read(&peripheral)
		XCTAssertEqual(value, 0xEC)

		keyboard_release(&keyboard, 0x14);
		value = peripheral_read(&peripheral)
		XCTAssertEqual(value, 0xFC)
	}

	//MARK: - Memory Mapping

//	func test_memorymapping_others() {
//		asic_t *asic = asic_init(TI84p, NULL);
//		memory_mapping_state_t *state = asic->cpu->peripherals[0x04].peripheral;
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

	func test_memorymapping_83p() {
		asic.mapping.a = 0;
		asic.mapping.flashA = false;

		mapping_reload(&asic.mapping)
		write_byte(0x400, 0x12)

		XCTAssertEqual(asic.mmu.ram[0], 0x12)
		XCTAssertEqual(read_byte(0xC000), 0x12)

		asic.mapping.mode = true;
		asic.mapping.a = 1;
		asic.mapping.flashA = false;
		asic.mapping.b = 0;
		asic.mapping.flashB = false;
		mapping_reload(&asic.mapping);

		XCTAssertEqual(read_byte(0x4000), 0x12)

		asic.mmu.ram[0x4000] = 0x34;
		XCTAssertEqual(read_byte(0x8000), 0x34)
		XCTAssertEqual(read_byte(0xC000), 0x12)
	}

	//MARK: - Status

	func test_battery() {
		var peripheral = peripheral()
		port_status(&peripheral, _asic)

		asic.battery.state = BATTERIES_GOOD
		asic.battery.remove_check = false
		let value = peripheral_read(&peripheral) & 0x1
		XCTAssertEqual(value, 0x1)
	}

	func test_flash() {
		var peripheral = peripheral()
		port_status(&peripheral, _asic)

		asic.mmu.flash_unlocked = true
		var value = peripheral_read(&peripheral) & 0x4
		XCTAssertEqual(value, 0x4)

		asic.mmu.flash_unlocked = false;
		value = peripheral_read(&peripheral) & 0x4
		XCTAssertEqual(value, 0)
	}

	//MARK: - Link Port

	func test_link_port() {
		var peripheral = get(peripheral: 0x00)

		var value = peripheral_read(&peripheral);
		XCTAssertEqual(value, 0)

		peripheral_write(&peripheral, 0x01)
		value = peripheral_read(&peripheral)
		XCTAssertEqual(value, 0x11)
	}

	//MARK: - Link Assist RX

//	func test_link_assist_rx() {
//		asic_t *asic = asic_init(TI83pSE, NULL);
//		struct peripheral link_assist_rx_read = asic->cpu->peripherals[0x0A];
//		struct peripheral link_assist_status = asic->cpu->peripherals[0x09];
//		link_device_t state = link_assist_rx_read.peripheral;
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
//		struct peripheral link_assist_tx_read = asic->cpu->peripherals[0x0D];
//		struct peripheral link_assist_status = asic->cpu->peripherals[0x09];
//		link_device_t state = link_assist_tx_read.peripheral;
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
