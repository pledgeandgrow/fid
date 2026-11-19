#!/bin/bash
# vf11-own-model-check.sh — VF11 (competitive roadmap): the ownership model
# (scripts/verify/own_model.py) with its extensions, and the differential
# check of the compiler's lowering against it (scripts/verify/own_diff.py).
#
# Usage: tests/tooling/vf11-own-model-check.sh
#   FID=<compiler>  default release/fid.exe (release/fid off Windows); the
#                   differential checks (5, 6) are skipped when it is missing
#   PER=<n>         programs per feature for check 5 (default 30)
#   OUT=<dir>       default build/vf11-check
#
# Checks, one line each (PASS/FAIL/SKIP), exit status the number of FAILs:
#  1. own_model.py --size 4 --self-test: the base calculus and each
#     extension (refs2, struct, enum, closure, rc) explored with 0 failures,
#     and every mutant caught (17 of them).
#  2. the base calculus gives its numbers at size 4
#     (70,771 programs, 46,671 accepted; Nov 16 gave 46,089: the 582 more
#     are two-phase pushes `x.push(*r)`, accepted since Nov 18).
#  3. own_diff.py --self-test: the translation to Fides and the expected
#     output of fixed programs (no compiler needed).
#  4. --ext refs2 --self-test catches one_ref_borrowck, and --ext none runs
#     no extension (the switches work).
#  5. own_diff.py --fid $FID --size 3 --per $PER: every feature's programs
#     print what the model's value run reads, leak nothing (--leaks) and do
#     not fault (--checked-alloc). The report is $OUT/own_diff.json.
#  6. own_diff.py --rejected 10 --modes checked on base and refs2: each
#     program the model rejects for a borrow or escape reason is rejected
#     by `fid check` too.
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
if [ $WIN = 1 ]; then FID="${FID:-release/fid.exe}"; else FID="${FID:-release/fid}"; fi
PY="${PY:-python3}"
command -v "$PY" > /dev/null 2>&1 || PY=python
PER="${PER:-30}"
OUT="${OUT:-build/vf11-check}"
mkdir -p "$OUT"
fails=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; fails=$((fails + 1)); }
skip() { echo "SKIP $1"; }

# 1
"$PY" scripts/verify/own_model.py --size 4 --self-test > "$OUT/model.txt" 2>&1
rc=$?
nexts=$(grep -c '^own_model: ext .* 0 failures' "$OUT/model.txt")
ncaught=$(grep -c 'caught:' "$OUT/model.txt")
if [ $rc -eq 0 ] && [ "$nexts" -eq 5 ] && [ "$ncaught" -eq 17 ] && ! grep -q 'NOT caught' "$OUT/model.txt"; then
    pass "1 model, 5 extensions, 17 mutants caught"
else
    fail "1 model (exit $rc, $nexts extensions clean, $ncaught mutants caught; $OUT/model.txt)"
fi

# 2
if grep -q '^own_model: size 4, 70771 programs, 46671 accepted, 0 failures' "$OUT/model.txt"; then
    pass "2 base calculus at size 4: 46,671 accepted"
else
    fail "2 base calculus at size 4: $(grep -m1 '^own_model: size 4' "$OUT/model.txt")"
fi

# 3
if "$PY" scripts/verify/own_diff.py --self-test > "$OUT/diff-self.txt" 2>&1 && grep -q 'self-test: ok' "$OUT/diff-self.txt"; then
    pass "3 translator self-test"
else
    fail "3 translator self-test: $(tail -3 "$OUT/diff-self.txt")"
fi

# 4
"$PY" scripts/verify/own_model.py --size 4 --ext refs2 --self-test > "$OUT/refs2.txt" 2>&1
"$PY" scripts/verify/own_model.py --size 2 --ext none > "$OUT/none.txt" 2>&1
if grep -q 'mutant one_ref_borrowck .*caught:' "$OUT/refs2.txt" && ! grep -q 'NOT caught' "$OUT/refs2.txt" \
        && ! grep -q 'ext ' "$OUT/none.txt" && ! grep -q 'mutant struct' "$OUT/refs2.txt"; then
    pass "4 --ext switches"
else
    fail "4 --ext switches ($OUT/refs2.txt, $OUT/none.txt)"
fi

# 5, 6
if [ ! -x "$FID" ] && [ ! -f "$FID" ]; then
    skip "5 differential run (no compiler at $FID)"
    skip "6 rejected programs (no compiler at $FID)"
else
    "$PY" scripts/verify/own_diff.py --fid "$FID" --size 3 --per "$PER" --out "$OUT/diff" > "$OUT/diff.txt" 2>&1
    rc=$?
    if [ $rc -eq 0 ]; then
        pass "5 differential run: $(grep -c '^own_diff: ' "$OUT/diff.txt") lines, no mismatch, leak or fault"
    else
        fail "5 differential run (exit $rc): $(grep '^own_diff: ' "$OUT/diff.txt" | head -6 | tr '\n' ' ')"
    fi
    "$PY" scripts/verify/own_diff.py --fid "$FID" --size 4 --ext base,refs2 --per 0 --modes checked \
        --rejected 10 --out "$OUT/rej" > "$OUT/rej.txt" 2>&1
    if grep -q 'rejected-but-accepted 0/10' "$OUT/rej.txt" && [ "$(grep -c 'rejected-but-accepted 0/10' "$OUT/rej.txt")" -eq 2 ]; then
        pass "6 rejected programs rejected by fid"
    else
        fail "6 rejected programs: $(grep '^own_diff: ' "$OUT/rej.txt" | head -3 | tr '\n' ' ')"
    fi
fi
echo "vf11-own-model-check: $fails failed"
exit $fails
