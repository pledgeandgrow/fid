# Contributing to Fides

Thanks for your interest. Fides is a self-hosted systems language: the
compiler `fid` is written in Fides (`compiler/*.fd`) and compiles itself.
This page is how to build it, test a change, and send it.

- Questions and ideas: [Discussions](https://github.com/pledgeandgrow/fid/discussions)
- Bugs: [issues](https://github.com/pledgeandgrow/fid/issues/new/choose)
- Security problems: privately, see [SECURITY.md](SECURITY.md)
- Behaviour: [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md)

## Where things are

| Path | What |
|------|------|
| `compiler/*.fd` | **The compiler.** Every language or compiler change goes here. Module map: [INDEX.md](INDEX.md), pipeline: [architecture.md § Self-hosted pipeline](docs/reference/architecture.md) |
| `stdlib/*.fd` | Standard library; `stdlib/core.fd` is imported into every program |
| `stdlib/pe_runtime/`, `stdlib/linux_runtime/` | Runtimes of the native Windows and Linux back ends |
| `src/*.rs` | The Rust bootstrap compiler `fidesc`. **Frozen**: it only builds the first stage and runs the test harness. Don't add features here |
| `tests/bootstrap/` | Test programs (`test_<name>.fd`; `*_fail.fd` must be rejected) |
| `docs/reference/reference.md` | The language, rule by rule: what `fid` accepts |
| `docs/file-analysis/<module>.md` | One page per compiler module: role, callers, known problems |

## Build

You need Rust (stable), a C compiler (`zig cc` 0.16 is the tested one;
gcc, clang or tcc also work) and Git Bash on Windows.

```bash
cargo build --release                      # target/release/fidesc
CC="zig cc" ./scripts/build-selfhost.sh    # one stage: release/fid-dev(.exe)
RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh   # 3 stages + fixpoint: release/fid(.exe)
```

A one-stage build is enough while you work. The 3-stage bootstrap checks
that the compiler reproduces itself byte for byte; run it before a change
that touches lowering, the C generator or the core library lands. Compiler
builds use a lot of CPU and memory: run one at a time.

## Test

```bash
./release/fid-dev run      tests/bootstrap/test_x.fd   # interpreter
./release/fid-dev ccompile tests/bootstrap/test_x.fd -o x && ./x   # through C
./release/fid-dev emit     tests/bootstrap/test_x.fd -o x.exe      # native (Windows)
bash scripts/leak-check.sh release/fid-dev              # ownership: every block freed
./target/release/fidesc test --compiler release/fid-dev # the whole suite (slow)
```

A change is checked when the programs it affects give the same output on
every back end it touches (`run`, `ccompile`, `emit`). Add a test for every
fix and feature: `tests/bootstrap/test_<feature>.fd`, or `_fail.fd` for a
program the compiler must reject (its first line is `// expect-error: <message>`).

## Rules for the working tree

These protect work in progress, yours and other people's:

- Never `git stash`, `git reset --hard`, `git checkout -- <file>` or
  `git clean` on a tree with uncommitted changes. Compare with `git diff`
  or `git show HEAD:<path>`.
- Never force-push or rewrite `main`'s history (it is protected).
- Don't commit build output (`release/`, `target/`, `build/`, generated
  `.c`, `.exe`); `.gitignore` covers them.

## Style

- Plain, short comments that say why. Gate a risky new behaviour behind a
  `const NAME: bool` switch or an environment variable so it can be turned
  off and compared.
- Keep a change small and focused: one fix, one test.
- Language changes start as a proposal: copy
  `docs/proposals/0000-template.md`.

## Sending a change

Open a pull request against `main`. The PR template lists the checks. CI
builds the Rust compiler on Linux and Windows and builds `fid`, runs a few
programs and the leak gate. Say in the PR what you ran locally.

By contributing you agree that your work is licensed under the MIT license
of the project ([LICENSE](LICENSE)).
