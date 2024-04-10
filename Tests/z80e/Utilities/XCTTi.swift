//
//  File.swift
//  
//
//  Created by Christophe Bronner on 2024-04-07.
//

import z80e

final class XCTTestCaseTi73: AsicTestCase {
	override var _type: ti_device_type { TI73 }
}

final class XCTTestCaseTI83p: AsicTestCase {
	override var _type: ti_device_type { TI83p }
}

final class XCTTestCaseTI83pSE: AsicTestCase {
	override var _type: ti_device_type { TI83pSE }
}

final class XCTTestCaseTI84p: AsicTestCase {
	override var _type: ti_device_type { TI84p }
}

final class XCTTestCaseTI84pSE: AsicTestCase {
	override var _type: ti_device_type { TI84pSE }
}

final class XCTTestCaseTI84pCSE: AsicTestCase {
	override var _type: ti_device_type { TI84pCSE }
}
