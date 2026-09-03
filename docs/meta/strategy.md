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

## 5. One Way to Do Things (with a safe escape hatch)

**Rule:** Don't provide multiple mechanisms for the same thing — but when
the primary mechanism can't cover a real pattern, provide a *safe* fallback,
not an *unsafe* one.

- One metaprogramming system (comptime, not macros + templates + generics)
- One error handling system (Result + ?, not exceptions + Option + Result)
- One concurrency model (effect handlers, not async/await + threads + channels)
- **One default memory model (regions)**, with RC as the safe escape hatch for
  shared ownership. The borrow checker is opt-in (`--borrowck=on`), not a
  co-equal memory model.

**Memory model hierarchy (Path B — decided Sep 2026):**

| Model | Role | When to use | Cost |
|-------|------|-------------|------|
| **Regions** | Default | Scoped data, bulk allocation, predictable lifetime | Zero runtime cost |
| **References** (`&`/`&mut`) | Always available | Passing borrowed data, no ownership transfer | Zero runtime cost (unchecked by default) |
| **Borrow checker** | Opt-in (`--borrowck=on`) | When you want compile-time aliasing safety | Zero runtime cost, opt-in complexity |
| **RC** (`Rc<T>`/`Arc<T>`) | Safe escape hatch | Shared ownership, graphs, caches, callbacks, dynamic lifetime | Runtime refcount cost |
| **`unsafe`** | Framework-only | Raw pointers, inline asm — never in user code | N/A |

**The key insight:** RC replaces `unsafe` as the fallback for patterns that
regions can't express. A language with regions + RC never needs `unsafe` for
memory safety. Without RC, every graph, cache, or observer pattern forces the
user into `unsafe` — which violates the vision.

**Anti-pattern:** Rust's two macro systems.
**Anti-pattern:** C++'s templates + constexpr + preprocessor.
**Anti-pattern:** Python's list comprehensions + map/filter + for loops.
**Anti-pattern:** Using RC where regions would suffice (lint: warn).
**Anti-pattern:** Using `unsafe` in user code where RC would work.

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

---

# Fides Language — Direction Analysis

> Are we going in the right direction?
> An honest assessment based on `docs/lessons.md` lessons, `docs/roadmap.md`,
> `docs/features-roadmap.md`, `docs/roadmap-limitations.md`, `features.md`,
> and actual test baselines.

---

## Executive Summary

**Fides is doing many things right. It also has serious strategic risks that could
make it another D, Crystal, or V — technically good, commercially irrelevant.**

The project has built an impressive amount of working compiler infrastructure
(86,742 LOC, 51 source files, 164/165 C codegen parity (164/165 interp, 165/165 PE) across 3 backends, 69 stdlib
modules, 6 backends, 4 binaries: fidesc/pacte/scribe/veil). But it's spreading into too many domains (games,
blockchain, robotics, medical, serverless) before it has proven it can do the
basics better than existing languages.

**The single most important question Fides has not answered:**
> *Why would someone choose Fides over Rust or Zig for a real project today?*

---

## What IS Going Right

### 1. Genuine Novel Combinations ✅

Fides combines things no other language has together:

| Feature | Exists in... | But combined with... |
|---------|-------------|---------------------|
| Region-based memory safety | Rust (ownership), Cyclone (regions) | Effects + capabilities + comptime |
| Compile-time capability revocation | None (Fides-unique) | Sandbox blocks |
| Effect system | Koka, Eff, Helium | Regions + native codegen |
| Comptime reflection | Zig | Effects + capabilities |
| Memory budgets | None (Fides-unique) | Region system |
| Native codegen (zero tools) | Go (partially), Zig | PE + ELF64 + Mach-O + COFF + WASM + SPIR-V |

This is genuinely new territory. History lesson #1 says: **"Solve a specific,
painful problem"** — Fides's combination of safety + native + no-GC + comptime +
capabilities is a real design-space point no one else occupies.

### 2. Multi-Backend Architecture ✅

```
Fides source
   ↓
  IR (SSA)
   ↓
  ├── Interpreter (reference oracle)
  ├── PE emitter (Windows native)
  ├── ELF64 emitter (Linux native, zero tools)
  ├── C codegen (portable, gcc/clang)
  ├── WASM backend (web)
  ├── SPIR-V backend (GPU)
  ├── ARM64 / RISC-V codegen
  └── Hot reload (native JIT)
```

164/165 C codegen parity (164/165 interp, 165/165 PE) is real. This is more
backends than most languages ever achieve. History lesson: **"A great runtime
can sustain multiple languages"** — Fides's IR is its "runtime" and it supports
many targets.

### 3. Syntax-vs-Stdlib Discipline ✅

The `features-roadmap.md` classification is excellent:

```
SYNTAX  = things the COMPILER must understand (can't be a library)
STDLIB  = things USERS can build themselves (just .fd files)
CUT     = things EXISTING features already cover (redundant)
```

15 features were CUT as redundant. This is the right instinct. History lesson
#7: **"Complexity is a one-way door."** Every feature cut now is a feature that
doesn't burden the language forever. Scala should have done this. C++ couldn't.

### 4. Test Infrastructure ✅

- 265/265 unit tests (cargo test --lib)
- harness tests 8/8 passed
- 164/165 interpreter + C codegen, 165/165 PE
- `verify.sh` with baseline tracking
- Golden file generation
- Bisect tooling

This is more test discipline than most research languages ever achieve. The
C codegen gap closure (test_asm, test_comptime_reflect) shows the test
infrastructure works and catches real issues.

### 5. Self-Hosting Plan ✅

Phase 10 plans to rewrite the compiler in Fides itself. This is the right path —
it's how C, Go, Rust, Zig, and many others proved their language was real.
History lesson: **"A language is proven by what it can express, including
itself."**

### 6. Comptime as Central Design ✅

Zig proved comptime can replace macros, complex generics, and much of what
other languages solve with type system gymnastics. Fides has comptime and uses it
for reflection, code generation, and f-string interpolation. This is the right
bet — it's one of Zig's strongest ideas.

---

## What IS NOT Going Right

### 1. Scope Expansion — Real Synergy, Execution Risk �

> **Note:** This section was revised after recognizing that Fides's multi-domain
> scope is not "trying to do everything" — it's **one safety system with many
> lenses**. The cross-domain synergy is architecturally real.

Fides's domain annotations (Phases 8–9) are not separate features. They're
different applications of the same three core systems:

```
Capability system (Phase 2)
  ├── @deterministic (medical) — reject IO/Random/Time
  ├── @gas (blockchain) — count ops, verify limit
  ├── sandbox { } (security) — revoke all capabilities
  └── @serverless (cloud) — tree-shake ungranted functions

Effect system (Phase 2)
  ├── @realtime (robotics) — reject !Alloc, !Spawn
  ├── @crashfree (gaming) — prove no !Diverge
  ├── @deterministic (medical) — reject !IO, !Read
  └── @no_grad (AI) — skip tracing, mark !{} pure

Region system (Phase 2)
  ├── @pool (gaming) — zero-allocation pools
  ├── @firmware (IoT) — reject heap, stack-only
  ├── @realtime (robotics) — no dynamic allocation
  └── Memory budgets (safety) — compile-time limit

Comptime (Phase 3)
  ├── @shader (gaming) — compile-time GPU codegen
  ├── @deterministic (medical) — compile-time verification
  ├── @simd (performance) — compile-time vectorization
  └── Reflection (power) — type introspection
```

This is architecturally different from a language that just "adds features."
Each new annotation is cheaper because it reuses existing infrastructure.
This is more like **Erlang's BEAM VM** — built for telecom, but its core
abstractions (actors, supervision, fault tolerance) turned out to be broadly
applicable to messaging, databases, and real-time web. Fides's core abstractions
(capabilities, effects, regions) may be similarly broadly applicable.

**The real risk is not the scope — it's the execution:**

1. **Depth before breadth:** `@deterministic` needs to *provably* reject
   FP/random/IO/time, not just check for keywords. If the first annotation is
   shallow, users won't trust the rest. The first one must be airtight.

2. **Proof before promises:** 18 annotations as "PLANNED" with 0 implemented
   looks like vaporware. Implementing 2-3 well is more convincing than
   planning 18. The cross-domain synergy should be *demonstrated*, not just
   argued architecturally.

3. **Performance blocks everything:** `@realtime` is meaningless if the
   binary is slow. `@gas` is meaningless if you can't measure real op counts.
   The regalloc bug must be fixed before domain annotations are credible.

**Verdict:** 🟡 The scope is architecturally justified — the domains share
infrastructure. The risk is execution: going broad before going deep, and
promising before proving. Fix the foundation, implement 2-3 annotations
airtight, then the cross-domain story sells itself.

### 2. No Killer App, No Users, No Ecosystem 🔴

History lesson #3: **"Ecosystem > Language."** History lesson #9: **"Without
a platform, you need a killer framework."**

Current state:
- 0 users (not released)
- 0 packages (pacte package manager implemented, registry pending)
- 0 real projects written in Fides
- No LSP, no formatter, no debugger
- No documentation for users (only internal docs)
- No community (no Discord, no forum, no GitHub stars)

The roadmap puts ecosystem at Phase 11 — after games, blockchain, robotics,
and self-hosting. **This is backwards.** Go got users because it had a working
toolchain and could deploy to Google's infrastructure on day one. Rust got
users because it solved a problem (memory safety) that people were desperate
to solve. Fides has neither a deploy target nor a desperate problem that users
can articulate.

**The killer app question:** What can you build in Fides today that you can't
build in Rust or Zig? The answer is "nothing, yet" — because Fides hasn't proven
any concrete advantage in a real project.

### 3. The Register Allocator Bug — Technical Debt Blocking Progress 🔴

The regalloc bug has been known for a long time and blocks the optimizer from
being enabled by default. This means:
- `--opt` can't be used
- PIR optimizer (inliner, CSE, DCE, unroll, TCO) is wasted
- Binary size and performance can't compete with Rust/Zig/Clang

History lesson: Rust's success was partly because it produced *fast* binaries.
If Fides can't optimize, it can't compete on performance — which is the #1 reason
to choose a systems language.

**This should be the #1 priority, not Phase 7 ergonomics.**

### 4. Documentation Inconsistency 🟡

~~Multiple docs are out of sync with reality:~~
**UPDATE Aug 2026:** Docs have been synchronized with actual state. All test
counts, LOC, file counts, and phase numbers now match verified reality:
- `features.md` now says "164/165 C codegen" ✅
- `features.md` now says "164/165 interpreter" ✅
- `roadmap-limitations.md` now says "164/165 normal" ✅
- `roadmap.md` summary now says "164/165" ✅
- Phase numbers updated (self-hosting is Phase 10, ecosystem Phase 11, world class Phase 12) ✅

This was a symptom of moving fast without updating all docs. The docs now
match reality. A pre-commit check would prevent future drift.

### 5. 136 Compiler Warnings 🟡

136 warnings in the Rust compiler — mostly dead code in `pe.rs`, `opt.rs`,
`debugger.rs`, `wasm.rs`, `fmt.rs` (future feature stubs). Down from 225
after hygiene cleanup (unused imports, unused variables, dead constants
removed). The remaining warnings are infrastructure for future phases
(PE emitter rewrite, WASM backend, debugger) and should not be removed.

### 6. No Generic Option<T> Yet 🟡

The `?.` optional chaining implementation note says: "Full field access on
wrapped structs requires generic Option<T> (Phase 10)." This means Fides's Option
type currently only wraps i64 — a significant limitation. Rust's Option<T> is
one of its most important features. Fides can't do real ergonomic null-safety
without it.

This is a foundational gap, not an ergonomic feature. It should be higher
priority than game development or blockchain annotations.

### 7. Too Many Annotations, Not Enough Substance 🟡

Phases 8–9 plan 18 annotation features (@shader, @simd, @crashfree, @pool,
@deterministic, @realtime, @gas, @interrupt, @firmware, @lowpower,
@serverless, @audit, @emit, @watchdog, @handler, @ota, @differentiable,
@no_grad).

Each of these requires:
- Parser changes
- Typechecker analysis
- IR/codegen support
- Tests
- Documentation

That's 18 × 5 = 90 units of work for features that serve domains Fides hasn't
proven it can serve yet. History lesson: **Carbon is "wait and see" because
it has to prove its C++ migration strategy.** Fides's @gas, @realtime, @firmware
annotations are the same — they sound impressive but need to prove they
actually work better than existing solutions.

### 8. "World Domination" Framing 🟡

The roadmap title is "Milestones to World Domination." Phase 12 plans to write
an OS kernel, web browser engine, database, AI framework, and robot control
system in Fides, plus get 3 companies using it and 1000+ developers.

This is aspirational, but history lesson #6: **"Technical merit ≠ adoption."**
D was technically excellent. Crystal is well-designed. Vale has novel ideas.
None achieved "world domination." The framing risks creating a culture where
ambition replaces focus.

---

## What We Should Keep in Mind

### 1. Pick ONE Domain and Win It

From history.md, the most successful languages each had ONE domain where they
were clearly the best before expanding:

| Language | Initial domain | Expanded to |
|----------|---------------|-------------|
| Rust | Systems (Firefox) | WebAssembly, embedded, CLI tools |
| Go | Server software (Google) | CLI tools, cloud infrastructure |
| Python | Scripting/education | Data science, AI, web |
| Erlang | Telecom | Messaging, distributed systems |
| Julia | Scientific computing | (Still expanding) |

**Recommendation:** Fides should pick **systems programming with safety + comptime**
as its initial domain. This is where Fides's unique combination (regions +
effects + capabilities + comptime + native codegen) is strongest. Prove it
there. Write a real project (a web server, a database client, a CLI tool
framework) in Fides. Then expand.

### 2. Fix the Register Allocator Before Adding Features

The regalloc bug is the single highest-impact technical issue. It blocks:
- The optimizer
- Performance competitiveness
- Binary size optimization
- Confidence in the codegen pipeline

Every hour spent on F8 `?.` or F11 `is` while the regalloc bug persists is an
hour not spent on the thing that would make Fides competitive with Rust/Zig.

**Recommendation:** Make the regalloc fix the #1 priority. Everything else
is secondary.

### 3. Get Real Users Before Phase 8

History lesson #9: **"Without a platform, you need a killer framework."**

Fides has no platform (no browser, no JVM, no cloud provider). So it needs a
killer framework or killer use case. The best candidate is:

> **Fides is the only language where you can write safe systems code with
> compile-time capability revocation, effect tracking, and comptime
> reflection — all with native codegen and no GC.**

That's a real pitch. But it needs to be proven with a real project, not just
test files.

**Recommendation:** Before starting Phase 8 (games), write one real project
in Fides that demonstrates the unique value proposition. A web server with
sandbox blocks for plugin isolation. A CLI tool with capability-restricted
subcommands. Something that makes people say "I can't do this in Rust."

### 4. Generic Option<T> and Vec<T> Are Foundational, Not Features

Fides currently has Option wrapping only i64, and Vec/HashMap/String are
hardcoded in the compiler (522 lines of special cases across backends).

This is not a "Phase 10" issue. This is a "Phase 1" issue that was deferred.
Without generic Option<T>, Fides can't do real null-safety. Without Vec<T> in Fides
code, Fides can't be self-hosting. These are foundational.

**Recommendation:** Move generic Option<T> and the Vec/HashMap/String
migration to immediately after the regalloc fix. They're prerequisites for
everything else.

### 5. Don't Build 18 Annotations — Build 3 and Prove Them

Phase 9 plans 12 domain annotations. Each is a significant compiler feature.
But none has been proven to work better than existing approaches.

History lesson: Mojo's @differentiable and GPU support are valuable because
they solve a real, painful problem (Python + C++ + CUDA fragmentation). Fides's
@gas, @realtime, @firmware need to prove they solve real problems better
than Solidity's runtime gas, C's manual WCET analysis, and existing bare-metal
toolchains.

**Recommendation:** Pick the 3 annotations with the clearest value
proposition (likely @deterministic, @crashfree, @simd) and implement them
well. Cut the rest until those 3 prove the concept.

### 6. Documentation Must Match Reality

Every doc should be updated when test counts change. ~~The current state where
`features.md` says 76/78 but reality is 114/114 is unacceptable for a project
that wants to be taken seriously.~~ **UPDATE Aug 2026: All docs now match
verified reality (164/165 interpreter+C codegen, 165/165 PE).**

**Recommendation:** Add a pre-commit check: run `fidesc test`, compare
output to docs, fail if mismatch. Or at minimum, update all docs in the same
commit that changes test results.

### 7. The "V Risk" Is Real

From history.md, V is classified as 🟠 because: "much smaller ecosystem and
adoption, and many of its desirable properties exist in better-established
competitors."

Fides faces the same risk. Its features are impressive, but:
- Region-based safety → Rust has ownership
- Comptime → Zig has comptime
- Native codegen → Go, Rust, Zig all have this
- Effects → Koka has effects
- Capabilities → object-capability languages exist

Fides's defense is the *combination*. But combinations are hard to sell. "We have
everything" is less compelling than "we're the best at X."

**Recommendation:** Define Fides's one-sentence pitch. Not a feature list. One
sentence that answers: "Why Fides instead of Rust or Zig?"

Suggested pitch: **"Fides is the only language with compile-time capability
revocation, effect tracking, and region-based memory safety — all with
native codegen and no GC."**

That's unique. That's provably impossible in Rust, Zig, Go, or C. Lead with
that.

### 8. Self-Hosting Should Come Before Ecosystem

The roadmap puts self-hosting at Phase 10 and ecosystem at Phase 11. This is
correct. Go self-hosted early. Rust self-hosted early. Zig is self-hosting
now. Self-hosting proves the language is real.

But self-hosting requires:
- Generic Option<T> ✅ (needed)
- Generic Vec<T> ✅ (needed)
- Raw pointers + unsafe ✅ (needed)
- String type in Fides ✅ (needed)

These are prerequisites. They should be prioritized over ergonomic features
and domain annotations.

### 9. Study D and Crystal as Cautionary Tales

D is technically excellent but lost to C++/Rust/Go because:
- No clear reason to switch from C++
- Ecosystem never reached critical mass
- Caught between multiple competitors

Crystal is well-designed but has a tiny community because:
- "Ruby but compiled" isn't enough differentiation
- Ruby users have Ruby; compiled users have many alternatives

Fides risks the same fate if it:
- Tries to compete in too many domains
- Doesn't achieve a critical mass of users in one domain
- Can't articulate why it's better than Rust/Zig for a specific use case

### 10. Study Erlang and Julia as Success Models

Erlang succeeded because it solved ONE problem (telecom reliability)
extremely well. Julia succeeded because it solved ONE problem (the
two-language problem in science) extremely well.

Fides should identify its ONE problem and solve it extremely well. The best
candidate:

> **The safety-vs-control problem:** You want memory safety without GC, but
> Rust's borrow checker is too complex and Zig doesn't guarantee safety. Fides
> provides region-based safety with effect tracking and capability
> revocation — a different point in the design space.

---

## Scorecard

| Dimension | Score | Notes |
|-----------|-------|-------|
| Novel design space | 🟢 9/10 | Unique combination no one else has |
| Technical execution | 🟢 8/10 | 164/165 parity, 6 backends, solid test infra |
| Scope discipline | 🔴 3/10 | Trying to do everything at once |
| Ecosystem building | 🔴 2/10 | No users, no packages, no tooling |
| Focus / prioritization | 🟡 5/10 | Optimizer enabled by default, but still adding features instead of fixing regalloc |
| Documentation accuracy | 🟡 5/10 | Multiple docs out of sync |
| Self-hosting path | 🟢 7/10 | Plan exists, prerequisites identified |
| Differentiation clarity | 🟡 5/10 | Combination is unique but not articulated |
| Risk of irrelevance | 🟠 Medium | Could become another D/Crystal if scope isn't narrowed |
| Overall trajectory | 🟡 6/10 | Good engineering, uncertain strategy |

---

## Recommended Priority Order

Based on the history.md lessons and current state:

```
1. 🔴 Fix register allocator bug
   → Unblocks optimizer, performance, binary size
   → This is the single highest-impact fix

2. 🔴 Implement generic Option<T>
   → Foundational for null-safety, ergonomics, self-hosting
   → Currently Option only wraps i64 — a serious limitation

3. 🔴 Migrate Vec/HashMap/String to Fides stdlib
   → Eliminates 522 lines of compiler special cases
   → Prerequisite for self-hosting
   → Proves Fides can express its own data structures

4. 🟡 Update all documentation to match reality
   → features.md, roadmap-limitations.md need sync
   → Trust erodes when docs lie

5. 🟡 Write ONE real project in Fides
   → A web server with sandbox blocks for plugin isolation
   → Or a CLI tool framework with capability-restricted subcommands
   → Proves the unique value proposition

6. 🟡 Finish Phase 7 ergonomics (F10, F12–F15)
   → These are small, high-value features
   → Complete the phase before starting new ones

7. 🟢 Begin self-hosting (Phase 10)
   → Start with lexer (simplest component)
   → C codegen bootstrap: gcc/clang provides runtime, zero blockers
   → Proves Fides is real

7b. 🟢 PE emitter rewrite (Phase 10B — after C codegen self-host lands)
   → Fix Issues 67-69 (val-in-loop, str_split, recursive Box<T>)
   → Rewrite pe.rs in Fides, switch default backend to PE
   → Zero-dependency deployment (no gcc/clang needed)
   → C backend stays as fallback (like Zig)

8. 🟢 Start ecosystem (Phase 11 basics)
   → LSP server (oracle/fidesc lsp), formatter (forge/fidesc fmt), package manager (pacte), doc generator (scribe), coverage (veil)
   → These are what make a language usable

9. ⏸ DEFER Phase 8 (Game Development)
   → Pick 2-3 features (vec/mat, @simd) not all 18
   → Only after self-hosting and ecosystem basics

10. ⏸ DEFER Phase 9 (Industry Domains)
    → Pick 1-2 annotations (@deterministic, @crashfree) not all 12
    → Only after proving Fides in its core domain

11. ⏸ DEFER Phase 12 (World Class)
    → Formal verification, OS kernel, browser — aspirational
    → Focus on being useful first
```

---

## The One Question That Matters

From history.md, the most important lesson is:

> **"A new language is worth creating if it solves a problem no existing
> language solves."**

Fides's answer:

> **Fides is the only language that combines compile-time capability revocation,
> effect tracking, region-based memory safety, and comptime reflection — all
> with native codegen and no GC.**

That's real. That's unique. That's worth building.

But only if Fides **proves it** with a real project, **fixes the performance
gap** (regalloc), and **gets real users** before trying to conquer games,
blockchain, robotics, and the world.

**Build the foundation. Prove the value. Then expand.**

---

*This document should be revisited quarterly to check whether priorities
have shifted and whether the trajectory has improved.*
