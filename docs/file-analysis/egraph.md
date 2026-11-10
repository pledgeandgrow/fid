# egraph.fd — e-graph rewriting and local store-to-load forwarding

> **New doc 2026-11-09.** Read at 790 lines.

- **Path:** `compiler/egraph.fd` — **790 LOC**
- **Role:** per function, pure integer/bool values become e-nodes; equality saturation with
  built-in rules; each instruction is rewritten in place into its class's cheapest form, a
  constant, or a move from an earlier dominating equal value (ME6-ME11). Also forwards loads of
  plain scalar locals from their single dominating store or the last store in the block (ME3 step).

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `main.fd` `cmd_build`, nat_ir path | :3076 | `egraph_run(&mut nat_mod)` after `iropt_native` and `rules_run`, skipped by `--debug` only |

Only importer: `main.fd`. Not on the pe_ir default Windows path, the C path, `run`, or PIR.
Uses `iro_cfg` / `iro_dominates` from iropt.fd.

## What it does

- `egraph_run` (:768): over every function (not only reachable ones), `eg_forward` then
  `eg_function`. Returns [loads forwarded, instructions rewritten] for the `[prof] native
  mid-end` line.
- Nodes: const, binary op, neg, not, bitnot, leaf; key includes the operand type string.
- `eg_round` (:227) rules: commutation, flipped comparisons, constant folding (`eg_fold`, marked
  `@wrapping`, refuses anything that would panic or does not fit the type, u64 never folded),
  identities (`x+0`, `x*1`, `x&-1`, `x*0`, `x-x`, `x==x`, ...), `x*2 = x+x`, `!!b = b`; in
  `@wrapping` functions `x*2^s = x<<s` and `(x+c1)+c2 = x+(c1+c2)`.
- Extraction: cost 1 per op, 3 mul, 20 div/mod; rewrites only when the new operands dominate.
- Limits: `EG_MAX_NODES` = 20000, `EG_MAX_ROUNDS` = 8, 50 cost iterations.
- Switches: `FID_NAT_OFF=egraph`, `FID_NAT_OFF=forward` (`eg_on` :79).

## Findings

### Code inspection

#### LOW — wrapping reassociation computes `c1 + c2` in checked arithmetic

`eg_round` (:280-285): in a `@wrapping` function `(x + c1) + c2` adds `inner.get(2) + k` in
`eg_round`, which is not `@wrapping`. Two large constants panic the compiler. The sum is also not
narrowed to the type (an i32 constant above 2^31 is made). Compute it in a `@wrapping` helper and
narrow it.

#### LOW — `!!b` rule is quadratic

The `EK_NOT` arm scans every node for each `not` node (:301-310). Bounded by `EG_MAX_NODES`, but
20000² comparisons are possible on a large function.

#### INFO — checked, no defect found

- Forwarding: a candidate local is a non-struct, non-array `Alloca` used only as a Load/Store
  address; the cross-block case needs exactly one store whose block dominates the load
  (single-def stored value: sound). Types must match.
- Identities removed only where they cannot remove a panic (no `x/x`, `0/x`).
- Unused parameter `module` in `eg_function`.

## Fix order

1. `@wrapping` helper for the reassociation constant.

## Verification state

- Whole file read. No live probe: s18 predates this file (its mid-end line has no e-graph counts).
