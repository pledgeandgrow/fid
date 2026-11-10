# lsp.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> New: every symbol request crashes the server (double free). Oct 23 findings still true.

- **Path:** `compiler/lsp.fd`
- **LOC:** 1,082 (Rust `src/lsp.rs`: 1,039)
- **Role:** language server over stdio: Content-Length framing, `initialize`, `shutdown`, `exit`,
  `didOpen` / `didChange` (full sync) / `didClose`, `publishDiagnostics`, `completion`, `hover`,
  `definition`, `rename`; `-32601` for unknown requests.

## Callers

| Caller | Site | Use |
|---|---|---|
| `compiler/main.fd` | :181 `lsp` → `lsp_run()` | `fid lsp` |

Imports `compiler::ast`, `compiler::json`, `compiler::compiler` (`build_program_with_path`,
`get_diagnostics`). Because of the last one, `fid check compiler/lsp.fd` type-checks the whole
compiler (did not finish in 115 s).

Changed since Oct 23: `doc_set` / `doc_remove` take `&mut Vec<DocEntry>` (commit e655b58, Nov 3).
Before, opened documents were not kept.

## Findings

### Verified live (scripted JSON-RPC session piped into `fid lsp`)

- **CRITICAL — NEW — completion, hover, definition and rename all crash.** After
  `initialize` + `didOpen` of a valid two-function file, each of the four requests ends the
  process with `Panic: Double free` in `vec_buf_free`, called from `Vec_drop` ←
  `__own_drop_67` ← `__own_drop_4` ← `handle_<request>` ← `lsp_run`. Common code: each handler
  gets a `Program` from `build_program_with_path` (:488, :604, :743, :823) into
  `var program: Program;` and drops it at the end. The program likely shares Vec buffers with
  the cached stdlib (cause is in ownership lowering or compiler.fd, not confirmed).
- **MEDIUM — NEW — diagnostics point at the wrong place.** A type error on line 2 is published with
  range line 0, character 0..1. The message keeps `(line 2)` but `parse_error_position`
  (compiler.fd) does not extract it.
- **OK:** `initialize` response, empty diagnostics for clean code, `-32601` for an unknown method.

### From code inspection

- **HIGH — STILL TRUE — every symbol feature gives up on broken code.** Each handler returns
  `Json::Null` when the build fails (:493, :609, :748, :828). Rust parses with
  `unwrap_or_default` and keeps going. Handlers also pass `OptStr::None` as the path (:488 etc.),
  so files with local `use` imports fail too.
- **MEDIUM — STILL TRUE — full pipeline per request.** Parse, stdlib merge, type check, borrow
  check on every keystroke and every request (a tiny file takes about 4-6 s with s18).
- **MEDIUM — STILL TRUE — trait-impl methods invisible.** Only `program.impls` is scanned
  (:504, :707, :791, :863); `trait_impls` is never read.
- **LOW — STILL TRUE — rename is a text search** (`find_all_occurrences`, :931): it rewrites
  matches in comments and strings.
- **LOW — columns are bytes**, LSP positions are UTF-16 units: wrong after non-ASCII text.
- **LOW — parser globals grow** across requests (see parser.md).
- **INFO — the raw runtime strings** `p_read_line` / `p_read_stdin` are used as `str` (:48, :78).
  `str_len` of the null at EOF is 0 (`p_str_length` checks 0), so the EOF test works.

## Fix order

1. Find the double free (run `fid lsp` with `--checked-alloc`, or reduce: build a program and
   drop it twice). Until then the LSP only serves diagnostics.
2. Return partial programs on parse/type errors for the symbol handlers; pass the document path.
3. Fix the diagnostic range (parse `(line N)` in `parse_error_position`).
4. Read `trait_impls`; skip borrow check for symbol requests.

## Verification state

- Live: four scripted sessions (one per symbol request) and one diagnostics session, input built
  with a small Python script; stdout and stderr read.
- `fid check compiler/lsp.fd`: not finished within 115 s (pulls in compiler.fd).
