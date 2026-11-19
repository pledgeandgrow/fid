#!/bin/bash
# irocache-check.sh — CS6 (competitive-roadmap): the memo of iropt_native's
# per-function passes (compiler/iropt.fd iro_imc_key / iro_imc_salt,
# compiler/ir_ser.fd imc_*), opt-in with FID_IROCACHE=<file>.
#
# Usage: tests/tooling/irocache-check.sh
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/irocache
#   NAT=1            the machine layer (`emit --nat`) instead of pe_ir
#   SUITE=1          also step 5 over every tests/bootstrap/test_*.fd that
#                    is not _fail.fd (slow)
#   TIME=1           also step 6 (timings, informational)
#
# 1. Cold: the first build of tests/bootstrap/test_irocache_closure.fd with
#    an empty memo prints "[irocache] 0 hits, M misses, M recorded, 0
#    unkeyed, 0 mismatches" (M > 0), writes the file, and its executable
#    prints the test's "Expected output" block.
# 2. Warm: the same build again: 0 misses, M hits; the executable is the
#    same bytes as one built without the memo (when two builds without it
#    are the same bytes; else SKIP) and prints the same.
# 3. Edits: a copy with the line marked IROCACHE-EDIT1 (a leaf inlined into
#    two levels of callers), EDIT2 (a dyn method's body) or EDIT3 (a
#    function passed by reference) changed, built with the memo of steps
#    1-2: some misses (the changed function and what names it) and some
#    hits (`other` and the runtime); the executable prints what one built
#    without the memo prints, and that differs from the unedited output (a
#    stale hit would print the old values); bytes equal as in 2.
# 4. Off: with FID_IROPT_STATS=1, FIDES_NO_CACHE=1 or --no-cache the memo
#    prints no [irocache] line (the passes run, nothing is read).
# 5. Verify: FID_IROCACHE_VERIFY=1 on a warm memo runs the passes on every
#    hit and compares: "0 mismatches" and no MISMATCH line, for the test
#    (and, SUITE=1, each program of the suite: a cold build, then a verified
#    warm one; MISMATCH lines are failures, programs that do not build are
#    SKIP).
# 6. TIME=1: `emit tests/bootstrap/hello.fd --nat --time` with FID_FNCACHE
#    and the memo, cold then warm, and without the memo: the iropt rows.
# Exit status: the number of failed checks.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/irocache}"
NAT="${NAT:-0}"
SUITE="${SUITE:-0}"
TIME="${TIME:-0}"
mkdir -p "$OUT"
MEMO="$OUT/imc.bin"
T=tests/bootstrap/test_irocache_closure.fd
bad=0
EMIT=()
if [ "$NAT" = 1 ]; then EMIT=(--nat); fi
n=0

# build <exe> <src> <env...> : 0 when the executable was made. Every build
# gets its own output name (the whole-output stamp would answer "is up to
# date") and no function cache (its replays relink: bytes may differ).
build() {
  local exe="$1" src="$2"; shift 2
  rm -f "$exe"
  env FID_FNCACHE=off "$@" "$FID" emit "$src" "${EMIT[@]}" -o "$exe" > "$exe.log" 2>&1 && [ -f "$exe" ]
}
# stat <log> <field>: hits / misses / recorded / unkeyed / mismatches
stat() {
  local line
  line=$(grep -m1 '^\[irocache\]' "$1")
  case "$2" in
    hits) echo "$line" | sed -n 's/^\[irocache\] \([0-9]*\) hits.*/\1/p' ;;
    misses) echo "$line" | sed -n 's/.* \([0-9]*\) misses.*/\1/p' ;;
    recorded) echo "$line" | sed -n 's/.* \([0-9]*\) recorded.*/\1/p' ;;
    unkeyed) echo "$line" | sed -n 's/.* \([0-9]*\) unkeyed.*/\1/p' ;;
    mismatches) echo "$line" | sed -n 's/.* \([0-9]*\) mismatches.*/\1/p' ;;
  esac
}
ok() { echo "OK    $*"; }
fail() { echo "FAIL  $*"; bad=$((bad + 1)); }
run() { timeout 60 "$1" 2>&1 | tr -d '\r'; }
expected() {  # <test.fd> -> the lines of its "// Expected output:" block
  awk '/^\/\/ Expected output:/{on=1; next} on && /^\/\/ /{sub(/^\/\/ /, ""); print; next} on{exit}' "$1"
}

# two builds without the memo: is the output deterministic?
build "$OUT/plain0.exe" "$T" FID_IROCACHE=off
build "$OUT/plain1.exe" "$T" FID_IROCACHE=off
SAMEBYTES=0
if [ -f "$OUT/plain0.exe" ] && cmp -s "$OUT/plain0.exe" "$OUT/plain1.exe"; then SAMEBYTES=1; fi
bytes_check() {  # <label> <exe with memo> <exe without>
  if [ "$SAMEBYTES" != 1 ]; then
    echo "SKIP  $1: bytes (two builds without the memo differ)"
  elif cmp -s "$2" "$3"; then
    ok "$1: same bytes as without the memo"
  else
    fail "$1: bytes differ from the build without the memo"
  fi
}

# 1. cold
rm -f "$MEMO"
if build "$OUT/cold.exe" "$T" FID_IROCACHE="$MEMO" FID_IROCACHE_STATS=1; then
  h=$(stat "$OUT/cold.exe.log" hits); m=$(stat "$OUT/cold.exe.log" misses); r=$(stat "$OUT/cold.exe.log" recorded)
  if [ "$h" = 0 ] && [ -n "$m" ] && [ "$m" -gt 0 ] && [ "$r" = "$m" ] && [ -s "$MEMO" ]; then
    ok "1. cold: $(grep -m1 '^\[irocache\]' "$OUT/cold.exe.log")"
  else
    fail "1. cold: '$(grep -m1 '^\[irocache\]' "$OUT/cold.exe.log")' memo file: $(ls -l "$MEMO" 2>&1)"
  fi
  expected "$T" > "$OUT/expected.txt"
  run "$OUT/cold.exe" > "$OUT/cold.out"
  if cmp -s "$OUT/expected.txt" "$OUT/cold.out"; then ok "1. cold: output"; else fail "1. cold: output"; diff "$OUT/expected.txt" "$OUT/cold.out" | head -6; fi
  COLD_M="$m"
else
  fail "1. cold: build: $(grep -m1 -i -E 'error|panic' "$OUT/cold.exe.log")"
  COLD_M=""
fi

# 2. warm
if build "$OUT/warm.exe" "$T" FID_IROCACHE="$MEMO" FID_IROCACHE_STATS=1; then
  h=$(stat "$OUT/warm.exe.log" hits); m=$(stat "$OUT/warm.exe.log" misses)
  if [ "$m" = 0 ] && [ -n "$h" ] && [ "$h" = "$COLD_M" ]; then
    ok "2. warm: $(grep -m1 '^\[irocache\]' "$OUT/warm.exe.log")"
  else
    fail "2. warm: '$(grep -m1 '^\[irocache\]' "$OUT/warm.exe.log")' (expected 0 misses, $COLD_M hits)"
  fi
  run "$OUT/warm.exe" > "$OUT/warm.out"
  if cmp -s "$OUT/expected.txt" "$OUT/warm.out"; then ok "2. warm: output"; else fail "2. warm: output"; fi
  bytes_check "2. warm" "$OUT/warm.exe" "$OUT/plain0.exe"
else
  fail "2. warm: build: $(grep -m1 -i -E 'error|panic' "$OUT/warm.exe.log")"
fi

# 3. edits, each on the memo the builds before left
edit() {  # <k> -> sed expression for IROCACHE-EDIT<k>
  case "$1" in
    1) echo 's|return 3; // IROCACHE-EDIT1|return 4; // IROCACHE-EDIT1|' ;;
    2) echo 's|return self.s \* self.s; // IROCACHE-EDIT2|return self.s * self.s + 1; // IROCACHE-EDIT2|' ;;
    3) echo 's|return x \* x; // IROCACHE-EDIT3|return x * x + 2; // IROCACHE-EDIT3|' ;;
  esac
}
for k in 1 2 3; do
  src="$OUT/edit$k.fd"
  sed "$(edit $k)" "$T" > "$src"
  if cmp -s "$src" "$T"; then fail "3. edit $k: the sed did not change the copy"; continue; fi
  if ! build "$OUT/edit$k.exe" "$src" FID_IROCACHE="$MEMO" FID_IROCACHE_STATS=1; then
    fail "3. edit $k: build: $(grep -m1 -i -E 'error|panic' "$OUT/edit$k.exe.log")"; continue
  fi
  if ! build "$OUT/edit$k.plain.exe" "$src" FID_IROCACHE=off; then
    fail "3. edit $k: build without the memo"; continue
  fi
  h=$(stat "$OUT/edit$k.exe.log" hits); m=$(stat "$OUT/edit$k.exe.log" misses)
  if [ -n "$h" ] && [ -n "$m" ] && [ "$h" -gt 0 ] && [ "$m" -gt 0 ]; then
    ok "3. edit $k: $(grep -m1 '^\[irocache\]' "$OUT/edit$k.exe.log")"
  else
    fail "3. edit $k: '$(grep -m1 '^\[irocache\]' "$OUT/edit$k.exe.log")' (expected hits and misses)"
  fi
  run "$OUT/edit$k.exe" > "$OUT/edit$k.out"
  run "$OUT/edit$k.plain.exe" > "$OUT/edit$k.plain.out"
  if cmp -s "$OUT/edit$k.out" "$OUT/edit$k.plain.out" && ! cmp -s "$OUT/edit$k.out" "$OUT/expected.txt"; then
    ok "3. edit $k: output as without the memo, and changed"
  else
    fail "3. edit $k: output (with memo vs without; must differ from the unedited output)"
    diff "$OUT/edit$k.out" "$OUT/edit$k.plain.out" | head -6
  fi
  bytes_check "3. edit $k" "$OUT/edit$k.exe" "$OUT/edit$k.plain.exe"
done

# 4. off
for mode in stats nocache flag; do
  case "$mode" in
    stats) build "$OUT/off.$mode.exe" "$T" FID_IROCACHE="$MEMO" FID_IROCACHE_STATS=1 FID_IROPT_STATS=1 ;;
    nocache) build "$OUT/off.$mode.exe" "$T" FID_IROCACHE="$MEMO" FID_IROCACHE_STATS=1 FIDES_NO_CACHE=1 ;;
    flag) rm -f "$OUT/off.$mode.exe"; env FID_FNCACHE=off FID_IROCACHE="$MEMO" FID_IROCACHE_STATS=1 "$FID" emit "$T" "${EMIT[@]}" --no-cache -o "$OUT/off.$mode.exe" > "$OUT/off.$mode.exe.log" 2>&1 ;;
  esac
  if [ -f "$OUT/off.$mode.exe" ] && ! grep -q '^\[irocache\]' "$OUT/off.$mode.exe.log"; then
    ok "4. off ($mode)"
  else
    fail "4. off ($mode): $(grep -m1 '^\[irocache\]' "$OUT/off.$mode.exe.log") $(grep -m1 -i -E 'error|panic' "$OUT/off.$mode.exe.log")"
  fi
done

# 5. verify
verify() {  # <label> <src>
  local memo="$OUT/v.imc.bin" exe="$OUT/v.exe"
  rm -f "$memo"
  if ! build "$exe" "$2" FID_IROCACHE="$memo"; then echo "SKIP  5. $1: does not build"; return; fi
  if ! build "$exe" "$2" FID_IROCACHE="$memo" FID_IROCACHE_STATS=1 FID_IROCACHE_VERIFY=1; then fail "5. $1: verified build"; return; fi
  local mm
  mm=$(stat "$exe.log" mismatches)
  if [ "$mm" = 0 ] && ! grep -q 'MISMATCH' "$exe.log"; then
    ok "5. $1: $(grep -m1 '^\[irocache\]' "$exe.log")"
  else
    fail "5. $1: $(grep -m1 '^\[irocache\]' "$exe.log") $(grep -m3 MISMATCH "$exe.log" | tr '\n' ' ')"
  fi
}
verify "$(basename "$T")" "$T"
if [ "$SUITE" = 1 ]; then
  for t in tests/bootstrap/test_*.fd; do
    case "$t" in *_fail.fd) continue ;; esac
    verify "$(basename "$t")" "$t"
  done
fi

# 6. timings (informational)
if [ "$TIME" = 1 ]; then
  tm="$OUT/time.imc.bin"; fc="$OUT/time.fncache"
  rm -f "$tm" "$fc"
  for run_ in cold warm; do
    env FID_FNCACHE="$fc" FID_IROCACHE="$tm" FID_IROCACHE_STATS=1 "$FID" emit tests/bootstrap/hello.fd --nat --time -o "$OUT/hello.$run_.exe" > "$OUT/hello.$run_.log" 2>&1
    echo "TIME  hello --nat, memo $run_: $(grep -m1 '^\[irocache\]' "$OUT/hello.$run_.log")"
    grep -i -E 'iropt|mid-end|total' "$OUT/hello.$run_.log" | head -8 | sed 's/^/      /'
  done
  env FID_FNCACHE="$fc" FID_IROCACHE=off "$FID" emit tests/bootstrap/hello.fd --nat --time -o "$OUT/hello.off.exe" > "$OUT/hello.off.log" 2>&1
  echo "TIME  hello --nat, no memo:"
  grep -i -E 'iropt|mid-end|total' "$OUT/hello.off.log" | head -8 | sed 's/^/      /'
fi

echo "irocache-check: $bad failed"
exit "$bad"
