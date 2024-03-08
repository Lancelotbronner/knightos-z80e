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

	//MARK: - Hooks

	override func setUp() {
		guard let device = asic_init(TI83p, nil) else {
			XCTFail("Failed to initialize device")
			return
		}
		_device = device
	}

	override func tearDown() {
		asic_free(_device)
	}

}

/* Replace RegularExpression for assertions

	.+?\b([a-zA-Z.]+?) != (.+?)\b.+

	XCTAssertEqual($1, $2)
 */
