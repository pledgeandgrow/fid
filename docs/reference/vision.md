# Fides — Vision & Direction

> Consolidated 2026-10-24 from `vision.md`, `strategy.md`, `decisions.md`, `future.md`.


## Vision

*From `vision.md`.*

> Build the last programming language the world needs.
>
> **Today (Nov 6, 2026):** see [AGENTS.md → Current Baseline](../../AGENTS.md#current-baseline-nov-9-2026-full-suite-last-run-nov-2) for what is
> true now — the self-hosted compiler `fid` has 68 modules, 158,516 lines,
> a byte-identical fixpoint checked on every release build (49 MB of C), and
> native output on Windows x64 (`fid emit`); Linux goes through C, and the
> other targets are experimental. The language `fid` accepts is described in
> [reference.md](reference.md). Much of this document states goals: where it
> says something is a compile-time error, check reference.md for what the
> compiler checks today (there is, for example, no region use-after-free
> check, and copy-on-write value semantics are written but switched off).
> The status below is from September and kept as history.
>
> **Status (Sep 6, 2026 — session 16):** Bootstrap fixpoint achieved. The
> Fides compiler is self-hosted (26 modules, 61K LOC) and reproduces
> byte-identical output across generations (`fides-c2.c == fides-c3.c`,
> SHA256-identical, 701,100 lines). Three generations verified end-to-end.
> Tier 3 OS features added: `@cpu_local`, inline asm clobbers, `--no-std`,
> cross-compilation dispatch, UEFI boot services bindings.

### Mission

Fides is a systems programming language built from first principles — every layer custom-made, from parser to machine code. No LLVM dependency. No borrowed infrastructure. A language designed to outperform C, Rust, and Zig in security, performance, developer velocity, and expressiveness.

### The Eight Pillars

#### 1. Originality — Own Our Uniqueness
- No existing language combines region-based memory safety, effect handlers, capability security, and comptime metaprogramming
- Every layer of the compiler stack is built by us: lexer, parser, typechecker, IR, optimizer, codegen, linker
- Fides is not "Rust with different syntax" or "Zig with safety" — it is a new architecture

#### 2. Security — Prevent CVEs by Construction
- **Capability-based security**: no ambient authority, no implicit filesystem/network access
- **Memory safety**: today buffer overflow and double free are run-time panics, dangling references are compile-time errors (borrow and escape checks), and values the compiler cannot prove finished are leaked, never freed early. Region use-after-free as a compile-time error is a goal
- **`unsafe` is explicit**: raw-pointer operations, `p_malloc`/`p_free` and calls to C need an `unsafe { }` block or an `@unsafe` function in the program's own module; `fid audit` lists every such site a program reaches
- **Effect system**: IO, network, spawn — all explicit, all auditable
- **Supply chain defense**: libraries receive only granted capabilities, can't access what they're not given
- Target: eliminate 90%+ of CWE categories that plague C/C++

#### 3. Performance — 10x Faster (Realistically)
- **Runtime**: 2-3x faster than Rust/Zig in specific cases via semantic optimization (regions enable better alias analysis, effects enable reordering, comptime eliminates runtime branches)
- **Compilation**: 10x faster than Rust via custom IR and codegen (no LLVM overhead)
- **Iteration**: 100x faster dev cycle via hot reload (milliseconds vs minutes)
- **Binary size**: smaller than C via dead code elimination at IR level
- **Memory**: 40% less overhead than Rust via region-based allocation (no Rc/RefCell needed)

#### 4. Memory Safety — Regions First, RC as the Safe Escape Hatch
- **Regions** (goal: the default memory model): `region r { ... }` is today a scope with an optional compile-time size budget; collections declared in it are freed at its end. Compile-time escape detection keeps references from outliving their block.
- **References are borrow-checked**: the borrow checker is on by default (`--borrowck=on`; `warn` and `off` exist). References are second-class: they cannot be stored, returned from locals or sent to tasks.
- **RC is the safe alternative to `unsafe`**: when a pattern needs shared ownership (graphs, caches, callbacks, dynamic lifetime), `Rc<T>`/`Arc<T>` provides it memory-safely. This eliminates the need for `unsafe` raw pointers in user code.
- **Little `unsafe` in user code** (goal): if you can't use regions and you can't use references, you use RC. Today `unsafe { }` is available to programs and checked only for where it is required.
- **Explicit allocators**: caller chooses allocation strategy (like Zig), but with compile-time safety (like Rust)
- **Zero runtime cost for the default path**: regions and references are zero-cost. RC has runtime cost, but only when explicitly chosen.

#### 5. Power & Flexibility — Like C, But Safe
- **Direct hardware access**: raw pointers, inside `unsafe { }` or an `@unsafe` function
- **Inline assembly**: for kernel/embedded development
- **Zero-cost abstractions**: generics, closures, traits — all compile to optimal code
- **comptime**: full metaprogramming without macros — regular code at compile time
- **No hidden control flow**: no exceptions, no hidden allocations, no implicit casts
- **FFI**: interop with C for legacy integration during transition

#### 6. Adoption — Make Switching Easy
- **C interop**: call C libraries directly, gradual migration path
- **Familiar syntax**: Rust-like syntax with Fides keywords (val/fn/const)
- **Hot reload** (goal): instant feedback loop attracts game/GUI/web developers — today `fidesc watch` recompiles and restarts
- **Fast compilation**: sub-second builds for small programs; large ones are slow today (~2,100 lines/s for an -O2 `fid`: 158,516 lines in 74 s, Nov 9)
- **AI tooling**: LLM-friendly syntax, semantic IR enables AI-assisted refactoring
- **Single binary**: no runtime, no VM, no dependencies — just a binary
- **Cross-compilation**: built-in, not an afterthought — Windows done; ARM64 and RISC-V runtimes are stubs ([platforms](platforms.md))

#### 7. Custom — Everything Built By Us
- **Custom lexer**: hand-written, not generated
- **Custom parser**: recursive descent, not parser generators
- **Custom typechecker**: generics with inference and bounds + effect system + capability checking + 17 safety rules
- **Custom IR (PIR)**: Fides Intermediate Representation — typed, semantic, multi-level
- **Custom optimizer**: semantic-aware, leverages region/effect/capability info
- **Custom codegen**: direct to machine code — **DONE** for Windows x64 (`fid emit`); ARM64 and RISC-V experimental
- **Custom linker**: integrated, not a separate process — **DONE** (target-aware: x86-64, ARM64, RISC-V)
- **Custom stdlib**: no libc dependency for core operations
- **Custom allocator**: region-aware, built into the language runtime
- Minimal external dependencies: the self-hosted `fid` builds itself; stage 1 still comes from the frozen Rust compiler (plus 4 crates) until a stage 0 is pinned

#### 8. Innovation — Optimized for AI, Robotics, Hardware, IoT
- **AI/ML**: comptime specialization for tensor operations, SIMD intrinsics as first-class
- **Robotics**: real-time guarantees via effect system (mark functions as !RealTime)
- **Hardware**: inline assembly, register access, interrupt handlers
- **IoT**: bare-metal targets, deterministic memory (FixedBufferAllocator), tiny binaries
- **Effect handlers**: model robot sensors, actuators, and control loops as effects
- **Region-based memory**: predictable allocation for real-time systems
- **Cross-compilation**: target any architecture from any host — Windows x64 native done; ARM64/RISC-V through C or the experimental native backends
- **No runtime**: bare metal, no OS needed

### What Fides Is NOT

- **Not a research language**: designed for production, not papers
- **Not a Rust replacement**: different architecture, different tradeoffs
- **Not a garbage-collected language**: zero runtime overhead
- **Not a managed language**: no VM, no JIT in production mode
- **Not a macro-based language**: comptime replaces all metaprogramming
- **Not an unsafe-by-default language**: `unsafe` must be written out, and `fid audit` lists every use a program reaches; RC is the safe alternative when regions can't express the ownership pattern

### The North Star

```
A language where:
- You can write an OS kernel
- You can write a web server
- You can program a robot
- You can train an AI model
- You can target a microcontroller
- All with the same language
- All memory-safe
- All fast
- All without fighting the compiler
- All with hot reload during development
- All compiled to native code by our own toolchain
```


## Design Principles

*From `strategy.md`.*

> The non-negotiable rules that guide every design decision.

### 1. Safety by Construction, Not by Convention

**Rule:** If a bug class can be eliminated at compile time, it must be.

- Memory safety → regions (not discipline, not runtime checks)
- Security → capabilities (not ambient authority, not "be careful")
- Concurrency safety → effects (not Send/Sync, not "don't share mutable state")
- Bounds safety → compile-time where possible, runtime otherwise

**Anti-pattern:** Adding a runtime check where a compile-time check is possible.
**Anti-pattern:** Relying on developer discipline to prevent a class of bugs.

### 2. Zero Hidden Costs

**Rule:** If you can't see it in the source, it doesn't happen.

- No hidden allocations (every allocation is explicit)
- No hidden control flow (no exceptions, no implicit returns)
- No hidden casts (no implicit type coercions)
- No hidden runtime (no GC, no VM, no reflection)
- No hidden dependencies (no LLVM; no external linker on the native Windows path)

**Anti-pattern:** Implicit garbage collection.
**Anti-pattern:** Operator overloading that allocates.
**Anti-pattern:** Exceptions that unwind the stack invisibly.

### 3. Compile-Time Over Runtime

**Rule:** Push as much work as possible to compile time.

- Type checking → compile time (not runtime)
- Memory safety → compile time (not runtime)
- Security checks → compile time (not runtime)
- Code specialization → compile time (comptime)
- Dead code elimination → compile time (not link time)

**Anti-pattern:** Runtime type information (RTTI).
**Anti-pattern:** Reflection at runtime.
**Anti-pattern:** Dynamic dispatch when static is possible.

### 4. Explicit Over Implicit

**Rule:** Make important things visible.

- Allocations are visible (allocator-as-parameter)
- Effects are visible (effect annotations)
- Capabilities are visible (capability parameters)
- Regions are visible (region annotations, inferred where possible)
- Error handling is visible (Result + ? operator, no exceptions)

**Anti-pattern:** Hidden global allocator.
**Anti-pattern:** Functions that silently perform IO.
**Anti-pattern:** Implicit error propagation (exceptions).

### 5. One Way to Do Things (with a safe escape hatch)

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

### 6. Composition Over Inheritance

**Rule:** Build complex things from simple, independent pieces.

- Traits (cap) compose, they don't inherit
- Effect handlers compose, they don't nest
- Regions compose, they don't overlap
- Capabilities compose, they don't escalate

**Anti-pattern:** Class hierarchies.
**Anti-pattern:** Effect systems that can't be combined.
**Anti-pattern:** Global state that breaks composition.

### 7. Fast by Default

**Rule:** The default behavior should be the fast behavior.

- Debug builds should compile in <1s for small projects
- Dev mode should support hot reload
- Release builds should produce optimal code
- Binary size should be minimal (no runtime bloat)

**Anti-pattern:** Rust's 30-minute debug builds.
**Anti-pattern:** Including a runtime in every binary.
**Anti-pattern:** Optimizations that require explicit flags.

### 8. Custom Infrastructure

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

### 9. AI-Native

**Rule:** The language should be optimized for AI-assisted development and AI execution.

- Semantic IR enables AI-assisted refactoring (the IR carries meaning, not just syntax)
- comptime enables AI code generation (AI writes comptime code, compiler verifies it)
- Effect system enables AI reasoning about side effects (AI knows what's pure)
- Type system enables AI code completion (types guide the AI)
- Hot reload enables AI-assisted iteration (AI changes code, sees result immediately)

**Anti-pattern:** Languages where AI has to guess semantics from syntax.
**Anti-pattern:** Languages where side effects are invisible.
**Anti-pattern:** Languages where types are optional or dynamic.

### 10. Accessibility

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

### 11. Zero-Cost Domain Features

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

### 12. Import-Scoped Attributes

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

### 13. Tiered Feature Support

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

### 14. Compile-Time Budget

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

### 15. Stdlib Discipline

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

### 16. Honest Positioning

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

## Fides Language — Direction Analysis

> Are we going in the right direction?
> An honest assessment based on `docs/lessons.md` lessons, `docs/roadmap.md`,
> `docs/features-roadmap.md`, `docs/roadmap-limitations.md`, `features.md`,
> and actual test baselines.
>
> Written in Aug–Sep 2026; its numbers are from then. Since then (Nov 6): the
> compiler is self-hosted (68 modules, 158,516 lines), the full suite passes
> 339/339 interp, 343/343 C, 345/345 PE (Nov 2), and generic `Option<T>` (§6
> below) is done in `fid`.

---

### Executive Summary

**Fides is doing many things right. It also has serious strategic risks that could
make it another D, Crystal, or V — technically good, commercially irrelevant.**

The project has built an impressive amount of working compiler infrastructure
(86,742 LOC, 51 source files, 164/165 C codegen parity (164/165 interp, 165/165 PE) across 3 backends, 99 stdlib
modules, 6 backends, 4 binaries: fidesc/ark/scribe/veil). But it's spreading into too many domains (games,
blockchain, robotics, medical, serverless) before it has proven it can do the
basics better than existing languages.

**The single most important question Fides has not answered:**
> *Why would someone choose Fides over Rust or Zig for a real project today?*

---

### What IS Going Right

#### 1. Genuine Novel Combinations ✅

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

#### 2. Multi-Backend Architecture ✅

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

#### 3. Syntax-vs-Stdlib Discipline ✅

The `features-roadmap.md` classification is excellent:

```
SYNTAX  = things the COMPILER must understand (can't be a library)
STDLIB  = things USERS can build themselves (just .fd files)
CUT     = things EXISTING features already cover (redundant)
```

15 features were CUT as redundant. This is the right instinct. History lesson
#7: **"Complexity is a one-way door."** Every feature cut now is a feature that
doesn't burden the language forever. Scala should have done this. C++ couldn't.

#### 4. Test Infrastructure ✅

- 265/265 unit tests (cargo test --lib)
- harness tests 8/8 passed
- 164/165 interpreter + C codegen, 165/165 PE
- `verify.sh` with baseline tracking
- Golden file generation
- Bisect tooling

This is more test discipline than most research languages ever achieve. The
C codegen gap closure (test_asm, test_comptime_reflect) shows the test
infrastructure works and catches real issues.

#### 5. Self-Hosting Plan ✅

Phase 10 plans to rewrite the compiler in Fides itself. This is the right path —
it's how C, Go, Rust, Zig, and many others proved their language was real.
History lesson: **"A language is proven by what it can express, including
itself."**

#### 6. Comptime as Central Design ✅

Zig proved comptime can replace macros, complex generics, and much of what
other languages solve with type system gymnastics. Fides has comptime and uses it
for reflection, code generation, and f-string interpolation. This is the right
bet — it's one of Zig's strongest ideas.

---

### What IS NOT Going Right

#### 1. Scope Expansion — Real Synergy, Execution Risk �

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

#### 2. No Killer App, No Users, No Ecosystem 🔴

History lesson #3: **"Ecosystem > Language."** History lesson #9: **"Without
a platform, you need a killer framework."**

Current state:
- 0 users (not released)
- 0 packages (ark package manager implemented, registry pending)
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

#### 3. The Register Allocator Bug — Technical Debt Blocking Progress 🔴

The regalloc bug has been known for a long time and blocks the optimizer from
being enabled by default. This means:
- `--opt` can't be used
- PIR optimizer (inliner, CSE, DCE, unroll, TCO) is wasted
- Binary size and performance can't compete with Rust/Zig/Clang

History lesson: Rust's success was partly because it produced *fast* binaries.
If Fides can't optimize, it can't compete on performance — which is the #1 reason
to choose a systems language.

**This should be the #1 priority, not Phase 7 ergonomics.**

#### 4. Documentation Inconsistency 🟡

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

#### 5. 136 Compiler Warnings 🟡

136 warnings in the Rust compiler — mostly dead code in `pe.rs`, `opt.rs`,
`debugger.rs`, `wasm.rs`, `fmt.rs` (future feature stubs). Down from 225
after hygiene cleanup (unused imports, unused variables, dead constants
removed). The remaining warnings are infrastructure for future phases
(PE emitter rewrite, WASM backend, debugger) and should not be removed.

#### 6. No Generic Option<T> Yet 🟡 — resolved in `fid` (Nov 2026)

`fid` now instantiates `Option<T>`'s methods per `T` (proposal 0002); an
`Option` written without its type argument has `T = i64`. The original note:

The `?.` optional chaining implementation note says: "Full field access on
wrapped structs requires generic Option<T> (Phase 10)." This means Fides's Option
type currently only wraps i64 — a significant limitation. Rust's Option<T> is
one of its most important features. Fides can't do real ergonomic null-safety
without it.

This is a foundational gap, not an ergonomic feature. It should be higher
priority than game development or blockchain annotations.

#### 7. Too Many Annotations, Not Enough Substance 🟡

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

#### 8. "World Domination" Framing 🟡

The roadmap title is "Milestones to World Domination." Phase 12 plans to write
an OS kernel, web browser engine, database, AI framework, and robot control
system in Fides, plus get 3 companies using it and 1000+ developers.

This is aspirational, but history lesson #6: **"Technical merit ≠ adoption."**
D was technically excellent. Crystal is well-designed. Vale has novel ideas.
None achieved "world domination." The framing risks creating a culture where
ambition replaces focus.

---

### What We Should Keep in Mind

#### 1. Pick ONE Domain and Win It

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

#### 2. Fix the Register Allocator Before Adding Features

The regalloc bug is the single highest-impact technical issue. It blocks:
- The optimizer
- Performance competitiveness
- Binary size optimization
- Confidence in the codegen pipeline

Every hour spent on F8 `?.` or F11 `is` while the regalloc bug persists is an
hour not spent on the thing that would make Fides competitive with Rust/Zig.

**Recommendation:** Make the regalloc fix the #1 priority. Everything else
is secondary.

#### 3. Get Real Users Before Phase 8

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

#### 4. Generic Option<T> and Vec<T> Are Foundational, Not Features

Fides currently has Option wrapping only i64, and Vec/HashMap/String are
hardcoded in the compiler (522 lines of special cases across backends).

This is not a "Phase 10" issue. This is a "Phase 1" issue that was deferred.
Without generic Option<T>, Fides can't do real null-safety. Without Vec<T> in Fides
code, Fides can't be self-hosting. These are foundational.

**Recommendation:** Move generic Option<T> and the Vec/HashMap/String
migration to immediately after the regalloc fix. They're prerequisites for
everything else.

#### 5. Don't Build 18 Annotations — Build 3 and Prove Them

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

#### 6. Documentation Must Match Reality

Every doc should be updated when test counts change. ~~The current state where
`features.md` says 76/78 but reality is 114/114 is unacceptable for a project
that wants to be taken seriously.~~ **UPDATE Aug 2026: All docs now match
verified reality (164/165 interpreter+C codegen, 165/165 PE).**

**Recommendation:** Add a pre-commit check: run `fidesc test`, compare
output to docs, fail if mismatch. Or at minimum, update all docs in the same
commit that changes test results.

#### 7. The "V Risk" Is Real

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

#### 8. Self-Hosting Should Come Before Ecosystem

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

#### 9. Study D and Crystal as Cautionary Tales

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

#### 10. Study Erlang and Julia as Success Models

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

### Scorecard

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

### Recommended Priority Order

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
   → LSP server (oracle/fidesc lsp), formatter (forge/fidesc fmt), package manager (ark), doc generator (scribe), coverage (veil)
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

### The One Question That Matters

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


## Design Decisions

*From `decisions.md`.*

> Record of deliberate design choices, why they were made, and what the
> alternatives were. Future sessions should check here before questioning
> an existing design.

---

### ADR-001: Memory Model Hierarchy (Path B)

**Date:** Sep 2026
**Status:** Accepted
**Supersedes:** Vision's original "one memory model (regions only)" claim

#### Context

The vision (`docs/reference/vision.md`) originally promised a single memory
model: regions, replacing Rust's ownership + borrowing + RC. In practice,
the implementation grew three mechanisms:

1. **Regions** — `region r { ... }`, compile-time escape detection, bulk free
2. **Borrow checker** — `&mut` exclusivity checking (opt-in via `--borrowck=on`)
3. **RC** — `Rc<T>`/`Arc<T>` runtime reference counting

This created a tension: the vision says "one way to do things," but the
implementation has three. We evaluated two paths:

- **Path A:** Commit to the vision — remove the borrow checker and RC, keep
  only regions. Bold, differentiated, but every graph/cache/callback pattern
  would require `unsafe` raw pointers.
- **Path B:** Keep all three, but establish a clear hierarchy — regions as
  default, borrow checker as opt-in, RC as the safe alternative to `unsafe`.

#### Decision

**Path B.** Keep all three memory mechanisms with a documented hierarchy:

| Model | Role | When | Cost |
|-------|------|------|------|
| Regions | Default | Scoped data, predictable lifetime | Zero |
| References | Always available | Borrowed access | Zero (unchecked by default) |
| Borrow checker | Opt-in | Compile-time aliasing safety | Zero (opt-in complexity) |
| RC | Safe escape hatch | Shared ownership, dynamic lifetime | Runtime refcount |
| `unsafe` | Framework-only | Raw pointers, inline asm | N/A (banned in user code) |

#### Rationale

The core insight: **RC replaces `unsafe` as the fallback for patterns regions
can't express.**

Patterns that are impossible with regions alone:
- Shared ownership (doubly-linked lists, graphs with cycles, parent/child links)
- Dynamic lifetime (caches, callbacks, observer registration)
- Thread-safe shared state (Arc across spawned threads)
- Self-referential structures

Without RC, these patterns force the user into `unsafe { }` raw pointers —
which violates the vision's "no `unsafe` in user code, ever." With RC, the
escape hatch is memory-safe. This reframes RC from "we copied Rust" to
"we eliminated the need for `unsafe` in user code."

#### Consequences

- **Positive:** No `unsafe` needed for any memory-safe pattern. Regions stay
  zero-cost default. Borrow checker is available but not forced.
- **Negative:** Three mechanisms instead of one. Users must learn which to
  use when. Potential for inconsistency across codebases.
- **Mitigation:** Lint warning when RC is used where regions would suffice.
  First-party code (stdlib, compiler) should prefer regions. Document the
  hierarchy clearly in all guides.
- **Reversibility:** If we later choose Path A (regions only), the cost is
  low today: RC is used in 0 stdlib files, 1 test file, and runtime support
  is ~30 lines. The borrow checker is a single isolated pass. Removal is
  ~1-2 days of work. **This cost grows as more code adopts RC.**

#### Reversibility guardrails

To keep the door open for Path A:
1. First-party code (stdlib, compiler, tests) must prefer regions over RC.
2. RC usage in stdlib/compiler requires justification in a comment.
3. The borrow checker must remain opt-in, never default.
4. A lint should warn on RC usage that could be replaced with regions.


## Future: AI, Robotics, Hardware, IoT

*From `future.md`.*

> Fides is designed for the next 50 years of computing, not the last 50.

### Why Existing Languages Fail at the Future

#### C/Rust/Zig were designed for CPUs
- No native SIMD support (C/Rust have intrinsics but they're not first-class)
- No tensor types (you need libraries like NumPy or PyTorch)
- No GPU targeting (you need CUDA/HIP/SYCL — separate languages)
- No real-time guarantees (C has volatile, Rust has no formal real-time story)

#### Python/Java/Go were designed for servers
- Garbage collection pauses break real-time
- Runtime overhead breaks embedded constraints
- No hardware access (can't write a device driver in Python)
- No memory predictability (GC can pause at any time)

#### The gap
No language can:
1. Write a robot control loop with real-time guarantees
2. Run an AI inference on a microcontroller
3. Target a GPU without a separate language
4. Write a device driver with memory safety
5. Hot-reload code on a running robot

**Fides targets all of these.**

---

### AI/ML — First-Class Support

#### SIMD Types (WORKING — see `stdlib/simd.fd`)
```
// SIMD is a first-class type, not an intrinsic
val a: simd[i64; 4] = [1, 2, 3, 4];
val b: simd[i64; 4] = [5, 6, 7, 8];
val c: simd[i64; 4] = a + b;  // vectorized addition, one instruction
```

#### Tensor Type (WORKING — see `stdlib/tensor.fd`)
```
// Tensors are n-dimensional arrays with stride info
val weights: Tensor[f32; 784, 128] = load_weights("model.bin");
val input: Tensor[f32; 1, 784] = load_input("image.raw");
val output: Tensor[f32; 1, 128] = matmul(input, weights);
```

#### comptime Shape Verification
```
fn comptime matmul[M: comptime, N: comptime, K: comptime](
    a: Tensor[f32; M, K],
    b: Tensor[f32; K, N]
) -> Tensor[f32; M, N] {
    // Shape mismatch = compile error, not runtime error
    val result: Tensor[f32; M, N] = Tensor::zeros();
    // ... implementation
    return result;
}
```

#### GPU Targeting (Implemented — SPIR-V codegen in `src/gpu.rs`)
```
// Same Fides code, different target
#[target(gpu)]
fn train_step(batch: Tensor[f32; 32, 784]) -> Tensor[f32; 32, 10] {
    // PIR lowers to SPIR-V or PTX for GPU
    // Same language, same types, different codegen
}
```

#### Auto-Parallelization
```
// Effect system knows this is pure → can auto-parallelize
fn sum(xs: &[i64]) -> i64 ![] {
    val total = 0;
    for x in xs {
        total += x;
    }
    return total;
}
// Compiler can split xs into chunks and run on multiple cores
// No explicit threading needed
```

---

### Robotics — Real-Time + Safety

#### Real-Time Effects
```
// !RealTime means: this function must complete within a deadline
fn control_loop(sensor: SensorCap) -> MotorCommand !RealTime !IO {
    val reading = sensor.read()?;
    val command = compute_pid(reading);
    return command;
}

// The compiler verifies:
// 1. No allocations in !RealTime functions (no GC pauses)
// 2. No blocking IO (must use effect handlers)
// 3. No unbounded loops (must have iteration limit)
// 4. Region memory only (deterministic deallocation)
```

#### Hardware Abstraction
```
// Robot sensors and actuators as capabilities
fn robot_main(motor: MotorCap, sensor: SensorCap) !IO !RealTime {
    loop {
        val reading = sensor.read()?;
        val command = control_loop(reading);
        motor.write(command)?;
    }
}

// The caller decides what hardware to use:
fn main() {
    val motor = MotorCap::new("/dev/ttyUSB0");
    val sensor = SensorCap::new("/dev/i2c-1");
    robot_main(motor, sensor)?;
}
```

#### Hot Reload on Running Robots
```
// Change the control_loop function while the robot is running
// No restart, no state loss, no safety violation
// The new code is type-checked and safety-checked before patching
```

---

### IoT & Embedded — Tiny + Deterministic

#### Bare Metal Targeting
```
// No OS, no runtime, no stdlib — just raw hardware
#[target(bare_metal)]
#[no_std]
fn main() {
    // Direct register access (framework code, not user code)
    val gpio = 0x3F200000 as *mut u32;
    unsafe_ptr_write(gpio, 1);  // turn on LED
}
```

#### Deterministic Memory
```
// FixedBufferAllocator — no heap, no fragmentation, no surprises
fn firmware_main() {
    val buf: [u8; 4096] = [0; 4096];
    val alloc = FixedBufferAllocator::new(&buf);
    
    // All allocations come from buf — deterministic, no surprises
    val data = alloc.alloc(256)?;
    process(data)?;
    // No free needed — allocator resets at end of scope (region)
}
```

#### Tiny Binaries
```
// Fides produces smaller binaries than C because:
// 1. Dead code elimination at IR level (not link time)
// 2. No libc dependency
// 3. No runtime
// 4. comptime inlines constants (no runtime initialization)
// Target: < 4KB for a simple LED blinker
```

#### Cross-Compilation
```
// Build for any target from any host
fidesc build --target riscv32-embedded    // RISC-V microcontroller
fidesc build --target arm64-bare-metal    // Raspberry Pi
fidesc build --target x86_64-linux        // Server
fidesc build --target wasm                // Web
```

---

### Hardware — Direct Access

#### Inline Assembly
```
// Inline assembly for hardware-specific instructions (framework code only)
fn read_cycle_counter() -> u64 {
    asm!("rdtsc", out("rax") _, out("rdx") _);
    // Returns the CPU cycle counter
}
```

#### Interrupt Handlers
```
#[interrupt(timer0)]
fn timer_interrupt() !RealTime {
    // Handle timer interrupt
    // No allocations, no blocking, no exceptions
    val state = get_global_state();
    state.tick += 1;
}
```

#### MMU Management
```
// OS kernel primitives (framework code)
fn map_page(virt: usize, phys: usize, flags: PageFlags) {
    val pt = get_page_table();
    pt[virt >> 12] = phys | flags.bits();
    asm!("invlpg ({})", in(reg) virt);
}
```

---

### The Unified Vision

```
┌─────────────────────────────────────────────────────┐
│                  Fides Language                     │
│                                                     │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌──────────┐  │
│  │ AI/ML   │ │ Robotics│ │  IoT    │ │ Servers  │  │
│  │ SIMD    │ │ RT Effects│ │ Bare   │ │ Effects  │  │
│  │ Tensors │ │ Caps    │ │ Metal   │ │ Hot Reload│  │
│  │ GPU     │ │ Hot     │ │ Tiny    │ │ Fast     │  │
│  │         │ │ Reload  │ │ Bins    │ │ Compile  │  │
│  └─────────┘ └─────────┘ └─────────┘ └─────────┘  │
│                                                     │
│  ┌─────────────────────────────────────────────┐    │
│  │              PIR (Fides IR)                 │    │
│  │  Regions + Effects + Capabilities + comptime │    │
│  └─────────────────────────────────────────────┘    │
│                                                     │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌────────┐ │
│  │ x86-64   │ │ ARM64    │ │ RISC-V   │ │ WASM   │ │
│  │ Codegen  │ │ Codegen  │ │ Codegen  │ │ Codegen│ │
│  └──────────┘ └──────────┘ └──────────┘ └────────┘ │
│                                                     │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐            │
│  │ ELF      │ │ PE       │ │ Mach-O   │            │
│  │ Linux    │ │ Windows  │ │ macOS    │            │
│  └──────────┘ └──────────┘ └──────────┘            │
└─────────────────────────────────────────────────────┘
```

**One language. Every platform. Every domain. All safe. All fast. All custom-built.**

---

## Extra — Nice-to-Have Community Goals

> Not on the critical path. Not required for Fides to be useful.
> These are ideas that would help adoption and community growth,
> but the language and compiler are complete without them.

### Migration Tools

#### C-to-Fides Transpiler (Feasible — moderate effort)

```
// Input: C code
int add(int a, int b) { return a + b; }

// Output: Fides code
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}
```

- Auto-translate C syntax to Fides syntax
- Add memory safety annotations (regions, capabilities)
- Flag unsafe patterns for manual review
- Gradual migration: translate one file at a time

**Honest assessment:** The `add` example is the easy 5%. Real C has
preprocessor macros, pointer arithmetic, `goto`, union punning, function
pointers, switch fallthrough, and implicit conversions. Existing precedent
(`c2rust`, `corrode`) shows this is a multi-year effort even with mature
ecosystems. Tractable but not trivial.

**What already works:** Fides has `cparser.rs` (C header parser) and
`extern "C"` FFI — calling C from Fides is already covered. A transpiler would
help porting C *logic* into Fides, but FFI covers the interop case.

#### Rust-to-Fides Transpiler (Aspirational — very high effort)

```
// Input: Rust code
fn add(a: i64, b: i64) -> i64 { a + b }

// Output: Fides code
fn add(a: i64, b: i64) -> i64 {
    return a + b;
}
```

- Convert Rust syntax to Fides syntax
- Replace lifetimes with regions (auto-inferred)
- Replace async/await with effect handlers
- Replace macros with comptime

**Honest assessment:** This is the hardest item in this file. Rust's borrow
checker, lifetimes, macros (`macro_rules!`, proc macros), complex trait
system, async, and `unsafe` don't map cleanly to Fides.

- "Replace lifetimes with regions" is optimistic — Fides's regions and Rust's
  lifetimes are semantically different systems. Mechanical translation is
  not straightforward.
- "Replace macros with comptime" only works for simple declarative macros.
  Proc macros generate arbitrary token streams that can't be transpiled
  without re-implementing the macro system.

**Practical alternative for Rust migration:** Use FFI to call Rust from Fides
(via C ABI), port files manually over time. A full Rust-to-Fides transpiler
that produces *correct, idiomatic* Fides code is a research project, not a
tooling task.

#### FFI Generator (Already working)

```
// Input: C header (openssl.h)
SSL_CTX* SSL_CTX_new(const SSL_METHOD* method);

// Output: Fides FFI binding
extern "C" fn SSL_CTX_new(method: *const SSL_METHOD) -> *mut SSL_CTX;
```

- Parse C headers automatically
- Generate Fides FFI bindings
- No hand-written bindings needed
- **Working now** — `fidesc parse-header <file.h>` parses C headers and
  emits Fides FFI bindings (`src/cparser.rs`)

This is the only migration tool that exists today. It covers the most
common need (calling C from Fides) without requiring a full transpiler.

---

### Community & Documentation

#### Language Specification
- Formal grammar + semantics document
- Not required for using Fides, but needed for third-party implementations
- and academic credibility

#### Fides Book
- Like "The Rust Programming Language"
- Tutorial-style introduction for new users
- Would significantly help adoption

#### Examples Library
- Real-world code samples (web server, CLI tool, embedded firmware)
- Shows Fides's strengths in practice

#### Playground
- Web-based Fides IDE (compile + run in browser)
- Uses WASM backend (already implemented)
- Low effort since WASM target exists

#### Community Forum / Discord
- For support, announcements, package sharing
- Pure community effort, no compiler work needed

---

### Medical Device Compliance Tooling

These items address gaps identified during analysis of the medical device industry's
language limitations. They are ecosystem/tooling features, not language features —
the language-level safety (regions, bounds, @crashfree, @medical, @audit) is already
in the roadmap (Phases 7.5 and 9B). These tools support the *process* side of medical
device compliance (IEC 62304, FDA cybersecurity guidance).

#### SBOM Generator (`ark sbom`) — Depends on package manager

- Scan Fides import graph, generate Software Bill of Materials
- List all dependencies: stdlib modules, FFI libraries, transitive imports
- Include versions and content hashes
- Required for FDA 2025 cybersecurity compliance and IEC 62304 SOUP management
- Output format: SPDX or CycloneDX (industry-standard SBOM formats)

**Why this is extra, not roadmap:** An SBOM is only meaningful with versioned,
registered packages. Fides has ark package manager (Phase 13.4). SBOM generation is now a natural companion feature.

#### CVE Checker (`ark audit`) — Depends on SBOM + package registry

- Check dependencies against known vulnerability databases (NVD, OSV)
- Flag CVEs in SOUP (Software of Unknown Provenance) components
- Requires SBOM generator + package registry to be useful
- Ongoing maintenance: CVE databases update constantly

**Why this is extra, not roadmap:** Same dependency as SBOM — needs a package
registry. Also requires ongoing infrastructure (CVE database updates). Consider
integrating with existing services (NVD API, OSV.dev) rather than hosting a
database. This is a CI/CD tool, not a compiler feature.

#### IEC 62304 Process Integration — Tooling, not compiler

IEC 62304 is a *process* standard, not a coding standard. It requires:
- **Requirements traceability:** each requirement → design → code → test
- **Risk management:** each hazard → risk control → verification
- **Problem resolution:** each bug → root cause → fix → regression test

Fides's language-level features (@crashfree, @medical, @audit, @bounded) serve
as *risk control measures* in the IEC 62304 risk management process. But the
*process tooling* — linking hazards to code, tracing requirements through
implementation, tracking problem resolution — is a separate concern.

**What this tooling would do:**
- Link Fides functions and tests to requirement IDs (via annotations or external mapping)
- Export traceability matrices for auditor review
- Map @crashfree/@medical/@bounded annotations to hazard IDs as risk controls
- Integrate with issue trackers for problem resolution tracking

**Why this is extra, not roadmap:** Full IEC 62304 process tooling is a product
category on its own (companies like Polarion, Jama, Visure sell this). Building
it into Fides is scope creep. The right approach: Fides provides the language-level
safety guarantees, and third-party tooling or integrations handle the process-level
requirements. Consider an export format that integrates with existing ALM tools
rather than building a full requirements management system.

#### CWE Mapping Documentation (`docs/fides-cwe-mapping.md`) — Low effort, high value

Map Fides safety features to CWE (Common Weakness Enumeration) categories for
medical device auditor review:

| CWE ID | CWE Name | Fides Prevention |
|--------|----------|-----------------|
| CWE-787 | Out-of-bounds Write | Bounds checking + region escape detection |
| CWE-416 | Use-After-Free | Region system (compile-time lifetime tracking) |
| CWE-476 | NULL Pointer Dereference | Option<T> (no null in the language) |
| CWE-190 | Integer Overflow | Checked arithmetic in debug mode |
| CWE-119 | Improper Bounds Restriction | Bounds checking (compile-time + runtime) |
| CWE-20 | Improper Input Validation | @crashfree (reject unhandled errors) |
| CWE-125 | Out-of-bounds Read | Bounds checking + region escape detection |
| CWE-788 | Access of Memory Location of Wrong Size | Type system (fixed-width types) |
| CWE-479 | Use of a Pointer Derived from an Untrusted Source | Capability system (untrusted code can't access pointers) |

**Why this is extra, not roadmap:** This is documentation, not code. It doesn't
change the compiler. It can be written now — the safety features it maps (regions,
bounds, Option) already exist. The only risk is accuracy: if the mapping claims a
CWE is prevented and there's an edge case, that's a false claim in a safety-critical
context. The mapping must be verified, not just written. Low effort, high value
for medical device adoption — auditors expect CWE coverage documentation.

---

### Why These Are Separate From the Roadmap

The main roadmap (`docs/roadmap.md`) tracks what's needed to make Fides a
complete, correct, fast language. The items here are about making Fides
*popular* — which depends on community, marketing, and adoption dynamics
that are orthogonal to compiler correctness.

**Priority:** Finish Phases 4–5 (fix regalloc, C codegen coverage) first.
Then Phase 6 (self-hosting). Then Phase 7 ecosystem tooling (LSP, package
manager, formatter). The items in this file are "if there's time and
interest" — not blockers for anything.

---

### What Actually Drives Adoption (Not Transpilers)

Based on how languages actually grow communities:

1. **A killer app** — something Fides can do that others can't (hot reload +
   safety + bare metal in one language)
2. **Good docs** — people can learn Fides in an afternoon
3. **LSP support** — IDE autocomplete, go-to-definition, error highlighting
4. **Package manager** — `ark add <dep>` just works
5. **Real projects** — at least one notable project written in Fides

Transpilers are nice marketing but historically don't drive adoption.
TypeScript succeeded because of types + JS ecosystem, not because of
transpilers *to* TypeScript. Rust succeeded because of safety + cargo,
not because of C-to-Rust tools.
