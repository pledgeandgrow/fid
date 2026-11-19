#!/bin/bash
# nat-tools-check.sh — the machine layer's tools and writers of Nov 16
# (docs/roadmap/llvm-roadmap.md DB1, DB7, RL6, WP17, WP20, TG12, MI14 and
# competitive-roadmap LG13), over the tests/bootstrap/test_nattools_*.fd
# programs:
#   DB7   fid explain --stages <fn>: the MIR after selection and each MIR
#         pass on stderr ("[mir] <fn> after ...")
#   DB1   the .debug_line file table names the imported file too (readelf)
#   RL6   FID_VERIFY=1 builds pass the instruction-selection check on the
#         three Linux targets (and with --cpu x86-64-v3 / rva22u64)
#   WP17  with --profile-use the hot function is placed before main (nm -n)
#   WP20  a file without main builds as a library: FID_LIBFACTS gets an
#         `ext` line for each of its functions
#   TG12  `emit --obj` (when the driver has it): an ELF relocatable that
#         `ld -e fid_main` links into the same program
#   MI14  `emit --nat` images carry a COFF symbol table (FID_NAT_OFF=debug:
#         none, the bytes otherwise equal)
#   LG13  asm! templates run in a process (x86-64 Linux: the values)
# Linux-only steps (readelf, ld, running the program) are skipped when the
# tool or the host is missing; each skip is printed.
#
# Usage:
#   scripts/nat-tools-check.sh
#   FID=<fid>   the compiler (default release/fid.exe, else release/fid)
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
DIR=build/nat-tools-check
mkdir -p "$DIR"
export FIDES_NO_CACHE=1
fail=0
linux=0
[ "$(uname -s 2>/dev/null)" = Linux ] && linux=1

has() { # has <label> <file> <fixed text>
  if grep -qF -- "$3" "$2"; then
    echo "  ok    $1"
  else
    echo "  FAIL  $1: no '$3' in $2"
    fail=1
  fi
}
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }
skip() { echo "  skip  $1"; }

echo "== DB7 MIR after each machine stage"
"$FID" explain tests/bootstrap/test_nattools_mir.fd --stages square > "$DIR/mir.out" 2> "$DIR/mir.err"
has "after selection" "$DIR/mir.err" "[mir] square after selection:"
has "after the cold-path layout" "$DIR/mir.err" "[mir] square after the cold-path layout:"
has "after the peephole" "$DIR/mir.err" "[mir] square after the peephole:"
has "after allocation (registers)" "$DIR/mir.err" "[mir] square:"

echo "== RL6 instruction selection checked on values (FID_VERIFY=1)"
for t in x86_64-linux aarch64-linux riscv64-linux; do
  if FID_VERIFY=1 "$FID" emit tests/bootstrap/test_nattools_selcheck.fd --target "$t" -o "$DIR/sel-$t" > "$DIR/sel-$t.log" 2>&1; then
    ok "$t"
  else
    bad "$t: $(grep -m1 'instruction selection' "$DIR/sel-$t.log")"
  fi
done
if FID_VERIFY=1 "$FID" emit tests/bootstrap/test_nattools_selcheck.fd --target x86_64-linux --cpu x86-64-v3 -o "$DIR/sel-v3" > "$DIR/sel-v3.log" 2>&1; then
  ok "x86-64-v3 (BMI2 shifts)"
else
  bad "x86-64-v3: $(grep -m1 'instruction selection' "$DIR/sel-v3.log")"
fi
if FID_VERIFY=1 "$FID" emit tests/bootstrap/test_nattools_selcheck.fd --target riscv64-linux --cpu rva22u64 -o "$DIR/sel-zb" > "$DIR/sel-zb.log" 2>&1; then
  ok "rva22u64 (Zba/Zbb)"
else
  bad "rva22u64: $(grep -m1 'instruction selection' "$DIR/sel-zb.log")"
fi

echo "== DB1 line rows for the program's other files"
"$FID" emit tests/bootstrap/test_nattools_lines.fd --target x86_64-linux -o "$DIR/lines" > "$DIR/lines.log" 2>&1 || bad "emit lines"
if command -v readelf > /dev/null 2>&1; then
  readelf --debug-dump=decodedline "$DIR/lines" > "$DIR/lines.dwarf" 2>&1
  has "input file in the table" "$DIR/lines.dwarf" "test_nattools_lines.fd"
  has "imported file in the table" "$DIR/lines.dwarf" "lines_mod.fd"
else
  skip "readelf not found"
fi

echo "== DB1 core, instances and inlined code"
python3 tests/nattools/dbg_db1_files_check.py --fid "$FID" --keep "$DIR/db1files" || bad "DB1 files"

echo "== WP17 hot functions first"
"$FID" run tests/bootstrap/test_nattools_hot.fd --profile "$DIR/hot.prof" > /dev/null 2>&1
"$FID" emit tests/bootstrap/test_nattools_hot.fd --target x86_64-linux --profile-use "$DIR/hot.prof" -o "$DIR/hot" > "$DIR/hot.log" 2>&1 || bad "emit with --profile-use"
if command -v nm > /dev/null 2>&1; then
  order=$(nm -n "$DIR/hot" 2>/dev/null | awk '$3 == "hot_step" || $3 == "main" { print $3 }' | tr '\n' ' ')
  case "$order" in
    "hot_step main "*) ok "hot_step before main ($order)" ;;
    *) bad "order with the profile: '$order'" ;;
  esac
else
  skip "nm not found"
fi

echo "== WP20 library build (no main)"
rm -f "$DIR/lib.facts"
if FID_LIBFACTS="$DIR/lib.facts" "$FID" emit tests/nattools/lib_nomain.fd --target x86_64-linux -o "$DIR/lib" > "$DIR/lib.log" 2>&1; then
  has "ext line for lib_add" "$DIR/lib.facts" "ext lib_add "
  has "ext line for lib_scale" "$DIR/lib.facts" "ext lib_scale "
  has "ext line for a method" "$DIR/lib.facts" "ext LibPair_total "
else
  bad "library build: $(head -3 "$DIR/lib.log")"
fi

echo "== TG12 relocatable object"
if "$FID" emit tests/bootstrap/test_nattools_object.fd --obj --target x86_64-linux -o "$DIR/obj.o" > "$DIR/obj.log" 2>&1 && [ -s "$DIR/obj.o" ]; then
  if command -v readelf > /dev/null 2>&1; then
    readelf -h -s -r "$DIR/obj.o" > "$DIR/obj.txt" 2>&1
    has "ET_REL" "$DIR/obj.txt" "REL (Relocatable file)"
    if grep -E "GLOBAL +DEFAULT +1 fid_main" "$DIR/obj.txt" > /dev/null; then ok "fid_main global in .text"; else bad "no global fid_main in .text"; fi
    has "absolute relocations" "$DIR/obj.txt" "R_X86_64_64"
  else
    skip "readelf not found"
  fi
  if [ $linux = 1 ] && command -v ld > /dev/null 2>&1; then
    if ld -e fid_main "$DIR/obj.o" -o "$DIR/obj" 2> "$DIR/ld.err"; then
      "$DIR/obj" > "$DIR/obj.out"
      has "linked program runs" "$DIR/obj.out" "sum 60"
    else
      bad "ld: $(head -2 "$DIR/ld.err")"
    fi
  else
    skip "linking needs Linux and ld"
  fi
else
  skip "emit --obj not accepted by this driver (main.fd calls nat_emit_object)"
fi

echo "== MI14 --nat COFF symbols"
if "$FID" emit tests/bootstrap/test_nattools_mir.fd --nat -o "$DIR/nat.exe" > /dev/null 2>&1 \
  && FID_NAT_OFF=debug "$FID" emit tests/bootstrap/test_nattools_mir.fd --nat -o "$DIR/nat0.exe" > /dev/null 2>&1; then
  if command -v python3 > /dev/null 2>&1; then
    if python3 - "$DIR/nat.exe" "$DIR/nat0.exe" <<'PY'
import struct, sys
a = open(sys.argv[1], "rb").read()
b = open(sys.argv[2], "rb").read()
ptr, n = struct.unpack_from("<II", a, 76)
ptr0, n0 = struct.unpack_from("<II", b, 76)
ok = ptr0 == 0 and n0 == 0 and ptr == len(b) and n > 0
ok = ok and a[:76] == b[:76] and a[84:len(b)] == b[84:]
sys.exit(0 if ok else 1)
PY
    then ok "symbol table appended, image otherwise equal"; else bad "--nat symbol table"; fi
  else
    skip "python3 not found"
  fi
else
  skip "--nat build failed (Windows layer missing?)"
fi

echo "== LG13 asm! in a process"
if "$FID" emit tests/bootstrap/test_nattools_asm.fd --target x86_64-linux -o "$DIR/asm" > "$DIR/asm.log" 2>&1; then
  ok "x86-64 templates encode"
  if [ $linux = 1 ] && [ "$(uname -m)" = x86_64 ]; then
    "$DIR/asm" > "$DIR/asm.out"
    has "add" "$DIR/asm.out" "asm add ok"
    has "sub" "$DIR/asm.out" "asm sub ok"
    # the values themselves: tests/nattools/asm_probe.fd prints them
    if "$FID" emit tests/nattools/asm_probe.fd --target x86_64-linux -o "$DIR/asm_probe" > /dev/null 2>&1; then
      "$DIR/asm_probe" > "$DIR/asm_probe.out"
      has "template ran (42)" "$DIR/asm_probe.out" "42"
    else
      bad "asm probe build"
    fi
    if FID_NAT_OFF=asm "$FID" emit tests/nattools/asm_probe.fd --target x86_64-linux -o "$DIR/asm_probe0" > /dev/null 2>&1; then
      "$DIR/asm_probe0" > "$DIR/asm_probe0.out"
      if [ "$(cat "$DIR/asm_probe0.out")" = 0 ]; then ok "FID_NAT_OFF=asm gives 0"; else bad "FID_NAT_OFF=asm printed $(cat "$DIR/asm_probe0.out")"; fi
    else
      bad "asm probe build with FID_NAT_OFF=asm"
    fi
  else
    skip "running needs x86-64 Linux"
  fi
else
  bad "asm build: $(head -3 "$DIR/asm.log")"
fi
for t in aarch64-linux riscv64-linux; do
  if "$FID" emit tests/bootstrap/test_nattools_asm.fd --target "$t" -o "$DIR/asm-$t" > "$DIR/asm-$t.log" 2>&1; then
    ok "$t templates encode"
  else
    bad "$t asm build: $(head -3 "$DIR/asm-$t.log")"
  fi
done

if [ $fail = 0 ]; then echo "nat-tools-check: all passed"; else echo "nat-tools-check: FAILED"; fi
exit $fail
