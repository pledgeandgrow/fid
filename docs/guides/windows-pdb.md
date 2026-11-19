# PDB files for Windows debug builds

Visual Studio, WinDbg and every tool built on dbghelp.dll read a program's
functions and source lines from a PDB (program database), which the
executable names in its debug directory. gdb, lldb and the llvm tools read
the DWARF sections and COFF symbols that `fid emit -g` already writes;
Microsoft's debuggers read neither. `compiler/pdb.fd` writes the PDB
(own-backend roadmap DB3).

## What a debug build writes

(The writer and the `--nat` call are in `compiler/pdb.fd` and
`compiler/nat_enc.fd` `nat_pe_debug`; pe_ir's call after its COFF symbols
and the driver's `pdb_setup(<output>)` in `compiler/main.fd` are what make
`fid emit -g` use it. Without the driver's call, `FID_PDB_OUT=<path>`
names the PDB for a backend that calls `pdb_attach`.)

`fid emit prog.fd -g -o prog.exe` (the default Windows backend, pe_ir; and
`--nat`) writes:

- `prog.exe` with a `.buildid` section holding an `IMAGE_DEBUG_DIRECTORY`
  entry of type CODEVIEW and its RSDS record: the PDB's GUID, age 1 and its
  path (as given with `-o`, `.exe` replaced by `.pdb`). Data directory 6
  points at it. Debuggers also look for `prog.pdb` beside the executable.
- `prog.pdb`, an MSF 7.00 container with:
  - the PDB info stream (VC70, the same GUID and age, `/names`, VC140);
  - empty TPI and IPI streams (functions have no type record, as assembly
    procedures do);
  - the DBI stream: one module (the input file), its section contribution
    (.text), the section map, the module's source files, and the image's
    section headers (stream 10) so addresses map to sections;
  - the module stream: `S_OBJNAME`, `S_COMPILE3`, an `S_GPROC32` ... `S_END`
    per function of the input file, and C13 line information: a
    `DEBUG_S_FILECHKSMS` entry per source file and a `DEBUG_S_LINES`
    subsection per function, from the same rows the DWARF line table uses
    (imported files included where the backend records them, DB1);
  - globals (`S_PROCREF` per function) and publics (`S_PUB32` per function
    and per other COFF function symbol: runtime stubs, primitives, thunks),
    with their GSI hash tables and address map.

The GUID is a hash of the image, so the same input gives the same image and
the same PDB (reproducible builds).

`FID_PDB=0` writes no PDB (the image then has no debug directory either).
`FID_PDB_OUT=<path>` names the PDB when the driver did not (a backend
called outside `fid emit`).

## Using it

- WinDbg: `windbg prog.exe`, then `bp main`, `g`, `k` (functions and
  `prog.fd` lines), `ln <address>`.
- Visual Studio: open `prog.exe` as a project (File > Open > Project/Solution),
  start debugging; breakpoints by function name work, the call stack shows
  source lines.
- dbghelp: `SymLoadModuleExW` on the executable finds the PDB by its RSDS
  record; `SymFromName`, `SymFromAddr`, `SymGetLineFromAddrW64` answer.

Variables and types are not in the PDB yet (no `S_REGREL32`/`S_LOCAL`
records, no TPI types): use gdb or lldb with the DWARF for those.

## Checking it

```
python3 tests/nattools/pe_pdb_check.py --unit --fid $FID     # the writer on a real image
python3 tests/nattools/pe_pdb_check.py --emit --fid $FID     # `emit -g` writes and names the PDB
python3 tests/nattools/pe_pdb_check.py --emit --nat --fid $FID
```

The checker reads the PDB three ways: its own MSF/PDB reader (written from
LLVM's description of the format: block map, stream directory, free block
map, info stream named map buckets, DBI substream sizes, string tables and
GSI hash buckets are all verified), dbghelp.dll (Windows: functions by name
and address, lines at every DWARF row's address), and `llvm-pdbutil dump
-all` / `llvm-symbolizer` when they are on PATH.
