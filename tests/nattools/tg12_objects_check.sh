#!/usr/bin/env bash
# TG12 (docs/roadmap/llvm-roadmap.md): COFF and Mach-O objects from the
# machine layer's writer (compiler/nat_enc.fd nat_coff_object,
# nat_macho_object), linked by a system linker (zig cc: lld for COFF, its
# Mach-O linker for macOS).
#
#   FID=release/fid.exe bash tests/nattools/tg12_objects_check.sh
#
# 1. unit: tests/nattools/nat_object_unit.fd (built with `fid ccompile`,
#    CC="zig cc") writes the objects of small hand-made images and prints
#    their fields; the output must be the file's "Expected output".
#    coff.obj linked with fid_main as the entry must print what the PE of
#    the same image prints; the Mach-O objects must link, and the linked
#    programs' code must reach their data (tg12_macho_check.py).
# 2. programs: `fid emit tests/bootstrap/test_nattools_object.fd --obj`
#    with --nat (COFF), --target x86_64-macos and --target aarch64-macos
#    (Mach-O) must write objects that link; the Windows one must run and
#    print the test's expected output (needs nat_ir.fd's nat_emit_object
#    to accept win/mac: before that, the error is reported as SKIP).
# Each line is PASS, FAIL or SKIP; the exit status is the number of FAILs.
set -u
FID=${FID:-release/fid.exe}
ZIG=${ZIG:-zig}
PY=${PY:-python3}
command -v "$PY" > /dev/null 2>&1 || PY=python
DIR=${DIR:-$(mktemp -d)}
mkdir -p "$DIR"
fails=0
ok() { echo "PASS $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
skip() { echo "SKIP $1"; }
windows=0
case "$(uname -s 2>/dev/null)" in
  MINGW*|MSYS*|CYGWIN*) windows=1 ;;
esac
EXE=""
[ $windows = 1 ] && EXE=".exe"

echo "== unit (nat_object_unit.fd)"
if CC="$ZIG cc" "$FID" ccompile tests/nattools/nat_object_unit.fd -o "$DIR/nou$EXE" > "$DIR/nou.log" 2>&1; then
  NAT_OBJ_DIR="$DIR" "$DIR/nou$EXE" > "$DIR/unit.out" 2>&1
  sed -n '/^\/\/ Expected output:/,/^use /p' tests/nattools/nat_object_unit.fd | sed -n 's|^// ||p' | grep -v '^Expected output:' > "$DIR/unit.expected"
  if diff -u "$DIR/unit.expected" "$DIR/unit.out" > "$DIR/unit.diff"; then
    ok "unit output"
  else
    bad "unit output differs: $(head -8 "$DIR/unit.diff" | tr '\n' ' ')"
  fi
else
  bad "ccompile nat_object_unit.fd: $(tail -3 "$DIR/nou.log" | tr '\n' ' ')"
fi

if [ -s "$DIR/coff.obj" ]; then
  if "$ZIG" cc -target x86_64-windows-gnu -nostdlib "$DIR/coff.obj" -lkernel32 -Wl,--entry=fid_main -o "$DIR/coff_linked.exe" > "$DIR/coff_link.log" 2>&1; then
    ok "coff.obj links (lld)"
    if [ $windows = 1 ]; then
      a=$("$DIR/coff_exe.exe" 2>&1)
      b=$("$DIR/coff_linked.exe" 2>&1)
      if [ "$a" = "coff object ok" ] && [ "$b" = "$a" ]; then ok "linked COFF program prints what the PE prints"; else bad "PE '$a', linked '$b'"; fi
    else
      skip "running the Windows program needs Windows"
    fi
  else
    bad "coff.obj link: $(head -3 "$DIR/coff_link.log" | tr '\n' ' ')"
  fi
fi
if [ -s "$DIR/coff_export.obj" ]; then
  # a C program calls the object's exported entry point (fid_probe:
  # ExitProcess(0) after the C side printed its line)
  cat > "$DIR/probe.c" <<'EOF'
#include <stdio.h>
void fid_probe(void);
int main(void) { printf("calling fid_probe\n"); fflush(stdout); fid_probe(); return 3; }
EOF
  if "$ZIG" cc -target x86_64-windows-gnu "$DIR/probe.c" "$DIR/coff_export.obj" -o "$DIR/probe.exe" > "$DIR/probe_link.log" 2>&1; then
    ok "C program links against the exported symbol"
    if [ $windows = 1 ]; then
      out=$("$DIR/probe.exe" 2>&1); rc=$?
      if [ "$out" = "calling fid_probe" ] && [ $rc = 0 ]; then ok "C calls fid_probe (exit 0)"; else bad "probe printed '$out', exit $rc"; fi
    else
      skip "running the Windows program needs Windows"
    fi
  else
    bad "probe link: $(head -3 "$DIR/probe_link.log" | tr '\n' ' ')"
  fi
fi
for t in x64:x86_64-macos a64:aarch64-macos; do
  tag=${t%%:*}; target=${t#*:}
  [ -s "$DIR/macho_$tag.o" ] || continue
  if "$ZIG" cc -target "$target" "$DIR/macho_$tag.o" -Wl,-e,_fid_main -o "$DIR/macho_$tag" > "$DIR/macho_$tag.log" 2>&1; then
    ok "macho_$tag.o links"
    if "$PY" tests/nattools/tg12_macho_check.py "$DIR/macho_$tag" "$tag" > "$DIR/macho_$tag.chk" 2>&1; then
      ok "macho_$tag linked code reaches its data"
    else
      bad "macho_$tag: $(grep FAIL "$DIR/macho_$tag.chk")"
    fi
  else
    bad "macho_$tag.o link: $(head -3 "$DIR/macho_$tag.log" | tr '\n' ' ')"
  fi
done

echo "== programs (fid emit --obj)"
want=$(sed -n '/^\/\/ Expected output:/,/^$/p' tests/bootstrap/test_nattools_object.fd | sed -n 's|^// ||p' | grep -v '^Expected output:')
if "$FID" emit tests/bootstrap/test_nattools_object.fd --obj --nat -o "$DIR/prog.obj" > "$DIR/prog_obj.log" 2>&1 && [ -s "$DIR/prog.obj" ]; then
  if "$ZIG" cc -target x86_64-windows-gnu -nostdlib "$DIR/prog.obj" -lkernel32 -lws2_32 -lbcrypt -lntdll -lapi-ms-win-core-synch-l1-2-0 -Wl,--entry=fid_main -o "$DIR/prog_linked.exe" > "$DIR/prog_link.log" 2>&1; then
    ok "--nat object links"
    if [ $windows = 1 ]; then
      got=$("$DIR/prog_linked.exe" 2>&1)
      if [ "$got" = "$want" ]; then ok "linked --nat program prints the expected output"; else bad "linked --nat program printed '$got'"; fi
    else
      skip "running the Windows program needs Windows"
    fi
  else
    bad "--nat object link: $(head -3 "$DIR/prog_link.log" | tr '\n' ' ')"
  fi
elif grep -q "ELF objects for the Linux targets only" "$DIR/prog_obj.log"; then
  skip "--obj --nat refused (nat_ir.fd nat_emit_object not wired yet)"
else
  bad "emit --obj --nat: $(head -3 "$DIR/prog_obj.log" | tr '\n' ' ')"
fi
for target in x86_64-macos aarch64-macos; do
  if "$FID" emit tests/bootstrap/test_nattools_object.fd --obj --target "$target" -o "$DIR/prog_$target.o" > "$DIR/prog_$target.log" 2>&1 && [ -s "$DIR/prog_$target.o" ]; then
    if "$ZIG" cc -target "$target" "$DIR/prog_$target.o" -Wl,-e,_fid_main -o "$DIR/prog_$target" > "$DIR/prog_${target}_link.log" 2>&1; then
      ok "$target object links"
    else
      bad "$target object link: $(head -3 "$DIR/prog_${target}_link.log" | tr '\n' ' ')"
    fi
  elif grep -q "ELF objects for the Linux targets only" "$DIR/prog_$target.log"; then
    skip "--obj --target $target refused (nat_ir.fd nat_emit_object not wired yet)"
  else
    bad "emit --obj --target $target: $(head -3 "$DIR/prog_$target.log" | tr '\n' ' ')"
  fi
done

echo "fails: $fails"
exit $fails
