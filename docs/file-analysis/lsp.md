# lsp.fd — LSP Server (JSON-RPC over stdio)

> **Re-verified 2026-10-23:** confirmed — symbol handlers still bail to Json::Null on any lex/parse/type error (LSP dark on broken code), still syntactic-only (no TypeChecker data, no trait_impls).

**1,082 LOC** vs `src/lsp.rs` 1,039 LOC (~104%). Live and nominally complete:
message loop, Content-Length framing, all five handlers, diagnostics.

## Where it's called

- `compiler/main.fd:152` — `fides-c lsp` → `lsp_run()`. Sole caller.
- Imports `compiler::json` (the sole consumer — see json.md).

## Verified faithful

- Wire protocol 1:1: `Content-Length` header parse, `p_read_stdin(len)` body read,
  `Content-Length: N\r\n\r\n` response framing, `-32601` for unknown requests.
- `initialize` capabilities byte-for-byte equivalent (full sync=1, `diagnosticProvider`,
  completion trigger `.`, hover/definition/rename on).
- `p_read_line`/`p_read_stdin`/`p_stdin_eof` exist in codegen_c (:901-903, :3618-3631) —
  the stdin primitives debugger.fd claimed missing. They work **only** on the compiled
  binary; under `fidesc run` they hit the `Int(0)` runtime-default (irrelevant — nobody
  interprets an LSP server).
- `didChange` takes `changes[0].text` — correct given declared `textDocumentSync: 1` (Full).

## Findings

### HIGH — every symbol feature dies while code is broken (the normal editing state)

All four handlers (`completion` :486, `hover` :598, `definition` :737, `rename` :817) call
`build_program_with_path` → `ResultProgram::Err` on **any** lex/parse/type/borrowck failure
→ `Json::Null`. Rust's `build_program_for_lsp` (src/compiler.rs:1297-1310) does
`parser.parse().unwrap_or_default()` — *"tolerate errors, we still want partial symbols"*.

An editor sends requests precisely while the file is mid-edit (unclosed braces, half-typed
idents). The Rust LSP keeps answering on the partial AST; **the Fides LSP goes completely
dark on the first syntax error**. This is the single biggest LSP UX regression — hover,
completion, goto-def and rename all stop working while you're actually typing.

### MEDIUM — no TypeChecker data: syntactic decls only

Rust's handlers consume `tc.get_funcs()`/`get_structs()`/`get_consts()`/`get_methods()` —
*resolved* types after typecheck. Fides walks `program.funcs`/`structs`/`enums`/`consts`/
`traits` — shows **as-written** types (`type_to_str` on AST nodes). Functionally similar for
annotated code but: no inferred-type display, no trait-impl method resolution (see below),
and every handler **re-runs the full pipeline** (parse + stdlib merge + typecheck + borrowck
+ `enrich_errors`) per keystroke — Rust's lsp path skips desugar/borrowck entirely
("simple merge — no desugaring/comptime for LSP", src/compiler.rs:1314).

### MEDIUM — trait-impl methods invisible to all features

`trait_impls` is **never referenced** — completion (:504), hover (:707), definition (:791),
rename (:863) scan `program.impls` only. `impl Iterator for MyType { ... }` methods are
dark to hover/goto-def/rename. Rust sees them via `tc.get_methods()` (impl + trait merged).

### LOW (inherited) — rename rewrites comments and string literals

`find_all_occurrences` (:931) is a raw whole-word scan with no lexing — renaming `foo`
also edits `"foo"` in string literals and `// foo` in comments. **Identical in Rust**
(src/lsp.rs:918-942 — same byte-scan). Semantic rename was never implemented on either side.

### Noted

- Stdlib symbols **are** visible — `build_program_with_path` merges cached stdlib, matching
  Rust's `get_cached_stdlib`. Parity.
- `[build]`/`[prof]` `p_eprint_str` spam per request → stderr only, doesn't corrupt stdout
  JSON-RPC, but noisy.
- `word_at_position`/`ident_before_dot`/`find_word_after`/`location_json` — careful
  word-boundary logic, 1:1 with Rust incl. the `pub fn `/`pub val ` keyword patterns.

## Assessment

A working, wire-correct LSP whose utility is capped by two port regressions: the
all-or-nothing program build (kills every feature on broken code — the exact moment an LSP
matters) and the missing typechecker/trait-impl data (methods defined via traits are
invisible). The fix is a tolerant `build_program_for_lsp` equivalent returning partial
symbols — the machinery exists; only the strictness differs.
