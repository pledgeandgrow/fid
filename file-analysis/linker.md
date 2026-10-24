# File Analysis: compiler/linker.fd

> **Re-verified 2026-10-23:** CRITICAL confirmed — `build_executable` writes all 9 u64 header fields via `write_u64(buf, buf.len())` at :998-1034; `buf` is only ever `push`ed (never pre-extended), so `set(len)` is out-of-bounds → interp panic / codegen_c silent drop → emitted ELF has no e_entry/phdr contents. Highest-blast-radius bug in the repo.

**Run:** 29/52 | **LOC:** 1,051 (src/linker.rs: 968) | **Status:** LIVE — `build` path for x86-64/arm64/riscv

## Where it's called

`compiler.fd:1722` — `link_with_runtime(objects, "_start", rt.text, rt_syms, rt.rodata, rt_rodata_syms, rt_relocs)` on the **x86-64 ELF build path**; `_for_target` variants serve arm64/riscv. Takes elf64's emitted object + runtime_linux's raw text/syms/relocs → final executable bytes.

## Findings

### CRITICAL — `build_executable` emits a malformed ELF: all 9 u64 fields are dropped

Every 64-bit header/program-header field is written via `write_u64(buf, buf.len(), val)` (:998-1034) — and `write_u64` is implemented with **`buf.set(off+i, …)`** (:205-211). `Vec.set` at `off == len` does **not grow**:

- Under interp — **verified live**: `v.set(v.len(), x)` → `Panic: Vec index out of bounds` → `Linker_link` crashes.
- Under codegen_c — `p_vec_set` is `if (idx >= v->len) return;` (codegen_c.fd:3329) → **silently drops every byte**.

Dropped fields: `e_entry`, `e_phoff`, `e_shoff`, `p_offset`, `p_vaddr`, `p_paddr`, `p_filesz`, `p_memsz`, `p_align`. The Rust original appends via `extend_from_slice` (linker.rs:851-862). The port translated appends into `write_u64(buf, buf.len(), …)` — a pattern that can never emit a single byte under either backend. Result: ELF header shrinks from 64→~40 bytes, the 56-byte phdr becomes 8 (only `p_type`+`p_flags` pushed as u32s), .text lands at ~offset 48. **The produced executable has no entry point, no program header contents — it cannot run.** `fides-c build --target x86-64` output is structurally invalid end-to-end (independent of elf64's own bugs — even a perfect object links to garbage).

Fix: `write_u64`/`write_u32` need an append mode (`push` per byte) or `buf.set` must grow — same class as the run-2 `Vec::set` boundary.

### MEDIUM (inherited) — ARM64 branch relocs are off by 4

`R_ARM64_BL/B/BCOND` compute `delta = sym_offset - (patch_offset + 4)` (:830/839). The AArch64 BL/B.cond immediate is relative to the **branch instruction's own address** (`target = PC_of_inst + imm*4`), so `next_pc` should be `patch_offset` — the `+4` yields a target of `S-4` — every call/branch lands one instruction early. RISC-V JAL/BRANCH correctly use `patch_offset` (:872/887). Inherited verbatim from src/linker.rs:643/657/670 — wrong upstream too, but the arm64 path already fails earlier (run 2).

### LOW — RELA addends parsed then discarded

`r_addend` read (:671/:706) but `LinkerReloc{offset,symbol,rtype}` has no addend field. Works **only** because elf64 always emits `-4` and the PC32 formula `S-(P+4)` exactly encodes `S+(-4)-P`. Any future nonzero/other addend mislinks silently.

### LOW — `dead_code_elimination` is cosmetic overhead

Real BFS reachability (:413-501, O(symbols²·relocs) nested linear scans) but **removes no bytes** — its output only gates the reloc-patch loop (:786-796). Rodata/vtable edges aren't in the graph (harmless only because `.rela.rodata` patching :919-950 is a separate unconditional pass). `entry` missing → `entry_offset=0` → silent wrong entry.

## Correct pieces (verified against Rust)

- ELF object parsing (sections/symtab/strtab/.rela.text+`.rela.rodata` — **linker-side rodata-reloc support is complete**, matching run-17's "elf64 never emits it" — the linker's ready for the fix).
- PC32/PLT32/R_X86_64_64 patch math; R_ARM64_ADRP/ADD_LO12 vaddr math consistent with the `0x400078` layout; RISC-V JAL/B-type bit-packing correct.
- `build_executable` *layout intent* is right (text at file-off 120 = vaddr `0x400078`, rodata appended) — it just never writes the u64s.

## Verdict

The linker's *logic* is a faithful port — relocation constants match (`PC32=2/PLT32=4/64=1`, ARM64 internal codes 10-15, RISC-V 20-21), object parsing is solid, rodata-reloc handling is complete. But **the executable writer is broken by one idiom**: `write_u64(buf, buf.len(), …)` → `Vec.set(len)` → panic(interp)/silent-drop(C). Until that's fixed to append, `build` on x86-64 emits a headerless stub — which is why the working matrix is interp+C+PE only.
