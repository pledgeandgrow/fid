# disasm.fd — disassembler of the machine layer's own output (`fid disasm`)

> **New doc 2026-11-21.** Read from the source; the status is the Nov 21
> check of llvm-roadmap TG5.

- **Path:** `compiler/disasm.fd`
- **LOC:** 2,563 (2026-11-21). In git since Nov 19 (49fb2b9).
- **Role:** llvm-roadmap TG5. Reads back what nat_enc.fd writes on
  x86-64, AArch64 and RISC-V 64. Every encoding form is a row of one table
  (`ef_init`; fixed bits and an operand template, `ef_id`, `ef_base`,
  `ef_mask`, `ef_template`), which the decoders match (`ef_decode_word`).
  Bytes no row has are shown as `.word` / `.byte`. `disasm_image(bytes,
  name)` reads an ELF executable (its `.symtab`) or a PE (its COFF
  symbols): every function, or the one named. `disasm_bytes` reads bare
  code at an address. Intel syntax for x86-64, the ARM and RISC-V
  assembler syntax (with the usual aliases) for the others.
- **Imports:** `core`, `vec`, `string`, `compiler::nat_enc`.

## Callers

| File | What |
|------|------|
| `main.fd` | `fid disasm <file.fd> [--target <t>] [--fn <name>]` (build in memory, then `disasm_image`); `fid disasm <executable> [--fn <name>]`; `fid disasm <file.bin> --raw --arch x86_64\|aarch64\|riscv64 [--base <addr>]` (`disasm_bytes`); a Mach-O is read by main.fd's `macho_disasm`, which calls `disasm_bytes`; `fid codegen` without `--elf/--coff/--macho` prints the build's disassembly (`g_main_disasm`); `fid explain` shows a function's machine code through it (DB7) |

Only main.fd imports it.

## Checks

- tests/nattools/enc_table_unit.fd: each encoder of nat_enc.fd against its
  row of the table.
- scripts/disasm-check.sh: a disassembly of the suite's images against
  llvm-objdump (where each instruction starts) and back through clang's
  assembler (each line gives its own bytes again).
- scripts/enc-oracle.py: the encoders against clang's assembler.

## Status (Nov 21 check, TG5: partly)

enc_table_unit passes. disasm-check: x86-64 instruction starts agree with
objdump, the AArch64 and RISC-V round trips are all right, but the x86-64
round trip fails on 3 tests: `shl rbx, 1` emitted as `C1 /n 1` reassembles
as `D1 /n`.

## Findings

- **LOW — the header overstates the sharing.** It says the encoders' words
  come from the same table (`EFW_*`, `ef_word`, `ef_x64_rr`) "so the two
  cannot drift". The `EFW_*` constants are defined only in disasm.fd and
  nat_enc.fd uses none of them: the encoders keep their own words, and
  only enc_table_unit ties the two together.
- **LOW:** the x86-64 round trip above (emit `D1` for a shift by 1, or
  accept the alternative form).
