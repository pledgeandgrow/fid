#!/usr/bin/env bash
# RA1 (docs/roadmap/llvm-roadmap.md): pe_ir's own register allocator
# (compiler/pe_ir.fd peir_pick_reg_vars, peir_global_ra, peir_ra_scan:
# PEIR_GLOBAL_RA, PEIR_COALESCE, PEIR_VOLATILE_RA, PEIR_XMM_RA) is to be
# deleted once the Windows default moves to the machine layer (MI17).
# FID_PE_OFF=ra builds without it: every value stays in its frame slot, as
# in a bare build and in a function holding asm!. This script builds each
# program twice with plain `fid emit` (pe_ir), with the allocator and
# without it, runs both and compares them with each other and with the
# interpreter (`fid run`).
#
#   FID=release/fid.exe bash tests/nattools/ra1_pe_ra_off_check.sh
#   FILES="tests/bootstrap/test_pe_ra_off.fd ..."   only these programs
#   JOBS=4                                         programs at a time
#   OUT=<dir>                                      default ${TMPDIR:-/tmp}/ra1_pe_ra_off
#   REUSE=1                                        keep what an earlier run built
#
# Default FILES: tests/bootstrap/test_pe_ra_off.fd and every 10th program
# of tests/bootstrap (ALL=1: the whole suite, minus what
# mi5_nat_peir_check.sh leaves out). Each program ends in one of:
#   SAME     both built, same stdout, exit code and panic message
#   OFFFIX   they differ and the build without the allocator matches fid run
#   DIFF     they differ and the build without the allocator does not
#            match fid run (blocks: the allocator-free path is wrong)
#   OFFBUILD built with the allocator, not without it        (blocks)
#   ONBUILD  built without the allocator, not with it
#   NOBUILD  neither built it
# Passing: no DIFF and no OFFBUILD, and the switch changes the image of
# test_pe_ra_off (else FID_PE_OFF=ra did nothing). The last lines are
#   "ra1: switch takes effect (test_pe_ra_off image differs)"
#   "ra1_pe_ra_off_check: N same, F off-fixes, 0 differ, 0 off-only build failures, ..."
# followed by the total image size both ways (what deleting the allocator
# would cost while pe_ir is still the default), and the exit code is 0.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/ra1_pe_ra_off}"
JOBS="${JOBS:-4}"
mkdir -p "$OUT"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) ;; *) echo "ra1_pe_ra_off_check: needs a Windows host (pe_ir builds Windows executables)"; exit 2 ;; esac

skip_re='^(test_comptime_basic|test_comptime_func|test_comptime_reflect|test_comptime_serialize|test_comptime_gen|test_http|test_json|test_tensor|test_autodiff|test_kv_cache|test_quantization|test_checkpoint|test_shard|test_lora|test_simd|test_shape|test_shape_verify|test_wasm|test_allocator|test_effect_handler|test_asm|test_bare.*|test_boot|test_interrupt|test_mmu|test_gat|test_ffi_header|test_dlopen|test_game_stdlib|test_kernel_serial|test_serial_hello|test_os_gaps|test_phase8b|test_vec_math|test_sqlite|test_pe_sqlite|test_parse_header_sqlite|test_lib_(sdl|gl|font|image|audio|audio_mixer|shader|mesh)|test_elf_native)\.fd$'
TESTS=()
if [ -n "${FILES:-}" ]; then
  for f in $FILES; do TESTS+=("$f"); done
else
  TESTS+=("tests/bootstrap/test_pe_ra_off.fd")
  k=0
  for f in tests/bootstrap/*.fd; do
    b=$(basename "$f")
    [ "$b" = test_pe_ra_off.fd ] && continue
    case "$b" in *_fail.fd|*_fail2.fd) continue ;; esac
    if [[ "$b" =~ $skip_re ]]; then continue; fi
    if head -3 "$f" | grep -q 'expect-error'; then continue; fi
    k=$((k+1))
    if [ "${ALL:-}" != 1 ] && [ $((k % 10)) != 0 ]; then continue; fi
    TESTS+=("$f")
  done
fi
echo "ra1_pe_ra_off_check: ${#TESTS[@]} programs, $JOBS at a time, output in $OUT"

# one <file>: build with and without the allocator, run both and fid run
one() {
  local f="$1" b; b=$(basename "$f" .fd)
  if [ "${REUSE:-}" = 1 ] && [ -f "$OUT/$b.done" ]; then return; fi
  timeout 600 "$FID" emit "$f" -o "$OUT/$b.on.exe" > "$OUT/$b.on.build" 2>&1
  echo $? > "$OUT/$b.on.build_rc"
  FID_PE_OFF=ra timeout 600 "$FID" emit "$f" -o "$OUT/$b.off.exe" > "$OUT/$b.off.build" 2>&1
  echo $? > "$OUT/$b.off.build_rc"
  local k
  for k in on off; do
    if [ "$(cat "$OUT/$b.$k.build_rc")" = 0 ]; then
      timeout 120 "$OUT/$b.$k.exe" < /dev/null > "$OUT/$b.$k.out" 2> "$OUT/$b.$k.err"
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

panic_key() { grep -a -m1 '^Panic:' "$1" 2>/dev/null | tr -d '\r' | sed -E 's/^(Panic: [^:]*).*/\1/; s/ at line [0-9]+$//'; }
same() {
  [ "$(cat "$OUT/$1.$2.rc" 2>/dev/null)" = "$(cat "$OUT/$1.$3.rc" 2>/dev/null)" ] \
    && [ "$(panic_key "$OUT/$1.$2.err")" = "$(panic_key "$OUT/$1.$3.err")" ] \
    && cmp -s <(tr -d '\r' < "$OUT/$1.$2.out" 2>/dev/null) <(tr -d '\r' < "$OUT/$1.$3.out" 2>/dev/null)
}
# A program whose two outputs differ is built and run once more on its own
# before it is counted: a task program's lines can change order, or two of
# its prints mingle, when the machine is loaded and two programs run at a
# time (test_lowering_async_generic_task, Nov 27: a DIFF beside another
# job, the same three times out of three on its own)
for f in "${TESTS[@]}"; do
  b=$(basename "$f" .fd)
  if [ "$(cat "$OUT/$b.on.build_rc" 2>/dev/null)" = 0 ] && [ "$(cat "$OUT/$b.off.build_rc" 2>/dev/null)" = 0 ] && ! same "$b" on off; then
    echo "ra1: $b differs; built and run again on its own"
    REUSE="" one "$f"
  fi
done
nsame=0; nfix=0; ndiff=0; noff=0; non=0; nnone=0; size_on=0; size_off=0
: > "$OUT/results.txt"
for f in "${TESTS[@]}"; do
  b=$(basename "$f" .fd)
  ob=$(cat "$OUT/$b.on.build_rc" 2>/dev/null)
  fb=$(cat "$OUT/$b.off.build_rc" 2>/dev/null)
  if [ "$ob" = 0 ] && [ "$fb" = 0 ]; then
    size_on=$((size_on + $(wc -c < "$OUT/$b.on.exe")))
    size_off=$((size_off + $(wc -c < "$OUT/$b.off.exe")))
    if same "$b" on off; then
      echo "SAME     $b" >> "$OUT/results.txt"; nsame=$((nsame+1))
    elif same "$b" off run; then
      echo "OFFFIX   $b (with the allocator rc $(cat "$OUT/$b.on.rc"), without it matches fid run)" >> "$OUT/results.txt"; nfix=$((nfix+1))
    else
      echo "DIFF     $b (with rc $(cat "$OUT/$b.on.rc"), without rc $(cat "$OUT/$b.off.rc"), fid run rc $(cat "$OUT/$b.run.rc" 2>/dev/null))" >> "$OUT/results.txt"; ndiff=$((ndiff+1))
    fi
  elif [ "$ob" = 0 ]; then
    echo "OFFBUILD $b: $(grep -a -m1 -i 'error\|unresolved\|panic' "$OUT/$b.off.build" | tr -d '\r' | cut -c1-160)" >> "$OUT/results.txt"; noff=$((noff+1))
  elif [ "$fb" = 0 ]; then
    echo "ONBUILD  $b" >> "$OUT/results.txt"; non=$((non+1))
  else
    echo "NOBUILD  $b" >> "$OUT/results.txt"; nnone=$((nnone+1))
  fi
done
grep -v '^SAME' "$OUT/results.txt" | head -80
effect=1
if [ -f "$OUT/test_pe_ra_off.on.exe" ] && [ -f "$OUT/test_pe_ra_off.off.exe" ]; then
  if cmp -s "$OUT/test_pe_ra_off.on.exe" "$OUT/test_pe_ra_off.off.exe"; then
    echo "ra1: FID_PE_OFF=ra changed nothing (test_pe_ra_off image identical)"; effect=0
  else
    echo "ra1: switch takes effect (test_pe_ra_off image differs)"
  fi
else
  echo "ra1: test_pe_ra_off was not built both ways"; effect=0
fi
echo "ra1_pe_ra_off_check: $nsame same, $nfix off-fixes, $ndiff differ, $noff off-only build failures, $non on-only build failures, $nnone neither built"
echo "ra1: images built both ways: $size_on bytes with the allocator, $size_off without"
[ "$ndiff" = 0 ] && [ "$noff" = 0 ] && [ "$effect" = 1 ]
