# image_writer.fd — one writer of executable and object files

> **New doc 2026-11-21.** Read from the source; the status is the Nov 21
> check of llvm-roadmap TG13. Design: docs/image-writer.md.

- **Path:** `compiler/image_writer.fd`
- **LOC:** 968 (2026-11-21). In git since Nov 19 (49fb2b9).
- **Role:** llvm-roadmap TG13. The records of ELF, PE/COFF and Mach-O
  written once: bytes (`img_u8`..`img_u64`, big-endian `img_be_*` for the
  Mach-O code signature and fat header, `img_uleb`, padding and
  alignment), symbols (`ImgSym`, `img_func_syms`), string tables, ELF
  headers, program headers, symbol tables and relocations, PE optional
  headers, data directories and sections, Mach-O load commands. It knows
  nothing about the machine layer or pe_ir; the format writers are thin
  layouts over it. Bytes are i64 in 0..255.
- **Imports:** `core`, `vec`, `string`.

## Callers

| File | Writers over it |
|------|-----------------|
| `nat_enc.fd` | `nat_elf`, `nat_elf_debug`, `nat_elf_object` (ELF), `nat_pe`, `nat_pe_symbols` (PE32+, COFF symbols), `nat_macho`, `nat_macho_fat` (Mach-O) |
| `pe_ir_enc.fd` | `pe_build_file`, `pe_build_efi`, `pe_efi_relocs`, `pe_append_coff_symbols_pre`, `pe_put_debug_section` |
| `elf64.fd` | `ElfEmitter_build_elf` (ELF relocatable object; only `fid build-x86` reaches it since MI18) |
| `linker.fd` | `Linker_build_executable` (ELF executable; `fid build-x86`) |
| `coff.fd`, `macho.fd` | `CoffEmitter_build_coff`, `MachOEmitter_build_macho` (no command reaches either since MI18) |
| `main.fd` | imported for load order only (main.fd header, release A4) |

## What changed in the images

The images are byte-identical to what each writer wrote before, except
the two objects of the old assembler path, whose old bodies wrote broken
files: `ElfEmitter_build_elf` (56-byte section headers without `sh_addr`,
the halves of `r_info` swapped, symbol indexes off by the null symbol,
`.rodata` labels undefined: known-bugs 2g.16, fixed by this) and
`MachOEmitter_build_macho` (88-byte section headers, `__const` symbols at
their offset). The old bodies stayed behind `FID_IMG_OLD=1` until Nov 17;
they are gone, and tests/nattools/image_writer_unit.fd and
image_writer_obj_unit.fd pin each writer's bytes by digest.

## Status (Nov 21 check, TG13: validated)

No old writer body is left; both unit tests match their bytes;
image_same_check on hello: 11 formats the same (`pe-g` differs only in the
PDB path it names). The COFF object format is skipped there: `fid codegen
--coff` fails (2g.110, below).

## Findings

- **LOW — stale header.** The header still names "the assembler path
  (`fid codegen --elf|--coff|--macho`, build_executable)" as the user of
  the elf64/coff/macho writers. Since MI18 (Nov 17) `build_executable` is
  gone and `fid codegen` builds through the machine layer (nat_enc.fd's
  object writers); only `build-x86` still reaches elf64.fd and linker.fd.
- Not this module: `fid codegen --coff` and `emit --obj --nat` fail on
  every program with `undefined symbol: FreeLibrary` (the runtime's
  kernel32 imports are not resolved in object mode; known-bugs 2g.110).
