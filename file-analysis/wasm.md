# wasm.fd

> **Re-verified 2026-10-23:** confirmed — DynCall/CallPtr still emit silent `0x41`/`i32.const 0` placeholders (:1163/:1169); `--wat` still a section dump; StructCopy still dropped (still unreachable anyway).

- **Path:** `compiler/wasm.fd`
- **LOC:** 1,656 (Rust `src/wasm.rs`: 1,277)
- **Role:** WASM/WAT backend — `IrModule` → WASM binary (or `--wat` debug dump). Custom `WasmBuf` byte emitter, full module layout, host-import table, block-dispatch control-flow scheme (`cur_block` local + `br $dispatch`).

## Where called

| Caller | Line | Mechanism |
|---|---|---|
| `compiler/main.fd` | 29, 150 | `use` + `wasm` command → `cmd_wasm` (:1285) |
| `compiler/compiler.fd` | 55 | `use compiler::wasm` |
| `compiler/compiler.fd` | 1481 | `compile_to_wasm` — build_program → `lower` → `generate_wasm`/`dump_wat` |

Live: `fides-c wasm file.fd [-o out.wasm] [--wat]`. Pipeline identical to Rust's `compile_to_wasm` (`src/compiler.rs:1691`).

## Findings

### LOW — `--wat` is a section-summary dump, not WAT

`dump_wat` (:1586) generates the binary then renders `; magic:`, `; section N (name) size=M`, `; Functions:` — a *structural listing*, not WAT text. `cmd_wasm` writes it to `.wat` anyway ("Emitted WAT text" message overstates). **Inherited verbatim** — Rust's `dump_wat` (:1213) is the same byte-dump. A `.wat` output file can't be fed to `wat2wasm`. Cosmetic, same both sides.

### LOW (inherited) — several Inst arms are stubs/no-ops; call defaults are silent

Both sides identically:

- `MakeDyn`/`DynCall`/`CallPtr` → `i32.const 0` / default-typed 0 (:1162-1183 == Rust :834-863) — dynamic dispatch and indirect calls unsupported, silently returning 0.
- Unknown callee → typed-0 default (:1150-1158 == Rust :821-830). The header comment at :20 claims `p_vec_*`/`p_map_*`/`p_string_*` emit `unreachable` — **they don't**; they hit the unknown-callee arm and emit `0`. Doc-vs-code mismatch, verbatim from Rust.
- `TxnBegin/End`, `UnsafeBegin/End`, `GradBegin/End`, `Backward`, `RegionBegin/End` → no-ops / const-0 (:1275-1300 == Rust :963).
- `StructCopy` → Fides drops it (`{}` + TODO :1282); Rust at least emits `memory.copy` with a placeholder size-0 (:966-972) — still wrong-but-present vs absent. Given `lower.fd` never emits `StructCopy` anyway (run-40 finding), unreachable in practice.
- `Asm` → const-0 / drop (:1291+) — inline asm unsupported in WASM, correct direction.

### Verified faithful (the parts that matter)

- **Inst-arm coverage is a superset**: 45 arms vs Rust's 43 — adds `None`/`Some` handling, zero missing.
- **Module layout 1:1**: Type(1) → Import(2) → Function(3) → Memory(5) → Global(6) → Export(7) → Code(10) → Data(11) — spec-correct order, same `section()` helper shape.
- **Host imports**: all 16 names identical (`print`, `println`, `eprint*`, `p_print_*`, `p_panic`, `p_malloc`, `p_alloc`, `p_free`, `p_str_concat`, `p_to_str*`) — env-module imports with matching param types.
- **Call lowering**: host-import index → `call $hi` with i32↔i64 extension (`0xAC`/`0xA7` opcodes) matching signatures; user-fn index → `call $fidx` — 1:1.
- **Control flow**: `Jump`/`Branch` → `cur_block` local + `br $dispatch` state-machine; `Return` with i32↔i64 fix-up; `Unreachable` → `0x00`; `Panic` → intern string + `p_panic` import + `unreachable`.
- **Memory ops**: `Alloca`/`Load`/`Store`/`GetFieldPtr`/`GetElemPtr` (with elem-size scale) all present.
- **`compile_to_wasm`**: `build_program_with_path` → `lower` → `generate_wasm` (or `dump_wat`→bytes) — same as Rust's :1691-1697; `cmd_wasm` arg parsing (`-o`, `--wat`, default stem derivation) faithful.

## Suggested fix order

1. Nothing load-bearing is broken — this file is a competent port of a deliberately-minimal backend. The honest improvements: emit real `unreachable` for `p_vec_*`/`p_map_*`/`p_string_*` (matching the header's claim) instead of silent 0s, or fix the comment.
2. If WAT output matters, emit real text format instead of the byte-dump.
3. `memory.copy` for `StructCopy` is a one-line addition when/if lower ever emits it.

## Verification

Inspection-only — no self-hosted binary. Verified by Inst-arm diff (superset, 2 extra), host-import diff (16/16 identical), section-order check, and direct arm-by-arm read of calls/conversions/control-flow vs `src/wasm.rs`. The gap list matches Rust's own TODO/no-op set almost exactly — this backend's limitations are upstream, not port-induced.
