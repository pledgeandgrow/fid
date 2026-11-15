# Internal tools: which are in Fides

Roadmap AD4: every internal tool that can be in Fides is. This is the list of
the project's own tools (Nov 15, 2026), what each is written in, and, for the
ones that are not in Fides, whether they could be and what stands in the way.

**In Fides now:** the compiler and everything built into it (`fid lint`,
`fmt`, `explain`, `audit`, `qualify`, `test-rules`, `split-c`, `lsp`, `doc`,
`veil`, `ark`); `tools/ark.fd`, `tools/scribe.fd`, `tools/veil.fd` (entries for
`compiler/{ark,scribe,veil}.fd`); `tools/cas.fd` (the build-cache server);
and, new on Nov 15, `tools/site.fd` (the website, [website.md](website.md)),
`tools/release_notes.fd` and `tools/release_checksums.fd` (ports, below).

## What blocks the rest

Most tools left are drivers: they run compilers, emulators and other
programs, compare what comes back, and keep going when one hangs. Fides can
run a program (`stdlib/process.fd`: `process_run(cmd)` through the shell,
with its stdout, stderr and exit code), but:

- **No timeout and no kill.** `process_run` waits for the program to end.
  Every driver that runs generated or fuzzed code needs a time limit (a
  miscompile is often an endless loop): the fuzzers, `expect.py`,
  `variants.py`, the cross and WASM test runners (`timeout` in shell).
- **No parallel children.** `process_run` blocks the whole thread; there is no
  spawn/wait pair to keep `JOBS=n` programs running (`cross-test.sh`,
  `sanitize.sh`, `bootstrap.sh`'s split C units).
- **Text only.** Output is a `str`, cut at the first NUL byte; there is no
  way to pass an environment or a working directory except by writing them
  into the shell command line.
- **Tools that are other people's.** NSIS, QEMU, Node's WASI, clang's
  assembler, z3, Cargo and GitHub Actions are what those scripts exist to
  drive; the driver could be Fides, the tool cannot.

With a `process_spawn(argv, env, dir) -> handle`, `process_wait(handle,
timeout_ms)` and `process_kill(handle)` in the stdlib (a runtime builtin on
each backend), most of the "could be" rows below become small ports.

## The list

"Could be": **yes** (nothing missing), **after P** (after the process API
above), **no** (the tool is someone else's, or must not depend on `fid`).

### Release and docs

| Tool | Language | In Fides? | Could be | Notes |
|---|---|---|---|---|
| `scripts/release-notes.py` | Python, 39 lines | **ported**: `tools/release_notes.fd` | — | Same output on the real changelog (below). |
| `scripts/release-checksums.sh` | Bash, 48 | **ported**: `tools/release_checksums.fd` | — | Uses `stdlib/crypto.fd`'s SHA-256; needs no `sha256sum`. |
| `scripts/doctest.py` | Python, 125 | no | after P | Extracts `/// Example:` blocks, runs each with `fid run`, 60 s timeout. The extraction is plain text work; the timeout needs P. |
| `scripts/gen-brotli-dict.py` | Python, 88 | no | yes | Base64 of a binary file into Fides literals, checked by SHA-256: `encoding` and `crypto` have both. Run once; low value. |
| `installer/build-installer.sh` | Bash, 157 | no | no | Drives NSIS. |
| `.github/workflows/*.yml`, `scripts/ci/cross.yml` | YAML | no | no | GitHub Actions. |
| `scribe` (Rust `src/scribe.rs`) | Rust, 882 | Fides version in `compiler/scribe.fd` | — | Shipped binary is still the Rust one (AD1). |
| website | — | **`tools/site.fd`** | — | New (AD2). |

### Building and checking the compiler

| Tool | Language | In Fides? | Could be | Notes |
|---|---|---|---|---|
| `scripts/bootstrap.sh` | Bash, 223 | no | after P, but should not | Builds `fid` from `fidesc` and checks the fixpoint: it must work when no `fid` works. Keep in shell. |
| `scripts/build-selfhost.sh` | Bash, 117 | no | no | Same reason. |
| `scripts/verify/clean-bootstrap.sh`, `build-variant.sh`, `repro.sh` | Bash, 63/94/85 | no | after P | `git archive`, copies of the tree, rebuilds, byte comparison. `repro.sh`'s comparison could be Fides today. |
| `verify.sh`, `verify-selfhost.sh` | Bash, 170/322 | no | after P | Drive Cargo and `fidesc test` (the Rust harness). |
| `tests/harness.rs` (`fidesc test`) | Rust, 841 | no | after P | The suite runner of the frozen Rust compiler. A Fides runner would also free the suite from `fidesc` (needs timeouts and parallel children). |
| `scripts/check-stdlib.sh` | Bash, 25 | no | after P | Builds one tiny program per stdlib module. Without timeouts it is portable as it is (no program is run), but slow without parallel builds. |
| `scripts/compile-gate.sh`, `bench/compile-check.sh` | Bash, 89/87 | no | after P | Parse `fid emit --time` lines, best of N, compare with a baseline file. The comparison is easy Fides; the runs need P only for a timeout. |
| `scripts/stale-cache-test.sh` | Bash, 49 | no | after P | Edits inputs, rebuilds, checks the cache was not reused. |
| `scripts/leak-check.sh` | Bash, 50 | no | after P | Runs `fid run --leaks`, compares with `.expected`. |
| `scripts/nat-gate.sh`, `pass-matrix.sh`, `me-measure.sh` | Bash, 72/33/65 | no | after P | Native builds with checkers on, then results compared. |
| `scripts/rules-smt.sh` | Bash, 36 | no | after P | Runs z3 on files `fid test-rules --smt` writes; z3 stays. |
| `scripts/enc-oracle.py` | Python, 359 | no | after P | Our encoders against clang's assembler; clang stays, the driver could move. |
| `scripts/macho-check.py` | Python, 297 | no | **yes** | Reads a Mach-O file and checks its structure: bytes in, verdict out, no other program. The best next port. |

### Testing on other targets

| Tool | Language | In Fides? | Could be | Notes |
|---|---|---|---|---|
| `scripts/cross-test.sh` | Bash, 158 | no | after P | `zig cc` cross builds, qemu-user or Node runs, `JOBS` in parallel, timeouts. |
| `scripts/wasm-test.sh` | Bash, 93 | no | after P | Same, for `fid wasm`. |
| `scripts/wasm-run.mjs` | JavaScript, 45 | no | no | It is the WASI host: it must be Node. |
| `scripts/bare-qemu-test.sh` | Bash, 179 | no | after P | Boots kernels under QEMU with a timeout. |
| `scripts/bare-run.c` | C, 113 | no | not yet | Maps a bare binary and jumps into it (Linux); Fides could with `mmap` and a call through a raw pointer, but this is the harness that checks such code. |
| `scripts/sanitize.sh` | Bash, 66 | no | after P | ASan/UBSan builds through C. |

### Verification and fuzzing

| Tool | Language | In Fides? | Could be | Notes |
|---|---|---|---|---|
| `scripts/fuzz/gen_programs.py`, `gen_vs_c.py` | Python, 178/657 | no | after P | Random program generators (`stdlib/random.fd` exists); running each candidate needs timeouts. A generator written in Fides would also test Fides. |
| `scripts/fuzz/fuzz_frontend.py`, `fuzz_parsers.py` | Python, 210/141 | no | after P | Mutate inputs, run `fid check` / parsers, classify crashes. |
| `scripts/fuzz/reduce.py` | Python, 128 | no | after P | Delta debugging over a failing program. |
| `scripts/fuzz/triage.py` | Python, 75 | no | **yes** | Groups kept failures by message: files and regexes only (`stdlib/regex.fd`). |
| `scripts/fuzz/nightly.sh` | Bash, 57 | no | after P | Runs the above. |
| `scripts/verify/expect.py` | Python, 176 | no | after P | Every backend on a test file, checks what it must do. |
| `scripts/verify/variants.py` | Python, 288 | no | after P | Builds with passes off / broken on purpose. |
| `scripts/verify/coverage.py`, `cov_instrument.py` | Python, 202/95 | no | after P / yes | `cov_instrument.py` rewrites generated C (text in, text out): yes. `coverage.py` drives runs. |
| `scripts/verify/chaos.py`, `soak.py`, `interleave.py` | Python, 212/108/117 | no | after P | Long-running children killed, restarted or starved on purpose: need spawn and kill. `soak.py` also reads a child's memory use. |

### Benchmarks

| Tool | Language | In Fides? | Could be | Notes |
|---|---|---|---|---|
| `bench/run.sh` | Bash, 91 | no | after P | Builds five languages' programs, best of N wall time, a table. |
| `bench/check.sh` | Bash, 90 | no | after P | Same, against `bench/baseline.txt`. |

### Package manager, docs, coverage (Rust binaries)

| Tool | Rust | Fides | State |
|---|---|---|---|
| `ark` | `src/ark.rs`, 881 lines | `compiler/ark.fd` 761, `tools/ark.fd`, `fid ark` | Fides version builds and runs (`version`, `list`); the installer still ships the Rust one (AD1). |
| `scribe` | `src/scribe.rs`, 882 | `compiler/scribe.fd` 1,023, `tools/scribe.fd`, `fid doc` | Same. |
| `veil` | `src/veil.rs`, 367 | `compiler/veil.fd` 655, `tools/veil.fd`, `fid veil` | Same; the Windows native build ran out of memory (AD1). |

## The two ports, checked

Both originals are kept; the ports were built with
`CC="zig cc" fid ccompile tools/<name>.fd` (Nov 15 compiler) and run from the
repository root.

**`tools/release_notes.fd`** against `python3 -X utf8 scripts/release-notes.py`
with the same arguments, on `docs/reference/changelog.md` (default and
`--all`: 5,873 lines), `docs/reference/stability.md` (default and `--all`),
`docs/reference/known-bugs.md`, `docs/roadmap/competitive-roadmap.md` and
`README.md` (`--all`; the roadmap has "breaking" lines, so the "Breaking
changes" section is exercised), and a file with no `## ` section (both exit 1
with `no sections in <path>`): the outputs are byte-identical once Python's
Windows newline translation (`\r\n` for `\n` on stdout) is undone. On Linux
both write `\n`. One known difference: Python's `splitlines()` also splits at
form feeds and Unicode line separators; the port splits at `\n` (and drops
`\r`); none of the repository's Markdown has those.

**`tools/release_checksums.fd`** with no arguments (the installer and the five
release binaries, 31 MB, 3.5 s) and on four named files, against
`sha256sum` run file by file as the script does: the same hashes and names.
Git Bash's `sha256sum` marks binary mode with `" *name"`, Linux's writes
`"  name"`; the port always writes `"  name"`, which `sha256sum -c` accepts
on both (checked in Git Bash). The port adds `--out FILE` (the script always
writes `SHA256SUMS.txt` at the repository root), so it can be checked without
touching the tree.
