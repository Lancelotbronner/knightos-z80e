import XCTest
import XCTz80e
import z80e

final class ArithmeticLogicUnitTests: XCTestCaseTI83p {

	//MARK: - ADD

	/// `ADD A B`
	let ADD_A_B: [UInt8] = [0x80]

	/// `ADD A, (HL)`
	let ADD_A_HL: [UInt8] = [0x86]

	func test_ADD() {
		asic.cpu.registers.A = 0x10
		asic.cpu.registers.B = 0x20
		flash(ADD_A_B);
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x30)
		XCTAssertEqual(asic.cpu.registers.B, 0x20)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	func test_ADD_C() {
		asic.cpu.registers.A = 0xF0;
		asic.cpu.registers.B = 0x20;
		flash(ADD_A_B);
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x10)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	func test_ADD_ZC() {
		asic.cpu.registers.A = 0xF0;
		asic.cpu.registers.B = 0x10;
		flash(ADD_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 1)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	func test_ADD_HL() {
		asic.cpu.registers.A = 0x10;
		asic.cpu.registers.HL = 0x1000;
		withUnsafeMutablePointer(to: &asic.mmu) {
			mmu_force_write(UnsafeMutableRawPointer($0), 0x1000, 0x20);
		}
		flash(ADD_A_HL);
		let cycles = cpu_execute(&asic.cpu, 7)
		XCTAssertEqual(asic.cpu.registers.A, 0x30)
		XCTAssertEqual(cycles, 0)
	}

	/// `ADD A, 0x20`
	let ADD_A_IMM: [UInt8] = [0xC6, 0x20]

	func test_ADD_IMM() {
		asic.cpu.registers.A = 0x10;
		flash(ADD_A_IMM)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x30)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - ADC

	/// `ADC A, B`
	let ADC_A_B: [UInt8] = [0x88]

	func test_ADC() {
		asic.cpu.registers.A = 0x10;
		asic.cpu.registers.B = 0x20;
		flash(ADC_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x30)
		XCTAssertEqual(asic.cpu.registers.B, 0x20)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	func test_ADC_C() {
		asic.cpu.registers.A = 0x10;
		asic.cpu.registers.B = 0x20;
		asic.cpu.registers.flags.C = 1;
		flash(ADC_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x31)
		XCTAssertEqual(asic.cpu.registers.B, 0x20)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - SUB

	/// `SUB A, B`
	let SUB_A_B: [UInt8] = [0x90]

	func test_SUB() {
		asic.cpu.registers.A = 0x20;
		asic.cpu.registers.B = 0x10;
		flash(SUB_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x10)
		XCTAssertEqual(asic.cpu.registers.B, 0x10)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	func test_SUB_C() {
		asic.cpu.registers.A = 0x10;
		asic.cpu.registers.B = 0x20;
		flash(SUB_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0xF0)
		XCTAssertEqual(asic.cpu.registers.B, 0x20)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - SBC

	/// `SBC A, B`
	let SBC_A_B: [UInt8] = [0x98]

	func test_SBC() {
		asic.cpu.registers.A = 0x20;
		asic.cpu.registers.B = 0x10;
		flash(SBC_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x10)
		XCTAssertEqual(asic.cpu.registers.B, 0x10)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	func test_SBC_C() {
		asic.cpu.registers.A = 0x10;
		asic.cpu.registers.B = 0x20;
		asic.cpu.registers.flags.C = 1;
		flash(SBC_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0xEF)
		XCTAssertEqual(asic.cpu.registers.B, 0x20)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - AND

	/// `AND A, B`
	let AND_A_B: [UInt8] = [0xA0]

	func test_AND() {
		asic.cpu.registers.A = 0xFF;
		asic.cpu.registers.B = 0x0F;
		flash(AND_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x0F)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - XOR

	/// `XOR A, B`
	let XOR_A_B: [UInt8] = [0xA8]

	func test_XOR() {
		asic.cpu.registers.A = 0xFF;
		asic.cpu.registers.B = 0x0F;
		flash(XOR_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0xF0)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - OR

	/// `OR A, B`
	let OR_A_B: [UInt8] = [0xB0]

	func test_OR() {
		asic.cpu.registers.A = 0x00;
		asic.cpu.registers.B = 0x0F;
		flash(OR_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0x0F)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - CP

	let CP_B: [UInt8] = [0xB8]

	func test_CP() {
		asic.cpu.registers.A = 0x00;
		asic.cpu.registers.B = 0x10;
		flash(CP_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		XCTAssertEqual(asic.cpu.registers.A, 0)
		XCTAssertEqual(asic.cpu.registers.flags.S, 1)
		XCTAssertEqual(asic.cpu.registers.flags.C, 1)
		XCTAssertEqual(asic.cpu.registers.flags.PV, 0)
		XCTAssertEqual(asic.cpu.registers.flags.N, 1)
		XCTAssertEqual(asic.cpu.registers.flags.Z, 0)
		XCTAssertEqual(cycles, 0)
	}

}
