# Current Focus

> Single source of truth for "what are we doing right now."
> Read this first every session. Updated after every milestone change.
> Last updated: Session 24 — **v1.0.0 released, Windows installer shipped.**

## Current direction: Fides is independent and released

The self-hosted compiler (`compiler/*.fd`, 50 modules, 93,605 LOC) is the
primary development path. The Rust compiler (`src/*.rs`, 55 files, ~93,269 LOC)
is the bootstrap reference, used only when `compiler/*.fd` changes.

**v1.0.0 was released Sep 9, 2026** with a Windows installer
(`Fides-Setup-1.0.0.exe`, 2.5 MB). GitHub Actions CI builds the installer
automatically on release tags.

### Honest self-hosting status

- **Bootstrap:** 3 generations verified (Rust → c1 → c2 → c3, all work)
- **Self-host tests:** 38/38 pass on all 3 backends (interp, C, PE)
- **Self-compilation:** **FAILS** — SIGSEGV in `Lowerer_lower_stmt` during
  lowering of the full 75K LOC compiler. This is a pre-existing known issue.
- **Fixpoint:** **NOT VERIFIABLE** — `release/fides-c2.c` does not exist.
  The fixpoint was achieved in Session 15 (701,100 lines, 0 diffs) but is
  no longer reproducible because the artifact was lost and subsequent
  sessions (17-24) added significant code that has not been re-verified.

### What this means

- **Everyday work uses `release/fides-c3.exe`** — writing programs, adding
  stdlib modules, adding tests, building tools (ark, scribe, veil).
- **The Rust compiler (`src/*.rs`) is the bootstrap reference only** — used
  when `compiler/*.fd` changes and the bootstrap chain must be regenerated.
- **New features go into `compiler/*.fd` first** when they don't require
  new language constructs. When they do, add to Rust first, then port.

### When you DO need the Rust compiler

| Task | Use Rust compiler? |
|------|-------------------|
| Writing Fides programs | No — use `fides-c3.exe` |
| Adding stdlib modules | No — use `fides-c3.exe` |
| Adding tests | No — use `fides-c3.exe` |
| Building tools (ark, etc.) | No — use `fides-c3.exe` |
| Changing `compiler/*.fd` | Yes — regenerate C1, verify fixpoint |
| Adding new language features | Yes — add to Rust first, then port to Fides |

## Current Baseline (v1.0.0)

```
SELF-HOSTED COMPILER (primary):
  compiler/*.fd:  50 modules, 93,605 LOC
  Fixpoint:       NOT VERIFIABLE (fides-c2.c missing)
  Self-compilation: FAILS (SIGSEGV in Lowerer_lower_stmt)
  Bootstrap:      3 generations verified (Rust → c1 → c2 → c3)
  Use:            release/fides-c3.exe for everyday work

RUST BOOTSTRAP (reference only):
  src/*.rs:       55 files, ~93,269 LOC
  Used when:      compiler/*.fd changes (regenerate C1, verify fixpoint)

WINDOWS INSTALLER (v1.0.0):
  installer/:     NSIS script + build helper
  CI:             .github/workflows/build-installer.yml
  Output:         Fides-Setup-1.0.0.exe (2.5 MB, tested on Windows)

TESTS:
  Self-host:      38/38 pass on interp + C + PE (tests/selfhost/)
  Bootstrap:      Interpreter 236/237, C Codegen 226/237, PE 237/237
  Total .fd tests: 303 (261 bootstrap + 42 selfhost)

STDLIB:
  stdlib/*.fd:    99 modules, 28,370 LOC

RECENT SESSIONS:
  Session 23:     @zeroize, @secret, @interrupt, @firmware, parallel{}, @simd
  Session 21:     26 game/media stdlib modules (9,140 LOC)
  Session 20:     12 Rust modules ported + 7 cross-comp backends (14,577 LOC)
```

## Critical blocker: Self-compilation SIGSEGV

The compiler cannot currently compile itself. The crash occurs in
`Lowerer_lower_stmt` (also referred to as `Lowerer_mangle_method` in
AGENTS.md) during lowering of the full 75K LOC compiler source.

**To fix:**
1. Debug the SIGSEGV in the lowerer when processing the full compiler
2. Regenerate `fides-c2.c` and verify the fixpoint
3. Run the full test suite through the self-hosted compiler

## What's NOT yet done

1. **Self-compilation fix** — the SIGSEGV must be resolved
2. **Fixpoint re-verification** — regenerate fides-c2.c, verify c2==c3
3. **DWARF debug info** — no DWARF generation exists (Phase 5 remaining item)
4. **CFI (Control Flow Integrity)** — not started (Phase 12)
5. **Green threads** — not started (Phase 13)
6. **Package registry** — ark works for path/git deps, no registry yet
7. **@shader annotation** — stdlib/shader.fd exists but compiler has no @shader parsing
8. **State machine transform** — parser has parse_state_machine(), lowerer doesn't use it

## What IS done (Sessions 17-24)

- Session 20: 12 Rust modules ported + 7 cross-comp backends (14,577 LOC)
- Session 21: 26 game/media stdlib modules (9,140 LOC, 99 total)
- Session 22: Self-host audit (fixpoint found NOT verifiable)
- Session 23: 6 language features (@zeroize, @secret, @interrupt, @firmware, parallel{}, @simd)
- Session 24: 16 roadmap bugs fixed, test baseline confirmed
- v1.0.0 release: Windows installer, GitHub Actions CI
- Industry stdlib: medical.fd, clinical.fd, can.fd, ecu.fd, arinc.fd, scada.fd, ota.fd
- @send/@sync: typechecker has is_send_type()/is_sync_type() with #[send]/#[sync] attributes
- Bare runtime stubs: p_hlt, p_cli, p_sti, p_inb, p_outb, p_serial_* all implemented in pe.rs

## Self-Host Module Status

All 50 modules compile and type-check. 38/38 self-host tests pass.

| Module | Status |
|--------|--------|
| token.fd, diag.fd, json.fd, fmt.fd | DONE |
| ast.fd, lexer.fd, parser.fd | DONE |
| ir.fd, typechecker.fd, lower.fd | DONE |
| mono.fd, borrowck.fd, pir.fd | DONE |
| opt.fd, codegen_c.fd | DONE |
| codegen_x64.fd, pe.fd, peephole.fd, regalloc.fd | DONE |
| compiler.fd, comptime_eval.fd, flat_ast.fd | DONE |
| lsp.fd, inc.fd, main.fd, ark.fd | DONE |
| + 26 more modules | DONE |
| **Total** | **50/50 — type-checks, 38/38 tests pass** |

---

## Build Commands

```bash
# Build bootstrap compiler (Rust)
cargo build --release

# Build self-hosted compiler (Fides → C → .exe)
.\target\release\fidesc.exe ccompile compiler\main.fd -o release\fides-c
# (tcc link is now automatic via the ccompile command)

# Everyday work — use the self-hosted compiler directly:
.\release\fides-c3.exe ccompile tests/bootstrap/hello.fd -o hello.exe
.\release\fides-c3.exe run tests/bootstrap/hello.fd
.\release\fides-c3.exe lsp

# Full verification
./verify.sh          # Bootstrap tests (tests/bootstrap/)
./verify-selfhost.sh # Self-host tests (tests/selfhost/)
```
