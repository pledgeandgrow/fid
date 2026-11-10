# pe_ir_enc.fd — x86-64 encoder, import tables, .rdata and PE writer (pe_ir)

> **New 2026-11-09** (not in the 10-22 audit). Written from the current
> source and live probes with `s18.exe` (see pe_ir.md for the probes).

- **Path:** `compiler/pe_ir_enc.fd`
- **LOC:** 2,579 (INDEX.md still says 2,279). Ported from the top half of
  `src/pe.rs`, then extended (value registers, `link` DLLs, string headers).
- **Role:** The byte layer of the pe_ir backend: `PeirEnc` (machine code,
  labels, RIP-relative references to .rdata), the register-slot convention,
  the kernel32/msvcrt/`link` DLL import lists, `PeirRdata` (strings, import
  directory, ILT/IAT) and `pe_build_file` (the PE32+ file).
- **Imports:** `core`, `vec`, `string` only.

## Callers

| Caller | What |
|--------|------|
| `pe_ir.fd` (`use compiler::pe_ir_enc`) | every `PeirEnc_*`, `PeirRdata_*`, `pe_build_file`, `pe_push_u*`, `pe_dll_*`, `pe_import_slot`, `PE_*` constants |
| `pe_ir_stubs.fd` (`use compiler::pe_ir_enc`) | `PeirEnc_*` for the stub bodies, `pe_import_slot`, `PIAT_*`, `PeirRdata` (DLL loader) |

No other module uses it (nat_ir has its own encoder and PE writer in
nat_enc.fd). Reached from the same commands as pe_ir: `build`/`emit` on a
Windows host, `--bare`, `--elf`/`--target linux`, and `build --native
--target windows`.

## What it does now

- **Encoder** (`PeirEnc` :120): append-only `code`, labels resolved at the
  end (`PeirEnc_resolve_labels` :1752), RIP references patched once the
  .rdata RVA is known (`PeirEnc_resolve_rip_refs` :1777). GP, SSE2 scalar
  and packed, a few VEX ops, byte/word/dword/qword memory forms.
- **Store→load forwarding** (:728-777): a `mov r, [rbp+d]` right after
  `mov [rbp+d], s` with no byte in between becomes `mov r, s`; a label clears
  the state.
- **Register slots** (:530-554): a frame offset `>= PEIR_REG_SLOT`
  (0x7F000000) names value register `peir_value_reg(i)` (rbx, rsi, rdi,
  r12-r15, then r10, r11); `>= PEIR_XMM_SLOT` names an xmm register. The
  rbp load/store helpers turn these into register moves, and
  `PeirEnc_emit_mem_rbp` panics if one is used as memory (:779).
- **Imports**: 54 kernel32 functions (`pe_kernel32_imports` :1799; the first
  13 have `PIAT_*` slots :2013-2025) and 77 msvcrt functions
  (`pe_msvcrt_imports` :1866), all in every executable. `extern "C" link
  "x"` adds `x.dll` (`pe_link_dll` :1929, `pe_add_extra_import` :1908);
  those DLLs get IAT slots but are loaded at start-up by the program itself
  (`PE_STATIC_DLLS` = 2 :1942, loader in pe_ir_stubs.fd), with
  `sqlite3.dll` → `winsqlite3.dll` fallback (:1946). `pe_dll_found` (:1955)
  warns at compile time when a DLL is not on this machine.
- **.rdata** (`PeirRdata_build` :2132): sorted string constants, each with
  core's static-string header `[0x4649444553545202][len]` before the bytes
  (`PeirRdata_add_string` :2068; same value as core's `P_STR_STATIC`), DLL
  names, hint/name entries, ILT, IAT, import directory; `PeirRdata_patch_rvas`
  (:2254) fills in RVAs. `PeirRdata_build_strings` (:2116) is the bare build's
  data (no imports).
- **PE file** (`pe_build_file` :2387): DOS header, `PE\0\0`, COFF (AMD64, 2
  sections, no timestamp), PE32+ optional header, ImageBase 0x140000000,
  `.text` (RX) at 0x1000 and `.rdata` (RW: globals, allocator state, IAT live
  there). SizeOfHeaders is the computed `header_size`; StackReserve 4 GB,
  StackCommit 1 MB, HeapReserve 1 MB, HeapCommit 4 KB.

## Findings

### Verified live

No byte-level problem seen: every probe exe in pe_ir.md loads and runs
(hello, Vec, files, JSON, tensors, SQLite through `link`).

### Code inspection

#### LOW — import directory size says one descriptor

:2482 writes size 20 ("1 descriptor + null") while there are at least two
DLLs plus the null entry. The Windows loader walks to the null descriptor,
so it works; tools that trust the size may not.

#### LOW — no ASLR, writable string literals

DllCharacteristics is 0 "(testing)" (:2468) and there is no `.reloc`, so the
image always loads at 0x140000000. String literals sit in the RW `.rdata`,
so a stray write through a `str` changes a literal instead of faulting.
(64-bit processes get DEP regardless.)

#### INFO — 4 GB stack reserve for every thread

:2469 reserves 4 GB; its comment ("one slot per IR value, no reuse") is
stale since slot reuse and register allocation. conc.fd creates threads with
stack size 0 (pe_runtime/conc.fd:151, :587), so each thread also reserves
4 GB of address space (committed lazily).

#### INFO — quadratic helpers

`pe_import_slot` (:1993) rebuilds the whole import list for each lookup and
is called per `extern` declaration; `pe_sort_strings` (:2099) is an insertion
sort over every string constant.

## Fix order

1. Write the real import directory size.
2. Consider a `.reloc` section + DYNAMIC_BASE/NX_COMPAT, and a read-only
   section for literals.
3. Update the stack-reserve comment (or lower it now that frames are small).

## Verification state

- Live: PE files from `s18.exe emit` run (Nov 9; see pe_ir.md).
- pe-roadmap items 1-12 (encoder and file format) were checked against
  `src/pe.rs` on Oct 30-31; not redone. src/pe.rs has most of the newer
  kernel32 names too (fibers, IOCP, VirtualAlloc/Free) but not VirtualProtect.
