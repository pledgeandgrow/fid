# riscv_enc.fd — RISC-V text assembler/encoder

> **Re-verified 2026-10-23:** findings consistent — LOW latent str_parse_int→0 paths and inherited .rodata-drop still present. Bit-level encodings were verified in the original pass.

- **Path:** `compiler/riscv_enc.fd` — **1,210 LOC** (Rust `src/riscv_enc.rs` = 933, ~130%)
- **Role:** assembles RISC-V asm text into machine code — `RiscvAssembler_encode_instruction` per mnemonic, label/branch resolution, `.rodata` section, `.quad`/`.asciz`/`.align` directives, reloc emission for the linker.

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `compiler.fd` | :1808 | `assemble_riscv(asm_rv)` — `--target riscv/riscv64` build path (codegen_riscv → riscv_enc → runtime_riscv → linker) |
| `runtime_riscv.fd` | :116 | `assemble_riscv` — assembling the RISC-V runtime stubs |

Live on the riscv64 target only.

## Verified faithful — encodings checked by hand

- **Field encodings correct**: I-type `(imm12<<20|rs1<<15|f3<<12|rd<<7|op)`, S-type `(imm11_5<<25|rs2<<20|rs1<<15|f3<<12|imm4_0<<7|op)`, shift-immediate with `funct7=0x20` for SRAI — all match spec and Rust.
- **B-type patch mask `0x01FFF07F`** verified — keeps [24:12] (rs2/rs1/funct3) + [6:0] (opcode), patches imm12@31, imm10_5@30:25, imm4_1@11:8, imm11@7.
- **J-type scatter** correct: imm20@31, imm10_1@30:21, imm11@20, imm19_12@19:12; sign bits propagate through arithmetic shifts (negative backward offsets OK).
- **Mnemonic coverage 55/55 identical** to Rust's dispatch (extracted both sets, diff empty); all 32 ABI register names (`zero`/`ra`/`sp`/`gp`/`tp`/`t0-6`/`s0-11`/`a0-7`/`fp`) + `x0`-`x31` range check.
- `li` LUI+ADDI expansion with `hi = (imm + 0x800) >> 12` rounding — correct for negatives too (verified: `li x, -5000` → `lui 0xFFFFF` + `addi 0xC78` = -4096 + -904). Rust's two-branch split is equivalent code.
- Directives: `.section`/`.globl`/`.asciz`/`.ascii`/`.quad`/`.byte`/`.align`; `.align` pads text with real NOPs (`0x00000013`), rodata with zeros.
- `resolve_branches`: local labels patched in-place, unknown symbols → `relocations` for the linker.

## Findings

### LOW — non-numeric `x`-register and `.quad`/`.byte` operands silently become 0 *(latent)*

`parse_reg` calls `str_parse_int` on `x<suffix>` — `str_parse_int` returns 0 on non-digits → `"xabc"` encodes as `x0`; Rust's `s[1..].parse()` errors. Same for `.quad sym`/`.byte sym` — numeric-only parse, non-numeric → 0 where Rust errors at :188. Unreachable from `codegen_riscv` (emits only valid reg names + `int_to_str(bits)`), but hides malformed-asm bugs.

### Inherited — `call` is a single `JAL ra` (±1MB range)

`enc_call` → `JAL ra, 0` + reloc — documented same-as-Rust limitation ("use JAL for now (20-bit range = ±1MB)" at :523). Real `call` should be `auipc+jalr` for ±4GB; both sides take the shortcut. Fails only if a function call exceeds ±1MB of code — not hit in practice.

### Inherited — `.rodata` silently drops instructions

`handle_instruction` returns `Ok` without emitting when `current_section == Rodata` (:434) — an instruction placed in `.rodata` vanishes quietly. Same in Rust.

## Fix order

1. (Optional) make `str_parse_int` consumers error on non-digits, or add a digit check in `parse_reg`/`.quad` — tightens malformed-asm handling.

## Verification state

- No live runs (no riscv64 target available + no self-hosted binary). Audit was bit-level: every encode function's field math checked against the spec, the B/J patch masks computed by hand, mnemonic dispatch extracted and diffed 55/55, register table compared 1:1, and the LUI+ADDI negative case worked through numerically.
