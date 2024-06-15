import XCTest
import XCTz80e
import z80e

final class PerformanceTests: XCTestCaseTI83p {

	func test_instructions() {
		measure {
			var i: CInt = 0
			while i < 1_000_000 {
				i -= cpu_execute(&asic.cpu, 1);
				i += 1
			}
		}
	}


}
