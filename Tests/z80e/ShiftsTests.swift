import XCTest
import XCTz80e
import z80e

final class ShiftsTests: XCTestCaseTI83p {

	//MARK: - RLCA

	func test_RLCA() {
		let test: [UInt8] = [0x07] // RLCA
		asic.cpu.registers.A = 0x80;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 1)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - RRCA

	func test_RRCA() {
		let test: [UInt8] = [0x0F] // RRCA
		asic.cpu.registers.A = 1;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x80)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - RLA

	func test_RLA() {
		let test: [UInt8] = [0x17] // RLA
		asic.cpu.registers.A = 0x80;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - RRA

	func test_RRA() {
		let test: [UInt8] = [0x1F] // RRA
		asic.cpu.registers.A = 1;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - RLC

	func test_RLC() {
		let test: [UInt8] = [0xCB, 0x00] // RLC B
		asic.cpu.registers.B = 0x80;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.B, 1)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - RRC

	func test_RRC() {
		let test: [UInt8] = [0xCB, 0x08] // RRC B
		asic.cpu.registers.B = 1;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.B, 0x80)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - RL

	func test_RL() {
		let test: [UInt8] = [0xCB, 0x10] // RL B
		asic.cpu.registers.B = 0x80;
		asic.cpu.registers.flags.C = 1;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.B, 1)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - RR

	func test_RR() {
		let test: [UInt8] = [0xCB, 0x18] // RR B
		asic.cpu.registers.B = 1;
		asic.cpu.registers.flags.C = 0;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.B, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - SLA

	func test_SLA() {
		let test: [UInt8] = [0xCB, 0x20] // SLA B
		asic.cpu.registers.B = 0x80;
		asic.cpu.registers.flags.C = 0;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.B, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - SRA

	func test_SRA() {
		let test: [UInt8] = [0xCB, 0x28] // SRA B
		asic.cpu.registers.B = 1;
		asic.cpu.registers.flags.C = 0;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.B, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - SLL

	func test_SLL() {
		let test: [UInt8] = [0xCB, 0x30] // SLL B
		asic.cpu.registers.B = 1;
		asic.cpu.registers.flags.C = 0;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.B, 3)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - SRL

	func test_SRL() {
		let test: [UInt8] = [0xCB, 0x38] // SRL B
		asic.cpu.registers.B = 1;
		asic.cpu.registers.flags.C = 0;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.B, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}


}
