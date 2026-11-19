#!/usr/bin/env bash
# MI5 / MI17 (docs/roadmap/llvm-roadmap.md): the gate before the machine
# layer becomes the Windows default. Every program of the suite is built
# twice on Windows, with plain `fid emit` (pe_ir, today's default) and with
# `fid emit --nat` (compiler/nat_ir.fd), both are run, and their output,
# exit code and panic message are compared with each other (and, when they
# differ, with the interpreter's, `fid run`, to tell which one is right).
#
#   FID=release/fid.exe bash tests/nattools/mi5_nat_peir_check.sh
#   FILES="tests/bootstrap/hello.fd ..."   only these programs
#   JOBS=4                                 programs at a time (default 4)
#   OUT=<dir>                              default ${TMPDIR:-/tmp}/mi5_nat_peir
#   REUSE=1                                keep what an earlier run built
#
# Each program ends in one of:
#   SAME     both built, same stdout, exit code and panic message
#   NATFIX   they differ and --nat matches the interpreter (pe_ir is wrong)
#   DIFF     they differ and --nat does not match the interpreter   (blocks)
#   NATBUILD pe_ir built it, --nat did not                          (blocks)
#   PEBUILD  --nat built it, pe_ir did not
#   NOBUILD  neither built it
# Passing (the precondition of MI5: `--nat` is at least as good as pe_ir on
# the whole suite): no DIFF and no NATBUILD; the last line is
#   "mi5_nat_peir_check: N same, F nat-fixes, 0 differ, 0 nat-only build failures, P pe_ir-only build failures, X neither built"
# and the exit code is 0. The programs left out are those scripts/cross-test.sh
# leaves out (expected failures, bare-metal and hardware programs, external
# libraries), except the Windows ones (test_pe_*), which are compared here.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/mi5_nat_peir}"
JOBS="${JOBS:-4}"
mkdir -p "$OUT"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) ;; *) echo "mi5_nat_peir_check: needs a Windows host (pe_ir and --nat build Windows executables)"; exit 2 ;; esac

skip_re='^(test_comptime_basic|test_comptime_func|test_comptime_reflect|test_comptime_serialize|test_comptime_gen|test_http|test_json|test_tensor|test_autodiff|test_kv_cache|test_quantization|test_checkpoint|test_shard|test_lora|test_simd|test_shape|test_shape_verify|test_wasm|test_allocator|test_effect_handler|test_asm|test_bare.*|test_boot|test_interrupt|test_mmu|test_gat|test_ffi_header|test_dlopen|test_game_stdlib|test_kernel_serial|test_serial_hello|test_os_gaps|test_phase8b|test_vec_math|test_sqlite|test_pe_sqlite|test_parse_header_sqlite|test_lib_(sdl|gl|font|image|audio|audio_mixer|shader|mesh)|test_elf_native)\.fd$'
TESTS=()
if [ -n "${FILES:-}" ]; then
  for f in $FILES; do TESTS+=("$f"); done
else
  for f in tests/bootstrap/*.fd; do
    b=$(basename "$f")
    case "$b" in test_panic_*_fail.fd) TESTS+=("$f"); continue ;; esac
    case "$b" in *_fail.fd|*_fail2.fd) continue ;; esac
    if [[ "$b" =~ $skip_re ]]; then continue; fi
    if head -3 "$f" | grep -q 'expect-error'; then continue; fi
    TESTS+=("$f")
  done
fi
echo "mi5_nat_peir_check: ${#TESTS[@]} programs, $JOBS at a time, output in $OUT"

# one <file>: build with pe_ir and --nat, run both and the interpreter
one() {
  local f="$1" b; b=$(basename "$f" .fd)
  if [ "${REUSE:-}" = 1 ] && [ -f "$OUT/$b.done" ]; then return; fi
  timeout 600 "$FID" emit "$f" -o "$OUT/$b.pe.exe" > "$OUT/$b.pe.build" 2>&1
  echo $? > "$OUT/$b.pe.build_rc"
  timeout 600 "$FID" emit "$f" --nat -o "$OUT/$b.nat.exe" > "$OUT/$b.nat.build" 2>&1
  echo $? > "$OUT/$b.nat.build_rc"
  local k
  for k in pe nat; do
    if [ "$(cat "$OUT/$b.$k.build_rc")" = 0 ]; then
      timeout 60 "$OUT/$b.$k.exe" < /dev/null > "$OUT/$b.$k.out" 2> "$OUT/$b.$k.err"
      echo $? > "$OUT/$b.$k.rc"
    fi
  done
  timeout 120 "$FID" run "$f" < /dev/null > "$OUT/$b.run.out" 2> "$OUT/$b.run.err"
  echo $? > "$OUT/$b.run.rc"
  touch "$OUT/$b.done"
}
export -f one
export FID OUT REUSE
printf '%s\n' "${TESTS[@]}" | xargs -P "$JOBS" -I{} bash -c 'one "$@"' _ {}

# The panic's message without its details (the interpreter also prints the
# operands and the source line), as scripts/cross-test.sh compares it
panic_key() { grep -a -m1 '^Panic:' "$1" 2>/dev/null | tr -d '\r' | sed -E 's/^(Panic: [^:]*).*/\1/; s/ at line [0-9]+$//'; }
# same <b> <kind1> <kind2>: same exit code, panic message and stdout
same() {
  [ "$(cat "$OUT/$1.$2.rc" 2>/dev/null)" = "$(cat "$OUT/$1.$3.rc" 2>/dev/null)" ] \
    && [ "$(panic_key "$OUT/$1.$2.err")" = "$(panic_key "$OUT/$1.$3.err")" ] \
    && cmp -s <(tr -d '\r' < "$OUT/$1.$2.out" 2>/dev/null) <(tr -d '\r' < "$OUT/$1.$3.out" 2>/dev/null)
}
nsame=0; nfix=0; ndiff=0; nnat=0; npe=0; nnone=0
: > "$OUT/results.txt"
for f in "${TESTS[@]}"; do
  b=$(basename "$f" .fd)
  pb=$(cat "$OUT/$b.pe.build_rc" 2>/dev/null)
  nb=$(cat "$OUT/$b.nat.build_rc" 2>/dev/null)
  if [ "$pb" = 0 ] && [ "$nb" = 0 ]; then
    if same "$b" pe nat; then
      echo "SAME     $b" >> "$OUT/results.txt"; nsame=$((nsame+1))
    elif same "$b" nat run; then
      echo "NATFIX   $b (pe_ir rc $(cat "$OUT/$b.pe.rc"), --nat matches fid run)" >> "$OUT/results.txt"; nfix=$((nfix+1))
    else
      echo "DIFF     $b (pe_ir rc $(cat "$OUT/$b.pe.rc"), --nat rc $(cat "$OUT/$b.nat.rc"), fid run rc $(cat "$OUT/$b.run.rc" 2>/dev/null))" >> "$OUT/results.txt"; ndiff=$((ndiff+1))
    fi
  elif [ "$pb" = 0 ]; then
    echo "NATBUILD $b: $(grep -a -m1 -i 'error\|unresolved\|panic' "$OUT/$b.nat.build" | tr -d '\r' | cut -c1-160)" >> "$OUT/results.txt"; nnat=$((nnat+1))
  elif [ "$nb" = 0 ]; then
    echo "PEBUILD  $b" >> "$OUT/results.txt"; npe=$((npe+1))
  else
    echo "NOBUILD  $b" >> "$OUT/results.txt"; nnone=$((nnone+1))
  fi
done
grep -v '^SAME' "$OUT/results.txt" | head -80
echo "mi5_nat_peir_check: $nsame same, $nfix nat-fixes, $ndiff differ, $nnat nat-only build failures, $npe pe_ir-only build failures, $nnone neither built"
[ "$ndiff" = 0 ] && [ "$nnat" = 0 ]
