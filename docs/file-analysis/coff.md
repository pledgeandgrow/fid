# coff.fd — COFF (.obj) writer for Windows AMD64

Re-audited 2026-11-09 (previous audit 2026-10-22/23).

- **Path:** `compiler/coff.fd`
- **LOC:** 426
- **Role:** format-only writer. `emit_coff_file` assembles the asm with the
  shared `ElfEmitter` (elf64.fd), `CoffEmitter_import` copies its text,
  rodata, symbols and relocations, `CoffEmitter_build_coff` writes the COFF
  header, `.text` and `.rdata` sections, relocations, symbol and string
  tables. Since 2026-10-23 it has no assembler of its own.

## Where it is called

| Caller | What |
|--------|------|
| `main.fd` `cmd_codegen` (`use compiler::coff`) | `emit_coff_file(0, asm)` |

Only command: `fid codegen f.fd [--target x86-64] --coff out.obj`
(asm from codegen_x64). `pe.fd` / `pe_ir.fd` do not use it.

## Findings

### Verified live (2026-11-09, `s18.exe`)

1. **INFO — the object is valid.** objdump (WSL) reads `p2.obj` as
   pe-x86-64 with `.text` (0x204e6 bytes) and `.rdata`; `main` is an
   external symbol and disassembles correctly. So the old F1-F4 (NOP-sled
   encoder, mnemonic mismatch, STATIC storage class) stay fixed.
2. **LOW (new) — slow.** `codegen --coff` on a 4-line program took about
   60 s more than `--elf`: symbol lookups (`coff_sym_index`,
   `symbol_name_map_lookup` over a Vec of tuples) are linear per
   relocation and per name.

### Code inspection

3. **F5 `emit_coff_file(module: i64, ...)` — STILL TRUE**, cosmetic.
4. **LOW (new) — no relocation overflow.** `NumberOfRelocations` is a u16
   (:296); more than 65,535 relocations would need
   `IMAGE_SCN_LNK_NRELOC_OVFL`. Not handled.
5. **LOW (new) — addends dropped.** REL32 works because COFF's implicit
   -4 matches elf64's addend; ADDR64 and any other addend are lost (the
   field is written as 0).

## Fix order

1. Hash maps for symbol lookups (2).
2. Relocation overflow and addends (4, 5) if the output is ever linked.

## Verification state

- Probe: `codegen p2.fd --coff p2.obj`, `objdump -h/-t/-d` in WSL.
- Read: import and build functions in full.
