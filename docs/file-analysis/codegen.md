# codegen.fd — legacy AST → ARM64 asm generator (dead)

> **Re-audited 2026-11-09.** Since 10-23 the only changes are a rename
> (`cg_ArmStringConstEntry_new`) and three no-op `Stmt` arms (Resume,
> SpawnScope, CancelScope) from commit 6c33558 (Oct 24).

- **Path:** `compiler/codegen.fd`
- **LOC:** 1,497
- **Role:** walks the AST (`Program`) straight to AArch64 assembly text
  (no IR, no regalloc). Port of `src/codegen.rs`. The live ARM64 paths are
  `nat_ir.fd` (machine layer) and the PIR-era `codegen_arm64.fd`.

## Where it's called

**Nowhere.** `compiler.fd:51` `use compiler::codegen;` is the only import.
None of its public functions (`generate` :316, `CodeGen_*`, `count_lets`
:1455) is called from any module (grep of every `pub fn` name). It is
imported only, so it is still parsed, type-checked and compiled into
`fid` on every self-compile. Two of its types duplicate others by name
with the same shape: `ArmStringConstEntry` (also codegen_arm64.fd:95) and
`OptI64` (:110; also ast.fd:76, opt.fd, regalloc.fd).

## Latent defects (all inherited from src/codegen.rs; line numbers updated)

| # | Defect | Site |
|---|--------|------|
| 1 | `print` → `write(1, x0, 64)` raw syscall, any value, 64 bytes | :1132-1142 |
| 2 | `mov x0, #n` for any immediate (no movz/movk) | gen_expr IntLit |
| 3 | `_ => {}` in `gen_expr`: Cast, Await, Spawn, TryBlock, ... emit nothing | :1447 |
| 4 | `Coalesce` returns the default | :1404 |
| 5 | `Range` → start only | :1233 |
| 6 | `FString` ignores literals | :1330 |
| 7 | `EnumLit` drops the payload | :1347 |
| 8 | `LetTuple` binds every name to the whole tuple | :544 |
| 9 | `bl p_?_method` for an unknown receiver | :1291 |
| 10 | arguments 9+ dropped at both ends | :446, :489, :1158 |
| 11 | fixed temp area; `count_lets` misses nested bodies | :1455-1492 |
| 12 | `DerefAssign`/`FieldAssign` empty | gen_stmt |
| 13 | `_start` exits 0 whatever main returns | :357-361 |
| 14 | `get_field_offset` first match across all structs | :223 |
| 15-16 | float in int context → 0; literals never freed | gen_expr |

Status: STILL TRUE by inspection (code unchanged); not executed (no caller).

## Recommended action

Delete the module and the import at compiler.fd:51. That also removes the
duplicate type names and ~1,500 lines from every self-compile.

## Verification state

Exhaustive grep of every public name across compiler/*.fd and stdlib;
`git diff 34259be HEAD` reviewed (rename + 3 arms only).
