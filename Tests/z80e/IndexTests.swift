import XCTest
import XCTz80e
import z80e

final class IndexTests: XCTestCaseTI83p {

	//MARK: - LD

	func test_LD_IX_A() {
		let test_1: [UInt8] = [0xDD, 0x77, 0x00] // LD (IX + 0), A
		asic.cpu.registers.IX = 0xC000;
		asic.cpu.registers.A = 0x30;
		flash(test_1)
		cpu_execute(&asic.cpu, 19)
		XCTAssertEqual(read_byte(0xC000), 0x30)
	}

	func test_LD_A_IX() {
		let test_2: [UInt8] = [0xDD, 0x7E, 0x00] // LD A, (IX + 0)
		asic.cpu.registers.IX = 0xC000;
		write_byte(0xC000, 0x30)
		flash(test_2)
		cpu_execute(&asic.cpu, 19)
		XCTAssertEqual(asic.cpu.registers.A, 0x30)
	}

	func test_LD_IY_A() {
		let test_3: [UInt8] = [0xFD, 0x77, 0x00] // LD (IY + 0), A
		asic.cpu.registers.IY = 0xC000;
		asic.cpu.registers.A = 0x30;
		flash(test_3)
		cpu_execute(&asic.cpu, 19)
		XCTAssertEqual(read_byte(0xC000), 0x30)
	}

	func test_LD_A_IY() {
		let test_4: [UInt8] = [0xFD, 0x7E, 0x00] // LD A, (IY + 0)
		asic.cpu.registers.IY = 0xC000;
		write_byte(0xC000, 0x30)
		flash(test_4)
		cpu_execute(&asic.cpu, 19)
		XCTAssertEqual(asic.cpu.registers.A, 0x30)
	}

	//MARK: - JP

	func test_JP_IX() {
		let test: [UInt8] = [0xDD, 0xE9] // JP (IX)
		asic.cpu.registers.IX = 0x1234;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.PC, 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	func test_JP_IY() {
		let test_2: [UInt8] = [0xFD, 0xE9] // JP (IY)
		asic.cpu.registers.IY = 0x1234;
		flash(test_2)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.PC, 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - ADD

	/// `ADD IX, BC`
	let ADD_IX: [UInt8] = [0xDD, 0x09]

	func test_ADD_IX() {
		asic.cpu.registers.IX = 0x1000;
		asic.cpu.registers.BC = 0x0234;
		flash(ADD_IX)
		let cycles = cpu_execute(&asic.cpu, 15)
		XCTAssertEqual(asic.cpu.registers.IX, 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	func test_ADD_IX_cc() {
		asic.cpu.registers.IX = 0xF000;
		asic.cpu.registers.BC = 0x1000;
		asic.cpu.registers.flags.Z = 0;
		flash(ADD_IX)
		let cycles = cpu_execute(&asic.cpu, 15)
		XCTAssertEqual(asic.cpu.registers.IX, 0)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - Miscellaneous

	func test_prefix_reset() {
		let test: [UInt8] = [0xDD, 0x09, 0x09] // ADD IX, BC \ ADD HL, BC
		asic.cpu.registers.IX = 0x1000;
		asic.cpu.registers.HL = 0x2000;
		asic.cpu.registers.BC = 0x0234;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 26)
		XCTAssertEqual(asic.cpu.registers.IX, 0x1234)
		XCTAssertEqual(asic.cpu.registers.HL, 0x2234)
		XCTAssertEqual(cycles, 0)
	}

	func test_index_offsets() {
		let test: [UInt8] = [0xDD, 0x86, 0x0A] // ADD A, (IX + 10)
		asic.cpu.registers.IX = 0x1000;
		asic.cpu.registers.A = 0x10;
		mmu_force_write(&asic.mmu, 0x1000 + 10, 0x20);
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 19)
		XCTAssertEqual(asic.cpu.registers.A, 0x30)
		XCTAssertEqual(cycles, 0)
	}
	
	//MARK: - IXH IXL

	func test_IXH() {
		let test: [UInt8] = [0xDD, 0x84] // ADD A, IXH
		asic.cpu.registers.IXH = 0x20;
		asic.cpu.registers.A = 0x10;
		flash(test)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.A, 0x30)
		XCTAssertEqual(cycles, 0)
	}

	func test_IXL() {
		let test2: [UInt8] = [0xDD, 0x85] // ADD A, IXL
		asic.cpu.registers.IXL = 0x20;
		asic.cpu.registers.A = 0x10;
		flash(test2)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.A, 0x30)
		XCTAssertEqual(cycles, 0)
	}

}
