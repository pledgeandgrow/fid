# ast_ser.fd — the AST to bytes and back (checked-library and import caches)

> **New doc 2026-11-21.** Read from the source (entry points and the
> callers in compiler.fd); the per-node writers were not each read.

- **Path:** `compiler/ast_ser.fd`
- **LOC:** 2,706 (2026-11-21). In git since Nov 11 (5f770f2). No `module`
  line; imported as `compiler::ast_ser`.
- **Role:** competitive-roadmap CS2. A byte codec for the whole front-end
  AST: `AwBuf` (writer) and `ArBuf` (reader, with an `ok` flag a short or
  damaged buffer clears), `aw_*`/`ar_*` pairs for integers (8 bytes little
  endian), strings, options, every enum, `Type`, `Expr`, `Stmt`,
  `Pattern`, each declaration kind and `Program` (`aw_program`,
  `ar_program`). Everything is tagged, so a stale or partial file fails
  the read and the caller parses the source instead.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`.

## Callers

| File | What |
|------|------|
| `compiler.fd` | the checked-library artifact (CS2): core and the merged runtime as one file `<TEMP>/fid-ast-<hash>.bin` (magic 4473414, version 4, the library hash, one or two Programs, DB1's library file pairs); `FID_ASTCACHE=<file>` names another file, `FID_ASTCACHE=off` turns it off |
| `compiler.fd` | the import cache on disk (CB1): one Program per imported module, `fid-imp-<hash>.bin` in `FID_IMPCACHE_DIR` or TEMP (magic 4473417, version 1); `FID_IMPCACHE=off` turns it off; only the first build of a process uses it |
| `ir_ser.fd` | the same buffers for IR; `aw_func` hashes a function's AST for the lowered-IR cache key |
| `nat_ir.fd`, `pe_ir.fd`, `iropt.fd` | `aw_i64`/`aw_str`/`ar_*` for their own cache keys and files (the function cache, `FID_FNCACHE`) |

## Findings

- **LOW — unused entry points.** `ast_ser_program`, `ast_deser_program`
  and `ast_deser_ok` (magic 4473414, version 3) have no callers:
  compiler.fd writes its own headers around `aw_program` (version 4 for
  the artifact, magic 4473417 for the import cache).
- **LOW — stale comments.** The file header says the format is "FIDA" +
  version + payload hash; the magic is the i64 4473414 (the bytes "FID"),
  and the hash is written by the caller. `ast_deser_program` is said to
  return `OptProgram::None` on a mismatch; it returns `program_new()`.
- **INFO:** `FID_ASTCACHE=off` exists so a build with the artifact can be
  compared byte for byte with one without it (scripts/repro-check.sh,
  SP20).
