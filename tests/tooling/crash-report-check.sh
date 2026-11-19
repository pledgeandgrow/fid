#!/bin/bash
# CO9 (docs/roadmap/complaint-roadmap.md): a panic under `fid run
# --crash-file <file>` (or FID_CRASH_REPORT=<file>) writes a report with the
# panic message, the trace and the build; `fid inspect <file>` prints it
# (compiler/interp.fd interp_write_crash_report, compiler/main.fd
# cmd_inspect). A program built through C writes the same report under
# FID_CRASH_REPORT (codegen_c.fd __p_crash_report), for a panic, a crash or
# a stack overflow, with the task chain (CA9) of a panic in a task.
# (`--crash-report` without a value is RL20's report of a compiler crash.)
#   FID=release/fid.exe bash tests/tooling/crash-report-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
P=tests/projects/co9_crash/crash.fd
# relative paths: Git Bash would rewrite a /tmp path given to a Windows
# program, and the program then names another spelling of it
D=$(mktemp -d ./_co9_check.XXXXXX)
D=${D#./}
trap 'rm -rf "$D"' EXIT
R="$D/r.txt"
R2="$D/r2.txt"
OUT="$D/out.txt"
fail=0
# N12: the version a report names is the one `fid version` prints
FIDVER=$("$FID" version 2>/dev/null | head -1 | tr -d '\r' | sed -n 's/^fid \([^ ]*\) .*/\1/p')
if [ -z "$FIDVER" ]; then echo "FAIL fid version names no version"; fail=1; FIDVER="?"; fi
"$FID" run "$P" --crash-file "$R" > /dev/null 2> "$OUT"
code=$?
if [ $code -ne 0 ]; then echo "ok   the run fails"; else echo "FAIL the run exits 0"; fail=1; fi
if [ -s "$R" ]; then echo "ok   report written"; else echo "FAIL no report at $R"; fail=1; fi
if grep -qF "crash report: $R" "$OUT"; then echo "ok   stderr names the report"; else echo "FAIL stderr does not name the report"; fail=1; fi
if [ "$(head -1 "$R" | tr -d '\r')" = "fid-crash-report 1" ]; then echo "ok   report header"; else echo "FAIL report header"; fail=1; fi
"$FID" inspect "$R" > "$OUT" 2>&1
icode=$?
has() {
    if grep -qF -- "$2" "$OUT"; then echo "ok   $1"; else echo "FAIL $1: no line with: $2"; fail=1; fi
}
if [ $icode -eq 0 ]; then echo "ok   fid inspect reads it"; else echo "FAIL fid inspect exit $icode"; fail=1; fi
has "program" "Crash of $P"
has "panic message" "panic:   empty age field"
has "build version" "build:   fid $FIDVER, build "
has "trace: innermost" "parse_age"
has "trace: caller" "load_record"
has "trace: main" "main"
# the environment variable asks for one too
FID_CRASH_REPORT="$R2" "$FID" run "$P" > /dev/null 2>&1
if [ -s "$R2" ] && grep -qF "message: empty age field" "$R2"; then echo "ok   FID_CRASH_REPORT"; else echo "FAIL FID_CRASH_REPORT wrote no report"; fail=1; fi
# not a report
printf 'hello\n' > "$R2"
if "$FID" inspect "$R2" > /dev/null 2>&1; then echo "FAIL fid inspect accepted a non-report"; fail=1; else echo "ok   fid inspect rejects a non-report"; fi
# a program that does not panic writes nothing
rm -f "$R2"
"$FID" run tests/bootstrap/hello.fd --crash-file "$R2" > /dev/null 2>&1
if [ -e "$R2" ]; then echo "FAIL a report without a panic"; fail=1; else echo "ok   no report without a panic"; fi
if [ $fail -ne 0 ]; then echo "--- report ---"; cat "$R" 2>/dev/null; echo "--- inspect ---"; cat "$OUT"; fi

# --- built through C: FID_CRASH_REPORT ---------------------------------------
EXE=""
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;; esac
if "$FID" ccompile "$P" -o "$D/crash$EXE" > "$D/build.txt" 2>&1; then
    "$D/crash$EXE" > /dev/null 2> "$D/plain.err"
    if grep -qF "crash report:" "$D/plain.err"; then echo "FAIL C: a report without FID_CRASH_REPORT"; fail=1; else echo "ok   C: no report unless asked"; fi
    FID_CRASH_REPORT="$D/c.txt" "$D/crash$EXE" > /dev/null 2> "$D/c.err"
    code=$?
    if [ $code -eq 1 ]; then echo "ok   C: the program fails"; else echo "FAIL C: exit $code"; fail=1; fi
    if [ "$(head -1 "$D/c.txt" 2>/dev/null | tr -d '\r')" = "fid-crash-report 1" ] && grep -qF "message: empty age field" "$D/c.txt" && grep -qF "version: fid $FIDVER" "$D/c.txt" && grep -qx "end" "$D/c.txt"; then echo "ok   C: report written"; else echo "FAIL C: no or bad report"; cat "$D/c.txt" 2>/dev/null; fail=1; fi
    if grep -qF "crash report: $D/c.txt" "$D/c.err"; then echo "ok   C: stderr names the report"; else echo "FAIL C: stderr does not name the report"; fail=1; fi
    "$FID" inspect "$D/c.txt" > "$OUT" 2>&1
    if [ $? -eq 0 ] && grep -qF "panic:   empty age field" "$OUT" && grep -qF "build:   fid $FIDVER" "$OUT" && grep -qF "(C)" "$OUT" && grep -qF "main" "$OUT"; then echo "ok   C: fid inspect reads it"; else echo "FAIL C: fid inspect"; cat "$OUT"; fail=1; fi
    # a stack overflow (the crash handler, not p_panic)
    printf 'fn deep(n: i64) -> i64 {\n    return deep(n + 1) + 1;\n}\n\nfn main() {\n    println(deep(0));\n}\n' > "$D/so.fd"
    if "$FID" ccompile "$D/so.fd" -o "$D/so$EXE" > /dev/null 2>&1; then
        FID_CRASH_REPORT="$D/so.txt" "$D/so$EXE" > /dev/null 2>&1
        if grep -qF "message: Stack overflow" "$D/so.txt" 2>/dev/null && grep -qF "deep" "$D/so.txt"; then echo "ok   C: a stack overflow writes a report"; else echo "FAIL C: no report of the stack overflow"; cat "$D/so.txt" 2>/dev/null; fail=1; fi
    else
        echo "FAIL C: cannot build the overflow program"; fail=1
    fi
    # a panic in a task: the report holds the task chain (CA9)
    if "$FID" ccompile tests/bootstrap/test_task_chain_fail.fd -o "$D/chain$EXE" > /dev/null 2>&1; then
        FID_CRASH_REPORT="$D/chain.txt" "$D/chain$EXE" > /dev/null 2>&1
        if grep -qF "message: b failed" "$D/chain.txt" 2>/dev/null && grep -qx "task chain: b <- a <- main" "$D/chain.txt"; then echo "ok   C: the report of a task's panic has the task chain"; else echo "FAIL C: task chain in the report"; cat "$D/chain.txt" 2>/dev/null; fail=1; fi
    else
        echo "FAIL C: cannot build test_task_chain_fail.fd"; fail=1
    fi
else
    echo "FAIL C: ccompile failed (needs a C compiler)"; tail -5 "$D/build.txt"; fail=1
fi

# --- built natively (`fid emit`): FID_CRASH_REPORT ---------------------------
# Windows: pe_ir (stdlib/pe_runtime/base.fd pert_crash_report, called by the
# p_panic stub) and `--nat` (stdlib/linux_runtime/base.fd pe_crash_report
# over win.fd); Linux: nat_ir (the same linux_runtime code). WSL, when it is
# there, also runs the Linux build from Windows.
# native_check <label> <exe> [runner]: the report of crash.fd's panic
native_check() {
    lbl=$1; exe=$2; run=$3
    rm -f "$D/n.txt"
    $run "$exe" > /dev/null 2> "$D/n.err"
    if grep -qF "crash report:" "$D/n.err"; then echo "FAIL $lbl: a report without FID_CRASH_REPORT"; fail=1; else echo "ok   $lbl: no report unless asked"; fi
    if [ -n "$run" ]; then
        $run env FID_CRASH_REPORT="$D/n.txt" "$exe" > /dev/null 2> "$D/n.err"
    else
        FID_CRASH_REPORT="$D/n.txt" "$exe" > /dev/null 2> "$D/n.err"
    fi
    code=$?
    if [ $code -eq 1 ]; then echo "ok   $lbl: the program fails"; else echo "FAIL $lbl: exit $code"; fail=1; fi
    if [ "$(head -1 "$D/n.txt" 2>/dev/null | tr -d '\r')" = "fid-crash-report 1" ] && grep -qF "message: empty age field" "$D/n.txt" && grep -qF "version: fid $FIDVER" "$D/n.txt" && grep -qF "(native)" "$D/n.txt" && grep -qx "end" "$D/n.txt"; then echo "ok   $lbl: report written"; else echo "FAIL $lbl: no or bad report"; cat "$D/n.txt" 2>/dev/null; fail=1; fi
    if grep -qF "crash report: $D/n.txt" "$D/n.err"; then echo "ok   $lbl: stderr names the report"; else echo "FAIL $lbl: stderr does not name the report"; cat "$D/n.err"; fail=1; fi
    # (native code may inline load_record: its frame is not required)
    if grep -qF "parse_age" "$D/n.txt" && grep -qF "called from main" "$D/n.txt"; then echo "ok   $lbl: the report's trace"; else echo "FAIL $lbl: trace (parse_age ... main)"; cat "$D/n.txt" 2>/dev/null; fail=1; fi
    "$FID" inspect "$D/n.txt" > "$OUT" 2>&1
    if [ $? -eq 0 ] && grep -qF "panic:   empty age field" "$OUT" && grep -qF "build:   fid $FIDVER" "$OUT" && grep -qF "parse_age" "$OUT"; then echo "ok   $lbl: fid inspect reads it"; else echo "FAIL $lbl: fid inspect"; cat "$OUT"; fail=1; fi
}
# native_so <label> <exe> [runner]: the report of a stack overflow
native_so() {
    lbl=$1; exe=$2; run=$3
    rm -f "$D/nso.txt"
    if [ -n "$run" ]; then
        $run env FID_CRASH_REPORT="$D/nso.txt" "$exe" > /dev/null 2>&1
    else
        FID_CRASH_REPORT="$D/nso.txt" "$exe" > /dev/null 2>&1
    fi
    if grep -qF "message: Stack overflow" "$D/nso.txt" 2>/dev/null && grep -qx "end" "$D/nso.txt"; then echo "ok   $lbl: a stack overflow writes a report"; else echo "FAIL $lbl: no report of the stack overflow"; cat "$D/nso.txt" 2>/dev/null; fail=1; fi
}
# the overflow program (written again: the C section makes it only when it builds)
printf 'fn deep(n: i64) -> i64 {\n    return deep(n + 1) + 1;\n}\n\nfn main() {\n    println(deep(0));\n}\n' > "$D/so.fd"
if [ -n "$EXE" ]; then
    if "$FID" emit "$P" -o "$D/crash_pe.exe" > "$D/nbuild.txt" 2>&1; then
        native_check "native (pe_ir)" "$D/crash_pe.exe"
    else
        echo "FAIL native (pe_ir): emit failed"; tail -5 "$D/nbuild.txt"; fail=1
    fi
    if "$FID" emit "$D/so.fd" -o "$D/so_pe.exe" > /dev/null 2>&1; then
        native_so "native (pe_ir)" "$D/so_pe.exe"
    else
        echo "FAIL native (pe_ir): cannot build the overflow program"; fail=1
    fi
    if "$FID" emit "$P" --nat -o "$D/crash_nat.exe" > "$D/nbuild.txt" 2>&1; then
        native_check "native (--nat)" "$D/crash_nat.exe"
    else
        echo "FAIL native (--nat): emit failed"; tail -5 "$D/nbuild.txt"; fail=1
    fi
    if command -v wsl > /dev/null 2>&1 && wsl true > /dev/null 2>&1; then
        if "$FID" emit "$P" --target x86_64-linux -o "$D/crash_lx" > "$D/nbuild.txt" 2>&1; then
            native_check "native (x86_64-linux, WSL)" "./$D/crash_lx" "wsl"
        else
            echo "FAIL native (x86_64-linux): emit failed"; tail -5 "$D/nbuild.txt"; fail=1
        fi
        if "$FID" emit "$D/so.fd" --target x86_64-linux -o "$D/so_lx" > /dev/null 2>&1; then
            native_so "native (x86_64-linux, WSL)" "./$D/so_lx" "wsl"
        fi
    else
        echo "skip native (x86_64-linux): no WSL"
    fi
else
    if "$FID" emit "$P" -o "$D/crash_n" > "$D/nbuild.txt" 2>&1; then
        native_check "native" "./$D/crash_n"
    else
        echo "FAIL native: emit failed"; tail -5 "$D/nbuild.txt"; fail=1
    fi
    if "$FID" emit "$D/so.fd" -o "$D/so_n" > /dev/null 2>&1; then
        native_so "native" "./$D/so_n"
    else
        echo "FAIL native: cannot build the overflow program"; fail=1
    fi
fi
exit $fail
