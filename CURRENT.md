# Current Focus

> Single source of truth for "what are we doing right now."
> Read this first every session. Updated after every milestone change.
> Last updated: Oct 27, 2026 — post-Session 34 (self-hosted parity).

## Current direction: the self-hosted compiler is the release compiler

The self-hosted compiler (`compiler/*.fd`, **54 modules, ~105K LOC**) is the
primary artifact and the compiler the installer ships (`fid.exe`). The Rust
compiler (`src/*.rs`, **55 files, ~101K LOC**) is the bootstrap reference: it
builds stage 1 of the self-hosted compiler and holds the test harness
(`fidesc test`); it ships beside it as `fidesc.exe`.

**v1.0.0 was released Sep 9, 2026** with a Windows installer
(`Fides-Setup-1.0.0.exe`).

### Honest status — what is actually true today

- **Bootstrap fixpoint:** VERIFIED byte-for-byte (stage 2 == stage 3, ~38 MB
  of C). `RELEASE=1 ./scripts/bootstrap.sh` checks it on every release
  build, then installs `release/fid.exe` and runs a program from an
  installed layout.
- **Self-hosted `run` and `ccompile` match `fidesc`:** `run` on all 302 test
  programs and all 10 examples; `ccompile` on 242 of 243 test programs
  (`test_sqlite` needs libsqlite3) and 9 of 10 examples (`minimal_game` needs
  SDL2); all 59 expected-fail tests are rejected.
- **Rust compiler suite:** interpreter 280/280, C 278/278, native Windows (PE)
  282/282, expected-fail 59/59 (`fidesc test`, `fidesc test --fail`).
- **Self-hosted native backend not working:** `build --native` (PIR →
  x86-64 → `compiler/pe.fd`) crashes in the optimizer and register
  allocator, so the self-hosted `build`/`ccompile` go through C and need a C
  compiler. `fidesc emit` makes native Windows executables with none.
- **Nondeterministic self-hosted typecheck:** reported earlier (HashMap-order
  instability); not reproduced in the Oct 25–27 bootstraps.
- **`verify.sh --golden` and `scripts/bisect.sh` do not exist** —
  `verify.sh --golden` will fail; bisect manually with `dump-*` commands.

### Recent sessions

| Session | What happened |
|---------|---------------|
| 34 | Self-hosted compiler: @conserve check completed, second `impl` blocks in imported modules kept, `extern "C" link` libraries linked. Tests that printed timings or random values, or had no `main`, made deterministic; `examples/riscv_blink` fixed. Self-hosted now matches on 302/302 (`run`) and 242/243 (`ccompile`). |
| 33 | Self-hosted interpreter and front end ported to parity with the Rust compiler (79 → 241 `run`, 164 → 239 `ccompile`); stdlib found from an installed location; release bootstrap with fixpoint check; installer ships the self-hosted compiler. |
| 32 | Native Windows executables run every test: a Fides runtime for the PE backend. |
| 31 | Fast, reproducible and incremental builds; short-circuit, HMAC and PE miscompile fixes. |
| 30 | Post-codegen constant-time verifier on x86-64 ELF (asm-level `jcc`/`cmov`/indexed-mem/call checks). `cast_arg` `Ptr(inner)→Generic` deref in both C codegens (heals `&mut Vec`→`Vec` forwards — the `record_def`/`ra_vec_find_i64` crash). `cmd /C` quote-strip fix so `cl.exe` links on Windows. |
| 29 | Reference escape check (`src/escape.rs`, `compiler/escape.fd`): references are second-class. `Vec<f64>` reads + `f64` returns fixed. Fixpoint re-verified. |
| 28 | Branch consolidation — `release-infra` merged into `main`; 15 `Vec` params made by-value; 92 `&mut` call sites removed; `str_equal` → `==`. |
| 27 | Soundness pass — typechecker rejects previously-accepted wrong programs. |
| 26 | Bootstrap fixpoint VERIFIED byte-for-byte (c4==c5); 7 Rust→Fides gap fixes; self-compilation SIGSEGV fixed. |

## Current Baseline

```
compiler/*.fd:  54 modules, ~105K LOC — THE COMPILER (primary source)
src/*.rs:       55 files, ~101K LOC — bootstrap reference + test harness
stdlib/*.fd:    104 modules, 30,446 LOC (kernel/uefi moved to fides-os)
tests:          302 bootstrap + 46 selfhost .fd files
Binaries:       release/fid.exe (self-hosted, shipped as fid.exe)
                target/release/fidesc.exe (Rust, shipped as fidesc.exe)
```

## What's NOT yet done

1. **Self-hosted native backend** — `build --native` crashes (optimizer
   `pass_dse`, then register allocation). Plan: port the working Rust PE
   backend (`src/pe.rs`, `src/pe_runtime/`) to `compiler/pe.fd`, checked
   byte-for-byte against `fidesc emit`; then a bootstrap with no C compiler.
2. **Self-hosted typecheck nondeterminism** — reported earlier; not seen in
   the Oct 25–27 bootstraps. Retry before assuming a regression.
3. **Post-codegen CT verification is x86-64 ELF only** — PE (`pe.rs` emits
   bytes directly, no asm text), ARM64, and RISC-V paths are not covered.
4. **`&mut Vec` and `Vec` param promotion is inconsistent** — mutated params
   may promote to `Ptr(Vec)` while callers were lowered against the
   `Generic` signature; `cast_arg` now papers over the common case, but the
   promotion itself is not yet stable.
5. ~~**`fidesc test` C-codegen on Windows**~~ — confirmed: 278/278 (Oct 27).
6. ~~**Docs consolidation leftovers**~~ — FIXED (session 30 docs audit):
   INDEX.md, selfhost.md, and all cross-links now point at the merged
   `docs/roadmap/roadmap.md` + `docs/selfhost/selfhost.md`.
7. **stdlib docs lag** — 30 of the 104 stdlib modules have no
   `docs/stdlib/<name>.md` page (mostly the game/media batch from
   session 21).
8. ~~**`release/fid-dev.c` instrumentation**~~ — regenerated by
   `./scripts/build-selfhost.sh` (clean `dump-c`, no debug prints).

## What IS done (Sessions 17–30)

- Sessions 17–24: v1.0.0 release, installer, industry stdlib, language
  features (@zeroize, @secret, @interrupt, @firmware, `parallel {}`, @simd).
- Sessions 26–29: fixpoint verified (c4==c5), soundness pass, escape check,
  Vec<f64>/f64-return fixes, branch consolidation.
- Session 30: post-codegen constant-time verifier (x86-64 ELF), `cast_arg`
  ABI repair, `cl.exe` quoting fix, alloca-hoist fix for the `--native` path.
- Sessions 31–34: reproducible/incremental builds, a Fides runtime for the
  PE backend (all tests run natively), self-hosted `run`/`ccompile` parity,
  release bootstrap, installer ships the self-hosted compiler.

## Self-Host Module Status

All **54** `compiler/*.fd` modules type-check. The full list and per-file
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
./verify.sh            # tests/bootstrap/ (Rust compiler)
./verify-selfhost.sh   # tests/selfhost/ (Fides components, 3 backends)
```
