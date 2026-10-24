# compiler.fd — pipeline orchestrator (2,841 LOC)

> **Re-verified 2026-10-23:** all headline findings confirmed — `fill_default_trait_methods` still drops new_func (:2602-2607), `--borrowck=warn` still reads `bc.errors`, `apply_cfg_filter` still a no-op (:2132 "simplified: always include"). New since analysis: `use compiler::x86` (:55) + `build_x86_linux` (:1912).

## Role

The hub: `parse → import-resolve → merge → cfg/derive/desugar → comptime →
typecheck → borrowck → substitute → monomorphize`, then per-target lowering
(IR / L1→L3+opt / C / PE / WASM). Also owns error enrichment
(`enrich_errors`, `parse_error_position`, `classify_error`,
`edit_distance`), `find_project_root`, the stdlib cache, the incremental
IR cache (via `inc.fd`), `merge_programs`, derives, desugars, and
`strip_ungranted_caps`. Port of `src/compiler.rs` (2,648 LOC).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `main.fd` | :21 import | every CLI cmd (`run`/`ccompile`/`build`/`lint`/`ir`/`pir`/`dump-*`/`test`) |
| `lsp.fd`, `hotreload.fd`, `native_debugger.fd`, `veil.fd` | imports | diagnostics/lower/rebuild entry points |
| self | `build_executable` :1617 | dispatch: x64→codegen_x64+elf64+linker; arm64/riscv same shape; windows→`generate_pe(module)`; wasm→`compile_to_wasm` |

## Findings

### F1 — `fill_default_trait_methods` is a no-op disguised as a port — HIGH (port regression)

:2485-2490 — builds the defaulted `Func`, then the comment admits
*"We can't mutate trait_impls in place easily in Fides… for now, skip
this"* — **`new_func` is dropped; nothing is ever added**. Rust does
`ti.methods.push(Func{…})` (src/compiler.rs:999+). Result: every trait
impl relying on a default method silently lacks it → downstream
"method not found" typecheck/call errors that appear to come from user
code. Fix: rebuild `program.trait_impls` entries instead of in-place push.

### F2 — `--borrowck=warn` prints nothing — HIGH (caller-side defect lives here)

:1340-1348 — warn mode iterates `bc.errors`, but `BorrowChecker` puts warn
violations in `bc.warnings` (Rust reads warnings, src:543-546). Warn mode =
silent + wasted pass. (Detail in `borrowck.md` F1.)

### F3 — `apply_cfg_filter` is a full no-op — HIGH (admitted)

:2025 — `// No-op -- include all items.` The comment explains: a real
implementation crashed on the Vec-of-struct push problem, so the whole pass
was disabled rather than fixed. **Every `@cfg` annotation is silently
ignored** — `#[cfg(platform="linux")]` items compile on all targets.
`check_cfg` (:2030) is dead code implementing real logic nobody calls.

### F4 — `build_program_no_std` diverges from the std pipeline — MEDIUM

:655-855 — the no-std path drops four things the std path does:

1. **Ignores `borrowck_mode`** — :848 hardcodes `BorrowckMode::Off` and
   creates a checker it never calls (`check` never invoked; `bc` is a dead
   var). `--no-std --borrowck=on` silently skips borrow checking.
2. **Skips `apply_cfg_filter` + `process_derive`** — `@cfg`/`@derive`
   ignored in bare-metal builds (moot given F3, but the omission is
   structural).
3. **Skips the `substitute` comptime pass** — std path runs `process_gen`
   pre-typecheck *and* `substitute` post-borrowck (:1358); no-std runs only
   `process_gen` → comptime expressions reach mono/lower unsubstituted.
4. No deferral logic for imports that need dependency names mid-parse.

### F5 — Unconditional `[build]`/`[prof]` stderr spam — MEDIUM (port drift)

Every pipeline stage emits `p_eprint_str("[build] …")` + `[prof] …ms`
lines (:913-1374, 1389-1429, 656-850). Rust gates identical tracing behind
`--trace` (src:1545+ `if trace`). Fides prints it always — every `run`,
`ccompile`, `lint`, `build` writes ~20 stderr lines. Leftover debug
instrumentation.

### F6 — Re-parse failure silently swallowed — MEDIUM

:1232-1239 — after imports resolve, user code is re-parsed with the full
name set; `ParseResultProgram::Err(_) => {}` keeps the **first** parse.
Any parse regression caused by newly-known type names is invisible —
the wrong AST proceeds to typecheck.

### F7 — `lint_source` skips import resolution — MEDIUM

:1493 comment: "simplified — no import resolution" — merges only
stdlib+user. Any file with `use` imports lints against missing modules →
false errors or silently skipped lints in multi-module projects.

### F8 — Dead plumbing: `g_lint_warnings`, `set_borrowck_mode`, `strip_ungranted_caps` — LOW-MEDIUM

- `g_lint_warnings` (:1318 written; :1468 reset) — write-only global;
  `lint_source` returns `tc.warnings` directly. Dead global.
- `set_borrowck_mode`/`g_borrowck_mode` (:871-877) — never called;
  `main.fd` threads the mode as a parameter. Dead global+setter.
- `strip_ungranted_caps` (:2506) — **zero callers**, yet the file header
  (:10) claims "fully functional". Capability DCE is dead code.

### F9 — Incremental cache ignores dependencies — MEDIUM (inherited shape)

`ccompile_source_with_path` :1386-1399 — `global_cache_get(source_path,
hash_source(source))` — the key is the **source file's own hash only**.
Change an imported module, keep the entry file identical → cache HIT →
stale IR module → wrong C emitted. Deps aren't tracked. (`inc.fd` design
issue surfaced here.)

### F10 — Import-resolution edge cases — LOW

- Cyclic import → warning + `continue` (:1113-1117): the module never
  loads, so the *real* failure surfaces downstream as unrelated "unknown
  name" errors instead of a clean cycle diagnostic.
- `cached_parse` caches by path only (:1075) — a module deferred once
  returns the *same* parse result with more names later; a
  successful-but-wrong parse can win (rescue path :1200 only fires on
  `None`, i.e. hard failure).
- Merge order stdlib→imports→user (:1243-1253) is fine; `merge_programs`
  (:1856) correctly prefers user names — but impls merge by
  `struct_name` only, so a user impl for `struct S` suppresses *all*
  stdlib impls of `S` even for different traits… actually trait_impls
  merge on (trait,type) (:1983) — only plain `impl` blocks conflate.

### F11 — Inherited/structural notes — INFO

- Windows PE path bypasses PIR (`generate_pe(module)` on raw IR) —
  `do_opt`/`do_lto` computed but unused on `windows` target. Deliberate in
  Rust too ("PE codegen works from the IR module directly", src:1680).
- Derive impls are theatrical in both ports — `_to_json` emits all-zero
  fields, `_debug_str` emits literal `f: ?` text, `_from_json` ignores its
  input. Only `PartialEq`/`Default` are real. Inherited placeholders.
- `p_file_read` null-checks via `== 0` (:752, :887) — pointer-sentinel
  idiom, works but opaque.
- `get_diagnostics` re-parses+rebuilds stdlib per call via the shared
  pipeline — `g_stdlib_cache` mitigates.

## Verification state

- Pipeline traced end-to-end from `main.fd` cmd handlers; every public fn
  mapped to callers; Rust parity checked at each suspicious site.
- F1/F2/F3/F5/F8 confirmed by direct source comparison (Rust does the
  work; Fides builds then drops, or reads the wrong collection, or
  removed the `if trace` gate).
- `>>` parse blocker still prevents `fidesc lint` E2E.

## Next

`comptime_eval.fd`.
