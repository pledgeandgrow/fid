# runtime_arm64.fd

> **Re-verified 2026-10-23:** confirmed — assembler Err still returns silent empty `RuntimeParts` (:139-141, Rust panics) → link proceeds with no runtime; caps still unwired.

- **Path:** `compiler/runtime_arm64.fd`
- **LOC:** 794 (Rust `src/runtime_arm64.rs`: 961)
- **Role:** ARM64 Linux runtime — unlike `runtime_linux.fd`'s hand-encoded bytes, this module **builds assembly text** (`build_runtime_asm_arm64`) and assembles it through `arm64_enc::assemble_arm64` (`runtime_arm64.fd:113-118`). Produces `RuntimeParts` (text + symbols + rodata + rodata_syms) plus a reloc list for the linker.

## Where called

| Caller | Line | Mechanism |
|---|---|---|
| `compiler/compiler.fd` | 51 | `use compiler::runtime_arm64` |
| `compiler/compiler.fd` | 1770 | `runtime_parts_arm64()` — ARM64/aarch64 `build` path |
| `compiler/compiler.fd` | 1771 | `runtime_relocations_arm64()` — global set inside `runtime_parts_impl_arm64` |

Pipeline: `codegen_arm64` → `arm64_enc` → `runtime_arm64` → `linker`. Also `arm64_enc.fd` itself was analyzed in run 2.

## Findings

### MEDIUM — assembler failure is silent where Rust panics

On `assemble_arm64` error, Fides returns **empty `RuntimeParts`** (`:135-138`, comment: "defensive"). Rust `src/runtime_arm64.rs:94-97` `eprintln!`s and **panics**. Empty parts mean the link proceeds with no `_start`, no `p_print`, no allocator — every extern reloc resolves to nothing (same `rel32=0` fall-through as the linker finding in `runtime_linux.md`). A broken assembler produces a *silently* broken binary instead of a loud build failure.

### MEDIUM — capability pipeline unwired (same as runtime_linux)

- `compiler.fd:1770` calls bare `runtime_parts_arm64()` → `has_caps=false`; `runtime_parts_with_caps_arm64` (:37) has **no callers**. Rust calls the caps variant when `granted_caps` is `Some` (`src/compiler.rs:1811`).
- Compounding it, `cap_granted_arm64` (:58-59) is a **bitfield stub**: `caps_set == 0 → false; nonzero → true` — *any* nonzero set grants every cap, vs Rust's per-cap `caps.contains(cap_name)` (`:847`). Even if the caps variant were wired, `FS`-only grants would also unlock Net.

### LOW — stale relocations on the error path

`g_runtime_relocations_arm64` is only cleared/refilled inside the `Ok` arm (:128). On `Err` the function returns early, leaving whatever the previous call stored — a subsequent link could consume stale relocs. (Rust's panic makes this unreachable there.)

### Verified faithful (inspection)

- **All 34 Rust asm functions present** — symbol-label diff is clean; the only FD-only labels are `.L` locals (`p_empty`, `p_loop`, etc.).
- **Syscall numbers match**: `x8=#93` exit, `#64` write, `#222` mmap — same as Rust and correct for arm64 Linux.
- **`_start` sequence**: `mov x8, #93 / svc #0` tail — matches Rust.
- **Stub coverage identical**: the three cap-gated arrays (16 Net, 7 FS, ~21 Spawn/thread/chan/task) match Rust's lists name-for-name (:707+ vs `:830-843`), plus `pledge_*` math stubs (real name in both — `pledge_strlen`/`pledge_strcmp` are real impls; sqrt/floor/ceil/pow/sin/cos/fabs are `mov x0,#0; ret` stubs both sides).
- rodata string table (`.Lpf_zero_char`, `.Lpanic_prefix`, `.Lpanic_nl`) + `push_str_bytes_arm64` — 1:1.
- Reloc type codes (BL/B internal codes) forwarded to the linker unchanged.

## Suggested fix order

1. Turn the `Err` arm into a hard failure (panic/Err propagation) — matches Rust and prevents silent empty-runtime links.
2. When `build_executable` gains `granted_caps` (the runtime_linux fix), call `runtime_parts_with_caps_arm64` here too — and replace the `caps_set != 0` bitfield stub with real per-cap string checks.
3. Clear `g_runtime_relocations_arm64` before the match (defensive hygiene).

## Verification

Inspection-only — no self-hosted binary in this environment. Syscall numbers, `_start`, symbol-label coverage, stub lists, and cap-gating logic verified by direct diff against `src/runtime_arm64.rs`; assembler error-path behavior confirmed by reading both match arms. Note this module's output correctness ultimately depends on `arm64_enc.fd` (run 2), which has its own verified encoder defects — an assembled instruction being wrong would surface here even with this file correct.
