#!/bin/bash
# rl9_writer_fixes_check.sh — RL9 (docs/roadmap/llvm-roadmap.md): the two
# writer bugs scripts/image-lint.py found (docs/verification/
# backend-checks.md "Findings"), on real programs built by the compiler
# under test:
#   (1) a dynamically linked ELF executable of the machine layer (PT_INTERP,
#       PT_DYNAMIC: compiler/nat_ir.fd ng_gen_ldyn) needs a SHT_DYNAMIC
#       .dynamic section at PT_DYNAMIC's address and file offset
#       (compiler/nat_enc.fd nat_elf_debug_sections); without it readelf
#       says "Error: no .dynamic section in the dynamic segment".
#   (2) a COFF object of the assembler path (`fid codegen --coff`,
#       compiler/coff.fd over image_writer.fd img_coff_symbols) gives each
#       symbol name longer than 8 bytes the string-table offset of that
#       name; the old writer gave every one offset 0.
#
# Usage: bash tests/nattools/rl9_writer_fixes_check.sh [FID]
#   FID   the compiler (default release/fid.exe, else release/fid)
#   OUT   build/rl9-writer-fixes
#   WSL_DISTRO  where readelf runs when it is not on PATH (Windows)
#
# Checks (one line each, PASS / FAIL / SKIP), exit status the number of FAILs:
#   1 <target>  tests/bootstrap/test_verify_abi_c_mixed.fd (libm's ldexp,
#               frexp, modf and libc's strtod are bound at load time) for
#               x86_64-linux, aarch64-linux, riscv64-linux: the image has
#               PT_DYNAMIC (else SKIP: not dynamically linked by this
#               compiler), a SHT_DYNAMIC section at the same address and
#               offset whose sh_link is a string table, image-lint.py finds
#               nothing, and readelf -W -a (here or in WSL; SKIP without
#               one) prints no error or warning.
#   2 coff      tests/nattools/rl9_coff_long_names.fd through `codegen
#               --coff`: image-lint.py finds nothing, every long name's
#               offset is inside the string table and names a non-empty
#               string, and a_function_with_a_long_name and
#               another_quite_long_function_name are among the names.
# Before the fixes (the stage-0 compiler of Nov 17): 1 fails on every
# target ("PT_DYNAMIC (segment 4) but no SHT_DYNAMIC section"), 2 fails
# ("4977 symbol names outside the string table").
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
FID="${1:-${FID:-}}"
if [ -z "$FID" ]; then
  if [ $WIN = 1 ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/rl9-writer-fixes}"
mkdir -p "$OUT"
PY=python3; command -v python3 > /dev/null 2>&1 || PY=python
LINT=scripts/image-lint.py
bad=0; good=0
pass() { echo "PASS $1"; good=$((good + 1)); }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }
skip() { echo "SKIP $1"; }

# readelf here, or in WSL with the image as /mnt/<drive>/...
WSL_D=""
if ! command -v readelf > /dev/null 2>&1 && [ $WIN = 1 ] && command -v wsl.exe > /dev/null 2>&1; then
  WSL_D="${WSL_DISTRO:-}"
  [ -z "$WSL_D" ] && WSL_D=$(wsl.exe -l -q 2>/dev/null | tr -d '\0\r' | grep -v '^docker-desktop' | head -n 1)
  [ "$WSL_D" = none ] && WSL_D=""
  if [ -n "$WSL_D" ] && ! wsl.exe -d "$WSL_D" -e sh -c 'command -v readelf' > /dev/null 2>&1; then WSL_D=""; fi
fi
have_readelf() { command -v readelf > /dev/null 2>&1 || [ -n "$WSL_D" ]; }
run_readelf() {  # run_readelf <image> -> stdout + stderr
  if command -v readelf > /dev/null 2>&1; then readelf -W -a "$1" 2>&1; return; fi
  local img
  img=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
  if command -v cygpath > /dev/null 2>&1; then
    # C:/...: Git Bash's own /tmp and /c/... spellings are not what WSL sees
    img="/mnt/$(cygpath -m "$img" | sed -E 's|^([a-zA-Z]):/|\L\1/|')"
  else
    img="/mnt/$(echo "$img" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
  fi
  MSYS_NO_PATHCONV=1 wsl.exe -d "$WSL_D" -e readelf -W -a "$img" 2>&1
}

# dyn_report <image>: "none" (no PT_DYNAMIC), "ok", or what is wrong
dyn_report() {
  "$PY" - "$1" <<'EOF'
import struct, sys
d = open(sys.argv[1], "rb").read()
if d[:4] != b"\x7fELF":
    print("not an ELF file"); sys.exit()
phoff, shoff = struct.unpack_from("<QQ", d, 32)
phentsize, phnum, shentsize, shnum = struct.unpack_from("<HHHH", d, 54)
dyn = None
for i in range(phnum):
    p_type, p_flags, p_off, p_vaddr = struct.unpack_from("<IIQQ", d, phoff + i * phentsize)
    if p_type == 2:
        dyn = (p_vaddr, p_off)
if dyn is None:
    print("none"); sys.exit()
secs = [struct.unpack_from("<IIQQQQIIQQ", d, shoff + i * 64) for i in range(shnum)] if shoff else []
ds = [s for s in secs if s[1] == 6]
if not ds:
    print("PT_DYNAMIC but no SHT_DYNAMIC section"); sys.exit()
s = ds[0]
if (s[3], s[4]) != dyn:
    print("SHT_DYNAMIC at %#x/%#x, PT_DYNAMIC at %#x/%#x" % (s[3], s[4], dyn[0], dyn[1])); sys.exit()
if s[6] >= len(secs) or secs[s[6]][1] != 3:
    print("the .dynamic section's sh_link %d is not a string table" % s[6]); sys.exit()
if s[9] != 16:
    print("the .dynamic section's entry size is %d, not 16" % s[9]); sys.exit()
print("ok")
EOF
}

# 1
T1=tests/bootstrap/test_verify_abi_c_mixed.fd
for t in x86_64-linux aarch64-linux riscv64-linux; do
  o="$OUT/mixed.$t"
  rm -f "$o"
  if ! "$FID" emit "$T1" --target "$t" -o "$o" > "$o.log" 2>&1 || [ ! -s "$o" ]; then
    fail "1 $t: build failed: $(grep -v '^\[' "$o.log" | tail -1)"
    continue
  fi
  r=$(dyn_report "$o")
  if [ "$r" = none ]; then
    skip "1 $t: no PT_DYNAMIC (this compiler did not link it dynamically)"
    continue
  fi
  if [ "$r" != ok ]; then fail "1 $t: $r"; continue; fi
  if ! "$PY" "$LINT" "$o" --quiet 2> "$o.lint"; then
    fail "1 $t: image-lint: $(head -2 "$o.lint" | tr '\n' ' ')"
    continue
  fi
  if have_readelf; then
    run_readelf "$o" > "$o.readelf" 2>&1
    hit=$(grep -aiE '^readelf: (error|warning)|^(error|warning):' "$o.readelf" | head -2 | tr '\n' ' ')
    if [ -n "$hit" ]; then fail "1 $t: readelf: $hit"; continue; fi
    pass "1 $t: .dynamic at PT_DYNAMIC, image-lint and readelf clean"
  else
    pass "1 $t: .dynamic at PT_DYNAMIC, image-lint clean (readelf not installed)"
  fi
done

# 2
T2=tests/nattools/rl9_coff_long_names.fd
o="$OUT/long.obj"
rm -f "$o"
if ! "$FID" codegen "$T2" --coff "$o" > "$o.log" 2>&1 || [ ! -s "$o" ]; then
  fail "2 coff: build failed: $(grep -v '^\[' "$o.log" | tail -1)"
elif ! "$PY" "$LINT" "$o" --quiet 2> "$o.lint"; then
  fail "2 coff: image-lint: $(head -2 "$o.lint" | tr '\n' ' ')"
else
  r=$("$PY" - "$o" <<'EOF'
import struct, sys
d = open(sys.argv[1], "rb").read()
ptr, n = struct.unpack_from("<II", d, 8)
st = ptr + 18 * n
size = struct.unpack_from("<I", d, st)[0]
names, bad, k = set(), 0, 0
while k < n:
    o = ptr + 18 * k
    if struct.unpack_from("<I", d, o)[0] == 0:
        off = struct.unpack_from("<I", d, o + 4)[0]
        end = d.find(b"\0", st + off) if 4 <= off < size else -1
        if end < 0 or end == st + off or end > st + size:
            bad += 1
        else:
            names.add(d[st + off:end].decode("latin-1"))
    else:
        names.add(d[o:o + 8].rstrip(b"\0").decode("latin-1"))
    k += 1 + d[o + 17]
want = ["a_function_with_a_long_name", "another_quite_long_function_name", "short8nm"]
miss = [w for w in want if w not in names]
if bad:
    print("%d long names name nothing" % bad)
elif miss:
    print("missing symbols: " + " ".join(miss))
else:
    print("ok")
EOF
)
  if [ "$r" = ok ]; then pass "2 coff: image-lint clean, every long name resolves"; else fail "2 coff: $r"; fi
fi

echo "rl9_writer_fixes_check: $good passed, $bad failed"
exit $bad
