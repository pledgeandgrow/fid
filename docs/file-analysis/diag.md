# diag.fd — structured diagnostics (659 LOC)

> **Re-verified 2026-10-23:** still dead — zero importers; self-hosted compiler still emits bare strings (no error codes/context/suggestions).

## Role

Structured diagnostic system: `Severity`/`ErrCode` enums, `Diag` struct
with source context + caret spans + help/notes, `DiagList` accumulator,
color rendering, `diag_extract_line`, Levenshtein "did you mean".
Port of `src/diag.rs` (463 LOC, "SH-6.2 rewrite").

## Where it's called

| Caller | Site | What |
|--------|------|------|
| — | — | **Nothing.** Zero `use compiler::diag`; zero references to `Diag_*`/`ErrCode_*`/`diag_*` outside the file. |

Rust wires it in: `compiler.rs` uses `ErrCode`/`error_with_source`,
`main.rs` calls `diag::set_color`, `typechecker.rs:3973` delegates
did-you-mean to `diag::levenshtein`. **All call sites dropped in the
port** — the self-hosted pipeline reports errors as bare strings
(`ResultProgram::Err(Vec<str>)` → printed raw in main.fd:784-787; the
LSP gets `StrTriple`s from `compiler.fd::get_diagnostics`). Sixth dead
module (autograd, codegen, coff, cparser, debugger, this).

## Findings

### F1 — Structured diagnostics exist but nothing emits them — HIGH (gap)

Every error the self-hosted compiler produces is a plain string — no
error codes, no source-context caret, no did-you-mean suggestions,
no notes/help. Rust users get `error[E044]: type mismatch` with the
offending line; self-hosted users get `errors.get(i)` verbatim. This is
the single largest *user-visible* regression in the port so far, and the
fix is call-site wiring (typechecker/parser would need to return `Diag`
or `ErrCode` instead of `str`) — a large refactor, not a bug fix.

### F2 — Gutter line number never printed — LOW (inherited)

`Diag_render` (:402-412) computes `line_str`/`line_width` then emits
`line_width` **spaces** — the source line renders as `   | code` with no
number, despite the comment saying `"  2 | val x: str = 42;"`. Rust
(src:276-278) does the identical thing — `" ".repeat(line_width)` —
inherited cosmetic bug; the `--> file:line:col` header still carries
the location.

### F3 — Adaptation notes — INFO

- Color: Rust's global `AtomicBool` + `set_color` → a `use_color` param
  threaded through render. Reasonable (Fides has no module-level `var`),
  but every future caller must plumb it.
- `DiagList` stores **rendered strings**, not `Diag`s — comment cites a
  Vec-can't-hold-structs limitation (:474-476) — same interp family as
  the `&mut Vec` no-op from run 2. `has_errors` works via a parallel
  counter.
- `diag_suggest_did_you_mean` uses `p_str_concat(cand, "")` as a
  str-copy workaround (:648) — "str-to-str assignment from Vec.get
  doesn't work in the interpreter".
- `diag_levenshtein` is a correct two-row byte-wise impl; `suggest`
  threshold ≤2.
- Constructor functions (`Severity_error()`, `ErrCode_*()`) exist for
  "enum names not visible across imports" — a parser limitation shim
  that generated ~40 lines of boilerplate.

### F4 — ErrCode coverage gap — LOW

37 codes vs Rust's enum; `Internal`→E999, `Custom`→E000. Codes exist for
borrowck/capability/conservation/medical that no self-hosted pass
produces — the vocabulary outlives its speakers.

## Verification state

- Dead status verified: no importer anywhere in `compiler/` or `stdlib/`.
- Render path reviewed against Rust: faithful including the gutter quirk.
- Can't be exercised — nothing calls it.

## Next

`elf64.fd` — the ELF object writer/encoder (HIGH-risk per AGENTS.md).
