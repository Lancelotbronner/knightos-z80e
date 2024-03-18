import XCTest
import z80e

final class LoadTests: z80eTestCase {

	//MARK: - LD

	func test_LD_rp_nn() {
		let test: [UInt8] = [0x21, 0x34, 0x12] // LD HL, 0x1234
		flash(test)
		let cycles = cpu_execute(&device.cpu, 10)
		XCTAssertEqual(device.cpu.registers.HL, 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	func test_LD_BCptr_A() {
		let test: [UInt8] = [0x02] // LD (BC), A
		device.cpu.registers.BC = 0xC000;
		device.cpu.registers.A = 0x2F;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 7)
		XCTAssertEqual(read_byte(0xC000), 0x2F)
		XCTAssertEqual(cycles, 0)
	}

	func test_LD_nnptr_HL() {
		let test: [UInt8] = [0x22, 0x00, 0xC0] // LD (0xC000), HL
		device.cpu.registers.HL = 0x1234;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 16)
		XCTAssertEqual(read_word(0xC000), 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	func test_LD_nnptr_A() {
		let test: [UInt8] = [0x32, 0x00, 0xC0] // LD (0xC000), A
		device.cpu.registers.A = 0x4F;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 13)
		XCTAssertEqual(read_word(0xC000), 0x4F)
		XCTAssertEqual(cycles, 0)
	}

	func test_LD_A_BCptr() {
		let test: [UInt8] = [0x0A] // LD (BC), A
		device.cpu.registers.BC = 0xC000;
		write_byte(0xC000, 0x2F)
		flash(test)
		let cycles = cpu_execute(&device.cpu, 7)
		XCTAssertEqual(device.cpu.registers.A, 0x2F)
		XCTAssertEqual(cycles, 0)
	}

	func test_LD_HL_nnptr() {
		let test: [UInt8] = [0x2A, 0x00, 0xC0] // LD HL, (0xC000)
		write_word(0xC000, 0x1234)
		flash(test)
		let cycles = cpu_execute(&device.cpu, 16)
		XCTAssertEqual(device.cpu.registers.HL, 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	func test_LD_A_nnptr() {
		let test: [UInt8] = [0x3A, 0x00, 0xC0] // LD A, (0xC000)
		write_byte(0xC000, 0x4F)
		flash(test)
		let cycles = cpu_execute(&device.cpu, 13)
		XCTAssertEqual(device.cpu.registers.A, 0x4F)
		XCTAssertEqual(cycles, 0)
	}

	func test_LD_r_n() {
		let test: [UInt8] = [0x06, 0x2F] // LD B, 0x2F
		flash(test)
		let cycles = cpu_execute(&device.cpu, 7)
		XCTAssertEqual(device.cpu.registers.B, 0x2F)
		XCTAssertEqual(device.cpu.registers.PC, 2)
		XCTAssertEqual(cycles, 0)
	}

	func test_LD_r_r() {
		let test: [UInt8] = [0x78] // LD A, B
		device.cpu.registers.B = 0x4F;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.A, 0x4F)
		XCTAssertEqual(cycles, 0)
	}

	func test_LD_nn_rp() {
		let test: [UInt8] = [0xED, 0x43, 0x00, 0xC0] // LD (0xC000), BC
		device.cpu.registers.BC = 0x1234;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 20)
		XCTAssertEqual(read_word(0xC000), 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	func test_LD_rp_nn_ind() {
		let test: [UInt8] = [0xED, 0x4B, 0x00, 0xC0] // LD BC, (0xC000)
		write_word(0xC000, 0x1234)
		flash(test)
		let cycles = cpu_execute(&device.cpu, 20)
		XCTAssertEqual(device.cpu.registers.BC, 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	func test_LD_A_I() {
		let test: [UInt8] = [0xED, 0x57] // LD A, I
		device.cpu.registers.I = 0x34;
		device.cpu.IFF2 = 1;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 9)
		XCTAssertEqual(device.cpu.registers.flags.PV, 1)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - POP

	func test_POP_rp2() {
		let test: [UInt8] = [0xC1] // POP BC
		device.cpu.registers.SP = 0xC000;
		write_byte(0xC000, 0x34)
		write_byte(0xC001, 0x12)
		flash(test)
		let cycles = cpu_execute(&device.cpu, 10)
		XCTAssertEqual(device.cpu.registers.BC, 0x1234)
		XCTAssertEqual(device.cpu.registers.SP, 0xC002)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - EXX

	func test_EXX() {
		let test: [UInt8] = [0xD9]
		device.cpu.registers.BC = 0x1111;
		device.cpu.registers._BC = 0x2222;
		device.cpu.registers.DE = 0x3333;
		device.cpu.registers._DE = 0x4444;
		device.cpu.registers.HL = 0x5555;
		device.cpu.registers._HL = 0x6666;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.BC, 0x2222)
		XCTAssertEqual(device.cpu.registers._BC, 0x1111)
		XCTAssertEqual(device.cpu.registers.DE, 0x4444)
		XCTAssertEqual(device.cpu.registers._DE, 0x3333)
		XCTAssertEqual(device.cpu.registers.HL, 0x6666)
		XCTAssertEqual(device.cpu.registers._HL, 0x5555)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - EX

	func test_EX_SP_HL() {
		let test: [UInt8] = [0xE3] // EX (SP), HL
		device.cpu.registers.HL = 0xDEAD;
		device.cpu.registers.SP = 0xC000;
		write_word(0xC000, 0xBEEF)
		flash(test)
		let cycles = cpu_execute(&device.cpu, 19)
		XCTAssertEqual(device.cpu.registers.HL, 0xBEEF)
		XCTAssertEqual(read_word(0xC000), 0xDEAD)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - EX

	func test_EX_DE_HL() {
		let test: [UInt8] = [0xEB] // EX DE, HL
		device.cpu.registers.HL = 0xDEAD;
		device.cpu.registers.DE = 0xBEEF;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 4)
		XCTAssertEqual(device.cpu.registers.HL, 0xBEEF)
		XCTAssertEqual(device.cpu.registers.DE, 0xDEAD)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - PUSH

	func test_PUSH_rp2() {
		let test: [UInt8] = [0xD5] // PUSH DE
		device.cpu.registers.DE = 0x1234;
		flash(test)
		let cycles = cpu_execute(&device.cpu, 11)
		XCTAssertEqual(device.cpu.registers.SP, 0xFFFE)
		XCTAssertEqual(read_word(0xFFFE), 0x1234)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - RRD RLD

	func test_RRD_RLD() {
		let test: [UInt8] = [0xED, 0x67, 0xED, 0x6F] // RRD \ RLD
		device.cpu.registers.HL = 0xC000;
		device.cpu.registers.A = 0x12;
		write_word(0xC000, 0x3456)
		flash(test)
		var cycles = cpu_execute(&device.cpu, 18)
		XCTAssertEqual(device.cpu.registers.A, 0x16)
		XCTAssertEqual(read_word(0xC000), 0x3425)
		XCTAssertEqual(cycles, 0)
		cycles = cpu_execute(&device.cpu, 18)
		XCTAssertEqual(device.cpu.registers.A, 0x12)
		XCTAssertEqual(read_word(0xC000), 0x3456)
		XCTAssertEqual(cycles, 0)
	}

}
