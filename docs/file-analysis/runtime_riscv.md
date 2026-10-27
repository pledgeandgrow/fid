# runtime_riscv.fd

> **Re-verified 2026-10-23:** confirmed — assembler Err still returns silent empty `RuntimeParts` (:116-ish, same pattern as runtime_arm64); caps unwired; no `p_string_*`/`p_map_*` (inherited).

- **Path:** `compiler/runtime_riscv.fd`
- **LOC:** 825 (Rust `src/runtime_riscv.rs`: 868)
- **Role:** RISC-V Linux runtime — same architecture as `runtime_arm64.fd`: builds assembly text (`build_runtime_asm_riscv`), assembles through `riscv_enc::assemble_riscv` (:116), produces `RuntimeParts` + a reloc list. Effectively the ARM64 runtime re-mnemonicked for RV64.

## Where called

| Caller | Line | Mechanism |
|---|---|---|
| `compiler/compiler.fd` | 54 | `use compiler::runtime_riscv` |
| `compiler/compiler.fd` | 1835 | `runtime_parts_riscv()` — `--target riscv`/`riscv64` build path |
| `compiler/compiler.fd` | 1836 | `runtime_relocations_riscv()` — global set inside `runtime_parts_impl_riscv` |

`riscv_enc.fd` (run 42) is the assembler this file depends on — verified clean there.

## Findings

### MEDIUM — assembler failure silently returns empty parts

Identical to `runtime_arm64.fd`: the `Err` arm (:135-138) returns empty `RuntimeParts` "defensively" while Rust `panic!`s (`src/runtime_riscv.rs` — same `match` on `assemble`, panic on error). A broken assembler → a link with no `_start`, no syscalls, no builtins — and every extern reloc falls through (the `rel32=0` linker gap from `runtime_linux.md` applies on this path too).

### MEDIUM — capability pipeline unwired + bitfield `cap_granted`

- `compiler.fd:1835` calls bare `runtime_parts_riscv()` → `has_caps=false`; `runtime_parts_with_caps_riscv` (:35) is dead code. Rust's arm64/riscv calls route through `runtime_parts_with_caps` when caps exist.
- `cap_granted_riscv` (:56-59) is the same `caps_set != 0 → all granted` bitfield approximation — no per-cap checks even if wired.

### LOW — stale relocations global on the error path

`g_runtime_relocations_riscv` cleared/refilled only inside `Ok` (:128). On `Err`, early return leaves the previous call's relocs — same pattern as arm64.

### Inherited (identical in Rust — not port regressions)

- **No `p_string_*`/`p_map_*`/`p_strlen`/`p_strcmp` at all.** The RISC-V runtime defines only `p_str_concat`, 10 `p_vec_*` basics, `p_vec_to_str`, and `pledge_strlen`/`pledge_strcmp` — symbol set verified 1:1 against Rust's asm labels. String/map builtins on `--target riscv` fail identically under the reference compiler.
- **`_start` prologue quirk**: `sd ra,8(sp) / sd s0,0(sp)` *before* `addi sp,sp,-16` — frame allocated after the saves. Verbatim in Rust (`:119-121`); harmless at `_start` (nothing below sp to clobber, never returns).
- **`call main` is a single JAL** — the ±1 MiB range limit documented in `riscv_enc.md` (run 42) applies; same in Rust.
- `ebreak` sentinels after `ecall` exits — same both sides.

### Verified faithful (inspection)

- **Syscall numbers**: `a7=93` exit, `a7=64` write, `a7=222` mmap — correct RV64 Linux.
- **Stub coverage**: 43 `append_cap_stub_riscv` entries + `pledge_*` zero-stubs match Rust's three arrays (16 Net / 7 FS / 20 spawn-thread-chan) name-for-name.
- **35-function coverage 1:1** — no dropped blocks (unlike `runtime_linux.fd`'s missing `runtime_asm`).
- Reloc forwarding, symbol conversion (`RuntimeSymbol_new(name, offset, 0)`), rodata table — all match the arm64/linux conventions.

## Suggested fix order

1. `Err` arm → hard failure (panic), matching Rust — same fix as `runtime_arm64.fd`.
2. Wire `granted_caps` through `build_executable` → `runtime_parts_with_caps_riscv` + replace bitfield `cap_granted` with per-cap checks (same fix arm64 needs).
3. If string/map builtins are wanted on RISC-V, that's a *feature gap shared with Rust* — not a port bug; file it separately.

## Verification

Inspection-only — no self-hosted binary. Direct source diff vs `src/runtime_riscv.rs`; syscall numbers cross-checked against the RV64 Linux table; symbol coverage diffed both directions (FD-only labels were `.L` locals). Correctness of emitted bytes ultimately rides on `riscv_enc.fd` (run 42 — verified clean) rather than this file.
