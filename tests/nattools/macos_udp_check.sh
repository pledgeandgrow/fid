#!/usr/bin/env bash
# MI16 / release F7 (docs/roadmap/llvm-roadmap.md): recvfrom's peer address
# and sysinfo on macOS (stdlib/linux_runtime/darwin.fd pe_mac_recvfrom,
# pe_mac_sa_back, pe_mac_sysinfo), and stdlib/net.fd's UDP functions on the
# machine layer (stdlib/linux_runtime/sys.fd pert_c_pledge_net_*).
# Program: tests/nattools/udp_peer.fd (its "// Expected output:" lines).
# Run after the compiler is built:
#   bash tests/nattools/macos_udp_check.sh [fid]   (default release/fid.exe)
# 1. Any host: the Mach-O for aarch64-macos and x86_64-macos builds and
#    binds _recvfrom and _sysctlbyname.
# 2. A macOS host: the image of the host's architecture prints the expected
#    lines (sender port and address right, memory pages from sysinfo).
# 3. A Linux host (or WSL): the x86_64-linux image prints them too, and the
#    aarch64-linux / riscv64-linux ones under qemu-user when it is there.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-macos-udp"
mkdir -p "$OUT"
P=tests/nattools/udp_peer.fd
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,$p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
want=$(expected "$P" | tr -d '\r')

HOSTARCH=""
case "$(uname -s)" in
    Darwin) case "$(uname -m)" in arm64) HOSTARCH=aarch64 ;; x86_64) HOSTARCH=x86_64 ;; esac ;;
esac

for arch in aarch64 x86_64; do
    img="$OUT/udp_peer-$arch-macos"
    if ! "$FID" emit "$P" --target "$arch-macos" -o "$img" >"$img.log" 2>&1; then
        bad "MI16 $arch-macos: emit failed ($(tail -1 "$img.log"))"
        continue
    fi
    miss=""
    for s in _recvfrom _sysctlbyname; do
        if ! LC_ALL=C grep -a -q -- "$s" "$img"; then miss="$miss $s"; fi
    done
    if [ -z "$miss" ]; then ok "MI16 $arch-macos: binds _recvfrom, _sysctlbyname"
    else bad "MI16 $arch-macos: missing imports:$miss"; fi
    if [ "$arch" = "$HOSTARCH" ]; then
        chmod +x "$img"
        got=$("$img" 2>/dev/null | tr -d '\r')
        if [ "$got" = "$want" ]; then ok "MI16 $arch-macos: runs on this Mac, expected lines"
        else bad "MI16 $arch-macos: output differs on this Mac:"; echo "$got"; fi
    fi
done

if [ "$(uname -s)" = Linux ]; then
    for arch in x86_64 aarch64 riscv64; do
        img="$OUT/udp_peer-$arch-linux"
        if ! "$FID" emit "$P" --target "$arch-linux" -o "$img" >"$img.log" 2>&1; then
            bad "$arch-linux: emit failed ($(tail -1 "$img.log"))"
            continue
        fi
        chmod +x "$img"
        run="$img"
        if [ "$arch" != "$(uname -m)" ]; then
            command -v "qemu-$arch" >/dev/null 2>&1 || { echo "skip $arch-linux: no qemu-$arch"; continue; }
            run="qemu-$arch $img"
        fi
        got=$(timeout 60 $run 2>/dev/null | tr -d '\r')
        if [ "$got" = "$want" ]; then ok "$arch-linux: UDP peer and sysinfo"
        else bad "$arch-linux: output differs:"; echo "$got"; fi
    done
fi

if [ "$fails" -ne 0 ]; then
    echo "$fails failure(s)"
    exit 1
fi
echo "all MI16 UDP/sysinfo checks passed"
