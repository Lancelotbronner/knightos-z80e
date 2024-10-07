#pragma once

#include <z80e/debugging/command.h>

// CLI Commands
const struct debugger_command ListCommand;
const struct debugger_command HelpCommand, HelpCommand1;

// Debugging Commands
const struct debugger_command BreakCommand;
const struct debugger_command RunCommand;
const struct debugger_command StepCommand;
const struct debugger_command StepOverCommand;
const struct debugger_command StopCommand;

// Print Commands
const struct debugger_command PrintCommand;

// Disassembly Commands
const struct debugger_command DisassembleCommand;

// Debugger Commands
const struct debugger_command SourceCommand;
const struct debugger_command SetCommand;
const struct debugger_command UnsetCommand;
const struct debugger_command OnCommand;

// z80 Commands
const struct debugger_command LoadCommand;
const struct debugger_command DumpRegisterCommand;
const struct debugger_command DumpStackCommand;

// Interrupt Commands
const struct debugger_command UnhaltCommand;
const struct debugger_command PowerCommand;
const struct debugger_command TickCommand;

// Memory Commands
const struct debugger_command DumpMappingsCommand;
const struct debugger_command HexdumpCommand;
const struct debugger_command HexdumpBackwardsCommand;

// Port Commands
const struct debugger_command InCommand;
const struct debugger_command OutCommand;

// Keyboard Commands
const struct debugger_command PressCommand;
const struct debugger_command ReleaseCommand;
const struct debugger_command TapCommand;

// Screen Commands
const struct debugger_command DumpScreenCommand;

// Link Commands
const struct debugger_command SendCommand;
const struct debugger_command ReceiveCommand;
const struct debugger_command ConnectCommand;
const struct debugger_command DumpLinkCommand;

// Timer Commands
const struct debugger_command TimerCommand;
