# Vector operations in the machine layer (TG11, OP21)

The machine layer (`compiler/nat_ir.fd`, `compiler/nat_enc.fd`: the
`fid emit` path of the Linux targets, macOS and `--nat` Windows) has
vector operations in its machine IR. The simd module's f64 intrinsics
(`stdlib/simd.fd`) compile to them. Status: written Nov 16; it has not yet
been run on a built compiler (see "Checks").

## Operations

| MIR op      | Meaning                                               | x86-64 (SSE2 / AVX2)        | AArch64 NEON          | RISC-V V 1.0              |
|-------------|-------------------------------------------------------|-----------------------------|-----------------------|---------------------------|
| `MO_VLD`    | `V[k]` = w bytes at `r1 + i0` (unaligned)             | movdqu / vmovdqu            | ldr q                 | vle64.v                   |
| `MO_VST`    | w bytes at `r0 + i0` = `V[k]`                         | movdqu / vmovdqu            | str q                 | vse64.v                   |
| `MO_VBIN`   | `V[d] = V[d] op V[b]`, op an `NVK_*` kind             | paddq..divpd / VEX 3-operand | add..fdiv .2d/.4s/.16b | vadd.vv..vfdiv.vv         |
| `MO_VSPLAT` | every 64-bit lane of `V[k]` = `r1`                    | movq + punpcklqdq / vpbroadcastq | dup .2d          | vmv.v.x / vfmv.v.f        |
| `MO_VLANE`  | `r0` = lane 0 or 1 of `V[k]`                          | movq, pshufd                | umov / dup d          | vmv.x.s, vslidedown.vi    |
| `MO_VZMASK` | lanes of `V[d]` zeroed where `V[b]` is +-0.0          | pxor, cmpneqpd, pand        | fcmeq #0.0, bic       | vsll.vi, vmseq.vi, vmerge.vim |
| `MO_VEND`   | end of 256-bit code                                   | vzeroupper                  | -                     | -                         |

`i1` is the width in bytes: 16 (SSE2, NEON, RVV at VL = 2 x e64, which
every VLEN >= 128 has) or 32 (AVX2, `--cpu x86-64-v3|v4`). `MO_VBIN` is
selected from the TG1 table (`ng_isel_rows`, forms `NG_IF_X64_SSEV`,
`NG_IF_X64_AVXV`, `NG_IF_A64_NEONV`, `NG_IF_RV_VV`); `FID_NAT_OFF=isel`
gives the hand-written `ng_vbin_hw`, and `nat_vsel_check` (run by
`nat_sel_check` under `FID_VERIFY=1`) compares the two byte for byte.

`MO_VZMASK` works on registers only: a legacy SSE compare with a memory
operand faults when the address is not 16-byte aligned, and a Vec's
buffer need not be.

## The vector class

Four registers, named in an operation by their index 0..3 (`ng_vphys`):

| Target            | Registers     | Why                                                     |
|-------------------|---------------|---------------------------------------------------------|
| x86-64 (System V) | xmm/ymm 12..15 | MI25's float pool is xmm2..xmm11, the scratch xmm0/xmm1 |
| `--nat` Windows   | xmm/ymm 2..5  | xmm6..15 are callee-saved there; used only while the float pool is empty (fregs off), else the intrinsics stay calls |
| AArch64           | v28..v31      | the pool is v16..v27, the scratch v0/v1                 |
| RISC-V            | v1..v4        | v0 is the mask; V registers are a file of their own     |

None of them is in the float pool or the scratch pair, so a float value can
stay in its register across vector code and float operations can come
between vector ones (`ng_vec_class_check`, part of the MIR verifier, says
so when they overlap). The class has no allocatable pool: a vector value
lives inside one IR instruction's lowering, from the load that fills its
register to the store or lane move that empties it, with no call between
(every vector register is caller-saved). The allocator never sees vector
registers; spill code only touches general registers.

### Vector values in frame slots, and calls (TG11)

A vector value that needs a register when none is free, or that must
outlive a call, goes through a frame slot: `MO_VST` into the slot's address
(`MO_SLOTADDR`) and `MO_VLD` back. The OP22 vectorizer
(docs/reference/loop-vectorization.md) does this when a loop needs more
vector values at once than the four registers: `ng_vz_regs` is run again
over up to `NG_VZ_HOMES` (16) vector homes, `ng_vz_pins` keeps the
accumulator in register 2 and the first splats in what is left of 2 and 3,
and every other home is a 16- or 32-byte slot (`ng_vz_vslot`). Each
operation then loads its operands into registers 0 and 1 (`ng_vz_vget`;
a pinned home is read where it is), computes into register 0 and stores the
result back (`ng_vz_vput`). The slots lie past the function's locals:
`ng_gen_function` notes where its locals end (`g_vz_frame0`), and its
record's `base_frame` grows by the slots' bytes (`ng_vz_frame_end`), so the
allocator's spill slots start after them. `FID_NAT_OFF=vzspill`: such a loop
stays scalar, as before. The `[vectorize]` line of a loop that spills ends
in `spilled to N vector slots`.

No vector register is in the allocator's pools, and every one is
caller-saved, so a vector value in a register may not be live across a
call. `FID_VERIFY=1` checks it: `ng_vec_live_check` (part of
`ng_mir_verify`) is a forward must-analysis over the machine blocks of
which class registers hold a value (16 or 32 bytes); a call empties them
all, a 32-byte `MO_VEND` (VZEROUPPER) keeps the low 16 bytes, the zeroing
idiom (`vbin xor d, d`) reads nothing. A read of a register that some path
leaves empty is a verifier error. `nat_vlive_unit_check` (run by
`nat_sel_check`) tries it on small sequences: a value across a call, a
path that skips the write and a 32-byte read after `MO_VEND` are caught; a
value through a slot around a call passes.

## What compiles to it

`ng_simd_call` (called from `ng_gen_call`) lowers `p_simd_add`, `_sub`,
`_mul`, `_div`, `_scale` and `_dot` (the simd module's `add`, `sub`, `mul`,
`div`, `scale`, `dot`) to a vector loop of w / 8 lanes and a scalar loop
for the rest, when both Vecs hold 8-byte elements and `b` is at least as
long as `a`; anything else calls the runtime as before. The result is
built as core's `Vec_filled_typed` builds one (`Vec_new` +
`vec_buf_alloc`, len = cap = n). The bits equal the elementwise loops of
the interpreter, the C runtime and `pert_simd_*`: `dot` adds its products
in order, `div` keeps `x / 0 is 0`. Not used for `@constant_time`
functions, `--size` builds, RISC-V without V, or with `FID_NAT_OFF=simd`.

Integer lanes (OP21): the simd module's `add_i64`, `sub_i64`, `and_i64`,
`or_i64` and `xor_i64` over Vec<i64> call Fides loops `simd_lanes_*_i64`
(`@wrapping`: add and sub wrap at 64 bits, as the lanes do), which `fid
run`, the C path and pe_ir run as written. `ng_simd_call` mode 3 turns a
call of one of them into the same vector loop with NVK_ADD64, NVK_SUB64,
NVK_AND, NVK_OR or NVK_XOR (paddq / vpaddq ymm, add .2d / eor .16b,
vadd.vv / vxor.vv) and an integer scalar tail, only when both arguments
and the result are typed Vec<i64> (a function of that name over other
types stays a call); the slow path calls the Fides function, so a shorter
`b` panics there. The names start with `simd_lanes_`, not `p_`: a `p_`
name is a runtime builtin to the interpreter (`p_simd_` prefix) and to the
lowering (no `__env` argument).

RISC-V needs `--cpu rva23u64` or `--cpu rv64gcv` (new name); qemu-user runs
it with `-cpu rv64,v=true,vlen=128`.

## Checks

- `bash tests/nattools/simd_check.sh $FID`: `tests/bootstrap/test_machine_simd.fd`
  on run / ccompile / emit for SSE2, AVX2, NEON, RVV and `--nat`, with
  `FID_VERIFY=1`, the MIR dump showing the vector operations,
  `FID_NAT_OFF=simd` and `FID_NAT_ON=fregs`, the disassembly, and the
  encoder oracles.
- `bash tests/nattools/simd_i64_check.sh $FID`:
  `tests/bootstrap/test_simd_i64.fd` (the integer lanes) the same way.
- `bash tests/nattools/vectorize_spill_check.sh $FID`:
  `tests/bootstrap/test_vectorize_spill.fd`, loops that need five to
  eighteen vector values at once, through the vectorizer's spill slots.
- `python3 tests/nattools/enc-oracle-tg11.py --fid $FID`: the forms the
  vector operations added, against clang's assembler (113 forms, 0
  mismatches on Nov 16 with the stage-0 interpreter on this nat_enc.fd).

## Left

- f32 intrinsics (Vec<f32>): a Vec<f32> holds each element as an 8-byte
  word with the f64 bits of the value, and reading elements back is wrong
  today on `fid run` and the C path (`v.get(0) + v.get(1)` gives 0), so
  there is no f32 lane layout to vectorize yet. Integer multiply has no
  64-bit lane form below AVX-512 (`vpmullq`) and is not offered.
- `sum` stays a call (an in-order sum gains nothing from two lanes).
- The allocator itself has no vector class: vector homes are chosen by the
  code that makes vector operations (the vectorizer's `ng_vz_regs` /
  `ng_vz_pins`, `ng_simd_call`'s fixed registers), and no producer holds a
  vector value across a call today (the slots make it possible; the
  liveness check would catch one held in a register).
- Checks still to run on a built compiler: `bash
  tests/nattools/vectorize_spill_check.sh $FID` (the spill slots, the
  liveness check under FID_VERIFY=1) besides the ones above.
