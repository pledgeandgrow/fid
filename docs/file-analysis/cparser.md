# cparser.fd — C header → Fides FFI bindings generator (523 LOC)

## Role

Line-based C header parser: strips comments, joins multi-line decls, maps
C types → `CType`, emits `extern "C" include "x" { fn … }` Fides source.
Port of `src/cparser.rs` (315 LOC).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `main.fd` | :42 `use compiler::cparser`, :1932 `cmd_parse_header` | `fidesc parse-header <file.h>` → `parse_header` + `generate_pledge_bindings` — **WIRED since 2026-10-23**, mirroring `src/main.rs:145-146`. |

Was the fourth dead/stranded module; now live.

## Findings

### F1 — ~~Dead module~~ RESOLVED (2026-10-23)

`use compiler::cparser` + real `cmd_parse_header` added — the suggested restore
was done verbatim. F2–F5 (the parse bugs below) now matter again since the
module is reachable — every finding remains open.

### F2 — `extern` declarations all skipped — MEDIUM (inherited)

:89 — any line starting `extern` is skipped outright. Real headers declare
the bulk of their API as `extern ret f(...)` → **silently dropped** →
generated bindings are missing most functions. Same in Rust:32.

### F3 — `long` → `i64` — wrong ABI on Windows — MEDIUM (inherited)

:253 — C `long` maps to CI64. Under LLP64 (Windows — the primary dev
platform) `long` is 32-bit → **generated bindings have wrong arg/return
widths** for `long` params on Windows. `unsigned long` same (:268). Rust
inherits (src:165/170).

### F4 — `void*` inconsistent + wrong — MEDIUM (inherited)

- **Return type**: `void* f()` — `parse_c_type` sees base `void` → `CStr` →
  emitted as `str` — `malloc`/`HANDLE`-style returns typed as strings
  (should be `&void`/opaque ptr).
- **Param**: `void* p` → `&void` — different wrong answer for the same C
  type, because param parsing routes `void` through `CRef(CVoid)`.

### F5 — Array params emit invalid identifiers — MEDIUM (inherited)

`int arr[10]` / `char *argv[]` — no `[]` handling → param name becomes
`arr[10]`/`argv[]` → generated Fides has unparseable identifiers
(`fn f(argv[]: &str)`). Decay-to-pointer never happens.

### F6 — Qualifier stripping is fragile — LOW (inherited)

- `str_replace(p, "restrict", "")` removes the substring anywhere →
  `restricted`, `__restrict__` mangle to `ed`/`__`.
- `const ` only stripped with trailing space — `const*x` survives.
- `bool`/`_Bool` unmapped → `CNamed("_Bool")` → emits `_Bool` as a Fides
  type (CBool variant exists but is never produced).
- `str_trim_end_char(d, ")")` strips one trailing `)` — nested decls can
  leave residue (edge).
- Comment stripper mangles `//` inside string literals — but `#` lines are
  skipped anyway; near-harmless.

### F7 — Dedup first-wins — INFO

`seen_names` by `c_name` — redeclarations collapse ✓; forward-vs-real
decls can't shadow.

## Verification state

- Faithful port — every defect exists identically in `src/cparser.rs`.
  No port regressions; the only port *loss* is the CLI entry.
- The parser is deliberately minimal (line-based, no real C grammar) —
  inherited scope, not a Fides bug.
- Recommendation: either wire the CLI back (it's a useful dev tool) or
  delete; if wired, fix F2/F3/F5 first since generated bindings are the
  output users compile.

## Next

`debugger.fd`.
