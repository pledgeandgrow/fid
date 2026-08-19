# Fides Development Tools

> Every serious language needs a toolchain. These are the names for Fides's
> standard development tools — each chosen to reflect the language's identity:
> **trust, safety, verification, and formal correctness.**

## Tool Mapping

| Tool             | Rust             | Go            | Python     | Fides       |
|------------------|------------------|---------------|------------|-------------|
| Package manager  | Cargo            | go mod        | pip        | **pledge**  |
| Formatter        | rustfmt          | gofmt         | black      | **forge**   |
| Linter           | clippy           | staticcheck   | pylint     | **sentinel**|
| LSP / IDE        | rust-analyzer    | gopls         | Pylance    | **oracle**  |
| Debugger         | gdb / lldb       | delve         | pdb        | **probe**   |
| Profiler         | perf             | pprof         | cProfile   | **pulse**   |
| Doc generator    | rustdoc          | godoc         | Sphinx     | **codex**   |
| Test framework   | cargo test       | go test       | pytest     | **trial**   |

---

## Name Rationale

### pledge — Package Manager

A **pledge** is a solemn promise or commitment. When you add a dependency,
you're making a pledge to trust that code. The name also reflects the
contractual nature of Fides — every import is a pledge of trust, tracked
and auditable. Package manifests are `pledge.toml` (or `pledge.fd`).

```
pledge add core
pledge build
pledge publish
```

### forge — Formatter

A **forge** shapes raw metal into a precise form. Code formatting is the
same act — taking raw, unshaped source and forging it into a canonical
shape. The blacksmith metaphor fits Fides's craftsmanship ethos: code is
crafted, not just written.

```
forge format src/
forge check src/      # check without modifying (CI mode)
```

### sentinel — Linter

A **sentinel** is a guard that stands watch. The linter is the sentinel
of your codebase — it watches for patterns that won't fail compilation
but indicate bugs, style violations, or unsafe constructs. Fides's safety
annotations (`@audit`, `@medical`, `@crashfree`) are sentinels at the
type level; `sentinel` is the sentinel at the lint level.

```
sentinel check src/
sentinel fix src/     # auto-fix where possible
sentinel --strict     # treat warnings as errors
```

### oracle — LSP / IDE Server

An **oracle** answers questions. The LSP server is the oracle for your
IDE — it answers "what type is this?", "where is this defined?", "what
completes this identifier?". The name is already used informally in the
Fides codebase (the interpreter is called "the reference oracle"). Now
it's the official name for the language server.

**Status: ✅ Implemented** — The LSP server is available as `fidesc lsp`
(see `src/lsp.rs`). It implements JSON-RPC 2.0 over stdio with no external
dependencies (built-in JSON parser/serializer). Currently supports:
`initialize`, `initialized`, `shutdown`, `exit`, `textDocument/didOpen`,
`didChange`, `didClose`, and `publishDiagnostics`. Future work: hover,
goto-def, completion, refactor (will be `oracle` standalone binary).

```
fidesc lsp             # LSP over stdio (current implementation)
oracle --stdio         # LSP over stdio (future standalone binary)
oracle --socket :9090  # LSP over TCP (future)
```

### probe — Debugger

A **probe** is an instrument for investigating something. Debugging is
probing — you insert probes into running code to observe state, trace
execution, and find the root cause of bugs. The name suggests precision
and surgical investigation, not brute force.

```
probe run tests/test_borrowck.fd
probe break main:42
probe step
probe inspect v
probe watch &mut v
```

### pulse — Profiler

A **pulse** is the rhythmic heartbeat of a living thing. The profiler
measures the pulse of your code — CPU cycles, memory allocations, call
frequency, hot paths. A healthy codebase has a steady pulse; a sick one
has irregular beats (spikes, leaks, stalls).

```
pulse run tests/test_async.fd
pulse --cpu          # CPU profiling
pulse --mem          # memory profiling
pulse --flame        # flame graph output
```

### codex — Documentation Generator

A **codex** is an ancient bound book of knowledge — the precursor to the
modern book. Documentation is the codex of your codebase: the authoritative
reference for how things work. The name reflects Fides's formal, scholarly
character — this is a language designed for systems that need to be
understood, audited, and certified.

```
codex build docs/          # generate HTML docs
codex check src/           # verify doc comments exist on public APIs
codex --private            # include private items
```

### trial — Test Framework

A **trial** is a formal examination of evidence. Testing is putting your
code on trial — you present evidence (test cases), the framework examines
them, and the verdict is pass or fail. The legal metaphor fits Fides
perfectly: contracts (`given`/`ensure`), audits (`@audit`), and now trials.
Every test run is a trial where your code must prove itself.

```
trial run                 # run all tests
trial run --borrowck=on   # run with borrow checker enabled
trial watch               # re-run on file change
trial bench               # run benchmarks
trial --fail-only         # show only expected-fail tests
```

---

## Design Principles

1. **One word, lowercase, no prefix.** No `fides-fmt`, no `fidesc-lint`.
   The tools stand on their own — `forge`, `sentinel`, `probe`. This is
   the Go tradition (`go`, `gofmt`, `delve`) not the Rust tradition
   (`rustc`, `rustfmt`, `rust-analyzer`).

2. **Thematic coherence.** Every name relates to trust, safety, or
   formal verification — the core identity of Fides. A developer who
   knows Fides's philosophy can guess the tool names.

3. **No abbreviations.** `pledge` not `pld`, `sentinel` not `sntl`.
   Full words are searchable, memorable, and unambiguous.

4. **Verb-friendly.** Each name works as both a noun ("run the sentinel")
   and a verb ("sentinel the codebase"). This makes documentation and
   conversation natural.

5. **Distinct from the compiler.** The compiler is `fidesc`. The tools
   are separate words — they're separate programs with separate concerns.
   `fidesc` compiles; `forge` formats; `sentinel` lints; `trial` tests.

---

## Current Status

| Tool     | Status       | Notes |
|----------|--------------|-------|
| pledge   | 🔲 Planned   | Package manager — dependency resolution, versioning, registry |
| forge    | 🔲 Planned   | Code formatter — canonical formatting rules for Fides syntax |
| sentinel | 🔲 Planned   | Linter — style, complexity, unsafe patterns, annotation misuse |
| oracle   | ✅ Implemented (as `fidesc lsp`) | LSP server — JSON-RPC over stdio, diagnostics (initialize, didOpen, didChange, publishDiagnostics). Hover, goto-def, completion, refactor pending. |
| probe    | 🔲 Planned   | Debugger — breakpoints, step, inspect, watch, trace |
| pulse    | 🔲 Planned   | Profiler — CPU, memory, flame graphs, allocation tracking |
| codex    | 🔲 Planned   | Doc generator — from `//` and `///` comments to HTML/Markdown |
| trial    | ⚠️ Partial   | `fidesc test` exists (runs `.fd` test files). `trial` will be the standalone test runner with richer features (watch, bench, coverage, parallel) |

> The compiler (`fidesc`) already has `test`, `run`, `compile`, `ccompile`,
> `ir`, `pir`, `codegen`, `wasm`, `verify` commands. The tools above will
> eventually be standalone programs, but many can start as `fidesc`
> subcommands during early development.
