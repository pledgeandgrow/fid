# pdb.fd — program databases (PDB) for Windows images

> **New doc 2026-11-21.** Read from the source; the status is the Nov 21
> check of llvm-roadmap DB3.

- **Path:** `compiler/pdb.fd`
- **LOC:** 1,438 (2026-11-21). In git since Nov 19 (49fb2b9).
- **Role:** llvm-roadmap DB3, release N9. Writes a PDB (MSF 7.00, block
  size 4096) for a debug build's Windows image, which Visual Studio,
  WinDbg and dbghelp read (they read neither the COFF symbols nor the
  DWARF sections the image already has). `pdb_attach` is the one call a
  backend makes, after its image is complete: `pdb_guid` (a hash of the
  image), `pdb_add_debug_directory` (a `.buildid` section with a CODEVIEW
  IMAGE_DEBUG_DIRECTORY entry and its RSDS record, data directory 6; the
  COFF symbol table moves after it), then `pdb_build` writes the PDB to
  the output path with `.exe` → `.pdb` (`pdb_setup`) or `$FID_PDB_OUT`.
  Streams: PDB info, empty TPI/IPI, DBI with one module, `/names`, the
  module's symbols (S_GPROC32 per function) and C13 line tables, globals
  (S_PROCREF), publics (S_PUB32 for every COFF function symbol), the
  section headers.
- **Imports:** `core`, `vec`, `string`.

## Callers

| File | What |
|------|------|
| `main.fd` | `pdb_setup(output)` for a debug `emit` of pe_ir (`-g`, `--debug` or `FID_PE_DEBUG`) and of `--nat` |
| `pe_ir.fd` | `pdb_attach(image, ...)` with its function ranges and line rows (file 0 only) |
| `nat_enc.fd` | `pdb_attach(...)` from `nat_pe` with the machine layer's rows (all source files) |

No path set (`pdb_setup` not called) or `FID_PDB=0`: the image comes back
unchanged.

## Status (Nov 21 check, DB3: validated)

tests/nattools/pe_pdb_check.py passes `--unit` (tests/nattools/pdb_unit.fd:
11 streams, 177 procedures, C13 lines, GUID and age match), `--emit` on
pe_ir (prog.pdb next to prog.exe, lines agree with `.debug_line`) and
`--emit --nat` (305 procedures, 5,338 line rows); dbghelp reads them.
WinDbg and Visual Studio were not run.

## Findings

- **LOW:** pe_ir PDBs list only the functions that have line rows.
- **LOW:** no variables (no type records, functions have type 0; DB4).
- Not this module, but seen with it: `emit -g` of optimized pe_ir shows
  wrong values for locals in a debugger (known-bugs 2g.104).
