#!/bin/bash
# image-lint-check.sh — RL9 (llvm roadmap): our own image reader
# (scripts/image-lint.py) passes the images our writers produce and finds a
# broken one, and scripts/image-check.sh runs it with the system's readers
# over every format.
#
# Usage: tests/tooling/image-lint-check.sh
#   FID=<compiler>  default release/fid.exe (release/fid off Windows)
#   OUT=<dir>       default build/image-lint-check
#   CHECK=0         skip check 5 (an image-check.sh run)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. image-lint.py --self-test: its own small ELF and PE pass, and each
#     of 16 corruptions of them is found.
#  2. tests/bootstrap/hello.fd as an ELF object (x86-64, AArch64), a PE of
#     the machine layer (--nat) and, on Windows, of pe_ir: image-lint.py
#     finds nothing.
#  3. hello.fd as ELF executables (x86-64, AArch64, RISC-V): image-lint.py
#     finds nothing (a PT_DYNAMIC without its SHT_DYNAMIC section, as
#     readelf reports it, was fixed by TG13: nat_enc.fd
#     nat_elf_debug_sections writes .interp, .dynstr and .dynamic;
#     tests/nattools/rl9_writer_fixes_check.sh checks it and the COFF
#     long-name fix on real programs).
#  4. Corrupted copies of those images are found: a PE with SizeOfImage
#     one page too big, an ELF object whose section headers start past the
#     end, an ELF executable whose first PT_LOAD alignment is 3.
#  5. scripts/image-check.sh FORMATS="pe obj" on hello.fd: exit 0, and the
#     "image-lint" reader ran (its .log files exist).
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
if [ $WIN = 1 ]; then FID="${FID:-release/fid.exe}"; else FID="${FID:-release/fid}"; fi
OUT="${OUT:-build/image-lint-check}"
mkdir -p "$OUT"
PY=python3; command -v python3 > /dev/null 2>&1 || PY=python
LINT=scripts/image-lint.py
T=tests/bootstrap/hello.fd
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

# 1
if "$PY" "$LINT" --self-test > "$OUT/self.log" 2>&1; then
  pass "1 self-test: $(tail -1 "$OUT/self.log")"
else
  fail "1 self-test: $(grep '^FAIL' "$OUT/self.log" | head -3 | tr '\n' ' ')"
fi

# emit <out> <args...>
emit() {
  local o="$1"; shift
  rm -f "$o"
  "$FID" emit "$T" "$@" -o "$o" > "$o.log" 2>&1 && [ -s "$o" ]
}
lint() { "$PY" "$LINT" "$1" --quiet 2> "$1.lint"; }

# 2
for spec in "obj-x64.o|--target x86_64-linux --obj" "obj-a64.o|--target aarch64-linux --obj" "nat.exe|--nat" "peir.exe|"; do
  IFS='|' read -r name args <<< "$spec"
  [ "$name" = peir.exe ] && [ $WIN = 0 ] && continue
  o="$OUT/hello.$name"
  # shellcheck disable=SC2086
  if ! emit "$o" $args; then fail "2 $name: build failed: $(tail -1 "$o.log")"; continue; fi
  if lint "$o"; then pass "2 $name: no finding"; else fail "2 $name: $(head -2 "$o.lint" | tr '\n' ' ')"; fi
done

# 3
for t in x86_64-linux aarch64-linux riscv64-linux; do
  o="$OUT/hello.$t"
  if ! emit "$o" --target "$t"; then fail "3 $t: build failed: $(tail -1 "$o.log")"; continue; fi
  if lint "$o"; then pass "3 $t: no finding"; else fail "3 $t: $(head -2 "$o.lint" | tr '\n' ' ')"; fi
done

# 4: patch <in> <out> <offset expression> <struct format> <value expression>
patch() {
  "$PY" - "$1" "$2" "$3" "$4" "$5" <<'EOF'
import struct, sys
src, dst, off, fmt, val = sys.argv[1:]
d = bytearray(open(src, "rb").read())
off = eval(off, {"d": d, "struct": struct})
val = eval(val, {"d": d, "struct": struct, "off": off})
struct.pack_into(fmt, d, off, val)
open(dst, "wb").write(d)
EOF
}
if [ -s "$OUT/hello.nat.exe" ]; then
  # SizeOfImage: optional header at e_lfanew + 24, field at +56
  patch "$OUT/hello.nat.exe" "$OUT/bad-pe.exe" 'struct.unpack_from("<I", d, 0x3C)[0] + 24 + 56' '<I' 'struct.unpack_from("<I", d, off)[0] + 0x1000'
  if lint "$OUT/bad-pe.exe"; then fail "4a a PE with a wrong SizeOfImage passed"; else pass "4a wrong SizeOfImage found: $(head -1 "$OUT/bad-pe.exe.lint" | sed 's/.*: //')"; fi
fi
if [ -s "$OUT/hello.obj-x64.o" ]; then
  patch "$OUT/hello.obj-x64.o" "$OUT/bad.o" '40' '<Q' 'len(d) + 64'
  if lint "$OUT/bad.o"; then fail "4b an ELF object with e_shoff past the end passed"; else pass "4b e_shoff past the end found"; fi
fi
if [ -s "$OUT/hello.x86_64-linux" ]; then
  # the first PT_LOAD's p_align (program header at e_phoff, p_align at +48)
  patch "$OUT/hello.x86_64-linux" "$OUT/bad-exe" 'next(struct.unpack_from("<Q", d, 32)[0] + 56 * i + 48 for i in range(struct.unpack_from("<H", d, 56)[0]) if struct.unpack_from("<I", d, struct.unpack_from("<Q", d, 32)[0] + 56 * i)[0] == 1)' '<Q' '3'
  if lint "$OUT/bad-exe"; then fail "4c an ELF with PT_LOAD alignment 3 passed"; else pass "4c PT_LOAD alignment 3 found"; fi
fi

# 5
if [ "${CHECK:-1}" != 0 ]; then
  FID="$FID" OUT="$OUT/ic" FORMATS="pe obj" bash scripts/image-check.sh "$T" > "$OUT/ic.log" 2>&1
  rc=$?
  nl=$(ls "$OUT/ic"/*.image-lint.log 2>/dev/null | wc -l)
  if [ $rc = 0 ] && [ "$nl" -gt 0 ]; then
    pass "5 image-check FORMATS=\"pe obj\": $(tail -1 "$OUT/ic.log") ($nl images through image-lint)"
  else
    fail "5 image-check (exit $rc, $nl image-lint logs): $(grep -E 'FAIL|BUILD' "$OUT/ic.log" | head -3 | tr '\n' ' ')"
  fi
fi
echo "image-lint-check: $bad failed"
exit $bad
