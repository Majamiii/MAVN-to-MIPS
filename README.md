# MAVNTranslator

A translator (compiler) that converts programs written in **MAVN** (MIPS Asembler Visokog Nivoa — "High-Level MIPS Assembler") into standard MIPS 32-bit assembly code, executable in the QtSpim simulator.

Developed as a project for the course *Osnove paralelnog programiranja i softverski alati* (Fundamentals of Parallel Programming and Software Tools), Faculty of Technical Sciences, University of Novi Sad.

## Overview

MIPS is a RISC architecture with a limited number of general-purpose registers. Writing MIPS assembly directly requires the programmer to manually track which registers are in use and what they hold — a process that's both tedious and error-prone.

MAVN solves this by introducing **register variables**: symbolic names that stand in for physical registers. The programmer writes code using declared variables instead of real hardware resources, and the translator takes care of mapping an arbitrary number of variables onto the limited set of physical registers (`$t0`–`$t3`).

## Features

- Full compiler pipeline: lexical analysis → syntax analysis → liveness analysis → register allocation → MIPS code generation
- Register allocation via **interference graph coloring** (simplification, spill detection, coloring)
- Detects and reports lexical and syntax errors
- Detects **spill** situations (when the number of live variables exceeds the available registers) and halts translation
- Supports the 10 base MAVN instructions plus 3 additional instructions:
  - `AND rd, rs, rt` — bitwise AND (`rd = rs & rt`)
  - `SLT rd, rs, rt` — set less than (`rd = 1` if `rs < rt`, else `0`)
  - `JR rs` — unconditional jump to the address in register `rs`
- Generates a valid `.s` (MIPS) output file with `.data` and `.text` sections, ready to run in QtSpim

## How It Works

The translator processes input through five sequential phases:

1. **Lexical Analysis** — Implemented as a finite state machine (FSM). Tokenizes the input and validates declarations (`_mem`, `_reg`, `_func`).
2. **Syntax Analysis** — Parses the token stream against the MAVN grammar, while recording which registers each instruction defines (`dst`/`def`) and uses (`src`/`use`). Also builds the control-flow graph (`pred`/`succ` links), with special handling for branch instructions (`B`, `BLTZ`).
3. **Liveness Analysis** — A backward data-flow analysis that computes, for every instruction, the sets of variables live on entry (`in`) and exit (`out`). This determines which variables can safely share a register.
4. **Register Allocation** — Builds an interference graph (two variables interfere if their live ranges overlap) and colors it:
   - **Simplification**: nodes with degree lower than the number of available registers are pushed onto a stack.
   - **Spill**: if no such node exists, the graph cannot be colored with the given number of registers, and translation is aborted.
   - **Coloring**: nodes are popped from the stack and assigned a register (color) not used by their neighbors.
5. **MIPS Code Generation** — Emits a `.globl`, `.data` (memory variable declarations), and `.text` (translated instructions and labels) section.

## Project Structure

```
MAVNTranslator/
├── LexicalAnalysis/    # Lexical analysis (FSM)
├── SyntaxAnalysis/     # Syntax analysis (parser)
├── LivenessAnalysis/   # Liveness analysis
├── ResourceAllocation/ # Register allocation / interference graph
├── MIPSGeneration/     # MIPS output file generation
├── examples/           # Sample .mavn test programs
└── main.cpp            # Entry point
```

## Getting Started

### Requirements

- Visual Studio (Windows SDK 10.0)
- C++ compiler/debugger with ISO C++14 support

### Running the Project

1. Place your `.mavn` file in the `examples/` folder.
2. In `main.cpp`, set the path to the input file:
   ```cpp
   string fileName = ".\\..\\examples\\simple.mavn";
   ```
3. Build and run the project in Visual Studio.

If a translation error occurs, it's printed to the console along with the progress through each compilation phase. On success, the translated MIPS code is printed to the console and saved alongside the input file with a `.s` extension.

## Example

**Input (`simple.mavn`):**
```asm
_mem m1 6;
_mem m2 5;

_reg r1;
_reg r2;
_reg r3;
_reg r4;
_reg r5;

_func main;
    la   r4, m1;
    lw   r1, 0(r4);
    la   r5, m2;
    lw   r2, 0(r5);
    add  r3, r1, r2;
```

**Output (`simple.s`):**
```asm
.globl main

.data
m1: .word 6
m2: .word 5

.text
main:
    la   $t0, m1
    lw   $t1, 0($t0)
    la   $t0, m2
    lw   $t0, 0($t0)
    add  $t0, $t1, $t0
```

Note that `r2` and `r3` share register `$t0` since their live ranges don't overlap — the translator allocates only 2 physical registers for 5 declared variables.

## Testing

The translator was verified against four test programs covering different aspects of the system:

| Test | Focus |
|------|-------|
| `simple.mavn` | Basic instructions and minimal register allocation |
| `multiply.mavn` | Larger variable set, branching, and spill detection (4 vs. 5 available registers) |
| `commands.mavn` | The three added instructions: `AND`, `SLT`, `JR` |
| `test.mavn` | Complex branching with multiple labels (8 variables allocated into 4 registers) |

All generated outputs were validated by executing them in the **QtSpim** simulator, confirming that the translated code's semantics match the original source behavior.

## Possible Future Improvements

- **Memory spilling**: automatically move variables to memory when a spill is detected, removing the current hard limit on the number of variables
- Extend instruction support to cover the full MIPS32 instruction set
- Code-level optimizations (e.g. dead code elimination)

## Author

**Maja Milović** — RA69/2024
Faculty of Technical Sciences, University of Novi Sad
Mentor: dr Teodora Novković