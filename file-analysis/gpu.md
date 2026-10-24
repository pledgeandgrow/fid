# gpu.fd — SPIR-V + PTX GPU codegen (826 LOC)

## Role

L3 → SPIR-V word emitter (`SpirVCodeGen`, header/capabilities/memory
model/entry point + per-inst dispatch) and L3 → PTX text emitter
(`PtxCodeGen`), plus `generate_spirv`/`generate_spirv_text`/
`generate_ptx` public API and a structural disassembler. Port of
`src/gpu.rs` (590 LOC; Fides is larger due to `GpuStrI64`/`GpuI64I64`
tuple-workaround structs and explicit enum dispatch).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `main.fd` | :29 `use compiler::gpu`, :1785 `cmd_gpu` | **`fidesc gpu <file> -o out`** now wired — runs the full L1→L2→L3 pipeline and calls `generate_spirv` (`.spv`, binary) or `generate_ptx` (`.ptx`, text). **Since 2026-10-23.** |

Mirrors Rust's `fidesc codegen --target spirv`/`ptx` (main.rs:2266/2280/2300)
as a dedicated command rather than a codegen flag. Was the ninth dead module.

## Findings

### F1 — ~~Dead~~ now reachable + structurally broken output (inherited) — HIGH

**Now wired via `fidesc gpu`** — the stubs below are live, not latent. Every
one is inherited from Rust (src:257-355) but now produces real bad output:

- **Branches target nonexistent IDs** — `Jmp`/`Br` allocate *fresh*
  label IDs (:483, :490-491) never bound to blocks — every
  `OpBranch`/`OpBranchConditional` points to an undefined ID →
  invalid SPIR-V (validation fails). Rust identical.
- **`Mov` emits nothing** — IDs computed "for side effects" (:435-445),
  no `OpStore`/`OpCopyMemory` — every assignment vanishes.
- **`Call` skipped** — arg IDs computed then dropped (:498-505).
- `_ => {}` catch-all (:506) — `Load`/`Store`/`Cast`/`CallPtr`/
  `ElemAddr`/`Vec*`/all SIMD silently produce no opcodes.

### F2 — PTX branch/register bugs (inherited) — HIGH

- `bra target_N` (:650/656) — block labels emit as `func_bN` (:623) —
  **no `target_N` label ever exists** → won't assemble.
- `mov.u64 ret_val, ...` (:643) — `ret_val` never declared (`.reg`
  only declares `r<32>`/`p<8>`) → undeclared identifier.
- `.reg .u64 r<32>` + `r0`-style uses (:612, :628) — PTX register
  syntax is `%r<32>`/`%r0` — likely rejected by `ptxas`/`nvdisasm`.
- Only 4 binops (add/sub/mul/div); `mul.lo.u64` correct; `Br`'s
  `setp.eq.u64 p0, cond, 0` + `@p0 bra else` logic is right.

### F3 — Coverage notes — INFO

`binop_to_spirv_opcode` matches Rust's 16-op table exactly; SPIR-V
header words (magic/version/bound/schema) and section ordering are
correct in shape; `disassemble` is a stub-quality dump. Workgroup
size honored from `func.workgroup_size`. Kernel selection: first
`is_kernel` fn or func 0 — no multi-entry support.

## Verification state

- Dead status verified: imported but never called; `--target spirv`/
  `ptx` absent from main.fd dispatch.
- All defects confirmed inherited — the Fides `gen_inst` is a line-
  for-line port of Rust's equally-stubbed version.
- Can't be exercised (no caller).

## Next

`green_thread.fd`.
