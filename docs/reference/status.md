# Fides status (detailed)

> Moved from the README on Nov 15, 2026; the README keeps a short summary.
> Rebuilt Nov 27 (the file was dropped by a cleanup commit and restored
> here). Counts: [selfhost.md → Numbers](selfhost.md#numbers). Open bugs:
> [bugs roadmap](../roadmap/bugs-roadmap.md), [known-bugs.md](known-bugs.md).

## Where things stand (Nov 27, 2026 — v0.8.6-unstable, `fid version` reports `1.0.0+dev`)

**Self-hosting.** The Fides compiler compiles itself. The 3-stage
fixpoint (stage 2 C == stage 3 C, byte-identical) was last verified
**Nov 21** on that day's ~361K-line tree, from a pinned self-hosted
stage 0. Everything since — the Nov 25 build, the Nov 27 build and the
v0.8.6-unstable `fid.exe` — is a **one-stage build from a self-hosted
fid, not a fixpoint**: faster to produce, but the byte-identical check
has not been re-run on the current tree. A bootstrap that starts from
the frozen Rust compiler still cannot finish: a `fidesc`-built `fid`
needs >16 GB to self-compile (known-bugs 2g.79), so `bootstrap.sh`
takes a released `fid` as stage 0 (downloaded by
`scripts/stage0-fetch.sh`, hash-checked).

**65 self-hosted modules** (361,317 lines Nov 27; 58 reached from
`main.fd`) in `compiler/*.fd` — lexer, parser, type checker, borrow
checker, escape analyzer, monomorphizer, IR lowering, IR optimizer
(SSA mid-end, rules/e-graph rewriter with translation validation),
C codegen, native PE emitter, the machine layer (MIR, encoders, one
register allocator for x86-64/AArch64/RISC-V Linux, Mach-O, `--nat`
Windows, Cortex-M Thumb-2), WASM emitter, disassembler, PDB writer,
PGO, comptime evaluator, LSP, formatter, incremental caches, and the
Ark package manager. The Rust bootstrap (`src/*.rs`, 57 files,
114,901 lines) is frozen: it builds stage 1 and runs the test harness.

**The language is what `fid` accepts.** Every rule, with the compiler
function that enforces it: [language reference](reference.md). The
frozen `fidesc` checks less: [language-guide.md § 10](language-guide.md).

**Tests.** The last full suite run is **Nov 25, `fid` as compiler,
interpreter lane only**: 1,897 of 1,983 pass; 82 of the 86 failures
fail the same with the earlier compiler, the other 4 are 300 s
timeouts with correct output. The C and native lanes of the whole
suite have never completed a run; feature work is checked with its own
tests on each backend. The last full `fidesc` (Rust) suite is Nov 2
(interp 339/339, C 343/343, PE 345/345, expected-fail 89/89) — on a
suite a third of today's size. The suite today: 2,131 bootstrap
programs (586 expected-error), 1,617 conformance programs (one per
rule of reference.md §1–§13), 47 selfhost, 11 leak-gate programs
(11/11 pass, Nov 25).

**Open bugs.** Three whole-repo reviews (Nov 22, Nov 24, Nov 27)
logged 489 entries in [bugs-roadmap.md](../roadmap/bugs-roadmap.md):
230 fixed with tests, 27 partly, **232 open** (most from the Nov 27
review, not yet triaged for severity in practice: 3 marked critical,
25 high, 46 medium, 35 low). Known-bugs.md additionally lists the open
2g.* items. The reviews read code rather than run programs; entries
marked "confirmed" were reproduced.

**CI.** `ci.yml`, `tier1.yml` and `macos-native.yml` have not had a
green run since Nov 21; the checks above were run by hand on a Windows
machine. `bare-metal.yml` last ran green Nov 22 (Multiboot kernels
booted under QEMU). The suite lanes on macos-15 runners reached the
suite Nov 20 but have not gone green, so **nothing is confirmed on a
Mac**.

## What holds today

- Borrow checking without lifetime annotations, on by default:
  exclusive `&mut`, no change while borrowed, no mutation of a
  collection a `for` loop walks; escape rules keep references
  second-class (not stored, not returned from locals, not sent to
  tasks).
- Copy-on-write value semantics for `Vec`/`HashMap`/`str` (proposal
  0004, `LOWER_RC` on): `var b = a` gives an independent value, shared
  until the first write copies it. Verified Nov 27 on `run` and
  `emit`.
- Capability grants on imports (`use m with { IO, FS }`) and `sandbox
  { }` — checked per call site in every module but `core`; the
  sandbox's check is transitive through the call graph. Capability
  grants do **not** propagate through the call graph (§15 of the
  reference): a module can reach IO through a granted dependency.
- Effect tracking (`!IO`, `!Net`, `!{}`, …) inferred across the call
  graph; under-declaring is a compile error. Resumable effect handlers
  (`handle`/`do`/`resume`).
- Bounds checks, integer-overflow checks and double-free detection
  panic in every build, on every backend — not only in debug.
- `comptime` runs ordinary Fid at compile time (no macros).
- Native executables with no assembler, linker or C compiler: Windows
  x64 PE (`fid emit`), Linux x64/AArch64/RISC-V ELF
  (`emit --target ...-linux`), WASM/WASI (`fid wasm`), Mach-O for
  macOS/iOS (built, not run), Multiboot/UEFI/bare metal. Other targets
  through C with `zig cc`.
- `comptime` generics with inference, bounds and turbofish for
  functions, enums, `impl<T>`; generic structs instantiate through
  type inference (`val p: Pair<i64> = Pair { .. }`) — the turbofish
  literal `Pair<i64> { .. }` does not parse.
- 21 safety rules ([safety-rules.md](safety-rules.md)), each with a
  must-fail test; `fid qualify` writes the evidence as JSON and always
  says `NOT_QUALIFIED` — evidence for a reviewer, not a certification.
- One binary is the compiler, interpreter, test runner, formatter,
  linter, debugger, fuzzer and language server; `ark` (packages),
  `scribe` (docs) and `veil` (coverage) beside it.

## Limits

- **Generated code is slower than Rust's.** fib(40): 1.34 s native,
  0.81 s through C, 0.53 s Rust (Nov 2 scoreboard); through C Fid was
  fastest on one of four benchmarks and up to 1.6x slower on the rest
  (Nov 15). [Benchmarks](../benchmarks.md).
- **Compilation is slow and memory-hungry.** Self-compile to C: 43.5 s
  for the 361K-line tree (Nov 27); generated C is ~1.8 MB for hello
  world; a `fidesc`-built `fid` cannot self-compile within 16 GB
  (2g.79). Two iropt passes that remove the most overhead (`soleown`,
  `vecsroa`) remain opt-in.
- **Memory leaks by design where ownership is not proven.** The
  compiler frees what it can prove dead; the rest lives until exit.
  The leak gate (11 programs) passes; long-running services should
  measure with `fid run --leaks`.
- **A panic always ends the program.** No unwinding, no per-task
  isolation, no cleanup runs.
- **Type-checker holes remain**: the bug reviews keep finding places
  where `check` accepts a program whose run then faults; treat
  compile-clean as likely-correct, not proven.
- **Data races**: globals written cross-thread are checked, and
  `spawn` arguments are deep copies; sharing raw memory through
  `unsafe` is on you. `Vec`/`HashMap`/`str` are not internally
  synchronized.
- **Platforms:** real coverage is Windows x64 (all three backends) and
  Linux-through-C; everything else is tier 2/3 — see
  [platforms](platforms.md). macOS, Android, iOS, FreeBSD, OpenBSD
  build but have never run.
- **Ecosystem:** the pacts index is just starting (`ark add textkit`
  is the one published pact); few libraries exist beyond the stdlib.
- **Not for safety-critical use.** `fid qualify` produces evidence
  for review, not a certification; no independent security review has
  signed off (the package is ready:
  [review-index.md](../security/review-index.md), J9 open).

## Numbers

[selfhost.md → Numbers](selfhost.md#numbers) is the one place counts
and measurements are kept.
