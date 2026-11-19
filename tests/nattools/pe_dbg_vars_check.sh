#!/usr/bin/env bash
# Release N9 / llvm-roadmap DB4 for the default Windows backend (pe_ir):
# source-level debugging of `fid emit --debug` / `-g` builds. Run on
# Windows (Git Bash) from the repository root:
#
#   FID=release/fid.exe bash tests/nattools/pe_dbg_vars_check.sh
#
# 1. tests/nattools/pe_dwarf_vars_unit.fd (pe_add_dwarf on a stand-in
#    image) prints its expected output; tests/nattools/pe_dwarf_vars_check.py
#    decodes the image it writes (subprograms, declaration lines,
#    parameters and variables with their locations and types, FDEs);
#    scripts/pe-dwarf-check.py reads its line table.
# 2. tests/bootstrap/test_pe_dbg_vars.fd built with `emit --debug` and with
#    `emit -g`: the program prints its expected output; pe-dwarf-check.py
#    passes; tests/nattools/pe_dbg_run_check.py runs it under the Windows
#    debugging API (break at .fd lines and at functions, step over a line,
#    read every variable through the DWARF): --strict for --debug (every
#    value available), without for -g (the mid-end may promote a variable
#    away: optimized out is allowed, a wrong value is not).
# 3. FID_PE_OFF=dbgvars: the three sections of before (no .debug_frame),
#    still passing pe-dwarf-check.py; FID_PE_DEBUG_RA=1 (the register
#    allocator kept): same output, no wrong value.
# 4. binutils' objdump (native, x86_64-w64-mingw32-objdump or through WSL)
#    decodes .debug_info, .debug_frame and .debug_line without a warning
#    and lists the variables; gdb and lldb, when on PATH, in a scripted
#    session: break at the line of @scale_add, print the locals and
#    arguments (acc = 42, base = 6), step to the next line (30).
# Passing: the last line is "pe_dbg_vars_check: N passed, 0 failed"
# (a tool that is not installed is listed as skipped, not counted).
set -u
FID="${FID:-release/fid.exe}"
PY="${PY:-python}"
OUT="${OUT:-${TMPDIR:-/tmp}/pe_dbg_vars_check}"
T=tests/bootstrap/test_pe_dbg_vars.fd
U=tests/nattools/pe_dwarf_vars_unit.fd
mkdir -p "$OUT"
pass=0
fail=0
skipped=""
ok() { echo "  ok   $1"; pass=$((pass + 1)); }
no() { echo "  FAIL $1"; fail=$((fail + 1)); }
skip() { echo "  skip $1"; skipped="$skipped $1;"; }

# the "// Expected output:" block of a test file
expected() {
    awk '/^\/\/ Expected output:/{on=1; next} on && /^\/\/ /{sub(/^\/\/ /, ""); print; next} on{exit}' "$1"
}
winpath() {
    if command -v cygpath > /dev/null 2>&1; then cygpath -w "$1"; else echo "$1"; fi
}

# 1. the writer on a stand-in image
if PE_DWARF_OUT="$(winpath "$OUT/unit.exe")" "$FID" run "$U" > "$OUT/unit.txt" 2> "$OUT/unit.err"; then
    grep -v '^\[' "$OUT/unit.txt" > "$OUT/unit.out"
    if diff <(expected "$U") "$OUT/unit.out" > "$OUT/unit.diff"; then ok "unit output"; else no "unit output: $(tr '\n' ' ' < "$OUT/unit.diff")"; fi
    if "$PY" tests/nattools/pe_dwarf_vars_check.py "$OUT/unit.exe" > "$OUT/unit.chk" 2>&1; then ok "unit DWARF decoded"; else no "unit DWARF: $(tr '\n' ' ' < "$OUT/unit.chk")"; fi
    if "$PY" scripts/pe-dwarf-check.py "$OUT/unit.exe" pe_dwarf_vars_unit.fd > "$OUT/unit.pdc" 2>&1; then ok "unit pe-dwarf-check"; else no "unit pe-dwarf-check: $(tr '\n' ' ' < "$OUT/unit.pdc")"; fi
else
    no "fid run $U: $(tail -3 "$OUT/unit.err" | tr '\n' ' ')"
fi

# 2. a real program, --debug and -g
for mode in debug g; do
    exe="$OUT/t_$mode.exe"
    flag="--debug"
    strict="--strict"
    if [ "$mode" = g ]; then
        flag="-g"
        strict=""
    fi
    rm -f "$exe"
    if ! "$FID" emit "$T" $flag -o "$exe" > "$OUT/emit_$mode.txt" 2>&1; then
        no "emit $flag: $(tail -3 "$OUT/emit_$mode.txt" | tr '\n' ' ')"
        continue
    fi
    "$exe" > "$OUT/run_$mode.out" 2>&1
    if diff <(expected "$T") "$OUT/run_$mode.out" > /dev/null; then ok "$flag: output"; else no "$flag: output $(tr '\n' ' ' < "$OUT/run_$mode.out")"; fi
    if "$PY" scripts/pe-dwarf-check.py "$exe" test_pe_dbg_vars.fd > "$OUT/pdc_$mode.txt" 2>&1; then ok "$flag: pe-dwarf-check"; else no "$flag: pe-dwarf-check $(tr '\n' ' ' < "$OUT/pdc_$mode.txt")"; fi
    if "$PY" tests/nattools/pe_dbg_run_check.py "$exe" "$T" $strict > "$OUT/run_chk_$mode.txt" 2>&1; then
        ok "$flag: debugger run ($(tail -2 "$OUT/run_chk_$mode.txt" | head -1))"
    else
        no "$flag: debugger run: $(grep -E 'FAIL|available' "$OUT/run_chk_$mode.txt" | head -12 | tr '\n' ' ')"
    fi
done

# 3. the switches
exe="$OUT/t_off.exe"
if FID_PE_OFF=dbgvars "$FID" emit "$T" --debug -o "$exe" > "$OUT/emit_off.txt" 2>&1; then
    if "$PY" scripts/pe-dwarf-check.py "$exe" test_pe_dbg_vars.fd > "$OUT/pdc_off.txt" 2>&1 && \
       "$PY" -c "import sys; sys.path.insert(0, 'tests/nattools'); import dbg_sections as s; d = open(sys.argv[1], 'rb').read(); sys.exit(0 if s.section(d, '.debug_frame') is None and s.section(d, '.debug_line') is not None else 1)" "$exe"; then
        ok "FID_PE_OFF=dbgvars: the line tables of before"
    else
        no "FID_PE_OFF=dbgvars: $(tr '\n' ' ' < "$OUT/pdc_off.txt")"
    fi
else
    no "FID_PE_OFF=dbgvars emit: $(tail -3 "$OUT/emit_off.txt" | tr '\n' ' ')"
fi
exe="$OUT/t_ra.exe"
if FID_PE_DEBUG_RA=1 "$FID" emit "$T" --debug -o "$exe" > "$OUT/emit_ra.txt" 2>&1; then
    "$exe" > "$OUT/run_ra.out" 2>&1
    if diff <(expected "$T") "$OUT/run_ra.out" > /dev/null && "$PY" tests/nattools/pe_dbg_run_check.py "$exe" "$T" > "$OUT/run_chk_ra.txt" 2>&1; then
        ok "FID_PE_DEBUG_RA=1: output, no wrong value ($(tail -2 "$OUT/run_chk_ra.txt" | head -1))"
    else
        no "FID_PE_DEBUG_RA=1: $(grep -E 'FAIL' "$OUT/run_chk_ra.txt" | head -8 | tr '\n' ' ')"
    fi
else
    no "FID_PE_DEBUG_RA=1 emit: $(tail -3 "$OUT/emit_ra.txt" | tr '\n' ' ')"
fi

# 4. other readers: objdump, gdb, lldb
exe="$OUT/t_debug.exe"
OD=""
if command -v x86_64-w64-mingw32-objdump > /dev/null 2>&1; then
    OD="x86_64-w64-mingw32-objdump"
elif command -v objdump > /dev/null 2>&1 && objdump --help 2>/dev/null | grep -q pei-x86-64; then
    OD="objdump"
fi
if [ -f "$exe" ]; then
    if [ -n "$OD" ]; then
        "$OD" --dwarf=info,frames,decodedline "$exe" > "$OUT/od.txt" 2>&1
    elif command -v wsl > /dev/null 2>&1 && wsl -e objdump --version > /dev/null 2>&1; then
        wsl -e objdump --dwarf=info,frames,decodedline "$(wsl -e wslpath "$(winpath "$exe")")" > "$OUT/od.txt" 2>&1
        OD="wsl objdump"
    fi
    if [ -n "$OD" ]; then
        if grep -qiE "warning|error" "$OUT/od.txt"; then
            no "$OD: $(grep -iE 'warning|error' "$OUT/od.txt" | head -3 | tr '\n' ' ')"
        elif grep -q "DW_AT_name *: acc" "$OUT/od.txt" && grep -q "DW_AT_name *: step" "$OUT/od.txt" && grep -q "DW_OP_call_frame_cfa" "$OUT/od.txt" && grep -q "FDE" "$OUT/od.txt"; then
            ok "$OD reads .debug_info, .debug_frame and .debug_line"
        else
            no "$OD: the variables or frames are missing (see $OUT/od.txt)"
        fi
    else
        skip "objdump (not installed)"
    fi
    L=$(grep -n "// @scale_add" "$T" | cut -d: -f1)
    if command -v gdb > /dev/null 2>&1; then
        gdb -batch -ex "break test_pe_dbg_vars.fd:$L" -ex run -ex "info args" -ex "info locals" -ex next -ex "info line" -ex kill "$exe" > "$OUT/gdb.txt" 2>&1
        if grep -q "acc = 42" "$OUT/gdb.txt" && grep -q "base = 6" "$OUT/gdb.txt" && grep -qE "Line $((L + 1))" "$OUT/gdb.txt"; then ok "gdb: break, info locals, next"; else no "gdb: $(tail -8 "$OUT/gdb.txt" | tr '\n' ' ')"; fi
    else
        skip "gdb (not installed)"
    fi
    if command -v lldb > /dev/null 2>&1; then
        lldb -b -o "breakpoint set -f test_pe_dbg_vars.fd -l $L" -o run -o "frame variable" -o "thread step-over" -o "frame info" -o kill "$exe" > "$OUT/lldb.txt" 2>&1
        if grep -q "acc = 42" "$OUT/lldb.txt" && grep -q "base = 6" "$OUT/lldb.txt" && grep -q "test_pe_dbg_vars.fd:$((L + 1))" "$OUT/lldb.txt"; then ok "lldb: break, frame variable, step-over"; else no "lldb: $(tail -8 "$OUT/lldb.txt" | tr '\n' ' ')"; fi
    else
        skip "lldb (not installed)"
    fi
fi

[ -n "$skipped" ] && echo "skipped:$skipped"
echo "pe_dbg_vars_check: $pass passed, $fail failed"
[ "$fail" -eq 0 ]
