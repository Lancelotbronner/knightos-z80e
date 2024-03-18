//
//  Utilities.swift
//  Tests/z80e
//
//  Created by Christophe Bronner on 2024-03-08.
//

import XCTest
import z80e

class z80eTestCase: XCTestCase {

	//MARK: - Device Management

	var _device: UnsafeMutablePointer<asic_t>!

	var device: asic_t {
		_read { yield _device.pointee }
		_modify { yield &_device.pointee }
	}

	func flash(_ data: [UInt8]) {
		for i in data.indices {
			device.mmu.flash[i] = data[i];
		}
	}

	func read_byte(_ address: UInt16) -> UInt8 {
		cpu_read_byte(&device.cpu, address)
	}

	func write_byte(_ address: UInt16, _ value: UInt8) {
		cpu_write_byte(&device.cpu, address, value)
	}

	func read_word(_ address: UInt16) -> UInt16 {
		cpu_read_word(&device.cpu, address)
	}

	func write_word(_ address: UInt16, _ value: UInt16) {
		cpu_write_word(&device.cpu, address, value)
	}

	//MARK: - Test Device

	var value: UInt8 = 0

	//MARK: - Hooks

	override func setUp() {
		guard let device = asic_init(TI83p, nil) else {
			XCTFail("Failed to initialize device")
			return
		}
		_device = device

		// Configure the default test device
		cpu_device(&self.device.cpu, 0x12)[0] = z80iodevice(
			device: Unmanaged<z80eTestCase>
				.passUnretained(self)
				.toOpaque(),
			read_in: test_read,
			write_out: test_write)
	}

	override func tearDown() {
		asic_free(_device)
	}

}

private func test_read(_ device: UnsafeMutableRawPointer!) -> UInt8 {
	let test = Unmanaged<z80eTestCase>
		.fromOpaque(device)
		.takeUnretainedValue()
	return test.value
}

private func test_write(_ device: UnsafeMutableRawPointer!, _ value: UInt8) {
	let test = Unmanaged<z80eTestCase>
		.fromOpaque(device)
		.takeUnretainedValue()
	test.value = value
}
