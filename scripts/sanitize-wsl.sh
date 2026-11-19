#!/usr/bin/env bash
# scripts/sanitize-wsl.sh — the sanitizer runs of scripts/sanitize.sh from a
# Windows machine (competitive roadmap VF14): the Windows `fid` writes each
# test's C (`dump-c`) and its expected output (`run`); gcc or clang inside
# WSL builds that C with -fsanitize=... and runs it. The generated C is the
# same for every target (its runtime picks the OS with #ifdef), so this is
# the Linux C path under ASan/UBSan/TSan without a Linux build of fid.
#
#   bash scripts/sanitize-wsl.sh                         # every test, ASan+UBSan
#   bash scripts/sanitize-wsl.sh test_vec_builtins test_closure
#   SAN=thread bash scripts/sanitize-wsl.sh              # sanitize.sh's TSan set
#   SAMPLE=40 bash scripts/sanitize-wsl.sh               # 40 tests, evenly spread
#   PATCH=scripts/verify/sanitize-patches.sed bash ...  # sed script applied to
#                                                        # each C file first (to
#                                                        # check a runtime fix
#                                                        # before it is in fid)
#
# FID (default release/fid.exe), WSL_DISTRO (default: WSL's default),
# WSL_CC (default gcc; clang if you have it), OUT (default
# ${TMPDIR:-/tmp}/fid-sanitize-wsl), LIMIT seconds per run (default 120),
# JOBS programs built and run at once in WSL (default 4; the Nov 17 run of
# the whole suite, ~1,100 programs, took ~4 h at 6 and most of a day at 1:
# each ASan build of 1-2 MB of C takes a core and much memory), WSL_BINDIR
# where WSL keeps the binaries (default /tmp/fid-sanitize-wsl-bin, WSL's own
# file system; KEEP_BINS=1 keeps them). The Windows step (dump-c, run) stays
# one fid at a time.
# A test that `fid dump-c` cannot lower, or that does not compile or link in
# WSL (C libraries such as sqlite or SDL), is skipped, not failed. A finding
# is a sanitizer report, or output that differs from `fid run` only under
# the sanitizers (the same C built without them is run to tell). The summary
# is written to $OUT/summary.san; exit 1 when there is a finding.
set -u
cd "$(dirname "$0")/.." || exit 2

FID="${FID:-release/fid.exe}"
SAN="${SAN:-address,undefined}"
WSL_CC="${WSL_CC:-gcc}"
LIMIT="${LIMIT:-120}"
JOBS="${JOBS:-4}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-sanitize-wsl}"
mkdir -p "$OUT"
command -v wsl.exe > /dev/null 2>&1 || { echo "sanitize-wsl: no wsl.exe (run scripts/sanitize.sh on Linux or macOS)"; exit 2; }
[ -x "$FID" ] || { echo "sanitize-wsl: no compiler $FID (set FID)"; exit 2; }
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
WOUT=$(to_wsl "$OUT")

TSAN_TESTS=$(sed -n '/^TSAN_TESTS="/,/"$/p' scripts/sanitize.sh | tr -d '"' | sed 's/^TSAN_TESTS=//')
if [ $# -gt 0 ]; then
    tests=(); for t in "$@"; do tests+=("tests/bootstrap/${t%.fd}.fd"); done
elif [ "${SAN#*thread}" != "$SAN" ]; then
    tests=(); for t in $TSAN_TESTS; do [ -f "tests/bootstrap/$t.fd" ] && tests+=("tests/bootstrap/$t.fd"); done
else
    tests=()
    for f in tests/bootstrap/test_*.fd; do
        case "$(basename "$f" .fd)" in *_fail|test_sqlite|test_gpu*|test_sdl*) continue ;; esac
        tests+=("$f")
    done
    if [ -n "${SAMPLE:-}" ] && [ "${#tests[@]}" -gt "$SAMPLE" ]; then
        n=${#tests[@]}; picked=()
        for ((i = 0; i < SAMPLE; i++)); do picked+=("${tests[$((i * n / SAMPLE))]}"); done
        tests=("${picked[@]}")
    fi
fi

# 1. Windows: the expected output and the C of each test (one fid at a time)
: > "$OUT/list"
skip=0
for f in "${tests[@]}"; do
    name=$(basename "$f" .fd)
    [ -f "$f" ] || { echo "no $f"; skip=$((skip + 1)); continue; }
    if ! "$FID" dump-c "$f" > "$OUT/$name.c" 2> "$OUT/$name.dumpc.log" || [ ! -s "$OUT/$name.c" ]; then
        skip=$((skip + 1)); echo "$name: dump-c failed (skipped)"; continue
    fi
    if [ -n "${PATCH:-}" ]; then sed -i -f "$PATCH" "$OUT/$name.c"; fi
    # one interpreter run (slow for the big tests): whether a program prints
    # something different each time is told from two runs of its plain build
    rm -f "$OUT/$name.want.timeout"
    timeout -k 5 "$LIMIT" "$FID" run "$f" < /dev/null > "$OUT/$name.want" 2> /dev/null
    [ $? -eq 124 ] && : > "$OUT/$name.want.timeout"   # no reference: sanitizer reports only
    echo "$name" >> "$OUT/list"
done

# 2. WSL: build with and without the sanitizers, run, compare
RECOVER=""
case ",$SAN," in *,undefined,*) RECOVER="-fno-sanitize-recover=undefined" ;; esac
# one.sh: one test (its result in <name>.res); inner.sh runs it on every
# test of the list, JOBS at a time (xargs -P). The binaries go to WSL's own
# file system (WSL_BINDIR, default /tmp/fid-sanitize-wsl-bin): building and
# running them on /mnt/c is several times slower. A test's binaries are
# removed once it is done (KEEP_BINS=1 keeps them).
WSL_BINDIR="${WSL_BINDIR:-/tmp/fid-sanitize-wsl-bin}"
cat > "$OUT/one.sh" << EOF
set -u
n=\$1
cd "$WOUT" || exit 2
B="$WSL_BINDIR"
BASE="-O1 -w -Wno-int-conversion -Wno-incompatible-pointer-types -fno-strict-overflow -fno-strict-aliasing -fPIE -pie"
SANF="-fsanitize=$SAN $RECOVER -fno-omit-frame-pointer -g"
export ASAN_OPTIONS="\${ASAN_OPTIONS:-detect_leaks=0:abort_on_error=0:halt_on_error=1}"
export UBSAN_OPTIONS="\${UBSAN_OPTIONS:-print_stacktrace=1:halt_on_error=1}"
export TSAN_OPTIONS="\${TSAN_OPTIONS:-halt_on_error=1:second_deadlock_stack=1}"
hit() { grep -q 'ERROR: AddressSanitizer\|runtime error:\|ERROR: LeakSanitizer\|WARNING: ThreadSanitizer\|ERROR: ThreadSanitizer' "\$1"; }
res() { echo "\$n \$*" > "\$n.res"; }
done_bins() { [ "${KEEP_BINS:-0}" = 1 ] || rm -f "\$B/\$n.san.bin" "\$B/\$n.plain.bin"; }
if ! $WSL_CC \$SANF \$BASE "\$n.c" -o "\$B/\$n.san.bin" -lm -lpthread > "\$n.cc.log" 2>&1; then
    res skip build; exit 0
fi
timeout $LIMIT "\$B/\$n.san.bin" < /dev/null > "\$n.got" 2> "\$n.san"
tr -d '\r' < "\$n.want" > "\$n.want.lf"
if hit "\$n.san"; then res fail sanitizer; done_bins; exit 0; fi
if cmp -s "\$n.got" "\$n.want.lf"; then res pass; done_bins; exit 0; fi
if [ -f "\$n.want.timeout" ]; then res pass no-reference; done_bins; exit 0; fi
: > "\$n.plain"; : > "\$n.plain2"
if $WSL_CC \$BASE "\$n.c" -o "\$B/\$n.plain.bin" -lm -lpthread > /dev/null 2>&1; then
    timeout $LIMIT "\$B/\$n.plain.bin" < /dev/null > "\$n.plain" 2> /dev/null
    timeout $LIMIT "\$B/\$n.plain.bin" < /dev/null > "\$n.plain2" 2> /dev/null
fi
done_bins
# prints something different each run (times, random values): only the
# sanitizer reports count
if ! cmp -s "\$n.plain" "\$n.plain2"; then res pass nondet; exit 0; fi
if cmp -s "\$n.plain" "\$n.got"; then res diff c-path
else res fail only-under-sanitizers; fi
EOF
cat > "$OUT/inner.sh" << EOF
set -u
cd "$WOUT" || exit 2
mkdir -p "$WSL_BINDIR" || exit 2
tr -d '\r' < list > list.lf
while read -r n; do rm -f "\$n.res"; done < list.lf
xargs -P "$JOBS" -n 1 bash "$WOUT/one.sh" < list.lf
# the results in the order of the list (a test with no .res did not finish)
: > results
while read -r n; do
    if [ -f "\$n.res" ]; then cat "\$n.res" >> results; else echo "\$n fail no-result" >> results; fi
done < list.lf
EOF
# MSYS_NO_PATHCONV: Git Bash would rewrite the /mnt/... argument
MSYS_NO_PATHCONV=1 "${WSL[@]}" -- bash "$WOUT/inner.sh"
[ -f "$OUT/results" ] || { echo "sanitize-wsl: the WSL step wrote no results ($WOUT/inner.sh)"; exit 2; }
tr -d '\r' < "$OUT/results" > "$OUT/results.lf"

pass=$(grep -c ' pass' "$OUT/results.lf")
fails=$(grep ' fail ' "$OUT/results.lf")
cdiff=$(grep ' diff c-path' "$OUT/results.lf")
bskip=$(grep -c ' skip build' "$OUT/results.lf")
nfail=0; [ -n "$fails" ] && nfail=$(echo "$fails" | wc -l)
ncd=0; [ -n "$cdiff" ] && ncd=$(echo "$cdiff" | wc -l)
{
    echo "sanitize-wsl ($SAN, $WSL_CC in WSL): $pass clean, $nfail failed," \
         "$ncd differ from fid run also without sanitizers (C path, not a sanitizer finding)," \
         "$((skip + bskip)) skipped"
    [ -n "$fails" ] && echo "$fails" | while read -r n _ why; do
        echo "  FAIL $n ($why: $OUT/$n.san)"
        grep -m1 'ERROR: AddressSanitizer\|runtime error:\|WARNING: ThreadSanitizer' "$OUT/$n.san" 2> /dev/null | sed 's/^/       /'
    done
    [ -n "$cdiff" ] && echo "$cdiff" | while read -r n _; do echo "  C-PATH $n"; done
} | tee "$OUT/summary.san"
[ "$nfail" -eq 0 ]
