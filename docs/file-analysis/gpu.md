# gpu.fd — SPIR-V + PTX GPU codegen

> **Re-audited 2026-11-09.** Since 10-23 the only change is tuples →
> `GpuStrI64`/`GpuI64I64` structs and `str_equal` → `==` (commit e1b2c3f).
> Probes with the latest self-hosted binary (s18).

- **Path:** `compiler/gpu.fd`
- **LOC:** 826 (Rust `src/gpu.rs`: 590)
- **Role:** PIR L3 → SPIR-V words (`SpirVCodeGen`, `generate_spirv` :786,
  `generate_spirv_text` :793 + `disassemble` :523) and L3 → PTX text
  (`PtxCodeGen`, `generate_ptx` :800).

## Where it's called

| Caller | Site | What |
|---|---|---|
| `main.fd` | :31 import, :187 → `cmd_gpu` :2303 | `fid gpu <file> --target spirv|ptx [-o out] [--text]`: front end → `lower` → L1 → L2 → L3 (no optimizer) → generator |

Nothing else imports it.

## Findings — verified live

### HIGH — SPIR-V output is malformed from the first types (STILL TRUE, worse than recorded)

- `SpirVCodeGen_emit_with_result(_id)` writes word count `2 + operands`
  (:149, :162) but emits header + type + id + operands (3 + operands).
- `OpTypeVoid`/`OpTypeInt`/`OpTypeBool` are emitted through `emit` with
  no result id (:178-205): the allocated id is never written.
- Probe (`@kernel fn add1`): words decode as Capability, Capability,
  MemoryModel, TypeVoid (1 word, no id), TypeFunction, then an
  instruction header with word count 0. Same bug in src/gpu.rs:139/370.
- Branch targets are fresh ids never bound to labels (:482-496), `Mov`
  emits nothing (:434-447), `Call` emits nothing (:498-505), `_ => {}`
  drops Load/Store/Cast/... (:506). Only one function (first `@kernel`,
  else function 0) is emitted; the entry point has no name.

### MEDIUM — `--text` labels every instruction "OpNop" (NEW)

`opcode_name` (:545) matches on constant names (`OP_NOP => ...`). In a
match these bind like a variable, so the first arm takes every value.
Probe: all lines "OpNop (n words)". The same trap may exist wherever
`match` uses `const` names; the type checker does not warn.

### HIGH — PTX output does not assemble (STILL TRUE)

Probe: 16,482 lines, 281 `.entry` (every function of the program, core
included, is an entry), 5,978 "// unhandled" comments (comparisons,
loads, stores, calls). `bra target_N` but labels are `<fn>_bN` (:623,
:650-657); `ret_val` undeclared (:643); only `r<32>` declared while
registers up to `r275` are used; parameters not declared. Only
add/sub/mul/div (:741-756). `ptxas` not available to confirm.

## Fix order

1. SPIR-V: correct word counts, result ids on type instructions, bound
   labels per block, real stores/calls — or remove the command until then.
2. PTX: emit only kernels, block labels that match `bra`, declare
   registers/params, comparisons.
3. `opcode_name`: compare with `==` (or literal patterns).

## Verification state

Whole generator read. Probes: `gpu --target spirv` (70 words, decoded),
`--text`, `--target ptx`. No SPIR-V validator or `ptxas` run.
