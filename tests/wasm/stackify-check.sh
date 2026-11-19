#!/bin/bash
# tests/wasm/stackify-check.sh — structured control flow in `fid wasm`
# (TG16, compiler/wasm.fd "Structured control flow"). Each program is built
# twice: as `fid wasm` writes it (WASM block / loop / if from the CFG) and
# with FID_WASM_SWITCH=1 (every function the old switch loop). For each:
#   - both modules are valid (tests/wasm/validate.mjs, Node's validator;
#     wabt's wasm-validate too when it is on PATH);
#   - the build reports `[wasm] control flow: S structured, ...` with S > 0
#     (with FID_WASM_SWITCH=1: 0 structured);
#   - both print the same and exit with the same code under Node's WASI
#     (scripts/wasm-run.mjs).
# A function that keeps the switch loop because its CFG is irreducible is
# reported (WARN), not failed: the switch loop is still correct for it.
#
#   FID=release/fid.exe bash tests/wasm/stackify-check.sh [test.fd ...]
#     no files: tests/bootstrap/test_wasm_*.fd, test_int_semantics, hello
#               and tests/wasm/test_wasm_task_switch.fd (tasks: switch
#               loops and structured functions in one module)
#     ALL=1:    every tests/bootstrap/*.fd that is not an expected failure
#   OUT=<dir>   default build/wasm-stackify
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) DEF=release/fid.exe ;; *) DEF=release/fid ;; esac
FID="${FID:-$DEF}"
OUT="${OUT:-build/wasm-stackify}"
mkdir -p "$OUT"
TESTS=()
if [ $# -gt 0 ]; then
  TESTS=("$@")
elif [ "${ALL:-}" = 1 ]; then
  for f in tests/bootstrap/*.fd; do
    case "$(basename "$f")" in *_fail.fd|*_fail2.fd) continue ;; esac
    if head -3 "$f" | grep -q 'expect-error'; then continue; fi
    TESTS+=("$f")
  done
else
  for f in tests/bootstrap/test_wasm_*.fd; do [ -f "$f" ] && TESTS+=("$f"); done
  TESTS+=(tests/bootstrap/test_int_semantics.fd tests/bootstrap/hello.fd tests/wasm/test_wasm_task_switch.fd)
fi
WABT=0
command -v wasm-validate > /dev/null 2>&1 && WABT=1

# "S T I F" from the build log's control-flow line, empty when it has none
cf_counts() {
  grep -a -m1 '^\[wasm\] control flow:' "$1" 2>/dev/null | tr -d '\r' \
    | sed -E 's/^\[wasm\] control flow: ([0-9]+) structured, [0-9]+ switch loops \(([0-9]+) tasks, ([0-9]+) irreducible, ([0-9]+) FID_WASM_SWITCH\).*/\1 \2 \3 \4/'
}

pass=0; fail=0; skip=0; warn=0
for f in "${TESTS[@]}"; do
  b=$(basename "$f" .fd)
  if ! timeout 600 "$FID" wasm "$f" -o "$OUT/$b.wasm" > "$OUT/$b.build.log" 2>&1; then
    if [ "${ALL:-}" = 1 ]; then
      echo "SKIP  $b (does not build: $(grep -a -m1 -i 'error\|not available\|panic' "$OUT/$b.build.log" | cut -c1-120))"
      skip=$((skip+1)); continue
    fi
    echo "BUILD $b: $(grep -a -m1 -i 'error\|not available\|panic' "$OUT/$b.build.log" | cut -c1-160)"
    fail=$((fail+1)); continue
  fi
  if ! FID_WASM_SWITCH=1 timeout 600 "$FID" wasm "$f" -o "$OUT/$b.sw.wasm" > "$OUT/$b.sw.build.log" 2>&1; then
    echo "BUILD $b (FID_WASM_SWITCH=1): $(grep -a -m1 -i 'error\|panic' "$OUT/$b.sw.build.log" | cut -c1-160)"
    fail=$((fail+1)); continue
  fi
  ok=1; why=""
  read -r s t i fo <<< "$(cf_counts "$OUT/$b.build.log")"
  read -r s2 t2 i2 fo2 <<< "$(cf_counts "$OUT/$b.sw.build.log")"
  if [ -z "${s:-}" ] || [ -z "${s2:-}" ]; then
    ok=0; why="$why no control-flow line in the build log;"
  else
    [ "$s" -gt 0 ] || { ok=0; why="$why 0 structured functions;"; }
    [ "$s2" = 0 ] || { ok=0; why="$why FID_WASM_SWITCH=1 still wrote $s2 structured;"; }
    if [ "$i" != 0 ]; then
      echo "WARN  $b: $i function(s) with an irreducible CFG keep the switch loop"
      warn=$((warn+1))
    fi
  fi
  if ! node tests/wasm/validate.mjs "$OUT/$b.wasm" "$OUT/$b.sw.wasm" > "$OUT/$b.valid.log" 2>&1; then
    ok=0; why="$why $(grep -a -m1 INVALID "$OUT/$b.valid.log" | cut -c1-200);"
  fi
  if [ $WABT = 1 ]; then
    wasm-validate "$OUT/$b.wasm" > "$OUT/$b.wabt.log" 2>&1 || { ok=0; why="$why wasm-validate: $(head -1 "$OUT/$b.wabt.log" | cut -c1-160);"; }
  fi
  timeout 120 node scripts/wasm-run.mjs "$OUT/$b.wasm" < /dev/null > "$OUT/$b.out" 2> "$OUT/$b.err"
  rc=$?
  timeout 120 node scripts/wasm-run.mjs "$OUT/$b.sw.wasm" < /dev/null > "$OUT/$b.sw.out" 2> "$OUT/$b.sw.err"
  rc2=$?
  [ "$rc" = "$rc2" ] || { ok=0; why="$why exit $rc, switch loop $rc2;"; }
  if [ "$rc" = 124 ]; then ok=0; why="$why timed out;"; fi
  cmp -s <(tr -d '\r' < "$OUT/$b.out") <(tr -d '\r' < "$OUT/$b.sw.out") || { ok=0; why="$why output differs from the switch loop's;"; }
  p1=$(grep -a -m1 '^Panic:' "$OUT/$b.err" | tr -d '\r')
  p2=$(grep -a -m1 '^Panic:' "$OUT/$b.sw.err" | tr -d '\r')
  [ "$p1" = "$p2" ] || { ok=0; why="$why panic '$p1' vs '$p2';"; }
  sz=$(wc -c < "$OUT/$b.wasm"); sz2=$(wc -c < "$OUT/$b.sw.wasm")
  if [ $ok = 1 ]; then
    echo "PASS  $b ($s structured, $t task + $i irreducible switch loops; $sz bytes, switch loop $sz2)"
    pass=$((pass+1))
  else
    echo "FAIL  $b:$why"
    diff <(tr -d '\r' < "$OUT/$b.sw.out") <(tr -d '\r' < "$OUT/$b.out") | head -6
    fail=$((fail+1))
  fi
done
echo "fid wasm structured control flow: $pass passed, $fail failed, $skip skipped, $warn with irreducible functions"
[ $fail = 0 ]
