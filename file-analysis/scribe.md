# scribe.fd

> **Re-verified 2026-10-23:** confirmed — still no `create_dir_all` + unchecked `p_file_write` → prints "Generated..." when nothing was written.

- **Path:** `compiler/scribe.fd`
- **LOC:** 999 (Rust `src/scribe.rs`: 881)
- **Role:** Documentation generator — scrapes `///` doc comments + `fn`/`struct`/`enum`/`impl`/`const`/`cap` declarations via line scanning (not the AST), renders HTML (sidebar + grouped sections + CSS) or Markdown, writes `docs/<module>.{html,md}`.

## Where called

| Caller | Line | Mechanism |
|---|---|---|
| `compiler/main.fd` | 25 | `use compiler::scribe` |
| `compiler/main.fd` | 153 | `scribe` command → `cmd_scribe` (:1883) |
| `compiler/main.fd` | 1891 | `scribe_process_file(input, output_dir, format)` |

Shipped as the standalone `scribe` binary per AGENTS.md. Line-scanning approach means it works on syntactically-broken files too (arguably a feature for a doc tool).

## Findings

### MEDIUM — output directory is never created + write result unchecked

`scribe_process_file` (:854-855) writes `output_dir + "/" + module_name + "." + ext` via `p_file_write` — but unlike Rust (`fs::create_dir_all` at `src/scribe.rs:817`), **no mkdir happens**. If `docs/` (the default) doesn't exist, `p_file_write` presumably fails — and its return value is ignored, so scribe prints `Generated docs/x.html (N items)` **even when nothing was written**. Silent failure producing a success message.

### LOW — empty file conflated with read failure

`str_len(source) == 0` → `"Cannot read <path>"` (:836) — same `p_file_read` ambiguity catalogued in earlier files. Rust distinguishes `fs::read_to_string` errors. Cosmetic for a doc tool, but a genuinely-empty `.fd` reports "Cannot read".

### LOW — no directory listing (documented limitation)

`process_directory` (:860-893) admits it: no `readdir`/`getdents` in the Fides runtime, so `scribe <dir>` can't enumerate `.fd` files like Rust's `std::fs::read_dir` (:842). The workaround is reasonable — treats a non-`.fd` input as a *file containing newline-separated paths* and batch-processes it. Still, `scribe stdlib/` (the help text's own example) doesn't work as advertised.

### Verified faithful (inspection)

- **`extract_docs` dispatch 1:1** — `pub fn/fn`, `pub struct/struct`, `pub enum/enum`, `impl`, `pub const/const`, `cap`/`pub cap` — including Rust's own dead duplicate `pub fn` arm (`:90`, "Already handled above"), faithfully ported.
- **All six parsers present**: `parse_function`, `scribe_parse_struct`, `scribe_parse_enum`, `parse_impl` (depth-1 method detection), `scribe_parse_const`, `scribe_parse_trait`.
- **`escape_html` correct**: `&`→`&amp;` applied *first* (the `"&" + "amp;"` concat produces the same literal), then `<`/`>`/`"` — same order as Rust, no double-escaping.
- **Grouping logic identical**: functions/structs/enums/consts gated on `is_pub`; `impl` and `trait` ungated — matches Rust's match arms exactly.
- **Module-name extraction**: `module X;` decl → else filename-stem fallback — same algorithm.

## Suggested fix order

1. `mkdir -p` the output dir before `p_file_write` (or check the write result and report failure) — currently the "success" message lies.
2. Distinguish empty-vs-unreadable in `process_file` if `p_file_read` ever gains error returns.
3. If `readdir`/`getdents` ever lands in the runtime (the `native_debugger`/`hotreload` runs showed the same syscall gap), wire real `process_directory`.

## Verification

Inspection-only — no self-hosted binary. Structure, dispatch, escaping, grouping, and I/O paths verified by direct diff against `src/scribe.rs`; this file has no machine-code or correctness-critical transforms, so the risk profile is low. The runtime-API gaps (mkdir, readdir, file-error distinction) are the systemic Fides-runtime limitation seen across `native_debugger`, `hotreload`, and the `runtime_*` runs.
