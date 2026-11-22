#!/bin/bash
# mecache-check.sh — CS1 (competitive roadmap): the mid-end's per-function
# memo (compiler/ir_ser.fd mec_*, used by compiler/rules.fd rules_run and
# compiler/egraph.fd egraph_run). A build records, per function, what the
# rules and the e-graph changed, keyed by the function's content hash, the
# run's switches and the callees' effect masks; the next build replays it
# and skips the passes. The results must be exactly those of the passes.
#
# Usage: tests/tooling/mecache-check.sh
#   FID=<compiler>   default release/fid.exe (release/fid off Windows)
#   TARGET=<flags>   default `--nat` on Windows, `--target x86_64-linux`
#                    elsewhere (the machine layer, where rules and e-graph run)
#   OUT=<dir>        default build/mecache
#   CC=<c compiler>  for checks 1-2 (C backend), default "zig cc" (cc without zig)
#   TIME=1           also print the --time rows of rules and egraph, cold,
#                    warm and memo off, for hello (the CS1 measurement)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. tests/bootstrap/test_midend_memo.fd (ccompile): its expected lines
#     (record, file, reload, replay, refusal of a stale entry, a damaged
#     file ignored, FID_MECACHE=off).
#  2. tests/tooling/mecache_passes.fd (ccompile): the real rules_run and
#     egraph_run on a module built by hand, cold then warm then memo off:
#     its expected lines, and the [mecache] lines show 0 hits then 0 misses.
#  3. hello, cold (an empty memo file): "[mecache] rules: 0 hits, N misses,
#     R recorded" and the same for egraph, N > 0 and R > 0; the file exists.
#  4. hello, warm: both passes 0 misses; the file is not written again.
#  5. hello and three programs the passes change: the executables of the
#     cold, warm and FID_MECACHE=off builds are the same bytes (when two
#     builds with the memo off are), print the same (and, where the test
#     has them, its expected lines), and the "[prof] native mid-end" counts
#     (rewritten, merged, forwarded) are the same.
#  6. FID_EGRAPH_OFF=all on the warm file: the e-graph misses (the switches
#     are in the key) and the build equals FID_EGRAPH_OFF=all memo off.
#  7. A damaged memo file (bytes flipped; then cut short): the build works
#     and gives the same executable.
#  8. FID_MECACHE=off and FIDES_NO_CACHE=1: no [mecache] line, no file.
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
if [ $WIN = 1 ]; then
  FID="${FID:-release/fid.exe}"; TARGET="${TARGET:---nat}"; EXE=.exe; RUNX=""
else
  FID="${FID:-release/fid}"; TARGET="${TARGET:---target x86_64-linux}"; EXE=""; RUNX=""
fi
OUT="${OUT:-build/mecache}"
if [ -n "${CC:-}" ]; then CCX="$CC"; elif command -v zig > /dev/null 2>&1; then CCX="zig cc"; else CCX=cc; fi
mkdir -p "$OUT"
OUT_ABS="$(cd "$OUT" && pwd)"
MEC="$OUT_ABS/memo.bin"
export FID_MECACHE_STATS=1
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }
want() {
  awk '/^\/\/ Expected output/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on && /^\/\/$/ {print ""; next} on {exit}' "$1"
}

# 1, 2: the unit programs, through the C backend
unit() {  # unit <label> <file>
  local label="$1" src="$2"
  want "$src" > "$OUT/$label.want"
  rm -f "$OUT/$label$EXE"
  if ! CC="$CCX" "$FID" ccompile "$src" -o "$OUT/$label$EXE" > "$OUT/$label.build.log" 2>&1 || [ ! -f "$OUT/$label$EXE" ]; then
    fail "$label: ccompile failed (see $OUT/$label.build.log)"
    return 1
  fi
  "$OUT/$label$EXE" > "$OUT/$label.out" 2> "$OUT/$label.err"
  if cmp -s <(tr -d '\r' < "$OUT/$label.out") "$OUT/$label.want"; then
    pass "$label: expected lines"
  else
    fail "$label: output differs (see $OUT/$label.out, want $OUT/$label.want)"
    return 1
  fi
  return 0
}
unit memo_unit tests/bootstrap/test_midend_memo.fd
if unit memo_passes tests/tooling/mecache_passes.fd; then
  e="$OUT/memo_passes.err"
  if grep -q '^\[mecache\] rules: 0 hits, [1-9][0-9]* misses' "$e" && grep -q '^\[mecache\] rules: [1-9][0-9]* hits, 0 misses' "$e" \
     && grep -q '^\[mecache\] egraph: 0 hits, [1-9][0-9]* misses' "$e" && grep -q '^\[mecache\] egraph: [1-9][0-9]* hits, 0 misses' "$e"; then
    pass "memo_passes: cold misses, warm hits every function"
  else
    fail "memo_passes: [mecache] lines (see $e)"
  fi
fi

# 3-8: real builds
build() {  # build <label> <file> [VAR=value ...]: emit; 0 if it built
  local label="$1" src="$2"; shift 2
  rm -f "$OUT/$label$EXE"
  # shellcheck disable=SC2086
  env "$@" "$FID" emit "$src" $TARGET -o "$OUT/$label$EXE" > "$OUT/$label.log" 2>&1
  [ -f "$OUT/$label$EXE" ]
}
run_out() {  # run_out <label>: its output, CR removed
  $RUNX "$OUT/$1$EXE" 2>&1 | tr -d '\r'
}
counts() {  # counts <label>: the [prof] native mid-end counts, without the time
  grep '^\[prof\] native mid-end' "$OUT/$1.log" | sed -E 's/^[^,]*, //'
}
line() {  # line <label> <pass>
  grep "^\[mecache\] $2:" "$OUT/$1.log" | tail -n 1
}

H=tests/bootstrap/hello.fd
rm -f "$MEC"
if build h_cold "$H" FID_MECACHE="$MEC"; then
  lr=$(line h_cold rules); le=$(line h_cold egraph)
  if echo "$lr" | grep -qE 'rules: 0 hits, [1-9][0-9]* misses, [1-9][0-9]* recorded' \
     && echo "$le" | grep -qE 'egraph: 0 hits, [1-9][0-9]* misses, [1-9][0-9]* recorded' && [ -f "$MEC" ]; then
    pass "hello cold: $lr; $le"
  else
    fail "hello cold: '$lr' '$le' file $( [ -f "$MEC" ] && echo present || echo missing) (see $OUT/h_cold.log)"
  fi
else
  fail "hello cold build (see $OUT/h_cold.log)"
fi
cp "$MEC" "$OUT/memo.after_cold" 2>/dev/null
if build h_warm "$H" FID_MECACHE="$MEC"; then
  lr=$(line h_warm rules); le=$(line h_warm egraph)
  if echo "$lr" | grep -qE 'rules: [1-9][0-9]* hits, 0 misses' && echo "$le" | grep -qE 'egraph: [1-9][0-9]* hits, 0 misses' \
     && cmp -s "$MEC" "$OUT/memo.after_cold"; then
    pass "hello warm: $lr; $le; file not rewritten"
  else
    fail "hello warm: '$lr' '$le' (file rewritten: $(cmp -s "$MEC" "$OUT/memo.after_cold" && echo no || echo yes); see $OUT/h_warm.log)"
  fi
else
  fail "hello warm build (see $OUT/h_warm.log)"
fi

# 5
same_three() {  # same_three <name> <file>
  local name="$1" src="$2"
  build "${name}_off" "$src" FID_MECACHE=off || { fail "$name: build with the memo off (see $OUT/${name}_off.log)"; return; }
  build "${name}_off2" "$src" FID_MECACHE=off || { fail "$name: second build with the memo off"; return; }
  # each program's cold starts from an empty memo (the file is shared
  # across this check's programs; without this, a later program's cold
  # already hits and the unrecordable count below has nothing to read)
  rm -f "$MEC"
  build "${name}_cold" "$src" FID_MECACHE="$MEC" || { fail "$name: cold build (see $OUT/${name}_cold.log)"; return; }
  build "${name}_warm" "$src" FID_MECACHE="$MEC" || { fail "$name: warm build (see $OUT/${name}_warm.log)"; return; }
  local o_off o_cold o_warm
  o_off=$(run_out "${name}_off"); o_cold=$(run_out "${name}_cold"); o_warm=$(run_out "${name}_warm")
  if [ "$o_off" != "$o_cold" ] || [ "$o_off" != "$o_warm" ]; then
    fail "$name: the outputs differ (memo off / cold / warm)"
    return
  fi
  want "$src" > "$OUT/$name.want"
  if [ -s "$OUT/$name.want" ] && [ "$o_off" != "$(cat "$OUT/$name.want")" ]; then
    fail "$name: output is not the test's expected lines (see $OUT/$name.want)"
    return
  fi
  if [ "$(counts "${name}_off")" != "$(counts "${name}_cold")" ] || [ "$(counts "${name}_off")" != "$(counts "${name}_warm")" ]; then
    fail "$name: mid-end counts differ: off '$(counts "${name}_off")' cold '$(counts "${name}_cold")' warm '$(counts "${name}_warm")'"
    return
  fi
  # warm must hit, and may miss only what cold could not record (a
  # string constant a delta cannot serialize without interning it) — the
  # same functions, deterministically, recomputed every time
  w_miss=$(sed -n 's/^\[mecache\] egraph: [1-9][0-9]* hits, \([0-9]*\) misses.*/\1/p' "$OUT/${name}_warm.log" | head -1)
  # the cold build of this check may already hit (hello's entries were
  # recorded by check 4): unrecordable = misses - recorded, 0 when warm
  c_line=$(grep '^\[mecache\] egraph:' "$OUT/${name}_cold.log" | head -1)
  case "$c_line" in
    *', 0 hits,'*) c_unrec=$(printf '%s' "$c_line" | sed 's/^.*: 0 hits, \([0-9]*\) misses, \([0-9]*\) recorded.*/\1 \2/' | awk '{print $1 - $2}') ;;
    *) c_unrec=0 ;;
  esac
  if [ -z "$w_miss" ] || [ -z "$c_unrec" ] || [ "$w_miss" != "$c_unrec" ]; then
    fail "$name: the warm build missed ($(line "${name}_warm" egraph); cold left $c_unrec unrecorded)"
    return
  fi
  if cmp -s "$OUT/${name}_off$EXE" "$OUT/${name}_off2$EXE"; then
    if cmp -s "$OUT/${name}_off$EXE" "$OUT/${name}_cold$EXE" && cmp -s "$OUT/${name}_off$EXE" "$OUT/${name}_warm$EXE"; then
      pass "$name: off, cold and warm are the same bytes; counts $(counts "${name}_warm")"
    else
      fail "$name: the executables differ (memo off / cold / warm) though two memo-off builds are the same"
    fi
  else
    pass "$name: same output and counts (two memo-off builds differ in bytes, so bytes are not compared)"
  fi
}
same_three hello "$H"
same_three families tests/bootstrap/test_egraph_rule_families.fd
same_three unary tests/bootstrap/test_midend_egraph_unary.fd
same_three rulesrand tests/bootstrap/test_machine_rules_random.fd

# 6
T6=tests/bootstrap/test_egraph_rule_families.fd
if build eg_all_warm "$T6" FID_MECACHE="$MEC" FID_EGRAPH_OFF=all && build eg_all_off "$T6" FID_MECACHE=off FID_EGRAPH_OFF=all; then
  if grep -qE '^\[mecache\] egraph: [0-9]+ hits, [1-9][0-9]* misses' "$OUT/eg_all_warm.log" \
     && [ "$(run_out eg_all_warm)" = "$(run_out eg_all_off)" ] && [ "$(counts eg_all_warm)" = "$(counts eg_all_off)" ]; then
    pass "FID_EGRAPH_OFF=all misses on a warm memo ($(line eg_all_warm egraph)) and builds as with the memo off"
  else
    fail "FID_EGRAPH_OFF=all: '$(line eg_all_warm egraph)', counts '$(counts eg_all_warm)' vs '$(counts eg_all_off)'"
  fi
else
  fail "FID_EGRAPH_OFF=all builds (see $OUT/eg_all_warm.log, $OUT/eg_all_off.log)"
fi

# 7
if [ -f "$MEC" ]; then
  PY=python3; command -v python3 > /dev/null 2>&1 && python3 -c 1 > /dev/null 2>&1 || PY=python
  "$PY" - "$MEC" <<'EOF'
import sys
p = sys.argv[1]
b = bytearray(open(p, 'rb').read())
for i in range(40, len(b), 997):
    b[i] ^= 0x5A
open(p, 'wb').write(bytes(b))
EOF
  ok=1
  build h_damaged "$H" FID_MECACHE="$MEC" || ok=0
  sz=$(wc -c < "$MEC"); head -c $((sz / 2)) "$MEC" > "$MEC.cut" && mv "$MEC.cut" "$MEC"
  build h_cut "$H" FID_MECACHE="$MEC" || ok=0
  # counts compare only when the damaged file loaded nothing: a file the
  # loader stops part-way through (check words) legitimately serves some
  # entries, and a replayed function does not recount its rewrites
  part_d=$(grep -c '^\[mecache\] egraph: [1-9][0-9]* hits' "$OUT/h_damaged.log" 2>/dev/null)
  part_c=$(grep -c '^\[mecache\] egraph: [1-9][0-9]* hits' "$OUT/h_cut.log" 2>/dev/null)
  if [ "${part_d:-0}" -gt 0 ] || [ "${part_c:-0}" -gt 0 ]; then counts_d=$(counts hello_off); counts_c=$(counts hello_off); else counts_d=$(counts h_damaged); counts_c=$(counts h_cut); fi
  if [ $ok = 1 ] && [ "$(run_out h_damaged)" = "$(run_out hello_off)" ] && [ "$(run_out h_cut)" = "$(run_out hello_off)" ] \
     && [ "$counts_d" = "$(counts hello_off)" ] && [ "$counts_c" = "$(counts hello_off)" ]; then
    pass "a damaged or cut memo file is ignored where it does not check (a partial load is allowed; its output matches)"
  else
    fail "damaged memo file (see $OUT/h_damaged.log, $OUT/h_cut.log)"
  fi
fi

# 8
rm -f "$OUT/none.bin"
if build h_moff "$H" FID_MECACHE=off && build h_nocache "$H" FIDES_NO_CACHE=1 FID_MECACHE=; then
  if ! grep -q '^\[mecache\]' "$OUT/h_moff.log" && ! grep -q '^\[mecache\]' "$OUT/h_nocache.log"; then
    pass "FID_MECACHE=off and FIDES_NO_CACHE=1 turn the memo off"
  else
    fail "the memo ran with FID_MECACHE=off or FIDES_NO_CACHE=1"
  fi
else
  fail "builds with the memo off"
fi

# TIME=1: the CS1 numbers (on a quiet machine, a compiler built at -O2)
if [ "${TIME:-0}" = 1 ]; then
  rm -f "$MEC"
  for k in off cold warm warm2; do
    v="$MEC"; [ $k = off ] && v=off
    # shellcheck disable=SC2086
    FID_MECACHE="$v" "$FID" emit "$H" $TARGET -o "$OUT/t_$k$EXE" --time > "$OUT/t_$k.log" 2>&1
    echo "TIME $k: $(grep -E '^\[time\] (lower|iropt|rules|egraph|native code|total):' "$OUT/t_$k.log" | sed 's/\[time\] //' | tr '\n' ' ')"
    grep '^\[prof\] native mid-end' "$OUT/t_$k.log" | sed "s/^/TIME $k: /"
  done
fi
exit $bad
