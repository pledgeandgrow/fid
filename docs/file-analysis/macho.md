# macho.fd — Mach-O 64-bit object writer (x86-64)

> **Nov 21: dead.** Nothing imports it. Since MI18 (Nov 17) `fid codegen
> --macho` builds a Mach-O object through the machine layer (nat_enc.fd,
> the macOS target of the architecture). `MachOEmitter_build_macho` now
> writes through image_writer.fd (`MachOEmitter_build_macho_img`, TG13;
> the old body wrote 88-byte section headers). Native macOS executables
> come from the machine layer (`--target x86_64-macos|aarch64-macos`,
> `nat_macho`). The callers below are as of Nov 9.

Re-audited 2026-11-09 (previous audit 2026-10-22/23).

- **Path:** `compiler/macho.fd`
- **LOC:** 363 (2026-11-21; 445 on Nov 9)
- **Role:** format-only writer: assembles via the shared `ElfEmitter`
  (elf64.fd) and translates to Mach-O (`__TEXT,__text`, `__TEXT,__const`,
  per-section relocations with BRANCH / SIGNED / UNSIGNED types, nlist_64
  symbols, string table). x86-64 only (`CPU_TYPE_X86_64`).

## Where it is called

| Caller | What |
|--------|------|
| `main.fd` `cmd_codegen` (`use compiler::macho`) | `emit_macho_file(0, asm)` |

Only command: `fid codegen f.fd [--target x86-64] --macho out.o`.
Mach-O executables for macOS come from the C path (`ccompile --target
x86_64-macos|aarch64-macos`), not from here.

## Findings

### Verified live (2026-11-09, `s18.exe`)

1. **INFO — header looks right.** `codegen p2.fd --macho` wrote 326,023
   bytes; the first bytes are MH_MAGIC_64, CPU_TYPE_X86_64, subtype 3,
   MH_OBJECT, 2 load commands, LC_SEGMENT_64 with `__text` size 0x204e6
   (same as the ELF text). Not linked or loaded: the WSL objdump has no
   Mach-O support and there is no ld64 here.

### Code inspection

2. **Old toy encoder findings — FIXED by deletion** (2026-10-23), still
   absent.
3. **INFO — no arm64 Mach-O.** Apple Silicon objects would need the arm64
   assembler path.
4. **LOW — `emit_macho_file(module: i64, ...)`** dummy parameter, as coff.

## Fix order

None needed unless the objects are to be linked; then test with ld64.

## Verification state

- Probe: header bytes of the output file.
- Read: header, constants, function list, import.
