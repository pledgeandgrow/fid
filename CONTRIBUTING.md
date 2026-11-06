# Contributing to Fides

Thank you for considering a contribution. This is the short version of what
you need to know; `AGENTS.md` holds the detail, including the list of files
where a mistake can break every program silently.

## The two compilers

Fides is self-hosted. The compiler is written in Fides (`compiler/*.fd`,
61 modules) and is the release compiler (the installer ships it as
`fid.exe`). It is built from a pinned earlier build of itself
(`release/stage0/fid.exe`, hash in `scripts/stage0.sha256`), as Go and Rust
are. The Rust implementation (`src/*.rs`) is frozen (roadmap ST1): it runs
the test harness and starts a cold build when no stage 0 is pinned. New
features go into `compiler/*.fd` only.

| You want to…                                      | Use                                                     |
| ------------------------------------------------- | ------------------------------------------------------- |
| Write Fides programs, add tests, add stdlib modules | `fid`; check with `fidesc test --compiler release/fid.exe` |
| Build tools (`ark`, `scribe`, `veil`)             | `cargo build --release`                                 |
| Change the language or compiler internals         | `compiler/*.fd` (`docs/reference/stability.md`)         |
| Check a `compiler/*.fd` change                    | `NATIVE=1 ./scripts/bootstrap.sh` (fixpoint, no C compiler) or `RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh` |
| Make the checked compiler the next stage 0        | `NATIVE=1 PIN=1 ./scripts/bootstrap.sh`                 |

A change to the language reaches the compiler's own source in two steps: the
compiler learns it first, and its own source may use it only after the next
pin, since the pinned stage 0 must still compile that source.

## Setting up

```bash
git clone https://github.com/pledgeandgrow/p-lang
cd p-lang
cargo build --release          # builds fidesc, ark, scribe, veil
./target/release/fidesc test   # runs every test program on all backends
```

`./verify.sh` runs the build and the tests and diffs the results against the
checked-in baseline (`tests/bootstrap/baseline/test_results.txt`), so you can
see exactly which tests a change affects. Run it before you start and after
every change. `./verify-selfhost.sh` does the same for the self-hosted
components in `tests/selfhost/`.

## Good first contributions

- **A test program.** Add `tests/bootstrap/test_<feature>.fd`, then run
  `./verify.sh --save` to update the baseline. Every test runs on the
  interpreter, the C backend, and the native emitter. If the test exercises a
  feature that is not implemented yet, add it to `is_skip_test()` in
  `tests/harness.rs`.
- **A standard library module or function.** `stdlib/*.fd` is plain Fides;
  mistakes surface as test failures, not silent miscompiles. Add a `///` doc
  comment and `scribe` documents it.
- **Docs and error messages.** The reference docs live in `docs/`; the website
  mirrors them.
- **A bug report with a minimal reproducer.** Often more valuable than a
  patch: the smallest `.fd` file that shows the problem, the command you ran,
  and the output of `fidesc version`.

## Compiler changes

Some files can break every program silently when edited: the register
allocator, the peephole optimiser, the object-file emitters (ELF64, PE, COFF,
Mach-O), the code generators, and the hand-encoded runtimes. `AGENTS.md`
lists them with the risk level of each. For those:

1. Run `./verify.sh` first to confirm a clean baseline.
2. Make the smallest possible change — one change, one commit, one
   verification run. Do not refactor while fixing.
3. If a test regresses, bisect stages manually with the dump commands
   (`fidesc dump-ir|dump-pir|dump-c`, `fidesc codegen`) — a bisect script
   (`scripts/bisect.sh`) is planned but not yet written.
4. Large changes go behind a flag so both paths can be tested before the old
   one is removed.
5. **Every bug fixed gets a regression test, always** (roadmap VF20): the
   smallest program that showed the bug, as `tests/bootstrap/test_<bug>.fd`
   (or `test_<bug>_fail.fd` for a program that must be rejected), in the same
   change as the fix. A fix without its test is not finished.

Before a change to code generation or the optimizer, run the random-program
differential tester (roadmap VF1), which compares every backend on generated
programs: `python scripts/fuzz/gen_programs.py -n 200 --fid release/fid.exe`.

Never run `git stash`, `git checkout -- <file>`, `git reset --hard`, or
`git clean` in a shared checkout, never rewrite history, and never delete
files you did not create. These rules apply to people and to agents alike.

## Language changes and releases

A change to the language, to what a program means, or to a public stdlib API
starts as a proposal: copy `docs/proposals/0000-template.md`. The process, the
stability promise, editions, the release cadence and the rule for release notes
(every change that can affect a working program is listed under **Breaking**)
are in `docs/reference/stability.md`. Supported platforms and their tiers are in
`docs/reference/platforms.md`.

## Pull requests

- One topic per PR, with the test that proves it.
- Say which backends you ran. "Interpreter only" is a fine answer; it tells
  the reviewer what to check.
- Keep the docs truthful: if a change moves a feature from partial to working
  (or the reverse), update `docs/reference/features.md` in the same PR. The
  website's Feature Status page mirrors that file.
- Changelog entries go in `docs/reference/changelog.md`; they are never
  rewritten to look better in hindsight.

## Conduct and licensing

All project spaces run under the [Code of Conduct](CODE_OF_CONDUCT.md).
Security-sensitive findings go through the [Security Policy](SECURITY.md),
not the public tracker. By contributing you agree that your contribution is
licensed under the [MIT License](LICENSE).
