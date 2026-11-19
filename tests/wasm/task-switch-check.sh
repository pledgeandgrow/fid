#!/bin/bash
# tests/wasm/task-switch-check.sh — the `fid wasm` programs of this folder
# whose reference is not the interpreter (PL4): tasks that switch stacks
# (the interpreter runs a task to its end when it starts) and processes,
# which WASI does not have. Each tests/wasm/test_wasm_<name>.fd is built
# with `fid wasm`, run under Node's WASI (scripts/wasm-run.mjs) and checked
# against test_wasm_<name>.expected (stdout), .expected_rc (exit code,
# default 0) and .expected_err (the first line of stderr that starts with
# "Panic:", when the file exists).
#
#   FID=release/fid.exe bash tests/wasm/task-switch-check.sh
#   OUT=<dir>   default build/wasm-checks
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) DEF=release/fid.exe ;; *) DEF=release/fid ;; esac
FID="${FID:-$DEF}"
OUT="${OUT:-build/wasm-checks}"
mkdir -p "$OUT"
pass=0; fail=0
for f in tests/wasm/test_wasm_*.fd; do
  b=$(basename "$f" .fd)
  exp="tests/wasm/$b.expected"
  [ -f "$exp" ] || continue
  if ! timeout 600 "$FID" wasm "$f" -o "$OUT/$b.wasm" > "$OUT/$b.build.log" 2>&1; then
    echo "BUILD $b: $(grep -a -m1 -i 'error\|not available\|panic' "$OUT/$b.build.log" | cut -c1-160)"
    fail=$((fail+1)); continue
  fi
  timeout 60 node scripts/wasm-run.mjs "$OUT/$b.wasm" < /dev/null > "$OUT/$b.out" 2> "$OUT/$b.err"
  rc=$?
  want_rc=0
  [ -f "tests/wasm/$b.expected_rc" ] && want_rc=$(tr -d '\r\n ' < "tests/wasm/$b.expected_rc")
  ok=1
  [ "$rc" = "$want_rc" ] || ok=0
  cmp -s <(tr -d '\r' < "$exp") <(tr -d '\r' < "$OUT/$b.out") || ok=0
  if [ -f "tests/wasm/$b.expected_err" ]; then
    want_err=$(head -1 "tests/wasm/$b.expected_err" | tr -d '\r')
    got_err=$(grep -a -m1 '^Panic:' "$OUT/$b.err" | tr -d '\r')
    [ "$want_err" = "$got_err" ] || ok=0
  fi
  if [ $ok = 1 ]; then
    echo "PASS  $b"; pass=$((pass+1))
  else
    echo "FAIL  $b (rc $rc, want $want_rc)"
    diff <(tr -d '\r' < "$exp") <(tr -d '\r' < "$OUT/$b.out") | head -10
    head -3 "$OUT/$b.err"
    fail=$((fail+1))
  fi
done
echo "fid wasm checks: $pass passed, $fail failed"
[ $fail = 0 ]
