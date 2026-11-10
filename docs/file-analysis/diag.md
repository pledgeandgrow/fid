# diag.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> Still dead, and now it no longer type-checks.

- **Path:** `compiler/diag.fd`
- **LOC:** 659 (Rust `src/diag.rs`: 463)
- **Role:** structured diagnostics: `Severity`, `ErrCode` (37 codes, `E001`..`E999`), `Diag`
  (source line, caret span, help, notes), `DiagList`, ANSI color rendering, `diag_extract_line`,
  Levenshtein "did you mean".

## Callers

| Caller | Site | What |
|---|---|---|
| `tests/selfhost/test_diag.fd`, `tests/selfhost/test_sl_debug.fd` | `use compiler::diag` | tests only |
| compiler | — | nothing. No `use compiler::diag` in `compiler/` or `stdlib/`. |

Not reached from any CLI command. The self-hosted compiler formats errors as plain strings in
`compiler.fd` `enrich_errors` (:408): `error: Type error (line N): ...`, then `--> file:line:col`,
the source line and a caret. No error code, no did-you-mean.

## Findings

### Verified live

- **HIGH — NEW — the module does not compile.** `fid check compiler/diag.fd` gives 4 errors:
  `Diag_with_source` (:276), `Diag_with_span` (:283), `Diag_with_help` (:294), `Diag_with_note`
  (:301) change their by-value parameter `d` ("the caller would not see the change"). The
  type checker now rejects this. `tests/selfhost/test_diag.fd` therefore cannot build either.
- **HIGH — STILL TRUE — nothing emits structured diagnostics.** fid errors carry no `Exxx` code
  and no suggestion. Partial change: source context and caret now exist, from `enrich_errors`
  in compiler.fd, not from this module.

### From code inspection

- **LOW — STILL TRUE — gutter prints spaces, not the line number** (:402-405,
  `diag_repeat_char(32, line_width)`). `enrich_errors` output has the same look (`   | code`).
- **LOW — `DiagList` keeps rendered strings and an untyped `Vec`** (:474-482). The comment says
  Vecs cannot hold structs; that limit is gone (structs are held in Vec buffers, AGENTS.md item 25).
- **INFO — workarounds for old interpreter bugs** remain: `p_str_concat(cand, "")` as a copy
  (:648), `Severity_*()` / `ErrCode_*()` constructor shims (:65-68, :172-204).
- **INFO — 37 codes, several for passes no self-hosted check produces** (medical, gas,
  conservation).

## Fix order

1. Make the four builders copy first (`var d = d;`) so the file compiles again.
2. Decide: wire it into `enrich_errors` (codes, did-you-mean via `diag_levenshtein`) or delete it
   and its two tests.

## Verification state

- `fid check compiler/diag.fd`: 4 errors (above). `fid lint` reports the same 4 errors.
- Importers by grep over `compiler/`, `stdlib/`, `tests/`.
- Live error format observed in several `fid run` probes this session.
