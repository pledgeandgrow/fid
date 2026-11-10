# riscv_enc.fd — RISC-V text assembler of the PIR path

Re-audited 2026-11-09 (previous audit 2026-10-22/23).

- **Path:** `compiler/riscv_enc.fd`
- **LOC:** 1,191
- **Role:** RISC-V asm text → bytes, symbols, relocations
  (`assemble_riscv`): I/S/B/J encodings, `li` expansion, directives,
  local branch resolution. Port of `src/riscv_enc.rs`. Unrelated to the
  `rv_*` encoders in `nat_enc.fd` used by the working native backend.

## Where it is called

| Caller | What |
|--------|------|
| `compiler.fd` `build_executable` (riscv branch) | `assemble_riscv(asm_rv)` |
| `runtime_riscv.fd` | `assemble_riscv(runtime asm)` |

**Unreachable from the CLI**: `build --native --target riscv*` goes to
nat_ir; `codegen --target riscv --elf` uses elf64's empty-object path.

## Findings

All code inspection.

1. **LOW — non-numeric `x` registers and `.quad`/`.byte` operands become
   0** — not re-checked.
2. **Inherited — `call` is one `JAL` (±1 MB)** — not re-checked.
3. **Inherited — instructions in `.rodata` vanish — STILL TRUE**
   (:415 returns Ok).
4. **INFO (new) — no rodata in the result.** `RiscVAssembleResult`
   (:79-83) has text, symbols, relocations only; `build_executable`
   passes empty rodata to the linker. Strings in RISC-V asm would have no
   home.
5. Bit-level encodings were verified in the original audit; not redone.

## Fix order

None while unreachable.

## Verification state

- Read: result struct, section handling. No probe (unreachable).
