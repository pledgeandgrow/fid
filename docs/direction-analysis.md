# Fides Language — Direction Analysis

> Are we going in the right direction?
> An honest assessment based on `docs/history.md` lessons, `docs/roadmap.md`,
> `docs/features-roadmap.md`, `docs/roadmap-limitations.md`, `features.md`,
> and actual test baselines.

---

## Executive Summary

**Fides is doing many things right. It also has serious strategic risks that could
make it another D, Crystal, or V — technically good, commercially irrelevant.**

The project has built an impressive amount of working compiler infrastructure
(52,845 LOC, 36 source files, 86/86 test parity across 3 backends, 9 stdlib
modules, 6 backends). But it's spreading into too many domains (games,
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

86/86 test parity across interpreter, PE, and C codegen is real. This is more
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

- 137 unit tests
- 8/8 harness tests
- 86/86 interpreter + PE + C codegen parity
- 3/3 expected-fail tests
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
- 0 packages (no package manager)
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
- `features.md` now says "86/86 C codegen — FULL PARITY" ✅
- `features.md` now says "86/86 interpreter" ✅
- `roadmap-limitations.md` now says "86/86 + 3/3 expected-fail" ✅
- `roadmap-limitations.md` now says "215 warnings" ✅
- `roadmap.md` summary now says "86/86" ✅
- Phase numbers updated (self-hosting is Phase 10, ecosystem Phase 11, world class Phase 12) ✅

This was a symptom of moving fast without updating all docs. The docs now
match reality. A pre-commit check would prevent future drift.

### 5. 214 Compiler Warnings 🟡

214 warnings in the Rust compiler means ~60 unused imports, dead code, and
type naming issues. This is technical debt that accumulates. Every warning is
a potential bug or a sign of abandoned code paths. The roadmap mentions
"remove 221 debug eprintln! statements" but the actual count is different.

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
`features.md` says 76/78 but reality is 86/86 is unacceptable for a project
that wants to be taken seriously.~~ **UPDATE Aug 2026: All docs now match
verified reality (86/86 FULL PARITY across all backends).**

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
| Technical execution | 🟢 8/10 | 86/86 parity, 6 backends, solid test infra |
| Scope discipline | 🔴 3/10 | Trying to do everything at once |
| Ecosystem building | 🔴 2/10 | No users, no packages, no tooling |
| Focus / prioritization | 🟡 4/10 | Regalloc bug unfixed, adding features instead |
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
   → Proves Fides is real

8. 🟢 Start ecosystem (Phase 11 basics)
   → LSP server, formatter, package manager
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
