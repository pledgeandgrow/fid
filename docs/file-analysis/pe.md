# pe.fd — old PIR-based PE / flat / multiboot writer + Win64 stub library

> **Re-audited 2026-11-09** against the current source, with live probes
> (`s18.exe` and `release/fid.exe`, test files in a scratch folder). Since the
> 10-23 audit the native Windows path moved to `compiler/pe_ir*.fd` (see
> pe_ir.md). This file is now reached by one command only, `build/emit
> --boot`, and that command panics on every program. The hosted PE writer
> (`generate_pe`) and the flat writer (`generate_flat_binary`) have no callers.

- **Path:** `compiler/pe.fd`
- **LOC:** 6,857 (6,852 on 10-23). Last changed in git on Nov 3 (e655b58).
- **Role:** Takes the PIR L3 module, prints it as x86-64 assembly
  (`codegen_x64.generate_asm` + `peephole_optimize`), assembles it with
  `ElfEmitter`, then links the user code with a library of hand-encoded
  Win64 runtime stubs (`X86Encoder`, ~90 `PeCodeGen_gen_*` stubs) into a PE
  (hosted), a flat binary loaded at 0x10000, or a multiboot2 kernel at
  0x100000.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `ir`, `pir`, `elf64`,
  `codegen_x64`, `peephole`.

## Callers

| Caller | Site | What |
|--------|------|------|
| `main.fd` `cmd_build` (`build`, `emit`) | :2867-2899 | `--boot`: `build_program_no_std` → `lower` → L1/L2/L3 (no optimizer) → `generate_boot_binary` (:2891) → writes the bytes |
| `main.fd` | :23 | `use compiler::pe` (only `generate_boot_binary` is called) |
| `compiler.fd` | :39 | `use compiler::pe`; nothing called. The comment at :2107 ("windows / x64-win -> pe::generate_pe") is stale: `build_executable` calls `pe_ir_generate_pe` (:2216) |

No other module uses `X86Encoder`, `RdataBuilder`, `build_pe_file`,
`PeCodeGen` or the `PE_REG_*` constants.

**Which commands reach it:** only `fid build|emit <f> --boot`.
- `build --native` does not reach pe.fd any more. With the default target
  (`x86-64`) it goes to `build_executable` → codegen_x64 + elf64 +
  runtime_linux + linker (a Linux ELF). With `--target windows` it goes to
  `pe_ir_generate_pe`. Both crash first in the PIR optimizer (see pe_ir.md).
- `--bare` uses `pe_ir_generate_flat`; `--elf` / `--target linux` use
  `pe_ir_generate_elf`; the Windows default uses `pe_ir_generate_pe`.

## What it does now

- `pe_build(asm, module, mode)` (:1937): assemble → collect undefined symbols
  (`p_*` = stubs, others = kernel32 imports; :1797) → .rdata (assembler
  rodata, "true"/"false"/"-"/".", import table when hosted) → enc stream
  (boot trampoline + IDT setup first in boot mode; SysV→Win64 wrappers; import
  trampolines; stub bodies; hosted `_start` last) → resolve labels → lay out
  `.text` ([user][stubs] or, for boot, [stubs][user]) → patch user refs,
  RIP refs, assembler relocations → PE (`build_pe_file` :1328) or raw image
  (multiboot2 header :2149 + text + 0xCC pad + rdata).
- Entry points: `generate_pe` (:2121), `generate_flat_binary` (:2128),
  `generate_boot_binary` (:2135).
- SysV glue: `pe_emit_sysv_prologue` (:1734) moves rdi/rsi/rdx/rcx into
  rcx/rdx/r8/r9 before jumping to a Win64 stub body; import trampolines
  (:1748) handle up to 6 args.
- Stub library: print, malloc/free, strings, Vec, HashMap, float formatting,
  and the bare-metal set (port I/O, CR0/2/3/4, MSRs, swapgs, IDT/GDT,
  paging, serial, user-mode entry, syscall handler) :2173-6852.

## Findings

### Verified live

#### HIGH — `--boot` panics on every program

`pe_build` builds the import table only when hosted (:1971-1973), so in boot
mode `gen.iat_rdata_offsets` is empty (:1978). `PeCodeGen_gen_runtime_stubs`
(:2298) still emits every hosted stub; the first, `gen_print_int`, calls
`PeCodeGen_emit_get_std_handle` (:2503-2507) → `PeCodeGen_iat_off` (:1671)
→ `Vec_get` out of bounds. Probe: a 4-line `main` returning 42,
`build k.fd --boot` with both `s18.exe` and `release/fid.exe`: "Panic: Vec
index out of bounds" in `PeCodeGen_iat_off` ← `gen_print_int` ←
`gen_runtime_stubs` ← `pe_build` ← `generate_boot_binary`. The module's only
reachable entry point does not work.

### Code inspection

#### MEDIUM — unresolved symbols and unknown builtins become silent zeros

`pe_apply_text_relocs` leaves the displacement 0 for a symbol it cannot
resolve (:1885-1890): the call jumps to the next instruction.
`pe_emit_stub_wrappers` gives an unknown `p_*` builtin a body of
`xor eax,eax; ret` (:1837-1841). Neither is reported. (pe_ir refuses such a
program instead.)

#### MEDIUM — Vec stubs use the old layout; five are no-ops (STILL TRUE)

`p_vec_new` allocates a 24-byte header (:3092-3107); the layout is now 40
bytes `{data, len, cap, code, tid}` (AGENTS.md #25).
`p_vec_insert`/`remove`/`reverse`/`extend`/`sort` return 0 (:5688-5726).
Boot programs have no core, so these stubs are what they get.

#### LOW — an assembler error is reported as success

`pe_build` prints the error and returns an empty Vec (:1940-1947);
`generate_boot_binary` returns it; `main.fd` writes a 0-byte file and prints
"Emitted multiboot2 kernel ... (0 bytes, boot-ready)", exit 0 (:2891-2898).

#### LOW — hosted PE writer bugs (dead code: no caller)

- StackReserve and HeapCommit are written as 0x100000 (:1426, :1432).
  STILL TRUE (10-23 finding).
- SizeOfHeaders is the constant 0x200 (:1418), not `header_size`. STILL TRUE.
- IAT directory size `(import_names.len() + 1) * 8` (:2092) leaves out the six
  fixed imports `RdataBuilder_build_imports` puts first (:1141-1146).
- Import trampolines are always built for 4 args (:1855), so args 5-6 of a
  kernel32 call are dropped.

#### INFO — `@cpu_local` support is gone

`cpu_local_names`/`cpu_local_offsets` (:1549-1551) are never filled;
`PeCodeGen_get_cpu_local_offset` (:1615) has no caller. The GS helpers
(:1627-1644) are only used by stubs. No module handles `@cpu_local` in code
generation now (only typechecker.fd mentions it).

#### Older findings

- `FloatLit` globals zero-initialized: CHANGED. That code was in the deleted
  second codegen; globals now come from codegen_x64's assembly (not
  re-checked there). pe_ir writes real float bits (pe_ir.fd:559-566).
- `pe_debug.txt` writes: FIXED (none left).

## Fix order

1. Decide the module's fate. pe_ir already has the bare-metal runtime and the
   flat binary (`--bare`); a multiboot2 header and the 32→64 boot trampoline
   are what `--boot` would need there. Then delete pe.fd (6.9K lines compiled
   into every `fid` for a command that crashes).
2. If kept: skip hosted stubs (or IAT use) when not hosted; make unresolved
   symbols and unknown builtins errors; fail the build on an assembler error.
3. Drop `use compiler::pe` from compiler.fd and fix the comment at :2107.

## Verification state

- Live (Nov 9): `--boot` panics with `s18.exe` and `release/fid.exe`.
- The hosted and flat paths cannot be run (no callers).
- Everything else is code inspection of the current file.
