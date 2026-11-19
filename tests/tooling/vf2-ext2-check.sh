#!/bin/bash
# vf2-ext2-check.sh — VF2 (competitive roadmap): checks the parts of
# scripts/fuzz/gen_vs_c.py added on Nov 17: --ext2 (floats printed to a
# fixed precision by `ffix`, str_slice / str_byte_at / str == / f-strings,
# a `_` arm on the enum), --jobs and --log.
#
# Usage: tests/tooling/vf2-ext2-check.sh
#   FID=<compiler>  default release/fid.exe (release/fid off Windows)
#   OUT=<dir>       default build/vf2-ext2-check
#   N=<programs>    for check 5, default 6
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. --ext2 is deterministic for a seed, and does not change the program of
#     a seed without it or with --ext only (its random numbers are its own)
#  2. over seeds 1..30, --ext2 programs use every new form at least once:
#     `fn ffix`, `println(ffix(`, `str_slice(`, `str_byte_at(`, `st == "`,
#     `f"{st}<`, `_ =>` inside a match on `en`
#  3. the C side of 10 --ext2 programs builds with zig cc (-O2) and exits 0
#     or 1 (a panic), and prints the `ffix` lines as -?D+.DDD
#  4. the Fides `ffix` on fixed values (tests/bootstrap/test_verify_vf2_ffix.fd)
#     prints what C's "%lld.%03lld" prints, on run and ccompile
#  5. N --ext2 programs on run and ccompile with --jobs 2 and --log: every
#     C build agrees (no oracle disagreement), the log has one line per seed
#     and the summary; differences from C are compiler bugs, listed on the
#     PASS line to triage (STRICT=1 makes them a failure)
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) FID="${FID:-release/fid.exe}" ;; *) FID="${FID:-release/fid}" ;; esac
OUT="${OUT:-build/vf2-ext2-check}"
N="${N:-6}"
mkdir -p "$OUT"
PY=python3
"$PY" -c 1 > /dev/null 2>&1 || PY=python
export PYTHONIOENCODING=utf-8
G=scripts/fuzz/gen_vs_c.py
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

# 1
"$PY" "$G" --print 7 > "$OUT/p7.txt" 2>&1
"$PY" "$G" --print 7 --ext > "$OUT/p7x.txt" 2>&1
"$PY" "$G" --print 7 --ext2 > "$OUT/p7y1.txt" 2>&1
"$PY" "$G" --print 7 --ext2 > "$OUT/p7y2.txt" 2>&1
"$PY" "$G" --print 7 > "$OUT/p7b.txt" 2>&1
"$PY" "$G" --print 7 --ext > "$OUT/p7xb.txt" 2>&1
if cmp -s "$OUT/p7y1.txt" "$OUT/p7y2.txt" && cmp -s "$OUT/p7.txt" "$OUT/p7b.txt" \
   && cmp -s "$OUT/p7x.txt" "$OUT/p7xb.txt" && ! cmp -s "$OUT/p7x.txt" "$OUT/p7y1.txt" \
   && grep -q '^fn ffix' "$OUT/p7y1.txt" && ! grep -q 'ffix' "$OUT/p7x.txt"; then
  pass "--ext2 is deterministic and leaves plain and --ext programs as they were"
else
  fail "--ext2 determinism / independence (see $OUT/p7*.txt)"
fi

# 2
: > "$OUT/all.txt"
for s in $(seq 1 30); do "$PY" "$G" --print "$s" --ext2 >> "$OUT/all.txt" 2>&1; done
miss=""
for pat in '^fn ffix' 'println(ffix(' 'str_slice(st, ' 'str_byte_at(st, ' 'if st == "' 'f"{st}<' '    _ => {'; do
  grep -qF -- "${pat#^}" "$OUT/all.txt" || miss="$miss [$pat]"
done
grep -q 'pfix(' "$OUT/all.txt" || miss="$miss [pfix in C]"
grep -q 'sslice(&st' "$OUT/all.txt" || miss="$miss [sslice in C]"
if [ -z "$miss" ]; then
  pass "seeds 1..30 with --ext2 use every new form (Fides and C)"
else
  fail "forms never generated:$miss (see $OUT/all.txt)"
fi

# 3
ok=0; bads=""
for s in $(seq 40 49); do
  "$PY" - "$s" "$OUT" <<'EOF'
import sys
sys.path.insert(0, "scripts/fuzz")
import gen_vs_c as g
g.EXT[0] = True
g.EXT2[0] = True
fd, c = g.pair(int(sys.argv[1]))
open(f"{sys.argv[2]}/c{sys.argv[1]}.c", "w", newline="\n").write(c)
EOF
  if zig cc -O2 -w "$OUT/c$s.c" -o "$OUT/c$s.exe" > /dev/null 2>&1; then
    "$OUT/c$s.exe" > "$OUT/c$s.out" 2>/dev/null
    rc=$?
    # the last three lines before xs.len() are ffix(fa), ffix(fb), str_len
    if { [ "$rc" = 0 ] && tail -n 4 "$OUT/c$s.out" | head -n 2 | grep -Eqv '^-?[0-9]+\.[0-9]{3}$'; }; then
      bads="$bads $s(format)"
    elif [ "$rc" = 0 ] || [ "$rc" = 1 ]; then
      ok=$((ok + 1))
    else
      bads="$bads $s(exit $rc)"
    fi
  else
    bads="$bads $s(cc)"
  fi
done
if [ "$ok" = 10 ]; then
  pass "the C of 10 --ext2 programs builds, runs and prints ffix as -?D.DDD"
else
  fail "C side of --ext2: $ok of 10 ok;$bads (see $OUT/c*.c)"
fi

# 4
if [ -x "$FID" ] || [ -x "$FID.exe" ]; then
  want=$(sed -n 's#^// \([-0-9][0-9.]*\)$#\1#p' tests/bootstrap/test_verify_vf2_ffix.fd)
  r1=$("$FID" run tests/bootstrap/test_verify_vf2_ffix.fd 2>/dev/null | tr -d '\r')
  "$FID" ccompile tests/bootstrap/test_verify_vf2_ffix.fd -o "$OUT/ffix.exe" > /dev/null 2>&1
  r2=$("$OUT/ffix.exe" 2>/dev/null | tr -d '\r')
  if [ -n "$want" ] && [ "$r1" = "$want" ] && [ "$r2" = "$want" ]; then
    pass "ffix prints the fixed-precision text on run and ccompile"
  else
    fail "ffix: run and ccompile against the expected lines (run: $(echo $r1 | head -c 200); ccompile: $(echo $r2 | head -c 200))"
  fi

  # 5
  rm -f "$OUT/run.log5"
  "$PY" "$G" -n "$N" --seed 3000 --ext2 --fid "$FID" --backends run,ccompile --cc clang \
    --jobs 2 --log "$OUT/run.log5" --keep "$OUT/run" > "$OUT/run.out5" 2>&1
  rc=$?
  lines=$(grep -c '^[0-9]* \(ok\|differs\|oracle\)' "$OUT/run.log5" 2>/dev/null)
  summary=$(grep -E "^$N programs: [0-9]+ differ from C, 0 with a C oracle" "$OUT/run.out5")
  if [ -n "$summary" ] && [ "$lines" = "$N" ] && grep -q '^# .*programs:' "$OUT/run.log5"; then
    if [ "$rc" = 0 ]; then
      pass "$N --ext2 programs agree with clang on run and ccompile (--jobs 2, log of $lines seeds)"
    elif [ "${STRICT:-0}" != 1 ]; then
      pass "$N --ext2 programs ran (--jobs 2, log of $lines seeds); differences to triage: $(grep 'differs from C' "$OUT/run.out5" | tr '\n' ';')"
    else
      fail "--ext2 programs differ from C: $(grep 'differs from C' "$OUT/run.out5" | tr '\n' ';')"
    fi
  else
    fail "--ext2 run: exit $rc, $lines log lines; $(tail -n 1 "$OUT/run.out5") (see $OUT/run.out5)"
  fi
else
  fail "no compiler at $FID (checks 4 and 5 not run)"
fi
exit $bad
