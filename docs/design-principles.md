# Pledge Design Principles

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

**Exception:** The Rust compiler (for now) — used to build Pledge, but the end goal is self-hosting.

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

- Syntax is familiar (Rust-like with Pledge keywords)
- Error messages are clear and actionable
- No lifetime annotations (regions are inferred)
- No macro complexity (comptime is just regular code)
- No async/await complexity (effect handlers are transparent)
- Hot reload provides instant feedback (learn by doing)

**Anti-pattern:** Rust's 6-month learning curve.
**Anti-pattern:** C++'s infinite complexity.
**Anti-pattern:** Haskell's category theory requirement.
