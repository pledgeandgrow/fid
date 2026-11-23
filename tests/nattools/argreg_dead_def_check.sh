#!/usr/bin/env bash
# 2g.88 (docs/reference/known-bugs.md; WP7, RA3, RA7 in
# docs/roadmap/llvm-roadmap.md): on x86-64 the FID_VERIFY=1 allocation
# checker rejected argument-register assignments ("defines vN in argument
# register R, whose argument or result is still to be read"): a dead
# definition (hull one position long) took an argument register still to
# be read (compiler/nat_ir.fd ng_arg_fit / ng_af_fit checked it only
# before the range's last position; ng_arg_fit_end now checks that one).
# Run after the compiler is built:
#   bash tests/nattools/argreg_dead_def_check.sh [fid]   (default release/fid.exe)
#
# 1. tests/nattools/argreg_dead_def_unit.fd through `fid run`: its expected
#    output (the dead definition kept out of rdi / x0, the forced one
#    rejected).
# 2. The programs the bug was seen on, built with FID_VERIFY=1 (MIR
#    verifier and allocation checker) for x86_64-linux, --nat Windows and
#    x86_64-linux with FID_NAT_ON=splitall (RA4: every value split): each
#    must build and, where the image runs (Linux through WSL on Windows),
#    print what `fid run` prints, with the same exit status.
# FILES="a.fd b.fd" checks other programs instead. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
UNIT=tests/nattools/argreg_dead_def_unit.fd
FILES="${FILES:-tests/bootstrap/test_machine_simd.fd tests/bootstrap/test_machine_brflags2.fd tests/bootstrap/test_machine_argfree.fd tests/bootstrap/test_machine_holes.fd tests/bootstrap/test_machine_segs.fd tests/bootstrap/test_regex_linear.fd tests/bootstrap/test_ai_stdlib.fd}"
OUT="${TMPDIR:-/tmp}/fid-argreg-dead"
mkdir -p "$OUT"
export FIDES_NO_CACHE=1
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

# 1. the unit test
UWANT="$(expected "$UNIT")"
UGOT="$("$FID" run "$UNIT" 2>&1 | tr -d '\r' | grep -v '^\[')"
if [ "$UGOT" = "$UWANT" ]; then
    ok "argreg_dead_def_unit: output as expected"
else
    bad "argreg_dead_def_unit: output differs"
    diff <(echo "$UWANT") <(echo "$UGOT") | head -20
fi

# run_exe <dir> <exe> <mode>: output and status (99: not runnable here)
run_exe() {
    local dir="$1" exe="$2" mode="$3"
    case "$mode" in
        windows)
            [ "$HOST" = windows ] || return 99
            (cd "$dir" && "./$exe" 2>&1 | tr -d '\r'; exit "${PIPESTATUS[0]}")
            ;;
        *)
            if [ "$HOST" = windows ] && [ -z "$WSL" ]; then return 99; fi
            (cd "$dir" && $WSL "./$exe" 2>&1 | tr -d '\r'; exit "${PIPESTATUS[0]}")
            ;;
    esac
}

for src in $FILES; do
    [ -f "$src" ] || { bad "$src: missing"; continue; }
    base=$(basename "$src" .fd)
    want=$("$FID" run "$src" 2>&1)
    wrc=$?
    want=$(echo "$want" | tr -d '\r' | grep -v '^\[')
    for mode in linux windows splitall; do
        exe="$base-$mode"
        targ=(--target x86_64-linux)
        envs=(FID_VERIFY=1)
        case "$mode" in
            windows) exe="$exe.exe"; targ=(--nat) ;;
            splitall) envs+=(FID_NAT_ON=splitall) ;;
        esac
        if ! env "${envs[@]}" "$FID" emit "$src" "${targ[@]}" -o "$OUT/$exe" >"$OUT/$exe.log" 2>&1; then
            bad "$base $mode: FID_VERIFY=1 build failed: $(grep -v '^\[' "$OUT/$exe.log" | tail -2 | tr '\n' ' ')"
            continue
        fi
        got=$(run_exe "$OUT" "$exe" "$mode")
        rc=$?
        if [ $rc -eq 99 ]; then
            ok "$base $mode: builds with FID_VERIFY=1 (not run on this host)"
        elif [ "$got" = "$want" ] && [ $rc -eq $wrc ]; then
            ok "$base $mode: builds with FID_VERIFY=1, runs as fid run"
        else
            bad "$base $mode: output or status differs from fid run ($rc vs $wrc)"
            diff <(echo "$want") <(echo "$got") | head -8
        fi
    done
done

echo "argreg_dead_def_check: $fails failed (logs in $OUT)"
[ $fails -eq 0 ]
