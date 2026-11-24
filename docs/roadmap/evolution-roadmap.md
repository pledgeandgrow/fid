# Evolution Roadmap — Ideas From Other Languages

> Language features that Fides does not have, taken from newer or research
> languages (Goose, Hylo, Vale, Pony, Swift, Koka/Roc/Lean, Zig, Odin/Jai,
> Erlang/Gleam, Dafny/SPARK), and ideas no known language has that Fides'
> own toolchain makes possible (EV17-EV24). Each entry was checked against
> docs/reference/reference.md and the repo on Nov 24 2026: it is not in the
> language today.
>
> Estimates are judgments, not measurements. **First version**: it runs and
> its own tests pass. **Solid**: it works on run, C, pe_ir and nat, has
> negative tests, passes the full gate, and survives a review pass. The gap
> is real: generics (0002) landed in two days, and the Nov 22 review still
> found 125 bugs (bugs-roadmap.md).
>
> Sizes: (S) days, (M) weeks, (L) months.

## Before any of this

- [ ] EV0. Fix BG108 (the SSA round trip miscompiles loops that rotate
      their variables, on pe_ir and nat) and run the full suite with `fid`
      as the compiler. Building new features on a compiler that miscompiles
      loops makes every estimate below slip.

Two costs come on top of every entry:
- **The test cycle.** A self-hosted build is about 10-15 minutes and the
  full gate hours (the whole-suite run lane alone up to 3): a few verified
  iterations a day, however fast the code is written.
- **Using a feature in the compiler.** compiler/*.fd is built by the
  pinned stage 0 (scripts/stage0.sha256): new syntax cannot appear in the
  compiler's own source until stage 0 is re-pinned to a fid that has it.

## Phase 1 — small, low risk (about a month)

- [ ] EV1. `must_use` results (S). From Rust, Swift.
      Ignoring a returned `Result` (or a function marked `@must_use`) is a
      warning. Type checker only; it already emits warnings
      (`self.warnings` in typechecker.fd).
      First version: hours. Solid: 1 day.

- [ ] EV2. `errdefer` (S). From Zig.
      Cleanup that runs only when the function leaves through an error:
      `?` passing up an `Err`/`None`, or a `return` of one. Today `defer`
      always runs at function exit (reference §5.5). Lowering only: it
      builds on `defer_stack` and the `?` path of
      `Lowerer_lower_try_expr` (whose drops BG65 fixed), and lowers to
      existing IR, so every backend gets it.
      First version: 1 day. Solid: 2-3 days.

- [ ] EV3. Tuple, struct and or-patterns (S-M). From Rust, ML.
      Reference §6: "there are no tuple, struct or `|` patterns". The
      parser already rewrites nested constructor patterns into inner
      matches (parser.fd `pd_desugar_arms`); these extend it. The
      exhaustiveness check (`TypeChecker_check_exhaustive`) must learn the
      new shapes.
      First version: 2-3 days. Solid: about a week.

- [ ] EV4. Generators (M). From Python, C#.
      `yield` is a keyword, but `yield e` "evaluates `e`: not implemented
      as a suspension" (reference §10.2). Green tasks already switch
      stacks on every backend, so a generator can be a task that suspends
      at each `yield`. The risk is ownership of values that live across a
      `yield` point.
      First version: about a week. Solid: 2-3 weeks.

## Phase 2 — the security niche (about two months)

- [ ] EV5. Proved contracts (M, then L). From Dafny, Liquid Haskell, SPARK.
      `given e` is checked only for constant arguments and `ensure e` is
      parsed and not checked (reference §2.1).
      Step 1 (M, 1-2 weeks): prove `@crashfree`'s "no division by zero"
      and "no out-of-bounds index" from the range analysis that already
      drops checks (compiler/checks.fd), with no solver.
      Step 2 (L, 2-3 months+): prove `given`/`ensure` with an SMT solver.
      z3 is not part of the compiler today: scripts/rules-smt.sh calls it
      from Python for the rewrite rules. Either z3 becomes an optional
      external tool (against "no external dependencies"), or Fides gets
      its own solver for integer arithmetic (+1-2 months). Loops need
      invariants.

- [ ] EV6. Isolated values across threads (M-L). From Swift (region-based
      isolation), Pony (`iso`).
      Closes the largest open safety gap: a `Vec`, `HashMap` or `str`
      passed to another thread is shared with the caller and not checked,
      and their owner counts are not atomic (memory-model.md; the BG75
      notes). A value sent to `spawn` or a channel must be the sender's
      last use, proved by the compiler; then it moves without a copy or a
      lock. The last-use analysis exists (compiler/cowcheck.fd, BG29). The
      cost is the rest: values held in struct fields, channels, globals,
      and stdlib code that relies on sharing a Vec between threads today.
      First version: about 2 weeks. Solid: 1-2 months.

## Phase 3 — the biggest friction (about three months)

- [ ] EV7. Projections (L). From Hylo (`subscript` with `yield`).
      References are second-class: they cannot be returned (reference
      §8.4), which is the main pain of writing Fides. A projection lends
      access to the caller without returning a reference, so `get_mut(i)`
      becomes safe with no lifetimes. It fits the second-class design
      exactly. New syntax, plus type checker, borrow check, escape check
      and lowering. Write a design proposal first, as for generics (0002)
      and ownership (0004).
      First version: 3-4 weeks. Solid: 2-3 months.

## Later, if users ask for them

- [ ] EV8. Memory reuse in place (M). From Koka, Roc, Lean 4 (Perceus).
      Half of it exists: `vec_own` (stdlib/core.fd) already writes in
      place when a Vec has one owner. Left: hand the last owner over
      instead of sharing then copying, and reuse a freed cell for the next
      allocation of its size. The work is in lower.fd's ownership code,
      where bugs have clustered (BG32, BG82, BG125). Measure the gain
      before starting: it may be small.
      First version: about a week. Solid: 2-3 weeks.

- [ ] EV9. Struct-of-arrays layout (M-L). From Jai, Odin, Zig's
      `MultiArrayList`.
      `@soa` on a `Vec<Particle>` stores each field as its own array: good
      for SIMD and caches, and for stdlib/ecs.fd and physics3d.fd. A second
      Vec layout across core, the C runtime's PVec and pe_ir's inlined Vec
      paths (fragile: BG7 broke them for f32).
      First version: 2-3 weeks. Solid: 1-2 months.

- [ ] EV10. Supervised tasks (L). From Erlang, Gleam.
      Today a panic in any task ends the whole program (from-go.md). A
      supervisor restarts a failed task instead. The hard part is
      recovering from a panic the same way on four backends: a dead task
      leaves memory, owner counts and held locks behind.
      First version: 3-4 weeks. Solid: 2-3 months.

- [ ] EV11. Generational references (L). From Vale.
      A reference that can be stored, checked cheaply on each use: an
      escape hatch for graphs, where Fides uses indices today. Needs a new
      pointer representation and allocator header in three runtimes
      (core, the C runtime, pe_runtime/linux_runtime) and a check on every
      use. It works against the rule that references cannot be stored:
      last priority.
      First version: 1-2 months. Solid: 3+ months.

## From Goose

Goose (github.com/aardappel/goose, Nov 2026) keeps all data on
compiler-assigned bump stacks freed at scope exit: no allocator, no GC, no
reference counting. Adopting that model would replace Fides' ownership
model, so it is not proposed. These parts fit on their own:

- [ ] EV12. Slices (S-M). `T[:]`, a view of part of a Vec or array that
      cannot outlive it. Fides has no slice type (reference §3.1); the
      rule that references cannot escape already gives the safety.

- [ ] EV13. Verified byte images (M). `to_bytes` writes a structure in one
      write; `from_bytes` checks every tag, length and link before the
      bytes become a value, so a hostile file returns an error, never a
      bad value. Parsing untrusted input with no parser to get wrong fits
      the security niche. Fides has `@derive(Serialize)` only.

- [ ] EV14. Variable-size enums (M-L). Each value takes the size of its
      own variant instead of `(1 + max payload)` words; Goose reports 4x
      less memory than a Rust `enum` on its benchmark. An array of them
      can be iterated, not indexed.

- [ ] EV15. Returning an error several frames up (M). `return err from
      load` leaves a function any number of frames up the stack, checked
      statically, with no `Result` or `?` on each call.

- [ ] EV16. Case functions (S-M). One overload per enum variant; the
      compiler checks they cover every variant and dispatches through a
      jump table.

## Only possible with an own toolchain

Ideas that need what Fides has and LLVM-based languages do not: the
effects of every function, four independent backends, and checks run on
the final machine code. No language is known to do these; each entry names
the closest existing tool. Checked against the repo on Nov 24: none is
done, some are half there.

- [ ] EV17. The compiler writes the OS sandbox (M-L).
      The effects of everything `main` reaches (reference §9.1) become a
      kernel filter installed at start: seccomp-BPF and Landlock on Linux,
      a job object / AppContainer on Windows, pledge/unveil on OpenBSD. A
      program whose source never touches the network cannot open a
      socket, even after an exploit or a compiler bug. Closest: OpenBSD
      `pledge` (called by hand), Capslock for Go (analysis, no
      enforcement), Deno permissions (set by the user at run time).
      Proposed in docs/reference/lessons.md (`--isolate`), not
      implemented. The allowlist must include the runtime's own system
      calls (allocator mmap, threads, the signal stack).
      First version (Linux, stdlib/linux_runtime and the C runtime): 1-2
      weeks. Solid (every OS, `sandbox { }` blocks too): 1-2 months.

- [ ] EV18. Diverse redundancy (M, then L).
      `@redundant` already makes two channels and a voter
      (compiler/mono.fd, safety-rules.md SR-19), but both are compiled
      the same way, so one compiler bug breaks both identically. Compile
      the two channels differently and link both into one binary: a
      miscompile in one is caught by the vote at run time (BG108 would
      have been). Closest: aviation's diverse compilers, used by hand by
      separate teams.
      Step 1 (M, 2-3 weeks): different pipelines in one backend (iropt
      off in one channel, the greedy allocator in the other).
      Step 2 (L, 2-3 months): different backends (pe_ir and nat) in one
      image.

- [ ] EV19. Proof-carrying binaries (L).
      The executable carries a section with what was proved per function
      (effects, `@constant_time`, `@no-alloc`, the stack bound), and a
      small independent checker, `fid verify-bin`, re-checks those facts on
      the machine code without trusting the compiler. Builds on the
      machine-level constant-time check (J7), `--validate` (compiler/tv.fd)
      and `fid qualify`. Closest: proof-carrying code (Necula, 1996), never
      in a mainstream language. The checker must stay small and share no
      code with the compiler, or it proves nothing.
      First version (effects and constant time): 3-4 weeks. Solid: 2-3
      months.

- [ ] EV20. Exact stack and time bounds from the machine code (M, then L).
      `@bounded-stack(N)` counts call frames (reference §11). The
      backends know every frame's size, so the bound can be exact bytes:
      `fid bounds main` gives the deepest stack in bytes, and
      `@bounded-stack(bytes = N)` is checked against the final code. On
      Cortex-M (compiler/nat_thumb.fd, a simple in-order core) a cycle
      bound per function (WCET) is possible too. Closest: AbsInt's aiT and
      StackAnalyzer, standalone commercial tools.
      Stack bytes: 1-2 weeks. Cycle bounds on Cortex-M: 2-3 months (needs
      loop bounds).

## The pieces exist already

- [ ] EV21. A lockfile for the effects of dependencies (M).
      `ark.lock` records each dependency's effects and capabilities; an
      update in which a package gains `Net`, `IO` or `Spawn` (a JSON
      library that starts opening sockets) fails the build until a person
      approves it. Catches the common supply-chain attack automatically.
      Artifacts already carry per-module metadata (LG4, version 5).
      Closest: Capslock (analysis only), cargo-vet (human audits), Deno
      (run-time permissions).
      First version: 1-2 weeks. Solid: about a month.

- [ ] EV22. A privacy data map from `@sensitive` (M).
      The compiler already tracks where personal data flows
      (compiler/secrecy.fd). `fid privacy-report` writes where it goes
      (files, network, logs) as a GDPR Article 30 record; a flow to a
      destination the program does not declare fails the build. Closest:
      information-flow research (Jif); no compiler produces the report.
      Report: 1-2 weeks. Declared destinations and the build failure:
      about a month.

- [ ] EV23. Verified secret wiping (M).
      Secret functions zero their scratch registers at each return
      (LF20). Next: check on the final code that no copy of a `@secret`
      value is left in a stack slot or a spill after the function
      returns, on pe_ir and nat. The C path cannot be checked this way
      (the C compiler places the values). Closest: Jasmin (verified
      crypto) covers part of it.
      First version: 2-3 weeks. Solid: 1-2 months.

- [ ] EV24. Deterministic simulation of whole systems (M).
      The pieces exist: fakes for files, the clock and HTTP (stdlib/
      world.fd, `@hermetic`, docs/guides/testing-without-mocks.md), the
      random scheduler (`FIDES_SCHED_RANDOM`) and record/replay (`fid run
      --record`, `fid replay`). Joined: one seed drives the scheduler and
      every fake (sockets and threads included), so any concurrency or
      network bug replays exactly. Closest: FoundationDB's and
      TigerBeetle's simulators, built by hand; madsim and turmoil for
      Rust, in part.
      First version: 2-3 weeks. Solid: 1-2 months.

Picks: EV17 and EV19 fit the security niche best and are the clearest "no
other language does this"; EV18 turns four backends from a maintenance
cost into a feature; EV21 is the easiest to explain. All need EV0 first: a
sandbox or a proof made by a compiler that miscompiles loops proves
nothing.

## Totals

- Phase 1 (EV1-EV4): about a month.
- Phase 2 (EV5 step 1, EV6): about two months.
- Phase 3 (EV7): about three months.
- EV1-EV16, solid: roughly 9-15 months of serial work.
- EV17-EV24 first versions: about 4-5 months; solid: roughly 12-18 months
  more. Pick, do not queue them all.
