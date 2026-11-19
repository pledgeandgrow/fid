# Internal tools: which are in Fides

Roadmap AD4: every internal tool that can be in Fides is. This is the list of
the project's own tools (Nov 15, 2026), what each is written in, and, for the
ones that are not in Fides, whether they could be and what stands in the way.

**In Fides now:** the compiler and everything built into it (`fid lint`,
`fmt`, `explain`, `audit`, `qualify`, `test-rules`, `split-c`, `lsp`, `doc`,
`veil`, `ark`); `tools/ark.fd`, `tools/scribe.fd`, `tools/veil.fd` (entries for
`compiler/{ark,scribe,veil}.fd`); `tools/cas.fd` (the build-cache server);
and, new on Nov 15, `tools/site.fd` (the website, [website.md](website.md)),
`tools/release_notes.fd` and `tools/release_checksums.fd`, on Nov 16
`tools/macho_check.fd` and `tools/fuzz_triage.fd`, and on Nov 17, after the
process API below, `tools/doctest.fd` and `tools/check_stdlib.fd` (ports,
below).

**The process API (Nov 17).** `stdlib/process.fd` now has
`process_spawn(argv, env, dir) -> handle` (no shell: each argument reaches
the program as written), `process_spawn_to` / `process_spawn_files` (stdout,
and stderr apart or with it, to files), `process_wait(handle, timeout_ms)
-> Option<code>`, `process_kill(handle)`, `process_child_pid`,
`process_release`, and `process_run_argv(argv, env, dir, timeout_ms)` (run
with a time limit, stdout and stderr captured). Each runtime implements it
(`fid_proc_*`): the C runtime (`compiler/codegen_c.fd`, fork/execvp and
CreateProcessA), native Windows (`stdlib/pe_runtime/base.fd`), the machine
layer's Linux, macOS and `--nat` Windows (`stdlib/linux_runtime/sys.fd`,
`win.fd`), and the interpreter (`compiler/interp.fd`, through the shell,
since fid's first stage is built by a compiler without these functions).
Test: `tests/bootstrap/test_process_spawn.fd`. The section below is what
stood in the way before it.

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
| `scripts/doctest.py` | Python, 125 | **yes** (`tools/doctest.fd`, Nov 17) | — | Extracts `/// Example:` blocks, runs each with `fid run`, 60 s timeout (native-only ones built and run with `--emit`). Same arguments and output; `tests/tools/doctest/check.sh` compares the two. |
| `scripts/gen-brotli-dict.py` | Python, 88 | no | yes | Base64 of a binary file into Fides literals, checked by SHA-256: `encoding` and `crypto` have both. Run once; low value. |
| `installer/build-installer.sh` | Bash, 157 | no | no | Drives NSIS. |
| `.github/workflows/*.yml`, `scripts/ci/cross.yml` | YAML | no | no | GitHub Actions. |
| `scribe` (Rust `src/scribe.rs`) | Rust, 882 | **ported**: `compiler/scribe.fd` through `tools/scribe.fd` | — | Takes the Rust tool's command line (`-o`/`-f`, `--help`, `--version`, errors on stderr with exit 1); the installer builds it through C, and `tests/tooling/tools-selfbuild-check.sh` compares it with the Rust tool. |
| website | — | **`tools/site.fd`** | — | New (AD2). |

### Building and checking the compiler

| Tool | Language | In Fides? | Could be | Notes |
|---|---|---|---|---|
| `scripts/bootstrap.sh` | Bash, 223 | no | after P, but should not | Builds `fid` from `fidesc` and checks the fixpoint: it must work when no `fid` works. Keep in shell. |
| `scripts/build-selfhost.sh` | Bash, 117 | no | no | Same reason. |
| `scripts/verify/clean-bootstrap.sh`, `build-variant.sh`, `repro.sh` | Bash, 63/94/85 | no | after P | `git archive`, copies of the tree, rebuilds, byte comparison. `repro.sh`'s comparison could be Fides today. |
| `verify.sh`, `verify-selfhost.sh` | Bash, 170/322 | no | after P | Drive Cargo and `fidesc test` (the Rust harness). |
| `tests/harness.rs` (`fidesc test`) | Rust, 841 | no | after P | The suite runner of the frozen Rust compiler. A Fides runner would also free the suite from `fidesc` (needs timeouts and parallel children). |
| `scripts/check-stdlib.sh` | Bash, 25 | **yes** (`tools/check_stdlib.fd`, Nov 17) | — | Builds one tiny program per stdlib module. The port has the same output and builds `JOBS=n` at once. |
| `scripts/compile-gate.sh`, `bench/compile-check.sh` | Bash, 89/87 | no | after P | Parse `fid emit --time` lines, best of N, compare with a baseline file. The comparison is easy Fides; the runs need P only for a timeout. |
| `scripts/stale-cache-test.sh` | Bash, 49 | no | after P | Edits inputs, rebuilds, checks the cache was not reused. |
| `scripts/leak-check.sh` | Bash, 50 | no | after P | Runs `fid run --leaks`, compares with `.expected`. |
| `scripts/nat-gate.sh`, `pass-matrix.sh`, `me-measure.sh` | Bash, 72/33/65 | no | after P | Native builds with checkers on, then results compared. |
| `scripts/rules-smt.sh` | Bash, 36 | no | after P | Runs z3 on files `fid test-rules --smt` writes; z3 stays. |
| `scripts/enc-oracle.py` | Python, 359 | no | after P | Our encoders against clang's assembler; clang stays, the driver could move. |
| `scripts/macho-check.py` | Python, 297 | **ported**: `tools/macho_check.fd` | — | Same output and exit status on the aarch64-macos and x86_64-macos hello builds and on 124 of 126 damaged copies; the other 2 crash the script (UnicodeEncodeError on a name byte >= 0x80 on a cp1252 console). Native `emit` is blocked by `use crypto`'s @secret errors (known-bugs 2g.71): build it through C. |

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
| `scripts/fuzz/triage.py` | Python, 75 | **ported**: `tools/fuzz_triage.fd` | — | Same output on a keep directory written as gen_vs_c.py writes them and on 6 random ones (decoding, newline translation, repr, grouping). Native `emit` is blocked by `memchr` missing from the msvcrt imports (2g.58): build it through C. |
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
| `ark` | `src/ark.rs`, 881 lines | `compiler/ark.fd` 761, `tools/ark.fd`, `fid ark` | Fides version builds and runs (`version`, `list`); since Nov 16 the installer builds it from `tools/ark.fd` with the shipped fid.exe through C (release N8). |
| `scribe` | `src/scribe.rs`, 882 | `compiler/scribe.fd` 1,023, `tools/scribe.fd`, `fid doc` | `tools/scribe.fd` takes the Rust tool's command line (`-o`/`-f`, `--help`, `--version`, errors on stderr with exit 1); the installer builds it with the shipped fid.exe through C; `tests/tooling/tools-selfbuild-check.sh` compares it with the Rust scribe. |
| `veil` | `src/veil.rs`, 367 | `compiler/veil.fd` 655, `tools/veil.fd`, `fid veil` | As scribe: the Rust tool's command line, built by the installer through C, compared with the Rust veil by `tests/tooling/tools-selfbuild-check.sh`. Only `fid emit tools/veil.fd` (pe_ir) runs out of memory, since veil imports the whole compiler (AD1). |

## The ports, checked

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

**`tools/macho_check.fd`** (Nov 16) against `python3 scripts/macho-check.py`
on `fid emit tests/bootstrap/hello.fd --target aarch64-macos` and
`--target x86_64-macos`, and on 126 damaged copies (truncated at every load
command, flipped bytes, bad protections, overlaps, bad rebase and bind
opcodes, signature damage, unknown commands; about 75 different FAIL
messages): identical text and exit status except 2, where the script itself
crashes. Values are 64-bit where Python's are unbounded; a missing file is
an error message, not a traceback.

**`tools/fuzz_triage.fd`** (Nov 16) against `python3 scripts/fuzz/triage.py`
on 7 keep directories: identical. Python's repr, isspace and `\d` are
matched for Latin-1 and the common Unicode ranges, not all of Unicode.

**`tools/doctest.fd`** (Nov 17) against `python scripts/doctest.py` with the
same arguments and the same compiler for the examples (the Nov 16 fid; the
port built through C with the new runtime's `fid_proc_*` linked in, as the
rebuilt compiler will have them): on `tests/tools/doctest/sample` (an
example that passes, a failing assertion, a panic, its own `main` and a
`use` line, one that does not compile, a native-only one, an early return)
with `-v`, with `--emit` (the native one built with `fid emit` and run) and
with `--emit --native-only`, and on a folder of four stdlib modules with a
subfolder (45 examples): identical output and exit status, in about the
same time. Differences, none of which the sources reach: lines split at
`\n` only, ASCII whitespace and word characters, a program a signal killed
is `exit 128+n` (Python: `exit -n`), a `--fid` that cannot start fails
each example instead of a traceback.

**`tools/check_stdlib.fd`** (Nov 17) against `bash scripts/check-stdlib.sh`
on a small stdlib (five modules and one that does not type-check): identical
output and exit status; with `JOBS=3` 19 s against the script's 54 s. The
port sorts the modules in byte order (the script's glob in the C locale).

All six ports write `\n` where Python on Windows writes `\r\n`.
