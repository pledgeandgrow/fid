#!/usr/bin/env bash
# cb3-incremental-check.sh — complaint-roadmap CB3: a one-line change
# rebuilds one function, not the program.
#
# Builds tests/bootstrap/test_cb3_one_edit.fd three times into the same
# output, on the caches the first build leaves beside it: cold, warm (no
# change), and after changing the line marked CB3-EDIT (`n * 3` ->
# `n * 5` in `tri`). Every build reads the same source file name (the line
# table names it, and the function caches are keyed by it).
#
#   pe   a Windows `fid emit` (pe_ir): its per-function cache is on by
#        default (`<output>.fides-fncache.pe`, B5); FID_PE_FNCACHE_STATS=1
#        prints "[pe] fncache: H hits, M misses".
#   nat  `fid emit --nat --stats` (nat_ir; `--target x86_64-linux` off
#        Windows) with FID_FNCACHE=<file> (DEFAULT=1: without it, for when
#        the driver turns the cache on for every emit); --stats prints
#        "[fncache] entries from ..., H hits, C compiled" and
#        "[fncache] re-emitted: f, g".
# Both: the mid-end's memo (compiler/ir_ser.fd imc_*, on by default for
# `emit -o`, `<output>.fides-irocache`) with FID_IROCACHE_STATS=1 prints
# "[irocache] H hits, M misses, ..." and "[irocache] ran: f, g".
#
# Checks, per back end:
#   cold  the program's output; the function cache compiled everything; the
#         memo missed everything (it is on without FID_IROCACHE)
#   warm  same output; the function cache re-emitted no function of the
#         program and the memo ran its passes on none
#   edit  the edited output (275 first); the function cache re-emitted
#         exactly one function, `tri`; the memo ran the passes on `tri` and
#         on no function that does not call it (fib, gcd, count_words,
#         digits; `main` calls it and may run); the edited program built
#         with no cache at all prints the same
#
# Usage: FID=release/fid.exe bash tests/tooling/cb3-incremental-check.sh
#   BACKENDS="pe nat" (default on Windows; "nat" elsewhere)
#   OUT=build/cb3     scratch directory
# Exit 0 when every check passes.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/cb3}"
DEFAULT="${DEFAULT:-0}"
SRC=tests/bootstrap/test_cb3_one_edit.fd
case "$(uname -s 2>/dev/null)" in
  MINGW*|MSYS*|CYGWIN*) WIN=1 ;;
  *) WIN=0 ;;
esac
if [ "$WIN" = 1 ]; then BACKENDS="${BACKENDS:-pe nat}"; else BACKENDS="${BACKENDS:-nat}"; fi
mkdir -p "$OUT"
bad=0
ok() { echo "OK    $*"; }
fail() { echo "FAIL  $*"; bad=$((bad + 1)); }
expected() {  # <test.fd> -> the lines of its "// Expected output:" block
  awk '/^\/\/ Expected output:/{on=1; next} on && /^\/\/ /{sub(/^\/\/ /, ""); print; next} on{exit}' "$1"
}
run() {
  if [ "$WIN" = 1 ] || [ "$2" = pe ]; then
    timeout 60 "$1" 2>&1 | tr -d '\r'
  else
    timeout 60 "$1" 2>&1
  fi
}
# the names after "<prefix> " on the first line of <log> starting with it,
# one per line
names() {
  grep -m1 "^$2" "$1" | sed "s/^$2 *//" | tr -d '\r' | sed 's/, /\n/g' | sed '/^$/d'
}
# <log> <field>: the number before " <field>" on the first [irocache] line
imc_stat() {
  grep -m1 '^\[irocache\] [0-9]' "$1" | sed -n "s/.* \([0-9][0-9]*\) $2.*/\1/p;s/^\[irocache\] \([0-9][0-9]*\) $2.*/\1/p" | head -1
}
OWN="tri fib gcd count_words digits main"
NOT_CALLERS="fib gcd count_words digits"

expected "$SRC" > "$OUT/expected.txt"
sed 's|n \* 3 + tri(n - 1); // CB3-EDIT|n * 5 + tri(n - 1); // CB3-EDIT|' "$SRC" > "$OUT/edited.fd"
if cmp -s "$SRC" "$OUT/edited.fd"; then
  echo "FAIL  the sed did not change the copy"; exit 1
fi
{ echo 275; sed -n '2,$p' "$OUT/expected.txt"; } > "$OUT/expected.edit.txt"

for be in $BACKENDS; do
  dir="$OUT/$be"
  mkdir -p "$dir"
  W="$dir/cb3_one_edit.fd"
  exe="$dir/prog"
  [ "$WIN" = 1 ] && exe="$exe.exe"
  # cold: no caches from an earlier run of this script
  rm -f "$exe" "$exe.fides-stamp" "$exe.fides-fncache" "$exe.fides-fncache.pe" "$exe.fides-irocache" "$dir/nat.fc"
  flags=()
  envs=(FID_IROCACHE_STATS=1)
  if [ "$be" = pe ]; then
    envs+=(FID_PE_FNCACHE_STATS=1)
  else
    if [ "$WIN" = 1 ]; then flags=(--nat --stats); else flags=(--target x86_64-linux --stats); fi
    [ "$DEFAULT" = 1 ] || envs+=(FID_FNCACHE="$dir/nat.fc")
  fi
  for step in cold warm edit; do
    if [ "$step" = edit ]; then cp "$OUT/edited.fd" "$W"; else cp "$SRC" "$W"; fi
    rm -f "$exe" "$exe.fides-stamp"
    log="$dir/$step.log"
    env -u FID_IROCACHE "${envs[@]}" "$FID" emit "$W" "${flags[@]}" -o "$exe" > "$log" 2>&1
    if [ ! -f "$exe" ]; then
      fail "$be $step: build: $(grep -m1 -i -E 'error|panic' "$log")"; continue
    fi
    want="$OUT/expected.txt"
    [ "$step" = edit ] && want="$OUT/expected.edit.txt"
    run "$exe" "$be" > "$dir/$step.out"
    if cmp -s "$want" "$dir/$step.out"; then ok "$be $step: output"; else fail "$be $step: output"; diff "$want" "$dir/$step.out" | head -6; fi
    # the function cache
    if [ "$be" = pe ]; then
      line=$(grep -m1 '^\[pe\] fncache:' "$log")
      h=$(echo "$line" | sed -n 's/.*: \([0-9]*\) hits.*/\1/p'); m=$(echo "$line" | sed -n 's/.* \([0-9]*\) misses.*/\1/p')
      case "$step" in
        cold) if [ "$h" = 0 ] && [ -n "$m" ] && [ "$m" -gt 0 ]; then ok "$be cold: $line"; COLD_N="$m"; else fail "$be cold: '$line' (expected 0 hits; the cache is on by default)"; COLD_N=""; fi ;;
        warm) if [ "$m" = 0 ] && [ "$h" = "${COLD_N:-x}" ]; then ok "$be warm: $line"; else fail "$be warm: '$line' (expected ${COLD_N:-?} hits, 0 misses)"; fi ;;
        edit) if [ "$m" = 1 ] && [ -n "$h" ] && [ "$h" -gt 0 ]; then ok "$be edit: $line (one function re-emitted)"; else fail "$be edit: '$line' (expected 1 miss)"; fi ;;
      esac
    else
      line=$(grep -m1 '^\[fncache\] entries' "$log")
      re=$(names "$log" '\[fncache\] re-emitted:' | tr '\n' ' ' | sed 's/ $//')
      case "$step" in
        cold) if echo "$line" | grep -q ' 0 hits, [1-9][0-9]* compiled'; then ok "$be cold: $line"; else fail "$be cold: '$line'"; fi ;;
        warm)
          own=""
          for f in $OWN; do case " $re " in *" $f "*) own="$own $f" ;; esac; done
          if [ -n "$line" ] && [ -z "$own" ]; then ok "$be warm: $line (re-emitted: ${re:-none})"; else fail "$be warm: '$line' re-emitted the program's$own"; fi ;;
        edit) if [ "$re" = tri ]; then ok "$be edit: $line, re-emitted: tri"; else fail "$be edit: '$line' re-emitted '$re' (expected exactly tri)"; fi ;;
      esac
    fi
    # the mid-end's memo
    ih=$(imc_stat "$log" hits); im=$(imc_stat "$log" misses)
    ran=$(names "$log" '\[irocache\] ran:' | tr '\n' ' ' | sed 's/ $//')
    case "$step" in
      cold) if [ "$ih" = 0 ] && [ -n "$im" ] && [ "$im" -gt 0 ]; then ok "$be cold: memo on by default, $im misses"; else fail "$be cold: memo '$(grep -m1 '^\[irocache\]' "$log")' (expected 0 hits; on without FID_IROCACHE)"; fi ;;
      warm) if [ "$im" = 0 ] && [ -n "$ih" ] && [ "$ih" -gt 0 ] && [ -z "$ran" ]; then ok "$be warm: memo $ih hits, 0 misses"; else fail "$be warm: memo '$(grep -m1 '^\[irocache\]' "$log")' ran: '$ran'"; fi ;;
      edit)
        extra=""
        for f in $NOT_CALLERS; do case " $ran " in *" $f "*) extra="$extra $f" ;; esac; done
        case " $ran " in
          *" tri "*) if [ -z "$extra" ] && [ -n "$ih" ] && [ "$ih" -gt 0 ]; then ok "$be edit: memo ran: $ran"; else fail "$be edit: memo ran '$ran' (not callers of tri:$extra; hits $ih)"; fi ;;
          *) fail "$be edit: memo ran '$ran' (expected tri)" ;;
        esac ;;
    esac
  done
  # the edited program with no cache at all
  rm -f "$dir/plain" "$dir/plain.exe"
  pexe="$dir/plain"; [ "$WIN" = 1 ] && pexe="$pexe.exe"
  env -u FID_IROCACHE -u FID_FNCACHE FIDES_NO_CACHE=1 FID_IROCACHE_STATS=1 FID_PE_FNCACHE_STATS=1 "$FID" emit "$W" "${flags[@]}" -o "$pexe" > "$dir/plain.log" 2>&1
  if [ -f "$pexe" ] && run "$pexe" "$be" | cmp -s "$OUT/expected.edit.txt" -; then
    ok "$be: the edited program built without caches prints the same"
  else
    fail "$be: the edited program built without caches: $(grep -m1 -i -E 'error|panic' "$dir/plain.log")"
  fi
  if [ -f "$pexe" ] && grep -q '^\[irocache\]\|^\[pe\] fncache\|^\[fncache\] entries' "$dir/plain.log"; then
    fail "$be: FIDES_NO_CACHE=1 still used a cache"
  fi
done
echo "cb3-incremental-check: $bad failed"
[ "$bad" = 0 ]
