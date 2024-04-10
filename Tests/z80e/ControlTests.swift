import XCTest
import XCTz80e
import z80e

final class ControlTests: XCTestCaseTI83p {

	//MARK: - RST

	func RST0() {
		let test: [UInt8] = [0xC7]
		flash(test)
		let cycles = cpu_execute(&device.cpu, 11)
		XCTAssertEqual(device.cpu.registers.PC, 0xC7)
		XCTAssertEqual(cycles, 0)
	}

	func RST1() {
		let test: [UInt8] = [0xCF]
		flash(test)
		let cycles = cpu_execute(&device.cpu, 11)
		XCTAssertEqual(device.cpu.registers.PC, 0x08)
		XCTAssertEqual(cycles, 0)
	}

	func RST2() {
		let test: [UInt8] = [0xD7]
		flash(test)
		let cycles = cpu_execute(&device.cpu, 11)
		XCTAssertEqual(device.cpu.registers.PC, 0x10)
		XCTAssertEqual(cycles, 0)
	}

	func RST3() {
		let test: [UInt8] = [0xDF]
		flash(test)
		let cycles = cpu_execute(&device.cpu, 11)
		XCTAssertEqual(device.cpu.registers.PC, 0x18)
		XCTAssertEqual(cycles, 0)
	}

	func RST4() {
		let test: [UInt8] = [0xE7]
		flash(test)
		let cycles = cpu_execute(&device.cpu, 11)
		XCTAssertEqual(device.cpu.registers.PC, 0x20)
		XCTAssertEqual(cycles, 0)
	}

	func RST5() {
		let test: [UInt8] = [0xEF]
		flash(test)
		let cycles = cpu_execute(&device.cpu, 11)
		XCTAssertEqual(device.cpu.registers.PC, 0x28)
		XCTAssertEqual(cycles, 0)
	}

	func RST6() {
		let test: [UInt8] = [0xF7]
		flash(test)
		let cycles = cpu_execute(&device.cpu, 11)
		XCTAssertEqual(device.cpu.registers.PC, 0x30)
		XCTAssertEqual(cycles, 0)
	}

	func RST7() {
		let test: [UInt8] = [0xFF]
		flash(test)
		let cycles = cpu_execute(&device.cpu, 11)
		XCTAssertEqual(device.cpu.registers.PC, 0x38)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - DJNZ

	func test_DJNZ() {
		let test: [UInt8] = [0x10, 0xFE] // DJNZ $
		flash(test)
		device.cpu.registers.B = 10;
		var cycles = cpu_execute(&device.cpu, 13)
		XCTAssertEqual(device.cpu.registers.PC, 0)
		XCTAssertEqual(device.cpu.registers.B, 9)
		XCTAssertEqual(cycles, 0)
		while (device.cpu.registers.B != 0) {
			cycles = cpu_execute(&device.cpu, 1)
		}
		XCTAssertEqual(device.cpu.registers.PC, 2)
	}

	//MARK: - JR

	func test_JR() {
		let test: [UInt8] = [0x18, 0x0E] // JR 0x10
		flash(test)
		let cycles = cpu_execute(&device.cpu, 12)
		XCTAssertEqual(device.cpu.registers.PC, 0x10)
		XCTAssertEqual(cycles, 0)
	}

	func test_JR_cc() {
		let test: [UInt8] = [0x28, 0x0E] // JR Z, 0x10
		device.cpu.registers.flags.Z = 0;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 7)
		XCTAssertEqual(device.cpu.registers.PC, 2)
		XCTAssertEqual(cycles, 0)
	}

	func test_JR_cc_nz() {
		let test_nz: [UInt8] = [0x20, 0x0E] // JR NZ, 0x10
		device.cpu.registers.flags.Z = 0;
		flash(test_nz)
		let cycles = cpu_execute(&device.cpu, 12)
		XCTAssertEqual(device.cpu.registers.PC, 0x10)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - RET

	func test_RET() {
		let test: [UInt8] = [0xC9] // RET
		device.cpu.registers.SP = 0x3FFE;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 10)
		XCTAssertEqual(device.cpu.registers.PC, 0xFFFF)
		XCTAssertEqual(cycles, 0)
	}

	func test_RET_cc() {
		let test: [UInt8] = [0xC8] // RET Z
		device.cpu.registers.flags.Z = 0;
		device.cpu.registers.SP = 0x3FFE;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 5)
		XCTAssertEqual(device.cpu.registers.PC, 1)
		XCTAssertEqual(cycles, 0)
	}

	func test_RET_cc_nz() {
		let test_nz: [UInt8] = [0xC0] // RET NZ
		device.cpu.registers.flags.Z = 0;
		device.cpu.registers.SP = 0x3FFE;
		flash(test_nz)
		let cycles = cpu_execute(&device.cpu, 11)
		XCTAssertEqual(device.cpu.registers.PC, 0xFFFF)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - JP

	/// `JP Z, 0x1234`
	let JP_Z_IMM: [UInt8] = [0xCA, 0x34, 0x12]

	func test_JP_HL() {
		let test: [UInt8] = [0xE9] // JP (HL)
		device.cpu.registers.HL = 0x1234;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.PC, 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	func test_JP_cc_nn() {
		flash(JP_Z_IMM)
		let cycles = cpu_execute(&device.cpu, 10)
		XCTAssertEqual(device.cpu.registers.PC, 3)
		XCTAssertEqual(cycles, 0)
	}

	func test_JP_cc_nn_z() {
		flash(JP_Z_IMM)
		device.cpu.registers.flags.Z = 1;
		let cycles = cpu_execute(&device.cpu, 10)
		XCTAssertEqual(device.cpu.registers.PC, 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	func test_JP_nn() {
		let test: [UInt8] = [0xC3, 0x34, 0x12] // JP 0x1234
		flash(test)
		let cycles = cpu_execute(&device.cpu, 10)
		XCTAssertEqual(device.cpu.registers.PC, 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - LD

	func test_LD_SP_HL() {
		let test: [UInt8] = [0xF9] // LD SP, HL
		device.cpu.registers.HL = 0x1234;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 6)
		XCTAssertEqual(device.cpu.registers.SP, 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - CALL

	/// `CALL z, 0x1234`
	let CALL: [UInt8] = [0xCC, 0x34, 0x12]

	func test_CALL_cc_nn() {
		flash(CALL)
		let cycles = cpu_execute(&device.cpu, 10)
		XCTAssertEqual(device.cpu.registers.PC, 3)
		XCTAssertEqual(cycles, 0)
	}

	func test_CALL_cc_nn_z() {
		device.cpu.registers.flags.Z = 1;
		flash(CALL)
		let cycles = cpu_execute(&device.cpu, 17)
		XCTAssertEqual(device.cpu.registers.PC, 0x1234)
		XCTAssertEqual(device.cpu.registers.SP, 0xFFFE)
		XCTAssertEqual(cycles, 0)
	}

	func test_CALL_nn() {
		let test: [UInt8] = [0xCD, 0x34, 0x12] // CALL 0x1234
		flash(test)
		let cycles = cpu_execute(&device.cpu, 17)
		XCTAssertEqual(device.cpu.registers.PC, 0x1234)
		XCTAssertEqual(device.cpu.registers.SP, 0xFFFE)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - IM

	func test_IM_set() {
		let test: [UInt8] = [0xED, 0x5E] // IM 2
		flash(test)
		let cycles = cpu_execute(&device.cpu, 8)
		XCTAssertEqual(device.cpu.int_mode, 2)
		XCTAssertEqual(device.cpu.registers.PC, 2)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - EI

	func test_EI() {
		let test: [UInt8] = [0xFB] // EI
		flash(test)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.IFF1, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - DI

	func test_DI() {
		let test2: [UInt8] = [0xF3] // DI
		device.cpu.IFF1 = 1;
		flash(test2)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.IFF1, 0)
		XCTAssertEqual(cycles, 0)
	}

}
