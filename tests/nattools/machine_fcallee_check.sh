#!/usr/bin/env bash
# RA18 (docs/roadmap/llvm-roadmap.md): float ranges that cross a call keep
# a callee-saved float register (AArch64 d8..d15, RISC-V fs0..fs11)
# instead of a stack slot; compiler/nat_ir.fd ng_float_assign,
# ng_float_callee_pool, the saves in ng_save_subset. Run after the
# compiler is built:
#   bash tests/nattools/machine_fcallee_check.sh [fid]   (default release/fid.exe)
#
# For aarch64-linux and riscv64-linux:
#  1. tests/bootstrap/test_machine_fcallee.fd built with FID_VERIFY=1 and
#     --stats: at least one "[fcallee] <fn>: N float ranges across calls in
#     callee-saved float registers" line (N > 0), and its output equal to
#     the file's expected block where the image can run (qemu-aarch64 /
#     qemu-riscv64, through WSL on Windows).
#  2. The same with FID_NAT_OFF=fcallee: no "[fcallee]" line, same output.
#  3. FID_NAT_MUTATE=fcallee (the registers used but not saved, on purpose;
#     no FID_VERIFY, whose checker would refuse it): the output differs
#     from the expected block wherever the image runs (the test sees a
#     missing save).
#  4. When llvm-objdump / objdump can read the image: the build of 1 holds
#     a save of d8..d15 (`stp d8` / `str d8`..) or fs0..fs11 (`fsd fs0`..),
#     the build of 2 none.
#  5. tests/bootstrap/test_machine_fcallee_tasks.fd (tasks yield while
#     floats are live): no "[fcallee]" line (a program with tasks keeps
#     floats off those registers, ng_fcs_tasks) and its expected output.
# x86_64-linux and --nat Windows have no callee-saved float register in
# this scheme: the test must build there with FID_VERIFY=1, print the same
# and have no "[fcallee]" line.
# Exit 1 on a failure; a target the host cannot run is build-only.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/bootstrap/test_machine_fcallee.fd
TSRC=tests/bootstrap/test_machine_fcallee_tasks.fd
OUT="${TMPDIR:-/tmp}/fid-machine-fcallee"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT="$(expected "$SRC")"
TWANT="$(expected "$TSRC")"

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

# run_exe <dir> <exe> <target>: the program's output; status 99 when this
# host cannot run it
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

OBJDUMP=""
for c in llvm-objdump objdump; do
    if command -v "$c" >/dev/null 2>&1; then OBJDUMP="$c"; break; fi
done

# build <name> <src> <target> [env...]: emit with --stats into $OUT/<name>
build() {
    local name="$1" src="$2" t="$3"
    shift 3
    local targ=(--target "$t") exe="$name"
    if [ "$t" = windows ]; then targ=(--nat); exe="$name.exe"; fi
    env "$@" "$FID" emit "$src" "${targ[@]}" --stats -o "$OUT/$exe" >"$OUT/$name.log" 2>&1
}

fcallee_n() { sed -n 's/^\[fcallee\] [^:]*: \([0-9]*\) .*/\1/p' "$1" | awk '{s += $1} END {print s + 0}'; }

for t in aarch64-linux riscv64-linux; do
    on="fc-$t-on"; off="fc-$t-off"; mut="fc-$t-mut"
    if build "$on" "$SRC" "$t" FID_VERIFY=1; then ok "$t builds (FID_VERIFY=1)"; else bad "$t builds (FID_VERIFY=1): $(tail -3 "$OUT/$on.log")"; continue; fi
    n_on=$(fcallee_n "$OUT/$on.log")
    if [ "$n_on" -gt 0 ]; then ok "$t: $n_on float ranges across calls in callee-saved registers"; else bad "$t: no [fcallee] line"; fi
    if build "$off" "$SRC" "$t" FID_VERIFY=1 FID_NAT_OFF=fcallee; then ok "$t builds with FID_NAT_OFF=fcallee"; else bad "$t builds with FID_NAT_OFF=fcallee"; continue; fi
    n_off=$(fcallee_n "$OUT/$off.log")
    if [ "$n_off" -eq 0 ]; then ok "$t: no [fcallee] line with the switch off"; else bad "$t: [fcallee] lines with FID_NAT_OFF=fcallee"; fi
    build "$mut" "$SRC" "$t" FID_NAT_MUTATE=fcallee || bad "$t builds with FID_NAT_MUTATE=fcallee"
    got_on=$(run_exe "$OUT" "$on" "$t"); s1=$?
    if [ "$s1" = 99 ]; then
        echo "skip $t: cannot run here (build-only)"
    else
        if [ "$got_on" = "$WANT" ]; then ok "$t: expected output"; else bad "$t: output differs"; diff <(echo "$WANT") <(echo "$got_on") | head -8; fi
        got_off=$(run_exe "$OUT" "$off" "$t")
        if [ "$got_off" = "$WANT" ]; then ok "$t: expected output (fcallee off)"; else bad "$t: output differs (fcallee off)"; fi
        got_mut=$(run_exe "$OUT" "$mut" "$t")
        if [ "$got_mut" != "$WANT" ]; then ok "$t: the mutation (no saves) changes the output"; else bad "$t: FID_NAT_MUTATE=fcallee prints the expected output: the test does not see a missing save"; fi
    fi
    if [ -n "$OBJDUMP" ]; then
        if "$OBJDUMP" -d "$OUT/$on" >"$OUT/$on.dis" 2>/dev/null && [ -s "$OUT/$on.dis" ]; then
            "$OBJDUMP" -d "$OUT/$off" >"$OUT/$off.dis" 2>/dev/null
            if [ "$t" = aarch64-linux ]; then pat='(stp|str)[[:space:]]+d(8|9|1[0-5])\b'; else pat='fsd[[:space:]]+(fs([0-9]|1[01])|f(8|9|1[89]|2[0-7]))\b'; fi
            c_on=$(grep -cE "$pat" "$OUT/$on.dis")
            c_off=$(grep -cE "$pat" "$OUT/$off.dis")
            if [ "$c_on" -gt 0 ] && [ "$c_off" -eq 0 ]; then ok "$t: $c_on saves of callee-saved float registers (none with the switch off)"; else bad "$t: saves of callee-saved float registers: $c_on on, $c_off off"; fi
        else
            echo "skip $t: $OBJDUMP cannot read the image"
        fi
    fi
    # 5. tasks: none of these registers
    tk="fc-$t-tasks"
    if build "$tk" "$TSRC" "$t" FID_VERIFY=1; then
        if [ "$(fcallee_n "$OUT/$tk.log")" -eq 0 ]; then ok "$t: a program with tasks uses no callee-saved float register"; else bad "$t: [fcallee] lines in a program with tasks"; fi
        got_t=$(run_exe "$OUT" "$tk" "$t"); s2=$?
        if [ "$s2" != 99 ]; then
            if [ "$got_t" = "$TWANT" ]; then ok "$t: tasks test output"; else bad "$t: tasks test output differs"; diff <(echo "$TWANT") <(echo "$got_t") | head -5; fi
        fi
    else
        bad "$t: tasks test builds: $(tail -3 "$OUT/$tk.log")"
    fi
done

for t in x86_64-linux windows; do
    nm="fc-$t"
    if build "$nm" "$SRC" "$t" FID_VERIFY=1; then ok "$t builds (FID_VERIFY=1)"; else bad "$t builds: $(tail -3 "$OUT/$nm.log")"; continue; fi
    if [ "$(fcallee_n "$OUT/$nm.log")" -eq 0 ]; then ok "$t: no [fcallee] line (no callee-saved float register)"; else bad "$t: [fcallee] lines on a target without them"; fi
    exe="$nm"; [ "$t" = windows ] && exe="$nm.exe"
    got=$(run_exe "$OUT" "$exe" "$t"); s=$?
    if [ "$s" = 99 ]; then echo "skip $t: cannot run here"; elif [ "$got" = "$WANT" ]; then ok "$t: expected output"; else bad "$t: output differs"; fi
done

if [ "$fails" -eq 0 ]; then echo "machine_fcallee_check: 0 failure(s)"; else echo "machine_fcallee_check: $fails failure(s)"; exit 1; fi
