#!/usr/bin/env bash
# L2B (docs/roadmap/roadmap.md): `fid emit <file> --target universal`
# writes one macOS universal (fat) binary with an x86-64 and an arm64
# slice (compiler/main.fd build_universal_macho, compiler/nat_enc.fd
# nat_macho_fat). Checks, on any host:
#   1. the file is a fat binary as `lipo -info` reads it ("x86_64 arm64",
#      tests/nattools/macho_fat_check.py);
#   2. each slice is byte for byte the thin Mach-O that
#      `--target x86_64-macos` / `--target aarch64-macos` writes;
#   3. each slice's imports are bound through its own tables
#      (tests/nattools/macho_imports_check.py);
#   4. `--target universal-macos` is the same file; a bad --cpu is refused;
#   5. on macOS: `lipo -info` agrees, and the program runs and prints
#      what `fid run` prints, natively and (with Rosetta) as x86_64.
# Usage: FID=release/fid.exe bash tests/nattools/universal_check.sh
set -u
FID=${FID:-release/fid.exe}
PY=${PY:-python3}
command -v "$PY" >/dev/null 2>&1 || PY=python
SRC=${SRC:-tests/bootstrap/hello.fd}
T=$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/fid_universal_$$")
mkdir -p "$T"
fail=0
bad() { echo "FAIL: $*"; fail=1; }

"$FID" emit "$SRC" --target universal -o "$T/u" >"$T/u.log" 2>&1 || { cat "$T/u.log"; bad "emit --target universal"; }
"$FID" emit "$SRC" --target x86_64-macos -o "$T/x" >/dev/null 2>&1 || bad "emit --target x86_64-macos"
"$FID" emit "$SRC" --target aarch64-macos -o "$T/a" >/dev/null 2>&1 || bad "emit --target aarch64-macos"
"$FID" emit "$SRC" --target universal-macos -o "$T/u2" >/dev/null 2>&1 || bad "emit --target universal-macos"

if [ -f "$T/u" ]; then
    archs=$("$PY" tests/nattools/macho_fat_check.py "$T/u")
    [ "$archs" = "x86_64 arm64" ] || bad "fat check: $archs"
    "$PY" - "$T/u" "$T/x" "$T/a" <<'EOF' || fail=1
import struct, sys
fat = open(sys.argv[1], 'rb').read()
want = {0x01000007: open(sys.argv[2], 'rb').read(), 0x0100000C: open(sys.argv[3], 'rb').read()}
n = struct.unpack_from('>I', fat, 4)[0]
ok = True
for k in range(n):
    cpu, sub, off, size, align = struct.unpack_from('>IIIII', fat, 8 + 20 * k)
    if fat[off:off + size] != want.get(cpu, b''):
        print('FAIL: slice %08x differs from the thin build' % cpu)
        ok = False
print('slices identical to the thin builds' if ok else 'slices differ')
sys.exit(0 if ok else 1)
EOF
    "$PY" tests/nattools/macho_imports_check.py "$T/u" >"$T/imp.log" 2>&1 || { cat "$T/imp.log"; bad "imports check"; }
    if [ -f "$T/u2" ]; then
        cmp -s "$T/u" "$T/u2" || bad "--target universal-macos differs from --target universal"
    fi
fi
# a --cpu neither slice accepts is refused before anything is built
if "$FID" emit "$SRC" --target universal --cpu no-such-cpu -o "$T/bad" >"$T/bad.log" 2>&1; then
    bad "--cpu no-such-cpu accepted"
fi
if [ "$(uname -s)" = "Darwin" ] && [ -f "$T/u" ]; then
    lipo -info "$T/u" | grep -q "x86_64 arm64" || bad "lipo -info: $(lipo -info "$T/u")"
    chmod +x "$T/u"
    want=$("$FID" run "$SRC" 2>/dev/null)
    [ "$("$T/u")" = "$want" ] || bad "run (native slice)"
    if arch -x86_64 /usr/bin/true 2>/dev/null; then
        [ "$(arch -x86_64 "$T/u")" = "$want" ] || bad "run (x86_64 slice)"
    fi
    if arch -arm64 /usr/bin/true 2>/dev/null; then
        [ "$(arch -arm64 "$T/u")" = "$want" ] || bad "run (arm64 slice)"
    fi
fi
rm -rf "$T"
if [ $fail -eq 0 ]; then
    echo "universal_check: ok"
fi
exit $fail
