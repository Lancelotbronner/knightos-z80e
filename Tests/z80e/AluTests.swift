import Testing
import XCTest
import XCTz80e
import z80e

@Suite(.tags(.alu))
struct AluTests {

	//MARK: - ADD

	/// `ADD A B`
	let ADD_A_B: [UInt8] = [0x80]

	/// `ADD A, (HL)`
	let ADD_A_HL: [UInt8] = [0x86]

	@Test("ADD A B", arguments: ti_device_type.z80)
	func test_ADD(_ type: ti_device_type) {
		var asic = Asic(manage: type).rawValue
		asic.cpu.registers.A = 0x10
		asic.cpu.registers.B = 0x20
		asic.flash(ADD_A_B);
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0x30)
		#expect(asic.cpu.registers.B == 0x20)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(asic.cpu.registers.flags.C == 0)
		#expect(cycles == 0)
	}

	@Test("ADD A B; carry")
	func test_ADD_C() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0xF0;
		asic.cpu.registers.B = 0x20;
		asic.flash(ADD_A_B);
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0x10)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(asic.cpu.registers.flags.C == 1)
		#expect(cycles == 0)
	}

	@Test("ADD A B; zero & carry")
	func test_ADD_ZC() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0xF0;
		asic.cpu.registers.B = 0x10;
		asic.flash(ADD_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0)
		#expect(asic.cpu.registers.flags.Z == 1)
		#expect(asic.cpu.registers.flags.C == 1)
		#expect(cycles == 0)
	}

	@Test("ADD A (HL)")
	func test_ADD_HL() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0x10;
		asic.cpu.registers.HL = 0x1000;
		mmu_force_write(&asic.mmu, 0x1000, 0x20)
		asic.flash(ADD_A_HL);
		let cycles = cpu_execute(&asic.cpu, 7)
		#expect(asic.cpu.registers.A == 0x30)
		#expect(cycles == 0)
	}

	/// `ADD A, 0x20`
	let ADD_A_IMM: [UInt8] = [0xC6, 0x20]

	@Test("ADD A, 0x20")
	func test_ADD_IMM() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0x10;
		asic.flash(ADD_A_IMM)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0x30)
		#expect(cycles == 0)
	}

	//MARK: - ADC

	/// `ADC A, B`
	let ADC_A_B: [UInt8] = [0x88]

	@Test("ADC A, B; C=0")
	func test_ADC() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0x10;
		asic.cpu.registers.B = 0x20;
		asic.flash(ADC_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0x30)
		#expect(asic.cpu.registers.B == 0x20)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(asic.cpu.registers.flags.C == 0)
		#expect(cycles == 0)
	}

	@Test("ADC A, B; C=1")
	func test_ADC_C() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0x10;
		asic.cpu.registers.B = 0x20;
		asic.cpu.registers.flags.C = 1;
		asic.flash(ADC_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0x31)
		#expect(asic.cpu.registers.B == 0x20)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(asic.cpu.registers.flags.C == 0)
		#expect(cycles == 0)
	}

	//MARK: - SUB

	/// `SUB A, B`
	let SUB_A_B: [UInt8] = [0x90]

	@Test("SUB A, B")
	func test_SUB() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0x20;
		asic.cpu.registers.B = 0x10;
		asic.flash(SUB_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0x10)
		#expect(asic.cpu.registers.B == 0x10)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(asic.cpu.registers.flags.C == 0)
		#expect(cycles == 0)
	}

	@Test("SUB A, B; carry")
	func test_SUB_C() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0x10;
		asic.cpu.registers.B = 0x20;
		asic.flash(SUB_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0xF0)
		#expect(asic.cpu.registers.B == 0x20)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(asic.cpu.registers.flags.C == 1)
		#expect(cycles == 0)
	}

	//MARK: - SBC

	/// `SBC A, B`
	let SBC_A_B: [UInt8] = [0x98]

	@Test("SBC A, B")
	func test_SBC() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0x20;
		asic.cpu.registers.B = 0x10;
		asic.flash(SBC_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0x10)
		#expect(asic.cpu.registers.B == 0x10)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(asic.cpu.registers.flags.C == 0)
		#expect(cycles == 0)
	}

	@Test("SUB A, B; carry")
	func test_SBC_C() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0x10;
		asic.cpu.registers.B = 0x20;
		asic.cpu.registers.flags.C = 1;
		asic.flash(SBC_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0xEF)
		#expect(asic.cpu.registers.B == 0x20)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(asic.cpu.registers.flags.C == 1)
		#expect(cycles == 0)
	}

	//MARK: - AND

	/// `AND A, B`
	let AND_A_B: [UInt8] = [0xA0]

	@Test("AND A, B")
	func test_AND() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0xFF;
		asic.cpu.registers.B = 0x0F;
		asic.flash(AND_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0x0F)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(cycles == 0)
	}

	//MARK: - XOR

	/// `XOR A, B`
	let XOR_A_B: [UInt8] = [0xA8]

	@Test("XOR A, B")
	func test_XOR() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0xFF;
		asic.cpu.registers.B = 0x0F;
		asic.flash(XOR_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0xF0)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(cycles == 0)
	}

	//MARK: - OR

	/// `OR A, B`
	let OR_A_B: [UInt8] = [0xB0]

	@Test("OR A, B")
	func test_OR() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0x00;
		asic.cpu.registers.B = 0x0F;
		asic.flash(OR_A_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0x0F)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(cycles == 0)
	}

	//MARK: - CP

	let CP_B: [UInt8] = [0xB8]

	@Test("CP B")
	func test_CP() {
		var asic = Asic(manage: TI83p).rawValue
		asic.cpu.registers.A = 0x00;
		asic.cpu.registers.B = 0x10;
		asic.flash(CP_B)
		let cycles = cpu_execute(&asic.cpu, 4)
		#expect(asic.cpu.registers.A == 0)
		#expect(asic.cpu.registers.flags.S == 1)
		#expect(asic.cpu.registers.flags.C == 1)
		#expect(asic.cpu.registers.flags.PV == 0)
		#expect(asic.cpu.registers.flags.N == 1)
		#expect(asic.cpu.registers.flags.Z == 0)
		#expect(cycles == 0)
	}

}
