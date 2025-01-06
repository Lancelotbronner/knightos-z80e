//
//  z80e+.swift
//  z80e
//
//  Created by Christophe Bronner on 2025-01-06.
//

import z80e

public final class Asic {
	public var rawValue: asic

	public init(manage type: ti_device_type) {
		rawValue = asic()
		asic_init(&rawValue, type)
	}

	deinit {
		asic_deinit(&rawValue)
	}
}

extension ti_device_type: @retroactive CustomStringConvertible {
	public static let z80: [ti_device_type] = [
		TI83p, TI83pSE, TI84p, TI84pSE, TI84pCSE,
	]

	public var description: String {
		switch self {
		case TI83p: "TI-83+"
		case TI83pSE: "TI-83+ SE"
		case TI84p: "TI-84+"
		case TI84pSE: "TI-84+ SE"
		case TI84pCSE: "TI-84+ CSE"
		case TI84pCE: "TI-84+ CE"
		default: "Unknown"
		}
	}
}

public extension asic {

	//MARK: - Test Peripherals

	mutating func installTestPeripheral(using value: UnsafeMutablePointer<UInt8>) {
		cpu_device(&cpu, 0x12).pointee = .init(
			data: value,
			read: test_read,
			write: test_write)
	}

	//MARK: - Port Management

	func flash(_ data: [UInt8]) {
		for i in data.indices {
			mmu.flash[i] = data[i];
		}
	}

	mutating func read_byte(_ address: UInt16) -> UInt8 {
		cpu_read_byte(&cpu, address)
	}

	mutating func write_byte(_ address: UInt16, _ value: UInt8) {
		cpu_write_byte(&cpu, address, value)
	}

	mutating func read_word(_ address: UInt16) -> UInt16 {
		cpu_read_word(&cpu, address)
	}

	mutating func write_word(_ address: UInt16, _ value: UInt16) {
		cpu_write_word(&cpu, address, value)
	}

	mutating func get(device position: Int) -> device {
		cpu_device(&cpu, UInt8(truncatingIfNeeded: position)).pointee
	}

}

private func test_read(_ device: UnsafeMutablePointer<device>) -> UInt8 {
	guard let data = device.pointee.data else { return 0 }
	return data.assumingMemoryBound(to: UInt8.self).pointee
}

private func test_write(_ device: UnsafeMutablePointer<device>, _ value: UInt8) {
	guard let data = device.pointee.data else { return }
	data.assumingMemoryBound(to: UInt8.self).pointee = value
}
