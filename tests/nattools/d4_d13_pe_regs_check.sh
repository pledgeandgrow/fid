#!/usr/bin/env bash
# D4 and D13 (docs/roadmap/release-roadmap.md, Track D) on pe_ir, the
# default Windows `fid emit` (compiler/pe_ir.fd, pe_ir_enc.fd):
#   r89    D4: r8 and r9 are value registers for live ranges that hold no
#          call, start and end at no call and are not parameters
#          (peir_global_ra: vok 2, peir_ra_scan, PEIR_ARG_RA)
#   cswap  D13: a constant left operand of a comparison or of a commutative
#          operation is moved right (`5 < x` is `cmp x, 5`, condition
#          mirrored; peir_op_commutes / peir_mirror_op)
#   cprop  D13: a register move that repeats or undoes the one emitted just
#          before is left out (PeirEnc_mov_rr_cp / PeirEnc_mov_imm_cp)
# FID_PE_OFF=<name> turns each one off. This script builds each program
# with all three on (the default) and with all three off, runs both and
# compares them with each other and with the interpreter (`fid run`).
#
#   FID=release/fid.exe bash tests/nattools/d4_d13_pe_regs_check.sh
#   FILES="a.fd b.fd"   only these programs (default: the two tests below
#                       plus every 10th program of tests/bootstrap;
#                       ALL=1: every one)
#   JOBS=4 OUT=<dir> REUSE=1   as in ra1_pe_ra_off_check.sh
#
# Each program ends in one of:
#   SAME     both built, same stdout, exit code and panic message
#   NEWFIX   they differ and the default build matches fid run
#   NEWDIFF  they differ and the build with the three off matches fid run
#            (blocks: D4/D13 changed what the program does)
#   BOTHDIFF they differ and neither matches fid run          (blocks)
#   NEWBUILD built with the three off, not by default           (blocks)
#   OFFBUILD built by default, not with the three off
#   NOBUILD  neither built it
# Then, on tests/bootstrap/test_pe_arg_regs.fd and test_pe_cmp_forms.fd:
#   - both print their files' expected output, both ways;
#   - d4:    `fid disasm --fn mix12` of the default image uses r8 or r9,
#            the FID_PE_OFF=r89 image's mix12 does not;
#   - cswap: signed_consts of the default image has no `mov eax, 0x4`
#            (the constant loaded to compare it), the FID_PE_OFF=cswap
#            image has at least 6;
#   - cprop: mix12 and mixed of the default image hold fewer adjacent
#            moves that repeat or undo the one before than the
#            FID_PE_OFF=cprop image, which holds at least one.
# Passing: no NEWDIFF, BOTHDIFF or NEWBUILD, every check ok; the last line
# is "d4_d13_pe_regs_check: N same, ..., 0 failed checks" and the exit
# code is 0.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/d4_d13_pe_regs}"
JOBS="${JOBS:-4}"
mkdir -p "$OUT"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) ;; *) echo "d4_d13_pe_regs_check: needs a Windows host (pe_ir builds Windows executables)"; exit 2 ;; esac
OFFALL="r89,cswap,cprop"

skip_re='^(test_comptime_basic|test_comptime_func|test_comptime_reflect|test_comptime_serialize|test_comptime_gen|test_http|test_json|test_tensor|test_autodiff|test_kv_cache|test_quantization|test_checkpoint|test_shard|test_lora|test_simd|test_shape|test_shape_verify|test_wasm|test_allocator|test_effect_handler|test_asm|test_bare.*|test_boot|test_interrupt|test_mmu|test_gat|test_ffi_header|test_dlopen|test_game_stdlib|test_kernel_serial|test_serial_hello|test_os_gaps|test_phase8b|test_vec_math|test_sqlite|test_pe_sqlite|test_parse_header_sqlite|test_lib_(sdl|gl|font|image|audio|audio_mixer|shader|mesh)|test_elf_native)\.fd$'
TESTS=()
if [ -n "${FILES:-}" ]; then
  for f in $FILES; do TESTS+=("$f"); done
else
  TESTS+=("tests/bootstrap/test_pe_arg_regs.fd" "tests/bootstrap/test_pe_cmp_forms.fd")
  k=0
  for f in tests/bootstrap/*.fd; do
    b=$(basename "$f")
    case "$b" in test_pe_arg_regs.fd|test_pe_cmp_forms.fd) continue ;; esac
    case "$b" in *_fail.fd|*_fail2.fd) continue ;; esac
    if [[ "$b" =~ $skip_re ]]; then continue; fi
    if head -3 "$f" | grep -q 'expect-error'; then continue; fi
    k=$((k+1))
    if [ "${ALL:-}" != 1 ] && [ $((k % 10)) != 0 ]; then continue; fi
    TESTS+=("$f")
  done
fi
echo "d4_d13_pe_regs_check: ${#TESTS[@]} programs, $JOBS at a time, output in $OUT"

one() {
  local f="$1" b; b=$(basename "$f" .fd)
  if [ "${REUSE:-}" = 1 ] && [ -f "$OUT/$b.done" ]; then return; fi
  timeout 600 "$FID" emit "$f" -o "$OUT/$b.on.exe" > "$OUT/$b.on.build" 2>&1
  echo $? > "$OUT/$b.on.build_rc"
  FID_PE_OFF="$OFFALL" timeout 600 "$FID" emit "$f" -o "$OUT/$b.off.exe" > "$OUT/$b.off.build" 2>&1
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
export FID OUT REUSE OFFALL
printf '%s\n' "${TESTS[@]}" | xargs -P "$JOBS" -I{} bash -c 'one "$@"' _ {}

panic_key() { grep -a -m1 '^Panic:' "$1" 2>/dev/null | tr -d '\r' | sed -E 's/^(Panic: [^:]*).*/\1/; s/ at line [0-9]+$//'; }
same() {
  [ "$(cat "$OUT/$1.$2.rc" 2>/dev/null)" = "$(cat "$OUT/$1.$3.rc" 2>/dev/null)" ] \
    && [ "$(panic_key "$OUT/$1.$2.err")" = "$(panic_key "$OUT/$1.$3.err")" ] \
    && cmp -s <(tr -d '\r' < "$OUT/$1.$2.out" 2>/dev/null) <(tr -d '\r' < "$OUT/$1.$3.out" 2>/dev/null)
}
nsame=0; nfix=0; ndiff=0; nboth=0; nnew=0; noff=0; nnone=0
: > "$OUT/results.txt"
for f in "${TESTS[@]}"; do
  b=$(basename "$f" .fd)
  ob=$(cat "$OUT/$b.on.build_rc" 2>/dev/null)
  fb=$(cat "$OUT/$b.off.build_rc" 2>/dev/null)
  if [ "$ob" = 0 ] && [ "$fb" = 0 ]; then
    if same "$b" on off; then
      echo "SAME     $b" >> "$OUT/results.txt"; nsame=$((nsame+1))
    elif same "$b" on run; then
      echo "NEWFIX   $b" >> "$OUT/results.txt"; nfix=$((nfix+1))
    elif same "$b" off run; then
      echo "NEWDIFF  $b (default rc $(cat "$OUT/$b.on.rc"), off rc $(cat "$OUT/$b.off.rc"), fid run rc $(cat "$OUT/$b.run.rc" 2>/dev/null))" >> "$OUT/results.txt"; ndiff=$((ndiff+1))
    else
      echo "BOTHDIFF $b (default rc $(cat "$OUT/$b.on.rc"), off rc $(cat "$OUT/$b.off.rc"), fid run rc $(cat "$OUT/$b.run.rc" 2>/dev/null))" >> "$OUT/results.txt"; nboth=$((nboth+1))
    fi
  elif [ "$fb" = 0 ]; then
    echo "NEWBUILD $b: $(grep -a -m1 -i 'error\|unresolved\|panic' "$OUT/$b.on.build" | tr -d '\r' | cut -c1-160)" >> "$OUT/results.txt"; nnew=$((nnew+1))
  elif [ "$ob" = 0 ]; then
    echo "OFFBUILD $b" >> "$OUT/results.txt"; noff=$((noff+1))
  else
    echo "NOBUILD  $b" >> "$OUT/results.txt"; nnone=$((nnone+1))
  fi
done
grep -v '^SAME' "$OUT/results.txt" | head -80

fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

for t in test_pe_arg_regs test_pe_cmp_forms; do
  want="$(expected "tests/bootstrap/$t.fd")"
  for k in on off; do
    if [ -f "$OUT/$t.$k.out" ] && [ "$(tr -d '\r' < "$OUT/$t.$k.out")" = "$want" ] && [ "$(cat "$OUT/$t.$k.rc")" = 0 ]; then
      ok "$t ($k) prints the expected output"
    else
      bad "$t ($k) does not print the expected output"
    fi
  done
done

# The disassembly of one function of an image built with FID_PE_OFF=$2
# ("" = default) of test $1: $OUT/$1.<off>.<fn>.s
dis() {
  local t="$1" off="$2" fn="$3" img="$OUT/$1.dis.${2:-none}.exe"
  if [ ! -f "$img" ]; then
    FID_PE_OFF="$off" timeout 600 "$FID" emit "tests/bootstrap/$t.fd" -o "$img" > /dev/null 2>&1 || return 1
  fi
  timeout 120 "$FID" disasm "$img" --fn "$fn" | tr -d '\r' > "$OUT/$t.${off:-none}.$fn.s"
}
# Adjacent moves where the second repeats or undoes the first
redundant() {
  awk '
    { line = $0; sub(/^.*:  [0-9a-f ]+  +/, "", line) }
    line ~ /^mov [a-z0-9]+, [a-z0-9x]+$/ {
      split(substr(line, 5), p, ", ")
      if (have && ((p[1] == d && p[2] == s) || (p[1] == s && p[2] == d))) n++
      d = p[1]; s = p[2]; have = 1; next
    }
    { have = 0 }
    END { print n + 0 }' "$1"
}

if dis test_pe_arg_regs "" mix12 && dis test_pe_arg_regs r89 mix12; then
  n_on=$(grep -cE '\br(8|9)[dwb]?\b' "$OUT/test_pe_arg_regs.none.mix12.s")
  n_off=$(grep -cE '\br(8|9)[dwb]?\b' "$OUT/test_pe_arg_regs.r89.mix12.s")
  if [ "$n_on" -gt 0 ] && [ "$n_off" = 0 ]; then
    ok "d4: mix12 uses r8/r9 on $n_on lines by default, none with FID_PE_OFF=r89"
  else
    bad "d4: mix12 lines with r8/r9: $n_on by default, $n_off with FID_PE_OFF=r89"
  fi
else
  bad "d4: could not build or disassemble test_pe_arg_regs"
fi

if dis test_pe_cmp_forms "" signed_consts && dis test_pe_cmp_forms cswap signed_consts; then
  c_on=$(grep -cE 'mov eax, 0x4$' "$OUT/test_pe_cmp_forms.none.signed_consts.s")
  c_off=$(grep -cE 'mov eax, 0x4$' "$OUT/test_pe_cmp_forms.cswap.signed_consts.s")
  if [ "$c_on" = 0 ] && [ "$c_off" -ge 6 ]; then
    ok "cswap: signed_consts loads the constant 4 to compare it 0 times by default, $c_off with FID_PE_OFF=cswap"
  else
    bad "cswap: \`mov eax, 0x4\` in signed_consts: $c_on by default, $c_off with FID_PE_OFF=cswap"
  fi
else
  bad "cswap: could not build or disassemble test_pe_cmp_forms"
fi

r_on=0; r_off=0
for pair in "test_pe_arg_regs mix12" "test_pe_cmp_forms mixed"; do
  set -- $pair
  if dis "$1" "" "$2" && dis "$1" cprop "$2"; then
    r_on=$((r_on + $(redundant "$OUT/$1.none.$2.s")))
    r_off=$((r_off + $(redundant "$OUT/$1.cprop.$2.s")))
  else
    bad "cprop: could not build or disassemble $1"
  fi
done
if [ "$r_off" -gt 0 ] && [ "$r_on" -lt "$r_off" ]; then
  ok "cprop: $r_on repeated or undone moves by default, $r_off with FID_PE_OFF=cprop"
else
  bad "cprop: repeated or undone moves: $r_on by default, $r_off with FID_PE_OFF=cprop"
fi

echo "d4_d13_pe_regs_check: $nsame same, $nfix default-only fixes, $ndiff new differences, $nboth both differ, $nnew default-only build failures, $noff off-only build failures, $nnone neither built, $fails failed checks"
[ "$ndiff" = 0 ] && [ "$nboth" = 0 ] && [ "$nnew" = 0 ] && [ "$fails" = 0 ]
