//
//  File.swift
//  
//
//  Created by Christophe Bronner on 2024-04-07.
//

import z80e

open class XCTestCaseTi73: XCTestCaseAsic {
	public final override var _type: ti_device_type { TI73 }
}

open class XCTestCaseTI83p: XCTestCaseAsic {
	public final override var _type: ti_device_type { TI83p }
}

open class XCTestCaseTI83pSE: XCTestCaseAsic {
	public final override var _type: ti_device_type { TI83pSE }
}

open class XCTestCaseTI84p: XCTestCaseAsic {
	public final override var _type: ti_device_type { TI84p }
}

open class XCTestCaseTI84pSE: XCTestCaseAsic {
	public final override var _type: ti_device_type { TI84pSE }
}

open class XCTestCaseTI84pCSE: XCTestCaseAsic {
	public final override var _type: ti_device_type { TI84pCSE }
}
