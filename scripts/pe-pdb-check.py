#!/usr/bin/env python3
"""pe-pdb-check.py [--fid FID] [--nat] [SRC]: the PDB of a Windows debug
build (docs/roadmap/llvm-roadmap.md DB3, compiler/pdb.fd).

Runs tests/nattools/pe_pdb_check.py --emit with the same arguments: it
builds SRC (default tests/bootstrap/hello.fd) with `fid emit -g` (pe_ir;
with --nat the machine layer), then reads the image's debug directory and
the PDB beside it with its own MSF reader, with dbghelp on Windows and
with llvm-pdbutil when present. Exit 0 when every check passes.
"""
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CHECKER = os.path.join(ROOT, "tests", "nattools", "pe_pdb_check.py")

if __name__ == "__main__":
    r = subprocess.run([sys.executable, CHECKER, "--emit"] + sys.argv[1:])
    sys.exit(r.returncode)
