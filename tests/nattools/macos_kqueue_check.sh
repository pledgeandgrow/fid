#!/usr/bin/env bash
# MI16 / release F7 (docs/roadmap/llvm-roadmap.md): the socket reactor of
# the shared runtime (stdlib/linux_runtime/conc.fd pe_ep_*: epoll_create1,
# eventfd2, epoll_ctl, epoll_pwait) on macOS, where darwin.fd emulates
# those four system calls over kqueue/kevent and a non-blocking pipe
# (pe_mac_epoll_*, pe_mac_eventfd; imports _kqueue, _kevent, _pipe at the
# end of compiler/nat_ir.fd ng_mac_imports). Without them every socket
# task polls for itself. Run after the compiler is built:
#   bash tests/nattools/macos_kqueue_check.sh [fid]   (default release/fid.exe)
# 1. On any host: the Mach-O of a socket program for aarch64-macos and
#    x86_64-macos binds _kqueue, _kevent and _pipe (their names are in the
#    image's bind information), and macho-check.py (when present) is clean.
# 2. On a macOS host: the image for the host's architecture prints the
#    program's expected lines (server and client tasks over loopback: the
#    reactor's waits go through kevent).
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-macos-kqueue"
mkdir -p "$OUT"
PY=python3
command -v python3 >/dev/null 2>&1 || PY=python
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

HOSTARCH=""
if [ "$(uname -s)" = Darwin ]; then
    case "$(uname -m)" in arm64) HOSTARCH=aarch64 ;; x86_64) HOSTARCH=x86_64 ;; esac
fi

for p in tests/bootstrap/test_ws_local.fd tests/bootstrap/test_http_server.fd; do
    [ -f "$p" ] || continue
    b=$(basename "$p" .fd)
    for arch in aarch64 x86_64; do
        img="$OUT/$b-$arch"
        if ! "$FID" emit "$p" --target "$arch-macos" -o "$img" >"$img.log" 2>&1; then
            bad "MI16 $arch-macos $b: emit failed ($(tail -1 "$img.log"))"
            continue
        fi
        miss=""
        for s in _kqueue _kevent _pipe; do
            if ! LC_ALL=C grep -a -q -- "$s" "$img"; then miss="$miss $s"; fi
        done
        if [ -z "$miss" ]; then
            ok "MI16 $arch-macos $b: binds _kqueue, _kevent, _pipe"
        else
            bad "MI16 $arch-macos $b: missing imports:$miss"
        fi
        if [ -f scripts/macho-check.py ]; then
            if $PY scripts/macho-check.py "$img" >"$img.macho.log" 2>&1; then
                ok "MI16 $arch-macos $b: macho-check clean"
            else
                bad "MI16 $arch-macos $b: macho-check: $(tail -1 "$img.macho.log")"
            fi
        fi
        if [ "$arch" = "$HOSTARCH" ]; then
            want=$(expected "$p" | tr -d '\r')
            got=$("$img" 2>/dev/null | tr -d '\r')
            if [ "$got" = "$want" ]; then
                ok "MI16 $arch-macos $b: runs on this Mac and prints the expected lines"
            else
                bad "MI16 $arch-macos $b: output differs on this Mac"
            fi
        fi
    done
done

if [ "$fails" -ne 0 ]; then
    echo "$fails failure(s)"
    exit 1
fi
echo "all MI16 kqueue checks passed"
