# codegen.fd — Legacy AST→ARM64 asm generator (1,483 LOC)

> **Re-verified 2026-10-23:** still dead — `use compiler::codegen` exists (compiler.fd:48) but zero symbols are referenced; `compile` still routes to cmd_ccompile. Findings accurate.

## Role

Legacy **AST-direct** AArch64 assembly generator — walks `Program` straight to
asm text (no IR, no regalloc). Port of `src/codegen.rs` (~1,134 LOC).
All values 8 bytes on stack; results in x0 (int) / d0 (float). AAPCS calling
convention, raw Linux `svc #0` syscalls.

This is the **pre-PIR backend**. The live ARM64 path is
`codegen_arm64.fd` (PIR L3 → asm, 557 LOC) — a different module.

## Where it's called

**Nowhere.** `compiler.fd:48` `use compiler::codegen` is the only import —
a symbol-by-symbol check shows **zero** references to `generate`, `CodeGen_*`,
`count_lets`, or any entry type from `compiler.fd` (it uses `CCodeGen_*` from
codegen_c.fd instead).

Divergence vs Rust: `src/main.rs:397` still routes `fidesc compile` →
`compiler::compile()` → `CodeGen::generate` (src/compiler.rs:1135). The Fides
port redirected `compile` → `cmd_ccompile` (main.fd:1394-1396 → C backend) and
never wired an equivalent — orphaning the entire module.

**The file is 1,483 LOC of dead code kept alive only by an unused import.**

## Latent defects (all inherited verbatim from src/codegen.rs)

If this module is ever re-enabled, it is badly broken:

| # | Defect | Site | Effect |
|---|--------|------|--------|
| 1 | `print` → `write(1, x0, 64)` raw syscall | :1118-1128 | `print(5)` does `write(1, 0x5, 64)` → EFAULT; any string forced to 64 bytes (truncated or over-read) |
| 2 | `mov x0, #n` | :1025 | immediates >65535 won't assemble (no movz/movk) |
| 3 | `_ => {}` in `gen_expr` | :1433 | Cast, Asm, Comptime*, PipeForward, QuantCast, Await, Spawn, TryBlock emit **nothing** → stale x0 |
| 4 | `Coalesce` evaluates default unconditionally AND returns it | :1390-1395 | `x ?? d` always yields `d` |
| 5 | `Range` → start only | :1219-1221 | `0..10` evaluates to `0` |
| 6 | `FString` ignores literals, no concat | :1316-1329 | `"a{x}b"` returns `x` |
| 7 | `EnumLit` → `mov x0, #0` | :1333-1345 | variant data dropped entirely |
| 8 | `LetTuple` stores same ptr in every name | :544-557 | all tuple names alias the whole tuple (comment admits "the interpreter handles it") |
| 9 | `bl p_?_method` literal `?` label | :1277 | unresolvable receiver type → assembler error |
| 10 | Args 9+: caller drops (`j<8`), callee never loads stack args (`i<8`) | :446-448,1144 | >8-arg calls silently wrong at both ends |
| 11 | Temp spill area fixed at +8 slots; `next_offset` unbounded | :427,1056-1062 | >8 concurrent temps or deep nesting writes past `frame_size` → **stack corruption**. `count_lets` also misses Block/Handle/Select/Parallel/RegionBlock bodies → undercounted frames |
| 12 | `OptI64::None` → emits nothing; `DerefAssign`/`FieldAssign` are empty stubs | :609, 670-675 | silent no-ops, zero diagnostics |
| 13 | `_start`: `mov x0, #0` before `svc` | :357-361 | main's return discarded → **exit code always 0** |
| 14 | `get_field_offset(field)` — first-match across ALL struct layouts | :223+ | `s.x` gets offset of first `x` in any struct; only works by accident of uniform 8-byte fields |
| 15 | `FloatLit`: `ldr d0` then `mov x0, #0` | :1031-1033 | float in int context → 0 silently |
| 16 | StructLit/ArrayLit allocate in temp region, never freed; spread ignored | :1170-1185,1283-1297 | `next_offset` permanently elevated → compounded overflow |

## Verification state

- Caller analysis: exhaustive grep — only `compiler.fd:48` imports it; zero of
  its ~30 public symbols are referenced anywhere.
- Dead status confirmed structurally; not executed (module unreachable).
- `fidesc lint` unavailable for any `ast.fd` importer right now (the
  `Vec<Vec<Stmt>>` at `ast.fd:344` trips the in-progress split_gt parser work).
- Defects verified by inspection; all mirror `src/codegen.rs` (spot-checked
  Coalesce/svc/compile wiring) — inherited, not port drift.

## Recommended action

**Delete the module and the unused import** (`compiler.fd:48`). The real ARM64
backend is `codegen_arm64.fd`; the Rust side only keeps this file reachable via
the legacy `fidesc compile` subcommand, which Fides deliberately rerouted to
`cmd_ccompile`. If it must stay (e.g., future bare-metal AST-direct use), add a
deprecation header and fix defects 1-4, 9-11 before any use — otherwise it
produces silently wrong binaries.

A secondary note for `compiler.fd`'s run: it carries ~15 unused `use` imports
in the same pattern (dead modules imported-but-never-referenced) — flag for
that report.
