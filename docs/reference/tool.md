# Fides Development Tools

> Every serious language needs a toolchain. These are the names for Fides's
> standard development tools — each chosen to reflect the language's identity:
> **trust, safety, verification, and formal correctness.**

## Tool Mapping

| Tool             | Rust             | Go            | Python     | Fides       |
|------------------|------------------|---------------|------------|-------------|
| Package manager  | Cargo            | go mod        | pip        | **ark**   |
| Formatter        | rustfmt          | gofmt         | black      | **forge**   |
| Linter           | clippy           | staticcheck   | pylint     | **sentinel**|
| LSP / IDE        | rust-analyzer    | gopls         | Pylance    | **oracle**  |
| Debugger         | gdb / lldb       | delve         | pdb        | **probe**   |
| Profiler         | perf             | pprof         | cProfile   | **pulse**   |
| Doc generator    | rustdoc          | godoc         | Sphinx     | **scribe**  |
| Coverage         | tarpaulin        | go test -cover| coverage.py| **veil**    |
| Test framework   | cargo test       | go test       | pytest     | **trial**   |

---

## `fid` commands (Nov 9, 2026; states corrected Nov 21)

The self-hosted `fid` has every tool built in; there are no separate
binaries. Dispatch is in compiler/main.fd (docs/file-analysis/main.md).

| Command | What it does | State (Nov 9) |
|---|---|---|
| `run <file>` | IR interpreter. `--leaks` / `--checked-alloc` rebuild through C (`ccompile --debug`, needs `CC`) and run with the debug allocator | works |
| `ccompile` (`compile`) `<file> -o <out>` | C, then a C compiler. `--target`, `--cc-opt`, `--debug`, `--release`, `--inc`, `--no-cache`, `--profile`, `--leaks`, `--checked-alloc` | works |
| `emit` / `build <file> -o <out>` | native: Windows PE (pe_ir) by default on Windows; `--target aarch64-linux\|riscv64-linux\|x86_64-linux` and `--nat` use the machine layer (also written OS first: `linux-arm64`, `linux-x64`, `macos-arm64`; TL6); `--target windows-x64` on any host is a pe_ir PE; `--bare`, `--elf`, `--boot`; through C on a POSIX host without `--target`; `--no-opt`, `--debug`, `--checked-alloc` | works; since Nov 17 (MI18) `--native` is the machine layer on every target and `--boot` the bootable image of pe_ir |
| `dump-c`, `split-c <c> <prefix> <n>` | generated C to stdout; cut a C file into a header and n units | works |
| `check <file> [--time]` | front end only: types, borrows, escapes | works |
| `lint <file>` | unused functions and imports, `unwrap()` outside tests | works (does not resolve `compiler::` imports) |
| `fmt <file> [--check] [-w]` | formatter: prints the formatted source; `-w` rewrites the file, `--check` is a pass/fail (CRLF endings are not a difference) | corrupted files until Nov 15 (known-bugs 2g.6, fixed) |
| `test [<file>] [--fail]` | a file's `test_*`/`bench_*` functions, or the suite / expected-fail list | works on Windows; without a file broken on POSIX |
| `fuzz <file> [--seeds N]` | a test file's `fuzz_*(data: Vec<u8>)` targets on N deterministic inputs (1000; `test --fuzz`: 100), coverage-guided (the harness is built with `ccompile --fuzz-cov`); failing inputs are saved under `testdata/fuzz/<target>/` and replayed by every `fid test`: [docs/guides/fuzzing.md](../fuzzing.md) | new Nov 15 (TL1), checked by scripts/tl-check.sh |
| `fix <file> [--check]` | applies the suggested fixes (`val` -> `var`, a narrowing `as T`, a misspelled name, an unused import), each kept only when the program then has fewer errors (or as many, none on the fixed line); `--check` writes nothing | new Nov 15 (TL3) |
| `test-rules [--smt <dir>]` | checks the rewrite rules on values; writes SMT obligations | works |
| `qualify <file>` | safety evidence as JSON | works |
| `audit <file> [--stdlib] [--strict]` | unsafe code a program reaches and its `// SAFETY:` comments | works |
| `doc` / `scribe` | documentation generator; `doc <dir>`: a site, a page per module and an index with search (TL11, Nov 15) | works, buggy (CRLF) |
| `veil` | coverage | crashed (Double free) until Nov 15 (2g.2, fixed); `fid test --coverage` still double-frees when the covered program panics (2g.30) |
| `lsp` | language server: diagnostics, completion, hover, definition, rename, references, formatting, signature help, inlay hints (parameter names), code actions (the fixes of `fix`) | symbol requests fixed Nov 15 (2g.5); rename and references are by whole word in one file |
| `ark ...` | package manager: path and git pacts, `use` finds them in ark_modules/, ark.lock records git commits and file hashes, `install` checks out the locked commit, `audit` reports what a build pulls in | TL9, Nov 15; a pact's own dependencies resolved to one version each, a per-machine cache, dev-dependencies, an API check on publish (docs/reference/ark.md) |
| `parse-header <h>` | C header to Fides bindings | works |
| `profile <file>` | `ccompile --profile`, run, top-20 table | works |
| `bench <file>`, `watch` | run an executable N times (`watch` is the same as `bench`) | works |
| `repl` | compile and run each line with tcc/gcc | not checked Nov 9 |
| `tokens`, `ast` (`dump-ast`, `dump-ast-raw`, `dump-ast-flat`), `ir`, `verify`, `sourcemap` | dumps (the four `ast` forms print the same summary) | work |
| `opt <f> [--fn <n>]` | the IR after the IR optimizer the machine layer runs (iropt_native) | Nov 17 (llvm MI18; `pir`/`dump-pir` were retired with the PIR path and name this instead: docs/reference/pir-retired.md) |
| `codegen <f> [--target t] [--fn <n>] [--elf/--coff/--macho <o>]` | the machine layer: a disassembly, or an object file as `emit --obj` | Nov 17 (llvm MI18) |
| `wasm <file> -o <out.wasm>` | native WebAssembly (WASI) | modules validate and run; 316 of 344 suite tests match `fid run` (Nov 15, 2g.21) |
| `build-x86`, `gpu` | experimental back ends | `gpu` rewritten Nov 15, not yet built; `build-x86`'s exe untested (2g.16, 2g.21; platforms.md, Tier 3) |
| `disasm`, `dump-mir`, `emit-ir`, `explain`, `serve`, `reduce`, `ir-fuzz`, `inspect`, `replay` | machine code and machine IR, `.fir` to the machine layer, what the optimizer did (CP3), a long-lived checker ([serve.md](serve.md)), shrinking a failing program, IR fuzzing, a crash report, a `run --record` trace run again (CT7) | new Nov 10–19 |
| `notebook` | HTML page of a file's code blocks (evaluates nothing) | works |
| `version` | version string (`1.0.0+dev` on a dev build, compiler/build_info.fd) | works |

Global flags: `--borrowck=off|on` (`warn` prints nothing), `--timeout N`
(parsed, not enforced); arguments after `--` go to the program. Environment:
`CC`, `FIDES_CC_OPT`, `FIDES_CFLAGS`, `FIDES_NO_CACHE`, `FID_HOME`,
`FID_VERIFY`, `FIDES_COW_CHECK`, `FIDES_INC_FLAT`, `FIDES_INC_EFFECTS=off`
and `FIDES_INC_DEBUG` (`ccompile --inc`: one unit; every prototype in
every unit; print each unit's prototype count; checks in
scripts/inc-check.sh), and the pass switches
`FID_IROPT_OFF`/`FID_IROPT_ON`/`FID_NAT_OFF`/`FID_RULES_ON`
(docs/reference/architecture.md, "Self-hosted pipeline"); the flag
`--no-pass=a,b` (or `--no-pass a,b`) adds names to the off lists of
iropt, iropt_me, the rules (`rules`, `purecse`) and the e-graph
(`egraph`, `forward`) for that command; `FID_IROPT_OFF=rangejoin` (or
`--no-pass=rangejoin`) turns off the range facts met at joins in
compiler/checks.fd (LF8/LF9), on every path, C included;
`FID_IROPT_OFF=licmcall` keeps calls to effect-free functions in a loop's
header where they are (`licm` moves them out otherwise, OP25). A program
built by `fid emit` for Linux (x86-64, AArch64, RISC-V) profiles itself
when run with `FID_PROF=1`: a sample per millisecond of CPU time, and at
exit a table on stderr of the functions by self and total samples
(llvm-roadmap DB9; scripts/prof-check.sh). Not in `fid`
(`fidesc` only): `debug`, `autopsy`, `diff-test`, `link`, `sbom`, `spdx`
(`fid replay` runs a trace of `fid run --record`, CT7).

---

## Name Rationale

### ark -- Package Manager

An **ark** is a vessel that carries and protects precious things. In Fides,
the ark carries **pacts** — binding agreements of trust from the community.
When you add a dependency, you load a pact into your ark. The metaphor
parallels Rust's Cargo + Crates: ark is the vessel (like cargo), pacts are
the packages (like crates). Package manifests are `ark.toml`, lock files are
`ark.lock`, and modules install to `ark_modules/`.

**Status: ✅ Implemented (Rust + self-hosted)** — Two implementations:
- **Rust ark** (`src/ark.rs`, `src/bin/ark.rs`) — the bootstrap version
- **Self-hosted ark** (`compiler/ark.fd`) — written in Fides and built into
  `fid`: run it as `fid ark <command>` (ark.fd has no `main` of its own).
  Nov 9: a directory path dependency is reported as missing on Windows,
  `lock` drops dependencies silently, and `update <name>` ignores the name
  (docs/file-analysis/ark.md). Fixed Nov 15 (TL9): a directory pact is
  copied (xcopy on Windows, `cp -R` elsewhere), `lock` names each pact that
  is not installed and fails, `update <name>` updates that pact only.

Supports path dependencies (files and directories), git dependencies (clone
with `--depth 1`, branch/tag support), lock file generation, and a custom
TOML parser (zero external dependencies). The package index is a Git
repository (`ark search`, `ark publish`, `ark add <name>`; live since
Nov 21 as pledgeandgrow/pacts, docs/reference/ark.md).

```
ark init [name]              # create ark.toml
ark add <dep> --path <path>  # add path dependency
ark add <dep> --git <url>    # add git dependency
ark install                  # install all deps to ark_modules/ (git: the locked commit)
ark list                     # list dependencies
ark remove <dep>             # remove dependency
ark lock                     # ark.lock: git commits, fnv hashes of path pacts
ark audit [--files]          # each pact against ark.lock; lines, unsafe, extern, effects
ark update [dep]             # newest commits, installed and locked
ark add <dep> --version 1.2  # a package of the index (ARK_INDEX), `fid ark` and Rust ark
ark vendor                   # copy every pact into vendor/ + vendor/vendor.lock: offline builds
ark search <query>           # packages of the index whose name holds <query>
ark publish [--git <url>]    # add this package's tagged version (v<version>) to the index
                             #   (a minor/patch version must keep the public API: CU5)
ark api [--against <rev>]    # the public API; what changed since <rev>
ark init app --author A --description D --license MIT   # [package] fields (L5A)
ark add <dep> ... --dev      # [dev-dependencies]: ark_modules/.dev/ (L5A)
ark install --no-dev         # without the dev-dependencies
ark add <dep> ... --features a,b   # features = [...]: ark_modules/.features (L5A)
ark build [names] [--release] [--dry-run]  # every [[bin]]/[[lib]]/[target] with fid (L5A)
ark test [names] [--dry-run] # fid test on every [[test]] target (default tests/*.fd)
ark targets                  # the build targets
ark prebuild [--target <t>]  # the lib as a prebuilt library: prebuilt/<target>/ (L5E)
ark add <dep> ... --binary   # binary = true: link its prebuilt library, not its source
ark install --binary|--source  # every pact's prebuilt library, or none
# ark update: newest version within each ^/~ constraint too (L5C)
# in a [workspace] root: install/list/lock/update/audit/vendor/build/test
# run in every member (L5F; --no-workspace: the root only)
```

Nov 15 (docs/reference/ark.md): `ark install` also installs the pacts the
pacts need, one version of each (the newest every requirement accepts; a
conflict names each path that asks), keeps one copy of each git
repository per machine (`ARK_CACHE`, else `~/.ark/cache`) that project
checkouts borrow, and puts dev-dependencies in `ark_modules/.dev/`.
Checks: `tests/ark/ark-features.sh`.

`fid ark` (Nov 15) has the index commands of the Rust ark: the index is a
Git repository (`ARK_INDEX`, else the default index), copied to
`<ARK_HOME or home>/.ark/index`, one file per package, one line per version
(`version git-url tag commit`). A registry pact is installed at its tag and
refused when the tag no longer points at the recorded commit. `ark add`
warns about a name one edit away from a known one. `use <name>` looks in
`vendor/` before `ark_modules/`; `ark install` and `ark audit` check each
vendored copy against `vendor/vendor.lock`. `ark audit` also lists each
pact's C libraries and the capabilities its imports ask for.

Nov 16 (docs/reference/ark.md): a workspace root resolves its members'
pacts together, installs each one once in the root's `ark_modules/` and
records it once in the root's ark.lock (L5F). `ark yank <version>
[--undo]` keeps a version out of new locks, while existing locks keep it
(N6). `[capabilities]` in ark.toml grants each pact what it may use, and
install, build, test and audit fail when a pact uses more. `ark update`
prints the capability diff (CD1). Checks: `tests/ark/ark-registry.sh`,
and `tests/ark/ark-selfbuild.sh`, which builds `tools/ark.fd` with fid
and runs every ark check against that build.

### forge — Formatter

A **forge** shapes raw metal into a precise form. Code formatting is the
same act — taking raw, unshaped source and forging it into a canonical
shape. The blacksmith metaphor fits Fides's craftsmanship ethos: code is
crafted, not just written.

**Status: ✅ Implemented (as `fidesc fmt`)** — The formatter is available
as a `fidesc` subcommand. See `src/fmt.rs`.

```
fidesc fmt <file.fd>           # format a file (current implementation)
forge format src/              # format a directory (future standalone)
forge check src/               # check without modifying (CI mode)
```

### sentinel — Linter

A **sentinel** is a guard that stands watch. The linter is the sentinel
of your codebase — it watches for patterns that won't fail compilation
but indicate bugs, style violations, or unsafe constructs. Fides's safety
annotations (`@audit`, `@medical`, `@crashfree`) are sentinels at the
type level; `sentinel` is the sentinel at the lint level.

**Status: ✅ Implemented (as `fidesc lint`)** — The linter is available
as a `fidesc` subcommand. Detects unused functions and unused imports.
See `src/main.rs` (lint command).

```
fidesc lint <file.fd>          # lint a file (current implementation)
sentinel check src/            # lint a directory (future standalone)
sentinel fix src/              # auto-fix where possible
sentinel --strict              # treat warnings as errors
```

### oracle — LSP / IDE Server

An **oracle** answers questions. The LSP server is the oracle for your
IDE — it answers "what type is this?", "where is this defined?", "what
completes this identifier?". The name is already used informally in the
Fides codebase (the interpreter is called "the reference oracle"). Now
it's the official name for the language server.

**Status: ✅ Implemented (as `fidesc lsp` and `fid lsp`)** —
`fid lsp` (compiler/lsp.fd) publishes diagnostics and has completion,
hover, definition, rename, references, formatting, signature help, inlay
hints and code actions; until Nov 15 completion, hover, definition and
rename ended with "Double free" and diagnostics sat at line 0 (known-bugs
2g.5, fixed). The LSP server below is `fidesc lsp` (see `src/lsp.rs`). It implements JSON-RPC 2.0 over stdio
with no external dependencies (built-in JSON parser/serializer). Supports:
`initialize`, `initialized`, `shutdown`, `exit`, `textDocument/didOpen`,
`didChange`, `didClose`, `publishDiagnostics`, hover, goto-def, completion,
references. Future work: rename, inlay hints, code actions, semantic tokens.

```
fidesc lsp                     # LSP over stdio (current implementation)
oracle --stdio                 # LSP over stdio (future standalone binary)
oracle --socket :9090          # LSP over TCP (future)
```

### probe — Debugger

A **probe** is an instrument for investigating something. Debugging is
probing — you insert probes into running code to observe state, trace
execution, and find the root cause of bugs. The name suggests precision
and surgical investigation, not brute force.

**Status: ✅ Implemented (as `fidesc debug`)** — The interactive debugger
is available as `fidesc debug` (see `src/debugger.rs`). Supports
breakpoints, step, next, continue, print, backtrace, list, IR view,
effects/regions/capabilities panels, watch variables, and hot-reload.

```
fidesc debug <file.fd>         # interactive debugger (current implementation)
probe run tests/bootstrap/test_borrowck.fd  # (future standalone)
probe break main:42
probe step
probe inspect v
probe watch &mut v
```

### autopsy — Native Debugger

An **autopsy** is a post-mortem examination. The native debugger examines
a compiled binary using ptrace (Linux), showing CPU registers and
execution state. The name evokes examining something that has stopped
running to find the cause.

**Status: ✅ Implemented (as `fidesc autopsy`)** — The native debugger
compiles Fides source to a native binary (via C codegen + gcc -g -O0)
and runs it under ptrace. Commands: `run`, `step`, `continue`, `regs`,
`quit`. Shows CPU registers (RAX-R15, RIP, RSP, RBP). See
`src/native_debugger.rs`. (Also available as `fidesc native-debug`.)

```
fidesc autopsy <file.fd>       # native debugger via ptrace
```

### pulse — Profiler

A **pulse** is the rhythmic heartbeat of a living thing. The profiler
measures the pulse of your code — CPU cycles, memory allocations, call
frequency, hot paths. A healthy codebase has a steady pulse; a sick one
has irregular beats (spikes, leaks, stalls).

**Status: ✅ Implemented (as `fidesc profile`)** — The profiler is
available as a `fidesc` subcommand. Per-function timing in interpreter
mode. The benchmark tool (`fidesc bench`) is also available for timing
statistics over N runs. Full profiling (CPU, memory, flame graphs) is planned.

```
fidesc profile <file.fd>        # per-function timing profiler (current)
fidesc bench <file.fd> [-n <count>] [--ccompile]  # benchmark (current)
pulse --cpu                     # CPU profiling (future)
pulse --mem                     # memory profiling (future)
pulse --flame                   # flame graph output (future)
```

### scribe — Documentation Generator

A **scribe** is a person who copies documents and writes manuscripts by
hand, historically in monasteries. Before the printing press, scribes
preserved and produced written knowledge. The documentation generator
"writes" documentation from your source code, just as a medieval scribe
would carefully produce a manuscript. The word comes from Latin *scriba*
(writer), the same root as "script" and "describe".

**Status: ✅ Implemented** — The documentation generator is available as
the `scribe` binary (see `src/scribe.rs`). Parses `.fd` source files,
extracts `///` doc comments, and generates styled HTML or Markdown
documentation. Supports single files and entire directories. Extracts
function signatures, parameters, return types, struct fields, enum
variants, and impl methods.

```
scribe <file.fd>                          # generate HTML docs
scribe <file.fd> --format markdown        # generate Markdown docs
scribe <dir>/ --output docs/              # generate docs for a directory
scribe <dir>/ --format markdown           # Markdown for a directory
```

### veil — Coverage Tool

A **veil** is a piece of fabric that covers or conceals. The coverage tool
reveals what's behind the veil — which lines, branches, and functions are
actually tested. "Lifting the veil" on your code coverage. The word comes
from Old French *veile* / Latin *velum* (sail, curtain, covering).

**Status: ✅ Implemented** — The coverage tool is available as the `veil`
binary (see `src/veil.rs`). Instruments the interpreter with a debug hook
to track which functions, source lines, and IR instructions are executed.
Reports coverage as text (with `+`/`!` source markers) or HTML (with
colored lines and summary cards).

```
veil <file.fd>                            # text coverage report
veil <file.fd> --format html              # HTML coverage report
veil <file.fd> --output coverage/         # custom output directory
```

### trial — Test Framework

A **trial** is a formal examination of evidence. Testing is putting your
code on trial — you present evidence (test cases), the framework examines
them, and the verdict is pass or fail. The legal metaphor fits Fides
perfectly: contracts (`given`/`ensure`), audits (`@audit`), and now trials.
Every test run is a trial where your code must prove itself.

**Status: ⚠️ Partial (as `fidesc test`)** — The test runner is available
as `fidesc test` (runs `.fd` test files across all 3 backends: interpreter,
C codegen, PE emitter). `trial` will be the standalone test runner with
richer features (watch, bench, coverage via veil, parallel).

```
fidesc test                   # run all tests (current implementation)
trial run                     # run all tests (future standalone)
trial run --borrowck=on       # run with borrow checker enabled
trial watch                   # re-run on file change
trial bench                   # run benchmarks
trial --fail-only             # show only expected-fail tests
```

### audit — IR Verifier

An **audit** is a formal examination of records. The IR verifier audits
the compiler's intermediate representation for correctness — checking
that blocks have terminators, types are consistent, and instructions are
well-formed. (Also available as `fidesc verify`.)

**Status: ✅ Implemented (as `fidesc audit`)** — See `src/main.rs`.

```
fidesc audit <file.fd>        # verify IR correctness
```

In `fid` the name means something else: `fid audit <file.fd> [--stdlib]
[--strict]` lists the unsafe code a program reaches.

### qualify — Safety Evidence

**Status: ✅ Implemented (`fid` only)** — `fid qualify <file.fd> [-o out.json]`
writes JSON evidence: compiler and source hashes, the check result, the
annotated functions, and each safety rule with its requirement and tests
([safety-rules.md](safety-rules.md)). The verdict is always `NOT_QUALIFIED`:
the file is evidence, not a certificate. Fides is not suitable for medical,
automotive or avionics use.

```
fid qualify <file.fd> -o evidence.json
```

### Memory checks and other test tools

| Command | Status |
|---|---|
| `fid run <file.fd> --leaks` | The C runtime's debug allocator reports blocks never freed |
| `fid run <file.fd> --checked-alloc` | The debug allocator detects use after free |
| `fid emit <file.fd> --checked-alloc` | Written, not run: every block on pages of its own, ending at a guard page; freed pages inaccessible, never reused (pe_ir, and since Nov 16 the machine layer's targets: stdlib/linux_runtime/base.fd) |
| `fid emit <file.fd> --target <linux/macos target> --leaks` (or `--nat`) | Written Nov 16, not run: the blocks the program never freed, reported at exit by where they were allocated, as `fid run --leaks` does (RL16) |
| `fid test <file.fd>` | Runs the file's functions whose names start with `test_` and `bench_` |
| `fid test <file.fd> --filter <text> --jobs <n>` | Only the names holding `<text>`; tests `n` at a time, output in order. Each test runs with FIDES_TEST_NOW and FIDES_TEST_SEED fixed (`--real-clock`, or a test named `*_real_clock`, opts out) |
| `fid run <file.fd> --trace f,g` | Each call of `f` and `g` (a method as `T::m`) with its arguments and result, on stderr |
| `fid --version` | The version and `build: exe:<hash>` of this executable (with the release stamp's commit when there is one) |
| `bench/run.sh`, `bench/check.sh` | `check.sh` fails on a slowdown over `LIMIT_PCT` against its baseline file (`BENCH_BASE`, default `bench/baseline.txt`); `.github/workflows/bench.yml` runs `bench/check.sh --ab` |
| `scripts/sanitize.sh` | The tests through C under ASan/UBSan (clang with ASan, Linux/macOS); `FIDES_CFLAGS` passes extra C flags; nightly in `.github/workflows/verify-nightly.yml` |
| `scripts/fuzz/nightly.sh` | Nightly in `.github/workflows/verify-nightly.yml` (Nov 21) |

---

## Incremental Builds

`ccompile`, `build` and `emit` (Rust and self-hosted compilers) keep a stamp
next to the output, `<output>.fides-stamp`, listing every file the build
read (the program and each imported module), each import location that was
empty, the compiler's identity, the command line and `CC`:

- If nothing changed and the output exists, the build is skipped
  (`... is up to date`).
- If the generated C is identical to the last build's (a comment or
  formatting change), the C compiler is not run again.
- Creating a file that would change what an import resolves to, rebuilding
  the compiler, or changing flags or `CC` invalidates the stamp.
- `--no-cache` or `FIDES_NO_CACHE=1` builds from scratch.

`fid` also keeps a build cache in `.fidcache/` that evicts the least recently
used artifacts past 64 files or 1 GB, and type-checks `stdlib/core.fd` once
per (compiler, core.fd) pair: a stamp in the temp directory skips core's
bodies on later builds (`FIDES_CHECK_CORE=1` forces the check).

`FIDES_TIME_PASSES=1` prints how long each frontend pass took.

`ccompile --cc-opt 0|1|2|3|s` (or `FIDES_CC_OPT`) sets the C compiler's
optimization level: `-O2` by default, `-O0` for a program over 16 MB of C
(gcc and clang take tens of minutes to optimize one that large).

---

## Design Principles

1. **One word, lowercase, no prefix.** No `fides-fmt`, no `fidesc-lint`.
   The tools stand on their own — `ark`, `scribe`, `veil`. This is
   the Go tradition (`go`, `gofmt`, `delve`) not the Rust tradition
   (`rustc`, `rustfmt`, `rust-analyzer`).

2. **Thematic coherence.** Every name relates to trust, safety, or
   formal verification — the core identity of Fides. A developer who
   knows Fides's philosophy can guess the tool names.

3. **No abbreviations.** `ark` not `pct`, `sentinel` not `sntl`.
   Full words are searchable, memorable, and unambiguous.

4. **Verb-friendly.** Each name works as both a noun ("run the sentinel")
   and a verb ("sentinel the codebase"). This makes documentation and
   conversation natural.

5. **Distinct from the compiler.** The compiler is `fid` (the frozen Rust
   compiler is `fidesc`). The tools are separate words — they're separate
   programs with separate concerns. `fid` compiles; `ark` manages packages;
   `scribe` documents; `veil` covers.

---

## Current Status

| Tool     | Status       | Notes |
|----------|--------------|-------|
| ark    | ✅ Implemented | Package manager — path/git deps, ark.toml, ark.lock, ark_modules/, the package index (pledgeandgrow/pacts, Nov 21) |
| forge    | ✅ Implemented (as `fidesc fmt`) | Code formatter — canonical formatting for Fides syntax |
| sentinel | ✅ Implemented (as `fidesc lint`) | Linter — unused functions, unused imports |
| oracle   | ✅ Implemented (as `fidesc lsp`) | LSP server — JSON-RPC, diagnostics, hover, goto-def, completion, references |
| probe    | ✅ Implemented (as `fidesc debug`) | Debugger — breakpoints, step, inspect, watch, trace, hot-reload |
| autopsy  | ✅ Implemented (as `fidesc autopsy`) | Native debugger — ptrace-based, register inspection |
| pulse    | ⚠️ Partial (as `fidesc bench`) | Benchmark tool exists. Full profiler (CPU, memory, flame graphs) pending |
| scribe   | ✅ Implemented | Doc generator — `///` comments to HTML/Markdown, files and directories |
| veil     | ✅ Implemented | Coverage tool — function/line/instruction coverage, text and HTML reports |
| trial    | ⚠️ Partial (as `fidesc test`) | `fidesc test` runs `.fd` test files across 3 backends. Standalone runner pending |
| audit    | ✅ Implemented (as `fidesc audit`) | IR verifier — checks block terminators, type consistency. `fid audit` lists reachable unsafe code |
| qualify  | ✅ Implemented (as `fid qualify`) | Safety evidence as JSON; always `NOT_QUALIFIED` |

> `fid` has `run`, `ccompile`, `emit`, `build`, `test`, `fmt`, `lint`,
> `check`, `bench`, `profile`, `lsp`, `doc`/`scribe`, `veil`, `ark`, `repl`,
> `audit`, `qualify`, `parse-header` and the `dump-*` commands
> (`compiler/main.fd`); its `watch` runs `bench`.
> The Rust compiler (`fidesc`) has `run`, `build`, `emit`, `ccompile`,
> `test`, `repl`, `ir`, `pir`, `ast`, `tokens`, `codegen`, `gpu`, `watch`,
> `replay`, `sourcemap` commands. The standalone tools (`ark`, `scribe`,
> `veil`) are separate binaries built from the same codebase. Others
> (`forge`, `sentinel`, `oracle`, `probe`, `pulse`, `trial`) are currently
> `fidesc` subcommands and will eventually become standalone programs.
