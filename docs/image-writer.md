# One image writer (TG13)

Own-backend roadmap TG13 (docs/roadmap/llvm-roadmap.md): the executable and
object files of every backend are written by one module,
`compiler/image_writer.fd`. It knows the records of the three formats
(headers, program and section headers, symbols, relocations, string
tables, PE data directories and base relocations, Mach-O load commands and
dyld opcodes) and nothing about the machine layer, pe_ir or the assembler.
Each format writer is a layout over it: it says which records go where and
where the bytes come from.

| Writer | File | Format | Same bytes as the old body |
|---|---|---|---|
| `nat_elf` → `nat_elf_img` | nat_enc.fd | ELF executable (machine layer, Linux) | yes |
| `nat_elf_debug` | nat_enc.fd | ELF executable with .symtab and DWARF | yes |
| `nat_elf_object` → `nat_elf_object_img` | nat_enc.fd | ELF ET_REL (`emit --obj`, TG12) | yes |
| `nat_pe` → `nat_pe_img`, `nat_pe_symbols` | nat_enc.fd | PE32+ (`emit --nat`) | yes |
| `nat_macho` → `nat_macho_img`, `nat_macho_fat` | nat_enc.fd | Mach-O executable, fat binary | yes |
| `pe_build_file` → `pe_build_file_img` | pe_ir_enc.fd | PE32+ (pe_ir, the Windows default) | yes |
| `pe_build_efi` → `pe_build_efi_img`, `pe_efi_relocs` | pe_ir_enc.fd | UEFI application | yes |
| `pe_append_coff_symbols_pre`, `pe_put_debug_section` | pe_ir_enc.fd | COFF symbols, DWARF sections of a PE | yes |
| `CoffEmitter_build_coff` → `CoffEmitter_build_coff_img` | coff.fd | COFF object (`codegen --coff`) | yes |
| `Linker_build_executable` → `Linker_build_executable_img` | linker.fd | ELF executable of the old PIR path | yes |
| `ElfEmitter_build_elf` → `ElfEmitter_build_elf_img` | elf64.fd | ELF ET_REL (`codegen --elf`) | no: fixed |
| `MachOEmitter_build_macho` → `MachOEmitter_build_macho_img` | macho.fd | Mach-O MH_OBJECT (`codegen --macho`) | no: fixed |

## The old bodies are gone (Nov 17)

Until Nov 17 every writer kept its old body behind `img_old()`
(`FID_IMG_OLD=1`), and the unit tests ran each writer both ways on the
same input. On Nov 17 every one gave the same bytes as its old body
(compiled from the tree with the stage-0 compiler: `fid ccompile` of
both unit tests), the debug ELF too with `FID_NAT_OFF=ehframe` (DB6's
`.eh_frame` and CT6's `.debug_loc` had only ever been added to the new
body, and TG15's dylibs other than libSystem only to `nat_macho_img`: the
old bodies had stopped being a reference). The old bodies, `img_old`,
`img_set_old` and `FID_IMG_OLD` were then removed, with the helpers only
they used (`nat_push_u16/32/64`, `nat_elf_phdr`, `nat_pe_section`,
`nat_shdr`, `nat_be_u32`, `nat_put_u64_be`, `nat_macho_seg/sect/name16`,
`nat_be32_push`, `nat_obj_sym/str`, pe_ir_enc's `pe_efi_section`, coff's
`symbol_name_map_lookup`, macho's `mo_emit_*`, `mo_sectname`,
`MachOEmitter_add_string`).

The unit tests now pin each writer's bytes: they compare a digest of each
output with the digest the old body's bytes had (`golden()` in each
test). A line that says `CHANGED: digest N` is a layout change: check the
new bytes with the system tools (below), then put N in `golden()`.

## Dynamically linked ELF with debug sections

`nat_elf_debug` (the default `fid emit --target <arch>-linux`) writes
section headers. For a dynamically linked image (CI2: PT_INTERP and
PT_DYNAMIC) they now also name those bytes: `.interp`, `.dynstr` (from
DT_STRTAB and DT_STRSZ) and `.dynamic` (SHT_DYNAMIC, `sh_link` the
`.dynstr`). Without them `readelf -S` says "no .dynamic section in the
dynamic segment"; the loader reads only the segments, so a program ran
either way. Unit test: `elf debug ldyn` lines of image_writer_unit.fd.

## Position-independent and dynamically linked (TG14)

A position-independent executable (`g_nat_pie`, ET_DYN) that also has
PT_INTERP starts its program headers with PT_PHDR (`nat_elf_img`):
glibc's ld.so computes the program's load bias as AT_PHDR minus PT_PHDR's
`p_vaddr`, and without PT_PHDR takes 0, so it would read the dynamic
section and fill the GOT at link addresses. `nat_elf_eh_frame` grows
PT_PHDR when it adds PT_GNU_EH_FRAME. The machine layer does not make such
an image yet: compiler/nat_ir.fd `ng_ldyn_ok` refuses `g_nat_pie`. Once it
does not, tests/nattools/pie_ldyn_prog.fd is the program to run, and
tests/nattools/elf_segments_check.py `--dyn` checks the PT_PHDR. The
loader's own tables are not in the program's self-relocation table
(`ng_gen_pie_table` runs before `ng_gen_ldyn`), so nothing is biased twice.

## The two writers that do not keep their bytes

The assembler path's ELF and Mach-O objects were broken, so their new
writers change the bytes on purpose.

`ElfEmitter_build_elf` (old body):
- section headers of 56 bytes (no `sh_addr`) while `e_shentsize` said 64:
  every tool, compiler/linker.fd included, read the wrong fields
- `r_info` written as `type << 32 | symbol` (halves swapped), the symbol
  counted without the null symbol
- `.symtab`'s `sh_info` 1 whatever the order, locals and globals mixed
- the `.rodata` labels (`.str_N`, `.flt_N`, `.vtable_*`) undefined in the
  symbol table, so a reference to a string could not be resolved
- an AArch64/RISC-V emitter's assembly text (`comment`) dropped

`ElfEmitter_build_elf_img`: 64-byte headers, `r_info = symbol << 32 |
type` with symbols counted from the null symbol, locals first (sh_info
the first global; relocations renumbered), `.rodata` labels defined in
section 2 (local), undefined symbols global, `.symtab`, relocations and
headers 8-byte aligned and `.rodata` 16, `.comment` for the AArch64 and
RISC-V emitters. Section indexes are unchanged (1 .text, 2 .rodata, ...,
7 .rela.rodata, then custom sections).

`MachOEmitter_build_macho` (old body): `section_64` headers of 88 bytes in
the wrong order (reloff and nreloc where `align` goes) while the segment
command said 80 each; a `__const` symbol's `n_value` its offset, not its
address. `MachOEmitter_build_macho_img`: 80-byte headers, the section data
together (`__const` at the next 8-byte address after `__text`), then the
relocations, the symbols and the strings; an unnamed rwx segment as
objects have it; LC_DYSYMTAB with the symbols in its order (locals,
defined externals, undefined; relocations renumbered); `__text` flagged
`S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS`.

The writers that keep their bytes keep what the old bodies did, odd
fields included (for example `Linker_build_executable`'s `e_shentsize` 0).
Two inputs the old bodies wrote inconsistently (they never occur: the
emitters make neither) are now written consistently: a COFF relocation
whose section is neither 1 nor 2 (counted but not written before; now in
.text's table) and Mach-O `__const` relocations without `__const` bytes.

## Checks

Unit tests (each writer twice on the same input, the two runs equal and
their digest the old body's; the two fixed objects read back by a checker
that rejected the old bytes, and the ELF object linked by
compiler/linker.fd):

```
$FID run tests/nattools/image_writer_unit.fd       # nat_enc.fd writers
$FID run tests/nattools/image_writer_obj_unit.fd   # pe_ir_enc, coff, linker, elf64, macho
```

(The interpreter takes about 15 minutes on the first; `$FID ccompile
<test> -o t.exe && ./t.exe` takes about 2.) Expected output: the
`Expected output:` comment at the top of each file.

Real programs, every format reachable from the command line: each image
built twice must be the same, and `readelf -W -h -l -S` (here or in WSL)
must accept the ELF ones without a warning, `llvm-objdump --macho -h` the
Mach-O ones when installed; `REF=<an older fid>` also compares with what
that compiler writes:

```
FID=$FID bash tests/nattools/image_same_check.sh
```

and the fixed objects with the system tools (WSL):

```
$FID codegen tests/bootstrap/hello.fd --elf /tmp/h.o && readelf -a -W /tmp/h.o
$FID codegen tests/bootstrap/hello.fd --macho /tmp/h.mo && llvm-objdump --macho -h -r -t /tmp/h.mo
```

`readelf` must print no warnings; `llvm-objdump` must list `__text` (and
`__const` when the program has strings) and the symbols.
