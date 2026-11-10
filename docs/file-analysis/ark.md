# ark.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18, native,
> so `p_system` runs `cmd.exe`). Only change since Oct 22: `str_equal` → `==` (merge 62fcc30).

- **Path:** `compiler/ark.fd`
- **LOC:** 761 (Rust `src/ark.rs`: 707)
- **Role:** package manager: `ark.toml` manifest, `ark.lock`, pacts installed into `ark_modules/`.
  Commands `init`, `add`, `install`/`i`, `remove`/`rm`, `list`/`ls`, `lock`, `update`, `search`,
  `publish` (both stubs), `version`, `help`. Small TOML reader.

## Callers

| Caller | Site | Use |
|---|---|---|
| `compiler/main.fd` | :184 `ark` → `ark_main()` | `fid ark <command> ...` |

`ark_main` reads `p_argc` / `p_argv` itself (:708-761) and skips a leading `ark`. A global flag
before the command (`fid --borrowck=off ark init`) becomes the command ("Unknown command").
Leaf module (`core`, `vec`, `string`).

## Findings

### Verified live (scratch folder: init, add, install, rm, lock, update)

- **HIGH — CHANGED — directory path pacts fail and stop the install.** `ark add libd --path libdir`
  then `ark install` prints `FAILED`, `Path pact 'libd' does not exist: libdir`, and returns 1
  (:501-509): `p_file_exists` is false for a directory on Windows. Pacts after it are not
  installed. (Oct 22: the xcopy branch ran under `sh` and claimed success.) The xcopy branch
  (:520-525) is now only reachable where `p_file_exists` accepts directories, and on Linux there
  is no `xcopy`.
- **MEDIUM — STILL TRUE — `ark lock` silently drops pacts** whose `ark_modules/<name>` is missing
  (:608-611): after the failed install, `Wrote ark.lock with 0 pact(s).`
- **MEDIUM — STILL TRUE — `ark update <name>` ignores the name** (:661-669): it reinstalls
  everything and hit the same failure.
- **LOW — a file pact is copied without its extension.** `yy = { path = "y.fd" }` is installed as
  `ark_modules/yy`, which `use yy;` would not find as `yy.fd`.
- **LOW — STILL TRUE — `use string` unused** (`fid lint`).
- **OK:** `init`, `add`, `rm`, `list`, file pact install, lock entry for it.

### From code inspection

- **MEDIUM — STILL TRUE — git pacts cannot be reinstalled**: `git clone` into an existing folder
  fails (:530, :545, :564).
- **MEDIUM — STILL TRUE — the lock does not lock**: git entries record the literal `"git"`, not a
  commit.
- **LOW — STILL TRUE — shell injection / path traversal**: names, URLs, branches and tags go
  unquoted into `p_system` strings (:521, :530, :545, :564) and into `ark_modules/<name>`.
- **LOW — STILL TRUE — `mkdir ark_modules 2>nul`** (:491) is Windows syntax; under a POSIX shell it
  creates a file named `nul`. (A stray `nul` file sits in the repo root today; its origin was not
  traced.)
- **LOW — STILL TRUE — write results ignored** (:269, :308, :515); `p_file_read` null and empty
  file look the same (:208-213).
- **LOW — STILL TRUE — TOML gaps** (trailing `#` comments, `[[pact]]`, commas in inline tables);
  `--branch`/`--tag` without `--git` fall back to a path pact (:367-385); `DepKind` (:37) unused.
- **INFO — no tests** exercise ark.

## Fix order

1. Path pacts: detect directories (`fs.fd` has `fs_is_dir`), copy them in Fides (`p_dir_list`)
   instead of `xcopy`, keep the `.fd` name for files, and continue with the next pact on failure.
2. Replace `mkdir ... 2>nul` with `p_mkdir`.
3. `lock`: warn on skipped pacts. `update <name>`: install only that pact.
4. Quote or validate names and URLs before `p_system`.

## Verification state

- `fid check compiler/ark.fd`: no errors. `fid lint`: `unused import: 'string'`.
- Live run of `fid ark` in the session scratch folder (`arkt/`); repo untouched.
