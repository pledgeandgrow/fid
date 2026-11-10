# embedded.fd — embedded/IoT scaffolding

- **Path:** `compiler/embedded.fd` — module `compiler::embedded`
- **LOC:** 511 (2026-11-09, unchanged). Rust `src/embedded.rs`: 622.
- **Role:** four "Phase 5B" models: `FixedBufferAllocator` (a bump
  allocator over a `Vec<i64>`), `BinarySizeOptimizer` (size report and
  advice strings), `RealTimeAnalyzer` (flags → Safe/Violation), and
  `HalFramework` (device/register table → generated Fides accessor source).
- **Status:** DEAD. Nothing imports it, here or in Rust.
- **Re-audited:** 2026-11-09.

## Where it is called

**Nowhere.** No `use compiler::embedded` and no reference to any of its
types outside the file (grep 2026-11-09 over `compiler/`, `stdlib/`,
`tests/`, `scripts/`). Not reachable from main.fd; no CLI command or
pipeline stage uses it. Rust: `src/lib.rs` declares `pub mod embedded`,
nothing uses it.

The language features with similar names are implemented elsewhere and do
not use this file: `@firmware` / bare-metal checks in typechecker.fd
(`is_os_dependent_func`, :3608), `p_phys_read*/write*` in codegen_c.fd and
the interpreter stubs. There is no `RealTime` effect in typechecker.fd.

Imports: `core`, `vec`, `string`. `s18.exe check compiler/embedded.fd`: no
errors.

## Findings

### From code inspection

1. **STILL TRUE, HIGH (dead) — scaffolding only.** None of the four parts
   is wired to a stage, flag or effect check.
2. **STILL TRUE, MEDIUM — `RealTimeAnalyzer_status` says `Safe(0)` for a
   function never checked** (:264-276). Needs an "unchecked" state.
3. **STILL TRUE, MEDIUM — `BinarySizeOptimizer_analyze` counts the whole
   binary as `.text`** (:155-165); data/bss/debug are hard 0; the
   recommendations are the same strings whatever the input.
4. **STILL TRUE, LOW — `FixedBufferAllocator`** hands out offsets into its
   own `Vec<i64>` (one i64 per "byte"); `free` drops the record without
   reclaiming (:43-75).
5. **STILL TRUE, LOW — HAL generator** (:371-407): emits
   `fn dev_reg_read() -> uN !Read { return p_phys_readN(0x…); }` and a
   `_write` for non-read-only registers. Write-1-to-clear gets a plain
   write; widths other than 8/16/32/64 produce types such as `u24`; no
   read-modify-write helper. `p_phys_read*`/`write*` exist in the C runtime.
6. **STILL TRUE, LOW — local `int_to_str` / `int_to_hex`** (:470-500)
   duplicate the stdlib ones; `char_from_digit` / `hex_char` are `pub`
   (:501, :507) for no caller.

## Fix order

1. Delete the module, or move the HAL generator (the only useful part) to
   a tool that something calls.
2. If kept: an `Unchecked` status, real section parsing, width check in
   the generator.

## Verification state

- 2026-11-09: whole file read; importers grepped; `s18.exe check` clean.
  Nothing can be exercised (no caller).
