# Internal tools: which are in Fides

Roadmap AD4: every internal tool that can be in Fides is. This is the list of
the project's own tools (Nov 15, 2026; line counts of Nov 21), what each is written in, and, for the
ones that are not in Fides, whether they could be and what stands in the way.

**In Fides now:** the compiler and everything built into it (`fid lint`,
`fmt`, `explain`, `audit`, `qualify`, `test-rules`, `split-c`, `lsp`, `doc`,
`veil`, `ark`); `tools/ark.fd`, `tools/scribe.fd`, `tools/veil.fd` (entries for
`compiler/{ark,scribe,veil}.fd`); `tools/cas.fd` (the build-cache server);
and, new on Nov 15, `tools/site.fd` (the website, [website.md](../website.md)),
`tools/release_notes.fd` and `tools/release_checksums.fd`, on Nov 16
`tools/macho_check.fd` and `tools/fuzz_triage.fd`, and on Nov 17, after the
process API below, `tools/doctest.fd` and `tools/check_stdlib.fd`, and on
Nov 25 the gate, verification, fuzzing and target drivers (ports, below).

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
| `scripts/doctest.py` | Python, 259 | **yes** (`tools/doctest.fd`, Nov 17) | — | Extracts `/// Example:` blocks, runs each with `fid run`, 60 s timeout (native-only ones built and run with `--emit`). Same arguments and output; `tests/tools/doctest/check.sh` compares the two. |
| `scripts/gen-brotli-dict.py` | Python, 88 | **ported**: `tools/gen_brotli_dict.fd` (Nov 25) | — | Base64 of a binary file into Fides literals, checked by SHA-256 (`encoding`, `crypto`). `tests/tools/gen_brotli_dict/check.sh`: 7 cases the same (below). |
| `installer/build-installer.sh` | Bash, 261 | no | no | Drives NSIS. |
| `.github/workflows/*.yml`, `scripts/ci/cross.yml` | YAML | no | no | GitHub Actions. |
| `scribe` (Rust `src/scribe.rs`) | Rust, 882 | **ported**: `compiler/scribe.fd` through `tools/scribe.fd` | — | Takes the Rust tool's command line (`-o`/`-f`, `--help`, `--version`, errors on stderr with exit 1); the installer builds it through C, and `tests/tooling/tools-selfbuild-check.sh` compares it with the Rust tool. |
| website | — | **`tools/site.fd`** | — | New (AD2). |

### Building and checking the compiler

| Tool | Language | In Fides? | Could be | Notes |
|---|---|---|---|---|
| `scripts/bootstrap.sh` | Bash, 239 | no | after P, but should not | Builds `fid` from a pinned stage 0 (`release/stage0/fid`) or from `fidesc` and checks the fixpoint: it must work when no `fid` works. Keep in shell. |
| `scripts/build-selfhost.sh` | Bash, 119 | no | no | Same reason. |
| `scripts/verify/clean-bootstrap.sh`, `build-variant.sh`, `repro.sh` | Bash, 122/137/246 | no | after P | `git archive`, copies of the tree, rebuilds, byte comparison. `repro.sh`'s comparison could be Fides today. Not ported in the Nov 25 pass: each run is a whole bootstrap (several 120 MB C builds), more than this shared machine could compare against the scripts. |
| `verify.sh`, `verify-selfhost.sh` | Bash, 208/328 | no | after P | Drive Cargo and `fidesc test` (the Rust harness). Not ported in the Nov 25 pass: comparing them means full suite runs (thousands of tests per backend); they would follow a Fides suite runner (next row). |
| `tests/harness.rs` (`fidesc test`) | Rust, 861 | no | after P | The suite runner of the frozen Rust compiler. A Fides runner would also free the suite from `fidesc` (needs timeouts and parallel children, which exist now). Not ported in the Nov 25 pass; `fid test` (compiler/main.fd) already runs the bootstrap suite itself. |
| `scripts/check-stdlib.sh` | Bash, 27 | **yes** (`tools/check_stdlib.fd`, Nov 17) | — | Builds one tiny program per stdlib module. The port has the same output and builds `JOBS=n` at once. |
| `scripts/compile-gate.sh`, `bench/compile-check.sh` | Bash, 96/87 | **ported**: `tools/compile_gate.fd` (Nov 25; `--bench`, or built as `compile_check`, is compile-check.sh) | — | Same environment, logs, baselines, output and exit status, bash's "integer expression expected" lines of compile-check.sh included. `tests/tools/compile_gate/check.sh`: 13 cases the same (below). |
| `scripts/stale-cache-test.sh` | Bash, 49 | **ported**: `tools/stale_cache_test.fd` (Nov 25) | — | Edits inputs, rebuilds, checks the cache was not reused. `tests/tools/stale_cache_test/check.sh`: 5 cases the same (below). |
| `scripts/leak-check.sh` | Bash, 62 | **ported**: `tools/leak_check.fd` (Nov 25) | — | Runs `fid run --leaks` (or `NATIVE=<target>` `emit --leaks` through `RUNNER`), compares with `.expected`, `JOBS` at once. `tests/tools/leak_check/check.sh`: 3 runs the same, the report folder's files too (below). |
| `scripts/nat-gate.sh`, `pass-matrix.sh`, `me-measure.sh` | Bash, 91/214/65 | **ported**: `tools/nat_gate.fd`, `tools/pass_matrix.fd`, `tools/me_measure.fd` (Nov 25) | — | Native builds with checkers on, then results compared. The builds and runs stay `scripts/cross-test.sh`'s (nat_gate, pass_matrix; its own port is `tools/cross_test.fd`), as do `scripts/rules-smt.sh` (nat_gate) and `scripts/switch-check.sh --list` (pass_matrix). `tests/tools/{nat_gate,pass_matrix,me_measure}/check.sh`: 3, 4 and 3 cases the same (below). |
| `scripts/rules-smt.sh` | Bash, 36 | **ported**: `tools/rules_smt.fd` (Nov 25) | — | Runs z3 (or z3's Python module) on files `fid test-rules --smt` writes; z3 stays. `tests/tools/rules_smt/check.sh`: 5 cases the same, the 253 real obligations included (below). |
| `scripts/enc-oracle.py` | Python, 362 | **ported**: `tools/enc_oracle.fd` (Nov 25) | — | Our encoders against clang's assembler (`zig cc -target`); clang stays. `tests/tools/enc_oracle/check.sh`: 7 cases the same, the 1,200 real forms included (below). |
| `scripts/macho-check.py` | Python, 297 | **ported**: `tools/macho_check.fd` | — | Same output and exit status on the aarch64-macos and x86_64-macos hello builds and on 124 of 126 damaged copies; the other 2 crash the script (UnicodeEncodeError on a name byte >= 0x80 on a cp1252 console). Native `emit` was blocked by `use crypto`'s @secret errors (known-bugs 2g.71, fixed Nov 19). |

### Testing on other targets

| Tool | Language | In Fides? | Could be | Notes |
|---|---|---|---|---|
| `scripts/cross-test.sh` | Bash, 434 | **ported**: `tools/cross_test.fd` (Nov 25) | — | Same arguments, environment, `$OUT` files (results.txt, run-all.sh, the `.expected`/`.build_rc`/`.actual_rc` files) and output; `JOBS` builds through `tools/target_test_lib.fd`'s pool, time limits by kill. zig cc (through fid), qemu-user, Node, wine, WSL and `scripts/bin-target-check.py` (the loader list) stay external. `tests/tools/cross_test/check.sh`: same on 7 lanes here (below); this machine has no qemu and no WSL distribution, so Linux targets are compared built only. |
| `scripts/wasm-test.sh` | Bash, 106 | **ported**: `tools/wasm_test.fd` (Nov 25) | — | Same, for `fid wasm`; Node stays (it runs `scripts/wasm-run.mjs` and `tests/wasm/validate.mjs`). `tests/tools/wasm_test/check.sh`: same on 9 tests run under Node, and again with `REUSE=1`. |
| `scripts/wasm-run.mjs` | JavaScript, 106 | no | no | It is the WASI host: it must be Node. |
| `scripts/bare-qemu-test.sh` | Bash, 179 | **ported**: `tools/bare_qemu_test.fd` (Nov 25) | — | Same arguments, environment and output; QEMU and `diff` (the failure report) stay external. `tests/tools/bare_qemu_test/check.sh` compares the build-only modes (`RUN=0`, `BUILD=0 RUN=0`, a test that does not build) and the missing-QEMU error: same. Booting (`RUN=1`) is not compared here: this machine has no `qemu-system-x86_64` (the check adds that lane where it is on PATH). |
| `scripts/bare-run.c` | C, 113 | no | not yet | Maps a bare binary and jumps into it (Linux); Fides could with `mmap` and a call through a raw pointer, but this is the harness that checks such code. |
| `scripts/sanitize.sh` | Bash, 192 | **ported**: `tools/sanitize.fd` (Nov 25) | — | Same arguments, environment (SAN, CC, LIMIT, COMPILER, SELF, the *SAN_OPTIONS), `$OUT` files and output; the C compiler's sanitizer runtimes stay external, and so does `scripts/verify/build-variant.sh` (COMPILER=1). `tests/tools/sanitize/check.sh`: same on UBSan through `zig cc` (built, run, compared), the defaults (no clang here: every build fails, all skipped) and the SAN=thread,address rejection. ASan, TSan and COMPILER=1 are not compared here (no clang or gcc with those runtimes on this Windows machine; COMPILER=1 also builds the whole compiler); `SAN_CC=clang` adds an ASan lane where there is one. |

### Verification and fuzzing

| Tool | Language | In Fides? | Could be | Notes |
|---|---|---|---|---|
| `scripts/fuzz/gen_programs.py`, `gen_vs_c.py` | Python, 590/1,121 | **ported**: `tools/fuzz_gen.fd` (its generator in `tools/fuzz_programs.fd`) and `tools/fuzz_vs_c.fd` (Nov 25) | — | Same arguments, the same programs for a seed, output, kept files, `--log` and exit status. The numbers come from `tools/py_random.fd`: CPython's MT19937 with its int seeding, `random()`, `getrandbits`, `_randbelow`, `randint`, `choice` (checked against Python on 14,000 draws), so a seed in a log names the same program in both. `tests/tools/fuzz_gen/check.sh`: 4 runs the same, and with `GEN=1` the programs of seeds 1-400, plain and `--rich`, and two batches (802 files) byte-identical; `tests/tools/fuzz_vs_c/check.sh`: 5 runs and the `--print` pairs of 100 seeds in 4 modes the same (below). |
| `scripts/fuzz/fuzz_frontend.py`, `fuzz_parsers.py` | Python, 317/156 | **ported**: `tools/fuzz_frontend.fd`, `tools/fuzz_parsers.fd` (Nov 25) | — | Same arguments, the same mutants for a seed (cut and spliced by code point, as the scripts cut a str), findings, `findings.tsv`, corpus and `seen.bin`, `--summary` JSON and exit status. `tests/tools/fuzz_frontend/check.sh` (4 runs, 500 mutants) and `tests/tools/fuzz_parsers/check.sh` (4 runs, 2,000 inputs): the same (below). Not compared: `--cov-fid`, which needs a compiler built with `scripts/verify/cov_instrument.py`. |
| `scripts/fuzz/reduce.py` | Python, 234 | **ported**: `tools/reduce.fd` (Nov 25) | — | The script's own command line, `--bad`/`--good` included. `fid reduce` (`compiler/main.fd` cmd_reduce) does the `--cmd` and `--fails` modes with a command line of its own (`-o`, `--timeout`) and stays. `tests/tools/reduce/check.sh`: 5 reductions the same (below). |
| `scripts/fuzz/triage.py` | Python, 156 | **ported**: `tools/fuzz_triage.fd` | — | Same output on a keep directory written as gen_vs_c.py writes them and on 6 random ones (decoding, newline translation, repr, grouping). Native `emit` was blocked by `memchr` missing from the msvcrt imports (2g.58, fixed Nov 16). |
| `scripts/fuzz/nightly.sh` | Bash, 88 | **ported**: `tools/fuzz_nightly.fd` (Nov 25) | — | Same environment and steps, running the Fides ports `tools/fuzz_gen.fd` and `tools/fuzz_triage.fd` (`FUZZ_GEN`/`FUZZ_TRIAGE`, the ones beside it, or built first). `tests/tools/fuzz_nightly/check.sh`: a run where the programs agree is the same; on a run that keeps failures the generator's output and every kept file are the same but **the triage lines differ**: `tools/fuzz_triage.fd` is older than triage.py's Nov 19 changes (below) and prints nothing for a gen_programs.py keep folder. The sanitizer step (Linux and macOS with clang) is not compared here. |
| `scripts/verify/expect.py` | Python, 294 | **ported**: `tools/expect.fd` (Nov 25) | — | Every backend on a test file, checks what it must do: the same command line, output lines and exit status. `tests/tools/expect/check.sh`: 9 runs the same, 40 conformance tests included (below). |
| `scripts/verify/variants.py` | Python, 765 | **ported**: `tools/variants.fd` (Nov 25) except `--gen` | — | Builds with passes off / broken on purpose. `--gen` (programs from the fuzzers' generators) is an error in the port: it needs `tools/fuzz_gen.fd`/`fuzz_vs_c.fd` wired in. Built only (`fid ccompile`): it uses getcwd and the runtime's `fid_proc_spawn_io`, which `fid run` lacks. `tests/tools/variants/check.sh`: 19 cases the same (below). |
| `scripts/verify/coverage.py`, `cov_instrument.py` | Python, 671/232 | **ported**: `tools/coverage.fd` (except `--gen`, as variants) and `tools/cov_instrument.fd` (Nov 25) | — | `cov_instrument.py` rewrites generated C (text in, text out); `coverage.py` drives runs of a counter-instrumented compiler. `tests/tools/{coverage,cov_instrument}/check.sh`: 18 and 6 cases the same (below). coverage.fd is built only (getcwd). |
| `scripts/verify/chaos.py`, `soak.py`, `interleave.py` | Python, 358/108/207 | **ported**: `tools/chaos.fd`, `tools/soak.fd`, `tools/interleave.fd` (Nov 25) | — | Long-running children killed, restarted or starved on purpose. Sockets, a child's memory (K32GetProcessMemoryInfo; /proc/<pid>/statm), CPU affinity and local time through `extern "C"` blocks in the tools. Built and checked on Windows only: their `@cfg(unix)` parts are not compiled here. `tests/tools/{chaos,soak,interleave}/check.sh`: 7, 9 and 10 cases the same (below). |

### Benchmarks

| Tool | Language | In Fides? | Could be | Notes |
|---|---|---|---|---|
| `bench/run.sh` | Bash, 184 | no | after P | Builds five languages' programs, best of N wall time, a table. Not ported in the Nov 25 pass: its output is timings, so a port can only be compared on the table's shape and its WRONG/`-` cells, and a run builds Rust, Zig, C++ and Go too. |
| `bench/check.sh` | Bash, 169 | no | after P | Same, against `bench/baseline.txt`. Not ported in the Nov 25 pass (as `bench/run.sh`). |

### Package manager, docs, coverage (Rust binaries)

| Tool | Rust | Fides | State |
|---|---|---|---|
| `ark` | `src/ark.rs`, 881 lines | `compiler/ark.fd` 7,169, `tools/ark.fd`, `fid ark` | Fides version builds and runs (`version`, `list`); since Nov 16 the installer builds it from `tools/ark.fd` with the shipped fid.exe through C (release N8). |
| `scribe` | `src/scribe.rs`, 882 | `compiler/scribe.fd` 1,910, `tools/scribe.fd`, `fid doc` | `tools/scribe.fd` takes the Rust tool's command line (`-o`/`-f`, `--help`, `--version`, errors on stderr with exit 1); the installer builds it with the shipped fid.exe through C; `tests/tooling/tools-selfbuild-check.sh` compares it with the Rust scribe. |
| `veil` | `src/veil.rs`, 367 | `compiler/veil.fd` 742, `tools/veil.fd`, `fid veil` | As scribe: the Rust tool's command line, built by the installer through C, compared with the Rust veil by `tests/tooling/tools-selfbuild-check.sh`. Only `fid emit tools/veil.fd` (pe_ir) runs out of memory, since veil imports the whole compiler (AD1). |

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

**The target test runners** (Nov 25): `tools/cross_test.fd`,
`tools/wasm_test.fd`, `tools/bare_qemu_test.fd` and `tools/sanitize.fd`,
with what they share in `tools/target_test_lib.fd` (a child as `timeout N
cmd < /dev/null > out 2> err` runs it: no shell, stdin at its end, killed at
the limit with status 124, 127 when it cannot start; a pool that keeps
`JOBS` children running; the `// Expected output:` blocks, panic keys and
`case` patterns of the scripts). Each was checked against its script with
the Nov 25 fid (`release/fid-dev.exe`), the port built with `fid ccompile`,
both run with the same tests and environment and their output, exit status
and files in `$OUT` compared (`tests/tools/<name>/check.sh`, OUT paths
hidden):

- `cross_test`: 7 of 7 lanes the same — no target (the usage text, exit 1),
  an unknown target, `NATIVE=1 x86_64-windows` (`emit --nat`) and
  `NATIVE=1 x86_64-windows-pe` (`emit`) built and run here, `BUILD_ONLY=1
  x86_64-linux` (C through zig cc) and `NATIVE=1 BUILD_ONLY=1
  aarch64-linux` (the machine layer, with the qemu runner's loader list
  from `bin-target-check.py`) built only, and `COMPARE_ONLY=1` on the first
  lane's files; on hello, a panic test, a test with its own exit code and
  one `scripts/lane-skips.txt` leaves out off Windows. Results, run-all.sh
  and every `_rc` file the same. Not compared: running Linux binaries (no
  qemu-user and no WSL distribution on this machine) and wine.
- `wasm_test`: 9 tests (four wasm tests, a panic test, the integer
  semantics, a process test that is SKIP, an expected-fail test that is
  BUILD, a DLL test that FAILs under WASI) built, validated and run under
  Node, and again with `REUSE=1`: the same.
- `bare_qemu_test`: `RUN=0` on three bare tests and three kernel tests,
  `BUILD=0 RUN=0`, a missing QEMU (exit 2) and a test that does not build:
  the same, `.expected` files included. Booting is not compared (no
  `qemu-system-x86_64` here).
- `sanitize`: `SAN=undefined CC="zig cc"` on five tests (four clean, one
  `_fail` skipped; the binaries hold the UBSan runtime), the defaults
  without clang (all skipped) and `SAN=thread,address` (exit 2): the same.

Differences, none of which these runs reach: the ports start children
without a shell, so bash's own messages about a crashed child ("Segmentation
fault") are not printed; every child's stdin is at its end (the scripts give
/dev/null to the runs only); on Windows a child's exit code is shown as Git
Bash shows it (0xC0000005 is 139, 0xC000001D 132, 0xC000013A 130, other
0xC... codes 127, the rest the low byte), which is what the scripts write;
`cross_test` runs run-all.sh's commands itself, one at a time as bash would,
except for Linux binaries on a Windows host (`wsl bash run-all.sh`, as
before); `wasm_test` does each test's steps in three rounds (all runs and
builds, then the validations, then the Node runs), `JOBS` at a time, where
the script does one test's steps in turn; `bare_qemu_test`'s default `$OUT`
is `bare-qemu.<pid><time>` where mktemp picks a random name; `$ROOT` and
`$(pwd)` are `$PWD` (in Git Bash's /c/... form on Windows). Run them from
the repository root (the scripts change to it themselves).

**The fuzz drivers** (Nov 25): `tools/fuzz_gen.fd`, `tools/fuzz_vs_c.fd`,
`tools/fuzz_frontend.fd`, `tools/fuzz_parsers.fd`, `tools/reduce.fd` and
`tools/fuzz_nightly.fd`, with what they share in `tools/py_random.fd`
(Python's `random.Random`), `tools/fuzz_lib.fd` (os.path, a child run as
subprocess runs it: no shell, a time limit, Python's return code, the
whole tree killed by `taskkill /T` on Windows as the scripts' `_run` does;
UTF-8 decoding with U+FFFD, text-mode newlines, `repr()`) and
`tools/fuzz_programs.fd` (the programs of gen_programs.py). Each was
checked against its script with the Nov 25 fid (`release/fid-dev.exe`),
the port built with `fid ccompile`, both run with the same arguments one
after the other into the same output folder, and their output, exit
status and files compared (`tests/tools/<name>/check.sh`; the compiler's
`[prof]` timings are blurred, Python runs with `-X utf8`):

- `fuzz_gen`: `-n 2 --backends run,ccompile` (agree), `--backends
  run,check --retries 2` (the `check` backend builds nothing, so every
  program is kept, run again and its outputs written), `--rich --batch 3`
  (batches that disagree, each seed run alone) and a batch that agrees:
  the same; `GEN=1`: 802 programs byte-identical.
- `fuzz_vs_c`: `--backends run` (ok), `GEN_VS_C_MUTATE=run` with
  `--backends run,ccompile --ext2 --log` (every seed differs: `.want`,
  `.out` and the log), `--cc clang,gcc` (gcc through WSL, which has no
  distribution here: "C oracle disagrees with itself" with the results'
  `repr()`, WSL's UTF-16 message included), `--jobs 2` (lines sorted: both
  report seeds as they finish) and `--backends all --avoid none` (emit,
  `--nat`, ccompile -O2 built and run, the WSL lanes failing): the same;
  the `--print` pairs of seeds 1-100 with the default, `--ext`, `--ext2`
  and `--avoid none`: 400 identical.
- `fuzz_frontend`: `--mode run` over the reduce samples (mutants that still
  fail are findings: crash files, findings.tsv, `--summary`), the same
  again on that `--out` with `--keep` (the finding is known), `--mode
  check` and `--mode ir`: the same; 500 mutants over the 379
  `tests/bootstrap/test_s*.fd` (16 with non-ASCII text) identical.
- `fuzz_parsers`: all nine parsers, three of them, `--timeout 0` (every
  input a finding, one kept per parser) and `--backend check` (no driver:
  the build's error, exit 2): the same; 2,000 inputs identical. No parser
  crashed in 1,500 inputs, so a crash's kept files are compared only
  through the timeout findings.
- `reduce`: `--fails run` (with and without `--match`), `--cmd "<fid> check
  {}" --match error`, `--bad run --good run` on a program that prints the
  clock, and an input that does not fail: output, exit status and reduced
  file the same.
- `fuzz_nightly`: `N=2 FUZZ_BACKENDS=run BATCH=2` the same; `N=2
  FUZZ_BACKENDS=run,check`: the two generator runs and all kept files the
  same, the triage lines not (next paragraph).

`tools/fuzz_triage.fd` (Nov 16) is older than scripts/fuzz/triage.py's
Nov 19 changes, so it no longer gives the script's output: without a
`.want` file the script takes the majority's result (gen_programs.py keep
folders), groups failures that point at the machine under "environment?",
adds `[likely M25]`/`[maybe M24]` hints, says "exit N, expected M" and
"want" (the port: "C exits", "C") and ends with two "seeds with ..."
lines. Bringing it up to date is left (it was outside this pass's files).

Differences of the fuzz ports, none of which these runs reach: children's
output is decoded as UTF-8 (Python on Windows uses the console's code page
unless in UTF-8 mode, and then fails with UnicodeEncodeError writing a
U+FFFD to a `.want`); `\d`, `\s` and `strip()` know ASCII and the common
Unicode spaces; an option is not taken from an abbreviation and a bad
value is a one-line error with exit status 2 (argparse prints its usage);
scratch folders are named `<prefix><pid>_<time>` where tempfile picks a
random name; `fuzz_frontend` and `fuzz_parsers` give the compiler our stdin
(the scripts close it: run them with stdin at its end); `fuzz_vs_c --jobs
N` runs N copies of itself, one seed each, where the script runs N
threads; `reduce --cmd` with a double quote in the command goes through a
temporary `.bat` on Windows. `fs_mkdir_all` cannot make `C:/x/y` (it tries
to make `C:` and stops: `fs_is_dir("C:")` is false), so `fuzz_lib.fd` has
its own `fl_mkdirs`.

**The gate and verification drivers** (Nov 25): each checked against its
original with the Nov 25 fid (`release/fid-dev.exe`), the port built with
`fid ccompile`, both run with the same arguments and environment
(`tests/tools/<name>/check.sh`; times, dates and temporary folders masked
where a run has them). Drivers that write beside themselves (build/,
docs/perf/, baselines) are compared in a scratch copy of the tree, with
`FID_HOME` pointing at the repository. On Windows the ports find Git's
bash for the scripts they still run (BASH_EXE, else the first bash.exe on
PATH that is not WSL's).

- **`tools/expect.fd`** against `python -X utf8 scripts/verify/expect.py`:
  9 runs the same. `tests/tools/expect/sample/` (//> lines that hold and
  ones that do not: an exit status, a line with a quote and a tab, a
  missing line; an `// Expected output:` block, trimmed, with a `...` line
  and `// Expected exit:`; a panic; `//! error:` with the right message,
  another and none; a `_fail.fd` file; an `expect-error` file that
  compiles; backends that agree and ones that do not; `//@ env:` and
  `//@ backends:` with `checked`; a subfolder and a `_modules` folder) on
  run, ccompile and emit at `-j 2`, then `-q -j 1`, glob patterns,
  `--backends=` and an abbreviated `--back`, one file, and argparse's
  errors (no paths, a bad `-j`, a missing value, an unknown option); and
  40 conformance tests (`FULL=1`). `-j n` runs the tool once per file (a
  hidden `--ex-one`). Differences, none of which these runs reach: the
  script's lock around a compile's second attempt is not kept; a glob's
  `*` and `?` match case-sensitively and only in its last component; a
  program a signal killed is `exit 128+n` (Python: `-n`); a compiler that
  cannot start fails the file (the script stops with a traceback); built
  only (the `checked` backend needs getcwd).
- **`tools/leak_check.fd`** against `bash scripts/leak-check.sh`: 3 runs the
  same, with each program's `.out` and `.status` in the two report
  folders: two of tests/leaks with a program that leaks a block, one whose
  output differs from a CRLF `.expected` and one a CRLF `.expected`
  matches; every tests/leaks program; `NATIVE=x86_64-windows JOBS=2`. With
  `fid run --leaks`, JOBS above 1 races in both (the fixed temporary file
  the script warns of). The report folder is `<temp>/tmp.<pid><time>`
  (mktemp picks a random name). Found on the way: with this fid,
  await_results, empty_drops and regions fail the leak gate.
- **`tools/stale_cache_test.fd`** against `bash scripts/stale-cache-test.sh`:
  5 runs the same — the real compiler (PASS); a stand-in compiler whose
  every build is a program printing 42 (the cache never invalidates:
  "FAIL: dep change not picked up"), the same saying "up to date"; one that
  fails at once (its exit status, no message, as `set -e` does); one that
  does not exist (127). The compiler is taken relative to the current
  folder, as the script's `$OLDPWD/$FID`; an absolute one is used as it is
  (the script fails there).
- **`tools/rules_smt.fd`** against `bash scripts/rules-smt.sh`: 5 runs the
  same — the 253 real obligations through z3's Python module (no z3
  command here: 1m44 for the script, 46 s for the port); a stand-in solver
  that leaves 27 unproved (`unknown`, `sat` with CRLF; exit 27); no solver
  and no Python on PATH (exit 2); a compiler that fails (`false`); one that
  writes no obligations (the glob's pattern is then the one file, as in
  bash).
- **`tools/compile_gate.fd`** against `bash scripts/compile-gate.sh` and
  `bash bench/compile-check.sh`: 13 runs the same, with the baselines and
  `run.txt`, on a stand-in compiler whose `[time]` lines (stages of one and
  of several words, a line that is not a time) change from build to build
  so that the best of three counts: no baseline (saved), `--save`, a
  baseline with stages over the limit, under MIN_MS, missing and 0, other
  LIMIT_PCT/MIN_MS, a build that fails; compile-check's `--save`, a slow
  stage, new stages and a faster total, another limit, no baseline, other
  FILES, no compiler; and the real compiler (`RUNS=1` and `RUNS=2`, times
  masked). compile-check.sh reads a stage line with `read -r stage ms`, so a
  stage of several words keeps its first run's time and bash complains
  ("line 51: [: shadowed imports 2: integer expression expected"); the port
  prints the same lines. Not emulated: bash's errors on numbers that are
  not numbers in a baseline.
- **`tools/me_measure.fd`** against `bash scripts/me-measure.sh`: 3 runs the
  same (the iropt and build times masked; instruction counts, same/DIFF and
  totals compared): hello, test_hashmap, a program printing its own
  executable's name (DIFF) and a `_fail` test that does not build, with the
  default OFF and with `OFF=dce,gvn`; no label (bash's `${1:?}` message,
  given with the script's name). The date comes from strftime through
  `extern "C" include "<time.h>"`.
- **`tools/nat_gate.fd`** against `bash scripts/nat-gate.sh` in a copy of
  the tree with three of the subset's tests: 3 runs the same on
  x86_64-windows (`emit --nat`) — with a stand-in z3 that leaves 27 rules
  unproved; with `REQUIRE_Z3=1` and neither z3 nor Python on PATH; with
  `ALL=1 JOBS=1`. Not reached here: a FAIL or BUILD line, a verifier
  rejection, REQUIRE_LOADER (no qemu-user, no WSL distribution).
- **`tools/pass_matrix.fd`** against `bash scripts/pass-matrix.sh`: 4 runs
  the same, with matrix.txt — `SELFTEST=1` (its six made-up cases, the
  e-graph families read from compiler/egraph.fd, the 27 rules of rules.fd,
  RESUME); no target (the script's header, kept in the tool); a matrix on
  x86_64-windows with `IROPT="dce gvn" NAT=fold IME=dce EGRAPH=comm RULES=0`
  over test_hashmap and a test printing its own path (every switch
  "differs" by an "output differs:" line); the same with `RESUME=1`.
  Lists are compared in byte order, as sort and comm in Git Bash's
  C.UTF-8 locale; `diff`'s output is rebuilt from the two sorted lists
  (between two lines both have, the base's own lines, then the run's).
- **`tools/variants.fd`** against `python scripts/verify/variants.py`, with
  no compiler built: run-time switches on the given fid; the compilers of
  constant toggles and mutants put in `<out>` first (a copy of fid, or
  `tests/tools/variants/fakecc.fd`, whose every build prints something
  else), one built from an empty tree (it fails at once). list and check
  on the tree and on an empty tree; emit and nat toggles at -j 2 over four
  samples (a binary the pass changes with the same output, an `//@ env:`
  line, a flaky one, one that does not compile); --prune-stats
  --prune-check 0.5 --resume, run twice (the second resumes everything);
  --sample through both of random.sample's ways (the same files as
  Python's Mersenne Twister picks); mutants killed, surviving and BUILD
  FAILED; a constant toggle that differs; an --only with no match;
  argparse's errors: stdout, exit status, the report and the sorted
  --resume lines the same in all 18 cases, and a nineteenth checks the
  port's TOGGLES and MUTANTS tables equal the script's field by field.
  -j runs the tool once per test and backend (a hidden `--va-item`); the
  script's lock around a second compile is a directory in `<out>/work`.
  Differences: --gen is not ported (an error, exit 2); built only (getcwd,
  `fid_proc_spawn_io` for a closed stdin); a program a signal killed exits
  128+n; one that cannot start counts as exit 127 (the script stops with
  FileNotFoundError); a --resume line without variant, test or backend is
  skipped (KeyError in the script); argparse's texts are those of an
  80-column terminal. Found on the way: 3 of the script's edits are stale
  on the current tree (rules-sub-zero-left, rules-lt-self-true,
  iv-sub-as-add): the weekly run reports them as BUILD FAILED.
- **`tools/coverage.fd`** against `python scripts/verify/coverage.py`, with
  a real counter compiler standing in (`tests/tools/coverage/fakefid.fd`,
  dumped to C with line marks, instrumented by `tools/cov_instrument.fd`,
  built with zig cc): every mode kind (check, run, emit, emit:<target>,
  nat, dump-c, cmd:<cmd>+<arg>), the line and block goals, a second run
  adding to merged.cov, 51 runs at -j 3, --limit, --report-only with
  --goal-modules all, --record and --note, --all-sources, a block-only map
  (the estimate table; --goal-lines exits 2), a missing bitmap, an empty
  map, --self-test and argparse's errors: stdout, exit status, report.txt,
  never-called.txt, uncovered-lines.txt and merged.cov the same in all 18
  cases. Differences: --gen is not ported; built only (getcwd); ints and
  floats read in plain forms only; --record's date from PowerShell's
  Get-Date on Windows and `date` elsewhere. Found on the way: the counters'
  file name (rand() seeded from time(0), clock() and an address that is the
  same in every run on Windows) is the same for runs started in the same
  second, so their bitmaps overwrite each other in FID_COV_DIR (both
  instrumenters write this C).
- **`tools/cov_instrument.fd`** against `python
  scripts/verify/cov_instrument.py`, on `dump-c tests/bootstrap/hello.fd`
  (1.8 MB, 935 functions) plain, with `FID_C_LINE_MARKS=1`'s `/*@L<n>*/`
  marks, with the marks as `#line` directives and every 997th line a
  `#line` back to a .c file, and that without the directive after each
  block label: byte-identical C, the same counter map and summary line;
  `--self-test` and a missing argument agree (6 cases). On 19 MB of C both
  take 0.7 s. `\s` and `\d` are ASCII; a missing argument or file is a
  one-line message instead of a traceback.
- **`tools/chaos.fd`** against `python scripts/verify/chaos.py` (each
  scenario's seconds and the --out names masked, NULs dropped): the host
  scenarios on ccompile (nine identical lines and report.txt, "OPEN C1
  silent server ... hung" and the stack exhaustion included), emit with
  --strict (exit 1), --linux with no host backend (the x86_64-linux build,
  full disk, out of memory, and chaos_linux.py through wsl, which fails at
  once here and is printed the same), a driver that does not compile, a bad
  --linux-out (exit 2), -h and a bad option: 7 cases the same. The test
  servers are children of the tool (a hidden `--ch-serve <kind>`) on
  127.0.0.1 and a free port; SO_LINGER 0 gives the reset.
- **`tools/soak.fd`** against `python scripts/verify/soak.py` (psutil
  7.2.2; numbers masked, the CSV compared by its header and row count):
  soak_service.fd for 6 s on ccompile and emit (PASS), a service that stops
  after three heartbeats, one whose memory grows (the MB/hour line), one
  that does not compile, -h and two argparse errors, and (SLOW=1) one that
  stalls for 70 s: 9 cases the same. Resident memory is
  K32GetProcessMemoryInfo's working set on Windows (psutil's rss) and
  /proc/<pid>/statm on Linux; the child's stdout goes to a file read as it
  grows, where the script reads a pipe in a thread.
- **`tools/interleave.fd`** against `python scripts/verify/interleave.py`
  (the --out names, seconds and --record's date masked): Python's
  `random.Random(seed)` drawn in the same order, so `--seeds
  1,2,3,17,42,99` gives the same CPU sets and load flags; -n 6 on ccompile
  and emit with --record, --sched-random, a program that never prints its
  expected block (five .out files kept, the replay line), an exit-3 one, a
  hanging one killed at --timeout 2, one that does not compile, and
  argparse's errors: 10 cases the same. Pinning is SetProcessAffinityMask on
  Windows and `taskset -p` on Linux; each busy loop is a child of the tool
  (a hidden `--il-burn`), which loads a full CPU where the script's
  threads share one under the GIL.
- **`tools/enc_oracle.fd`** against `python scripts/enc-oracle.py`, each in
  its own copy of compiler/ (build/enc-oracle and `use compiler::nat_enc`
  are resolved from the current folder): the real run ("1200 forms, 0
  mismatches") with every file under build/enc-oracle byte-identical (the
  generated program, the three .s files and zig's three objects); a
  nat_enc.fd with three encoders broken (the same three MISMATCH lines,
  exit 3); `--zig false` (exit 1); a missing fid or zig (the traceback's
  last line, the system's own text); -h and a bad option: 7 cases the same.
- **`tools/gen_brotli_dict.fd`** against `python scripts/gen-brotli-dict.py`
  (the dictionary itself is not in the tree: the check decodes it from
  stdlib/brotli_dict.fd's literals): byte-identical output (165,978
  characters) and the same "round trip ok" line; one byte flipped (the
  sha256 warning); a short file; no argument (the docstring, exit 1); a
  missing file, a directory, a missing output folder (one line where
  Python prints a traceback ending in it): 7 cases the same. Found on the
  way: stdlib/brotli_dict.fd differs from what both generate today from
  line 15 on (it was reformatted after it was generated).

The tools that use `extern "C"` blocks for what the stdlib lacks
(chaos, soak, interleave: sockets, process memory, affinity, local time;
me_measure: local time; expect, stale_cache_test, variants, coverage:
getcwd) are built with `fid ccompile`; `fid run` has none of them.

All the ports write `\n` where Python on Windows writes `\r\n`.
