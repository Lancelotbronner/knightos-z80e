// swift-tools-version: 6.0
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
	name: "z80e",
	products: [
		.library(name: "libz80e", targets: ["libz80e"]),
		.library(name: "libz80ejs", targets: ["libz80ejs"]),
		.library(name: "XCTz80e", targets: ["XCTz80e"]),
	],
	dependencies: [
		.package(url: "https://github.com/Lancelotbronner/scas", revision: "f333a1d3a6c16bb7d5c40d920edba31399294874"),
	],
	targets: [
		.target(
			name: "libz80e",
			dependencies: [
				.product(name: "ScasKit", package: "scas")
			]),

		.target(
			 name: "libz80ejs",
			 dependencies: ["libz80e"]),

		// Applications

		.target(
			name: "z80e",
			dependencies: ["libz80e"]),

		.executableTarget(
			name: "z80e-tui",
			dependencies: ["z80e"]),

		.systemLibrary(
			name: "libsdl2",
			pkgConfig: "sdl2",
			providers: [
				.brew(["sdl2"]),
			]),

		.executableTarget(
			name: "z80e-gui",
			dependencies: ["z80e", "libsdl2"],
			cSettings: [
				.unsafeFlags([
					"-I/opt/homebrew/include",
				]),
			]),

		// Testing

		.target(
			name: "XCTz80e",
			dependencies: ["z80e"],
			path: "Tests/xct"),

		.testTarget(
			name: "z80eTests",
			dependencies: ["XCTz80e"],
			path: "Tests/z80e"),

		.testTarget(
			name: "z80eInstructionSetExerciser",
			dependencies: ["XCTz80e"],
			path: "Tests/zex",
			resources: [
				.copy("zexall.com"),
				.copy("zexall.src"),
				.copy("zexdoc.com"),
				.copy("zexdoc.src"),
			]),
	],
	cLanguageStandard: .c2x
)
