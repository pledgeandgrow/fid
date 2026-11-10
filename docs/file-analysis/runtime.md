# runtime.fd — Fides-authored runtime scaffold (superseded)

- **Path:** `compiler/runtime.fd` — module `compiler::runtime`
- **LOC:** 546 (2026-11-09).
- **Role (intended):** the `p_*` builtins of native binaries written once in
  Fides over a small `rt_sys_*` per-arch ABI (`extern "C"` block :76-87):
  a first-fit allocator over `rt_sys_mmap`, memset/memcpy, C-string
  helpers, print/panic/exit, file read/write/append/exists, argv/getenv,
  clocks.
- **Status:** DEAD and does **not type-check**. Superseded by
  `stdlib/linux_runtime/*.fd` (native Linux and `--nat` Windows, built on
  `nat_syscall` and friends) and `stdlib/pe_runtime/*.fd` (Windows `emit`).
- **Re-audited:** 2026-11-09.

## Where it is called

**Nowhere.** No `use compiler::runtime` (grep 2026-11-09 over `compiler/`,
`stdlib/`, `tests/`, `scripts/`). Nothing provides `rt_sys_*` either: no
backend emits those symbols. Not reachable from main.fd.

Imports: `core`, `vec`, `string`.

## Findings

### Verified live

1. **NEW, HIGH (for anyone reviving it) — type check fails.**
   `s18.exe check compiler/runtime.fd` → 7 errors, all
   "Visibility error: 'p_str_length' is private to module 'core'". core
   now has its own private `p_str_length` (stdlib/core.fd:3059); this
   file's `pub fn p_str_length` (:242) and its callers collide with it.
   (The 2026-10-23 doc said "typechecks clean"; no longer true.)

### From code inspection

2. **NEW, HIGH — every byte operation works on 8-byte words.** `p_memset`
   (:217), `p_memcpy` (:227), `p_str_length` (:242), `p_str_equal` (:268),
   `p_str_compare` (:288), `p_to_str` (:312) and the NUL writes use
   `*mut i64` where a byte is meant: memset/memcpy write 8 bytes per
   index (7 bytes past the end), strlen stops only at 8 zero bytes in a
   row. None of it would work if linked.
3. **NEW, MEDIUM — string layout is out of date.** Strings here are plain
   NUL-terminated blocks (or `p_str_alloc`'s layout at the time). Proposal
   0004 strings are `[P_STR_RC + count][len]` bytes NUL (AGENTS.md rule
   25); a runtime built from this file would not match core.
4. **CHANGED — "consolidation target for the three runtimes".** That role
   went to `stdlib/linux_runtime/` (header: "builtin p_X runs pert_X, in
   Fides, merged into the program … No C library"), which implements the
   whole builtin surface this file lists as TODO (:531-546).
5. **STILL TRUE, LOW — allocator never coalesces** (:131-208, documented).
6. **INFO — the file header (:1-64) still describes it as the plan for
   native backends.** It is not; INDEX.md:139 calls it "not yet wired",
   which undersells that it is replaced.

## Fix order

1. Delete it (preferred): `stdlib/linux_runtime/` does this job.
2. If kept as a reference: rename `p_str_length`, switch the byte loops to
   `*mut u8`, and mark the header as historical.

## Verification state

- 2026-11-09: whole file read; importers grepped; `s18.exe check` run
  (7 visibility errors).
