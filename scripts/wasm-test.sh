#!/bin/bash
# wasm-test.sh — run tests through the native WebAssembly backend (release
# F9): each test is built with `fid wasm` (compiler/wasm.fd, no C compiler),
# run under Node's WASI (scripts/wasm-run.mjs), and its output, exit code
# and panic message compared with the interpreter's (`fid run`), as
# scripts/cross-test.sh does for the other targets.
#
# Usage:
#   scripts/wasm-test.sh [test.fd ...]
#     no files: tests/bootstrap/test_wasm_*.fd and the F6 integer tests
#     ALL=1:    the suite's normal tests and run-time panic tests (the
#               selection of scripts/cross-test.sh)
#   FID=<path to fid>   default release/fid.exe (release/fid off Windows)
#   JOBS=<n>            parallel builds, default 4
#   OUT=<dir>           default build/wasm-native
#   REUSE=1             keep the results of tests already run in $OUT
#   LANE_SKIPS=<file>   default scripts/lane-skips.txt (/dev/null: none)
#   WASM_LANE=<name>    the lane name its patterns are matched against,
#                       default wasm32-wasi-native (so `wasm32-wasi*`
#                       entries hold for this lane and the C path's)
#
# Results: $OUT/results.txt (PASS/FAIL/BUILD/SKIP per test) and a summary.
#
# XT28: what a test cannot do on WebAssembly by its nature (threads that
# share memory, sockets, processes, C libraries, an x86-64-only main) is
# not built and is listed as "SKIP  <test> (<reason>)", from
#   - scripts/lane-skips.txt: "<lane pattern> <test> <reason>" (the list of
#     scripts/cross-test.sh), and
#   - the test's own `//@ backends:` line (scripts/verify/expect.py): a
#     test that names its backends and not `wasm` is skipped here.
# When the interpreter gives no reference (it timed out, cannot call C,
# lacks a runtime function, or hit its own memory or recursion limit), the
# test's `// Expected output:` block is the reference, as in
# scripts/cross-test.sh; with no block it is a SKIP.
set -u
cd "$(dirname "$0")/.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; *) HOST=other ;; esac
if [ -z "${FID:-}" ]; then
  if [ "$HOST" = windows ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
JOBS="${JOBS:-4}"
OUT="${OUT:-build/wasm-native}"
LANE_SKIPS="${LANE_SKIPS:-scripts/lane-skips.txt}"
WASM_LANE="${WASM_LANE:-wasm32-wasi-native}"
mkdir -p "$OUT"

skip_re='^(test_comptime_basic|test_comptime_func|test_comptime_reflect|test_comptime_serialize|test_comptime_gen|test_http|test_json|test_tensor|test_autodiff|test_kv_cache|test_quantization|test_checkpoint|test_shard|test_lora|test_simd|test_shape|test_shape_verify|test_wasm|test_allocator|test_effect_handler|test_asm|test_asm_pe|test_bare.*|test_boot|test_interrupt|test_mmu|test_gat|test_ffi_header|test_dlopen|test_game_stdlib|test_kernel_serial|test_serial_hello|test_os_gaps|test_phase8b|test_vec_math|test_sqlite|test_pe_.*|test_parse_header_sqlite|test_lib_(sdl|gl|font|image|audio|audio_mixer|shader|mesh)|test_elf_native|test_dll_.*)\.fd$'
TESTS=()
if [ $# -gt 0 ]; then
  TESTS=("$@")
elif [ "${ALL:-}" = 1 ]; then
  # the files with `expect-error` in their first 3 lines, in one pass
  experr=$'\n'$(awk 'FNR <= 3 && /expect-error/ { print FILENAME; nextfile } FNR > 3 { nextfile }' tests/bootstrap/*.fd)$'\n'
  for f in tests/bootstrap/*.fd; do
    b="${f##*/}"
    case "$b" in test_panic_*_fail.fd) TESTS+=("$f"); continue ;; esac
    case "$b" in *_fail.fd|*_fail2.fd) continue ;; esac
    if [[ "$b" =~ $skip_re ]]; then
      case "$b" in test_wasm_*) ;; *) continue ;; esac
    fi
    if [[ "$experr" == *$'\n'"$f"$'\n'* ]]; then continue; fi
    TESTS+=("$f")
  done
else
  for f in tests/bootstrap/test_wasm_*.fd; do [ -f "$f" ] && TESTS+=("$f"); done
  for b in test_int_semantics test_panic_overflow_fail test_panic_div_zero_fail test_panic_min_div_fail; do
    TESTS+=("tests/bootstrap/$b.fd")
  done
fi

# The tests left out on this lane, "<test>|<reason>" lines
LSKIP=$'\n'
if [ -f "$LANE_SKIPS" ]; then
  while read -r spat stest swhy; do
    case "$spat" in ''|'#'*) continue ;; esac
    stest="${stest%$'\r'}"; swhy="${swhy%$'\r'}"   # a CRLF checkout
    sneg=0
    case "$spat" in '!'*) sneg=1; spat="${spat#!}" ;; esac
    smatch=0
    # shellcheck disable=SC2254
    case "$WASM_LANE" in $spat) smatch=1 ;; esac
    [ "$smatch" != "$sneg" ] && LSKIP="$LSKIP$stest|${swhy:-skipped on $WASM_LANE}"$'\n'
  done < "$LANE_SKIPS"
fi
# A `//@ backends:` line without `wasm` (one awk over all the files)
if [ ${#TESTS[@]} -gt 0 ]; then
  while IFS='|' read -r bf bl; do
    [ -n "$bf" ] || continue
    bb="${bf##*/}"; bb="${bb%.fd}"
    case ",$bl," in *,wasm,*) ;; *) LSKIP="$LSKIP$bb|its //@ backends: line is $bl"$'\n' ;; esac
  done < <(awk '{ sub(/\r$/, "") } /^\/\/@ backends:/ { v = $0; sub(/^\/\/@ backends: */, "", v); gsub(/[ \t]/, "", v); print FILENAME "|" v; nextfile }' "${TESTS[@]}" 2>/dev/null)
fi
ALL_TESTS=("${TESTS[@]+"${TESTS[@]}"}")
TESTS=()
lskip=0
for f in "${ALL_TESTS[@]+"${ALL_TESTS[@]}"}"; do
  b="${f##*/}"; b="${b%.fd}"
  if [[ "$LSKIP" == *$'\n'"$b|"* ]]; then lskip=$((lskip + 1)); else TESTS+=("$f"); fi
done
echo "fid wasm: ${#TESTS[@]} tests, $JOBS at a time, output in $OUT$([ "$lskip" = 0 ] || echo " ($lskip skipped on $WASM_LANE)")"

one() {
  local f="$1" b; b=$(basename "$f" .fd)
  if [ "${REUSE:-}" = 1 ] && [ -f "$OUT/$b.actual_rc" ]; then return; fi
  # the test's `//@ env: NAME=value` lines (scripts/verify/expect.py), set
  # for the reference run, the build and the run under Node
  local ev=() l
  while IFS= read -r l; do [ -n "$l" ] && ev+=("$l"); done < <(tr -d '\r' < "$f" | sed -n 's#^//@ env: *##p')
  timeout 120 env "${ev[@]+"${ev[@]}"}" "$FID" run "$f" < /dev/null > "$OUT/$b.expected" 2> "$OUT/$b.expected_err"
  echo $? > "$OUT/$b.expected_rc"
  timeout 600 env "${ev[@]+"${ev[@]}"}" "$FID" wasm "$f" -o "$OUT/$b.wasm" > "$OUT/$b.build.log" 2>&1
  local rc=$?
  echo $rc > "$OUT/$b.build_rc"
  # TG16: a module the engine does not validate counts as BUILD
  if [ $rc = 0 ] && ! node tests/wasm/validate.mjs "$OUT/$b.wasm" > "$OUT/$b.valid.log" 2>&1; then
    cat "$OUT/$b.valid.log" >> "$OUT/$b.build.log"
    echo 1 > "$OUT/$b.build_rc"
    rc=1
  fi
  if [ $rc = 0 ]; then
    timeout 60 env "${ev[@]+"${ev[@]}"}" node scripts/wasm-run.mjs "$OUT/$b.wasm" < /dev/null > "$OUT/$b.actual" 2> "$OUT/$b.stderr"
    echo $? > "$OUT/$b.actual_rc"
  fi
}
export -f one
export FID OUT REUSE
if [ ${#TESTS[@]} -gt 0 ]; then
  printf '%s\n' "${TESTS[@]}" | xargs -P "$JOBS" -I{} bash -c 'one "$@"' _ {}
fi

# The panic's message without its details (the interpreter also prints
# the operands, and " at line N", which compiled code does not)
panic_key() { grep -a -m1 '^Panic:' "$1" 2>/dev/null | tr -d '\r' | sed -E 's/^(Panic: [^:]*).*/\1/; s/ at line [0-9]+$//; s/ at 0x[0-9A-Fa-f]+$//'; }
# What the interpreter cannot do, so that its run is no reference: call C
# (compiler/interp.fd "FFI function ... is not available in the
# interpreter", a C function pointer), a builtin it does not have, its own
# memory limit and its recursion limit (compiled code has neither), and
# the fid process itself out of memory (its C runtime's __p_oom, "out of
# memory (allocating N bytes)": the host's memory, not the program's)
interp_no_ref() {
  grep -aq -e 'is not available in the interpreter' \
           -e '^Panic: a C function pointer .* can only be called in a C build' \
           -e '^Panic: Unknown runtime function' \
           -e 'the interpreter holds at most' \
           -e '^Panic: out of memory (allocating [0-9]* bytes)' \
           -e '^Panic: Recursion depth limit ([0-9]*) exceeded' "$1" 2>/dev/null
}
# The test's `// Expected output:` block (scripts/cross-test.sh): each `// `
# stripped, a line ending in `...` matching any line that starts with the
# rest, `// Expected exit: N` (default 0), lines compared without their
# leading and trailing blanks
block_of() {
  awk '
    { sub(/\r$/, "") }
    /^\/\/ Expected output:/ { on = 1; next }
    on && /^\/\/ Expected exit:/ { next }
    on && /^\/\// { sub(/^\/\/ ?/, ""); sub(/^[ \t]+/, ""); sub(/[ \t]+$/, ""); print; next }
    on { exit }
  ' "$1"
}
has_block() { grep -q '^// Expected output:' "$1" 2>/dev/null; }
block_exit_of() {
  local e
  e=$(tr -d '\r' < "$1" | sed -n 's#^// Expected exit: *\([0-9][0-9]*\).*#\1#p' | head -1)
  echo "${e:-0}"
}
same_as_block() {   # $1 = actual output, $2 = expected lines (block_of)
  tr -d '\r' < "$1" 2>/dev/null | awk -v exp_file="$2" '
    BEGIN { n = 0; while ((getline l < exp_file) > 0) { n++; e[n] = l } }
    { sub(/^[ \t]+/, ""); sub(/[ \t]+$/, ""); m++; a[m] = $0 }
    END {
      if (m != n) exit 1
      for (i = 1; i <= n; i++) {
        if (e[i] ~ /\.\.\.$/) {
          p = substr(e[i], 1, length(e[i]) - 3)
          if (substr(a[i], 1, length(p)) != p) exit 1
        } else if (a[i] != e[i]) exit 1
      }
    }
  '
}
pass=0; fail=0; berr=0; skip=0
: > "$OUT/results.txt"
printf '%s' "$LSKIP" | while IFS='|' read -r sb swhy; do
  [ -n "$sb" ] || continue
  for f in "${ALL_TESTS[@]+"${ALL_TESTS[@]}"}"; do
    case "$f" in */"$sb.fd"|"$sb.fd") echo "SKIP  $sb ($swhy)" >> "$OUT/results.txt"; break ;; esac
  done
done
for f in "${TESTS[@]+"${TESTS[@]}"}"; do
  b=$(basename "$f" .fd)
  erc=$(cat "$OUT/$b.expected_rc" 2>/dev/null)
  if grep -aq '^Compilation error\|^Compilation errors\|^error: ' "$OUT/$b.expected" "$OUT/$b.expected_err" 2>/dev/null; then
    echo "SKIP  $b (fid run gives no reference: a compile error)" >> "$OUT/results.txt"; skip=$((skip+1)); continue
  fi
  byblock=0; whyblock=""
  if [ "$erc" = 124 ]; then
    whyblock="fid run timed out"
  elif interp_no_ref "$OUT/$b.expected_err"; then
    whyblock="fid run: $(grep -a -m1 '^Panic:' "$OUT/$b.expected_err" | tr -d '\r' | cut -c8-80)"
  fi
  if [ -n "$whyblock" ]; then
    if ! has_block "$f"; then
      echo "SKIP  $b (no reference: $whyblock, and the test has no Expected output)" >> "$OUT/results.txt"; skip=$((skip+1)); continue
    fi
    byblock=1
  fi
  if [ "$(cat "$OUT/$b.build_rc" 2>/dev/null)" != 0 ]; then
    echo "BUILD $b: $(grep -av '^\[build\]\|^\[prof\]' "$OUT/$b.build.log" 2>/dev/null | grep -a -m1 -i 'error\|not available\|panic\|INVALID' | cut -c1-160)" >> "$OUT/results.txt"
    berr=$((berr+1)); continue
  fi
  # PL4: what WebAssembly has no way to do (processes) is a clear panic
  if grep -aq '^Panic: not available in WebAssembly' "$OUT/$b.stderr" 2>/dev/null; then
    echo "SKIP  $b ($(grep -a -m1 '^Panic: not available in WebAssembly' "$OUT/$b.stderr" | cut -c8-120))" >> "$OUT/results.txt"; skip=$((skip+1)); continue
  fi
  arc=$(cat "$OUT/$b.actual_rc" 2>/dev/null)
  if [ "$byblock" = 1 ]; then
    block_of "$f" > "$OUT/$b.expected_block"
    xrc=$(block_exit_of "$f")
    if [ "$arc" = "$xrc" ] && same_as_block "$OUT/$b.actual" "$OUT/$b.expected_block"; then
      echo "PASS  $b" >> "$OUT/results.txt"; pass=$((pass+1))
    else
      echo "FAIL  $b (rc $arc, its Expected output: rc $xrc; $whyblock)" >> "$OUT/results.txt"; fail=$((fail+1))
    fi
    continue
  fi
  if [ "$arc" = "$erc" ] && [ "$(panic_key "$OUT/$b.expected_err")" = "$(panic_key "$OUT/$b.stderr")" ] \
     && cmp -s <(tr -d '\r' < "$OUT/$b.expected") <(tr -d '\r' < "$OUT/$b.actual" 2>/dev/null); then
    echo "PASS  $b" >> "$OUT/results.txt"; pass=$((pass+1))
  else
    echo "FAIL  $b (rc $arc, interpreter $erc)" >> "$OUT/results.txt"; fail=$((fail+1))
  fi
done
skip=$((skip + lskip))
grep -v '^PASS' "$OUT/results.txt" | head -60
echo "fid wasm: $pass passed, $fail failed, $berr did not build, $skip skipped (of ${#ALL_TESTS[@]})"
[ $fail = 0 ] && [ $berr = 0 ]
