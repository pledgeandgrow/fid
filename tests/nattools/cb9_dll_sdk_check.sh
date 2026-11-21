#!/usr/bin/env bash
# 2g.106 (docs/reference/known-bugs.md; CB9 in
# docs/roadmap/complaint-roadmap.md): `--nat` Windows and the macOS targets
# of the machine layer must report a C function no library the program
# links defines, as x86_64-linux and ccompile do:
#
#   error: link: the C function 'fides_cb9_not_in_libm' is not in any library the program links
#     --> tests/bootstrap/test_nat_link_missing_fail.fd:20: declared in extern "C" link "m"
#
# Windows reads each imported DLL's export directory (compiler/nat_enc.fd
# nat_pe_link_missing: the DLLs of %SystemRoot%\System32 or
# FID_NAT_LIBPATH), macOS the SDK's text-based stubs (nat_macho_link_missing:
# SDKROOT, else the Command Line Tools' or Xcode's SDK). Run after the
# compiler is built:
#   bash tests/nattools/cb9_dll_sdk_check.sh [fid]   (default release/fid.exe)
# 1. --nat Windows (on a Windows host): the missing function is reported;
#    test_verify_abi_c_mixed.fd (libm and libc functions that exist)
#    builds and prints its expected output.
# 2. macOS against a made-up SDK (SDKROOT=<dir>/sdk, its
#    usr/lib/libSystem.B.tbd written here): with no symbols every C function
#    of test_verify_abi_c_mixed.fd is reported; with those symbols listed it
#    builds; the missing function is still reported, with its declaration.
#    For x86_64-macos and aarch64-macos.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-cb9-dll-sdk"
rm -rf "$OUT/sdk"
mkdir -p "$OUT/sdk/usr/lib"
export FIDES_NO_CACHE=1
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
SRC=tests/bootstrap/test_nat_link_missing_fail.fd
MIXED=tests/bootstrap/test_verify_abi_c_mixed.fd
MSG="error: link: the C function 'fides_cb9_not_in_libm' is not in any library the program links"
LOC="--> $SRC:20: declared in extern \"C\" link \"m\""

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

# want_error <name> <log>: the build failed with the message and the place
want_error() {
    if grep -qF -- "$MSG" "$2" && grep -qF -- "$LOC" "$2"; then
        ok "$1: the missing C function reported with its declaration"
    else
        bad "$1: no Fides link error with the declaration (see $2)"
    fi
}

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac

# 1. --nat Windows
if [ "$HOST" = windows ]; then
    if "$FID" emit "$SRC" --nat -o "$OUT/missing.exe" >"$OUT/win-missing.log" 2>&1; then
        bad "nat_windows: built (want the link error)"
    else
        want_error nat_windows "$OUT/win-missing.log"
    fi
    if "$FID" emit "$MIXED" --nat -o "$OUT/mixed.exe" >"$OUT/win-mixed.log" 2>&1; then
        got=$(cd "$OUT" && ./mixed.exe 2>&1 | tr -d '\r')
        if [ "$got" = "$(expected "$MIXED")" ]; then
            ok "nat_windows: functions that exist build and run"
        else
            bad "nat_windows: test_verify_abi_c_mixed output differs"
            diff <(expected "$MIXED") <(echo "$got") | head -6
        fi
    else
        bad "nat_windows: test_verify_abi_c_mixed did not build: $(grep -v '^\[' "$OUT/win-mixed.log" | tail -3 | tr '\n' ' ')"
    fi
else
    echo "skip nat_windows: not a Windows host"
fi

# 2. macOS against a made-up SDK
tbd() {
    {
        echo "--- !tapi-tbd"
        echo "tbd-version:     4"
        echo "targets:         [ x86_64-macos, arm64-macos ]"
        echo "install-name:    '/usr/lib/libSystem.B.dylib'"
        echo "exports:"
        echo "  - targets:         [ x86_64-macos, arm64-macos ]"
        echo "    symbols:         [ $1 ]"
        echo "..."
    } > "$OUT/sdk/usr/lib/libSystem.B.tbd"
}
for t in x86_64-macos aarch64-macos; do
    tbd "_fides_cb9_placeholder"
    if SDKROOT="$OUT/sdk" "$FID" emit "$MIXED" --target "$t" -o "$OUT/mixed-$t" >"$OUT/$t-none.log" 2>&1; then
        bad "$t: built against an SDK that exports nothing"
        continue
    fi
    names=$(sed -n "s/.*error: link: the C function '\([A-Za-z0-9_]*\)'.*/\1/p" "$OUT/$t-none.log" | sort -u)
    for f in ldexp frexp modf fmod strtod; do
        if ! echo "$names" | grep -qx "$f"; then
            bad "$t: $f not reported against an SDK that exports nothing"
        fi
    done
    ok "$t: with no symbols reported: $(echo $names | tr '\n' ' ')"
    list=""
    for f in $names; do list="$list, _$f"; done
    tbd "${list#, }"
    if SDKROOT="$OUT/sdk" "$FID" emit "$MIXED" --target "$t" -o "$OUT/mixed-$t" >"$OUT/$t-all.log" 2>&1; then
        ok "$t: builds once the SDK exports them"
    else
        bad "$t: still fails with the SDK exporting them: $(grep -v '^\[' "$OUT/$t-all.log" | tail -2 | tr '\n' ' ')"
    fi
    if SDKROOT="$OUT/sdk" "$FID" emit "$SRC" --target "$t" -o "$OUT/missing-$t" >"$OUT/$t-missing.log" 2>&1; then
        bad "$t: the missing function built (want the link error)"
    else
        want_error "$t" "$OUT/$t-missing.log"
    fi
done

echo "cb9_dll_sdk_check: $fails failed (logs in $OUT)"
[ $fails -eq 0 ]
