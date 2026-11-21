# iropt_me.fd — restructuring mid-end passes (Track ME / LF)

> **New doc 2026-11-09.** Every pass here is opt-in and, per the roadmap, none has been run on
> the suite yet. Read at 2,342 lines.

> **Nov 21: no longer all opt-in.** `ime_on(name)` is true for the passes
> of `IME_DEFAULT_ON` unless `FID_IROPT_OFF` names them: loops, sra,
> loadhoist, forward, matchfacts, inline2, inlineclo, lendref, unroll,
> linear, placefwd, vecalias, version, ssa, ssaexit (ME22/ME26 on Nov 16,
> unroll and version on Nov 17); the others still need `FID_IROPT_ON`. The
> module imports `checks`, `ssa` (the SSA round trip, ssa.md), `tv` and
> `pgo` too. New open: partial unrolling miscompiles a loop with a dead
> store on pe_ir (known-bugs 2g.80), translation validation rejects inline2
> on `Str_drop` (2g.90), the `ime_sra` miscompile crashes a `--nat` build
> of the front end (2g.94). The text below is of Nov 9.
>
> **Nov 21 (5fb5516):** 6,608 lines. 2g.109's mid-end half is fixed
> here: `ime_remap` renames the operands of `PersistLoad`, `PersistSave`
> and `Backward` (a forward-renamed load's `backward` read a value defined
> nowhere), and `ime_unrenamed_uses` (:352) marks every value an
> instruction remap cannot rename reads (a TryUnwrap, an asm! input) so
> `ime_forward` leaves it itself. A counted-loop `step` of `i64::MIN` is
> rejected in `ime_reduce_one` (`-i64::MIN` does not exist; the 2g.89
> family). 2g.80 and 2g.94 turned out to live elsewhere and are fixed
> there: the dead-store miscompile was pe_ir's global register allocator
> starting a later-laid-out block's value on a live register (pe_ir.md),
> and the sra crash was `ng_rf_promote` writing its zero-words before the
> argument moves of the prologue (nat_ir.md `ng_rf_zero_at`). Still open:
> inline2 on `Str_drop` (2g.90).

- **Path:** `compiler/iropt_me.fd` — **2,342 LOC** on Nov 9 (6,608 on Nov 21)
- **Role:** the mid-end passes that change the CFG or memory shape, called by `iropt_native`
  (iropt.fd), plus an alias query built from language rules (LF1-LF4).

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `iropt.fd` `iropt_native` | :45 | `if ime_on("inline2") { ime_inline(module) }` |
| `iropt.fd` `iropt_native` | :130 | `ime_function_passes(&mut f)` per reachable function, before fold |
| `iropt.fd` | :59-72, :157 | `ime_on(...)` gates branchfold, ipcp, spec, dse, simplify, deadf |

Only importer: `iropt.fd`. Reached from the same commands as iropt (pe_ir and nat_ir builds),
never from C, `run` or PIR paths.

## What it does

`ime_on(name)` (:51): true only if `FID_IROPT_ON` names the pass (or `all`) and `FID_IROPT_OFF`
does not. `ime_function_passes` (:1727) runs, in order:

| Name | Function | What |
|---|---|---|
| sra | `ime_sra` :1390 | struct local with field-only loads/stores becomes one local per field (ME26) |
| switch | `ime_switch` :1593 | chain of >= `IME_SWITCH_MIN` = 4 `x == k` tests becomes a balanced compare tree (ME28) |
| loops | `ime_canonical_loops` :835 | preheader, one latch, dedicated exits (ME22) |
| ivsr | `ime_strength_reduce` :966 | `i * c` stepped with `i`, `@wrapping` functions only (ME23) |
| unroll | `ime_unroll` :1169 | one-block loop, <= `IME_UNROLL_TRIPS` = 8 trips, <= `IME_UNROLL_INSTS` = 160 (ME24) |
| loadhoist | `ime_hoist_loads` :2053 | loads out of loops that cannot change them (LF5) |
| forward | `ime_forward` :2152 | store-to-load forwarding, repeated loads (LF6) |
| matchfacts | `ime_match_facts` :2261 | in a match arm, the scrutinee's tag load is the constant (LF23) |

Module level: `ime_inline` (:748, name `inline2`, ME20/21): bottom-up over the call graph,
callees up to `IME_INLINE_MAX` = 60 insts (400 = `IME_INLINE_ONCE_MAX` for a callee called once),
callers kept under `IME_CALLER_MAX` = 4000, at most `IME_SITES_MAX` = 64 sites.

Alias query (LF1-LF4): `ime_addr` classifies an address (local / written param / `&` param /
global / unknown, with a field path); `ime_unescaped` (:1837), `ime_may_alias` (:1948),
`ime_writes` (:1995). Two different params never overlap when either is written (rests on the
borrow checker and the `borrow_kind` the lowering puts on `&`/`&mut` params), `*u8` params are
left out.

## Findings

### Code inspection

#### MEDIUM — unmarked pointer parameters count as `&mut`

`ime_may_alias` treats two different parameters as disjoint because the borrow checker forbids
overlapping `&mut`. The lowering now marks `&mut T` / `&T` parameters (`meta.borrow_kind`,
lower.fd:4129, uncommitted on Nov 9) and `ime_addr` (:1802) uses that, but an unmarked parameter
of any pointer type other than `*u8` (`ime_raw_ptr` :1794) is also taken as kind 2 ("written
param", disjoint from the others). Raw `*mut i64` parameters in unsafe code carry no borrow-rule
guarantee and may point to the same place. Opt-in passes only (loadhoist, forward, matchfacts).
Make an unmarked pointer kind 0 (unknown) before turning these on.

#### LOW — no suite run yet

The header (:26-29) says the passes have not been run on the suite; the roadmap agrees (ME20-ME28,
LF5, LF6, LF23 "written, opt-in"). Treat any of them as unverified.

#### INFO

- No by-value `Vec` parameter is mutated (scan of all functions; value semantics would drop the
  write).
- ME25 (loop versioning) and ME27 (two-register returns) are explicitly not here (:34-38).
- Not re-checked pass by pass; only the header, pass list, switches and alias query were read.

## Fix order

1. Run the suite with `FID_IROPT_ON=all` and with each pass alone before enabling any.
2. Narrow `ime_addr`'s "written parameter" to params the lowering marked `&mut`.

## Verification state

- Read: header, `ime_on`, helpers list, pass list, alias-query comments and signatures.
- No live run: s18 does not contain this module's passes.
