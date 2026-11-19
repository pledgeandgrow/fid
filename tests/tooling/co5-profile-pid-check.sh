#!/bin/bash
# CO5 (docs/roadmap/complaint-roadmap.md), competitive SL15: `fid profile
# --pid <N> [--seconds S]` attaches to a program that is already running
# (built through C), samples every thread and writes the same collapsed
# stacks as `fid profile <file.fd>` (compiler/codegen_c.fd cg_prof_attach,
# cg_prof_attach_c; the table it reads is __p_prof_anchor). Windows:
# SuspendThread/GetThreadContext + dbghelp StackWalk64; Linux: ptrace.
#   FID=release/fid.exe bash tests/tooling/co5-profile-pid-check.sh
# Run from the repository root. Needs a C compiler (CC, or zig/gcc/clang on
# the PATH). Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
P=tests/projects/co5_service/service.fd
# relative paths: Git Bash would rewrite a /tmp path given to a Windows
# program
D=$(mktemp -d ./_co5_check.XXXXXX)
D=${D#./}
SVC=""
OTHER=""
cleanup() {
    [ -n "$SVC" ] && kill "$SVC" 2>/dev/null
    [ -n "$OTHER" ] && kill "$OTHER" 2>/dev/null
    rm -rf "$D"
}
trap cleanup EXIT
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
EXE=""
WIN=0
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe"; WIN=1 ;; esac
# the process id the profiler takes: Windows' own one under Git Bash
os_pid() {
    if [ $WIN -eq 1 ] && [ -r "/proc/$1/winpid" ]; then cat "/proc/$1/winpid"; else echo "$1"; fi
}

# 1. every program built through C holds the anchor the profiler finds
if "$FID" dump-c "$P" 2> /dev/null | grep -q "const __p_prof_anchor_t __p_prof_anchor = { { 0x31464F5250444946ULL, 0x2121524F48434E41ULL }, &__p_prof_anchor, __p_fns, &__p_fn_count };"; then
    ok "the C defines __p_prof_anchor"
else
    bad "no __p_prof_anchor in the C of $P"
fi

# 2. build the service and start it (FID_PROF_ATTACH=1: Linux with Yama
# ptrace_scope 1 lets only an ancestor attach otherwise)
if ! "$FID" ccompile "$P" -o "$D/svc$EXE" > "$D/build.txt" 2>&1; then
    bad "ccompile $P"; cat "$D/build.txt"; exit 1
fi
FID_PROF_ATTACH=1 "$D/svc$EXE" > "$D/svc.out" 2>&1 &
SVC=$!
sleep 1
TARGET=$(os_pid $SVC)

# 3. attach for 2 seconds
"$FID" profile --pid "$TARGET" --seconds 2 -o "$D/p.folded" > "$D/report.txt" 2>&1
code=$?
if [ $code -eq 0 ]; then ok "fid profile --pid exits 0"; else bad "fid profile --pid exit $code"; fi
if kill -0 $SVC 2>/dev/null; then ok "the service still runs after the profiler detached"; else bad "the service stopped"; fi
F="$D/p.folded"
total=$(grep -c . "$F" 2>/dev/null); total=${total:-0}
hot=$(grep -c "co5_hot 1\$" "$F" 2>/dev/null); hot=${hot:-0}
cold=$(grep -c "co5_cold 1\$" "$F" 2>/dev/null); cold=${cold:-0}
if [ "$total" -ge 100 ]; then ok "$total samples"; else bad "only $total samples in $F"; fi
if [ "$(grep -vc ' 1$' "$F" 2>/dev/null)" = "0" ]; then ok "one line per sample (\"stack 1\")"; else bad "a line without its count"; fi
if [ "$hot" -gt 0 ] && [ "$cold" -gt 0 ]; then ok "both functions named ($hot co5_hot, $cold co5_cold)"; else bad "co5_hot $hot, co5_cold $cold"; fi
# co5_hot does three times co5_cold's work
if [ "$hot" -gt $((cold * 3 / 2)) ]; then ok "co5_hot takes most samples"; else bad "co5_hot $hot not above 1.5 x co5_cold $cold"; fi
if grep -q "^main;.*co5_hot;co5_hot 1\$" "$F"; then ok "stacks run from main to the leaf"; else bad "no stack main;...;co5_hot;co5_hot"; fi
if grep -qF "Profile of process $TARGET" "$D/report.txt" && grep -q "co5_hot" "$D/report.txt"; then ok "the report names the process and co5_hot"; else bad "report"; fi
kill $SVC 2>/dev/null
SVC=""

# 4. a process that is not a Fides program: an error, no samples
sleep 30 &
OTHER=$!
OP=$(os_pid $OTHER)
if "$FID" profile --pid "$OP" --seconds 1 -o "$D/o.folded" > "$D/o.txt" 2>&1; then
    bad "profile --pid of a non-Fides process exits 0"
elif grep -q "profile: " "$D/o.txt"; then
    ok "a non-Fides process: $(grep 'profile: ' "$D/o.txt" | head -1)"
else
    bad "a non-Fides process: no message"; cat "$D/o.txt"
fi
kill $OTHER 2>/dev/null
OTHER=""

# 5. a process that is gone
if "$FID" profile --pid 999999 --seconds 1 -o "$D/g.folded" > "$D/g.txt" 2>&1; then
    bad "profile --pid of no process exits 0"
else
    ok "no such process: $(grep 'profile: ' "$D/g.txt" | head -1)"
fi

# 6. Linux, Yama ptrace_scope 1, no FID_PROF_ATTACH: the message says what to do
if [ $WIN -eq 0 ] && [ "$(cat /proc/sys/kernel/yama/ptrace_scope 2>/dev/null)" = "1" ] && [ "$(id -u)" != "0" ]; then
    "$D/svc$EXE" > /dev/null 2>&1 &
    SVC=$!
    sleep 0.5
    if "$FID" profile --pid "$SVC" --seconds 1 -o "$D/y.folded" > "$D/y.txt" 2>&1; then
        bad "ptrace_scope 1 without FID_PROF_ATTACH attached"
    elif grep -q "FID_PROF_ATTACH=1" "$D/y.txt"; then
        ok "ptrace_scope 1: the message names FID_PROF_ATTACH"
    else
        bad "ptrace_scope 1: $(cat "$D/y.txt")"
    fi
    kill $SVC 2>/dev/null
    SVC=""
fi

if [ $fail -ne 0 ]; then
    echo "--- report ---"; cat "$D/report.txt"
    echo "--- folded (counts) ---"; sort "$F" 2>/dev/null | uniq -c | sort -rn | head -10
    exit 1
fi
echo "all CO5 checks passed"
exit 0
