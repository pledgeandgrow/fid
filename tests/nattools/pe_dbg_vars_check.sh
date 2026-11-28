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
#    scripts/pe-dwarf-check.py reads its line table. BG243:
#    tests/nattools/pe_dwarf_inl_unit.fd writes inlined calls the same way
#    and tests/nattools/pe_dwarf_inl_check.py decodes them (an abstract
#    DW_TAG_subprogram, a DW_TAG_inlined_subroutine per call;
#    llvm-dwarfdump --verify).
# 2. tests/bootstrap/test_pe_dbg_vars.fd built with `emit --debug` and with
#    `emit -g`: the program prints its expected output; pe-dwarf-check.py
#    passes; tests/nattools/pe_dbg_run_check.py runs it under the Windows
#    debugging API (break at .fd lines and at functions, step over a line,
#    read every variable through the DWARF): --strict for --debug (every
#    value available), without for -g (the mid-end may promote a variable
#    away: optimized out is allowed, a wrong value is not). BG243: -g
#    inlines scale and area and folds scale away; their lines still have
#    rows (one-byte nops where no code was kept), every marker is hit, and
#    `// dbg-inlined` says they are described as inlined.
# 3. FID_PE_OFF=dbgvars: the three sections of before (no .debug_frame),
#    still passing pe-dwarf-check.py; FID_PE_DEBUG_RA=1 (the register
#    allocator kept): same output, no wrong value. FID_PE_OFF=dbginl (BG243
#    off): the -g check sees lines 29, 30 and 35 without rows again.
# 4. binutils' objdump (native, x86_64-w64-mingw32-objdump or through WSL)
#    decodes .debug_info, .debug_frame and .debug_line without a warning
#    and lists the variables; gdb and lldb, when on PATH, in a scripted
#    session: break at the line of @scale_add, print the locals and
#    arguments (acc = 42, base = 6), step to the next line (30). BG243:
#    gdb on the -g build stops at line 29 in scale's inlined frame (bt:
#    scale, then main); llvm-dwarfdump --verify of the -g build (PATH,
#    LLVM_DWARFDUMP or the usual Windows install). An lldb that cannot
#    start is skipped.
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

# 1b. BG243: inlined calls in the DWARF writer
UI=tests/nattools/pe_dwarf_inl_unit.fd
if PE_DWARF_OUT="$(winpath "$OUT/inl_unit.exe")" "$FID" run "$UI" > "$OUT/inl_unit.txt" 2> "$OUT/inl_unit.err"; then
    grep -v '^\[' "$OUT/inl_unit.txt" > "$OUT/inl_unit.out"
    if diff <(expected "$UI") "$OUT/inl_unit.out" > "$OUT/inl_unit.diff"; then ok "inlined-calls unit output"; else no "inlined-calls unit output: $(tr '\n' ' ' < "$OUT/inl_unit.diff")"; fi
    if "$PY" tests/nattools/pe_dwarf_inl_check.py "$OUT/inl_unit.exe" > "$OUT/inl_unit.chk" 2>&1; then ok "inlined-calls unit DWARF decoded"; else no "inlined-calls unit DWARF: $(tr '\n' ' ' < "$OUT/inl_unit.chk")"; fi
else
    no "fid run $UI: $(tail -3 "$OUT/inl_unit.err" | tr '\n' ' ')"
fi
# BG243: pe_ir's nops for an inlined body folded away (a hand-made module;
# interpreted, so slow: SKIP_NOPS_UNIT=1 leaves it out)
UN=tests/nattools/pe_inl_nops_unit.fd
if [ -z "${SKIP_NOPS_UNIT:-}" ]; then
    if PE_INL_OUT="$(winpath "$OUT/inl_nops.exe")" PE_INL_OUT_PLAIN="$(winpath "$OUT/inl_nops_plain.exe")" "$FID" run "$UN" > "$OUT/inl_nops.txt" 2> "$OUT/inl_nops.err"; then
        grep -v '^\[' "$OUT/inl_nops.txt" > "$OUT/inl_nops.out"
        if diff <(expected "$UN") "$OUT/inl_nops.out" > "$OUT/inl_nops.diff"; then ok "inlined-body nops unit output"; else no "inlined-body nops unit: $(tr '\n' ' ' < "$OUT/inl_nops.diff")"; fi
        if "$PY" tests/nattools/pe_inl_nops_check.py "$OUT/inl_nops.exe" "$OUT/inl_nops_plain.exe" > "$OUT/inl_nops.chk" 2>&1; then ok "inlined-body nops: rows, nops, DWARF"; else no "inlined-body nops: $(grep FAIL "$OUT/inl_nops.chk" | tr '\n' ' ')"; fi
    else
        no "fid run $UN: $(tail -3 "$OUT/inl_nops.err" | tr '\n' ' ')"
    fi
else
    skip "inlined-body nops unit (SKIP_NOPS_UNIT)"
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
# BG243: FID_PE_OFF=dbginl, the -g build of before: the lines of the
# inlined scale and area that kept no code (29, 30, 35) have no row, and
# nothing describes the inlined calls; the check above sees the difference
exe="$OUT/t_noinl.exe"
if FID_PE_OFF=dbginl "$FID" emit "$T" -g -o "$exe" > "$OUT/emit_noinl.txt" 2>&1; then
    "$PY" tests/nattools/pe_dbg_run_check.py "$exe" "$T" > "$OUT/run_chk_noinl.txt" 2>&1
    if grep -q "line 29 has no row" "$OUT/run_chk_noinl.txt" && grep -q "scale: no code of its own" "$OUT/run_chk_noinl.txt"; then
        ok "FID_PE_OFF=dbginl: inlined lines without rows, as before BG243 (the check sees it)"
    else
        no "FID_PE_OFF=dbginl: the -g check did not see the missing rows: $(grep -E 'FAIL' "$OUT/run_chk_noinl.txt" | head -4 | tr '\n' ' ')"
    fi
else
    no "FID_PE_OFF=dbginl emit: $(tail -3 "$OUT/emit_noinl.txt" | tr '\n' ' ')"
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
    # BG243: in the -g build scale is inlined into main and folded away: a
    # breakpoint on its line 29 stops (on the nop that keeps the row) in
    # scale's inlined frame
    G="$OUT/t_g.exe"
    if command -v gdb > /dev/null 2>&1 && [ -f "$G" ]; then
        gdb -batch -ex "break test_pe_dbg_vars.fd:$L" -ex run -ex bt -ex kill "$G" > "$OUT/gdb_g.txt" 2>&1
        if grep -qE "^#0 .*scale" "$OUT/gdb_g.txt" && grep -qE "^#1 .*main" "$OUT/gdb_g.txt"; then
            ok "gdb -g: break at line $L of the inlined scale, bt: scale inlined in main"
        else
            no "gdb -g: $(tail -6 "$OUT/gdb_g.txt" | tr '\n' ' ')"
        fi
    fi
    # BG243: llvm-dwarfdump's verifier on the -g build (its inlined calls)
    DD="${LLVM_DWARFDUMP:-llvm-dwarfdump}"
    if ! command -v "$DD" > /dev/null 2>&1 && [ -x "/c/Users/$USERNAME/AppData/Local/Programs/llvm-21.1.0/bin/llvm-dwarfdump.exe" ]; then
        DD="/c/Users/$USERNAME/AppData/Local/Programs/llvm-21.1.0/bin/llvm-dwarfdump.exe"
    fi
    if command -v "$DD" > /dev/null 2>&1 && [ -f "$G" ]; then
        "$DD" --verify "$G" > "$OUT/dd_verify.txt" 2>&1
        vr=$?
        "$DD" --debug-info "$G" > "$OUT/dd_info.txt" 2>&1
        if [ $vr -eq 0 ] && grep -q "DW_TAG_inlined_subroutine" "$OUT/dd_info.txt" && grep -A2 "DW_TAG_inlined_subroutine" "$OUT/dd_info.txt" | grep -q '"scale"'; then
            ok "llvm-dwarfdump --verify of the -g build, scale's inlined call described"
        else
            no "llvm-dwarfdump (-g): verify exit $vr: $(grep -iE 'error|warning' "$OUT/dd_verify.txt" | head -3 | tr '\n' ' ')"
        fi
    else
        skip "llvm-dwarfdump (not installed)"
    fi
    # (an lldb that cannot start, as one whose Python is missing, is skipped)
    if command -v lldb > /dev/null 2>&1 && lldb --version > /dev/null 2>&1; then
        lldb -b -o "breakpoint set -f test_pe_dbg_vars.fd -l $L" -o run -o "frame variable" -o "thread step-over" -o "frame info" -o kill "$exe" > "$OUT/lldb.txt" 2>&1
        if grep -q "acc = 42" "$OUT/lldb.txt" && grep -q "base = 6" "$OUT/lldb.txt" && grep -q "test_pe_dbg_vars.fd:$((L + 1))" "$OUT/lldb.txt"; then ok "lldb: break, frame variable, step-over"; else no "lldb: $(tail -8 "$OUT/lldb.txt" | tr '\n' ' ')"; fi
    else
        skip "lldb (not installed)"
    fi
fi

[ -n "$skipped" ] && echo "skipped:$skipped"
echo "pe_dbg_vars_check: $pass passed, $fail failed"
[ "$fail" -eq 0 ]
