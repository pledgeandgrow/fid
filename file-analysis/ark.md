# ark.fd — file analysis

> **Re-verified 2026-10-23:** all headline findings re-checked against current source — still accurate.

- **Path:** `compiler/ark.fd` — 761 LOC
- **Role:** Package manager ("pacts" via `ark.toml` / `ark.lock` / `ark_modules/`)
- **Analyzed:** 2026-10-22 — run 1 of 52 (alphabetical order)

## Where it is called

| Caller | Site | How |
|--------|------|-----|
| `compiler/main.fd` | line 31 `use compiler::ark;`, line 155 `if str_equal(command, "ark") { return ark_main(); }` | `fides ark ...` subcommand |

That is the **only** call site. `ark_main()` takes no parameters — it reads process
argv directly via `p_argc()`/`p_argv()` (`compiler/ark.fd:712-740`), scans for `--`,
then skips a leading `ark` token. This differs from every other `cmd_*` handler in
`main.fd`, which receive `(n, start)` — ark is coupled to global argv and can't be
driven programmatically.

Parallel implementation (not a caller): `src/ark.rs` (707 LOC) builds the standalone
`target/release/ark.exe` bootstrap binary. Behavior is similar but not identical
(Rust uses `BTreeMap` so deps sort alphabetically; Fides keeps insertion order).

Imports: `core`, `vec`, `string`. No `compiler::*` deps — **leaf module**.

## Findings

Severity: HIGH = wrong results / silent failure · MEDIUM = broken feature · LOW = robustness/style

### Verified live (interp, `fidesc run`)

1. **HIGH — Directory path-deps silently fail to install.**
   `ark_cmd_install` probes `p_file_read(p_path)`; a directory returns `""`
   (interp: `fs::read_to_string` errors → `Ptr(0)`), so it falls into the
   `xcopy` branch (`compiler/ark.fd:520-525`). `p_system` in the interpreter
   always runs `sh -c` (`src/interp.rs:4958`) where `xcopy` doesn't exist —
   **nothing is copied, yet it prints `OK (path/dir)` and writes a lock entry.**
   Verified: `ark_modules/` empty after "successful" install. The `p_system`
   return code is not checked in this branch. Same failure on Linux/macOS in
   every backend. Also: empty files and non-UTF-8 files hit the same `""` path
   and get misrouted to xcopy.

2. **HIGH — Windows-isms run under `sh -c` → stray `nul` file.**
   `p_system("mkdir ark_modules 2>nul")` (`compiler/ark.fd:491`): under the
   interpreter `2>nul` creates a real file named `nul` in cwd (verified).
   `>nul 2>nul` in the xcopy line does the same. Works only in the C backend on
   Windows where `system()` → cmd.exe. `mkdir` is also not `-p`/exist-checked.

3. **MEDIUM — `ark lock` silently drops pacts.**
   `ark_cmd_lock` skips pacts whose `ark_modules/<name>` dir doesn't exist with
   a bare `continue` — no warning (`compiler/ark.fd:608-611`). After the
   silently-failed install above: `Wrote ark.lock with 0 pact(s)` — verified.

4. **MEDIUM — `ark update <name>` ignores the name.**
   Prints `Updating '<name>'...` then calls `ark_cmd_install(Vec::new())` →
   reinstalls everything (`compiler/ark.fd:661-669`). Verified.

5. **LOW — `use string` is unused.** `fidesc lint compiler/ark.fd` →
   `warning: unused import: 'string'`.

### From code inspection

6. **MEDIUM — Git deps can't be reinstalled.** `git clone` into existing
   `ark_modules/<name>` fails → `ark update` and second `ark install` always
   FAIL for git pacts (`compiler/ark.fd:530-537` etc.). Needs dest-exists
   handling (fetch/pull or wipe+clone).

7. **MEDIUM — Lock file doesn't lock.** Git entries record
   `resolved = "git"` — a literal, not a commit SHA (`compiler/ark.fd:539,556,575,617-624`).
   Nothing reproducible. Recording the SHA needs stdout capture; `p_system`
   returns only the exit code — a `p_system_out`/popen-style builtin is missing.

8. **LOW — Command injection / traversal surface.** Dep name, `--git` url,
   `--branch`, `--tag` are interpolated unquoted into shell strings
   (`compiler/ark.fd:521,530,545,564`). A malicious `ark.toml` or
   `ark add 'x; cmd'` executes arbitrary commands. Dep name is also used raw in
   `ark_modules/<name>` → `../` escapes the modules dir. No validation anywhere.

9. **LOW — Write failures ignored.** `p_file_write` return discarded in
   `manifest_save`, `lock_save`, and the path-dep copy
   (`compiler/ark.fd:269,308,515`); `p_system` mkdir result ignored.

10. **LOW — `--branch`/`--tag` without `--git`** silently degrade to
    `DepSource::Path(dep_name)` (`compiler/ark.fd:367-385`) — flags ignored, no
    error. Usage text `[--path | --git | --branch | --tag]` implies all are
    alternatives.

11. **LOW — TOML parser gaps.** `parse_toml` only strips full-line comments;
    `name = "x" # comment` leaves `# comment` in the value and `strip_quotes`
    then fails (`compiler/ark.fd:99-128`). `[[pact]]` headers parse to a
    `[pact]` section. Inline-table `str_split(inner, ",")` breaks values
    containing commas.

12. **LOW — Dead code / surface.** `pub enum DepKind` never used
    (`compiler/ark.fd:37-43`); `DepSource::Registry("")` at line 367 is always
    overwritten; unreachable `return` after exhaustive matches in
    `dep_kind_str`/`dep_source_str`; unused `args` in `ark_cmd_list`/
    `ark_cmd_install`; all types and helpers are `pub` though only `ark_main`
    is consumed; `dep_kind_str` collapses Git/GitBranch/GitTag → "git" so
    `ark list` can't distinguish branch vs tag pins.

13. **INFO — No test coverage.** Nothing in `tests/selfhost/` or
    `tests/bootstrap/` exercises ark — silent regressions guaranteed (the bugs
    above went unnoticed). `ark list` header implies column alignment but
    prints raw values. Bare `fides ark` prints help but exits 1.

## Suggested fix order

1. Path-dep install: check `p_file_exists(dest)`, implement dir copy without
   xcopy (needs a `p_dir_list` builtin or `xcopy`/`cp -r` per-OS via
   `system() == windows` check — neither exists today), and honor `p_system`
   exit codes.
2. Replace `mkdir 2>nul` shell-out with `p_file_exists` check → removes the
   stray-`nul` bug entirely.
3. `lock`: warn on skipped pacts.
4. `update <name>`: filter to the named pact.
5. Validate dep names/URLs (`..`, whitespace, shell metachars) before shell
   interpolation — ideally a `p_system_argv` builtin that skips the shell.
6. `git clone`: handle existing dest (wipe or `git -C dest pull`).
7. TOML: strip trailing comments outside quotes.
8. Delete `DepKind`, drop `use string`, make non-exported items private.
9. Add a `tests/` coverage hook (e.g. selfhost test driving `ark init`/`add`/
   `install` in a temp dir).

## Verification state

- Parses/type-checks/runs under the interpreter (exercised `init`, `add`,
  `list`, `install`, `lock`, `update` live via a temp wrapper — all artifacts
  cleaned up; repo untouched).
- `release/fides-c3.exe` not built in this checkout — C-backend behavior
  inferred from `src/codegen_c.rs` (`system()` semantics).
