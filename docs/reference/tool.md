# Fides Development Tools

> Every serious language needs a toolchain. These are the names for Fides's
> standard development tools — each chosen to reflect the language's identity:
> **trust, safety, verification, and formal correctness.**

## Tool Mapping

| Tool             | Rust             | Go            | Python     | Fides       |
|------------------|------------------|---------------|------------|-------------|
| Package manager  | Cargo            | go mod        | pip        | **pacte**   |
| Formatter        | rustfmt          | gofmt         | black      | **forge**   |
| Linter           | clippy           | staticcheck   | pylint     | **sentinel**|
| LSP / IDE        | rust-analyzer    | gopls         | Pylance    | **oracle**  |
| Debugger         | gdb / lldb       | delve         | pdb        | **probe**   |
| Profiler         | perf             | pprof         | cProfile   | **pulse**   |
| Doc generator    | rustdoc          | godoc         | Sphinx     | **scribe**  |
| Coverage         | tarpaulin        | go test -cover| coverage.py| **veil**    |
| Test framework   | cargo test       | go test       | pytest     | **trial**   |

---

## Name Rationale

### pacte — Package Manager

A **pacte** is French for "pact" — a formal agreement or covenant. When you
add a dependency, you're making a pact to trust that code. The name reflects
the contractual nature of Fides — every import is a pact of trust, tracked
and auditable. Package manifests are `pacte.toml`, lock files are `pacte.lock`,
and modules install to `pacte_modules/`.

**Status: ✅ Implemented** — The package manager is available as the `pacte`
binary (see `src/pacte.rs`). Supports path dependencies (files and directories),
git dependencies (clone with `--depth 1`, branch/tag support), lock file
generation with resolved commit hashes, and a custom TOML parser (zero external
dependencies). Registry support (pacte.dev) is planned for the future.

```
pacte init [name]              # create pacte.toml
pacte add <dep> --path <path>  # add path dependency
pacte add <dep> --git <url>    # add git dependency
pacte install                  # install all deps to pacte_modules/
pacte list                     # list dependencies
pacte remove <dep>             # remove dependency
pacte lock                     # generate pacte.lock
pacte update [dep]             # update dependencies
pacte search <query>           # search registry (future)
pacte publish                  # publish to registry (future)
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

**Status: ✅ Implemented (as `fidesc lsp`)** — The LSP server is available
as `fidesc lsp` (see `src/lsp.rs`). It implements JSON-RPC 2.0 over stdio
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
probe run tests/test_borrowck.fd  # (future standalone)
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

---

## Design Principles

1. **One word, lowercase, no prefix.** No `fides-fmt`, no `fidesc-lint`.
   The tools stand on their own — `pacte`, `scribe`, `veil`. This is
   the Go tradition (`go`, `gofmt`, `delve`) not the Rust tradition
   (`rustc`, `rustfmt`, `rust-analyzer`).

2. **Thematic coherence.** Every name relates to trust, safety, or
   formal verification — the core identity of Fides. A developer who
   knows Fides's philosophy can guess the tool names.

3. **No abbreviations.** `pacte` not `pct`, `sentinel` not `sntl`.
   Full words are searchable, memorable, and unambiguous.

4. **Verb-friendly.** Each name works as both a noun ("run the sentinel")
   and a verb ("sentinel the codebase"). This makes documentation and
   conversation natural.

5. **Distinct from the compiler.** The compiler is `fidesc`. The tools
   are separate words — they're separate programs with separate concerns.
   `fidesc` compiles; `pacte` manages packages; `scribe` documents;
   `veil` covers.

---

## Current Status

| Tool     | Status       | Notes |
|----------|--------------|-------|
| pacte    | ✅ Implemented | Package manager — path/git deps, pacte.toml, pacte.lock, pacte_modules/ (registry pending) |
| forge    | ✅ Implemented (as `fidesc fmt`) | Code formatter — canonical formatting for Fides syntax |
| sentinel | ✅ Implemented (as `fidesc lint`) | Linter — unused functions, unused imports |
| oracle   | ✅ Implemented (as `fidesc lsp`) | LSP server — JSON-RPC, diagnostics, hover, goto-def, completion, references |
| probe    | ✅ Implemented (as `fidesc debug`) | Debugger — breakpoints, step, inspect, watch, trace, hot-reload |
| autopsy  | ✅ Implemented (as `fidesc autopsy`) | Native debugger — ptrace-based, register inspection |
| pulse    | ⚠️ Partial (as `fidesc bench`) | Benchmark tool exists. Full profiler (CPU, memory, flame graphs) pending |
| scribe   | ✅ Implemented | Doc generator — `///` comments to HTML/Markdown, files and directories |
| veil     | ✅ Implemented | Coverage tool — function/line/instruction coverage, text and HTML reports |
| trial    | ⚠️ Partial (as `fidesc test`) | `fidesc test` runs `.fd` test files across 3 backends. Standalone runner pending |
| audit    | ✅ Implemented (as `fidesc audit`) | IR verifier — checks block terminators, type consistency |

> The compiler (`fidesc`) already has `run`, `build`, `emit`, `ccompile`,
> `test`, `repl`, `ir`, `pir`, `ast`, `tokens`, `codegen`, `gpu`, `watch`,
> `replay`, `sourcemap` commands. The standalone tools (`pacte`, `scribe`,
> `veil`) are separate binaries built from the same codebase. Others
> (`forge`, `sentinel`, `oracle`, `probe`, `pulse`, `trial`) are currently
> `fidesc` subcommands and will eventually become standalone programs.
