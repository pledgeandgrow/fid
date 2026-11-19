#!/bin/bash
# cb9_native_link_check.sh [FID]: complaint-roadmap CB9 on the machine
# layer's images. tests/bootstrap/test_nat_link_missing_fail.fd calls a C
# function no library has; each native build of it must fail with the
# Fides link error and the declaration's line and library:
#
#   error: link: the C function 'fides_cb9_not_in_libm' is not in any library the program links
#     --> tests/bootstrap/test_nat_link_missing_fail.fd:20: declared in extern "C" link "m"
#
#   --nat                    (Windows PE of the machine layer: an unresolved call)
#   --target x86_64-macos    (Mach-O: an unresolved call)
#   --target aarch64-macos
#   --target x86_64-linux    (dynamic import, CI2: libm and libc read at build
#                             time from FID_NAT_LIBPATH, here a folder with the
#                             WSL distribution's libm.so.6 and libc.so.6; on a
#                             Linux host the usual directories)
#   --target x86_64-linux with FID_NAT_OFF=ldyn (an unresolved call)
# and `fid ccompile` (the C linker's message, the first half of CB9).
#
#   bash tests/nattools/cb9_native_link_check.sh release/fid.exe
#   CB9_LIBDIR=<dir with libm.so.6 and libc.so.6> bash tests/nattools/cb9_native_link_check.sh
set -u
FID="${1:-release/fid.exe}"
export FIDES_NO_CACHE=1
SRC=tests/bootstrap/test_nat_link_missing_fail.fd
OUT=$(mktemp -d)
fail=0
MSG="error: link: the C function 'fides_cb9_not_in_libm' is not in any library the program links"
LOC="--> $SRC:20: declared in extern \"C\" link \"m\""

# The Linux libraries: CB9_LIBDIR, else copies from WSL
LIBDIR="${CB9_LIBDIR:-}"
if [ -z "$LIBDIR" ] && command -v wsl.exe > /dev/null 2>&1; then
    LIBDIR="$OUT/libs"
    mkdir -p "$LIBDIR"
    w=$(cd "$LIBDIR" && MSYS_NO_PATHCONV=1 wsl.exe wslpath -a "$(pwd -W 2>/dev/null || pwd)" 2>/dev/null | tr -d '\r')
    if [ -n "$w" ]; then
        MSYS_NO_PATHCONV=1 wsl.exe sh -c "for d in /lib/x86_64-linux-gnu /usr/lib/x86_64-linux-gnu /lib64 /usr/lib64; do for f in libm.so.6 libc.so.6; do [ -f \"\$d/\$f\" ] && [ ! -f \"$w/\$f\" ] && cp -L \"\$d/\$f\" \"$w/\$f\"; done; done" 2>/dev/null
    fi
fi

check() {
    local name="$1"
    shift
    if env "$@" "$FID" "${CMD[@]}" > "$OUT/$name.log" 2>&1; then
        echo "  FAIL  $name: built (want the link error)"
        fail=$((fail + 1))
        return
    fi
    if grep -qF "$MSG" "$OUT/$name.log" && grep -qF "$LOC" "$OUT/$name.log"; then
        echo "  ok    $name"
    else
        echo "  FAIL  $name: no Fides link error with the declaration (see $OUT/$name.log)"
        fail=$((fail + 1))
    fi
}

CMD=(emit "$SRC" --nat -o "$OUT/nat.exe");                       check nat_windows FIDES_NO_CACHE=1
CMD=(emit "$SRC" --target x86_64-macos -o "$OUT/mac_x64");       check macos_x64 FIDES_NO_CACHE=1
CMD=(emit "$SRC" --target aarch64-macos -o "$OUT/mac_a64");      check macos_a64 FIDES_NO_CACHE=1
CMD=(emit "$SRC" --target x86_64-linux -o "$OUT/linux_noldyn");  check linux_unresolved FID_NAT_OFF=ldyn
if [ -n "$LIBDIR" ] && [ -f "$LIBDIR/libm.so.6" ] && [ -f "$LIBDIR/libc.so.6" ]; then
    CMD=(emit "$SRC" --target x86_64-linux -o "$OUT/linux_ldyn"); check linux_ldyn "FID_NAT_LIBPATH=$LIBDIR"
else
    echo "  skip  linux_ldyn: no libm.so.6/libc.so.6 to read (set CB9_LIBDIR)"
fi
CMD=(ccompile "$SRC" -o "$OUT/c.exe");                           check ccompile FIDES_NO_CACHE=1

echo "cb9_native_link_check: $fail failed (logs in $OUT)"
[ $fail -eq 0 ]
