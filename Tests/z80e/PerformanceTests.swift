import XCTest
import z80e

final class PerformanceTests: z80eTestCase {

	func test_instructions() {
		measure {
			var i: CInt = 0
			while i < 1_000_000 {
				i -= cpu_execute(&device.cpu, 1);
				i += 1
			}
		}
	}


}
