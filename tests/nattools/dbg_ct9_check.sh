#!/usr/bin/env bash
# CT9 (docs/roadmap/complaint-roadmap.md): debug info names Fides functions
# and types as written, with variables and their locations, in every image
# the machine layer writes: Linux ELF (checked by dbg_ct6_check.sh too),
# macOS Mach-O (__DWARF segment, compiler/nat_enc.fd nat_macho_debug_cfi)
# and `--nat` Windows PE (COFF DWARF sections, nat_pe_debug).
#
#   FID=release/fid.exe bash tests/nattools/dbg_ct9_check.sh
#
# Run from the repository root (Git Bash on Windows, Linux, WSL or macOS;
# python3 or python on PATH). For each image:
#   - test_machine_dbg_names.fd: dbg_names_check.py (Point::len2 with the
#     linkage name Point_len2; struct Point and struct Seg with their
#     members, Seg.a referring to Point);
#   - test_machine_dbg_vars.fd: dbg_vars_check.py (frame base, declaration,
#     the parameters and variables of scale, area, mix, count_bytes and
#     main, each with a type and a location);
#   - Mach-O: macho_dwarf_check.py (the __DWARF segment, the code
#     signature over it, functions and line rows) on both;
#   - `--nat` Windows: built with FID_NAT_DWARF=1 (`-g` sets it once the
#     driver does, see the CT9 notes) and, on Windows, run: its output must
#     be the test's expected lines;
#   - the interpreter-run unit test macho_dbg_types_unit.fd (the sections
#     nat_macho_dwarf_full writes, walked as lldb does).
# With lldb on PATH (macOS), the host's Mach-O is also debugged: a
# breakpoint on 'Point::len2', `frame variable`, `type lookup Seg`; on
# Linux/WSL with gdb, dbg_gdb_check.sh covers the ELF side.
# Exit 0 when every check holds (a target whose build fails is a failure;
# a check that needs a missing tool prints SKIP).
set -u
FID="${FID:-release/fid.exe}"
PY="$(command -v python3 || command -v python)"
OUT="$(mktemp -d)"
fail=0
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | grep '^// ' | grep -v 'Expected output' | sed 's#^// ##'; }
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }

echo "== unit: macho_dbg_types_unit.fd"
if diff <(expected tests/nattools/macho_dbg_types_unit.fd) <("$FID" run tests/nattools/macho_dbg_types_unit.fd 2>/dev/null | tr -d '\r') > "$OUT/unit.diff"; then
    ok "macho_dbg_types_unit"
else
    bad "macho_dbg_types_unit: output differs"; head -20 "$OUT/unit.diff"
fi

for t in aarch64-macos x86_64-macos; do
    for prog in names vars; do
        src="tests/bootstrap/test_machine_dbg_$prog.fd"
        exe="$OUT/$prog-$t"
        if ! "$FID" emit "$src" --target "$t" -g -o "$exe" > "$exe.log" 2>&1; then
            bad "build $prog $t"; tail -3 "$exe.log"; continue
        fi
        if [ "$prog" = names ]; then
            "$PY" tests/nattools/dbg_names_check.py "$exe" && ok "names $t" || bad "names $t"
            "$PY" tests/nattools/macho_dwarf_check.py "$exe" --fn Point::len2 --fn main --file test_machine_dbg_names.fd > "$exe.mdc" 2>&1 \
                && ok "macho_dwarf_check $prog $t" || { bad "macho_dwarf_check $prog $t"; cat "$exe.mdc"; }
        else
            "$PY" tests/nattools/dbg_vars_check.py "$exe" "$src" && ok "vars $t" || bad "vars $t"
            "$PY" tests/nattools/macho_dwarf_check.py "$exe" --fn scale --fn count_bytes > "$exe.mdc" 2>&1 \
                && ok "macho_dwarf_check $prog $t" || { bad "macho_dwarf_check $prog $t"; cat "$exe.mdc"; }
        fi
        # __debug_frame and the variables' types are in the segment
        "$PY" tests/nattools/dbg_sections.py "$exe" | grep -q '^.debug_frame ' && ok "__debug_frame $prog $t" || bad "__debug_frame $prog $t"
    done
done

# lldb on a Mac: the host's image under the debugger
if command -v lldb >/dev/null 2>&1 && [ "$(uname -s)" = Darwin ]; then
    case "$(uname -m)" in arm64) ht=aarch64-macos ;; *) ht=x86_64-macos ;; esac
    exe="$OUT/names-$ht"
    if [ -f "$exe" ]; then
        chmod +x "$exe"
        lldb --batch -o "breakpoint set -n 'Point::len2'" -o run -o "frame variable" -o "type lookup Seg" -o "print self" -o kill "$exe" > "$OUT/lldb.log" 2>&1
        cat "$OUT/lldb.log"
        grep -q "Point::len2" "$OUT/lldb.log" && ok "lldb breakpoint Point::len2" || bad "lldb breakpoint Point::len2"
        grep -Eq "struct Seg|Seg \{" "$OUT/lldb.log" && ok "lldb type lookup Seg" || bad "lldb type lookup Seg"
        grep -Eq "x = 3" "$OUT/lldb.log" && ok "lldb self.x" || bad "lldb self.x"
    fi
else
    echo "SKIP lldb (needs macOS with lldb)"
fi

# --nat Windows: DWARF in COFF sections
for prog in names vars; do
    src="tests/bootstrap/test_machine_dbg_$prog.fd"
    exe="$OUT/$prog-nat.exe"
    exe_arg="$exe"
    if [[ "$FID" == *.exe ]] && command -v wslpath >/dev/null 2>&1; then
        exe_arg="$(wslpath -w "$exe")"
    elif command -v cygpath >/dev/null 2>&1; then
        exe_arg="$(cygpath -w "$exe")"
    fi
    if ! FID_NAT_DWARF=1 WSLENV="${WSLENV:+$WSLENV:}FID_NAT_DWARF" "$FID" emit "$src" --nat -g -o "$exe_arg" > "$exe.log" 2>&1; then
        bad "build $prog --nat"; tail -3 "$exe.log"; continue
    fi
    if [ "$prog" = names ]; then
        "$PY" tests/nattools/dbg_names_check.py "$exe" && ok "names --nat" || bad "names --nat"
    else
        "$PY" tests/nattools/dbg_vars_check.py "$exe" "$src" && ok "vars --nat" || bad "vars --nat"
    fi
    "$PY" tests/nattools/dbg_sections.py "$exe" | grep -q '^.debug_info ' && ok ".debug_info $prog --nat" || bad ".debug_info $prog --nat"
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*)
            if diff <(expected "$src") <("$exe" 2>&1 | tr -d '\r') > /dev/null; then
                ok "run $prog --nat"
            else
                bad "run $prog --nat: output differs"
            fi ;;
        *) echo "SKIP run $prog --nat (not on Windows)" ;;
    esac
done

rm -rf "$OUT"
[ $fail = 0 ] && echo "CT9 debug checks OK"
exit $fail
