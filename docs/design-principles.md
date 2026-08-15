# Fides Design Principles

> The non-negotiable rules that guide every design decision.

## 1. Safety by Construction, Not by Convention

**Rule:** If a bug class can be eliminated at compile time, it must be.

- Memory safety → regions (not discipline, not runtime checks)
- Security → capabilities (not ambient authority, not "be careful")
- Concurrency safety → effects (not Send/Sync, not "don't share mutable state")
- Bounds safety → compile-time where possible, runtime otherwise

**Anti-pattern:** Adding a runtime check where a compile-time check is possible.
**Anti-pattern:** Relying on developer discipline to prevent a class of bugs.

## 2. Zero Hidden Costs

**Rule:** If you can't see it in the source, it doesn't happen.

- No hidden allocations (every allocation is explicit)
- No hidden control flow (no exceptions, no implicit returns)
- No hidden casts (no implicit type coercions)
- No hidden runtime (no GC, no VM, no reflection)
- No hidden dependencies (no libc, no LLVM, no external linker)

**Anti-pattern:** Implicit garbage collection.
**Anti-pattern:** Operator overloading that allocates.
**Anti-pattern:** Exceptions that unwind the stack invisibly.

## 3. Compile-Time Over Runtime

**Rule:** Push as much work as possible to compile time.

- Type checking → compile time (not runtime)
- Memory safety → compile time (not runtime)
- Security checks → compile time (not runtime)
- Code specialization → compile time (comptime)
- Dead code elimination → compile time (not link time)

**Anti-pattern:** Runtime type information (RTTI).
**Anti-pattern:** Reflection at runtime.
**Anti-pattern:** Dynamic dispatch when static is possible.

## 4. Explicit Over Implicit

**Rule:** Make important things visible.

- Allocations are visible (allocator-as-parameter)
- Effects are visible (effect annotations)
- Capabilities are visible (capability parameters)
- Regions are visible (region annotations, inferred where possible)
- Error handling is visible (Result + ? operator, no exceptions)

**Anti-pattern:** Hidden global allocator.
**Anti-pattern:** Functions that silently perform IO.
**Anti-pattern:** Implicit error propagation (exceptions).

## 5. One Way to Do Things

**Rule:** Don't provide multiple mechanisms for the same thing.

- One metaprogramming system (comptime, not macros + templates + generics)
- One error handling system (Result + ?, not exceptions + Option + Result)
- One concurrency model (effect handlers, not async/await + threads + channels)
- One memory model (regions, not borrow checking + reference counting + GC)

**Anti-pattern:** Rust's two macro systems.
**Anti-pattern:** C++'s templates + constexpr + preprocessor.
**Anti-pattern:** Python's list comprehensions + map/filter + for loops.

## 6. Composition Over Inheritance

**Rule:** Build complex things from simple, independent pieces.

- Traits (cap) compose, they don't inherit
- Effect handlers compose, they don't nest
- Regions compose, they don't overlap
- Capabilities compose, they don't escalate

**Anti-pattern:** Class hierarchies.
**Anti-pattern:** Effect systems that can't be combined.
**Anti-pattern:** Global state that breaks composition.

## 7. Fast by Default

**Rule:** The default behavior should be the fast behavior.

- Debug builds should compile in <1s for small projects
- Dev mode should support hot reload
- Release builds should produce optimal code
- Binary size should be minimal (no runtime bloat)

**Anti-pattern:** Rust's 30-minute debug builds.
**Anti-pattern:** Including a runtime in every binary.
**Anti-pattern:** Optimizations that require explicit flags.

## 8. Custom Infrastructure

**Rule:** If we depend on it, we build it.

- Custom lexer, parser, typechecker
- Custom IR (PIR)
- Custom optimizer
- Custom codegen
- Custom linker
- Custom stdlib
- Custom allocator

**Exception:** The Rust compiler (for now) — used to build Fides, but the end goal is self-hosting.

**Anti-pattern:** Using LLVM because "it's easier."
**Anti-pattern:** Using libc because "everyone has it."
**Anti-pattern:** Using an external linker because "that's how it's done."

## 9. AI-Native

**Rule:** The language should be optimized for AI-assisted development and AI execution.

- Semantic IR enables AI-assisted refactoring (the IR carries meaning, not just syntax)
- comptime enables AI code generation (AI writes comptime code, compiler verifies it)
- Effect system enables AI reasoning about side effects (AI knows what's pure)
- Type system enables AI code completion (types guide the AI)
- Hot reload enables AI-assisted iteration (AI changes code, sees result immediately)

**Anti-pattern:** Languages where AI has to guess semantics from syntax.
**Anti-pattern:** Languages where side effects are invisible.
**Anti-pattern:** Languages where types are optional or dynamic.

## 10. Accessibility

**Rule:** The language should be learnable by any programmer in <1 week.

- Syntax is familiar (Rust-like with Fides keywords)
- Error messages are clear and actionable
- No lifetime annotations (regions are inferred)
- No macro complexity (comptime is just regular code)
- No async/await complexity (effect handlers are transparent)
- Hot reload provides instant feedback (learn by doing)

**Anti-pattern:** Rust's 6-month learning curve.
**Anti-pattern:** C++'s infinite complexity.
**Anti-pattern:** Haskell's category theory requirement.

## 11. Zero-Cost Domain Features

**Rule:** Domain-specific features (banking, medical, automotive, aerospace,
etc.) must have zero runtime and zero compile-time cost when not used.

- Industry attributes (`@autosar`, `@gas`, `@audit`, `@medical`, ...) add no
  typechecker work unless attached to a declaration
- Domain stdlib modules (`stdlib/banking.fd`, `stdlib/medical.fd`, ...) are
  never auto-included — only pulled in by explicit `use`
- A kernel developer pays nothing for the AI/ML features; a web developer pays
  nothing for `@autosar`; a game developer pays nothing for `txn {}`
- No domain feature may pull in heavy runtime support (bignum, blockchain
  client, logging infrastructure) unless the user explicitly imports it

**Anti-pattern:** A "money type" that links a bignum library into every binary.
**Anti-pattern:** An `@audit` attribute that injects logging calls globally.
**Anti-pattern:** Auto-including `stdlib/banking.fd` so `txn {}` always parses.

## 12. Import-Scoped Attributes

**Rule:** Domain-specific syntax and attributes are only valid after the
relevant module is imported.

- `use automotive` enables `@autosar`, `@can-bus` — without it, they are parse
  errors
- `use blockchain` enables `@gas`, `@emit` — without it, they don't exist
- `use medical` enables `@medical`, `@audit` — without it, they're unknown
- This keeps the language *feeling* small in any given domain. A web developer
  never sees `@dal-a` in their editor, in docs, or in error messages
- Core attributes (`@memo`, `@differentiable`, `@no_grad`) are always available
  because they're part of the core language, not a domain

**Anti-pattern:** Globally parsing all attributes so every developer must learn
all domain syntax.
**Anti-pattern:** A 30-attribute language where nobody can master the whole
surface (the Scala/Ada/C++ trap).

## 13. Tiered Feature Support

**Rule:** Every feature is explicitly tiered by backend coverage, and the tier
is documented.

| Tier | Meaning | Requirement |
|------|---------|-------------|
| **Core** | Always works in every backend | Must pass in interpreter + C codegen + ELF64/PE before merging |
| **Platform** | Works in 2+ backends | Must work in at least the primary backend + one other |
| **Domain** | Interpreter-only experiments OK | Must be labeled as such in docs and in the harness skip list |

- The **primary backend** is C codegen (most portable, easiest to verify). New
  features must work there first.
- WASM, bare-metal, ARM64, RISC-V, SPIR-V can be subsets — they implement the
  Core tier and whatever Platform features are relevant to their target.
- A feature that only works in the interpreter is a **prototype**, not a
  feature. It must be labeled as such. The harness skip list is the source of
  truth.
- No feature is "done" until it works in the primary backend. Interpreter-only
  features are explicitly marked `// Domain tier — interpreter only` in the
  roadmap.

**Anti-pattern:** Claiming a feature "works" when it only works in the
interpreter.
**Anti-pattern:** Letting the harness skip list grow silently without
documenting why each test is skipped.

## 14. Compile-Time Budget

**Rule:** The compiler must stay fast as features are added. Set a ceiling and
enforce it.

- Target: 10,000 lines of Fides source compiles in <2 seconds (release mode,
  no optimization)
- Target: 1,000 lines compiles in <200ms (for hot-reload responsiveness)
- Measure `cargo build --release` time on a representative benchmark before
  every phase that adds significant typechecker work (Phase 9 adds 9 industry
  domains — measure first)
- If compile time exceeds the budget, the offending phase must optimize before
  adding more features
- Every new attribute, effect, or type-level feature must be profiled for
  typechecker impact before merge

**Anti-pattern:** Rust's compile times growing invisibly until incremental
builds take minutes.
**Anti-pattern:** Adding 9 industry domains without measuring the cumulative
typechecker cost.

## 15. Stdlib Discipline

**Rule:** `stdlib/core.fd` stays small. Domain logic lives in separate,
opt-in modules.

- `stdlib/core.fd` contains only: Option, Result, Vec, HashMap, String, basic
  traits, print/println, assert. Nothing domain-specific.
- Industry features go in `stdlib/banking.fd`, `stdlib/medical.fd`,
  `stdlib/automotive.fd`, `stdlib/aerospace.fd`, etc.
- Domain stdlib files are **never** auto-included. Users must `use` them
  explicitly.
- No domain stdlib file may be imported by `stdlib/core.fd` (transitive
  inclusion defeats the purpose)
- Audit stdlib imports whenever a new domain feature lands: if a `use core`
  pulls in domain code, that's a bug

**Anti-pattern:** `stdlib/core.fd` growing to contain money types, CAN bus
helpers, and audit logging.
**Anti-pattern:** Transitive imports that make "opt-in" a lie.

## 16. Honest Positioning

**Rule:** Describe Fides accurately. Don't claim it does everything.

- Fides is a **multi-domain systems language with opt-in domain-specific
  features** — not "a language that does everything"
- Each domain (banking, medical, automotive, ...) is a separate, optional
  module with its own documentation. Users learn the core + their domain, not
  the whole language.
- The README leads with the core language. Domain features are listed in a
  separate section, clearly marked as opt-in.
- Don't market Fides as a replacement for COBOL/Java/Python simultaneously.
  Market it as a systems language that *can* target those domains when the
  relevant module is imported.

**Anti-pattern:** "Fides does OS, mobile, web, banking, games, AI, and
blockchain!"
**Anti-pattern:** A README that lists 30 attributes and scares off new users.
**Anti-pattern:** Positioning that makes Fides sound like a jack of all trades,
master of none.
