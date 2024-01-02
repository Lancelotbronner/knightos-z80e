// swift-tools-version: 5.8
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
	name: "z80e",
	products: [
		.library(name: "z80e", targets: ["z80e"]),
	],
	dependencies: [
		.package(name: "scas", path: "../scas"),
	],
	targets: [
		
		.target(
			name: "z80e",
			dependencies: [
				.product(name: "ScasKit", package: "scas")
			]),
		
		.testTarget(
			name: "z80eTests",
			dependencies: ["z80e"],
			path: "Tests/z80e"),
		
		.executableTarget(
			name: "tui",
			dependencies: ["z80e"]),
	],
	cLanguageStandard: .c2x
)
