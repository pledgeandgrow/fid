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
  row of the table (since Nov 27 also the words nat_ir.fd writes itself:
  the SIMD&FP loads, stores and pairs).
- tests/tooling/disasm-forms-check.sh: `fid disasm --raw` on byte sequences
  no suite program reaches (BG233, BG208, BG232), the bytes llvm-mc's.
- scripts/disasm-check.sh: a disassembly of the suite's images against
  llvm-objdump (where each instruction starts) and back through clang's
  assembler (each line gives its own bytes again).
- scripts/enc-oracle.py: the encoders against clang's assembler.

## Status (Nov 21 check, TG5: partly)

enc_table_unit passes. disasm-check: x86-64 instruction starts agree with
objdump, the AArch64 and RISC-V round trips are all right, but the x86-64
round trip fails on 3 tests: `shl rbx, 1` emitted as `C1 /n 1` reassembles
as `D1 /n`.

Nov 27 (night), BG232 (bugs-roadmap.md): the encoders that had no row have
one, each listed in enc_table_unit: x86 `blsmsk` and `blsi`; AArch64 the
pre/post-index loads and stores (24 rows; a signed 9-bit offset field,
`simm9`), the SIMD&FP `ldr`/`str` d and s and `ldp`/`stp` d, `bic v.16b`,
`dup v.2d, v.d[i]` and `mov d, v.d[i]`; RISC-V `vsll.vi`, `vmseq.vi`,
`vmerge.vim`, `vslidedown.vi`, `vfmv.f.s` and `vfmv.v.f` (two 5-bit
immediate fields, `vimm5` and `vsimm5`). With the build of 09:17:
disasm-forms-check 30 of 30, disasm-check 18 ok and 0 words undecoded on
AArch64 in test_machine_fast (8 before), enc_table_unit ok on ccompile and
emit.

## Findings

- **LOW — the header overstates the sharing.** It says the encoders' words
  come from the same table (`EFW_*`, `ef_word`, `ef_x64_rr`) "so the two
  cannot drift". The `EFW_*` constants are defined only in disasm.fd and
  nat_enc.fd uses none of them: the encoders keep their own words, and
  only enc_table_unit ties the two together.
- **LOW:** the x86-64 round trip above (emit `D1` for a shift by 1, or
  accept the alternative form).
