//
//  Testing+.swift
//  z80e
//
//  Created by Christophe Bronner on 2025-01-06.
//

import Testing

public extension Tag {
	@Tag static var alu: Self
	@Tag static var arithmetic: Self
	@Tag static var block: Self
	@Tag static var control: Self
	@Tag static var debugger: Self
	@Tag static var device: Self
	@Tag static var disassembler: Self
	@Tag static var index: Self
	@Tag static var interrupt: Self
	@Tag static var io: Self
	@Tag static var load: Self
	@Tag static var performance: Self
	@Tag static var shift: Self
}
