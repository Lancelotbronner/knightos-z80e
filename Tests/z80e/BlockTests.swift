import XCTest
import XCTz80e
import z80e

final class BlockTests: XCTestCaseTI83p {

	// Note: INI, INIR, IND, INDR, OUTI, OUTD, OTIR, and OTDR are tested in IOTests.swift

	//MARK: - LDI

	/// `LDI`
	let LDI: [UInt8] = [0xED, 0xA0]

	func test_LDI() {
		asic.cpu.registers.HL = 0xC000;
		asic.cpu.registers.DE = 0xD000;
		asic.cpu.registers.BC = 5;
		write_byte(0xC000, 0x11)
		write_byte(0xC001, 0x22)
		write_byte(0xC002, 0x33)
		write_byte(0xC003, 0x44)
		write_byte(0xC004, 0x55)
		flash(LDI)
		let cycles = cpu_execute(&asic.cpu, 16)
		XCTAssertEqual(read_byte(0xD000), 0x11)
		XCTAssertEqual(asic.cpu.registers.HL, 0xC001)
		XCTAssertEqual(asic.cpu.registers.DE, 0xD001)
		XCTAssertEqual(asic.cpu.registers.BC, 4)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - LDIR

	/// `LDIR`
	let LDIR: [UInt8] = [0xED, 0xB0]

	func test_LDIR() {
		asic.cpu.registers.HL = 0xC000;
		asic.cpu.registers.DE = 0xD000;
		asic.cpu.registers.BC = 5;
		write_byte(0xC000, 0x11)
		write_byte(0xC001, 0x22)
		write_byte(0xC002, 0x33)
		write_byte(0xC003, 0x44)
		write_byte(0xC004, 0x55)
		flash(LDIR)
		let cycles = cpu_execute(&asic.cpu, 100)
		XCTAssertEqual(read_byte(0xD000), 0x11)
		XCTAssertEqual(read_byte(0xD001), 0x22)
		XCTAssertEqual(read_byte(0xD002), 0x33)
		XCTAssertEqual(read_byte(0xD003), 0x44)
		XCTAssertEqual(read_byte(0xD004), 0x55)
		XCTAssertEqual(asic.cpu.registers.HL, 0xC005)
		XCTAssertEqual(asic.cpu.registers.DE, 0xD005)
		XCTAssertEqual(asic.cpu.registers.BC, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - LDD

	/// `LDD`
	let LDD: [UInt8] = [0xED, 0xA8]

	func test_LDD() {
		asic.cpu.registers.HL = 0xC004;
		asic.cpu.registers.DE = 0xD004;
		asic.cpu.registers.BC = 5;
		write_byte(0xC000, 0x11)
		write_byte(0xC001, 0x22)
		write_byte(0xC002, 0x33)
		write_byte(0xC003, 0x44)
		write_byte(0xC004, 0x55)
		flash(LDD)
		let cycles = cpu_execute(&asic.cpu, 16)
		XCTAssertEqual(read_byte(0xD004), 0x55)
		XCTAssertEqual(asic.cpu.registers.HL, 0xC003)
		XCTAssertEqual(asic.cpu.registers.DE, 0xD003)
		XCTAssertEqual(asic.cpu.registers.BC, 4)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - LDDR

	/// `LDDR`
	let LDDR: [UInt8] = [0xED, 0xB8]

	func test_LDDR() {
		asic.cpu.registers.HL = 0xC004;
		asic.cpu.registers.DE = 0xD004;
		asic.cpu.registers.BC = 5;
		write_byte(0xC000, 0x11)
		write_byte(0xC001, 0x22)
		write_byte(0xC002, 0x33)
		write_byte(0xC003, 0x44)
		write_byte(0xC004, 0x55)
		flash(LDDR)
		let cycles = cpu_execute(&asic.cpu, 100)
		XCTAssertEqual(read_byte(0xD000), 0x11)
		XCTAssertEqual(read_byte(0xD001), 0x22)
		XCTAssertEqual(read_byte(0xD002), 0x33)
		XCTAssertEqual(read_byte(0xD003), 0x44)
		XCTAssertEqual(read_byte(0xD004), 0x55)
		XCTAssertEqual(asic.cpu.registers.HL, 0xBFFF)
		XCTAssertEqual(asic.cpu.registers.DE, 0xCFFF)
		XCTAssertEqual(asic.cpu.registers.BC, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - CPI

	/// `CPI`
	let CPI: [UInt8] = [0xED, 0xA1]

	func test_CPI() {
		asic.cpu.registers.HL = 0xC000;
		asic.cpu.registers.BC = 5;
		asic.cpu.registers.A = 0x33;
		write_byte(0xC000, 0x11)
		write_byte(0xC001, 0x22)
		write_byte(0xC002, 0x33)
		flash(CPI)
		let cycles = cpu_execute(&asic.cpu, 16)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.HL, 0xC001)
		XCTAssertEqual(asic.cpu.registers.BC, 4)
		XCTAssertEqual(cycles, 0)
		asic.cpu.registers.PC = 0;
		cpu_execute(&asic.cpu, 16)
		asic.cpu.registers.PC = 0;
		cpu_execute(&asic.cpu, 16)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 1)
		XCTAssertEqual(asic.cpu.registers.HL, 0xC003)
		XCTAssertEqual(asic.cpu.registers.BC, 2)
	}

	//MARK: - CPD

	/// `CPD`
	let CPD: [UInt8] = [0xED, 0xA9]

	func test_CPD() {
		asic.cpu.registers.HL = 0xC002;
		asic.cpu.registers.BC = 5;
		asic.cpu.registers.A = 0x11;
		write_byte(0xC000, 0x11)
		write_byte(0xC001, 0x22)
		write_byte(0xC002, 0x33)
		flash(CPD)
		let cycles = cpu_execute(&asic.cpu, 16)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.HL, 0xC001)
		XCTAssertEqual(asic.cpu.registers.BC, 4)
		XCTAssertEqual(cycles, 0)
		asic.cpu.registers.PC = 0;
		cpu_execute(&asic.cpu, 16)
		asic.cpu.registers.PC = 0;
		cpu_execute(&asic.cpu, 16)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 1)
		XCTAssertEqual(asic.cpu.registers.HL, 0xBFFF)
		XCTAssertEqual(asic.cpu.registers.BC, 2)
	}

	//MARK: - CPIR

	/// `CPIR`
	let CPIR: [UInt8] = [0xED, 0xB1]

	func test_CPIR() {
		asic.cpu.registers.HL = 0xC000;
		asic.cpu.registers.BC = 5;
		asic.cpu.registers.A = 0x33;
		write_byte(0xC000, 0x11)
		write_byte(0xC001, 0x22)
		write_byte(0xC002, 0x33)
		write_byte(0xC003, 0x44)
		flash(CPIR)
		let cycles = cpu_execute(&asic.cpu, 58)
		XCTAssertEqual(asic.cpu.registers.HL, 0xC003)
		XCTAssertEqual(asic.cpu.registers.BC, 2)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - CPDR

	/// `CPDR`
	let CPDR: [UInt8] = [0xED, 0xB9]

	func test_CPDR() {
		asic.cpu.registers.HL = 0xC004;
		asic.cpu.registers.BC = 5;
		asic.cpu.registers.A = 0x33;
		write_byte(0xC001, 0x22)
		write_byte(0xC002, 0x33)
		write_byte(0xC003, 0x44)
		write_byte(0xC004, 0x55)
		flash(CPDR)
		let cycles = cpu_execute(&asic.cpu, 58)
		XCTAssertEqual(asic.cpu.registers.HL, 0xC001)
		XCTAssertEqual(asic.cpu.registers.BC, 2)
		XCTAssertEqual(cycles, 0)
	}

}
