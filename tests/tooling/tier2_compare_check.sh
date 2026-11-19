#!/bin/bash
# tier2_compare_check.sh — PL3: a lane built on one machine and run on
# another (Windows ARM64, FreeBSD) is compared by scripts/cross-test.sh
# COMPARE_ONLY=1: no build, no run, the results run-all.sh left in OUT
# compared with the references made at build time. Needs no compiler.
#
# Checks: matching output and exit code -> PASS; other output -> FAIL; a
# test that did not build -> BUILD; a binary that never ran -> FAIL; the
# run-all.sh a built-only lane writes cds to the root and runs each binary;
# HOST_ARCH=aarch64 makes an aarch64-windows lane one this host runs.
# Usage: bash tests/tooling/tier2_compare_check.sh   (exit 0 = all good)
set -u
cd "$(dirname "$0")/../.." || exit 1
W=$(mktemp -d "${TMPDIR:-/tmp}/tier2cmp.XXXXXX")
trap 'rm -rf "$W"' EXIT
OUT="build/tier2-compare-check"
rm -rf "$OUT"
mkdir -p "$OUT"
bad=0
t1=tests/bootstrap/hello.fd
t2=tests/bootstrap/arithmetic.fd
t3=tests/bootstrap/test_int_width.fd
for t in "$t1" "$t2" "$t3"; do [ -f "$t" ] || { echo "missing $t"; exit 1; }; done
# references as the build machine left them
printf 'Hello\n' > "$OUT/hello.expected";       : > "$OUT/hello.expected_err";       echo 0 > "$OUT/hello.expected_rc"
printf '42\n' > "$OUT/arithmetic.expected";     : > "$OUT/arithmetic.expected_err";  echo 0 > "$OUT/arithmetic.expected_rc"
printf 'x\n' > "$OUT/test_int_width.expected";  : > "$OUT/test_int_width.expected_err"; echo 0 > "$OUT/test_int_width.expected_rc"
echo 0 > "$OUT/hello.build_rc"; echo 0 > "$OUT/arithmetic.build_rc"; echo 1 > "$OUT/test_int_width.build_rc"
# what the target machine's run left: hello right (CRLF is fine), arithmetic wrong
printf 'Hello\r\n' > "$OUT/hello.actual";  : > "$OUT/hello.stderr";      echo 0 > "$OUT/hello.actual_rc"
printf '41\n' > "$OUT/arithmetic.actual";  : > "$OUT/arithmetic.stderr"; echo 0 > "$OUT/arithmetic.actual_rc"

expect() {  # results.txt line prefix
  if ! grep -q "^$1" "$OUT/results.txt"; then
    echo "FAIL: expected a line '$1' in results.txt:"; sed 's/^/  /' "$OUT/results.txt"; bad=$((bad + 1))
  fi
}
FID=/nonexistent COMPARE_ONLY=1 OUT="$OUT" bash scripts/cross-test.sh x86_64-freebsd "$t1" "$t2" "$t3" > "$W/log" 2>&1
rc=$?
expect "PASS  hello"
expect "FAIL  arithmetic"
expect "BUILD test_int_width"
[ "$rc" != 0 ] || { echo "FAIL: cross-test.sh exited 0 with a failing test"; bad=$((bad + 1)); }
grep -q "comparing the results run-all.sh left" "$W/log" || { echo "FAIL: no COMPARE_ONLY message"; cat "$W/log"; bad=$((bad + 1)); }
# no reference was rewritten (nothing was built or run)
[ "$(cat "$OUT/hello.expected")" = Hello ] || { echo "FAIL: the reference was rebuilt"; bad=$((bad + 1)); }

# all right now: exit 0
printf '42\n' > "$OUT/arithmetic.actual"
FID=/nonexistent COMPARE_ONLY=1 OUT="$OUT" bash scripts/cross-test.sh x86_64-freebsd "$t1" "$t2" > "$W/log2" 2>&1 \
  || { echo "FAIL: cross-test.sh failed on matching results"; cat "$W/log2"; bad=$((bad + 1)); }

# run-all.sh for the built tests: back to the root, each binary once
grep -q 'cd "$(dirname "$0")/../.."' "$OUT/run-all.sh" || { echo "FAIL: run-all.sh does not cd to the root"; head -3 "$OUT/run-all.sh"; bad=$((bad + 1)); }
[ "$(grep -c 'hello.bin\|arithmetic.bin' "$OUT/run-all.sh")" = 2 ] || { echo "FAIL: run-all.sh does not run the two built binaries"; cat "$OUT/run-all.sh"; bad=$((bad + 1)); }

# HOST_ARCH: an aarch64-windows lane is built only on an x86-64 host and run
# on an ARM64 one (only meaningful on Windows: elsewhere both are built only)
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*)
    O2="$OUT-arm"; rm -rf "$O2"; mkdir -p "$O2"
    printf 'Hello\n' > "$O2/hello.expected"; : > "$O2/hello.expected_err"; echo 0 > "$O2/hello.expected_rc"; echo 0 > "$O2/hello.build_rc"
    : > "$O2/hello.exe"
    FID=/nonexistent REUSE=1 HOST_ARCH=x86_64 OUT="$O2" bash scripts/cross-test.sh aarch64-windows "$t1" > "$W/log3" 2>&1
    grep -q "built only" "$W/log3" || { echo "FAIL: aarch64-windows ran on an x86-64 host"; cat "$W/log3"; bad=$((bad + 1)); }
    rm -rf "$O2" ;;
esac
rm -rf "$OUT"
if [ "$bad" = 0 ]; then echo "tier2_compare_check: ok"; else echo "tier2_compare_check: $bad failed"; fi
exit "$bad"
