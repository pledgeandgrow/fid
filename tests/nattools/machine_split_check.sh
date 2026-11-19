#!/usr/bin/env bash
# Checks of RA4 (live-range splitting) of the machine layer
# (compiler/nat_ir.fd ng_mir_alloc_split, ng_split_plan, the checker
# ng_split_check run from ng_mir_check), after the compiler is built:
#   bash tests/nattools/machine_split_check.sh [fid]   (default release/fid.exe)
#
# For x86_64-linux, aarch64-linux, riscv64-linux and --nat Windows,
# tests/bootstrap/test_machine_split.fd is built with FID_VERIFY=1 (MIR
# verifier, allocation checker, split checker) and --stats four ways:
#   on        the default: some function must report "[split] ...: N
#             pieces" (N > 0); on aarch64 `wide` must say "kept" (its
#             call-free loop gets caller-saved registers x8-x11); the
#             reloads `[stats]` counts for `wide` must not grow against off
#   off       FID_NAT_OFF=split: no [split] line
#   all       FID_NAT_ON=splitall: every integer value split, always kept
#   mutant    FID_NAT_ON=splitall FID_NAT_MUTATE=split (no stores at loop
#             exits): the build must fail with the checker's "live-range
#             split" message
# Each image that builds is run where this host can (WSL / qemu-user for
# Linux, Windows directly) and must print the file's expected output.
# Then tests/nattools/split_unit.fd is built through C (`fid ccompile`,
# CC or zig cc; `fid run` without one) and must print its expected lines.
# FILES="a.fd b.fd" builds other programs with FID_VERIFY=1 and
# FID_NAT_ON=splitall too and compares their output with `fid run` where
# the image runs (FILES="$(ls tests/bootstrap/test_*.fd | grep -v _fail)"
# for the suite). Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/bootstrap/test_machine_split.fd
OUT="${TMPDIR:-/tmp}/fid-machine-split"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

# the `// ` lines right after `// Expected output:` (up to the first other
# line: the comment after the blank line that ends the block is not taken)
expected() { awk '/^\/\/ Expected output:/ { f = 1; next } f && /^\/\/ / { sub(/^\/\/ /, ""); print; next } f { exit }' "$1" | tr -d '\r'; }
WANT="$(expected "$SRC")"

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

# run_exe <dir> <exe name> <target>: the program's output (status 99 when
# this host cannot run it)
run_exe() {
    local dir="$1" exe="$2" t="$3" q=""
    case "$t" in
        aarch64-linux) q=qemu-aarch64 ;;
        riscv64-linux) q=qemu-riscv64 ;;
    esac
    case "$t" in
        *-linux)
            if [ "$HOST" = windows ] && [ -z "$WSL" ]; then return 99; fi
            if [ -n "$q" ]; then
                if [ -n "$WSL" ]; then
                    (cd "$dir" && $WSL sh -c "command -v $q >/dev/null") || return 99
                elif ! command -v "$q" >/dev/null 2>&1; then
                    if [ "$(uname -m)" = "${t%%-*}" ]; then q=""; else return 99; fi
                fi
            fi
            (cd "$dir" && $WSL $q "./$exe" 2>&1 | tr -d '\r')
            ;;
        windows)
            [ "$HOST" = windows ] || return 99
            (cd "$dir" && "./$exe" 2>&1 | tr -d '\r')
            ;;
    esac
}

# emit_one <src> <target> <exe path> <log> [env assignments...]
emit_one() {
    local src="$1" t="$2" exe="$3" log="$4"
    shift 4
    local targ=(--target "$t")
    if [ "$t" = windows ]; then targ=(--nat); fi
    env FID_VERIFY=1 "$@" "$FID" emit "$src" "${targ[@]}" --stats -o "$exe" >"$log" 2>&1
}

reloads() { sed -n "s/^\[stats\] $2: .*reloads \([0-9]*\),.*/\1/p" "$1" | head -1; }

for t in x86_64-linux aarch64-linux riscv64-linux windows; do
    sfx=""
    [ "$t" = windows ] && sfx=".exe"
    built=""
    for mode in on off all; do
        exe="split-$t-$mode$sfx"
        envs=()
        case "$mode" in
            off) envs=(FID_NAT_OFF=split) ;;
            all) envs=(FID_NAT_ON=splitall) ;;
        esac
        if ! emit_one "$SRC" "$t" "$OUT/$exe" "$OUT/$exe.log" ${envs[@]+"${envs[@]}"}; then
            bad "$t $mode: emit failed: $(grep -v '^\[' "$OUT/$exe.log" | tail -2)"
            continue
        fi
        built="$built $mode"
        got=$(run_exe "$OUT" "$exe" "$t")
        rc=$?
        if [ $rc -eq 99 ]; then
            ok "$t $mode: built with FID_VERIFY=1 (not run on this host)"
        elif [ "$got" = "$WANT" ]; then
            ok "$t $mode: output as expected"
        else
            bad "$t $mode: output differs"
            diff <(echo "$WANT") <(echo "$got") | head -10
        fi
    done
    on="$OUT/split-$t-on$sfx.log"
    off="$OUT/split-$t-off$sfx.log"
    all="$OUT/split-$t-all$sfx.log"
    case "$built" in
        *on*off*)
            np=$(sed -n 's/^\[split\] [^:]*: \([0-9]*\) pieces.*/\1/p' "$on" | awk '{s += $1} END {print s + 0}')
            if [ "$np" -gt 0 ]; then ok "$t on: $np piece(s) made"; else bad "$t on: no [split] line with pieces"; fi
            if grep -q '^\[split\]' "$off"; then bad "$t off: [split] line with FID_NAT_OFF=split"; else ok "$t off: no [split] line"; fi
            # (aarch64 only: RISC-V's caller-saved pool is t4 alone, which
            # a loop's counter usually holds, so its pieces get no register
            # and the split is rightly not kept; tests/nattools/
            # split_alloc_unit.fd shows the same on hand-made `wide`)
            if [ "$t" = aarch64-linux ]; then
                if grep -q '^\[split\] wide: .*, kept$' "$on"; then ok "$t on: the split of wide is kept"; else bad "$t on: the split of wide is not kept ($(grep '^\[split\] wide' "$on" | head -1))"; fi
            else
                echo "info $t on: $(grep '^\[split\] wide' "$on" | head -1)"
            fi
            ron=$(reloads "$on" wide)
            roff=$(reloads "$off" wide)
            if [ -n "$ron" ] && [ -n "$roff" ]; then
                if [ "$ron" -le "$roff" ]; then ok "$t: wide reloads $roff -> $ron with the split"; else bad "$t: wide reloads grew with the split ($roff -> $ron)"; fi
            else
                bad "$t: no [stats] line for wide"
            fi
            ;;
    esac
    case "$built" in
        *all*)
            if grep -q '^\[split\] .*, kept$' "$all"; then ok "$t all: splits kept"; else bad "$t all: no kept split with FID_NAT_ON=splitall"; fi
            ;;
    esac
    # the mutation: no stores at loop exits; the split checker must refuse it
    mut="split-$t-mut$sfx"
    if emit_one "$SRC" "$t" "$OUT/$mut" "$OUT/$mut.log" FID_NAT_ON=splitall FID_NAT_MUTATE=split; then
        bad "$t: FID_NAT_MUTATE=split built under FID_VERIFY=1 (the checker missed it)"
    elif grep -q 'live-range split' "$OUT/$mut.log"; then
        ok "$t: the checker rejects the mutation ($(grep -o '[a-z_0-9]*: live-range split: [^,]*' "$OUT/$mut.log" | head -1))"
    else
        bad "$t: the mutated build failed without the checker's message: $(grep -v '^\[' "$OUT/$mut.log" | tail -2)"
    fi
done

# the unit test of the planner and the checker: built through C (it
# imports compiler::nat_ir: `fid run` interprets all of that, which takes
# hours for the random programs; natively it runs in under a second), or
# run when no C compiler is set (CC unset and none found)
UNIT=tests/nattools/split_unit.fd
uwant="$(expected "$UNIT")"
UEXE="$OUT/split_unit"
[ "$HOST" = windows ] && UEXE="$UEXE.exe"
if [ -z "${CC:-}" ] && command -v zig >/dev/null 2>&1; then export CC="zig cc"; fi
if [ -n "${CC:-}" ] && "$FID" ccompile "$UNIT" -o "$UEXE" >"$OUT/split_unit.log" 2>&1; then
    ugot=$("$UEXE" 2>&1 | tr -d '\r' | grep -v '^\[')
else
    echo "info split_unit.fd: not built through C (CC unset or ccompile failed: $OUT/split_unit.log); running it, which is slow"
    ugot=$("$FID" run "$UNIT" 2>&1 | tr -d '\r' | grep -v '^\[')
fi
if [ "$ugot" = "$uwant" ]; then
    ok "split_unit.fd: as expected"
else
    bad "split_unit.fd differs"
    diff <(echo "$uwant") <(echo "$ugot") | head -20
fi

# the whole allocation with splitting on hand-made machine IR, the
# allocated functions run (tests/nattools/split_alloc_unit.fd): its
# expected lines; FID_NAT_OFF=splitprune the same lines but for wide's
# verdict; FID_NAT_MUTATE=split: some rejected, none computing something
# else unnoticed. The cost numbers ([split_alloc] lines) are printed.
AUNIT=tests/nattools/split_alloc_unit.fd
awant="$(expected "$AUNIT")"
AEXE="$OUT/split_alloc_unit"
[ "$HOST" = windows ] && AEXE="$AEXE.exe"
if [ -n "${CC:-}" ] && "$FID" ccompile "$AUNIT" -o "$AEXE" >"$OUT/split_alloc_unit.log" 2>&1; then
    agot=$("$AEXE" 2>"$OUT/split_alloc_unit.err" | tr -d '\r')
    if [ "$agot" = "$awant" ]; then
        ok "split_alloc_unit.fd: as expected"
    else
        bad "split_alloc_unit.fd differs"
        diff <(echo "$awant") <(echo "$agot") | head -20
    fi
    grep '^\[split_alloc\] [a-z0-9]* ' "$OUT/split_alloc_unit.err" | tr -d '\r' | sed 's/^/info /'
    pgot=$(FID_NAT_OFF=splitprune "$AEXE" 2>/dev/null | tr -d '\r' | grep -v 'kept by default')
    if [ "$pgot" = "$(echo "$awant" | grep -v 'kept by default')" ]; then
        ok "split_alloc_unit.fd with FID_NAT_OFF=splitprune: sound"
    else
        bad "split_alloc_unit.fd with FID_NAT_OFF=splitprune differs"
    fi
    mgot=$(FID_NAT_MUTATE=split "$AEXE" 2>/dev/null | tr -d '\r' | grep ' random: ')
    if echo "$mgot" | grep -q ' [1-9][0-9]* rejected, 0 differ' && ! echo "$mgot" | grep -qv ', 0 differ,'; then
        ok "split_alloc_unit.fd with FID_NAT_MUTATE=split: the mutants rejected, none unnoticed"
    else
        bad "split_alloc_unit.fd with FID_NAT_MUTATE=split: $mgot"
    fi
else
    bad "split_alloc_unit.fd: not built through C (CC unset or ccompile failed: $OUT/split_alloc_unit.log)"
fi

# other programs (FILES=...), every value split, against `fid run`
for src in ${FILES:-}; do
    want=$("$FID" run "$src" 2>&1 | tr -d '\r')
    for t in x86_64-linux aarch64-linux riscv64-linux windows; do
        sfx=""
        [ "$t" = windows ] && sfx=".exe"
        exe="f-$(basename "$src" .fd)-$t$sfx"
        if ! emit_one "$src" "$t" "$OUT/$exe" "$OUT/$exe.log" FID_NAT_ON=splitall; then
            bad "$src $t: emit failed: $(grep -v '^\[' "$OUT/$exe.log" | tail -1)"
            continue
        fi
        got=$(run_exe "$OUT" "$exe" "$t")
        rc=$?
        if [ $rc -eq 99 ]; then
            ok "$src $t: built with FID_VERIFY=1"
        elif [ "$got" = "$want" ]; then
            ok "$src $t: output as with fid run"
        else
            bad "$src $t: output differs from fid run"
        fi
    done
done

if [ "$fails" -eq 0 ]; then
    echo "all passed"
    exit 0
fi
echo "$fails failed"
exit 1
