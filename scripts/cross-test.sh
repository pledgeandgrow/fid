#!/bin/bash
# cross-test.sh — run the test suite on another target through the C backend
# (release F5, F8): every test is built with `fid ccompile --target <target>`
# (zig cc cross-compiles from any host) and its output compared with the
# interpreter's (`fid run`), the reference.
#
# Usage:
#   scripts/cross-test.sh <target> [test.fd ...]
#     target: aarch64-linux | riscv64-linux | x86_64-linux   (built and run)
#             x86_64-macos | aarch64-macos                    (built only here;
#             run them on a Mac with scripts/cross-run.sh)
#             wasm32-wasi                                     (run with Node,
#             scripts/wasm-run.mjs)
#   FID=<path to fid>   default release/fid.exe (release/fid on Linux)
#   JOBS=<n>            parallel builds, default 8
#   OUT=<dir>           default build/cross/<target> (-native with NATIVE=1)
#   NATIVE=1            build with the native backend (`fid emit --target`,
#                       aarch64-linux and riscv64-linux) instead of C
#
# Running: on Linux natively (x86_64) or with qemu-user (qemu-aarch64,
# qemu-riscv64, Debian/Ubuntu package qemu-user). On Windows the binaries run
# in WSL (`wsl qemu-aarch64 ...`); set WSL_DISTRO to pick a distribution.
# The Linux binaries are static (musl), so no sysroot is needed.
#
# Results: $OUT/results.txt (PASS/FAIL/BUILD/SKIP per test) and a summary.
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT=$(pwd)
TARGET="${1:-}"
if [ -z "$TARGET" ]; then sed -n '2,22p' "$0"; exit 1; fi
shift
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; *) HOST=linux ;; esac
if [ -z "${FID:-}" ]; then
  if [ "$HOST" = windows ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
JOBS="${JOBS:-8}"
OUT="${OUT:-build/cross/$TARGET}"
if [ "${NATIVE:-}" = 1 ]; then OUT="${OUT}-native"; fi
EXT=bin
case "$TARGET" in wasm*|wasi) EXT=wasm ;; *-windows*) EXT=exe ;; esac
mkdir -p "$OUT"
export CC="${CC:-zig cc}"

case "$TARGET" in
  aarch64-linux*|arm64-linux*) RUNNER=qemu-aarch64; OS=linux ;;
  riscv64-linux*)              RUNNER=qemu-riscv64; OS=linux ;;
  x86_64-linux*)               RUNNER="";           OS=linux ;;
  x86_64-windows*)             RUNNER="";           OS=windows ;;   # NATIVE=1: emit --nat, run here
  *-macos*)                    RUNNER="";           OS=macos ;;
  wasm32-wasi*|wasm*|wasi)     RUNNER="node scripts/wasm-run.mjs"; OS=wasi ;;
  *) echo "unknown target: $TARGET"; exit 1 ;;
esac

# Tests: the suite's normal tests (expected failures and tests that need
# external libraries, hardware or a bare machine are left out, as in the
# Rust harness), or the files given.
skip_re='^(test_comptime_basic|test_comptime_func|test_comptime_reflect|test_comptime_serialize|test_comptime_gen|test_http|test_json|test_tensor|test_autodiff|test_kv_cache|test_quantization|test_checkpoint|test_shard|test_lora|test_simd|test_shape|test_shape_verify|test_wasm|test_allocator|test_effect_handler|test_asm|test_asm_pe|test_bare.*|test_boot|test_interrupt|test_mmu|test_gat|test_ffi_header|test_dlopen|test_game_stdlib|test_kernel_serial|test_serial_hello|test_os_gaps|test_phase8b|test_vec_math|test_sqlite|test_pe_.*|test_parse_header_sqlite|test_lib_(sdl|gl|font|image|audio|audio_mixer|shader|mesh)|test_elf_native|test_dll_.*)\.fd$'
if [ $# -gt 0 ]; then
  TESTS=("$@")
else
  TESTS=()
  for f in tests/bootstrap/*.fd; do
    b=$(basename "$f")
    # Run-time panic tests (test_panic_*_fail) are compared too: the
    # output before the panic, the exit code and the panic's message
    case "$b" in test_panic_*_fail.fd) TESTS+=("$f"); continue ;; esac
    case "$b" in *_fail.fd|*_fail2.fd) continue ;; esac
    if [[ "$b" =~ $skip_re ]]; then continue; fi
    if head -3 "$f" | grep -q 'expect-error'; then continue; fi
    TESTS+=("$f")
  done
fi
echo "target $TARGET: ${#TESTS[@]} tests, $JOBS builds at a time, output in $OUT"

# 1. The reference output (interpreter) and the cross build of each test
build_one() {
  local f="$1" b; b=$(basename "$f" .fd)
  # REUSE=1: keep what built before (rebuild only failures)
  if [ "${REUSE:-}" = 1 ] && [ "$(cat "$OUT/$b.build_rc" 2>/dev/null)" = 0 ]; then return; fi
  timeout 120 "$FID" run "$f" < /dev/null > "$OUT/$b.expected" 2> "$OUT/$b.expected_err"
  echo $? > "$OUT/$b.expected_rc"
  if [ "${NATIVE:-}" = 1 ]; then
    # The native backend (compiler/nat_ir.fd): no C compiler
    local nat=""
    case "$TARGET" in *-windows*) nat=--nat ;; esac
    timeout 600 "$FID" emit "$f" --target "$TARGET" $nat -o "$OUT/$b.$EXT" > "$OUT/$b.build.log" 2>&1
  else
    timeout 600 "$FID" ccompile "$f" --target "$TARGET" --cc-opt 0 --no-cache -o "$OUT/$b.$EXT" > "$OUT/$b.build.log" 2>&1
  fi
  echo $? > "$OUT/$b.build_rc"
}
export -f build_one
export FID TARGET OUT EXT REUSE NATIVE
# One build first: zig builds the target's libc into its cache on first use,
# and parallel first builds race on that cache (CacheCheckFailed)
build_one "${TESTS[0]}"
printf '%s\n' "${TESTS[@]:1}" | xargs -P "$JOBS" -I{} bash -c 'build_one "$@"' _ {}

# 2. Run every binary that built (a script, so it can run in WSL)
RUNS="$OUT/run-all.sh"
{
  echo '#!/bin/bash'
  # Back from $OUT (relative to the root) to the root
  echo "cd \"\$(dirname \"\$0\")/$(echo "${OUT%/}" | sed -E 's#^\./##; s#[^/]+#..#g')\""
  for f in "${TESTS[@]}"; do
    b=$(basename "$f" .fd)
    [ "$(cat "$OUT/$b.build_rc" 2>/dev/null)" = 0 ] || continue
    echo "timeout 60 $RUNNER $OUT/$b.$EXT < /dev/null > $OUT/$b.actual 2> $OUT/$b.stderr; echo \$? > $OUT/$b.actual_rc"
  done
} > "$RUNS"
chmod +x "$RUNS"
if [ "$OS" = macos ] && [ "$HOST" != macos ]; then
  echo "macOS binaries built; run $RUNS on a Mac (scripts/cross-run.sh), then rerun this script there"
  RAN=0
elif [ "$HOST" = windows ] && [ "$OS" != wasi ] && [ "$OS" != windows ]; then
  # C:/x/y -> /mnt/c/x/y; MSYS must not rewrite the path given to wsl
  WROOT=$(cygpath -m "$ROOT" | sed -E 's#^([A-Za-z]):#/mnt/\L\1#')
  MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} bash "$WROOT/$RUNS"
  RAN=1
else
  bash "$RUNS"
  RAN=1
fi

# 3. Compare
# A panic's message without its details: "Panic: Integer overflow on
# addition" (the interpreter also prints the operands)
panic_key() { grep -a -m1 '^Panic:' "$1" 2>/dev/null | tr -d '\r' | sed -E 's/^(Panic: [^:]*).*/\1/'; }
pass=0; fail=0; berr=0; skip=0
: > "$OUT/results.txt"
for f in "${TESTS[@]}"; do
  b=$(basename "$f" .fd)
  # The reference is the interpreter's output and exit code; a program may
  # return a code of its own (ret42), but a compile error or a timeout
  # leaves no reference
  erc=$(cat "$OUT/$b.expected_rc" 2>/dev/null)
  if [ "$erc" = 124 ] || grep -q '^Compilation error\|^Compilation errors\|^error: ' "$OUT/$b.expected" 2>/dev/null; then
    echo "SKIP  $b (fid run gives no reference: rc $erc)" >> "$OUT/results.txt"; skip=$((skip+1)); continue
  fi
  if [ "$(cat "$OUT/$b.build_rc" 2>/dev/null)" != 0 ]; then
    echo "BUILD $b" >> "$OUT/results.txt"; berr=$((berr+1)); continue
  fi
  [ "$RAN" = 1 ] || continue
  arc=$(cat "$OUT/$b.actual_rc" 2>/dev/null)
  if [ "$arc" = "$erc" ] && [ "$(panic_key "$OUT/$b.expected_err")" = "$(panic_key "$OUT/$b.stderr")" ] \
     && cmp -s <(tr -d '\r' < "$OUT/$b.expected") <(tr -d '\r' < "$OUT/$b.actual" 2>/dev/null); then
    echo "PASS  $b" >> "$OUT/results.txt"; pass=$((pass+1))
  else
    echo "FAIL  $b (rc $arc, interpreter $erc)" >> "$OUT/results.txt"; fail=$((fail+1))
  fi
done
echo "$TARGET: $pass passed, $fail failed, $berr did not build, $skip without reference"
grep -v '^PASS' "$OUT/results.txt" | head -50
[ "$fail" = 0 ] && [ "$berr" = 0 ]
