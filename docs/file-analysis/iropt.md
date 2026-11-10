# iropt.fd — IR mid-end for the native backends

> **New doc 2026-11-09.** Written from the file as it stood on Nov 9 (3,656 lines; another
> session was editing it during the read: deadf became opt-in, gconst got two phases, envtrim /
> ipcp / spec now skip functions named by a string constant). Line numbers are of that state.

- **Path:** `compiler/iropt.fd` — **3,656 LOC**
- **Role:** the IR optimizer that runs on `IrModule` before the own native backends (pe_ir and
  nat_ir). Pass manager `iropt_native` (:42), shared CFG facts (`IroCfg`, `iro_cfg` :985,
  `iro_dominates` :1105), use lists (`IroUses`, `iro_uses` :1580), per-function and whole-program
  passes, a library-facts writer. The restructuring passes live in `iropt_me.fd`.

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `main.fd` `cmd_build` (`build`/`emit`, Windows default, pe_ir) | :3162 | `if do_opt { iropt_native(&mut ir_mod) }` — skipped by `--no-opt` and `--debug` |
| `main.fd` `cmd_build` (nat_ir: `--target aarch64-linux / riscv64-linux / x86_64-linux`, `--nat`, `--native` with arm64/riscv64/x64 targets) | :3070 | `if !debug_mode { iropt_native; rules_run; egraph_run }` — **`--no-opt` does not turn it off**, only `--debug` |
| `compiler.fd` `build_executable` | :2215 | `build --native --target windows` (x86-64-windows): iropt then `pe_ir_generate_pe` |
| `egraph.fd`, `rules.fd`, `irverify.fd` | — | import only `iro_cfg` / `iro_dominates` |

Not reached from: the C path (`ccompile`, `dump-c`, posix `build`: compiler.fd:1838 says C
generation runs no IR optimizer), `run` (interpreter), `--elf`, `--bare`, `--boot`, the PIR path.

## Passes, in order (iropt_native :42-158)

Module level first, then per function (only functions main reaches, `pe_ir_reachable_functions`),
then module level again. "on" = on by default, off with `FID_IROPT_OFF=<name>`; "opt-in" = needs
`FID_IROPT_ON=<name>` (or `all`) as well (`ime_on`, iropt_me.fd:51).

| # | Name | Function | Default | What it does |
|---|---|---|---|---|
| 1 | inline | `iro_inline` :375 | on | one-block callee, <= `IRO_INLINE_MAX` = 16 plain insts, scalar result, same `@wrapping`, no drops/zeroize/persist |
| 2 | inline2 | `ime_inline` (iropt_me) | opt-in | multi-block, bottom-up, cost model |
| 3 | envtrim | `iro_env_trim` :3310 | opt-in (since Nov 9 evening: a caller still passes the trimmed slot, test_brotli) | drops unused `__env` param and call args (WP7) |
| 4 | ipcp | `iro_ipcp` :2894 | opt-in | constant arguments into callees (WP13) |
| 5 | spec | `iro_specialize` :3422 | opt-in | clones for constant arguments some sites pass (WP14) |
| 6 | gconst | `iro_const_globals` :3134 | on | loads of never-written `IntLit` globals become constants (WP18), two phases |
| 7 | (ime) sra, switch, loops, ivsr, unroll, loadhoist, forward, matchfacts | `ime_function_passes` | opt-in | see iropt_me.md |
| 8 | fold | `iro_fold` :489 | on | i64 constant arithmetic without overflow, comparisons |
| 9 | simplify | `iro_simplify` (+ `iro_identities` :2467) | opt-in | identities, reassociation, `*2^k`, `/2^k`, `%2^k` |
| 10 | branchfold | `iro_branch_fold` :1664 | opt-in | constant / same-target branch to jump (made 10 suite programs wrong, roadmap ME12) |
| 11 | branchfacts | `iro_branch_facts` :1803 | on | condition known on single-entry edges; same-operand comparisons settled |
| 12 | sccp | `iro_sccp` :1997 | on | sparse conditional constant propagation, dead edges dropped |
| 13 | promote | `iro_promote` :2728 | on | load with one reaching store becomes that value (ME3 partial) |
| 14 | devirt | `iro_devirt` :3078 | on | `DynCall` on a `MakeDyn` becomes a direct call (WP16) |
| 15 | dse | `iro_dse` :2199 | opt-in | dead stores to plain locals |
| 16 | dce | `iro_dce` :578 | on | unused pure values (3 rounds) |
| 17 | tailcall | `iro_tailcalls` :1382 | on (`IRO_TAILCALL`) | self tail call into a jump |
| 18 | gvn | `iro_gvn` :1276 | on (`IRO_GVN`) | dominating equal pure value reused; repeated local loads in a block |
| 19 | licm | `iro_licm` :687 | on (`IRO_LICM`) | loads of locals the loop never stores, `Vec_len`, comparisons/bit ops, moved to the preheader |
| 20 | deadf | `IrModule_dead_function_elimination` | opt-in (since Nov 9) | dead functions after optimization (dropped runtime entry points and task bodies when on) |
| 21 | — | `iro_libfacts` :3637 | when `FID_LIBFACTS=<path>` | writes `fn <name> params=N effects=<..> @flags` lines (WP20) |

One sequence per function, no fixpoint. Passes 1-6 and 20 need `main` in the module.

## Switches

- `FID_IROPT_OFF=a,b` — turns any pass above off (names as in the table).
- `FID_IROPT_ON=a,b|all` — turns the opt-in ones on.
- `FID_IROPT_STATS=1` — functions changed per pass, instruction count before/after, per-pass ms
  (`iro_time` :191 wraps fold..licm; inline, devirt and the module passes are not timed).
- `FID_LIBFACTS=<path>` — the facts file (nat_ir.fd:5130 appends `ext` lines to the same path).
- Consts: `IRO_INLINE_MAX` = 16, `IRO_LICM` = `IRO_GVN` = `IRO_TAILCALL` = true.
- `inc.fd:230` puts `FID_IROPT_OFF`/`FID_IROPT_ON`/`FID_NAT_OFF` in the build key.

## Findings

### Code inspection

#### HIGH — SCCP folds narrow and unsigned types with i64 arithmetic

`iro_sccp` (:2031-2036) evaluates every integer BinOp whose result type passes `iro_int_ty`
(:2427: i8..u64 and bool) with `iro_sccp_eval` (:1944), which computes in signed i64 and checks
overflow only at 64 bits. For `i32` `2147483647 + 1` it gives the constant 2147483648 with type
i32: the overflow panic is lost and the value does not fit the type. Unsigned u64 values at or
above 2^63 compare and divide as negative numbers; `>>` is arithmetic for unsigned types.
Reachable after inlining: literal arguments are `ConstInt(_, i64)` (seen in `fid ir`), so
`add32(2147483647, 1)` inlined gives exactly this BinOp. On by default on pe_ir and nat_ir.
Not shown live: the probe binary (s18) predates sccp (its `FID_IROPT_STATS` lists only
fold/dce/tailcall/gvn/licm). Roadmap ME14 says "a fold that would panic stays varying": true at
64 bits only. Fix: fold only I64 (as `iro_fold` and `iro_step` do), or narrow and range-check.

#### HIGH — SCCP evaluation can panic the compiler

`iro_sccp_eval` computes `r = a + b`, `a - b`, `a * b` before testing for overflow, and the
UnaryOp arm computes `0 - av - 1` for BitNot (:2044). The function is not `@wrapping` (only
egraph's `eg_fold` is), and the compiler runs with checked arithmetic (s18 panicked "Integer
overflow on multiplication" in nat_ir's `ng_mir_key` in another probe). A program with
`9223372036854775807 + 1` on constants, or `~(-9223372036854775808)`, should panic at run time;
instead `fid build` itself panics. Fix: mark `iro_sccp_eval` `@wrapping` (the checks after the
operation are already written for wrapped results), same for the BitNot arm.

#### HIGH — branchfacts settles float comparisons as if they were total

`iro_branch_facts` (:1803) takes the mask of any comparison (`iro_cmp_mask` :1744) without
looking at the operand type. On the false edge of `a < b` it treats `a >= b` as true (and
`a <= b` false → `a > b` true, etc.). With a NaN operand both are false. `fn f(a: f64, b: f64)
{ if a < b { return 1; } if a >= b { return 2; } return 3; }` returns 2 for NaN instead of 3
(parameters are used as values directly, so both comparisons share operands). On by default.
Fix: skip when the comparison's operands are floats (or only use the true edge for floats).

#### MEDIUM — gconst encodes the initializer as `iv + 2`

`iro_const_globals` (:3144) stores `v = iv + 2` (0 / -1 mean "not a constant" / "excluded"):
an `IntLit` initializer of `i64::MAX - 1` or `MAX` overflows in the compiler (panic), and a
global initialized to -2 is never folded. Folded loads take the load's type even for bool or
narrow loads. Use a separate flag vector.

#### LOW — `FID_LIBFACTS` output does not match its reader

`iro_libfacts` writes `effects=` with `EffectSet_to_str` (ir.fd:889): `pure` or `!read,diverge`.
The reader in rules.fd (`ru_lib_load`) accepts `read`/`diverge` pieces without the `!`, so the
first piece `!read` never matches: read-only facts are never loaded (pure ones are). It also
writes `f.effects`, which counts local stores, so few functions come out `pure`. Missed
optimization only. The file is built with `out = out + line` per function (quadratic).

#### LOW — the pass sequence runs once

No fixpoint: what promote exposes (constants) is not seen by fold/sccp of the same function, and
dce runs before gvn/licm leave values dead. Intended for compile time; noted for ME work.

#### INFO — checked, no defect found

- inline: callee copy renames through a map, keeps source lines, refuses drops, zeroize, persist,
  interrupt and `@wrapping` mismatch; `Alloca`s land in the caller's block, not its entry.
- fold: I64 only for arithmetic; Mul folded only within sqrt(MAX); Div/Mod never.
- licm: single preheader ending in a Jump required; Vec_len hoisted only if the loop calls nothing
  but Vec_get/set/len; loads only of plain locals the loop does not store.
- gvn: dominance-based reuse; repeated loads only of plain locals with no store between.
- simplify (opt-in): reassociation and strength reduction limited to I64/unsigned with fit checks.
- promote: reaching-store dataflow, loads read by TryUnwrap/Asm kept (fix of Nov 9, log M8).
- not re-checked in depth: tailcalls, ipcp, specialize, env_trim, devirt, dse.

## Fix order

1. `@wrapping` on `iro_sccp_eval` (compiler panic) and fold only I64 or narrow results (SCCP).
2. Exclude float comparisons from branchfacts.
3. gconst flag vector instead of `iv + 2`.
4. Strip the `!` in the facts reader or writer.

## Verification state

- Read: header, pass manager, inline, fold, dce, licm, gvn, sccp eval and lattice step, branch
  facts, simplify, promote start, gconst, libfacts. Not read in depth: the rest.
- Live: s18 (latest binary given) is older than this file (no sccp/branchfacts/promote), so the
  three HIGH findings are code inspection only. Probes ov32/ov64/shr8 gave correct results on s18.
