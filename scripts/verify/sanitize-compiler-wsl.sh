#!/usr/bin/env bash
# scripts/verify/sanitize-compiler-wsl.sh — the compiler itself under
# AddressSanitizer + UndefinedBehaviorSanitizer, from a Windows machine
# (own-backend roadmap RL15; `COMPILER=1 scripts/sanitize.sh` is the same
# check on Linux or macOS, where a Linux `fid` can write its own C).
#
# What it does:
#   1. Windows: a snapshot of compiler/ and stdlib/ (other sessions edit the
#      tree while this runs, AGENTS.md 26); the Windows $FID writes the
#      compiler's C from it (`dump-c compiler/main.fd`, with OS=Linux so
#      `@cfg` and the platform modules take the Linux side), wraps `main`
#      on a big-stack thread (as scripts/build-selfhost.sh) and splits it
#      into SPLIT units (`fid split-c`).
#   2. WSL: builds fid-plain (no sanitizers, -O2) from those units. With
#      LINUX_C=1 (default) fid-plain then writes the compiler's C again, on
#      Linux, and fid-san is built from that C; otherwise from the Windows
#      C. fid-san: -fsanitize=$SAN at $SAN_OPT.
#   3. WSL: every test goes through fid-san and fid-plain for each of STEPS
#      (default `check dump-c run`): `check` type-checks, `dump-c` lowers
#      to C, `run` interprets. A finding is
#        - a sanitizer report in fid-san's stderr (memory error or UB in
#          the compiler or its C runtime), or
#        - fid-san's stdout or exit status differing from fid-plain's, when
#          fid-plain gives the same thing twice (the same source compiled
#          the same way, so a difference is fid-san reading memory it
#          should not, or a layout-dependent result); a step whose plain
#          runs differ from each other counts on sanitizer reports only.
#      A step that times out under the sanitizers only is listed, not failed.
#   4. SELF=1: fid-san compiles compiler/main.fd (`dump-c`, the biggest
#      program there is: slow, about 10-30 minutes); its C must equal
#      fid-plain's.
#   The summary is printed and written to $OUT/summary.san, a Markdown
#   table for docs/verification to $OUT/report.md; exit 1 on a finding.
#
#   FID=release/fid.exe bash scripts/verify/sanitize-compiler-wsl.sh
#   FID=$FID bash scripts/verify/sanitize-compiler-wsl.sh test_closure test_hashmap
#   FID=$FID SAMPLE=100 SELF=1 bash scripts/verify/sanitize-compiler-wsl.sh
#   SELFTEST=1 bash scripts/verify/sanitize-compiler-wsl.sh   # no fid needed
#
# Environment:
#   FID          the Windows compiler that writes the C (default release/fid.exe)
#   FIDC         use this C of the compiler instead of running `$FID dump-c`
#                (e.g. build/selfhost/fid-dev.c of a retained build; LINUX_C still
#                applies)
#   WSL_DISTRO   WSL distribution (default: WSL's default)
#   WSL_CC       C compiler inside WSL (default: clang when installed, else gcc)
#   SAN          sanitizers (default address,undefined; SAN=undefined alone
#                is much faster; thread makes no sense for the compiler,
#                which is single-threaded)
#   SAN_OPT      optimisation of fid-san (default -O1)
#   UB_RECOVER=1 UBSan keeps going after a report (every UB site of a run is
#                listed, not only the first)
#   SPLIT, JOBS  C units (default 8) and how many compile at once (default 3:
#                an ASan unit of the compiler takes a few GB to compile)
#   STACK_MB     the compiler thread's stack (default 1024: ASan's red zones
#                make frames larger than the release build's 256 MB allows
#                for the deepest recursion)
#   STEPS        default "check dump-c run"
#   LIMIT        seconds per step (default 900)
#   SAMPLE=n     n tests, evenly spread over the list
#   RUN_JOBS     tests at once inside WSL (default 1: some tests bind ports)
#   LINUX_C=0    build fid-san from the Windows-written C
#   OUT          default ${TMPDIR:-/tmp}/fid-sanitize-compiler
#   WTMP         working folder inside WSL for the tree and the binaries
#                (default /tmp/fid-sancomp; the 9p mount is slow)
# Arguments: test names (tests/bootstrap/<name>.fd), .fd paths, or folders
# (every .fd in it). Default: every tests/bootstrap/test_*.fd, expected-fail
# tests included (their errors are compiler paths too).
set -u
cd "$(dirname "$0")/../.." || exit 2
ROOT=$(pwd)

FID="${FID:-release/fid.exe}"
SAN="${SAN:-address,undefined}"
SAN_OPT="${SAN_OPT:--O1}"
SPLIT="${SPLIT:-8}"
JOBS="${JOBS:-3}"
STACK_MB="${STACK_MB:-1024}"
STEPS="${STEPS:-check dump-c run}"
LIMIT="${LIMIT:-900}"
RUN_JOBS="${RUN_JOBS:-1}"
LINUX_C="${LINUX_C:-1}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-sanitize-compiler}"
WTMP="${WTMP:-/tmp/fid-sancomp}"
mkdir -p "$OUT"
OUT=$(cd "$OUT" && pwd)

command -v wsl.exe > /dev/null 2>&1 || {
    echo "sanitize-compiler-wsl: no wsl.exe (on Linux or macOS: COMPILER=1 scripts/sanitize.sh)"; exit 2; }
WSL=(wsl.exe)
[ -n "${WSL_DISTRO:-}" ] && WSL=(wsl.exe -d "$WSL_DISTRO")
to_wsl() {   # C:\a\b or /c/a/b -> /mnt/c/a/b
    local p; p=$(cd "$1" && pwd -W 2>/dev/null || pwd)
    p="${p//\\//}"
    case "$p" in
        [A-Za-z]:/*) echo "/mnt/$(echo "${p:0:1}" | tr 'A-Z' 'a-z')${p:2}" ;;
        /[a-z]/*) echo "/mnt${p}" ;;
        *) echo "$p" ;;
    esac
}
wsl_run() { MSYS_NO_PATHCONV=1 "${WSL[@]}" -- "$@"; }
WOUT=$(to_wsl "$OUT")
WROOT=$(to_wsl "$ROOT")

if [ -z "${WSL_CC:-}" ]; then
    if wsl_run sh -c 'command -v clang' > /dev/null 2>&1; then WSL_CC=clang; else WSL_CC=gcc; fi
fi
wsl_run sh -c "command -v $WSL_CC" > /dev/null 2>&1 || {
    echo "sanitize-compiler-wsl: no $WSL_CC inside WSL (sudo apt install clang, or WSL_CC=gcc)"; exit 2; }

# The wrapper: fides_main on a thread with STACK_MB of stack (the compiler
# recurses deeply), as scripts/build-selfhost.sh and build-variant.sh do
cat > "$OUT/wrap-tail.c" << EOF

// === Stack wrapper (scripts/verify/sanitize-compiler-wsl.sh) ===
#include <pthread.h>
typedef struct { int argc; char** argv; int ret; } MainArgs;
static void* thread_main(void* p) { MainArgs* a = (MainArgs*)p; a->ret = fides_main(a->argc, a->argv); return 0; }
int main(int argc, char** argv) {
    MainArgs a = { argc, argv, 0 };
    pthread_attr_t attr; pthread_t t;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, (size_t)$STACK_MB*1024*1024);
    if (pthread_create(&t, &attr, thread_main, &a) != 0) return 1;
    pthread_join(t, NULL);
    return a.ret;
}
EOF
wrap() {   # in.c out.c
    sed 's/^int main(int argc, char\*\* argv)/int fides_main(int argc, char** argv)/' "$1" > "$2"
    cat "$OUT/wrap-tail.c" >> "$2"
}

# ---- the tests -----------------------------------------------------------
tests=()
if [ -n "${SELFTEST:-}" ]; then
    # A stand-in compiler (no fid needed): `check` prints "ok", `dump-c`
    # prints the file, `run` prints its line count, and overruns a heap
    # block when the file holds OVERFLOW_ME or divides by zero (UB) when it
    # holds DIVIDE_ME. Expected: clean.fd passes every step, overflow.fd
    # fails `run` with an ASan report, ub.fd fails `run` with a UBSan one,
    # and SELF=1 passes.
    mkdir -p "$OUT/selftest"
    printf 'fn main() {\n    print("clean")\n}\n' > "$OUT/selftest/clean.fd"
    printf '// OVERFLOW_ME\nfn main() {}\n' > "$OUT/selftest/overflow.fd"
    printf '// DIVIDE_ME\nfn main() {}\n' > "$OUT/selftest/ub.fd"
    cat > "$OUT/selftest/standin.c" << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char** argv) {
    if (argc < 3) { fprintf(stderr, "usage: standin <step> <file>\n"); return 2; }
    FILE* f = fopen(argv[2], "rb");
    if (!f) { fprintf(stderr, "Error: could not read file: %s\n", argv[2]); return 1; }
    static char buf[1 << 20]; size_t n = fread(buf, 1, sizeof buf - 1, f); fclose(f); buf[n] = 0;
    if (strcmp(argv[1], "check") == 0) { printf("ok\n"); return 0; }
    if (strcmp(argv[1], "dump-c") == 0) { fwrite(buf, 1, n, stdout); return 0; }
    long lines = 0; for (size_t i = 0; i < n; i++) if (buf[i] == '\n') lines++;
    if (strstr(buf, "OVERFLOW_ME")) { char* p = malloc(8); volatile int k = 8 + (int)lines; p[k] = 1; free(p); }
    if (strstr(buf, "DIVIDE_ME")) { volatile int z = (int)(lines - lines); volatile int q = (int)lines / z; printf("%d\n", q); }
    printf("%ld\n", lines);
    return 0;
}
EOF
    tests=("$OUT/selftest/clean.fd" "$OUT/selftest/overflow.fd" "$OUT/selftest/ub.fd")
    FIDC="$OUT/selftest/standin.c"; LINUX_C=0; SPLIT=1
elif [ $# -gt 0 ]; then
    for t in "$@"; do
        if [ -d "$t" ]; then for f in "$t"/*.fd; do [ -f "$f" ] && tests+=("$f"); done
        elif [ -f "$t" ]; then tests+=("$t")
        else tests+=("tests/bootstrap/${t%.fd}.fd"); fi
    done
else
    for f in tests/bootstrap/test_*.fd; do tests+=("$f"); done
fi
if [ -n "${SAMPLE:-}" ] && [ "${#tests[@]}" -gt "$SAMPLE" ]; then
    n=${#tests[@]}; picked=()
    for ((i = 0; i < SAMPLE; i++)); do picked+=("${tests[$((i * n / SAMPLE))]}"); done
    tests=("${picked[@]}")
fi
: > "$OUT/list"
for f in "${tests[@]}"; do
    [ -f "$f" ] || { echo "no $f (skipped)"; continue; }
    d=$(to_wsl "$(dirname "$f")")
    echo "$d/$(basename "$f")" >> "$OUT/list"
done
echo "sanitize-compiler-wsl: $(wc -l < "$OUT/list") tests, steps: $STEPS, SAN=$SAN, $WSL_CC in WSL"

# ---- 1. Windows: the snapshot, the compiler's C, its units ---------------
rm -rf "$OUT/tree" "$OUT/units_a"
mkdir -p "$OUT/tree" "$OUT/units_a"
if [ -n "${SELFTEST:-}" ]; then
    mkdir -p "$OUT/tree/compiler" "$OUT/tree/stdlib"
    printf 'fn main() {}\n' > "$OUT/tree/compiler/main.fd"
else
    cp -r compiler stdlib "$OUT/tree/" || { echo "sanitize-compiler-wsl: could not copy the tree"; exit 2; }
fi
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
if [ -n "${FIDC:-}" ]; then
    cp "$FIDC" "$OUT/fid-a.c" || exit 2
    echo "[1] the compiler's C: $FIDC"
else
    [ -x "$FID" ] || { echo "sanitize-compiler-wsl: no compiler $FID (set FID)"; exit 2; }
    echo "[1] $FID dump-c compiler/main.fd (snapshot, OS=Linux)"
    ( cd "$OUT/tree" && OS=Linux "$FID" dump-c compiler/main.fd > "$OUT/fid-a.c" 2> "$OUT/fid-a.log" ) &&
        [ -s "$OUT/fid-a.c" ] || {
        echo "sanitize-compiler-wsl: dump-c failed: $(grep -m5 -i error "$OUT/fid-a.log")"; exit 1; }
fi
wrap "$OUT/fid-a.c" "$OUT/fid-a_wrapped.c"
if [ "$SPLIT" -gt 1 ] && [ -z "${SELFTEST:-}" ] &&
   ( cd "$OUT/units_a" && "$FID" split-c ../fid-a_wrapped.c u "$SPLIT" > ../split-a.log 2>&1 ) &&
   [ -f "$OUT/units_a/u.h" ]; then
    rm -f "$OUT/fid-a_wrapped.c"
    echo "    $(wc -c < "$OUT/fid-a.c") bytes of C, $SPLIT units"
else
    rm -f "$OUT/units_a"/u_* "$OUT/units_a"/u.h
    mv "$OUT/fid-a_wrapped.c" "$OUT/units_a/u_0.c"
    echo "    $(wc -c < "$OUT/fid-a.c") bytes of C, one unit"
fi

# ---- 2 and 3. WSL: build, then run each test -----------------------------
RECOVER="-fno-sanitize-recover=undefined"
UBH=1
[ -n "${UB_RECOVER:-}" ] && { RECOVER=""; UBH=0; }
case ",$SAN," in *,undefined,*) ;; *) RECOVER="" ;; esac
{
    printf 'WOUT=%q\nWROOT=%q\nWTMP=%q\nCC=%q\nSAN=%q\nSAN_OPT=%q\nRECOVER=%q\n' \
        "$WOUT" "$WROOT" "$WTMP" "$WSL_CC" "$SAN" "$SAN_OPT" "$RECOVER"
    printf 'UBH=%q\nJOBS=%q\nSPLIT=%q\nSTEPS=%q\nLIMIT=%q\nRUN_JOBS=%q\nLINUX_C=%q\nSELF=%q\nSELFTEST=%q\n' \
        "$UBH" "$JOBS" "$SPLIT" "$STEPS" "$LIMIT" "$RUN_JOBS" "$LINUX_C" "${SELF:-}" "${SELFTEST:-}"
} > "$OUT/conf.sh"

cat > "$OUT/inner.sh" << 'EOF'
set -u
. "$1"
cd "$WOUT" || exit 2
: > results
WFLAGS="-w -Wno-error -Wno-int-conversion -Wno-incompatible-pointer-types -Wno-implicit-function-declaration -Wno-return-type -fno-strict-aliasing -fwrapv"
SANF="-fsanitize=$SAN $RECOVER -fno-omit-frame-pointer -g"
export ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=0:abort_on_error=0:halt_on_error=1:detect_stack_use_after_return=0}"
export UBSAN_OPTIONS="${UBSAN_OPTIONS:-print_stacktrace=1:halt_on_error=$UBH}"
hit() { grep -aq 'ERROR: AddressSanitizer\|runtime error:\|ERROR: LeakSanitizer\|SUMMARY: UndefinedBehaviorSanitizer' "$1"; }

# build <units dir> <binary> <flags...>: every u_K.c at once (JOBS), then link
build() {
    local dir="$1" bin="$2"; shift 2
    local objs=() k f pids=() ok=1
    : > "$bin.cc.log"
    for f in "$dir"/u_*.c; do
        k=$(basename "$f" .c)
        "$CC" "$@" $WFLAGS -c "$f" -o "$bin.$k.o" >> "$bin.cc.log" 2>&1 &
        pids+=($!); objs+=("$bin.$k.o")
        if [ ${#pids[@]} -ge "$JOBS" ]; then wait "${pids[0]}" || ok=0; pids=("${pids[@]:1}"); fi
    done
    for k in "${pids[@]}"; do wait "$k" || ok=0; done
    [ "$ok" = 1 ] && "$CC" "$@" "${objs[@]}" -o "$bin" -lm -lpthread >> "$bin.cc.log" 2>&1
    local st=$?
    rm -f "${objs[@]}"
    return $st
}
wrap() {
    sed 's/^int main(int argc, char\*\* argv)/int fides_main(int argc, char** argv)/' "$1" > "$2"
    cat "$WOUT/wrap-tail.c" >> "$2"
}

rm -rf "$WTMP"; mkdir -p "$WTMP"
cp -r "$WOUT/tree" "$WTMP/tree" || exit 2
# tests that read files relative to the repository root find them
ln -s "$WROOT/tests" "$WTMP/tree/tests" 2> /dev/null
ln -s "$WROOT/examples" "$WTMP/tree/examples" 2> /dev/null
PLAIN="$WTMP/fid-plain"; SANB="$WTMP/fid-san"
echo "[2] $CC: fid-plain (-O2)"
build "$WOUT/units_a" "$PLAIN" -O2 || { echo "BUILD fid-plain" >> results; cp "$PLAIN.cc.log" "$WOUT/"; exit 1; }
SAN_UNITS="$WOUT/units_a"
REFC=""
if [ "$LINUX_C" = 1 ]; then
    echo "    fid-plain dump-c compiler/main.fd (the compiler's C written on Linux)"
    ( cd "$WTMP/tree" && timeout 7200 "$PLAIN" dump-c compiler/main.fd > "$WTMP/fid-b.c" 2> "$WOUT/fid-b.log" )
    if [ -s "$WTMP/fid-b.c" ]; then
        REFC="$WTMP/fid-b.c"
        if cmp -s "$WTMP/fid-b.c" "$WOUT/fid-a.c"; then echo "    same C as Windows wrote" > "$WOUT/c-a-b.txt"
        else echo "    differs from the Windows-written C ($(cmp "$WTMP/fid-b.c" "$WOUT/fid-a.c" | head -1))" > "$WOUT/c-a-b.txt"; fi
        cat "$WOUT/c-a-b.txt"
        mkdir -p "$WTMP/units_b"
        wrap "$WTMP/fid-b.c" "$WTMP/fid-b_wrapped.c"
        if [ "$SPLIT" -gt 1 ] && ( cd "$WTMP/units_b" && "$PLAIN" split-c ../fid-b_wrapped.c u "$SPLIT" > /dev/null 2>&1 ) && [ -f "$WTMP/units_b/u.h" ]; then
            rm -f "$WTMP/fid-b_wrapped.c"
        else
            rm -f "$WTMP/units_b"/u_*; mv "$WTMP/fid-b_wrapped.c" "$WTMP/units_b/u_0.c"
        fi
        SAN_UNITS="$WTMP/units_b"
    else
        echo "    fid-plain could not write the compiler's C: fid-san is built from the Windows C"
        echo "LINUXC failed $WOUT/fid-b.log" >> results
    fi
fi
echo "[2] $CC: fid-san ($SAN_OPT -fsanitize=$SAN)"
build "$SAN_UNITS" "$SANB" $SAN_OPT $SANF || { echo "BUILD fid-san" >> results; cp "$SANB.cc.log" "$WOUT/"; exit 1; }
cp "$PLAIN.cc.log" "$SANB.cc.log" "$WOUT/" 2> /dev/null

# one step of one test: fid-san, then fid-plain (twice when they differ)
step_one() {   # name file step
    local n="$1" f="$2" s="$3" d="$WOUT/runs/$1"
    mkdir -p "$d"
    ( cd "$WTMP/tree" && timeout -k 5 "$LIMIT" "$SANB" "$s" "$f" < /dev/null > "$d/$s.san.out" 2> "$d/$s.san.err" ); local rs=$?
    if hit "$d/$s.san.err"; then echo "$n fail $s sanitizer"; return; fi
    if [ "$rs" = 124 ] || [ "$rs" = 137 ]; then echo "$n slow $s timeout-under-sanitizers"; return; fi
    ( cd "$WTMP/tree" && timeout -k 5 "$LIMIT" "$PLAIN" "$s" "$f" < /dev/null > "$d/$s.ref.out" 2> /dev/null ); local rp=$?
    if [ "$rs" = "$rp" ] && cmp -s "$d/$s.san.out" "$d/$s.ref.out"; then
        rm -f "$d/$s.san.out" "$d/$s.ref.out"; echo "$n pass $s"; return
    fi
    ( cd "$WTMP/tree" && timeout -k 5 "$LIMIT" "$PLAIN" "$s" "$f" < /dev/null > "$d/$s.ref2.out" 2> /dev/null ); local rp2=$?
    if [ "$rp" != "$rp2" ] || ! cmp -s "$d/$s.ref.out" "$d/$s.ref2.out"; then
        echo "$n pass $s nondet"; return
    fi
    if [ "$rs" != "$rp" ]; then echo "$n fail $s exit-$rs-plain-$rp"; else echo "$n fail $s output-differs"; fi
}
test_one() {   # file
    local f="${1%$'\r'}" n
    n=$(basename "$f" .fd)
    for s in $STEPS; do step_one "$n" "$f" "$s"; done
}
echo "[3] the tests ($(wc -l < list), $RUN_JOBS at a time)"
mkdir -p "$WOUT/runs"
pids=()
k=0
while read -r f; do
    f="${f%$'\r'}"; [ -n "$f" ] || continue
    k=$((k + 1))
    test_one "$f" > "$WTMP/r.$k" &
    pids+=($!)
    if [ ${#pids[@]} -ge "$RUN_JOBS" ]; then wait "${pids[0]}"; pids=("${pids[@]:1}"); fi
done < list
for p in "${pids[@]}"; do wait "$p"; done
for ((i = 1; i <= k; i++)); do cat "$WTMP/r.$i" >> results; rm -f "$WTMP/r.$i"; done

if [ -n "$SELF" ]; then
    echo "[4] fid-san dump-c compiler/main.fd"
    ( cd "$WTMP/tree" && timeout 14400 "$SANB" dump-c compiler/main.fd > "$WTMP/self.c" 2> "$WOUT/self.san" )
    if hit "$WOUT/self.san"; then echo "compiler/main.fd fail self sanitizer" >> results
    else
        if [ -z "$REFC" ]; then
            ( cd "$WTMP/tree" && timeout 7200 "$PLAIN" dump-c compiler/main.fd > "$WTMP/self.ref.c" 2> /dev/null )
            REFC="$WTMP/self.ref.c"
        fi
        if cmp -s "$WTMP/self.c" "$REFC"; then echo "compiler/main.fd pass self" >> results
        else echo "compiler/main.fd fail self output-differs" >> results; fi
    fi
fi
EOF
wsl_run bash "$WOUT/inner.sh" "$WOUT/conf.sh"
[ -f "$OUT/results" ] || { echo "sanitize-compiler-wsl: the WSL step wrote no results ($OUT/inner.sh)"; exit 2; }
tr -d '\r' < "$OUT/results" > "$OUT/results.lf"

if grep -q '^BUILD ' "$OUT/results.lf"; then
    b=$(sed -n 's/^BUILD //p' "$OUT/results.lf" | head -1)
    echo "sanitize-compiler-wsl: could not build $b (see $OUT/$b.cc.log):"
    grep -m5 -a 'error' "$OUT/$b.cc.log" 2>/dev/null | sed 's/^/  /'
    exit 1
fi
pass=$(grep -c ' pass ' "$OUT/results.lf")
nondet=$(grep -c ' pass .* nondet' "$OUT/results.lf")
slow=$(grep ' slow ' "$OUT/results.lf")
fails=$(grep ' fail ' "$OUT/results.lf")
nfail=0; [ -n "$fails" ] && nfail=$(echo "$fails" | wc -l)
nslow=0; [ -n "$slow" ] && nslow=$(echo "$slow" | wc -l)
first_report() {   # name step
    local e="$OUT/runs/$1/$2.san.err"
    [ "$1" = compiler/main.fd ] && e="$OUT/self.san"
    grep -a -m1 'ERROR: AddressSanitizer\|runtime error:' "$e" 2>/dev/null | cut -c1-200
}
{
    echo "sanitize-compiler-wsl ($SAN, $WSL_CC $SAN_OPT in WSL; steps $STEPS): $pass steps clean" \
         "($nondet of them on sanitizer reports only), $nfail failed, $nslow timed out under the sanitizers only"
    [ -f "$OUT/c-a-b.txt" ] && sed 's/^ */  the compiler'"'"'s C written on Linux: /' "$OUT/c-a-b.txt"
    grep '^LINUXC ' "$OUT/results.lf" | sed 's/^/  /'
    [ -n "$fails" ] && echo "$fails" | while read -r n _ s why; do
        echo "  FAIL $n $s ($why: $OUT/runs/$n/$s.san.err)"
        [ "$why" = sanitizer ] && first_report "$n" "$s" | sed 's/^/       /'
    done
    [ -n "$slow" ] && echo "$slow" | while read -r n _ s _; do echo "  SLOW $n $s"; done
} | tee "$OUT/summary.san"
{
    echo "| Test | Step | Finding | First report |"
    echo "|---|---|---|---|"
    [ -n "$fails" ] && echo "$fails" | while read -r n _ s why; do
        echo "| $n | $s | $why | $(first_report "$n" "$s" | sed 's/|/\\|/g') |"
    done
} > "$OUT/report.md"

if [ -n "${SELFTEST:-}" ]; then
    ok=1
    grep -q '^clean pass check$' "$OUT/results.lf" && grep -q '^clean pass dump-c$' "$OUT/results.lf" &&
        grep -q '^clean pass run$' "$OUT/results.lf" || ok=0
    grep -q '^overflow fail run sanitizer$' "$OUT/results.lf" || ok=0
    case ",$SAN," in *,undefined,*) grep -q '^ub fail run sanitizer$' "$OUT/results.lf" || ok=0 ;; esac
    grep -q '^overflow pass check$' "$OUT/results.lf" || ok=0
    [ -z "${SELF:-}" ] || grep -q '^compiler/main.fd pass self$' "$OUT/results.lf" || ok=0
    if [ "$ok" = 1 ]; then echo "sanitize-compiler-wsl selftest: OK"; exit 0; fi
    echo "sanitize-compiler-wsl selftest: FAIL (results: $OUT/results.lf)"; exit 1
fi
[ "$nfail" -eq 0 ]
