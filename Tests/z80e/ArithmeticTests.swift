import Testing
import XCTest
import XCTz80e
import z80e

final class ArithmeticTests: XCTestCaseTI83p {

	//MARK: - ADD

	/// `ADD HL, BC`
	let ADD_HL_BC: [UInt8] = [0x09]

	func test_ADD_HL_BC() {
		asic.cpu.registers.HL = 0x1000;
		asic.cpu.registers.BC = 0x0234;
		flash(ADD_HL_BC)
		let cycles = cpu_execute(&asic.cpu, 11)
		XCTAssertEqual(asic.cpu.registers.HL, 0x1234)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(asic.cpu.registers.flags.H, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - ADC

	/// `ADC HL, BC`
	let ADC_HL_BC: [UInt8] = [0xED, 0x4A]

	func test_ADC() {
		asic.cpu.registers.HL = 0x4000;
		asic.cpu.registers.BC = 0x100;
		flash(ADC_HL_BC)
		let cycles = cpu_execute(&asic.cpu, 15)
		XCTAssertEqual(asic.cpu.registers.HL, 0x4100)
		XCTAssertEqual(asic.cpu.registers.BC, 0x100)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	func test_ADC_C() {
		asic.cpu.registers.HL = 0xFF00;
		asic.cpu.registers.BC = 0x1000;
		asic.cpu.registers.flags.C = 0;
		flash(ADC_HL_BC)
		let cycles = cpu_execute(&asic.cpu, 15)
		XCTAssertEqual(asic.cpu.registers.HL, 0x0F00)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.H, 0)
		XCTAssertEqual(asic.cpu.registers.flags.PV, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	func test_ADC_CC() {
		asic.cpu.registers.HL = 0xF000;
		asic.cpu.registers.BC = 0x2000;
		asic.cpu.registers.flags.C = 1;
		flash(ADC_HL_BC)
		let cycles = cpu_execute(&asic.cpu, 15)
		XCTAssertEqual(asic.cpu.registers.HL, 0x1001)
		XCTAssertEqual(asic.cpu.registers.BC, 0x2000)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - INC

	/// `INC HL`
	let INC_HL: [UInt8] = [0x23]

	func test_INC_rp() {
		asic.cpu.registers.HL = 0xFFFF;
		flash(INC_HL)
		let cycles = cpu_execute(&asic.cpu, 6)
		XCTAssertEqual(asic.cpu.registers.HL, 0)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	/// `INC A`
	let INC_A: [UInt8] = [0x3C]

	func test_INC_r() {
		asic.cpu.registers.A = 0xFF;
		flash(INC_A)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.HL, 0)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 1)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - DEC

	/// `DEC HL`
	let DEC_HL: [UInt8] = [0x2B]

	func test_DEC_rp() {
		asic.cpu.registers.HL = 1;
		flash(DEC_HL)
		let cycles = cpu_execute(&asic.cpu, 6)
		XCTAssertEqual(asic.cpu.registers.HL, 0)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	/// `DEC A`
	let DEC_A: [UInt8] = [0x3D]

	func test_DEC_r() {
		asic.cpu.registers.A = 1;
		flash(DEC_A)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.HL, 0)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 1)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - CPL

	let CPL: [UInt8] = [0x2F]

	func test_CPL() {
		asic.cpu.registers.A = 0x80;
		flash(CPL)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x7F)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - DAA

	/// `ADD A, B \ DAA`
	let ADD_A_B_DAA: [UInt8] = [0x80, 0x27]

	func test_DAA() {
		// TODO: This could be more comprehensive
		asic.cpu.registers.A = 0x15;
		asic.cpu.registers.B = 0x27;
		flash(ADD_A_B_DAA)
		cpu_execute(&asic.cpu, 1)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x42)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - BIT

	/// `BIT 7, B`
	let BIT_7_B: [UInt8] = [0xCB, 0x78]

	func test_BIT() {
		asic.cpu.registers.B = 0x80;
		flash(BIT_7_B)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertNotEqual(asic.cpu.registers.flags.Z, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - RES

	let RES_7_B: [UInt8] = [0xCB, 0xB8]

	func test_RES() {
		asic.cpu.registers.B = 0xFF;
		flash(RES_7_B)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.B, 0x7F)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - SET

	let SET_7_B: [UInt8] = [0xCB, 0xF8]

	func test_SET() {
		asic.cpu.registers.B = 0;
		flash(SET_7_B)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.B, 0x80)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - NEG

	let NEG: [UInt8] = [0xED, 0x44]

	func test_NEG() {
		asic.cpu.registers.A = 2;
		flash(NEG)
		let cycles = cpu_execute(&asic.cpu, 8)
		XCTAssertEqual(asic.cpu.registers.A, 0xFE)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - SBC

	/// `SBC HL, BC`
	let SBC_HL_BC: [UInt8] = [0xED, 0x42]

	func test_SBC_HL_rp() {
		asic.cpu.registers.HL = 0x4000;
		asic.cpu.registers.BC = 0x100;
		flash(SBC_HL_BC)
		let cycles = cpu_execute(&asic.cpu, 15)
		XCTAssertEqual(asic.cpu.registers.HL, 0x3F00)
		XCTAssertEqual(asic.cpu.registers.BC, 0x100)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	func test_SBC_HL_rp_C() {
		asic.cpu.registers.HL = 0x1000;
		asic.cpu.registers.BC = 0x2000;
		asic.cpu.registers.flags.C = 1;
		flash(SBC_HL_BC)
		let cycles = cpu_execute(&asic.cpu, 15)
		XCTAssertEqual(asic.cpu.registers.HL, 0xEFFF)
		XCTAssertEqual(asic.cpu.registers.BC, 0x2000)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

}
