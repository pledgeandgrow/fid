#!/usr/bin/env bash
# XT77 and XT22 (docs/roadmap/target-roadmap.md): target routing of
# `fid emit|build --target` for iOS and Windows on ARM64. Structure only:
# nothing here runs the output (no Mac, no iPhone, no ARM64 Windows).
#
# XT77: `--target aarch64-ios` and `aarch64-ios-simulator` go to the machine
# layer's Mach-O writer (compiler/nat_enc.fd nat_macho_img) with the darwin
# runtime; LC_BUILD_VERSION names PLATFORM_IOS (2) or PLATFORM_IOSSIMULATOR
# (7) and the minimum iOS version (15.0 unless the target names one:
# `aarch64-ios17.2`). Checks:
#   1. each spelling builds; LC_BUILD_VERSION has the platform, minos and
#      sdk expected; scripts/macho-check.py --platform=N passes;
#   2. the iOS device image is the aarch64-macos image except for
#      LC_BUILD_VERSION and the code signature (the same code, runtime,
#      imports), and aarch64-macos still says macOS 11.0 / SDK 13.0;
#   3. `emit-ir` of IR saved from an iOS build writes the iOS platform;
#   4. a device target that is not aarch64, a bad version, and
#      `ccompile --target aarch64-ios` are refused with a message;
#   5. llvm-objdump (when found) reads "platform ios" / "iossimulator".
# XT22: `--target aarch64-windows` (and `windows-arm64`, `arm64-windows`,
# with or without --nat, and emit-ir) is the machine layer's: until its PE
# writer has ARM64 (XT14) the build must fail naming XT14 and write
# nothing; once it builds, the PE's machine must be 0xAA64 (never an x86-64
# PE and never the C route).
#
# Usage: FID=release/fid.exe bash tests/nattools/xt_ios_target_check.sh
#   LLVM_BIN=<dir with llvm-objdump/llvm-readobj> (optional)
set -u
FID=${FID:-release/fid.exe}
PY=${PY:-python3}
command -v "$PY" >/dev/null 2>&1 || PY=python
SRC=${SRC:-tests/bootstrap/hello.fd}
LLVM_BIN=${LLVM_BIN:-}
if [ -z "$LLVM_BIN" ]; then
    if command -v llvm-objdump >/dev/null 2>&1; then
        LLVM_BIN=$(dirname "$(command -v llvm-objdump)")
    elif [ -x "/c/Users/$USER/AppData/Local/Programs/llvm-21.1.0/bin/llvm-objdump.exe" ]; then
        LLVM_BIN="/c/Users/$USER/AppData/Local/Programs/llvm-21.1.0/bin"
    fi
fi
T=$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/fid_xt_ios_$$")
mkdir -p "$T"
fail=0
bad() { echo "FAIL: $*"; fail=1; }

# platform, minos and sdk of LC_BUILD_VERSION, and the CPU type, of a
# 64-bit Mach-O: "<platform> 0x<minos> 0x<sdk> 0x<cputype>"
bv() {
    "$PY" - "$1" <<'EOF'
import struct, sys
d = open(sys.argv[1], 'rb').read()
if len(d) < 32 or struct.unpack_from('<I', d, 0)[0] != 0xFEEDFACF:
    print('not-macho'); sys.exit(0)
cpu = struct.unpack_from('<I', d, 4)[0]
n = struct.unpack_from('<I', d, 16)[0]
off = 32
for _ in range(n):
    cmd, sz = struct.unpack_from('<II', d, off)
    if cmd == 0x32:
        p, mi, sdk = struct.unpack_from('<III', d, off + 8)
        print('%d 0x%x 0x%x 0x%x' % (p, mi, sdk, cpu)); sys.exit(0)
    off += sz
print('no-build-version')
EOF
}

# target | platform | minos | sdk | cputype
CASES="aarch64-ios|2|0xf0000|0x110000|0x100000c
aarch64-ios-simulator|7|0xf0000|0x110000|0x100000c
arm64-ios|2|0xf0000|0x110000|0x100000c
ios-arm64|2|0xf0000|0x110000|0x100000c
iossim-arm64|7|0xf0000|0x110000|0x100000c
aarch64-ios17.2|2|0x110200|0x110200|0x100000c
aarch64-ios16.4-simulator|7|0x100400|0x110000|0x100000c
aarch64-ios18|2|0x120000|0x120000|0x100000c
arm64-apple-ios15.0|2|0xf0000|0x110000|0x100000c
arm64-apple-ios14.0-simulator|7|0xe0000|0x110000|0x100000c
x86_64-ios-simulator|7|0xf0000|0x110000|0x1000007
aarch64-macos|1|0xb0000|0xd0000|0x100000c"

i=0
while IFS='|' read -r tgt plat minos sdk cpu; do
    [ -n "$tgt" ] || continue
    i=$((i + 1))
    out="$T/img$i"
    if ! "$FID" emit "$SRC" --target "$tgt" -o "$out" </dev/null >"$out.log" 2>&1; then
        tail -5 "$out.log"
        bad "emit --target $tgt"
        continue
    fi
    got=$(bv "$out")
    want="$plat $minos $sdk $cpu"
    [ "$got" = "$want" ] || bad "--target $tgt: LC_BUILD_VERSION/cpu '$got', expected '$want'"
    "$PY" scripts/macho-check.py "$out" --quiet --platform="$plat" </dev/null >"$out.chk" 2>&1 || { cat "$out.chk"; bad "macho-check --platform=$plat on --target $tgt"; }
    if [ -n "$LLVM_BIN" ]; then
        hdr=$("$LLVM_BIN/llvm-objdump" --macho --private-headers "$out" </dev/null 2>&1)
        case "$plat" in
            2) echo "$hdr" | grep -q "platform ios$" || bad "llvm-objdump: no 'platform ios' for $tgt" ;;
            7) echo "$hdr" | grep -q "platform iossimulator" || bad "llvm-objdump: no 'platform iossimulator' for $tgt" ;;
            1) echo "$hdr" | grep -q "platform macos" || bad "llvm-objdump: no 'platform macos' for $tgt" ;;
        esac
    fi
    [ "$tgt" = "aarch64-ios" ] && cp "$out" "$T/dev"
    [ "$tgt" = "aarch64-macos" ] && cp "$out" "$T/mac"
done <<EOF
$CASES
EOF
[ -n "$LLVM_BIN" ] && "$LLVM_BIN/llvm-objdump" --macho --private-headers "$T/dev" 2>/dev/null | grep -A5 LC_BUILD_VERSION

# 2. the device image is the macOS image but for LC_BUILD_VERSION and the
# signature (whose page hashes cover the header)
if [ -f "$T/dev" ] && [ -f "$T/mac" ]; then
    "$PY" - "$T/dev" "$T/mac" <<'EOF' || fail=1
import struct, sys
def norm(p):
    d = bytearray(open(p, 'rb').read())
    n = struct.unpack_from('<I', d, 16)[0]; off = 32; sig = len(d)
    for _ in range(n):
        cmd, sz = struct.unpack_from('<II', d, off)
        if cmd == 0x32:
            d[off + 8:off + 20] = bytes(12)
        if cmd == 0x1D:
            sig = struct.unpack_from('<I', d, off + 8)[0]
        off += sz
    return bytes(d[:sig])
a, b = norm(sys.argv[1]), norm(sys.argv[2])
if a != b:
    k = next((i for i in range(min(len(a), len(b))) if a[i] != b[i]), min(len(a), len(b)))
    print('FAIL: aarch64-ios and aarch64-macos differ beyond LC_BUILD_VERSION (first at 0x%x, sizes %d/%d)' % (k, len(a), len(b)))
    sys.exit(1)
print('aarch64-ios = aarch64-macos but for LC_BUILD_VERSION and the signature')
EOF
fi

# 3. emit-ir keeps the iOS target of the saved IR
if "$FID" emit "$SRC" --target aarch64-ios-simulator --save-ir "$T/sim.fir" -o "$T/sim0" >"$T/fir.log" 2>&1; then
    if "$FID" emit-ir "$T/sim.fir" -o "$T/sim1" >"$T/eir.log" 2>&1; then
        got=$(bv "$T/sim1")
        case "$got" in
            "7 0xf0000 0x110000 0x100000c") ;;
            *) bad "emit-ir of aarch64-ios-simulator IR: '$got'" ;;
        esac
    else
        tail -3 "$T/eir.log"; bad "emit-ir of aarch64-ios-simulator IR"
    fi
else
    tail -3 "$T/fir.log"; bad "emit --target aarch64-ios-simulator --save-ir"
fi

# 4. refused, with a message, nothing written
refuse() {  # <expected text> <fid args...>
    local want=$1; shift
    rm -f "$T/no"
    if "$FID" "$@" -o "$T/no" >"$T/no.log" 2>&1; then
        bad "accepted: fid $*"
    elif ! grep -q "$want" "$T/no.log"; then
        tail -3 "$T/no.log"; bad "fid $*: no '$want' in the message"
    fi
    [ -f "$T/no" ] && bad "fid $*: wrote an output"
}
refuse "iOS devices are aarch64" emit "$SRC" --target x86_64-ios
refuse "iOS devices are aarch64" emit "$SRC" --target riscv64-ios
refuse "not a version" emit "$SRC" --target aarch64-ios1x
refuse "not a version" emit "$SRC" --target aarch64-ios15.0.0.1
refuse "machine layer" ccompile "$SRC" --target aarch64-ios
refuse "machine layer" ccompile "$SRC" --target arm64-apple-ios15.0

# XT22: Windows on ARM64. A PE, when one is written, must be ARM64.
pe_machine() {
    "$PY" - "$1" <<'EOF'
import struct, sys
d = open(sys.argv[1], 'rb').read()
if len(d) < 0x40 or d[:2] != b'MZ':
    print('not-pe'); sys.exit(0)
pe = struct.unpack_from('<I', d, 0x3C)[0]
if d[pe:pe + 4] != b'PE\0\0':
    print('not-pe'); sys.exit(0)
print('0x%x' % struct.unpack_from('<H', d, pe + 4)[0])
EOF
}
for args in "--target aarch64-windows" "--target windows-arm64" "--target arm64-windows" "--target aarch64-windows --nat"; do
    rm -f "$T/w.exe"
    # shellcheck disable=SC2086
    if "$FID" emit "$SRC" $args -o "$T/w.exe" >"$T/w.log" 2>&1; then
        m=$(pe_machine "$T/w.exe")
        [ "$m" = "0xaa64" ] || bad "emit $args: wrote a PE of machine $m, not 0xaa64 (XT14)"
        if [ -n "$LLVM_BIN" ]; then
            "$LLVM_BIN/llvm-readobj" --file-headers "$T/w.exe" 2>&1 | grep -q "IMAGE_FILE_MACHINE_ARM64" || bad "llvm-readobj: $args not IMAGE_FILE_MACHINE_ARM64"
        fi
    else
        grep -q "XT14" "$T/w.log" || { tail -3 "$T/w.log"; bad "emit $args: the error does not name XT14"; }
        grep -q "Using C codegen\|zig cc -target" "$T/w.log" && bad "emit $args: went to the C route"
        [ -f "$T/w.exe" ] && bad "emit $args: failed but wrote $T/w.exe"
    fi
done
grep -h "XT14" "$T/w.log" 2>/dev/null | head -1
# emit-ir: IR saved for x86_64-windows, built for aarch64-windows
if "$FID" emit "$SRC" --target x86_64-windows --nat --save-ir "$T/win.fir" -o "$T/win0.exe" >"$T/wfir.log" 2>&1; then
    rm -f "$T/w1.exe"
    if "$FID" emit-ir "$T/win.fir" --target aarch64-windows -o "$T/w1.exe" >"$T/weir.log" 2>&1; then
        m=$(pe_machine "$T/w1.exe")
        [ "$m" = "0xaa64" ] || bad "emit-ir --target aarch64-windows: PE machine $m, not 0xaa64"
    else
        grep -q "XT14" "$T/weir.log" || { tail -3 "$T/weir.log"; bad "emit-ir --target aarch64-windows: the error does not name XT14"; }
    fi
fi

rm -rf "$T"
if [ $fail -eq 0 ]; then
    echo "xt_ios_target_check: ok"
fi
exit $fail
