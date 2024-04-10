// swift-tools-version: 5.8
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
	name: "z80e",
	products: [
		.library(name: "z80e", targets: ["z80e"]),
		.library(name: "z80ejs", targets: ["z80ejs"]),
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

		.target(
			name: "z80ejs",
			dependencies: ["z80e"]),

		.executableTarget(
			name: "z80e-cli",
			dependencies: ["z80e"],
			path: "Sources/tui"),

		.executableTarget(
			name: "z80e-gui",
			dependencies: ["z80e"],
			path: "Sources/gui"),

		.testTarget(
			name: "z80eTests",
			dependencies: ["z80e"],
			path: "Tests/z80e",
			resources: [
				.copy("zex")
			]),
	],
	cLanguageStandard: .c2x
)
