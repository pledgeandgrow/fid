#!/usr/bin/env bash
# build-host-suite.sh -- `fid build` with no flags over the test suite on a
# Linux or macOS host (docs/roadmap/complaint-roadmap.md CB4).
#
# On such a host `fid build <file>` builds for this machine through the
# machine layer (compiler/nat_ir.fd + nat_enc.fd: no C compiler, no linker)
# and falls back to the C route by itself when the machine layer rejects a
# program (compiler/main.fd cmd_build, host_auto). Every test is built that
# way, run, and its output and exit code compared with `fid run`'s (the
# interpreter, the reference), as scripts/cross-test.sh does for one target.
#
# Results: $OUT/results.txt, one line per test:
#   PASS-NATIVE    built by the machine layer, same output and exit code
#   PASS-FALLBACK  the machine layer rejected it, the C route built it, same
#   FAIL-BUILD     `fid build` failed (where `fid run` gave a result)
#   FAIL-RUN       built, but the output or exit code differs
# and a summary. Exit 0 when nothing failed (REQUIRE_NATIVE=1: a fallback
# counts as a failure too, to measure what the machine layer covers).
#
# Usage: tests/tooling/build-host-suite.sh [test.fd ...]
#   FID=<fid>   default release/fid
#   JOBS=<n>    builds at a time (default 4)
#   OUT=<dir>   default build/host-suite
# On Windows: run it in WSL with a Linux fid
#   (wsl bash tests/tooling/build-host-suite.sh, FID=release/fid).
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
case "$(uname -s 2>/dev/null)" in
  Linux|Darwin) ;;
  *) echo "build-host-suite: needs a Linux or macOS host (on Windows: run it in WSL with a Linux fid)"; exit 2 ;;
esac
FID="${FID:-release/fid}"
case "$FID" in /*) ;; *) FID="$ROOT/$FID" ;; esac
if [ ! -x "$FID" ]; then echo "build-host-suite: no compiler at $FID (FID=<fid>)"; exit 2; fi
export FID_HOME="${FID_HOME:-$ROOT}"
unset FIDES_BUILD_VIA_C FID_NAT_REJECT
JOBS="${JOBS:-4}"
OUT="${OUT:-build/host-suite}"
mkdir -p "$OUT"

# The tests: as scripts/cross-test.sh (external libraries, hardware, bare
# machines and expected compile failures left out; run-time panics kept)
skip_re='^(test_comptime_basic|test_comptime_func|test_comptime_reflect|test_comptime_serialize|test_comptime_gen|test_http|test_json|test_tensor|test_autodiff|test_kv_cache|test_quantization|test_checkpoint|test_shard|test_lora|test_simd|test_shape|test_shape_verify|test_wasm|test_allocator|test_effect_handler|test_asm|test_asm_pe|test_bare.*|test_boot|test_interrupt|test_mmu|test_gat|test_ffi_header|test_dlopen|test_game_stdlib|test_kernel_serial|test_serial_hello|test_os_gaps|test_phase8b|test_vec_math|test_sqlite|test_pe_.*|test_parse_header_sqlite|test_lib_(sdl|gl|font|image|audio|audio_mixer|shader|mesh)|test_elf_native|test_dll_.*)\.fd$'
if [ $# -gt 0 ]; then
  TESTS=("$@")
else
  TESTS=()
  for f in tests/bootstrap/*.fd; do
    b=$(basename "$f")
    case "$b" in test_panic_*_fail.fd) TESTS+=("$f"); continue ;; esac
    case "$b" in *_fail.fd|*_fail2.fd) continue ;; esac
    if [[ "$b" =~ $skip_re ]]; then continue; fi
    if head -3 "$f" | grep -q 'expect-error'; then continue; fi
    TESTS+=("$f")
  done
fi
echo "build-host-suite: ${#TESTS[@]} tests, $JOBS at a time, output in $OUT"

one() {
  local f="$1" b; b=$(basename "$f" .fd)
  local d="$OUT/$b"
  timeout 120 "$FID" run "$f" < /dev/null > "$d.expected" 2> /dev/null
  local erc=$?
  timeout 600 "$FID" build "$f" -o "$d.bin" > "$d.build.log" 2>&1
  local brc=$?
  local how=NATIVE
  if grep -q 'falling back to the C compiler' "$d.build.log"; then how=FALLBACK; fi
  if [ $brc -ne 0 ] || [ ! -x "$d.bin" ]; then
    echo "FAIL-BUILD $b" > "$d.result"; return
  fi
  timeout 60 "$d.bin" < /dev/null > "$d.got" 2> /dev/null
  local grc=$?
  if [ $grc -eq $erc ] && cmp -s "$d.expected" "$d.got"; then
    echo "PASS-$how $b" > "$d.result"
  else
    echo "FAIL-RUN $b (exit $grc, fid run $erc; $how)" > "$d.result"
  fi
}
export -f one
export FID OUT
printf '%s\n' "${TESTS[@]}" | xargs -P "$JOBS" -I{} bash -c 'one "$@"' _ {}

: > "$OUT/results.txt"
for f in "${TESTS[@]}"; do
  b=$(basename "$f" .fd)
  cat "$OUT/$b.result" 2>/dev/null >> "$OUT/results.txt" || echo "FAIL-BUILD $b (no result)" >> "$OUT/results.txt"
done
n_nat=$(grep -c '^PASS-NATIVE' "$OUT/results.txt")
n_fb=$(grep -c '^PASS-FALLBACK' "$OUT/results.txt")
n_fb_fail=$(grep -c '^FAIL.*FALLBACK' "$OUT/results.txt")
n_fail=$(grep -c '^FAIL' "$OUT/results.txt")
echo "  machine layer: $n_nat passed"
echo "  fallback to C: $n_fb passed (+ $n_fb_fail failed after falling back)"
echo "  failed:        $n_fail"
grep '^FAIL' "$OUT/results.txt" | head -40 | sed 's/^/    /'
if [ "$n_fb" -gt 0 ]; then
  echo "  fell back:"; grep '^PASS-FALLBACK' "$OUT/results.txt" | head -40 | sed 's/^/    /'
fi
status=0
[ "$n_fail" -gt 0 ] && status=1
[ "${REQUIRE_NATIVE:-}" = 1 ] && [ "$n_fb" -gt 0 ] && status=1
if [ $status = 0 ]; then echo "build-host-suite: PASS"; else echo "build-host-suite: FAIL"; fi
exit $status
