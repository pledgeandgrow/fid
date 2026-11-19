#!/bin/bash
# llvm-roadmap DB9: the sampling profiler on every backend, every thread,
# and its folded-stacks output (FID_PROF=folded).
#   FID=release/fid.exe bash tests/prof/db9-check.sh
# Run from the repository root. Programs: tests/prof/db9_threads.fd (no
# profiler calls: FID_PROF starts it before main) and tests/prof/db9_api.fd
# (prof_start / prof_stop / prof_report: works without the entry point's
# hook). Both spend ~400 ms in fib on main and on a second OS thread.
# Backends this host has:
#   ccompile (C runtime, compiler/codegen_c.fd emit_prof_api_runtime: SIGPROF
#     on POSIX, a sampler thread suspending every thread on Windows),
#   emit (Windows pe_ir: stdlib/pe_runtime/conc.fd; elsewhere nat_ir),
#   emit --nat (Windows: stdlib/linux_runtime/win.fd + sys.fd),
#   emit --target x86_64-linux under WSL from Windows (sys.fd, SIGPROF).
# Checks per backend:
#   - without FID_PROF: stdout as the file says, nothing on stderr;
#   - FID_PROF=1: the table, "profile (FID_PROF): N samples", N > 0, fib
#     first by self time;
#   - FID_PROF=folded: stdout the same, stderr only "a;b;c N" lines, one
#     stack rooted at main reaching fib, and one of the other thread
#     (rooted elsewhere: thread_work on C, the runtime's thread entry on
#     native) reaching fib: both threads were sampled.
# db9_threads under pe_ir needs the entry point's pert_prof_boot /
# pert_prof_exit calls (SL15 needsOther A+B); until they land those lines
# FAIL with that note while db9_api passes.
# Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
export CC=${CC:-zig cc}
D=$(mktemp -d ./_db9_check.XXXXXX)
D=${D#./}
trap 'rm -rf "$D"' EXIT
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
EXE=""
WIN=0
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe"; WIN=1 ;; esac

want() {   # the "Output" lines of a program's header comment
    sed -n '/Output (stdout), on every backend:/,/^[^/]/p' "$1" \
        | grep '^// ' | grep -v 'Output (stdout)' | sed 's#^// ##'
}
want tests/prof/db9_threads.fd > "$D/want_threads.txt"
want tests/prof/db9_api.fd > "$D/want_api.txt"

table_ok() {   # name, stderr file
    tr -d '\r' < "$2" > "$2.t"
    if grep -qE '^profile \(FID_PROF\): [1-9][0-9]* samples' "$2.t" && sed -n '/self%/{n;p;}' "$2.t" | grep -q '  fib$'; then
        ok "$1: table, fib first"
    else
        bad "$1: no table with fib first"; head -5 "$2.t"
    fi
}

folded_ok() {   # name, stderr file
    tr -d '\r' < "$2" > "$2.f"
    if [ ! -s "$2.f" ]; then bad "$1: no folded stacks on stderr"; return; fi
    local odd
    odd=$(grep -cvE '^[^ ].* [0-9]+$' "$2.f")
    if [ "$odd" -ne 0 ]; then bad "$1: $odd stderr lines are not 'a;b;c N'"; grep -vE '^[^ ].* [0-9]+$' "$2.f" | head -3; return; fi
    if ! grep -qE '^main;(.*;)?fib [0-9]+$' "$2.f"; then bad "$1: no stack main;...;fib"; head -3 "$2.f"; return; fi
    if ! grep -vE '^main;' "$2.f" | grep -qE ';fib [0-9]+$'; then bad "$1: no stack of the second thread"; head -3 "$2.f"; return; fi
    ok "$1: folded stacks of both threads"
}

out_ok() {   # name, stdout file, want file
    if diff -q <(tr -d '\r' < "$2") "$3" > /dev/null; then ok "$1: output"; else bad "$1: output differs"; diff <(tr -d '\r' < "$2") "$3"; fi
}

# check name exe-runner prog-want [note]
check() {
    local name=$1 run=$2 w=$3 note=${4:-}
    $run > "$D/p.out" 2> "$D/p.err"
    out_ok "$name plain" "$D/p.out" "$w"
    if [ -s "$D/p.err" ] && ! grep -q "profile (FID_PROF)" "$D/p.err"; then bad "$name plain: stderr not empty"; head -3 "$D/p.err"; fi
    FID_PROF=1 $run > "$D/t.out" 2> "$D/t.err"
    table_ok "$name FID_PROF=1$note" "$D/t.err"
    FID_PROF=folded $run > "$D/f.out" 2> "$D/f.err"
    out_ok "$name FID_PROF=folded" "$D/f.out" "$w"
    folded_ok "$name FID_PROF=folded$note" "$D/f.err"
}

build() {   # out, args...
    local out=$1; shift
    "$FID" "$@" -o "$out" > "$out.log" 2>&1 && [ -f "$out" ]
}

for P in threads api; do
    SRC=tests/prof/db9_$P.fd
    W="$D/want_$P.txt"
    # C
    if build "$D/c_$P$EXE" ccompile "$SRC"; then
        check "ccompile db9_$P" "./$D/c_$P$EXE" "$W"
    else
        bad "ccompile db9_$P: build"; tail -5 "$D/c_$P$EXE.log"
    fi
    # native default (pe_ir on Windows)
    if build "$D/n_$P$EXE" emit "$SRC"; then
        note=""
        [ "$WIN" = 1 ] && [ "$P" = threads ] && note=" (pe_ir: needs SL15 needsOther A+B)"
        check "emit db9_$P" "./$D/n_$P$EXE" "$W" "$note"
    else
        bad "emit db9_$P: build"; tail -5 "$D/n_$P$EXE.log"
    fi
    if [ "$WIN" = 1 ]; then
        # --nat Windows
        if build "$D/nat_$P.exe" emit "$SRC" --nat; then
            check "emit --nat db9_$P" "./$D/nat_$P.exe" "$W"
        else
            bad "emit --nat db9_$P: build"; tail -5 "$D/nat_$P.exe.log"
        fi
        # x86_64 Linux under WSL
        if command -v wsl > /dev/null 2>&1; then
            if build "$D/lx_$P" emit "$SRC" --target x86_64-linux; then
                WD=$(cygpath -m "$(pwd)" | sed -E 's#^([A-Za-z]):#/mnt/\L\1#')
                cat > "$D/lx_$P.sh" <<EOF
#!/bin/bash
cd "$WD/$D"
chmod +x lx_$P
./lx_$P > lx_p.out 2> lx_p.err
FID_PROF=1 ./lx_$P > lx_t.out 2> lx_t.err
FID_PROF=folded ./lx_$P > lx_f.out 2> lx_f.err
EOF
                MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} bash "$WD/$D/lx_$P.sh"
                out_ok "x86_64-linux db9_$P plain" "$D/lx_p.out" "$W"
                table_ok "x86_64-linux db9_$P FID_PROF=1" "$D/lx_t.err"
                out_ok "x86_64-linux db9_$P FID_PROF=folded" "$D/lx_f.out" "$W"
                folded_ok "x86_64-linux db9_$P FID_PROF=folded" "$D/lx_f.err"
            else
                bad "x86_64-linux db9_$P: build"; tail -5 "$D/lx_$P.log"
            fi
        fi
    fi
done
[ $fail = 0 ] && echo "db9-check: all passed"
exit $fail
