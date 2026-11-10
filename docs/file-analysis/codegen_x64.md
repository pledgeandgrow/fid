# codegen_x64.fd — PIR L3 → x86-64 AT&T assembly

Re-audited 2026-11-09 (previous audit 2026-10-22/23).

- **Path:** `compiler/codegen_x64.fd`
- **LOC:** 2,000
- **Role:** instruction selector of the old PIR path: `L3Module` +
  `regalloc.fd` allocation → AT&T asm (stack canary, overflow `jo`,
  division checks, recursion guard). Output goes through `peephole.fd` and
  `elf64.fd`. Port of `src/codegen_x64.rs`. Not used by `fid emit` (pe_ir)
  or the new native backend (nat_ir).

## Where it is called

| Caller | What |
|--------|------|
| `compiler.fd` `build_executable` | `generate_asm(l3)` for target `x86-64` / `x64` |
| `main.fd` `cmd_codegen` | `generate_asm(l3)` for `--target x86-64` (default) |
| `pe.fd` `generate_boot_binary` (also `generate_pe`, `generate_flat_binary`, which have no callers) | `generate_asm(l3)` |

CLI commands: `fid codegen f.fd [--peephole] [--elf|--coff|--macho o]`,
`fid emit f.fd --boot`, and `fid build --native` only with the default
target `x86-64` (`x64` and `x86_64*` go to nat_ir).

## Findings

### Verified live (2026-11-09, `s18.exe`)

1. **INFO — `fid codegen p.fd` works** (35,141 lines of asm for a 4-line
   program, core included); the COFF built from it disassembles to the
   expected prologue, canary and calls.
2. **HIGH (not this file) — `build --native` never reaches it with
   optimization on**: it panics "Vec index out of bounds" in
   `opt.fd pass_jump_thread`. With `--no-opt` it gets through codegen and
   fails at link (elf64.md 1).

### Code inspection (old findings re-checked)

3. **F1 F-4A call ABI missing — STILL TRUE.** `gen_call_abi` (:1505-1575)
   puts every argument in integer registers, no xmm arguments, result
   only from rax, no sret, no rdx half, no `%al` for varargs.
4. **F2 scratch registers in the allocatable pool — STILL TRUE.**
   `regalloc.fd physreg_allocatable` (:72-91) still offers rax, rcx, rdx,
   rsi, rdi, r8-r11; this file uses them as fixed scratch.
5. **F3 FloatToInt converts rax — STILL TRUE** (:1763-1766).
6. **F4 canary `xorq $imm64` — CHANGED.** :417, :511 unchanged; elf64 now
   encodes it as imm32 0xCAFEBABE sign-extended (seen in the COFF
   disassembly). Prologue and epilogue match, so it is a weak but working
   canary. The constant is still public.
7. **F5 stack-arg alignment after the pushes — STILL TRUE** (:1527-1540).
8. **F6 `gen_elem_addr` uses `ir_type_size` — STILL TRUE** (:1478).
9. **F7 calls drop arguments — STILL TRUE**: `gen_call_ptr` has no stack
   arguments (:1592-1599); `gen_dyn_call` loads only `args[0]` (:1650).
10. **F8 StackAlloc → `xorq` — STILL TRUE** (:653-665).
11. **F9 SIMD `movapd` on 8-aligned slots, F10 ImmF → 0 — not re-checked.**
12. **GlobalAddr → 0 — STILL TRUE** (:550-554).
13. **INFO — semantics lag the IR.** The PIR path predates ownership
    (proposal 0004), generics in `fid`, and the shared `Vec_*`; nobody
    compares its output with `fid run`.

## Fix order

The roadmap (MI17) plans to retire this path. If kept: reserve the scratch
registers (4), port the call ABI (3), then 5, 7, 8, 9, 12.

## Verification state

- Probes: `codegen` text output; `codegen --coff` disassembled in WSL;
  `build --native` with and without `--no-opt`.
- Read: entry points, `gen_inst`, StackAlloc, call generation, casts,
  dyn call; regalloc pool.
