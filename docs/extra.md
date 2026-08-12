# Extra — Nice-to-Have Community Goals

> Not on the critical path. Not required for P to be useful.
> These are ideas that would help adoption and community growth,
> but the language and compiler are complete without them.

## Migration Tools

### C-to-P Transpiler (Feasible — moderate effort)

```
// Input: C code
int add(int a, int b) { return a + b; }

// Output: P code
pf add(a: i32, b: i32) -> i32 {
    ret a + b;
}
```

- Auto-translate C syntax to P syntax
- Add memory safety annotations (regions, capabilities)
- Flag unsafe patterns for manual review
- Gradual migration: translate one file at a time

**Honest assessment:** The `add` example is the easy 5%. Real C has
preprocessor macros, pointer arithmetic, `goto`, union punning, function
pointers, switch fallthrough, and implicit conversions. Existing precedent
(`c2rust`, `corrode`) shows this is a multi-year effort even with mature
ecosystems. Tractable but not trivial.

**What already works:** P has `cparser.rs` (C header parser) and
`extern "C"` FFI — calling C from P is already covered. A transpiler would
help porting C *logic* into P, but FFI covers the interop case.

### Rust-to-P Transpiler (Aspirational — very high effort)

```
// Input: Rust code
fn add(a: i64, b: i64) -> i64 { a + b }

// Output: P code
pf add(a: i64, b: i64) -> i64 {
    ret a + b;
}
```

- Convert Rust syntax to P syntax
- Replace lifetimes with regions (auto-inferred)
- Replace async/await with effect handlers
- Replace macros with comptime

**Honest assessment:** This is the hardest item in this file. Rust's borrow
checker, lifetimes, macros (`macro_rules!`, proc macros), complex trait
system, async, and `unsafe` don't map cleanly to P.

- "Replace lifetimes with regions" is optimistic — P's regions and Rust's
  lifetimes are semantically different systems. Mechanical translation is
  not straightforward.
- "Replace macros with comptime" only works for simple declarative macros.
  Proc macros generate arbitrary token streams that can't be transpiled
  without re-implementing the macro system.

**Practical alternative for Rust migration:** Use FFI to call Rust from P
(via C ABI), port files manually over time. A full Rust-to-P transpiler
that produces *correct, idiomatic* P code is a research project, not a
tooling task.

### FFI Generator (Already working)

```
// Input: C header (openssl.h)
SSL_CTX* SSL_CTX_new(const SSL_METHOD* method);

// Output: P FFI binding
extern "C" pf SSL_CTX_new(method: *const SSL_METHOD) -> *mut SSL_CTX;
```

- Parse C headers automatically
- Generate P FFI bindings
- No hand-written bindings needed
- **Working now** — `pledgec parse-header <file.h>` parses C headers and
  emits P FFI bindings (`src/cparser.rs`)

This is the only migration tool that exists today. It covers the most
common need (calling C from P) without requiring a full transpiler.

---

## Community & Documentation

### Language Specification
- Formal grammar + semantics document
- Not required for using P, but needed for third-party implementations
- and academic credibility

### P Book
- Like "The Rust Programming Language"
- Tutorial-style introduction for new users
- Would significantly help adoption

### Examples Library
- Real-world code samples (web server, CLI tool, embedded firmware)
- Shows P's strengths in practice

### Playground
- Web-based P IDE (compile + run in browser)
- Uses WASM backend (already implemented)
- Low effort since WASM target exists

### Community Forum / Discord
- For support, announcements, package sharing
- Pure community effort, no compiler work needed

---

## Why These Are Separate From the Roadmap

The main roadmap (`docs/roadmap.md`) tracks what's needed to make P a
complete, correct, fast language. The items here are about making P
*popular* — which depends on community, marketing, and adoption dynamics
that are orthogonal to compiler correctness.

**Priority:** Finish Phases 4–5 (fix regalloc, C codegen coverage) first.
Then Phase 6 (self-hosting). Then Phase 7 ecosystem tooling (LSP, package
manager, formatter). The items in this file are "if there's time and
interest" — not blockers for anything.

---

## What Actually Drives Adoption (Not Transpilers)

Based on how languages actually grow communities:

1. **A killer app** — something P can do that others can't (hot reload +
   safety + bare metal in one language)
2. **Good docs** — people can learn P in an afternoon
3. **LSP support** — IDE autocomplete, go-to-definition, error highlighting
4. **Package manager** — `pledge get <package>` just works
5. **Real projects** — at least one notable project written in P

Transpilers are nice marketing but historically don't drive adoption.
TypeScript succeeded because of types + JS ecosystem, not because of
transpilers *to* TypeScript. Rust succeeded because of safety + cargo,
not because of C-to-Rust tools.
