# Current Focus

> Single source of truth for "what are we doing right now."
> Read this first every session. Updated after every milestone change.
> Last updated: Nov 6, 2026 — release-roadmap batches H, K, L, M and C/D
> landed in `fid` (uncommitted on `main`); numbers re-counted.
>
> **The "Numbers" section below is the one place project numbers are kept.**
> Other docs round them and link here; change them here first.

## Current direction: the self-hosted compiler is the release compiler

The self-hosted compiler (`compiler/*.fd`, **63 modules, ~142K lines**) is the
primary artifact and the compiler the installer ships (`fid.exe`). The Rust
compiler (`src/*.rs`, **57 files, ~113K lines**) is frozen: it builds stage 1
of the self-hosted compiler and holds the test harness (`fidesc test
--compiler <fid>`); it ships beside it as `fidesc.exe`. **The language is
what `fid` accepts**; where the two differ, see
[language-guide.md § 10](docs/reference/language-guide.md). The rules, with
the code that enforces each: [reference.md](docs/reference/reference.md).

**v1.0.0 was released Sep 9, 2026** with a Windows installer
(`Fides-Setup-1.0.0.exe`). `fid version` still prints `0.1.0` (Cargo.toml).

### Honest status — what is actually true today

- **Bootstrap fixpoint:** VERIFIED byte-for-byte on Nov 6 (stage 2 == stage
  3, 49 MB of C). `RELEASE=1 ./scripts/bootstrap.sh` checks it on every
  release build. `PIN=1` can pin a stage 0, but none is pinned yet: stage 1
  is still built by `fidesc`.
- **Tests:** the last full suite run is from Nov 2 (interpreter 339/339, C
  343/343, native Windows 345/345, expected-fail 89/89), **with `fidesc` as
  the compiler under test**: the shipped `fid` has never run the full suite
  (its native output crashed on every task program until Nov 6). The Nov 4–6 compiler
  was checked on the new and the key tests on all three backends, and every
  new expected-fail test is rejected; the full suite has not been rerun.
- **Backends:** interpreter (`fid run`), C (`fid ccompile`, needs a C
  compiler), native Windows x64 (`fid emit`, no C compiler). Native `emit`
  works again (Nov 6): core's `p_str_free` took plain strings for
  length-headered ones and freed the wrong address. The old `build
  --native` path (PIR, compiler/pe.fd) still crashes and is unused.
- **New in `fid` (Nov 4–6):** generics with inference, bounds and turbofish
  for functions, enums and `impl<T>` ([0002](docs/proposals/0002-real-generics.md);
  a literal of a user-defined generic struct is still rejected); `if`/`match` values;
  `@cfg`; freeing of provably finished locals (Vec, HashMap, owned elements,
  regions, string temporaries); iterator invalidation and field-borrow
  checks; `@send` on every spawned argument; `parallel {}` branches on
  threads; `async fn` with f64 and struct arguments and results; every
  safety annotation enforced, 17 rules with negative tests
  ([safety-rules.md](docs/reference/safety-rules.md)) and `fid qualify`
  evidence; owned and borrowed strings (`&str`, `@scoped`, `str_clone`;
  `http_serve_scoped` is no longer `@unsafe`); integer-key `HashMap`s;
  per-call-site effects of higher-order functions.
- **Value semantics ([0001](docs/proposals/0001-value-semantics.md)):**
  decided (copy-on-write for `Vec`, `str`, `HashMap`; `==` by content, done).
  Copy-on-write is written for `Vec`, `HashMap` and the `Vec` fields of
  copied structs, behind `LOWER_COW` (off): shares are lazy (elements are
  copied at the first write), counted (freed with their last owner) and left
  out for copies that are never written or read for the last time. On,
  test programs get value semantics on every backend and `fid` compiling
  itself reaches its fixpoint (Nov 6), with a peak of 4.7 GB against 1.8 GB
  off. As of the afternoon of Nov 6 `LOWER_COW` is `true` in
  `compiler/lower.fd` (AGENTS.md #25); the full suite has not run with it,
  and the Nov 2 `release/fid.exe` still aliases on `var b = a` for a `Vec`.
- **Native async works (Nov 6):** `fid emit` programs that use `async`/`spawn`
  + `await` crashed on every worker thread: `pe_fn_lookup` left out runtime
  functions, so the scheduler started its threads at address 0
  (compiler/pe_ir.fd, uncommitted). The task tests give the interpreter's
  output, except `test_async_suspend`: both interpreters run an `async fn`
  to the end at the call, where native and the test's own header have it
  run until it first waits. `fid ccompile` of that test fails
  (`p_co_pool_call` is missing from the C runtime).
- **Claims checked by running (Nov 6):** `@crashfree`, `@gas`, `!{}`, the
  borrow checker, the unlocked-global race check and `@constant_time`
  reject what they should. Weaker than documented: `sandbox {}` misses IO in
  called functions and `p_file_read`; `region ... with budget(N)` counts
  declared locals, not heap growth; `@conserve` only forbids `Money_new`
  (`Money_from_cents` mints); `@crashfree`/`@gas` do not bound recursion;
  `txn {}` is a plain block on C and native; typed-string constructors take
  any string. Not working in `fid`: `build-x86`
  (out of memory), `fid wasm` (invalid module), stack traces in native
  programs. Details: [positioning.md](docs/reference/positioning.md) (top).
- **Speed:** fib(40) 1.34 s native, 0.81 s through C, 0.53 s Rust (Nov 2).
  `fid` turns its own source into C in about 133 s (type check 59 s);
  `fidesc` in 21 s. The native path now runs a small IR optimizer
  (`compiler/iropt.fd`); there is no full register allocator.
- **Written, not run yet:** `fid emit --checked-alloc` (blocks on guard
  pages), `scripts/sanitize.sh` (ASan/UBSan, needs clang on Linux/macOS),
  `scripts/fuzz/nightly.sh` (no CI workflow in the repo schedules it).
- **`verify.sh --golden` and `scripts/bisect.sh` do not exist** — bisect
  with the `dump-*` commands.

### Recent sessions

| Session | What happened |
|---------|---------------|
| Nov 4–6 | Release roadmap: C5–C8 freeing, D5–D9 speed and tooling, H4/H7/H8/H9, K4–K11, L1–L4, M7–M9 scripts, M11; integer-key HashMap; higher-order effects; native `emit` fixed; copy-on-write written (off); language reference and safety-rule traceability written. Fixpoint verified. |
| Nov 2–3 | Proposal 0002 (generics) in `fid`; `==` by content; `&mut` migration of 403 parameters; Linux CI green; self-compile 4.4× faster. |
| 34 | Self-hosted compiler: @conserve check completed, second `impl` blocks in imported modules kept, `extern "C" link` libraries linked. Self-hosted matched `fidesc` on 302/302 (`run`) and 242/243 (`ccompile`). |
| 33 | Self-hosted interpreter and front end ported to parity with the Rust compiler; stdlib found from an installed location; release bootstrap with fixpoint check; installer ships the self-hosted compiler. |
| 32 | Native Windows executables run every test: a Fides runtime for the PE backend. |

## Numbers

Counted Nov 6, 2026 (`wc -l`, so blank lines and comments included).
Measured figures carry their own date.

```
compiler/*.fd:   63 modules, 142,378 lines — THE COMPILER (primary source)
src/*.rs:        57 files, 112,511 lines — bootstrap reference + test harness
stdlib/*.fd:     137 modules, 43,576 lines (+ 4 in stdlib/pe_runtime/)
tests:           493 .fd under tests/bootstrap/ (452 top level, 91 of them
                 expected-fail), 51 in tests/selfhost/, 10 examples
Suite (Nov 2):   interp 339/339, C 343/343, PE 345/345, expected-fail 89/89
                 (not rerun with the Nov 4–6 compiler)
Fixpoint:        stage 2 == stage 3, 49 MB of C (Nov 6)
Self-compile:    fid ~133 s to C (Nov 6), fidesc 21 s
Platforms:       Tier 1 Windows x64 (interp, C, native) and Linux x64 (interp,
                 C); through C cross builds: Linux ARM64 270/286 under QEMU
                 (Nov 7), RISC-V/macOS/WASI tier 2; native Linux AArch64 and
                 RISC-V 64 (`fid emit --target`, Nov 7): 225/231 building
                 tests under QEMU — docs/reference/platforms.md
Binaries:        release/fid.exe (self-hosted, shipped as fid.exe)
                 target/release/fidesc.exe (Rust, shipped as fidesc.exe)
```

## What's NOT yet done

1. **Ownership and layout (proposal 0004, written Nov 7, not yet built):**
   one compiler-inserted ownership rule (`LOWER_RC`) replaces copy-on-write
   shares and the auto-drop passes; counted strings everywhere; null-free
   builtins (`read_file`, `env_var`, `arg_at`, ... give `Option`). Next:
   the 3-stage fixpoint, the suite, and `scripts/leak-check.sh`.
2. **Async in the interpreters** runs an `async fn` to the end at the call
   (`test_async_suspend`); the C runtime has no `p_co_pool_call` (H10).
   Native async was fixed on Nov 6. Also open: **checks weaker than their
   names** (Nov 6): `sandbox {}` through called functions, `region` budgets
   for heap growth, `@conserve` mints other than `Money_new`, recursion
   under `@crashfree`/`@gas`, `txn {}` rollback on C and native.
3. **A full native register allocator** (D4): today 7 callee-saved
   registers hold the hottest locals, parameters and short-lived values.
4. **`Vec` elements at their real size and references into containers**
   (D14, D15).
5. **Other platforms** (Track F). Through C, since Nov 7: `fid ccompile
   --target aarch64-linux|riscv64-linux|x86_64-linux|x86_64-macos|aarch64-macos|wasm32-wasi`
   cross-compiles with `zig cc` from any host, and `scripts/cross-test.sh
   <target>` runs the suite there (qemu-user, Node WASI). AArch64 Linux
   matched `fid run` on 270 of 286 tests under QEMU; RISC-V, x86-64 Linux
   and WASI ran sample programs, their suites were stopped unfinished; macOS
   builds but is not run (CI job in `scripts/ci/cross.yml`, inactive). Left:
   those full runs; on the native Linux AArch64/RISC-V backend (Nov 7:
   `fid emit --target aarch64-linux|riscv64-linux`, compiler/nat_ir.fd +
   nat_enc.fd + stdlib/linux_runtime, 225 of 231 building tests match under
   QEMU) the builtins it lacks (threads, tasks, channels, atomics, C math,
   tensors, sockets) and fault messages; native Mach-O executables (F7), a
   valid native `fid wasm`, native ELF
   with the stdlib (F4). **Bare metal**: `emit --bare` works since Nov 6,
   not yet under QEMU (G1). **Crypto** J5–J9, `parse-header` on SDL2/SQLite
   (E5).
6. **`// SAFETY:` comments** on the `unsafe` blocks of the stdlib (M10).
7. **Post-codegen CT verification is x86-64 ELF only** (J7).
8. **The full suite** has to be rerun with the Nov 4–6 compiler and the
   baseline saved.

## Self-Host Module Status

All **61** `compiler/*.fd` modules type-check (the compiler builds itself). The full list and per-file
findings are tracked in `file-analysis/PROGRESS.md` — that file is the
authoritative per-module ledger (newest: `runtime.fd`, `escape.fd`).

---

## Build Commands

```bash
# Build bootstrap compiler (Rust) — builds stage 1, runs the test harness
cargo build --release

# Release build of the self-hosted compiler: 3 stages, fixpoint checked,
# installed as release/fid.exe (what the installer ships as fid.exe)
RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh

# Single stage for development (same output name, no fixpoint check)
# → release/fid-dev.c + release/fid-dev(.exe)
./scripts/build-selfhost.sh

# The self-hosted compiler
.\release\fid.exe run      tests/bootstrap/hello.fd
.\release\fid.exe ccompile tests/bootstrap/hello.fd -o hello.exe   # needs a C compiler

# The Rust compiler (same output; emit needs no C compiler)
.\target\release\fidesc.exe run      tests/bootstrap/hello.fd
.\target\release\fidesc.exe ccompile tests/bootstrap/hello.fd -o hello.exe
.\target\release\fidesc.exe build    tests/bootstrap/hello.fd -o hello --target x86-64

# Regenerate the self-hosted C (needed after compiler/*.fd changes)
.\target\release\fidesc.exe ccompile compiler\main.fd -o ...  # then link with cl/zig

# Full verification
./target/release/fidesc.exe test --compiler release/fid.exe   # the suite on fid
./verify.sh            # tests/bootstrap/ (Rust compiler)
./verify-selfhost.sh   # tests/selfhost/ (Fides components, 3 backends)

# Evidence, sanitizers, fuzzing
.\release\fid.exe qualify file.fd -o evidence.json
./scripts/sanitize.sh          # needs clang with ASan (Linux/macOS)
./scripts/fuzz/nightly.sh
```
