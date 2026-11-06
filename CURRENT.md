# Current Focus

> Single source of truth for "what are we doing right now."
> Read this first every session. Updated after every milestone change.
> Last updated: Nov 6, 2026 — release-roadmap batches H, K, L, M and C/D
> landed in `fid` (uncommitted on `main`); numbers re-counted.
>
> **The "Numbers" section below is the one place project numbers are kept.**
> Other docs round them and link here; change them here first.

## Current direction: the self-hosted compiler is the release compiler

The self-hosted compiler (`compiler/*.fd`, **61 modules, ~134K lines**) is the
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
  343/343, native Windows 345/345, expected-fail 89/89). The Nov 4–6 compiler
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
  off. It stays off until the full suite has run with it on, so today
  `var b = a` on a `Vec` still aliases.
- **Native async is broken:** `fid emit` programs that use `async`/`spawn` +
  `await` crash on the worker threads (interpreter and C are fine).
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
compiler/*.fd:   61 modules, 133,688 lines — THE COMPILER (primary source)
src/*.rs:        57 files, 112,511 lines — bootstrap reference + test harness
stdlib/*.fd:     137 modules, 43,576 lines (+ 4 in stdlib/pe_runtime/)
tests:           493 .fd under tests/bootstrap/ (452 top level, 91 of them
                 expected-fail), 51 in tests/selfhost/, 10 examples
Suite (Nov 2):   interp 339/339, C 343/343, PE 345/345, expected-fail 89/89
                 (not rerun with the Nov 4–6 compiler)
Fixpoint:        stage 2 == stage 3, 49 MB of C (Nov 6)
Self-compile:    fid ~133 s to C (Nov 6), fidesc 21 s
Platforms:       Tier 1 Windows x64 (interp, C, native) and Linux x64 (interp,
                 C); everything else tier 2 or 3 — docs/reference/platforms.md
Binaries:        release/fid.exe (self-hosted, shipped as fid.exe)
                 target/release/fidesc.exe (Rust, shipped as fidesc.exe)
```

## What's NOT yet done

1. **Copy-on-write on** (M5): run the full suite with `LOWER_COW = true`,
   then switch it on. Its memory cost (4.7 GB vs 1.8 GB for the
   self-compile) comes mostly from shares held by struct copies that are
   never freed; freeing them at scope end needs a check that no alias read
   out of the struct outlives it.
2. **Native async/tasks** crash in `fid emit` programs (H10).
3. **A full native register allocator** (D4): today 7 callee-saved
   registers hold the hottest locals, parameters and short-lived values.
4. **`Vec` elements at their real size and references into containers**
   (D14, D15).
5. **Other platforms** (Track F: macOS, native ELF, ARM64/RISC-V/WASM),
   **bare metal** (Track G: `emit --bare` panics), **crypto** J5–J9,
   `parse-header` on SDL2/SQLite (E5).
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
