# Notes

My current goals are the following:

- Simplify the emulator structure
- Improve documentation
- Improve interop capabilities
- Improve performance

Here are some non-goal:

- Be super portable, I'm assuming the host machine is one of macOS, Linux or Windows.
- Keep compatibility with the existing toolchain, I'm doing this to learn more and the best way is to hack through

## Tasks

- Split the asic into components that communicate via hooks
	- [] scheduler (+runloop, +clock_rate, +timers)
		- find way to hook cpu and devices (link)
	- [] mmu (+mapping)
		- modularization
	- [] cpu (+interrupts)
		- find way to hook mmu
	- [x] lcd
	- [x] keyboard
	- [] link (+socket)
	- [x] battery (+battery_state, +battery_remove_check)
- Refactor `mmu` and `mapping` into spans that forward a `read`/`write` interface?
- Refactor `ti_device_type` into functions that assemble and configure the components
- Debugger proper breakpoints
- Debugger command aliases & help grouping
