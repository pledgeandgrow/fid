# arm64_enc.fd — AArch64 text assembler of the PIR path

Re-audited 2026-11-09 (previous audit 2026-10-22, run 2).

- **Path:** `compiler/arm64_enc.fd`
- **LOC:** 1,854
- **Role:** AArch64 asm text → bytes, symbols, rodata, relocations
  (`assemble_arm64`). Port of `src/arm64_enc.rs`. Unrelated to the
  `a64_*` encoders in `nat_enc.fd` that the working native backend uses.

## Where it is called

| Caller | What |
|--------|------|
| `compiler.fd` `build_executable` (arm64 branch) | `assemble_arm64(asm_arm)` |
| `runtime_arm64.fd` | `assemble_arm64(runtime asm)` |

**Unreachable from the CLI.** `build --native --target arm64|aarch64`
goes to nat_ir, `codegen --target arm64 --elf` uses elf64's empty-object
path, and `runtime_arm64` is only called from the same dead branch.

## Findings

All code inspection (no command reaches the file).

1. **`fp` register alias — FIXED** (`parse_reg` :194-195, `lr` too).
2. **`mov xN, sp` / `mov sp, xN` → XZR — FIXED**: aliased to `add ..., #0`
   (:663-690).
3. **LSL bitfields — FIXED** (`immr << 16 | imms << 10`, :1141-1144).
4. **ASR — CHANGED, still wrong (new).** The fields are in the right place
   now, but the 64-bit immediate form uses opcode 0xD3400000 (:1182),
   which is UBFM, i.e. LSR. SBFM 64-bit is 0x93400000 (the comment says
   SBFM; the 32-bit form 0x13000000 is right). `asr x0, x1, #k` shifts in
   zeros for negative values. MEDIUM, latent.
5. **Register-offset and pre-index memory operands — FIXED** in the
   parser (`OkReg`, `OkPre`, :1725-1766). Post-index (`[sp], #16`) not
   re-checked.
6. **`.rodata` dropped — FIXED**: `Arm64AssembleResult` has `rodata` and
   `rodata_symbols` (:77-83) and compiler.fd forwards them.
7. **linker arm64 branch delta +4 — FIXED** (linker.md 4).
8. **Immediate ranges, `parse_reg` garbage, lossy directives,
   `resolve_branches` rtypes 14/15 — not re-checked.**
9. **Interpreter `&mut Vec` write-back** — `push_u32_le` / `patch_u32_le`
   now take and return Vecs by value (:139-168), so the interp issue no
   longer applies here.

## Fix order

None while unreachable. If `build_executable`'s arm64 branch is ever
revived: ASR opcode (4), then re-check 8.

## Verification state

- Read: function list, `parse_reg`, `enc_mov`, `enc_lsl`, `enc_asr`,
  memory-operand parser, result struct. No probe (unreachable).
