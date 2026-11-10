# elf64.fd — AT&T x86-64 assembler and ELF64 object writer

Re-audited 2026-11-09 (previous audit 2026-10-22/23).

- **Path:** `compiler/elf64.fd`
- **LOC:** 2,780
- **Role:** the assembler of the old PIR-era backends: AT&T x86-64 text
  → machine code, symbols and relocations (`ElfEmitter_assemble`), then an
  ELF64 relocatable object (`ElfEmitter_build_elf`). Also `encode_asm_block`
  (runtime_linux's asm part) and `dump_symbols`. Port of `src/elf64.rs`.
  The new native backend (`nat_ir.fd` / `nat_enc.fd`) does not use it.

## Where it is called

| Caller | What |
|--------|------|
| `compiler.fd` `build_executable` (x86-64) | `emit_object_file(0, asm)` |
| `compiler.fd` `build_x86_linux` | `emit_object_file(0, asm)` |
| `main.fd` `cmd_codegen` | `emit_object_file` (`--elf`), `emit_object_file_for_target` (arm64/riscv `--elf`) |
| `pe.fd` `pe_build` | `ElfEmitter_assemble` (PIR boot / flat / PE images) |
| `coff.fd`, `macho.fd` | `ElfEmitter_assemble`, then their own writers |
| `runtime_linux.fd` | `encode_asm_block(runtime_asm)` |

CLI commands that reach it: `fid codegen f.fd [--target x86-64] --elf|--coff|--macho <o>`,
`fid codegen f.fd --target arm64|riscv --elf <o>`, `fid build-x86`,
`fid build --native` (default target `x86-64` only), `fid emit --boot`.

## Findings

### Verified live (2026-11-09, `s18.exe`, scratch files)

1. **HIGH (new) — every section header is 56 bytes, not 64.**
   `ElfEmitter_build_elf` (:2497-2537) writes name, type, flags, then the
   offset: `sh_addr` is missing on all headers, the null one included.
   `e_shentsize` says 64. `codegen p.fd --elf p.o` wrote 465,569 bytes with
   `e_shoff` 465,121: readelf says the headers run past the end of file,
   objdump says "file format not recognized". The Rust original writes
   `sh_addr: 0`. Consequence: `linker.fd` reads 64-byte headers, gets
   garbage offsets, finds no `.text` or symbols. So:
   - `fid build-x86` writes a 120-byte executable (header + one empty
     segment) that segfaults;
   - `fid build --native --no-opt` fails "Undefined symbols: main"
     (only the runtime is linked).
   Fix: emit `emit_u64(&mut buf, 0)` for `sh_addr` after `sh_flags` in
   all nine header writers.
2. **MEDIUM (was F6, still true) — arm64 / riscv `--elf` objects are
   empty.** `emit_object_file_for_target` (:325-350) leaves `.text` empty
   and stores the asm in `emitter.comment`, which `build_elf` never writes.
   `codegen --target arm64 --elf` wrote a 603-byte object with no code
   and no text.
3. **INFO — the assembler itself encodes correctly enough for COFF.** The
   same `ElfEmitter` output, written by `coff.fd`, loads in objdump and
   disassembles `main` correctly (push/mov/sub, canary xor, call relocs).

### Code inspection (old findings re-checked)

4. **F1 negative displacements — FIXED.** The pack is still
   `offset * 256 + base` (:640) and `offset * 4096 + ...` (:687), but all
   decoders use `floor_div` / `floor_mod` (71 uses, :596-604).
5. **F2 `.rela.rodata` never emitted — FIXED.** Written as section 7
   (:2466-2476, :2518-2519). `.rela.text` link/info are 3/1.
6. **F3 unknown instructions → NOP — CHANGED.** `jo`/`jno` (:1042),
   `movapd`/`movupd` (:1130), `*pd` arithmetic, `ud2` are encoded now. The
   fallback NOP is still there for anything else (:1194, :1507, :1630,
   :1770, :2003, :2138) — silent, LOW.
7. **F4 hex immediates — FIXED** (`parse_dollar_imm`). Note the canary
   `xorq $0x5DEADBEEFCAFEBABE` is encoded as imm32 `0xCAFEBABE`
   sign-extended (x86 has no xor imm64); prologue and epilogue agree, so it
   works as a weaker canary (INFO).
8. **F5 custom-section names after shstrtab is written — STILL TRUE,
   wider.** The `.shstrtab` bytes are copied at :2451, then
   `ElfEmitter_add_shstrtab` is called for every header name (:2499-2524).
   Built-in names are pre-seeded in `ElfEmitter_new`, so only custom
   sections get offsets past the table. No caller adds custom sections.
9. **LOW — `encode_asm_block` hides assembler errors** (:2564-2570): an
   `Err` returns empty bytes, so runtime_linux links without its asm part.
10. **F6 rest:** `.align`/`.section`/`.globl`/`.type` ignored — not
    re-checked. `encode_jcc` local-label assumption — not re-checked.

## Fix order

1. Add `sh_addr` to every section header (1). This alone may revive
   `build-x86`, `build --native` and `codegen --elf`; then test them end to
   end in WSL.
2. Either encode arm64/riscv through `arm64_enc`/`riscv_enc` or make
   `--elf` for those targets an error (2).
3. Turn the NOP fallbacks and `encode_asm_block` errors into errors (6, 9).

## Verification state

- Probes: `s18.exe codegen p2.fd --elf` → readelf/objdump in WSL;
  `build-x86` output run in WSL (segfault, 120 bytes); `build --native
  --no-opt` → "Undefined symbols: main"; `codegen --target arm64 --elf`
  → 603-byte object.
- Read: emitter structs, `emit_object_file*`, memory-operand parsers,
  dispatch samples, `build_elf` in full, `encode_asm_block`.
