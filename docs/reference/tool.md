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

## `fid` commands (Nov 9, 2026)

The self-hosted `fid` has every tool built in; there are no separate
binaries. Dispatch is in compiler/main.fd (docs/file-analysis/main.md).

| Command | What it does | State (Nov 9) |
|---|---|---|
| `run <file>` | IR interpreter. `--leaks` / `--checked-alloc` rebuild through C (`ccompile --debug`, needs `CC`) and run with the debug allocator | works |
| `ccompile` (`compile`) `<file> -o <out>` | C, then a C compiler. `--target`, `--cc-opt`, `--debug`, `--release`, `--inc`, `--no-cache`, `--profile`, `--leaks`, `--checked-alloc` | works |
| `emit` / `build <file> -o <out>` | native: Windows PE (pe_ir) by default on Windows; `--target aarch64-linux\|riscv64-linux\|x86_64-linux` and `--nat` use the machine layer; `--bare`, `--elf`, `--boot`; through C on a POSIX host without `--target`; `--no-opt`, `--debug`, `--checked-alloc` | works except `--boot` and `--native` with the default target (panic) |
| `dump-c`, `split-c <c> <prefix> <n>` | generated C to stdout; cut a C file into a header and n units | works |
| `check <file> [--time]` | front end only: types, borrows, escapes | works |
| `lint <file>` | unused functions and imports, `unwrap()` outside tests | works (does not resolve `compiler::` imports) |
| `fmt <file> [--check]` | formatter, rewrites the file | **corrupts files** (known-bugs 2g.6) |
| `test [<file>] [--fail]` | a file's `test_*`/`bench_*` functions, or the suite / expected-fail list | works on Windows; without a file broken on POSIX |
| `test-rules [--smt <dir>]` | checks the rewrite rules on values; writes SMT obligations | works |
| `qualify <file>` | safety evidence as JSON | works |
| `audit <file> [--stdlib] [--strict]` | unsafe code a program reaches and its `// SAFETY:` comments | works |
| `doc` / `scribe` | documentation generator | works, buggy (CRLF, `::` names) |
| `veil` | coverage | **crashes** (Double free, 2g.2) |
| `lsp` | language server | diagnostics only; symbol requests crash (2g.5) |
| `ark ...` | package manager | works, buggy |
| `parse-header <h>` | C header to Fides bindings | works |
| `profile <file>` | `ccompile --profile`, run, top-20 table | works |
| `bench <file>`, `watch` | run an executable N times (`watch` is the same as `bench`) | works |
| `repl` | compile and run each line with tcc/gcc | not checked Nov 9 |
| `tokens`, `ast` (`dump-ast`, `dump-ast-raw`, `dump-ast-flat`), `ir`, `pir`, `verify`, `sourcemap` | dumps (the four `ast` forms print the same summary) | work; `pir --opt` panics |
| `opt`, `codegen`, `build-x86`, `gpu`, `wasm` | old PIR path and experimental back ends | `opt` panics; `codegen` prints assembly; the rest produce broken output (platforms.md, Tier 3) |
| `notebook` | HTML page of a file's code blocks (evaluates nothing) | works |
| `version` | version string (`0.1.0`) | works |

Global flags: `--borrowck=off|on` (`warn` prints nothing), `--timeout N`
(parsed, not enforced); arguments after `--` go to the program. Environment:
`CC`, `FIDES_CC_OPT`, `FIDES_CFLAGS`, `FIDES_NO_CACHE`, `FID_HOME`,
`FID_VERIFY`, `FIDES_COW_CHECK`, `FIDES_INC_FLAT`, and the pass switches
`FID_IROPT_OFF`/`FID_IROPT_ON`/`FID_NAT_OFF`/`FID_RULES_ON`
(docs/reference/architecture.md, "Self-hosted pipeline"). Not in `fid`
(`fidesc` only): `debug`, `replay`, `autopsy`, `diff-test`, `link`, `sbom`,
`spdx`.

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
  (docs/file-analysis/ark.md).

Supports path dependencies (files and directories), git dependencies (clone
with `--depth 1`, branch/tag support), lock file generation, and a custom
TOML parser (zero external dependencies). Registry support (ark.dev) is
planned for the future.

```
ark init [name]              # create ark.toml
ark add <dep> --path <path>  # add path dependency
ark add <dep> --git <url>    # add git dependency
ark install                  # install all deps to ark_modules/
ark list                     # list dependencies
ark remove <dep>             # remove dependency
ark lock                     # generate ark.lock
ark update [dep]             # update dependencies
ark search <query>           # search registry (future)
ark publish                  # publish to registry (future)
```

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

**Status: ✅ Implemented (as `fidesc lsp`); ⚠️ `fid lsp` diagnostics only** —
`fid lsp` (compiler/lsp.fd, Nov 9) publishes diagnostics (always at line 0)
and has completion, hover, definition and rename, but each of those four
ends with "Double free" (known-bugs 2g.5); it has no references. The LSP
server below is `fidesc lsp` (see `src/lsp.rs`). It implements JSON-RPC 2.0 over stdio
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
| `fid emit <file.fd> --checked-alloc` | Written, not run: every block on pages of its own, ending at a guard page; freed pages inaccessible, never reused |
| `fid test <file.fd>` | Runs the file's functions whose names start with `test_` and `bench_` |
| `bench/run.sh`, `bench/check.sh` | `check.sh` fails on a slowdown over `LIMIT_PCT` against `bench/baseline.txt`; not in CI |
| `scripts/sanitize.sh` | Written, not run: the tests through C under ASan/UBSan (clang with ASan, Linux/macOS); `FIDES_CFLAGS` passes extra C flags |
| `scripts/fuzz/nightly.sh` | Written, not run; no CI workflow schedules it |

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
| ark    | ✅ Implemented | Package manager — path/git deps, ark.toml, ark.lock, ark_modules/ (registry pending) |
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
