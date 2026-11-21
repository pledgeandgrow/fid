# build_info.fd — the version and build stamp `fid` names

> **New doc 2026-11-21.** Whole file read (60 lines).

- **Path:** `compiler/build_info.fd`
- **LOC:** 60 (2026-11-21). In git since Nov 19 (49fb2b9). No Rust counterpart.
- **Role:** release N12. Six functions that return constants:
  `fid_version_base` ("1.0.0"), `fid_build_release`, `fid_version`
  (`fid_build_release()` when set, else "1.0.0+dev"), `fid_build_source`
  (D9: a hash of the compiler and stdlib sources), `fid_build_commit`,
  `fid_build_date`. In the source all but the base are "".
- **Imports:** `core` only.

## Callers

| File | What |
|------|------|
| `main.fd` | `fid version` / `-V` / `--version` prints `fid <version> (self-hosted Fides compiler)` and the commit and date when set; crash reports name `fid <version>, build <id>` |
| `codegen_c.fd` | the version string compiled into a C build |
| `compiler.fd` | the version in native crash reports |
| `inc.fd` | `inc_compiler_hash`: with `fid_build_source` set (and `FIDES_SELF_HASH` not `exe`), every build cache is keyed on that hash plus the executable's size instead of a hash of the whole executable |
| `lsp.fd` | `serverInfo.version` |
| `scribe.fd` | the generator label of the pages |

## How the values get in

`scripts/build-info.sh` writes the commit, the date, the release version
and the source hash into this file before a release build (`RELEASE=1
scripts/bootstrap.sh`), and `scripts/build-info.sh --reset` puts the empty
values back after it. Every stage of a bootstrap reads the same file, so
the fixpoint still holds.

## Findings

- **INFO:** a development build (every build but a release one) prints
  `fid 1.0.0+dev`; it names the exact build by the hash of its executable
  (main.fd `fid_build_id`), not by a commit.
- No bugs found.
