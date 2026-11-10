# scribe.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> Oct 23 findings still true; two new live bugs on Windows (CRLF module names, `::` file names).

- **Path:** `compiler/scribe.fd`
- **LOC:** 989 (Rust `src/scribe.rs`: 881)
- **Role:** documentation generator. Scans lines (not the AST) for `///` comments and
  `fn` / `struct` / `enum` / `impl` / `const` / `cap` declarations, renders HTML (sidebar, CSS) or
  Markdown, writes `<output>/<module>.<html|md>`.

## Callers

| Caller | Site | Use |
|---|---|---|
| `compiler/main.fd` | :178 `doc`, :182 `scribe` → `cmd_scribe` (:3316) → `scribe_process_file` | `fid doc` / `fid scribe <file.fd> [--output dir] [--format html|markdown]` |

`process_directory` (:863) and `scribe_print_help` (:924) have no caller: `scribe <dir>` is not
reachable. `cmd_scribe` returns 0 in every case, even "Cannot read".

## Findings

### Verified live

- **HIGH — NEW — module name keeps the `;` on CRLF files.** `extract_module_name` (:899-921)
  splits on `\n`, then `str_trim_end_char(s, 59)` stops at the trailing `\r`. The repo has CRLF
  files (`core.autocrlf=true`). `fid scribe stdlib/math.fd` writes `math;.md` with the title
  `# math; -- Fides Documentation`.
- **HIGH — NEW — `module a::b;` gives a file name with `:`.** `fid scribe compiler/token.fd`
  prints `Generated .../compiler::token;.md (7 items)` and writes nothing (`:` is not allowed in
  Windows file names).
- **MEDIUM — STILL TRUE — output folder is not created and the write is not checked.**
  `--output <missing dir>` prints `Generated ...` and writes nothing (:857-860).
- **LOW — STILL TRUE — empty file reported as "Cannot read"** (:839-841).

### From code inspection

- **LOW — CHANGED — directory listing.** The comment at :864-870 says the runtime has no
  directory listing. It has one now (`p_dir_list`, used by `stdlib/fs.fd:87`; `p_mkdir` at
  `fs.fd:108`). `process_directory` still reads the "directory" as a list of paths, and is not
  called anyway.
- **LOW — declarations after an `@annotation` line lose their doc comment**: the annotation line
  ends the comment block and is skipped (:116-163). `pub var`, `type` aliases and `module`-level
  `var`s are not documented.
- **INFO — FIXED — the dead duplicate `pub fn` arm** of the Rust port is gone (:117).

## Fix order

1. Strip `\r` and `;`, and replace `::` with `_` or `.` in the module name used for the file name.
2. Create the output folder (`fs.fd` has a recursive mkdir) and check the write; return non-zero
   on failure.
3. Wire `scribe <dir>` through `p_dir_list`.

## Verification state

- `fid check compiler/scribe.fd`: no errors. `fid lint`: `unused import: 'string'`.
- Live: `fid scribe stdlib/math.fd`, `fid scribe compiler/token.fd`, and a missing `--output`
  folder, all writing into the session scratch folder.
