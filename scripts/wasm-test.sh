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
#
# Results: $OUT/results.txt (PASS/FAIL/BUILD/SKIP per test) and a summary.
set -u
cd "$(dirname "$0")/.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; *) HOST=other ;; esac
if [ -z "${FID:-}" ]; then
  if [ "$HOST" = windows ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
JOBS="${JOBS:-4}"
OUT="${OUT:-build/wasm-native}"
mkdir -p "$OUT"

skip_re='^(test_comptime_basic|test_comptime_func|test_comptime_reflect|test_comptime_serialize|test_comptime_gen|test_http|test_json|test_tensor|test_autodiff|test_kv_cache|test_quantization|test_checkpoint|test_shard|test_lora|test_simd|test_shape|test_shape_verify|test_wasm|test_allocator|test_effect_handler|test_asm|test_asm_pe|test_bare.*|test_boot|test_interrupt|test_mmu|test_gat|test_ffi_header|test_dlopen|test_game_stdlib|test_kernel_serial|test_serial_hello|test_os_gaps|test_phase8b|test_vec_math|test_sqlite|test_pe_.*|test_parse_header_sqlite|test_lib_(sdl|gl|font|image|audio|audio_mixer|shader|mesh)|test_elf_native|test_dll_.*)\.fd$'
TESTS=()
if [ $# -gt 0 ]; then
  TESTS=("$@")
elif [ "${ALL:-}" = 1 ]; then
  for f in tests/bootstrap/*.fd; do
    b=$(basename "$f")
    case "$b" in test_panic_*_fail.fd) TESTS+=("$f"); continue ;; esac
    case "$b" in *_fail.fd|*_fail2.fd) continue ;; esac
    if [[ "$b" =~ $skip_re ]]; then
      case "$b" in test_wasm_*) ;; *) continue ;; esac
    fi
    if head -3 "$f" | grep -q 'expect-error'; then continue; fi
    TESTS+=("$f")
  done
else
  for f in tests/bootstrap/test_wasm_*.fd; do [ -f "$f" ] && TESTS+=("$f"); done
  for b in test_int_semantics test_panic_overflow_fail test_panic_div_zero_fail test_panic_min_div_fail; do
    TESTS+=("tests/bootstrap/$b.fd")
  done
fi
echo "fid wasm: ${#TESTS[@]} tests, $JOBS at a time, output in $OUT"

one() {
  local f="$1" b; b=$(basename "$f" .fd)
  if [ "${REUSE:-}" = 1 ] && [ -f "$OUT/$b.actual_rc" ]; then return; fi
  timeout 120 "$FID" run "$f" < /dev/null > "$OUT/$b.expected" 2> "$OUT/$b.expected_err"
  echo $? > "$OUT/$b.expected_rc"
  timeout 600 "$FID" wasm "$f" -o "$OUT/$b.wasm" > "$OUT/$b.build.log" 2>&1
  local rc=$?
  echo $rc > "$OUT/$b.build_rc"
  # TG16: a module the engine does not validate counts as BUILD
  if [ $rc = 0 ] && ! node tests/wasm/validate.mjs "$OUT/$b.wasm" > "$OUT/$b.valid.log" 2>&1; then
    cat "$OUT/$b.valid.log" >> "$OUT/$b.build.log"
    echo 1 > "$OUT/$b.build_rc"
    rc=1
  fi
  if [ $rc = 0 ]; then
    timeout 60 node scripts/wasm-run.mjs "$OUT/$b.wasm" < /dev/null > "$OUT/$b.actual" 2> "$OUT/$b.stderr"
    echo $? > "$OUT/$b.actual_rc"
  fi
}
export -f one
export FID OUT REUSE
printf '%s\n' "${TESTS[@]}" | xargs -P "$JOBS" -I{} bash -c 'one "$@"' _ {}

# The panic's message without its details (the interpreter also prints
# the operands, and " at line N", which compiled code does not)
panic_key() { grep -a -m1 '^Panic:' "$1" 2>/dev/null | tr -d '\r' | sed -E 's/^(Panic: [^:]*).*/\1/; s/ at line [0-9]+$//'; }
pass=0; fail=0; berr=0; skip=0
: > "$OUT/results.txt"
for f in "${TESTS[@]}"; do
  b=$(basename "$f" .fd)
  erc=$(cat "$OUT/$b.expected_rc" 2>/dev/null)
  if [ "$erc" = 124 ] || grep -q '^Compilation error\|^Compilation errors\|^error: ' "$OUT/$b.expected" 2>/dev/null; then
    echo "SKIP  $b (fid run gives no reference: rc $erc)" >> "$OUT/results.txt"; skip=$((skip+1)); continue
  fi
  if grep -aq 'is not available in the interpreter' "$OUT/$b.expected_err" 2>/dev/null; then
    echo "SKIP  $b (fid run cannot run its FFI)" >> "$OUT/results.txt"; skip=$((skip+1)); continue
  fi
  if [ "$(cat "$OUT/$b.build_rc" 2>/dev/null)" != 0 ]; then
    echo "BUILD $b: $(grep -a -m1 -i 'error\|not available\|panic\|INVALID' "$OUT/$b.build.log" | cut -c1-160)" >> "$OUT/results.txt"
    berr=$((berr+1)); continue
  fi
  # PL4: what WebAssembly has no way to do (processes) is a clear panic
  if grep -aq '^Panic: not available in WebAssembly' "$OUT/$b.stderr" 2>/dev/null; then
    echo "SKIP  $b ($(grep -a -m1 '^Panic: not available in WebAssembly' "$OUT/$b.stderr" | cut -c8-120))" >> "$OUT/results.txt"; skip=$((skip+1)); continue
  fi
  arc=$(cat "$OUT/$b.actual_rc" 2>/dev/null)
  if [ "$arc" = "$erc" ] && [ "$(panic_key "$OUT/$b.expected_err")" = "$(panic_key "$OUT/$b.stderr")" ] \
     && cmp -s <(tr -d '\r' < "$OUT/$b.expected") <(tr -d '\r' < "$OUT/$b.actual" 2>/dev/null); then
    echo "PASS  $b" >> "$OUT/results.txt"; pass=$((pass+1))
  else
    echo "FAIL  $b (rc $arc, interpreter $erc)" >> "$OUT/results.txt"; fail=$((fail+1))
  fi
done
grep -v '^PASS' "$OUT/results.txt" | head -60
echo "fid wasm: $pass passed, $fail failed, $berr did not build, $skip skipped (of ${#TESTS[@]})"
[ $fail = 0 ] && [ $berr = 0 ]
