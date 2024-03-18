import XCTest
import z80e

final class InterruptsTests: z80eTestCase {

	//MARK: - IM 1

	/// `IM 1 \ EI \ JR $`
	let IM_1: [UInt8] = [0xED, 0x56, 0xFB, 0x18, 0xFE]

	func test_IM_1() {
		flash(IM_1)
		cpu_execute(&device.cpu, 100)
		XCTAssertEqual(device.cpu.registers.PC, 3)
		device.cpu.interrupt = 1;
		let cycles = cpu_execute(&device.cpu, 13)
		XCTAssertEqual(device.cpu.registers.PC, 0x38)
		XCTAssertEqual(device.cpu.registers.SP, 0xFFFE)
		XCTAssertEqual(cycles, 0)
	}

	//MARK: - IM2

	/// `IM 2 \ LD A, 0x80 \ LD I, A \ EI \ JR $`
	let IM2: [UInt8] = [0xED, 0x5E, 0x3E, 0x80, 0xED, 0x47, 0xFB, 0x18, 0xFE]

	func test_IM_2() {
		flash(IM2)
		cpu_execute(&device.cpu, 100)
		XCTAssertEqual(device.cpu.registers.PC, 7)
		XCTAssertEqual(device.cpu.registers.I, 0x80)
		device.cpu.bus = 0x90;
		device.cpu.interrupt = 1;
		let cycles = cpu_execute(&device.cpu, 19)
		XCTAssertEqual(device.cpu.registers.PC, 0x8090)
		XCTAssertEqual(device.cpu.registers.SP, 0xFFFE)
		XCTAssertEqual(cycles, 0)
	}

}
