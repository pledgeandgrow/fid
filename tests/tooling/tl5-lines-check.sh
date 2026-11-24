#!/bin/bash
# TL5 (docs/roadmap/competitive-roadmap.md): line tables on every path, for
# a program of two files (tests/projects/tl5_lines: main.fd uses shapes.fd).
#   1. `fid ccompile -g`: the C has `#line N ".../main.fd"` and `#line N
#      ".../shapes.fd"` (compiler/codegen_c.fd cg_lines_file_for: the
#      imported module's functions too), and the program still runs right.
#   2. `fid emit --target aarch64-macos` and `x86_64-macos`: a __DWARF
#      segment with the line table and functions of both files
#      (compiler/nat_enc.fd nat_macho_debug), read by
#      tests/nattools/macho_dwarf_check.py; with FID_NAT_OFF=debug no
#      __DWARF segment.
#   3. On Windows, `fid emit -g` (pe_ir): scripts/pe-dwarf-check.py reads
#      the PE's .debug_line and its compile unit names main.fd.
#   FID=release/fid.exe bash tests/tooling/tl5-lines-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
PY=${PYTHON:-python3}
command -v "$PY" > /dev/null 2>&1 || PY=python
P=tests/projects/tl5_lines/main.fd
# relative paths: Git Bash would rewrite a /tmp path given to a Windows program
D=$(mktemp -d ./tests/_tl5_check.XXXXXX)
D=${D#./}
trap 'rm -rf "$D"' EXIT
fail=0
ok() { echo "ok   $1"; }
no() { echo "FAIL $1"; fail=1; }
want="area 12
perimeter 14
total 26"

# 1. the C path
EXE="$D/tl5.exe"
if "$FID" ccompile "$P" -g -o "$EXE" > "$D/cc.txt" 2>&1; then
    ok "ccompile -g builds"
    C="$D/tl5.c"
    if [ -f "$C" ]; then
        if grep -Eq '^#line [0-9]+ ".*main\.fd"' "$C"; then ok "#line to main.fd"; else no "no #line to main.fd in $C"; fi
        if grep -Eq '^#line [0-9]+ ".*shapes\.fd"' "$C"; then ok "#line to shapes.fd (imported module)"; else no "no #line to shapes.fd in $C"; fi
        # perimeter's body is lines 21-24 of shapes.fd
        if grep -Eq '^#line (21|22|24) ".*shapes\.fd"' "$C"; then ok "shapes.fd lines of perimeter"; else no "no #line 21-24 for shapes.fd"; fi
    else
        no "no C file $C next to the output"
    fi
    got=$("./$EXE" 2>&1 | tr -d '\r')
    if [ "$got" = "$want" ]; then ok "the -g build runs"; else no "the -g build printed: $got"; fi
else
    no "ccompile -g failed: $(tail -5 "$D/cc.txt")"
fi

# 2. Mach-O
for T in aarch64-macos x86_64-macos; do
    M="$D/tl5-$T"
    if "$FID" emit "$P" --target "$T" -o "$M" > "$D/emit.txt" 2>&1; then
        if "$PY" tests/nattools/macho_dwarf_check.py "$M" --file main.fd --file shapes.fd --fn main --fn perimeter > "$D/mc.txt" 2>&1; then
            ok "$T: __DWARF line table (macho_dwarf_check)"
        else
            no "$T: macho_dwarf_check: $(tr '\n' ' ' < "$D/mc.txt")"
        fi
        M2="$D/tl5-$T-nodebug"
        if FID_NAT_OFF=debug "$FID" emit "$P" --target "$T" -o "$M2" > /dev/null 2>&1; then
            if grep -q "__DWARF" "$M2"; then no "$T: FID_NAT_OFF=debug still writes __DWARF"; else ok "$T: FID_NAT_OFF=debug: no __DWARF"; fi
            if "$PY" scripts/macho-check.py "$M2" --quiet > /dev/null 2>&1; then ok "$T: the image without DWARF passes macho-check"; else no "$T: macho-check of the image without DWARF"; fi
        else
            no "$T: emit with FID_NAT_OFF=debug failed"
        fi
    else
        no "$T: emit failed: $(tail -5 "$D/emit.txt")"
    fi
done

# 3. the PE of pe_ir (Windows hosts)
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*)
        PE="$D/tl5-pe.exe"
        if "$FID" emit "$P" -g -o "$PE" > "$D/pe.txt" 2>&1; then
            if "$PY" scripts/pe-dwarf-check.py "$PE" main.fd > "$D/pc.txt" 2>&1; then ok "pe_ir -g: pe-dwarf-check"; else no "pe-dwarf-check: $(tr '\n' ' ' < "$D/pc.txt")"; fi
            got=$("./$PE" 2>&1 | tr -d '\r')
            if [ "$got" = "$want" ]; then ok "the pe_ir -g build runs"; else no "the pe_ir -g build printed: $got"; fi
        else
            no "emit -g failed: $(tail -5 "$D/pe.txt")"
        fi
        ;;
    *)
        echo "skip pe_ir (not a Windows host)"
        ;;
esac

if [ $fail -eq 0 ]; then echo "tl5-lines-check: ok"; else echo "tl5-lines-check: FAILED"; fi
exit $fail
