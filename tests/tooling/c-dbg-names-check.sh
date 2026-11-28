#!/bin/bash
# BG244 (docs/roadmap/bugs-roadmap.md): `fid ccompile -g` names the C
# variables after the Fides ones (compiler/codegen_c.fd cg_dbg_c_names), so
# a C debugger (gdb, lldb, Visual Studio on the C build) shows `acc`, not
# v12; without -g the C stays what it was.
#   FID=release/fid.exe bash tests/tooling/c-dbg-names-check.sh
# Run from the repository root. Checks, each printed as ok / FAIL / skip:
#   1. tests/tooling/c_dbg_names_unit.fd (`fid run`) prints its expected
#      lines: slots, parameters, values, reserved and shadowed names, a slot
#      accessed wider than its type kept as an alloca, no names without -g;
#      the -g C it writes compiles (`$CC -c -g`, default zig cc).
#   2. `fid ccompile -g` of tests/bootstrap/test_pe_dbg_vars.fd: the program
#      prints its expected output, and its C (next to the output) declares
#      scale's acc and step as the slots (`int64_t acc = 0;`,
#      `= (void*)&acc;`), the parameters base and factor, and main's total,
#      name (`const char*`) and big (`bool`).
#   3. without -g, `fid dump-c` of the same file has none of them: the
#      slots are alloca()s, no copies (the C of a build without -g is
#      unchanged; the 3-stage fixpoint compares it byte for byte).
#   4. gdb, when installed and the C build has DWARF it reads: a breakpoint
#      at line 29 (@scale_add) shows acc = 42, step = 3, base = 6 by name.
# Passing: the last line is "c-dbg-names-check: ok".
FID=${FID:-release/fid.exe}
CC=${CC:-zig cc}
export CC
D=$(mktemp -d ./tests/_c_dbg_names.XXXXXX)
D=${D#./}
trap 'rm -rf "$D"' EXIT
fail=0
ok() { echo "ok   $1"; }
no() { echo "FAIL $1"; fail=1; }
skip() { echo "skip $1"; }
expected() {
    awk '/^\/\/ Expected output:/{on=1; next} on && /^\/\/ /{sub(/^\/\/ /, ""); print; next} on{exit}' "$1"
}
winpath() {
    if command -v cygpath > /dev/null 2>&1; then cygpath -w "$1"; else echo "$1"; fi
}

# 1. the unit
U=tests/tooling/c_dbg_names_unit.fd
if FID_C_OUT="$(winpath "$D/unit.c")" "$FID" run "$U" > "$D/unit.txt" 2> "$D/unit.err"; then
    grep -v '^\[' "$D/unit.txt" | tr -d '\r' > "$D/unit.out"
    if diff <(expected "$U") "$D/unit.out" > "$D/unit.diff"; then ok "unit output"; else no "unit output: $(tr '\n' ' ' < "$D/unit.diff")"; fi
    if [ -f "$D/unit.c" ]; then
        # (the unit has no core: what the runtime calls of it is undeclared)
        if $CC -c -g -w -Wno-implicit-function-declaration "$D/unit.c" -o "$D/unit.o" > "$D/unit_cc.txt" 2>&1; then ok "the unit's -g C compiles ($CC)"; else no "the unit's -g C: $(grep -i error "$D/unit_cc.txt" | head -3 | tr '\n' ' ')"; fi
    else
        no "the unit wrote no C"
    fi
else
    no "fid run $U: $(tail -3 "$D/unit.err" | tr '\n' ' ')"
fi

# 2. a real program with -g
T=tests/bootstrap/test_pe_dbg_vars.fd
EXE="$D/dv.exe"
C="$D/dv.c"
if "$FID" ccompile "$T" -g -o "$EXE" > "$D/cc.txt" 2>&1; then
    got=$("./$EXE" 2>&1 | tr -d '\r')
    if [ "$got" = "$(expected "$T")" ]; then ok "ccompile -g: output"; else no "ccompile -g printed: $got"; fi
    if [ -f "$C" ]; then
        # the C of scale and of main (main_entry)
        awk '/^[a-z_0-9 ]+ scale\(/{on=1} on{print} on && /^}/{exit}' "$C" > "$D/scale.c"
        awk '/^[a-z_0-9 ]+ main_entry\(/{on=1} on{print} on && /^}/{exit}' "$C" > "$D/main.c"
        want_ok=1
        for pat in '    int64_t acc = 0;' '= (void\*)&acc;' '    int64_t step = 0;' '= (void\*)&step;' '    int64_t base = (int64_t)v' '    int64_t factor = (int64_t)v'; do
            grep -q -- "$pat" "$D/scale.c" || { no "scale's C has no \`$pat\`"; want_ok=0; }
        done
        for pat in '    int64_t total = 0;' '    const char\* name = 0;' '    bool big = 0;' '= (void\*)&total;'; do
            grep -q -- "$pat" "$D/main.c" || { no "main's C has no \`$pat\`"; want_ok=0; }
        done
        [ $want_ok -eq 1 ] && ok "ccompile -g: scale and main's locals under their Fides names"
    else
        no "no C file $C next to the output"
    fi
else
    no "ccompile -g failed: $(tail -5 "$D/cc.txt" | tr '\n' ' ')"
fi

# 3. without -g: the C of before
if "$FID" dump-c "$T" > "$D/plain.c" 2> "$D/plain.err"; then
    awk '/^[a-z_0-9 ]+ scale\(/{on=1} on{print} on && /^}/{exit}' "$D/plain.c" > "$D/plain_scale.c"
    if [ -s "$D/plain_scale.c" ] && ! grep -q "acc\|(void\*)&" "$D/plain_scale.c" && grep -q "= alloca(8);" "$D/plain_scale.c"; then
        ok "without -g: no names, the slots are alloca()s"
    else
        no "without -g, scale's C: $(head -20 "$D/plain_scale.c" | tr '\n' ' ')"
    fi
else
    no "dump-c failed: $(tail -3 "$D/plain.err" | tr '\n' ' ')"
fi

# 4a. what Visual Studio / WinDbg read: the C build's PDB (zig cc and cl
# write one on Windows), its S_LOCAL records in scale's S_GPROC32 (the
# slots acc and step must be there; base and factor, copies an optimizing
# C compiler may drop, are a note)
PU="${LLVM_PDBUTIL:-llvm-pdbutil}"
if ! command -v "$PU" > /dev/null 2>&1 && [ -x "/c/Users/$USERNAME/AppData/Local/Programs/llvm-21.1.0/bin/llvm-pdbutil.exe" ]; then
    PU="/c/Users/$USERNAME/AppData/Local/Programs/llvm-21.1.0/bin/llvm-pdbutil.exe"
fi
PDB="${EXE%.exe}.pdb"
if command -v "$PU" > /dev/null 2>&1 && [ -f "$PDB" ]; then
    "$PU" dump --symbols "$PDB" > "$D/pdb.txt" 2>&1
    awk '/S_GPROC32 .*`scale`/{on=1} on{print} on && /S_END/{exit}' "$D/pdb.txt" > "$D/pdb_scale.txt"
    if grep -q 'S_LOCAL .*`acc`' "$D/pdb_scale.txt" && grep -q 'S_LOCAL .*`step`' "$D/pdb_scale.txt"; then
        ok "PDB: scale's locals acc and step by name ($(grep -o 'S_LOCAL [^`]*`[a-z_0-9]*`' "$D/pdb_scale.txt" | grep -o '`.*`' | tr '\n' ' '))"
    else
        no "PDB: scale's S_LOCAL records: $(grep -o 'S_LOCAL [^`]*`[a-z_0-9]*`' "$D/pdb_scale.txt" | tr '\n' ' ')"
    fi
else
    skip "llvm-pdbutil or the C build's PDB"
fi

# 4. gdb on the -g build
if command -v gdb > /dev/null 2>&1 && [ -f "$EXE" ]; then
    L=$(grep -n "// @scale_add" "$T" | cut -d: -f1)
    gdb -batch -ex "break test_pe_dbg_vars.fd:$L" -ex run -ex "info locals" -ex kill "$EXE" > "$D/gdb.txt" 2>&1
    if grep -q "No symbol table\|No source file named" "$D/gdb.txt"; then
        skip "gdb (the C build has no DWARF gdb reads)"
    elif grep -q "acc = 42" "$D/gdb.txt" && grep -q "step = 3" "$D/gdb.txt" && grep -q "base = 6" "$D/gdb.txt"; then
        ok "gdb: acc = 42, step = 3, base = 6 at line $L"
    else
        no "gdb: $(tail -8 "$D/gdb.txt" | tr '\n' ' ')"
    fi
else
    skip "gdb (not installed)"
fi

if [ $fail -eq 0 ]; then echo "c-dbg-names-check: ok"; else echo "c-dbg-names-check: FAILED"; fi
exit $fail
