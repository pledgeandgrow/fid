#!/usr/bin/env python3
"""BG243: decode the image tests/nattools/pe_dwarf_inl_unit.fd writes
($PE_DWARF_OUT) and check its inlined calls:

    PE_DWARF_OUT=u.exe fid run tests/nattools/pe_dwarf_inl_unit.fd
    python tests/nattools/pe_dwarf_inl_check.py u.exe

  - one abstract DW_TAG_subprogram `inner` (DW_AT_inline 1, no
    DW_AT_low_pc, DW_AT_decl_line 26: its `fn` line in the unit);
  - `outer` (a subprogram with code) has two DW_TAG_inlined_subroutine
    children whose DW_AT_abstract_origin is that entry: [outer + 8,
    outer + 20) called from line 52 and [outer + 30, outer + 34) from line
    53, DW_AT_call_file 1;
  - llvm-dwarfdump --verify passes (when installed: PATH, LLVM_DWARFDUMP or
    the usual Windows install), and its --debug-info names `inner` as the
    abstract origin.
Exit 0 when every check holds."""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import dbg_vars_check as dv  # noqa: E402


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    img = sys.argv[1]
    data = open(img, "rb").read()
    info = dv.section(data, ".debug_info")
    ab = dv.section(data, ".debug_abbrev")
    fails = []
    if info is None or ab is None:
        print("FAIL no .debug_info / .debug_abbrev")
        return 1
    dies = dv.dies(info, dv.abbrevs(ab))
    abstract = [d for d in dies if d[1] == dv.DW_TAG_subprogram and d[2].get(0x03) == "inner"]
    outer = [d for d in dies if d[1] == dv.DW_TAG_subprogram and d[2].get(0x03) == "outer"]
    if len(abstract) != 1:
        fails.append("%d subprograms named inner, want 1 (abstract)" % len(abstract))
    if len(outer) != 1:
        fails.append("%d subprograms named outer, want 1" % len(outer))
    if not fails:
        a = abstract[0]
        o = outer[0]
        if a[2].get(0x20) != 1 or 0x11 in a[2]:
            fails.append("inner: DW_AT_inline %r, low_pc %r: not an abstract entry" % (a[2].get(0x20), a[2].get(0x11)))
        if a[2].get(0x3B) != 26:
            fails.append("inner: DW_AT_decl_line %r, want 26" % a[2].get(0x3B))
        lo = o[2].get(0x11, 0)
        calls = sorted((k[2].get(0x11, 0) - lo, k[2].get(0x11, 0) - lo + k[2].get(0x12, 0), k[2].get(0x59), k[2].get(0x58))
                       for k in dies if k[1] == 0x1D and k[4] == o[0] and k[2].get(0x31) == a[0])
        want = [(8, 20, 52, 1), (30, 34, 53, 1)]
        if calls != want:
            fails.append("outer's inlined calls %r, want %r (start, end, call line, call file)" % (calls, want))
        else:
            print("ok   inner: abstract entry (decl line 26), inlined into outer twice")
    dd = os.environ.get("LLVM_DWARFDUMP") or shutil.which("llvm-dwarfdump")
    if not dd:
        cand = os.path.join("C:\\Users", os.environ.get("USERNAME", ""), "AppData", "Local", "Programs", "llvm-21.1.0", "bin", "llvm-dwarfdump.exe")
        if os.path.exists(cand):
            dd = cand
    if dd:
        r = subprocess.run([dd, "--verify", img], capture_output=True, text=True)
        if r.returncode != 0:
            fails.append("llvm-dwarfdump --verify: %s" % (r.stdout + r.stderr)[-600:])
        r2 = subprocess.run([dd, "--debug-info", img], capture_output=True, text=True)
        if "DW_TAG_inlined_subroutine" not in r2.stdout or '"inner"' not in r2.stdout:
            fails.append("llvm-dwarfdump --debug-info shows no inlined call of inner")
        if r.returncode == 0:
            print("ok   llvm-dwarfdump --verify")
    else:
        print("skip llvm-dwarfdump (not installed)")
    for f in fails:
        print("FAIL " + f)
    print("pe_dwarf_inl_check: %s" % ("ok" if not fails else "FAILED"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
