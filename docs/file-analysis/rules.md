# rules.fd — rewrite rules as data, pure-call merging, rule tests

> **New doc 2026-11-09.** Read at 822 lines while another session was editing it (read-only call
> merging became opt-in, a library-facts reader was added). Line numbers are of that state.

- **Path:** `compiler/rules.fd` — **822 LOC**
- **Role:** 27 identity rules on integer BinOps (`ru_rules` :71), applied in place (`ru_rewrite`
  :170); merging of repeated pure calls under dominance (`ru_pure_cse`, WP9) and, opt-in, of
  read-only calls with no write between (WP10); `fid test-rules` self-test and SMT-LIB proof files
  (ME16, ME17); a reader for a library-facts file (WP20).

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `main.fd` `cmd_build`, nat_ir path | :3073 | `rules_run(&mut nat_mod)` after `iropt_native`, skipped by `--debug` only |
| `main.fd` dispatch | :171-176 | `fid test-rules [--smt <dir>]` → `rules_selftest` (+ `rules_smt`) |

Only importer: `main.fd`. Not on the pe_ir default Windows path, C path, `run` or PIR.

## What it does

- Rules: `x+0`, `0+x`, `x-0`, `x-x`, `x*1`, `1*x`, `x*0`, `0*x`, `x/1`, `x&0`, `0&x`, `x&-1`,
  `x&x`, `x|0`, `0|x`, `x|x`, `x^0`, `0^x`, `x^x`, `x<<0`, `x>>0`, and `x==x`..`x>=x`. Integers
  only, operand types equal; result is a `BitCast` move, a constant or a bool.
- Pure calls: a callee whose externally visible effects (`ru_ext_effects`: local alloca traffic
  ignored, `@crashfree`/`@no-alloc` drop Diverge/Alloc) are empty, scalar result, not a runtime
  prefix (`p_`, `pe_`, `pert_`, `nat_`); the second call becomes a move when the first's block
  dominates. Arguments keyed by constant value or by id.
- Read-only calls (`readcse`): same, when every path between the two calls is free of writes
  (`ru_read_reuse` :409).
- Library facts: `FID_LIBFACTS_IN=<file>` (`ru_lib_load`) marks absent callees pure/read-only.

## Switches

- `FID_NAT_OFF=rules`, `FID_NAT_OFF=purecse` (`ru_on`).
- `FID_RULES_ON=readcse` turns read-only call merging on (`ru_opt_in` :572; off since Nov 9:
  "an access at -1 on every native program").
- `FID_RULE_STATS=1` prints hits per rule (ME18). `FID_LIBFACTS_IN=<path>`.

## Findings

### Code inspection

#### HIGH (opt-in now) — read-only call merging misses writes after the second call in a loop

`ru_read_reuse` (:409-450) walks predecessors backward from the second call's block `bi`.
`seen2[bi]` is set first, so when the walk reaches `bi` again through a loop back edge it is
skipped, and the part of `bi` after the call (`bk+1..end`) is checked only for a direct self
loop (:423). Example: `a = rd()` before a `while` loop whose body does `b = rd(); G = G + 1;`
with `rd` a multi-block function reading `G`: the body call is replaced by `a` although the store
to `G` sits on the path body → header → body. Fix: when a predecessor is `bi` at any depth, check
`ru_block_barrier(f, esc, bi, bk + 1, MAX)`. Now behind `FID_RULES_ON=readcse`, so not on by
default.

#### LOW — library-facts reader never sees read-only entries

`ru_lib_load` (:780-) accepts effect pieces `read`/`diverge`, but iropt's writer emits
`!read,...` (ir.fd `EffectSet_to_str`): the first piece always fails. Only `pure` entries load.

#### INFO — checked, no defect found

- Each rule is exact under checked arithmetic (its left side never overflows); `fid test-rules`
  (s18): 27 rules, 0 failed. The SMT files use `bvashr` for `>>`, irrelevant for shift 0.
- `ru_rewrite` walks `f.insts`, including instructions in no block: harmless.
- Pure-call merging needs dominance only; a pure callee reads no memory, so no barrier check is
  needed. A callee that calls anything has the call's pessimistic IO effect, so only leaf
  functions qualify.
- `x & -1` matches only the literal -1, so unsigned all-ones constants (255 as u8) are not used:
  missed rewrite only.

## Fix order

1. Fix the back-edge check in `ru_read_reuse`, then find the "-1" crash, before `readcse` goes
   back on.
2. Agree the facts-file effect format with `iro_libfacts`.

## Verification state

- Whole file read. Live: `s18.exe test-rules` passes. A probe for the loop case (rd.fd) was built
  with s18 for aarch64-linux, but s18 is older than this code (its line says "0 pure calls
  merged") and its nat backend panicked in `ng_mir_key` (nat_ir, not this file), so the finding
  is code inspection only.
