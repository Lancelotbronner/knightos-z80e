import XCTest
import z80e

func cpu_reset(_ device: UnsafeMutableRawPointer?, _ data: UInt8) {
	XCTFail("Jumped to 0x00!")
	exit(0)
}

func write_text(_ device: UnsafeMutableRawPointer?) -> CUnsignedChar {
	guard let _device = device?.assumingMemoryBound(to: asic_t.self) else {
		return 0
	}

	var asic: asic_t {
		_read { yield _device.pointee }
	}

	if (asic.cpu.registers.C == 2) {
		print(Character(UnicodeScalar(asic.cpu.registers.E)), terminator: "")
	} else if (asic.cpu.registers.C == 9) {
		var i = Int(asic.cpu.registers.DE)
		while asic.mmu.ram[i & 0xffff] != 0x24 {
			print(Character(UnicodeScalar(asic.mmu.ram[i & 0xffff])), terminator: "")
			if (asic.mmu.ram[i & 0xffff] == 0) {
				break
			}
			i += 1
		}
	}
	fflush(stdout);
	return 0;
}

final class ZexTests: z80eTestCase {

	func zex(_ resource: String, _ ext: String) {
		guard let url = Bundle.module.url(forResource: resource, withExtension: ext, subdirectory: "zex") else {
			XCTFail("Couldn't find testsuite \(resource).\(ext)")
			return
		}

		for i in 0..<0x100 {
			let _iodevice = cpu_device(&device.cpu, UInt8(i))
			var iodevice: z80iodevice {
				_read { yield _iodevice.pointee }
				_modify { yield &_iodevice.pointee }
			}

			iodevice.write_out = cpu_reset;
			iodevice.read_in = write_text;
			iodevice.device = UnsafeMutableRawPointer(_device);
		}

		guard let file = fopen(url.path(percentEncoded: false), "rb") else {
			let message = strerror(errno).map { String(cString: $0) } ?? ""
			XCTFail("\(resource).\(ext): \(message)")
			return
		}

		fread(device.mmu.ram + 0x100, 1, Int(device.mmu.settings.ram_pages) * 0x4000, file);
		fclose(file);

		device.mmu.ram[0] = 0xd3; /* OUT N, A */
		device.mmu.ram[1] = 0x00;

		device.mmu.ram[5] = 0xdb; /* IN A, N */
		device.mmu.ram[6] = 0x00;
		device.mmu.ram[7] = 0xc9; /* RET */

		device.mmu.banks.0.page = 0;
		device.mmu.banks.1.page = 1;
		device.mmu.banks.2.page = 2;
		device.mmu.banks.3.page = 3;

		device.mmu.banks.0.flash = 0;
		device.mmu.banks.1.flash = 0;
		device.mmu.banks.2.flash = 0;
		device.mmu.banks.3.flash = 0;

		device.cpu.registers.PC = 0x100;

		while true {
			cpu_execute(&device.cpu, 10000);
		}
	}

	func testAll() {
		zex("zexall", "com")
	}

	func testDoc() {
		zex("zexdoc", "com")
	}

}
