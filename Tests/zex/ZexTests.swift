import XCTest
import XCTz80e
import z80e

func cpu_reset(_ device: UnsafeMutablePointer<device>, _ data: UInt8) {
	XCTFail("Jumped to 0x00!")
	exit(0)
}

func write_text(_ device: UnsafeMutablePointer<device>) -> CUnsignedChar {
	guard let _device = device.pointee.data?.assumingMemoryBound(to: asic.self) else {
		return 0
	}

	var asic: asic {
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

final class ZexTests: XCTestCaseAsic {

	func zex(_ resource: String, _ ext: String) {
		guard let url = Bundle.module.url(forResource: resource, withExtension: ext, subdirectory: "zex") else {
			XCTFail("Couldn't find testsuite \(resource).\(ext)")
			return
		}

		for i in 0..<0x100 {
			let _iodevice = cpu_device(&asic.cpu, UInt8(i))
			var iodevice: device {
				_read { yield _iodevice.pointee }
				_modify { yield &_iodevice.pointee }
			}

			iodevice.write = cpu_reset;
			iodevice.read = write_text;
			iodevice.data = UnsafeMutableRawPointer(_asic);
		}

		guard let file = fopen(url.path(percentEncoded: false), "rb") else {
			let message = strerror(errno).map { String(cString: $0) } ?? ""
			XCTFail("\(resource).\(ext): \(message)")
			return
		}

		fread(asic.mmu.ram + 0x100, 1, Int(asic.mmu.settings.ram_pages) * 0x4000, file);
		fclose(file);

		asic.mmu.ram[0] = 0xd3; /* OUT N, A */
		asic.mmu.ram[1] = 0x00;

		asic.mmu.ram[5] = 0xdb; /* IN A, N */
		asic.mmu.ram[6] = 0x00;
		asic.mmu.ram[7] = 0xc9; /* RET */

		asic.mmu.banks.page.0 = 0;
		asic.mmu.banks.page.1 = 1;
		asic.mmu.banks.page.2 = 2;
		asic.mmu.banks.page.3 = 3;

		asic.mmu.banks.flash0 = false;
		asic.mmu.banks.flash1 = false;
		asic.mmu.banks.flash2 = false;
		asic.mmu.banks.flash3 = false;

		asic.cpu.registers.PC = 0x100;

		while true {
			cpu_execute(&asic.cpu, 10000);
		}
	}

	func testAll() {
		zex("zexall", "com")
	}

	func testDoc() {
		zex("zexdoc", "com")
	}

}
