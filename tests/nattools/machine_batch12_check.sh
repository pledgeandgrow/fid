#!/usr/bin/env bash
# Checks of the machine-layer goal RA9 (docs/roadmap/llvm-roadmap.md), run
# after the compiler is built:
#   bash tests/nattools/machine_batch12_check.sh [fid]   (default release/fid.exe)
# - tests/nattools/greedy_unit.fd (nat_greedy_unit: the greedy assignment
#   on given hulls) through `fid run` prints its expected lines.
# - tests/bootstrap/test_machine_greedy.fd built for x86-64, AArch64,
#   RISC-V Linux and --nat Windows with FID_VERIFY=1 (MIR verifier and
#   allocation checker), once with the linear scan and once with
#   FID_NAT_ON=greedy: both build; with the switch `fid emit --stats`
#   prints a `[greedy]` line for `pressure`, without it none; where the
#   image runs (x86-64 Linux through WSL on Windows, natively on Linux; the
#   --nat image on Windows) its output equals the test's expected lines.
# - a table of the spills `--stats` counts per function, linear scan
#   against greedy (the comparison RA9 asks for), on every target.
# FILES="a.fd b.fd" checks other programs too (tests/bootstrap/*.fd for
# the whole suite; see step 3: the checker on every target in FTARGETS,
# and the greedy image's output against the linear scan's where it runs).
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-batch12"
mkdir -p "$OUT"
RUN=""
if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

runs_here() {
    if [ "$1" = nat ]; then [ "$WIN" = 1 ]; return; fi
    [ "$1" = x86_64-linux ] || return 1
    if [ -n "$RUN" ]; then return 0; fi
    [ "$(uname -m)" = x86_64 ] && [ "$(uname -s)" = Linux ]
}

build() {
    local img="$1" t="$2" tgt="$3"
    shift 3
    if [ "$tgt" = nat ]; then
        FID_VERIFY=1 "$FID" emit "$t" --nat --stats "$@" -o "$img" >"$img.log" 2>&1
    else
        FID_VERIFY=1 "$FID" emit "$t" --target "$tgt" --stats "$@" -o "$img" >"$img.log" 2>&1
    fi
}

run_img() {
    if [ "$2" = nat ]; then "$1" 2>&1; else $RUN "$1" 2>&1; fi
}

spilled() { sed -n "s/^\[stats\] $2: spilled \([0-9]*\),.*/\1/p" "$1" | head -1; }

# 1. the unit test of the assignment itself
got=$("$FID" run tests/nattools/greedy_unit.fd 2>&1 | grep -v '^\[')
if [ "$got" = "$(expected tests/nattools/greedy_unit.fd)" ]; then ok "greedy_unit"; else bad "greedy_unit"; echo "$got" | head -20; fi

# 2. builds, checker, runs, [greedy] lines
T=tests/bootstrap/test_machine_greedy.fd
FNS="pressure mixed chain tri squares main"
printf '\n%-14s %-10s %8s %8s\n' target function linear greedy >"$OUT/table.txt"
for tgt in x86_64-linux aarch64-linux riscv64-linux nat; do
    ext=""
    [ "$tgt" = nat ] && ext=".exe"
    lin="$OUT/lin-$tgt$ext"
    gre="$OUT/gre-$tgt$ext"
    if build "$lin" "$T" "$tgt"; then ok "$tgt linear scan builds (FID_VERIFY=1)"; else bad "$tgt linear scan build"; tail -5 "$lin.log"; continue; fi
    if FID_NAT_ON=greedy build "$gre" "$T" "$tgt"; then ok "$tgt greedy builds (FID_VERIFY=1)"; else bad "$tgt greedy build"; tail -5 "$gre.log"; continue; fi
    if grep -q '^\[greedy\] pressure:' "$gre.log"; then ok "$tgt [greedy] line"; else bad "$tgt no [greedy] line for pressure"; fi
    if grep -q '^\[greedy\]' "$lin.log"; then bad "$tgt [greedy] line without the switch"; else ok "$tgt no [greedy] line without the switch"; fi
    if runs_here "$tgt"; then
        want=$(expected "$T")
        if [ "$(run_img "$lin" "$tgt")" = "$want" ]; then ok "$tgt linear runs"; else bad "$tgt linear output differs"; fi
        if [ "$(run_img "$gre" "$tgt")" = "$want" ]; then ok "$tgt greedy runs"; else bad "$tgt greedy output differs"; fi
    fi
    for f in $FNS; do
        printf '%-14s %-10s %8s %8s\n' "$tgt" "$f" "$(spilled "$lin.log" "$f")" "$(spilled "$gre.log" "$f")" >>"$OUT/table.txt"
    done
done

# 3. other programs (FILES=...; tests/bootstrap/*.fd for the whole suite).
# Each program on each of FTARGETS (default every target above) is built
# with FID_VERIFY=1 twice: with the linear scan, then FID_NAT_ON=greedy.
#  - the linear build fails: not the allocator's (counted as skipped);
#  - only the greedy build fails: FAIL (the MIR verifier or the allocation
#    checker rejected the greedy assignment: its last error line is shown);
#  - where the image runs, the greedy image must print what the linear
#    one prints and end with the same exit code (both native, so a program
#    whose native output differs from `fid run` for other reasons is not
#    counted against the allocator).
# Expected-fail tests (*_fail*.fd) are left out.
n_files=0
n_skip=0
for t in ${FILES:-}; do
    case "$t" in *_fail*.fd) continue ;; esac
    [ -f "$t" ] || continue
    n_files=$((n_files + 1))
    for tgt in ${FTARGETS:-x86_64-linux aarch64-linux riscv64-linux nat}; do
        ext=""
        [ "$tgt" = nat ] && ext=".exe"
        b=$(basename "$t" .fd)
        lin="$OUT/fl-$b-$tgt$ext"
        img="$OUT/fg-$b-$tgt$ext"
        if ! build "$lin" "$t" "$tgt"; then n_skip=$((n_skip + 1)); continue; fi
        if ! FID_NAT_ON=greedy build "$img" "$t" "$tgt"; then
            bad "$t $tgt greedy build: $(grep -v '^\[' "$img.log" | tail -1)"
            continue
        fi
        if runs_here "$tgt"; then
            lo_out=$(run_img "$lin" "$tgt"); lo_rc=$?
            gr_out=$(run_img "$img" "$tgt"); gr_rc=$?
            if [ "$lo_out" = "$gr_out" ] && [ "$lo_rc" = "$gr_rc" ]; then ok "$t $tgt greedy"; else bad "$t $tgt greedy output or exit code differs from the linear scan's ($lo_rc vs $gr_rc)"; fi
        else
            ok "$t $tgt greedy builds (FID_VERIFY=1)"
        fi
        rm -f "$lin" "$img"
    done
done
if [ "$n_files" -gt 0 ]; then echo "FILES: $n_files program(s), $n_skip build(s) failing with the linear scan too (not counted)"; fi

echo "spills per function (fid emit --stats), linear scan vs FID_NAT_ON=greedy:"
cat "$OUT/table.txt"
if [ "$fails" -gt 0 ]; then echo "$fails failure(s)"; exit 1; fi
echo "all passed"
