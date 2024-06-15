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

- Devices could receive an `asic_t` pointer? Starting to doubt its a good idea
- Review the mapping device and its relationship to the MMU
- Investigate `cpu.prefix` and if it could be replaced by storing the opcodes into the `ExecutionContext` 
