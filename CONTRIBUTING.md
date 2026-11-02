# Contributing to Fides

Thank you for considering a contribution. This is the short version of what
you need to know; `AGENTS.md` holds the detail, including the list of files
where a mistake can break every program silently.

## The two compilers

Fides is self-hosted. The primary compiler is written in Fides
(`compiler/*.fd`, 54 modules) and is the release compiler (the installer
ships it as `fid.exe`). A Rust implementation (`src/*.rs`) remains as the
bootstrap reference: it builds stage 1 of the self-hosted compiler and runs
the test harness. Both give the same output on the test programs.

| You want to…                                      | Use                                                     |
| ------------------------------------------------- | ------------------------------------------------------- |
| Write Fides programs, add tests, add stdlib modules | either compiler; check with `fidesc test`             |
| Build tools (`ark`, `scribe`, `veil`)             | `cargo build --release`                                 |
| Change the language or compiler internals         | `compiler/*.fd`; the Rust compiler only when the bootstrap needs it (`docs/reference/stability.md`) |
| Check a `compiler/*.fd` change                    | `RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh` (fixpoint) |

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
5. Large changes go behind a flag so both paths can be tested before the old
   one is removed.

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
