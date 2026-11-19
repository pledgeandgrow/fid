#!/bin/bash
# rl9_writer_check.sh — RL9 (docs/roadmap/llvm-roadmap.md): the two writer
# bugs scripts/image-lint.py and readelf found (docs/verification/
# backend-checks.md, Nov 16) stay fixed, on real programs:
#   1. a dynamically linked ELF executable of the machine layer has an
#      SHT_DYNAMIC .dynamic section over its PT_DYNAMIC (readelf: "no
#      .dynamic section in the dynamic segment"); compiler/nat_enc.fd
#      nat_elf_debug_sections writes .interp, .dynstr and .dynamic (TG13)
#   2. a COFF object's long (> 8 bytes) symbol names point into its string
#      table (they had offset 0); compiler/image_writer.fd img_coff_symbols,
#      which coff.fd and nat_enc.fd nat_coff_object both use
#
# Usage: bash tests/nattools/rl9_writer_check.sh [<fid>]
#   <fid>   the compiler (default $FID, else release/fid.exe or release/fid);
#           it needs a C compiler for `ccompile` (CC, e.g. CC="zig cc")
#   OUT=<dir>     default build/rl9-writer-check
#   TARGETS="x86_64-linux aarch64-linux riscv64-linux"
#   REUSE=1       keep the two tools built by an earlier run in OUT
#   DIRECT=0      skip step 3 (for a compiler older than TG13, whose own
#                 ELF images still lack the section: steps 1 and 2 then
#                 check this tree's writers on its images)
#
# Steps, one PASS/FAIL line each; exit status the number of FAILs:
#  1. COFF: tests/nattools/rl9_coff_long.fd (ccompile) writes an object
#     with four long names and prints its expected lines; image-lint.py
#     finds nothing; objdump -t (pe-x86-64, here or in WSL) and dumpbin
#     -symbols (when found) list all four names.
#  2. ELF, through this tree's writer: tests/nattools/rl9_rewrite.fd
#     (ccompile) writes each target's hello.fd executable (built with
#     FID_NAT_OFF=ehframe) again with nat_elf_debug (FID_NAT_OFF=ehframe,
#     tests/nattools/rl9_elf_params.py reading what it needs); its loaded bytes (file offset 0
#     to the image's end, e_shoff/e_shnum/e_shstrndx masked) are the
#     input's, it has an SHT_DYNAMIC section when it has PT_DYNAMIC, and
#     image-lint.py and `readelf -W -a --debug-dump=info,line` report no
#     warning or error.
#  3. ELF, as <fid> writes it (`emit --target <t>`): the same image-lint and
#     readelf checks on the executable itself.
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
FID="${1:-${FID:-}}"
if [ -z "$FID" ]; then
  if [ $WIN = 1 ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/rl9-writer-check}"
TARGETS="${TARGETS:-x86_64-linux aarch64-linux riscv64-linux}"
mkdir -p "$OUT"
PY=python3; command -v python3 > /dev/null 2>&1 || PY=python
EXE=""; [ $WIN = 1 ] && EXE=.exe
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }
have() { command -v "$1" > /dev/null 2>&1; }

# A tool here, else in WSL (the file argument, last, as /mnt/<drive>/...)
WSL_OK=0
if [ $WIN = 1 ] && have wsl.exe; then WSL_OK=1; fi
tool_ok() { have "$1" && return 0; [ $WSL_OK = 1 ] && MSYS_NO_PATHCONV=1 wsl.exe -e sh -c "command -v $1" > /dev/null 2>&1; }
run_tool() {
  local tool="$1"; shift
  if have "$tool"; then "$tool" "$@"; return; fi
  local args=("$@") n=$#
  local img; img=$(cd "$(dirname "${args[$((n-1))]}")" && pwd)/$(basename "${args[$((n-1))]}")
  args[$((n-1))]="/mnt/$(echo "$img" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
  MSYS_NO_PATHCONV=1 wsl.exe -e "$tool" "${args[@]}"
}
DUMPBIN="${DUMPBIN:-}"
if [ -z "$DUMPBIN" ] && [ $WIN = 1 ]; then
  for d in "/c/Program Files/Microsoft Visual Studio"/*/*/VC/Tools/MSVC/*/bin/Hostx64/x64/dumpbin.exe; do
    [ -x "$d" ] && DUMPBIN="$d"
  done
fi
READELF=0; tool_ok readelf && READELF=1
OBJDUMP=0; tool_ok objdump && OBJDUMP=1

# readelf_clean <image>: no warning or error on stderr, exit 0
readelf_clean() {
  [ $READELF = 1 ] || return 0
  run_tool readelf -W -a --debug-dump=info,line "$1" > "$1.readelf" 2> "$1.readelf.err" || return 1
  ! grep -aqiE 'warning|error' "$1.readelf.err"
}
lint_clean() { "$PY" scripts/image-lint.py "$1" --quiet 2> "$1.lint"; }

# 1. COFF long names
if { [ "${REUSE:-0}" = 1 ] && [ -x "$OUT/cl$EXE" ]; } || "$FID" ccompile tests/nattools/rl9_coff_long.fd -o "$OUT/cl$EXE" > "$OUT/cl.build.log" 2>&1; then
  "$OUT/cl$EXE" "$OUT/long.obj" > "$OUT/cl.out" 2>&1
  rc=$?
  # the "Expected output:" lines of rl9_coff_long.fd
  want=$(sed -n '/^\/\/ Expected output:/,/^\/\/ (tests/p' tests/nattools/rl9_coff_long.fd | sed -e '1d' -e '$d' -e 's|^// ||')
  got=$(tr -d '\r' < "$OUT/cl.out")
  if [ $rc = 0 ] && [ "$got" = "$want" ]; then pass "1a rl9_coff_long prints its expected lines"; else fail "1a rl9_coff_long (exit $rc): $(head -3 "$OUT/cl.out" | tr '\n' ' ')"; fi
  if lint_clean "$OUT/long.obj"; then pass "1b image-lint: COFF object clean"; else fail "1b image-lint: $(head -2 "$OUT/long.obj.lint" | tr '\n' ' ')"; fi
  names="a_rather_long_function_name another_long_helper_function .rodata_table_with_long_name fid_main_entry_point"
  if [ $OBJDUMP = 1 ]; then
    run_tool objdump -t "$OUT/long.obj" > "$OUT/long.objdump" 2>&1
    miss=""; for nm in $names; do grep -q -- " $nm\$" <(tr -d '\r' < "$OUT/long.objdump") || miss="$miss $nm"; done
    if [ -z "$miss" ]; then pass "1c objdump -t lists the four long names"; else fail "1c objdump -t misses:$miss"; fi
  else
    echo "SKIP 1c no objdump"
  fi
  if [ -n "$DUMPBIN" ]; then
    "$DUMPBIN" -nologo -symbols "$(cygpath -w "$OUT/long.obj" 2>/dev/null || echo "$OUT/long.obj")" > "$OUT/long.dumpbin" 2>&1
    miss=""; for nm in $names; do grep -q -- "| $nm\$" <(tr -d '\r' < "$OUT/long.dumpbin") || miss="$miss $nm"; done
    if [ -z "$miss" ]; then pass "1d dumpbin -symbols lists the four long names"; else fail "1d dumpbin misses:$miss"; fi
  else
    echo "SKIP 1d no dumpbin"
  fi
else
  fail "1 ccompile rl9_coff_long.fd: $(grep -iE 'error' "$OUT/cl.build.log" | head -2 | tr '\n' ' ')"
fi

# 2 and 3. ELF executables
RW=0
if { [ "${REUSE:-0}" = 1 ] && [ -x "$OUT/rw$EXE" ]; } || "$FID" ccompile tests/nattools/rl9_rewrite.fd -o "$OUT/rw$EXE" > "$OUT/rw.build.log" 2>&1; then
  RW=1
else
  fail "2 ccompile rl9_rewrite.fd: $(grep -iE 'error' "$OUT/rw.build.log" | head -2 | tr '\n' ' ')"
fi
[ $READELF = 1 ] || echo "SKIP readelf (not here, not in WSL): image-lint only"
for t in $TARGETS; do
  img="$OUT/hello.$t"
  rm -f "$img"
  if ! "$FID" emit tests/bootstrap/hello.fd --target "$t" -o "$img" > "$img.log" 2>&1 || [ ! -s "$img" ]; then
    fail "2/3 $t: emit failed: $(tail -1 "$img.log")"
    continue
  fi
  # step 2's input without .eh_frame (DB6 appends it to the image and adds
  # PT_GNU_EH_FRAME; the rewrite, with FID_NAT_OFF=ehframe, does not)
  src="$img.noeh"
  rm -f "$src"
  if ! FID_NAT_OFF=ehframe "$FID" emit tests/bootstrap/hello.fd --target "$t" -o "$src" > "$src.log" 2>&1 || [ ! -s "$src" ]; then
    fail "2 $t: emit (FID_NAT_OFF=ehframe) failed: $(tail -1 "$src.log")"
    RWT=0
  else
    RWT=$RW
  fi
  if [ $RWT = 1 ]; then
    "$PY" tests/nattools/rl9_elf_params.py "$src" > "$img.params" 2> "$img.params.err"
    if FID_NAT_OFF=ehframe "$OUT/rw$EXE" "$img.params" "$src" "$img.new" > "$img.rw.log" 2>&1; then
      same=$("$PY" - "$src" "$img.new" "$img.params" <<'EOF'
import struct, sys
a = bytearray(open(sys.argv[1], "rb").read())
b = bytearray(open(sys.argv[2], "rb").read())
n = 0x1000 + int(open(sys.argv[3]).readline().split()[9])
for x in (a, b):
    x[0x28:0x30] = bytes(8)
    x[0x3C:0x40] = bytes(4)
d = [i for i in range(n) if i >= len(a) or i >= len(b) or a[i] != b[i]]
# PT_DYNAMIC present -> an SHT_DYNAMIC section present
c = open(sys.argv[2], "rb").read()
phoff, shoff = struct.unpack_from("<QQ", c, 32)
phnum, shnum = struct.unpack_from("<H", c, 56)[0], struct.unpack_from("<H", c, 60)[0]
pdyn = any(struct.unpack_from("<I", c, phoff + 56 * i)[0] == 2 for i in range(phnum))
sdyn = any(struct.unpack_from("<I", c, shoff + 64 * i + 4)[0] == 6 for i in range(shnum))
print("ok" if not d and (sdyn or not pdyn) else "loaded bytes differ: %d (first %s); PT_DYNAMIC %s, SHT_DYNAMIC %s" % (len(d), hex(d[0]) if d else "-", pdyn, sdyn))
EOF
)
      if [ "$same" = ok ]; then pass "2a $t: rewritten, same loaded bytes, .dynamic section when PT_DYNAMIC"; else fail "2a $t: $same"; fi
      if lint_clean "$img.new"; then pass "2b $t: image-lint clean (rewritten)"; else fail "2b $t: image-lint: $(head -2 "$img.new.lint" | tr '\n' ' ')"; fi
      if readelf_clean "$img.new"; then pass "2c $t: readelf clean (rewritten)"; else fail "2c $t: readelf: $(head -2 "$img.new.readelf.err" | tr '\n' ' ')"; fi
    else
      fail "2 $t: rewrite: $(tail -2 "$img.rw.log" | tr '\n' ' ') $(tail -1 "$img.params.err")"
    fi
  fi
  if [ "${DIRECT:-1}" != 0 ]; then
    if lint_clean "$img"; then pass "3a $t: image-lint clean ($FID emit)"; else fail "3a $t: image-lint: $(head -2 "$img.lint" | tr '\n' ' ')"; fi
    if readelf_clean "$img"; then pass "3b $t: readelf clean ($FID emit)"; else fail "3b $t: readelf: $(head -2 "$img.readelf.err" | tr '\n' ' ')"; fi
  fi
done
echo "rl9_writer_check: $bad failed"
exit $bad
