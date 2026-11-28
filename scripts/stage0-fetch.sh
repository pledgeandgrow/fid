#!/bin/bash
# stage0-fetch.sh — download the pinned self-hosted stage-0 fid for this
# host into release/stage0/, verified against scripts/stage0.<asset>.sha256.
#
# Why: bootstrap.sh / build-selfhost.sh start stage 0 from a pinned
# self-hosted fid when it exists, else fall back to fidesc — and a
# fidesc-built fid cannot self-compile the tree (known-bugs 2g.79, the
# >16 GB arena wall). The binaries are gitignored; they ship as assets on
# the fixed `stage0` GitHub release, one per CI host platform.
#
# Usage:
#   ./scripts/stage0-fetch.sh            # fetch this host's binary
#   FID_STAGE0_REF=tag ./scripts/stage0-fetch.sh   # fetch from another ref
#
# Assets (built by scripts/stage0-build-all.sh):
#   fid.exe                windows-x64
#   fid-windows-arm64.exe  windows-arm64
#   fid-linux-x64          linux-x64
#   fid-linux-arm64        linux-arm64
#   fid-macos-x64          macos-x64
#   fid-macos-arm64        macos-arm64
set -eu
cd "$(dirname "$0")/.."
REF="${FID_STAGE0_REF:-stage0}"
DEST="release/stage0"
mkdir -p "$DEST"

os="$(uname -s)"; arch="$(uname -m)"
case "$os" in
  MINGW*|MSYS*|CYGWIN*) osk=windows ;;
  Darwin)               osk=macos ;;
  *)                    osk=linux ;;
esac
case "$arch" in
  x86_64|AMD64|amd64)   archk=x64 ;;
  aarch64|arm64|ARM64)  archk=arm64 ;;
  *) echo "stage0-fetch: unsupported arch $arch"; exit 1 ;;
esac
# On Windows runners git-bash itself may be the x64 build under emulation,
# so uname -m lies; PROCESSOR_ARCHITECTURE is the real machine arch.
if [ "$osk" = windows ] && [ "${PROCESSOR_ARCHITECTURE:-}" = "ARM64" ]; then
  archk=arm64
fi
if [ "$osk" = windows ] && [ "$archk" = x64 ]; then
  asset=fid.exe
else
  asset="fid-$osk-$archk"
  [ "$osk" = windows ] && asset="$asset.exe"
fi

pin="scripts/$asset.sha256"
if [ ! -f "$pin" ]; then
  # windows-x64 keeps the historical name: scripts/stage0.sha256 pins fid.exe
  [ "$asset" = fid.exe ] && pin="scripts/stage0.sha256" || {
    [ "${1:-}" = --pin-file ] && { echo "$pin"; exit 1; }
    echo "stage0-fetch: no pin file $pin"; exit 1; }
fi
# --pin-file: only print the pin file that applies to this host
# (bootstrap.sh checks release/stage0/fid against it, BG359)
if [ "${1:-}" = --pin-file ]; then
  echo "$pin"
  exit 0
fi
want=$(cut -d' ' -f1 "$pin")
[ -n "$want" ] || { echo "stage0-fetch: empty pin $pin"; exit 1; }

sha256_of() {
  if command -v sha256sum >/dev/null; then sha256sum "$1" | cut -d' ' -f1
  elif command -v shasum >/dev/null; then shasum -a 256 "$1" | cut -d' ' -f1
  fi
}
# Canonical name the scripts look for: release/stage0/fid (POSIX) or
# fid.exe (Windows). The x64 Windows asset already is fid.exe.
canon="$DEST/fid"; [ "$osk" = windows ] && canon="$DEST/fid.exe"

out="$DEST/$asset"
if [ -x "$out" ]; then
  have=$(sha256_of "$out")
  if [ -n "$have" ] && [ "$have" = "$want" ]; then
    # BG359: the canonical copy is what bootstrap.sh runs: make sure it is
    # this binary too (it was left missing or stale)
    if [ "$out" != "$canon" ] && { [ ! -x "$canon" ] || [ "$(sha256_of "$canon")" != "$want" ]; }; then
      cp "$out" "$canon" && chmod +x "$canon" 2>/dev/null || true
    fi
    cp "$pin" "$canon.sha256"
    echo "stage0-fetch: $out already matches pin"
    exit 0
  fi
fi

url="https://github.com/pledgeandgrow/fid/releases/download/$REF/$asset"
echo "stage0-fetch: $url -> $out"
if command -v curl >/dev/null; then
  curl -fsSL "$url" -o "$out"
elif command -v wget >/dev/null; then
  wget -qO "$out" "$url"
else
  echo "stage0-fetch: need curl or wget"; exit 1
fi
chmod +x "$out" 2>/dev/null || true

have=$(sha256_of "$out")
if [ -z "$have" ]; then
  echo "stage0-fetch: no sha256 tool; cannot verify"; exit 1
fi
if [ "$have" != "$want" ]; then
  echo "stage0-fetch: HASH MISMATCH for $asset"
  echo "  want $want"
  echo "  have $have"
  rm -f "$out"
  exit 1
fi

if [ "$out" != "$canon" ]; then
  cp "$out" "$canon" && chmod +x "$canon" 2>/dev/null || true
fi
# Record the pin that verified this binary: bootstrap.sh's hash check uses
# it instead of guessing (stage0.sha256 pins only the windows-x64 fid.exe).
cp "$pin" "$canon.sha256"
echo "stage0-fetch: ok ($have)"
