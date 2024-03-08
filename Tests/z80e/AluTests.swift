import XCTest
import z80e

final class ArithmeticLogicUnitTests: z80eTestCase {

	//MARK: - Add

	/// `ADD A B`
	let ADD_A_B: [UInt8] = [0x80]

	/// `ADD A, (HL)`
	let ADD_A_HL: [UInt8] = [0x86]

	func test_ADD() {
		device.cpu.registers.A = 0x10
		device.cpu.registers.B = 0x20
		flash(ADD_A_B);
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0x30)
		XCTAssertEqual(device.cpu.registers.B, 0x20)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(device.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	func test_ADD_carry() {
		device.cpu.registers.A = 0xF0;
		device.cpu.registers.B = 0x20;
		flash(ADD_A_B);
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0x10)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(device.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	func test_ADD_zeroAndCarry() {
		device.cpu.registers.A = 0xF0;
		device.cpu.registers.B = 0x10;
		flash(ADD_A_B)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0)
		XCTAssertEqual(device.cpu.registers.flags.Z, 1)
		XCTAssertEqual(device.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	func test_ADD_HL() {
		device.cpu.registers.A = 0x10;
		device.cpu.registers.HL = 0x1000;
		mmu_force_write(device.mmu, 0x1000, 0x20);
		flash(ADD_A_HL);
		let cycles = cpu_execute(&device.cpu, 7)
		XCTAssertEqual(device.cpu.registers.A, 0x30)
		XCTAssertEqual(cycles, 0)
	}

	/// `ADD A, 0x20`
	let ADD_A_IMM: [UInt8] = [0xC6, 0x20]

	func test_ADD_immediate() {
		device.cpu.registers.A = 0x10;
		flash(ADD_A_IMM)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0x30)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - Add with Carry

	/// `ADC A, B`
	let ADC_A_B: [UInt8] = [0x88]

	func test_ADC_withoutCarry() {
		device.cpu.registers.A = 0x10;
		device.cpu.registers.B = 0x20;
		flash(ADC_A_B)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0x30)
		XCTAssertEqual(device.cpu.registers.B, 0x20)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(device.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	func test_ADC_withCarry() {
		device.cpu.registers.A = 0x10;
		device.cpu.registers.B = 0x20;
		device.cpu.registers.flags.C = 1;
		flash(ADC_A_B)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0x31)
		XCTAssertEqual(device.cpu.registers.B, 0x20)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(device.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - Subtract

	/// `SUB A, B`
	let SUB_A_B: [UInt8] = [0x90]

	func test_SUB() {
		device.cpu.registers.A = 0x20;
		device.cpu.registers.B = 0x10;
		flash(SUB_A_B)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0x10)
		XCTAssertEqual(device.cpu.registers.B, 0x10)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(device.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	func test_SUB_borrow() {
		device.cpu.registers.A = 0x10;
		device.cpu.registers.B = 0x20;
		flash(SUB_A_B)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0xF0)
		XCTAssertEqual(device.cpu.registers.B, 0x20)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(device.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - Subtract with Carry

	/// `SBC A, B`
	let SBC_A_B: [UInt8] = [0x98]

	func test_SBC_withoutCarry() {
		device.cpu.registers.A = 0x20;
		device.cpu.registers.B = 0x10;
		flash(SBC_A_B)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0x10)
		XCTAssertEqual(device.cpu.registers.B, 0x10)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(device.cpu.registers.flags.C, 0)
		XCTAssertEqual(cycles, 0)
	}

	func test_SBC_withCarry() {
		device.cpu.registers.A = 0x10;
		device.cpu.registers.B = 0x20;
		device.cpu.registers.flags.C = 1;
		flash(SBC_A_B)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0xEF)
		XCTAssertEqual(device.cpu.registers.B, 0x20)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(device.cpu.registers.flags.C, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - And

	/// `AND A, B`
	let AND_A_B: [UInt8] = [0xA0]

	func test_AND() {
		device.cpu.registers.A = 0xFF;
		device.cpu.registers.B = 0x0F;
		flash(AND_A_B)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0x0F)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - XOR

	/// `XOR A, B`
	let XOR_A_B: [UInt8] = [0xA8]

	func test_XOR() {
		device.cpu.registers.A = 0xFF;
		device.cpu.registers.B = 0x0F;
		flash(XOR_A_B)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0xF0)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - OR

	/// `OR A, B`
	let OR_A_B: [UInt8] = [0xB0]

	func test_OR() {
		device.cpu.registers.A = 0x00;
		device.cpu.registers.B = 0x0F;
		flash(OR_A_B)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0x0F)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - CP

	let CP_B: [UInt8] = [0xB8]

	func test_CP() {
		device.cpu.registers.A = 0x00;
		device.cpu.registers.B = 0x10;
		flash(CP_B)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0)
		XCTAssertEqual(device.cpu.registers.flags.S, 1)
		XCTAssertEqual(device.cpu.registers.flags.C, 1)
		XCTAssertEqual(device.cpu.registers.flags.PV, 0)
		XCTAssertEqual(device.cpu.registers.flags.N, 1)
		XCTAssertEqual(device.cpu.registers.flags.Z, 0)
		XCTAssertEqual(cycles, 0)
	}

}
