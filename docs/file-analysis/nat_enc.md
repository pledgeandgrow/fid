# nat_enc.fd — code buffer, encoders, ELF and PE writers of the native backend

Audited 2026-11-09 (new doc).

- **Path:** `compiler/nat_enc.fd`
- **LOC:** 1,369
- **Role:** the byte layer under `nat_ir.fd`: a code buffer with labels
  and fixups, AArch64, RISC-V 64 and x86-64 instruction encoders, a static
  ELF writer (with optional symbols and DWARF), and a PE32+ writer.

## Where it is called

| Caller | What |
|--------|------|
| `compiler/nat_ir.fd` (`use compiler::nat_enc`) | only importer: `NatEnc_*`, `a64_*`, `rv_*`, `x64_*`, `nat_elf`, `nat_elf_debug`, `nat_pe` |

Reached from the same commands as `nat_ir.fd`: `fid emit --target
aarch64-linux | riscv64-linux | x86_64-linux` and `fid emit --nat`.

## What it does

- **NatEnc**: `code` bytes, `labels` (offset or -1; label 0 never used),
  `fixups`. `NatEnc_resolve` patches every fixup once all labels are
  placed: A64 B26 / B19 (range-checked), ADRP+ADD, RV JAL (range-checked),
  RV AUIPC+I, x86-64 REL32, PE RVA32, absolute 64-bit (`NAT_IMAGE_BASE +
  offset`).
- **Encoders**: AArch64 integer, load/store, branches, FP, exclusive and
  acquire/release; RISC-V I/M/A/D subsets with far jumps; x86-64 integer,
  SSE2 scalar, `roundsd`, `vfmadd213sd`, lock prefixes, fs/gs loads.
  `scripts/enc-oracle.py` checks them against clang's assembler (the
  roadmap reports 1,181 forms, 0 mismatches; not re-run here).
- **`nat_elf`**: one PT_LOAD, RWX, the whole file at 0x400000, image at
  file offset 0x1000 (= `NAT_IMAGE_BASE` 0x401000), `p_align` 64 KB.
  EM_AARCH64, EM_RISCV (flags 4 = double-float ABI) or EM_X86_64.
- **`nat_elf_debug`**: the same image plus `.symtab`, `.strtab`,
  `.debug_abbrev`, `.debug_info` (DWARF 4, one CU, a subprogram per
  function), `.debug_frame` (CIE + an FDE per function from the fixed
  prologue), `.debug_line` (one file, rows from nat_ir), outside the load
  segment.
- **`nat_pe`**: PE32+, ImageBase 0x400000, image at RVA 0x1000 split into
  `.text` (RX) and `.data` (RW) at a page-aligned `data_off`; import
  directory and IAT inside the image; RELOCS_STRIPPED, no DYNAMIC_BASE
  (absolute fixups need the fixed base); NX_COMPAT; 8 MB stack reserve.

## Findings

### Verified live

1. **INFO — writers produce runnable images.** ELF files for x86-64 and
   AArch64 built by `release/fid.exe` ran in WSL / qemu-aarch64 with the
   expected output and exit codes (see nat_ir.md). PE output of `--nat`
   could not be produced by the probe binaries (nat_ir.md finding 2).

### Code inspection

2. **LOW — the Linux image is one RWX segment.** Code, strings and
   globals share a writable, executable mapping. Works; weak against
   code injection. The PE already splits RX and RW.
3. **LOW — some fixups have no range check.** ADRP+ADD (±4 GB), RV
   AUIPC+I (±2 GB) and x86-64 REL32 are patched without a check. Images
   are far smaller, so this is latent.
4. **LOW — stale comments.** The file header names only AArch64 and
   RISC-V; the debug-section comment says "Line tables wait for the
   lowering to record source lines", but `.debug_line` is written now.
5. **INFO — `x64_li(r, 0)` uses `xor`**, which clobbers flags. nat_ir never
   places it between a compare and its `setcc`/`jcc` in the code read
   (compares and their use are emitted together in `ng_setcc`,
   `ng_panic_if`), so no bug found.
6. **INFO — the FDE assumes the standard prologue at the function's
   first byte** (push rbp / mov rbp,rsp; stp x29,x30 / mov x29,sp; RISC-V
   four instructions). True for every MIR function today; it would be
   wrong for a frameless leaf (WP8) or if `ng_cold_last` moved the
   prologue (nat_ir.md finding 4).

## Fix order

1. Comments (4).
2. Split the Linux image into RX and RW segments like the PE (2).

## Verification state

- Read: buffer and resolve, every encoder header, ELF, PE and debug
  writers in full.
- Probes: ELF output run in WSL and qemu-aarch64 (nat_ir.md).
- Encoder oracle not re-run.
