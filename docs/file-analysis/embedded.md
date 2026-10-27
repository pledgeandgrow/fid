# embedded.fd — embedded/IoT support scaffolding (511 LOC)

> **Re-verified 2026-10-23:** still dead in both implementations — zero importers on either side. Findings accurate.

## Role

"Phase 5B" embedded features: `FixedBufferAllocator` (deterministic
bump allocator), `BinarySizeOptimizer` (size report/recommendations),
`RealTimeAnalyzer` (real-time safety checks), `HalFramework` (device
registry + Fides FFI-binding generator). Port of `src/embedded.rs`
(622 LOC).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| — | — | **Nothing.** No `use compiler::embedded`; no references to any of its types outside the file. **Dead in Rust too** — `src/lib.rs:54` declares `pub mod embedded` but `embedded::` is referenced nowhere in `src/`. |

Seventh dead module — and the first that was dead *before* the port.

## Findings

### F1 — Spec-feature scaffolding, dead on both sides — HIGH (dead)

None of the four subsystems is wired to a pipeline stage, CLI flag, or
effect check. The module documents intended design (F13/embedded
targets) rather than working code.

### F2 — `RealTimeAnalyzer_status` returns `Safe(0)` for unchecked — MEDIUM (latent)

`:264-276` — looking up a function that was never analyzed returns
`Safe(0)` instead of an "unverified" state — a caller iterating a
program would see **unverified functions reported as real-time-safe**.
Should be a third variant (`Unknown`/`Unchecked`).

### F3 — `BinarySizeOptimizer_analyze` is a stub in disguise — MEDIUM

`:155-165` — `total_size = binary.len()`, `text_size = binary.len()`,
data/bss/debug hard-zeroed: it counts the *entire* binary as .text
with no section parsing — the "report" is `len()` with extra steps.
`recommendations` emits generic advice strings regardless of content.

### F4 — `FixedBufferAllocator` is a simulation, not an allocator — LOW (by design)

It manages a `Vec<i64>` of "bytes" — offsets, not real pointers —
fine as a model, but `alloc` returning raw offsets means generated
embedded code couldn't use it for actual storage; `free` removes the
record without reclaiming (documented, still a leak-by-design).

### F5 — `HalFramework_generate_bindings` — latent issues — LOW

The generator is the most substantive piece (emits
`fn dev_reg_read() -> u32 !Read { return p_phys_read32(0xADDR); }`
per register — `p_phys_read*` builtins **do exist** in codegen_c
:756-763, so emitted code could compile). But:
- `WriteToClear` access gets a plain `_write` fn — W1C semantics
  (write-1-to-clear) aren't modeled.
- `txctrl`/`rxctrl`-style shared registers generate per-register
  read/write fns with no read-modify-write helper — user must handle
  partial-field updates.
- Generates `u<width>` return types — `u32` etc. — needs the width
  to be a valid Fides type name (widths other than 8/16/32/64 emit
  garbage like `u24`).

### F6 — Local duplicates + misplaced `pub` — LOW

Private `int_to_str`/`int_to_hex` reimplemented (:470-511) while
`use string` is imported — the stdlib versions exist (stdlib/string.fd);
the imports are probably unused (lint would flag). `char_from_digit`/
`hex_char` marked `pub` (:506/:510) — leaking helpers; `int_to_hex`
returns "" for negatives (fine for addresses).

## Verification state

- Dead status verified: zero importers in `compiler/`+`stdlib/`; also
  dead in Rust (`pub mod` with zero references).
- `check_function`/`status` semantics verified by reading; can't be
  exercised (no caller).
- No port regressions — the file is a faithful port of already-dead
  Rust scaffolding.

## Next

`errors.fd`.
