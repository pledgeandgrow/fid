#!/bin/bash
# Competitive SL15: a sampling profiler reachable from a running program.
#   FID=release/fid.exe bash tests/prof/sl15-check.sh
# Run from the repository root. Checks:
#  1. tests/bootstrap/test_prof_sampling.fd (stdlib/profiler.fd prof_start /
#     prof_stop / prof_report / prof_samples) on every backend this machine
#     has: `fid run` (compiler/interp.fd, counting calls), `fid ccompile`
#     (compiler/codegen_c.fd emit_prof_api_runtime), `fid emit` (Windows:
#     pe_ir and stdlib/pe_runtime/conc.fd; elsewhere nat_ir and
#     stdlib/linux_runtime/sys.fd), and `fid emit --target x86_64-linux`
#     run under WSL from Windows: stdout is the file's "Expected output",
#     stderr holds the table with prof_burn in it.
#  2. Windows `fid emit` only: tests/prof/sl15_service.fd under FID_PROF=1
#     prints the profile when its named event is set (by the program
#     itself, and by this script through PowerShell from outside) and at
#     exit; under FID_PROF_EVERY=1 every second too; without FID_PROF there
#     is no event and no report.
# Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
T=tests/bootstrap/test_prof_sampling.fd
SVC=tests/prof/sl15_service.fd
D=$(mktemp -d ./_sl15_check.XXXXXX)
D=${D#./}
BG=""
cleanup() {
    [ -n "$BG" ] && kill "$BG" 2>/dev/null
    rm -rf "$D"
}
trap cleanup EXIT
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
EXE=""
WIN=0
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe"; WIN=1 ;; esac

# The expected stdout: the "// Expected output:" lines of the test
sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$T" | grep '^// ' | grep -v 'Expected output:' | sed 's#^// ##' > "$D/want.txt"

check_run() {   # name, stdout file, stderr file
    if diff -q "$D/want.txt" "$2" > /dev/null; then
        ok "$1: output"
    else
        bad "$1: output differs"; diff "$D/want.txt" "$2"
    fi
    if grep -q "samples, one per" "$3" && grep -q "  prof_burn$" "$3"; then
        ok "$1: the table names prof_burn"
    else
        bad "$1: no table with prof_burn on stderr"; cat "$3"
    fi
}

# 1a. the interpreter (counting calls)
"$FID" run "$T" > "$D/run.out" 2> "$D/run.err"
check_run "fid run" "$D/run.out" "$D/run.err"

# 1b. through C
if "$FID" ccompile "$T" -o "$D/t_c$EXE" > "$D/cc.log" 2>&1; then
    "./$D/t_c$EXE" > "$D/c.out" 2> "$D/c.err"
    check_run "ccompile" "$D/c.out" "$D/c.err"
else
    bad "ccompile failed"; tail -20 "$D/cc.log"
fi

# 1c. the own native backend of this machine
if "$FID" emit "$T" -o "$D/t_n$EXE" > "$D/emit.log" 2>&1; then
    "./$D/t_n$EXE" > "$D/n.out" 2> "$D/n.err"
    check_run "emit" "$D/n.out" "$D/n.err"
else
    bad "emit failed"; tail -20 "$D/emit.log"
fi

# 1d. Windows: the Linux x86-64 build under WSL
if [ $WIN -eq 1 ] && command -v wsl.exe > /dev/null 2>&1; then
    if "$FID" emit "$T" --target x86_64-linux -o "$D/t_lx" > "$D/lx.log" 2>&1; then
        WIN_DIR=$(cygpath -m "$PWD/$D")
        WD=$(MSYS_NO_PATHCONV=1 wsl.exe -e wslpath -a "$WIN_DIR" 2> /dev/null | tr -d '\r')
        if [ -n "$WD" ] && MSYS_NO_PATHCONV=1 wsl.exe -e sh -c "cd '$WD' && chmod +x t_lx && ./t_lx > lx.out 2> lx.err"; then
            check_run "emit x86_64-linux (WSL)" "$D/lx.out" "$D/lx.err"
        else
            bad "emit x86_64-linux (WSL): could not resolve or run $WIN_DIR"
        fi
    else
        bad "emit --target x86_64-linux failed"; tail -20 "$D/lx.log"
    fi
fi

# 2. Windows native: FID_PROF, the named event, FID_PROF_EVERY
if [ $WIN -eq 1 ]; then
    if "$FID" emit "$SVC" -o "$D/svc.exe" > "$D/svc.log" 2>&1; then
        "./$D/svc.exe" > "$D/s0.out" 2> "$D/s0.err"
        if [ "$(tr -d '\r' < "$D/s0.out")" = "$(printf 'no event\ndone')" ] && ! grep -q "profile (FID_PROF)" "$D/s0.err"; then
            ok "service without FID_PROF: no event, no report"
        else
            bad "service without FID_PROF"; cat "$D/s0.out" "$D/s0.err"
        fi
        FID_PROF=1 "./$D/svc.exe" > "$D/s1.out" 2> "$D/s1.err"
        n=$(grep -c "profile (FID_PROF)" "$D/s1.err")
        if [ "$(tr -d '\r' < "$D/s1.out")" = "$(printf 'event set\ndone')" ] && [ "$n" -ge 2 ] && grep -q "  svc_work$" "$D/s1.err"; then
            ok "FID_PROF=1: a report on the event and one at exit ($n)"
        else
            bad "FID_PROF=1: $n report(s)"; cat "$D/s1.out" "$D/s1.err"
        fi
        FID_PROF=1 FID_PROF_EVERY=1 "./$D/svc.exe" > "$D/s2.out" 2> "$D/s2.err"
        n=$(grep -c "profile (FID_PROF)" "$D/s2.err")
        if [ "$n" -ge 3 ]; then
            ok "FID_PROF_EVERY=1: $n reports in 1.6 s"
        else
            bad "FID_PROF_EVERY=1: $n report(s)"; cat "$D/s2.err"
        fi
        # From another process: PowerShell sets the event while it runs
        if command -v powershell.exe > /dev/null 2>&1; then
            FID_PROF=1 "./$D/svc.exe" > "$D/s3.out" 2> "$D/s3.err" &
            BG=$!
            sleep 0.4
            WPID=$BG
            [ -r "/proc/$BG/winpid" ] && WPID=$(cat "/proc/$BG/winpid")
            powershell.exe -NoProfile -Command "[System.Threading.EventWaitHandle]::OpenExisting('Local\\fid-prof-$WPID').Set() | Out-Null" > /dev/null 2>&1
            wait "$BG"
            BG=""
            n=$(grep -c "profile (FID_PROF)" "$D/s3.err")
            if [ "$n" -ge 3 ]; then
                ok "an event set from another process: $n reports"
            else
                bad "an event set from another process: $n report(s)"; cat "$D/s3.err"
            fi
        fi
    else
        bad "emit $SVC failed"; tail -20 "$D/svc.log"
    fi
fi
exit $fail
