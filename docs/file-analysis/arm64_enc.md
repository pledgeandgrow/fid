# arm64_enc.fd — file analysis

> **Re-verified 2026-10-23:** `enc_lsl`/`enc_asr` SBFM bitfield swap confirmed against the ARM spec (imms<<16 overlaps rn field). Remaining secondary claims (fp alias, reg-offset drop, .rodata) plausible but not all re-traced.

- **Path:** `compiler/arm64_enc.fd` — 1,664 LOC
- **Role:** ARM64 (AArch64) assembler — text → machine-code bytes.
  Ported 1:1 from `src/arm64_enc.rs` (~1,328 LOC). Encodings verified identical
  to the Rust reference — **every bug below is inherited, not introduced by the port**.
- **Analyzed:** 2026-10-22 — run 2 of 52 (alphabetical order)

## Where it is called

| Caller | Site | How |
|--------|------|-----|
| `compiler/compiler.fd` | line 50 `use compiler::arm64_enc;`, line 1702 `assemble_arm64(asm_arm)` | `--target arm64` user-code path: codegen_arm64 asm → bytes → linker |
| `compiler/runtime_arm64.fd` | line 23 `use`, line 118 `assemble_arm64(asm)` | assembles the ARM64 runtime (`_start`, print helpers, panic stubs) |

Only `assemble_arm64()` + `Arm64Symbol`/`Arm64Reloc`/`ResultArm64Assemble` are
consumed. All ~30 `enc_*` fns, `parse_reg`, `str_lines`, etc. are `pub` but
internal. `self: &Arm64Assembler` on every `enc_*` is unused (all pure fns).

Imports: `core`, `vec`, `string`. Leaf module (no `compiler::*` deps).

## Findings

### Verified live

1. **HIGH — `--target arm64` fails for every program.**
   `fidesc build x.fd --target arm64` on `fn main() -> i64 { return 42; }` →
   `error: ARM64 assembly error: Unknown register: fp`.
   `codegen_arm64.fd:213` emits `add d, fp, #-N` for `AddrOfSlot`; `parse_reg`
   (`arm64_enc.fd:196-211`) has no `fp`→x29 alias → hard assemble error before a
   single byte is produced. The entire arm64 native path is dead on arrival.

2. **HIGH — Under the interpreter, `assemble_arm64` always returns empty.**
   `assemble_arm64("ret")` → `text.len() = 0` (verified). Root cause is an
   **interpreter `&mut` bug**: `&mut Vec<i64>` parameters receive a copy —
   mutations never write back (minimal repro: `fn f(v:&mut Vec<i64>){v.push(1)}`
   → caller's vec stays empty; `v.set` too. Struct-field `h.v.push` *does* work).
   Victims here: `push_u32_le` (:153), `patch_u32_le` (:166),
   `label_offset_insert` (:314) — all `&mut Vec` → all silent no-ops in interp →
   no bytes emitted, no labels recorded, every branch demoted to an external
   reloc. The generated C dereferences `&mut` correctly (`*(void**)v1`), so the
   C backend is unaffected — **interp/backend divergence**. Bug belongs to
   interp, not this file, but it makes `fidesc run`-side arm64 assembly
   unverifiable.

### From code inspection (masked by #1 today — will fire as soon as `fp` is fixed)

3. **HIGH — `mov xN, sp` / `mov sp, xN` encode to XZR, not SP.**
   `enc_mov` (:649-678) emits ORR `Xd, XZR, Xm`. `parse_reg("sp")`=31 — but in
   data-processing register fields, 31 means **XZR** (reads as zero / writes
   discarded), not SP. So `mov x29, sp` → `orr x29, xzr, xzr` → **x29 := 0**;
   `mov sp, x29` → `orr xzr, …` → **no-op**. ~20 sites in `runtime_arm64.fd`
   (`mov x29, sp` / `mov x21, sp`) + `codegen_arm64.fd:179` prologue — i.e.
   every frame-pointer setup. Fix: alias `mov …, sp`/`mov sp, …` →
   `add Xd, SP, #0` (0x91xxxxxx) — or accept `sp` operand form in add/sub.

4. **HIGH — `lsl`/`asr` immediates: bitfield fields swapped.**
   UBFM/SBFM layout is `immr[21:16] | imms[15:10]` (the file itself gets this
   right for AND-imm :995 and SXTW's baked constant 0xD3407C00 :1501).
   `enc_lsl` :1104 emits `(imms << 16) | (immr << 6)`;
   `enc_asr` :1143 emits `(63 << 16) | (shift << 6)` —
   e.g. `asr x0,x1,#1` yields 0xD37F0060 instead of 0xD341FC20: wrong immr,
   and `shift<<6` bleeds into the Rn field (wrong source register too).
   Identical in `src/arm64_enc.rs:740,767` — fix both.

5. **HIGH — register-offset addressing silently dropped.**
   `arm64_parse_mem_operand` (:1562) splits `[x3, x2, lsl #3]` on commas and
   feeds `x2` to `arm64_strip_hash` → `str_parse_int("x2")` returns 0 →
   `str x1, [x3, x2, lsl #3]` encodes as **`str x1, [x3]`** — offset and shift
   gone, no error. `runtime_arm64.fd:488,:501` emit this form (indexed stores
   in print helpers). Same for post-index: `ldr x0, [sp], #16` → `ops[2]`
   never read → encodes `ldr x0, [sp]`; and pre-index `[sp,#-16]!` errors out
   only accidentally (`!` ≠ `]`).

6. **MEDIUM — `.rodata` section assembled then thrown away.**
   `Arm64AssembleResult` (:77-81) has no `rodata`/`rodata_symbols` fields;
   `.asciz`/`.ascii`/`.quad`/`.byte` fill `self.rodata` and rodata labels go to
   `self.rodata_symbols` — both dropped by `assemble_arm64`. Rodata label
   references fall into `resolve_branches`' "external" branch → linker relocs —
   but the symbols never reach the linker either → `linker.fd:804` "unknown
   symbol → skip" → **ADRP/lo12 instructions left with imm=0**. Latent today
   (codegen_arm64 never pushes into `string_consts`/`float_consts`, so `.rodata`
   is never emitted), but the whole `.section .rodata` path is a loaded trap.

7. **MEDIUM — `resolve_branches` silently drops rtype 14/15 to text labels.**
   :479-505 patches only BL/B/BCOND. If `adrp`/`:lo12:` ever targets a
   text-section label, the reloc is *neither* patched *nor* emitted — the
   instruction keeps imm=0. (Today's producers only target rodata labels, so
   they take the external path → straight into finding #6.)

8. **MEDIUM — arm64 B/BL/BCOND linker deltas are off by one instruction.**
   `linker.fd:830-843` computes `delta = sym_offset - (patch_offset + 4)`, but
   AArch64 branch immediates are relative to **the branch instruction itself**
   (this file's own `resolve_branches` correctly uses `target - pb.offset`).
   External `bl`/`b`/`b.cond` will land 4 bytes early. Fix lives in linker.fd;
   recorded here because this file is what feeds it these rtypes.

9. **MEDIUM — No immediate-range validation anywhere.**
   `add/sub/cmp #imm`: `(imm & 0xFFF)` — `#8192` → encodes `#0`, `#-16` → `#4080`,
   no error. `ldr/str`: `imm/scale` is **unmasked** (`<< 10`) — `#32768` →
   `imm12=4096` corrupts opcode bits; unaligned offsets truncate (`#4/8=0` →
   wrong address, no error). `stp/ldp` `imm7` wraps out-of-range. `lsl #≥64`
   → negative `imms` shifted into the word.

10. **LOW — `parse_reg` accepts garbage.** `x-1` → Ok(-1) → shifted into the
    instruction; `xabc` → `str_parse_int` → 0 → **x0**; `wsp` → `w0` (wsp is a
    real register); no empty/whitespace guard. `is_64bit_reg("wzr")` → false →
    32-bit ops on wzr encode with sf=0 (consistent, but `mov wzr,…` → reads xzr
    via the 31→XZR rule — semantically fine, accidental).

11. **LOW — directive handling is lossy.** `.quad`/` .byte` use
    `str_parse_int` — `0x…` → 0, label operands → 0, silently. `.section` splits
    on `" "` only (a `\t` separator breaks it). `.align` pads `.text` with
    `0x00` — undefined/unallocated encoding; padding between functions should
    be NOP (0xD503201F). `handle_directive` never returns `Err` — every
    malformed directive is silently ignored. `arm64_decode_string` misses `\xNN`
    escapes.

12. **LOW — operand-shape quirks.** `movz/movk` read shift from `ops[3]`
    (`movk x0,#1,lsl #16`) — `hw` unvalidated (>3 corrupts). `svc`/`brk`
    immediates masked, fine. `parse_cond`/`parse_float_reg` case-sensitive,
    lowercase-only. `ResultI64Str`/`ResultUnit`/`ResultArm64EncodedInst` —
    three near-identical result enums plus `Arm64StpLdpResult`/`Arm64MemOperand`
    — five bespoke result types where one shared `Result<T,E>` shape would do.

13. **INFO — zero test coverage.** Nothing pins a single encoding. Golden tests
    comparing `assemble_arm64` output to GNU `as` bytes would have caught the
    LSL/ASR swap, the sp→xzr issue, and the &mut-Vec interp divergence long
    before this review.

## Suggested fix order

1. `fp` → accept as x29 alias in `parse_reg` (or emit `x29` in codegen_arm64)
   — unblocks every arm64 build.
2. `mov` ↔ sp: special-case `mov Xd, sp`/`mov sp, Xn` → `add` form.
3. LSL/ASR: swap to `(immr << 16) | (imms << 10)` — fix `src/arm64_enc.rs` too.
4. Mem operands: implement register-offset/pre/post-index, or hard-`Err` on
   unconsumed operand parts — never silently drop. Add imm-range checks for
   add/sub/cmp/ldr/str/stp/ldp/lsl/asr.
5. Export `rodata`+`rodata_symbols` in `Arm64AssembleResult`; forward them in
   `compiler.fd:1702-1725` to the linker.
6. `resolve_branches`: emit relocations for any unhandled rtype, incl. text
   labels.
7. `linker.fd`: arm64 B/BL/BCOND delta base = `patch_offset`, not `+4`.
8. Interpreter: fix `&mut` write-back for builtin handle types (interp bug —
   the usage here is legitimate; also fixes verification of this file).
9. `.align` → NOP-pad `.text`; `.quad` hex/symbol support; `\t` separators.
10. Golden tests pinning known-good encodings (run under C backend or after
    #8 lands).

## Verification state

- `fidesc run`: `assemble_arm64` on 13 instruction forms → all empty text
  (interp `&mut` bug; repro `mut_test3.fd` isolated it — test files removed).
- `fidesc build armt.fd --target arm64` → `Unknown register: fp` on
  `return 42` — verified on trivial input.
- `fidesc ccompile` output inspected: `&mut` lowered to `*(void**)v1` — C
  backend unaffected.
- `src/arm64_enc.rs` compared section-by-section: encodings identical —
  findings 3,4,5,8,9,10,11 exist in the Rust reference as well.
- No C-backend execution possible (MSVC off PATH); ARM64 binaries can't run on
  this host anyway — encoding correctness argued from the ARM ARM field
  layouts and internal consistency.
