#!/usr/bin/env bash
# test-runner-check.sh — `fid test` (the suite runner and `fid test <file>`)
# and `fid repl` (docs/roadmap/bugs-roadmap.md):
#   BG410  on Windows the suite's programs ran as "tests/_fides_test_tmp.exe",
#          which cmd.exe reads as the command `tests`: every test failed
#   BG454  on POSIX `ls tests/bootstrap/*.fd` names each file with its
#          directory: every test was "(could not read)"
#   BG455  an expected-fail test is one whose first lines say
#          `// expect-error` (or a *_fail.fd name), not only one of a list
#          of 74; `fid test --fail` passes it only when the error it names
#          is printed
#   BG456  `fid test <file>` runs `@cold fn test_x()` and does not call a
#          method `fn test_helper()` of an impl as a test
#   BG480  a bench_* that prints: its time is read from the harness's own
#          line (it was "10000000000 runs  0 ns/op")
#   BG458  `fid repl` builds an expression through `fid ccompile` (CC is
#          honoured; it called tcc / gcc whatever CC said)
# Everything here builds C: it needs a C compiler (CC, else gcc, clang, cc,
# zig, tcc on PATH); without one it says so and exits 0.
#
#   FID=release/fid.exe CC="zig cc" bash tests/tooling/test-runner-check.sh
#   OUT=<dir>   default build/test-runner-check
# Exit status 1 on a failure.
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
export FID_HOME="${FID_HOME:-$(cygpath -m "$ROOT" 2>/dev/null || echo "$ROOT")}"
OUT="${OUT:-build/test-runner-check}"
mkdir -p "$OUT"
case "$OUT" in /*|[A-Za-z]:*) ;; *) OUT="$ROOT/$OUT" ;; esac
if [ -z "${CC:-}" ]; then
    for c in gcc clang cc tcc; do
        if command -v $c > /dev/null 2>&1; then export CC=$c; break; fi
    done
    if [ -z "${CC:-}" ] && command -v zig > /dev/null 2>&1; then export CC="zig cc"; fi
fi
if [ -z "${CC:-}" ]; then
    echo "test-runner-check: no C compiler (set CC): skipped"
    exit 0
fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
run() {
    "$@" > "$OUT/.so" 2> "$OUT/.se" < /dev/null; rc=$?
    so=$(tr -d '\r' < "$OUT/.so"); se=$(tr -d '\r' < "$OUT/.se")
}

# --- BG410 / BG454 / BG455: a small suite --------------------------------------
S="$OUT/suite"
mkdir -p "$S/tests/bootstrap"
printf 'fn main() {\n    println("hi");\n}\n' > "$S/tests/bootstrap/test_ok.fd"
printf '// expect-error: undefined variable\nfn main() {\n    println(zz);\n}\n' > "$S/tests/bootstrap/test_new_fail.fd"
# refused, but not with the text it names
printf '// expect-error: this text is never printed\nfn main() {\n    println(zz);\n}\n' > "$S/tests/bootstrap/test_wrong_reason.fd"
# accepted: an expected-fail test that does not fail
printf '// expect-error: undefined variable\nfn main() {\n    println(1);\n}\n' > "$S/tests/bootstrap/test_accepted.fd"
( cd "$S" && "$FID" test > "$OUT/suite.out" 2>&1 < /dev/null ); rc=$?
out=$(tr -d '\r' < "$OUT/suite.out")
if printf '%s' "$out" | grep -q "\[PASS\] test_ok.fd"; then
    ok "BG410/BG454 the suite runs and passes test_ok.fd"
else
    bad "BG410/BG454 test_ok.fd: [$(printf '%s' "$out" | grep test_ok | head -2 | tr '\n' '|')]"
fi
if ! printf '%s' "$out" | grep -q "could not read" && ! printf '%s' "$out" | grep -q "is not recognized\|n'est pas reconnu"; then
    ok "BG410/BG454 no \"could not read\", no cmd.exe error"
else
    bad "BG410/BG454 [$(printf '%s' "$out" | grep -m2 "could not read\|recogni\|reconnu" | tr '\n' '|')]"
fi
if ! printf '%s' "$out" | grep -q "test_new_fail.fd\|test_wrong_reason.fd\|test_accepted.fd" && [ "$rc" = 0 ]; then
    ok "BG455 the expect-error tests are not run as normal tests (exit 0)"
else
    bad "BG455 normal run: exit $rc, [$(printf '%s' "$out" | grep "_fail\|wrong_reason\|accepted" | tr '\n' '|')]"
fi
( cd "$S" && "$FID" test --fail > "$OUT/fail.out" 2>&1 < /dev/null ); rc=$?
out=$(tr -d '\r' < "$OUT/fail.out")
if printf '%s' "$out" | grep -q "\[PASS\] test_new_fail.fd"; then
    ok "BG455 --fail: test_new_fail.fd refused with its error"
else
    bad "BG455 --fail test_new_fail.fd: [$(printf '%s' "$out" | grep test_new_fail | tr '\n' '|')]"
fi
if printf '%s' "$out" | grep -q "\[FAIL\] test_wrong_reason.fd.*without"; then
    ok "BG455 --fail: refused for another reason is a failure"
else
    bad "BG455 --fail test_wrong_reason.fd: [$(printf '%s' "$out" | grep wrong_reason | tr '\n' '|')]"
fi
if printf '%s' "$out" | grep -q "\[FAIL\] test_accepted.fd.*accepted"; then
    ok "BG455 --fail: an accepted program is a failure"
else
    bad "BG455 --fail test_accepted.fd: [$(printf '%s' "$out" | grep test_accepted | tr '\n' '|')]"
fi
if [ "$rc" = 1 ]; then
    ok "BG455 --fail exits 1 (two of three are wrong)"
else
    bad "BG455 --fail exit $rc (want 1)"
fi

# --- BG456 -----------------------------------------------------------------------
cat > "$OUT/an.fd" <<'EOF'
fn test_plain() {
    assert(1 + 1 == 2);
}

@cold fn test_annotated() {
    assert(1 + 1 == 3);
}
EOF
run "$FID" test "$OUT/an.fd"
if [ "$rc" != 0 ] && printf '%s' "$so$se" | grep -q "test_annotated"; then
    ok "BG456 @cold fn test_x() is run (and fails)"
else
    bad "BG456 @cold test: exit $rc, [$(printf '%s' "$so" | tail -3 | tr '\n' '|')]"
fi
cat > "$OUT/t.fd" <<'EOF'
struct S {
    x: i64,
}

impl S {
    fn test_helper() -> i64 {
        return 1;
    }
}

/* fn test_old() { assert(false); } */

fn test_a() {
    assert(S::test_helper() == 1);
}
EOF
run "$FID" test "$OUT/t.fd"
if [ "$rc" = 0 ] && printf '%s' "$so" | grep -q "1 passed"; then
    ok "BG456 an impl's fn test_helper() and a commented-out test are not tests"
else
    bad "BG456 impl/comment: exit $rc, [$(printf '%s' "$so$se" | grep -m3 "error\|passed\|failed" | tr '\n' '|')]"
fi

# --- BG480 -----------------------------------------------------------------------
cat > "$OUT/b.fd" <<'EOF'
var G: i64 = 0;

fn bench_print() {
    G = G + 1;
    if G == 1 {
        println("warming up");
    }
}

fn test_one() {
    assert(true);
}
EOF
run timeout 600 "$FID" test "$OUT/b.fd"
line=$(printf '%s' "$so" | grep "bench_print" | head -1)
if [ -n "$line" ] && ! printf '%s' "$line" | grep -q "10000000000 runs" && ! printf '%s' "$line" | grep -q "FAIL"; then
    ok "BG480 a printing bench is timed: [$line]"
else
    bad "BG480 bench: exit $rc, [$line]"
fi

# --- BG458 -----------------------------------------------------------------------
( cd "$OUT" && printf '1 + 2\n:quit\n' | "$FID" repl > "$OUT/repl.out" 2> "$OUT/repl.err" ); rc=$?
if tr -d '\r' < "$OUT/repl.out" | grep -qx "3"; then
    ok "BG458 fid repl evaluates 1 + 2 with CC=$CC"
else
    bad "BG458 repl: exit $rc, stdout [$(tr -d '\r' < "$OUT/repl.out" | head -3)], stderr [$(tr -d '\r' < "$OUT/repl.err" | grep -v "^fid REPL" | head -3)]"
fi

if [ "$fails" -gt 0 ]; then
    echo "test-runner-check: $fails failed"
    exit 1
fi
echo "test-runner-check: all passed"
