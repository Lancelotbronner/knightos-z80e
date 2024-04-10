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

- Inline the registers struct into the CPU, making the functions take a CPU or asic pointer.
- Rework the z80 CPU and execution: the `asic` should be the top-level, so `asic_execute(calculator, 23)`
- The `ExecutionContext` can be renamed `z80_cycle_t` and would contain the `const asic_t *chip`, current opcode and timings (executed cycle count).
- Devices should also receive an `asic_t` pointer
