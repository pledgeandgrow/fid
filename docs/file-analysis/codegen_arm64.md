# codegen_arm64.fd — PIR L3 → AArch64 assembly

Re-audited 2026-11-09 (previous audit 2026-10-22/23).

- **Path:** `compiler/codegen_arm64.fd`
- **LOC:** 553
- **Role:** instruction selector of the old PIR path for AArch64: L3 →
  asm text, vregs mapped `id % 29` onto x0-x28, fixed 256-byte frame. Port
  of `src/codegen_arm64.rs`. The working AArch64 backend is now nat_ir
  (`fid emit --target aarch64-linux`).

## Where it is called

| Caller | What |
|--------|------|
| `main.fd` `cmd_codegen` | `generate_asm_arm64(l3)` for `--target arm64|aarch64` |
| `compiler.fd` `build_executable` | arm64 branch — unreachable from the CLI (`build --native --target arm64` goes to nat_ir) |

Only live command: `fid codegen f.fd --target arm64 [--elf o]`. The
`--elf` object is empty (elf64.md 2).

## Findings

### Verified live (2026-11-09, `s18.exe`)

1. **HIGH — wrong asm.** For `print("hi\n"); return 3;` `main` became
   `mov x0, x1; bl p_print_int; mov x3, #3; mov x0, x3; ... return`:
   the string argument is a stale register, the call is the int printer,
   and 550 functions end in `return`, which is not an AArch64 mnemonic.

### Code inspection (old findings re-checked)

2. **F1 Load/Store lost `Operand::Slot` — STILL TRUE** (only `AddrOfSlot`
   handles slots, :207).
3. **F2 non-VReg operands → `x9` — STILL TRUE** (:508-510).
4. **F3 `return` instead of `ret` — STILL TRUE** (:412).
5. **F4 `% 29` aliasing, F5 movz/movk stops at bit 47 — STILL TRUE**
   (:315-326).
6. **F6 fixed 256-byte frame — STILL TRUE.**
7. **F7 CallPtr `i < 7` vs `< 8` (:417, :440, :463), GlobalAddr → 0
   (:211) — STILL TRUE.**

## Fix order

Retire with the PIR path (roadmap MI17), or point `codegen --target
arm64` at nat_ir's MIR dump. Fixing this selector is not worth it.

## Verification state

- Probe: `codegen --target arm64` text and `--elf` object.
- Read: entry, gen_inst, gen_mov, gen_ret, calls, operand mapping.
