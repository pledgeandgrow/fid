# codegen_x64.fd — PIR L3 → x86-64 asm selector (2,001 LOC)

> **Re-verified 2026-10-23:** headline findings confirmed — float-return → rax (:731 `movq %xmm0,%rax`), scratch regs hardcoded against all-13-allocatable pool, canary `xorq $imm64`, StackAlloc→xorq. Emits `movapd` for XMM spills (:1896/1910) which elf64's assembler can't encode (see elf64.md). Float-arg side not re-traced in detail.

## Role

The real native backend: `L3Module + regalloc → AT&T x86-64 asm` →
`peephole_optimize` → `elf64.emit_object_file` → `runtime_linux` + `linker`
→ ELF executable. Port of `src/codegen_x64.rs` (2,226 LOC). Unlike the other
selectors, this one consumes a real `ModuleAllocation`
(`Location::Reg/Spill/Remat`) and emits stack canaries, overflow panics,
div-by-zero checks, and a recursion guard.

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `compiler/compiler.fd:44` | `use compiler::codegen_x64` | import |
| `compiler/compiler.fd:1640` | `generate_asm(l3)` | `--target x86-64`/`x64` in `build_executable` — the only call site |

`generate_asm` runs `allocate_module` (regalloc) internally, then selects.

## Findings

### F1 — F-4A ABI support stripped in the port — HIGH (port regression)

`gen_call_abi` (:1507-1574) is the *pre*-F-4A version. Dropped vs
`src/codegen_x64.rs:1290-1400`:

- **Float args** → Rust routes F32/F64 args to `xmm0-xmm7`; Fides sends every
  arg through integer regs → callee reads xmm → wrong values.
- **Float returns** → Rust moves result out of `xmm0`; Fides only moves
  `rax` → float-returning calls get garbage.
- **sret** (large struct returns via hidden RDI pointer) → dropped.
- **Struct returns ≤16B in rax+rdx** → dropped (rdx half lost).
- **Variadic `movb $n, %al`** for extern calls → dropped.

Any function call involving floats or >8B struct returns miscompiles on this
backend. Fix: port the F-4A arg classification + return handling.

### F2 — Fixed scratch regs overlap the fully-allocatable pool — HIGH (inherited, systemic)

`regalloc.fd:72-90` makes **all 13 GPRs allocatable** — including rax, rcx,
rdx, rsi, rdi, r10, r11 — while this file hardcodes them as scratch:

| Site | Scratch used | Live-value hazard |
|------|-------------|-------------------|
| `scratch_reg(dst)` :244 | rcx/rdx/r10 (avoids only `dst`) | `imulq`, `lea`, comparison temps smash a vreg in rcx/rdx |
| comparisons :1123-1138 | `%al`, `%cl` | `setne/setcc` write rax+rcx — kills live vregs there |
| `gen_shift` :867 | rcx | shift count load overwrites a live rcx vreg |
| `gen_idiv` :1018-1063 | rax (cqto) + rdx | **every Div/Mod writes rax+rdx**; a `b` in rax is clobbered by the `a→rax` load before `idivq` |
| `gen_load`/`gen_store` | rax (+rcx) | value/ptr staging kills rax/rcx vregs |
| `gen_struct_copy` | rsi/rdi/rax | copy clobbers arg regs' vregs |
| `gen_call_ptr`/`dyn_call` | r11, rax, rdi | same |
| `gen_branch` :1243 | rax | every conditional branch kills a vreg in rax |

rax is **first** in the allocatable list → assigned constantly → the
`setcc %al`/`movzbq`/cqto/lea-via-rax paths corrupt ordinary code whenever
pressure is nonzero. Rust has the same flaw, but it's the defining hazard
of this backend. Fix: exclude fixed scratch regs from `physreg_allocatable`,
or model per-instruction clobbers in regalloc.

### F3 — `FloatToInt` cast converts %rax, not the source reg — HIGH (inherited)

`gen_cast` :1748 loads `src` into `dst_reg`, then :1764 emits
`movq %rax, -16(%rbp)` — reads **rax** regardless of dst_reg. If the src
vreg isn't in rax (usual case), it converts garbage. Identical in Rust
(src:1593-1597). Fix: `movq %dst_reg` or `movq src→rax` first.

### F4 — Stack-canary `xorq $0x5DEADBEEFCAFEBABE` — no such encoding — HIGH (inherited)

:417 + :511 — `xorq` takes **imm32** sign-extended only; imm64 literal
requires `movabsq` + `xorq %reg`. Every function prologue and epilogue emits
it → the elf64 assembler must reject or truncate (verify in elf64.fd's
encoder — likely an assembler error or silently wrong canary). Same in
Rust:273/395 — so the canary may have *never worked*. Also: the canary is
derived from the return address via a public constant — deterministic,
defeats the security purpose.

### F5 — Odd count of stack args misaligns — MEDIUM (inherited)

`gen_call_abi` :1529-1540: pushes stack args **first**, then `subq $8` for
alignment → for an odd stack-arg count, the padding sits *between* %rsp and
the args → the callee's arg7+ are shifted 8 bytes (arg7 reads the padding).
Padding must be subtracted *before* pushing. Inherited from Rust:1362.

### F6 — StructCopy/ElemAddr via `ir_type_size` — struct arrays stride 1/8 — MEDIUM (partially inherited)

`gen_elem_addr` :1478 uses `ir_type_size(elem_ty)` = **0 for Struct** →
`.max(1)` → `leaq` scale=1 → `arr[i]` on struct arrays computes `base + i`.
The file already has `struct_type_size` (:1372, layout-aware) and uses it in
`gen_store` — `gen_elem_addr` was missed, the same incomplete-fix pattern as
codegen_c's GetElemPtr. Also: non-power-of-2 sizes (3,5,6,7,12…) emit
`leaq` with an illegal scale — needs the imulq fallback (Rust also emits
illegal LEA here — inherited).

### F7 — Calls drop arguments — MEDIUM (inherited)

- `gen_call_ptr` :1592-1599: `if reg_idx < arg_regs.len()` — **no stack-arg
  path at all** — args beyond the registers are silently dropped.
- `gen_dyn_call` :1650-1655: only `args[0]` (self) is loaded; **all
  remaining args dropped** — trait methods with parameters get garbage regs.
- `gen_dyn_call` spilled dst: only `Location::Reg` handled → result lost on
  spill (other call paths handle Spill — inconsistent, inherited).

### F8 — `StackAlloc` → `xorq %r,%r` — dst := 0 — MEDIUM (inherited)

:661 — allocates nothing; dst becomes a null pointer. Rust:498 same, with a
comment admitting the shortcut ("L3 already computed frame slots"). If L3
ever emits StackAlloc for a real buffer, writes go through address 0.

### F9 — SIMD path can't survive contact — MEDIUM (inherited)

- `gen_operand_to_xmm` loads spilled vectors via `movapd -off(%rbp)` — movapd
  **requires 16-byte alignment**; spill slots are 8-byte aligned → #GP fault.
- `store_xmm_to_dst` Spill → `movsd` stores only 8 of the 16 bytes.
- `Reg` path `movq %xmm0, %r` keeps the low 64 bits only — 128-bit vectors
  can't live in GPRs; the SIMD value model is incoherent.

### F10 — `gen_operand_to_mem` ImmF → `movq $0` — LOW (inherited)

:813-815 — a float immediate destined for a spill slot stores 0. Dead if
L3 never spills ImmF directly.

### Notes (correct or cosmetic)

- `TryUnwrap` checks the heap tag `(%rax)==0` — **consistent with the C
  backend** (better than riscv's `beqz`).
- Prologue/epilogue: callee-saved pushes+pops in reverse, `leave;ret`,
  per-function epilogue label — correct. `jle` recursion guard ok.
- Overflow `jo` on add/sub/imul + always-on div-zero `testq`+`jz` — good.
- String dedup in `alloc_string`; float consts via `__float_bits:` prefixed
  `.quad`; escape_str covers \\ \" \n \t \r (not other ctrl chars).
- `GlobalAddr` → `Imm(0)` — deliberate stub, code comments it ("x64 backend
  is not the primary target") — globals return address 0. Same class as
  the arm64/riscv stubs, but at least self-aware.
- `movq $imm64` immediates (ImmI > 2³², :726) need movabsq in the encoder —
  unverified against elf64.fd.
- `Bin` on a `Operand::Slot` operand → `addq` the slot *address* — L3 likely
  never emits it (slots only via Load/Store/AddrOfSlot).

## Verification state

- 22/22 L3Inst arms covered; port fidelity checked section-by-section vs
  `src/codegen_x64.rs` — the Fides port is faithful *including bugs*,
  except the F-4A call-ABI strip (F1).
- Cannot run E2E (`build` needs Linux ELF output; fidesc's x64 target
  doesn't run on this host) — findings grounded in ABI/encoding rules +
  regalloc pool inspection.
- Recommended fix order: F2 (reserve scratch regs) → F1 (F-4A port) →
  F3/F4 → F5-F7 → F9.

## Next

`codegen.fd`… no — next alphabetical: **comptime.fd**.
