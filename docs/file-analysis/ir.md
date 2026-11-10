# ir.fd — IR data model, indexes, reachability

> **Re-audited 2026-11-09** against the working tree. ir.fd has large
> uncommitted edits (+439/-117 against HEAD) and grew while this audit ran
> (2,733 → 2,908 lines): line numbers are from the last read.

**Path:** `compiler/ir.fd` — **2,908 lines** (2,551 on Oct 22). Imports
`ast` only.

**Role:** the typed SSA IR every stage after lowering shares: `IrType`,
`Effect`/`EffectSet`, `Region`, `BorrowKind`, `ValueMeta`, the 43-variant
`Inst`, `BasicBlock`, `IrFunction`, `IrModule`, `lower_type`, struct
layouts, string interning, the structural verifier, IR printing, source
maps. Since Oct it also holds the backend name resolution and call-graph
reachability shared by dead-function elimination, pe_ir and nat_ir
(moved from pe_ir_stubs.fd).

## Callers

Imported by 25 modules: autograd, checks, codegen_c, codegen_x64, compiler,
debugger, egraph, hotreload, inc, interp, iropt, iropt_me, irverify, lower,
main, nat_ir, opt, pe, pe_ir, pe_ir_stubs, pir, regalloc, rules, veil, wasm,
x86. Every CLI command that compiles code reaches it.

| Function | Live callers |
|----------|--------------|
| `IrModule_verify` (:2561) | `fid verify` (main.fd:2682) |
| `IrModule_dead_function_elimination` (:2344) | iropt.fd:157 (`deadf` pass; emit/nat paths) |
| `pe_ir_reachable_from` / `_functions` / `_needed_functions` | iropt.fd:101, pe_ir.fd:471/651/770/4562/4647, DFE |
| `inst_value_refs` (:1229) | lower.fd ownership passes (:13519, :13730, :13765, :13822, :14014) |
| `IrModule_to_str` | `ir`/`dump-ir` (main.fd:465) |
| `IrModule_source_map` | `sourcemap` (main.fd:2723) |
| `IrModule_optimize` (:2005) | none (dead) |

## Indexes (AGENTS.md item 14)

| Index | Where | How it works |
|-------|-------|--------------|
| `IrFunction.inst_at` | :1365, kept by `IrFunction_emit` (:1463) | value id → position in `insts` + 1; 0 = no instruction (parameters, block params). `IrFunction_get_inst` (:1523) and `IrFunction_set_inst` are O(1). |
| `IrModule.fn_index` | :1627 | HashMap name → position + 1, filled lazily by `IrModule_get_function` (:1864) from `HashMap_len` on. Both wholesale replacements of `functions` clear it (ir.fd:2387 DFE, compiler.fd:3745). |
| `IrModule.struct_index` | :1630 | name → position + 1, kept by `IrModule_add_struct`; `IrModule_get_struct` (:1763) checks the name and falls back to a scan. |
| `IrModule.str_index` | :1633 | string → position + 1 in `string_constants`; `IrModule_intern_string` (:1778) is O(1). |

`IrFunction_get_meta` (:1531) reads `value_metas[id]` when its id matches,
else scans.

## Findings — verified (old doc re-checked)

- HIGH `inst_effects` misses `StructCopy`: **FIXED** (:1163, Write).
- MEDIUM `inst_value_refs` misses `StructCopy`: **FIXED** (:1256). The match
  now covers all 43 variants; it is live (lower.fd passes).
- MEDIUM `IrModule_verify` reads `Branch` fields in the wrong order:
  **FIXED** (`Inst::Branch(_, then_b, else_b)`, :2658). CHANGED: it is now
  reached by `fid verify`. It still checks block structure only; use-def
  and dominance live in irverify.fd.
- MEDIUM DFE strips functions reached through pointers: **FIXED**. Roots are
  main, `p_*`, C callbacks, externs; reachability follows calls (resolved to
  aliases and `pert_*`), `ConstFnRef`, `ConstStr` naming a function (thread
  entries), `dyn` methods by suffix, print formatters, autograd builtins and
  stub dependencies (`pe_stub_deps`). Live in iropt.
- MEDIUM linear lookups on the hot path: **FIXED** for `get_inst`,
  `get_function`, `get_struct`, `intern_string`, mostly fixed for
  `get_meta`. See the remaining scans below.

## Findings — code inspection

### LOW — `IrModule_get_function` trusts its index without a name check

:1874 returns `functions[at - 1]` without comparing the name
(`get_struct` does compare). A slot set to a function of another name, or a
`functions` list replaced without `HashMap_clear`, returns the wrong
function silently. Today every in-place `functions.set` keeps the name.

### LOW — clobber operands are value id -1

`inst_value_refs` (:1309-1319) returns every Asm input id, including the
-1 of a clobber (lowering's encoding; see lower.md HIGH). Callers must
skip ids < 0.

### INFO (perf) — scans left

`IrModule_add_function`/`replace_function` (:1821, :1836) scan by name
(O(F²) while lowering); `IrFunction_lookup_local` (:1510) and
`IrFunction_set_meta` (:2464) scan; `pe_module_has_fn` (:2128) scans all
functions and is called per callee and per `ConstStr` in reachability;
`pe_ir_reachable_from` keeps `seen` as a list (:2217); DFE tests membership
with a scan (:2377); `pe_sort_strs` is an insertion sort.

### INFO — dead code

`IrModule_optimize` (:2005) and `IrFunction_constant_fold` (:2393) have no
caller. `constant_fold` indexes `blocks` by block id, which only holds
while ids equal positions.

### Inherited, unchanged

- `ir_type_size(Struct)` is 0; use `IrModule_type_size` for structs.
- `lower_type` maps `I128` to `I64`.
- `inst_effects` still ends in `_ => {}`; constants, casts and pointer
  arithmetic have no effects, which is intended.

## Fix order

1. Name check in `IrModule_get_function` (cheap safety).
2. A real clobber list in `Inst::Asm`.
3. Use `fn_index` in `pe_module_has_fn`; a set for `seen` and DFE.

## Verification state

All findings above checked by reading current source. `fid verify` was not
run in this audit. The file changed during the audit.
