#!/bin/bash
# release-checksums.sh — SHA-256 checksums of release files, to publish beside
# them on the GitHub release page.
#
#   scripts/release-checksums.sh [file ...]
#
# Without arguments it checksums the installer(s) in installer/ and the
# shipped binaries in release/ (fid.exe, fidesc.exe, ark.exe, scribe.exe,
# veil.exe) that exist. Writes SHA256SUMS.txt at the repository root (file
# names without directories) and prints it.
#
# Users check a download with:
#   sha256sum -c SHA256SUMS.txt --ignore-missing          (Linux, Git Bash)
#   Get-FileHash Fid-Setup-1.0.0.exe -Algorithm SHA256  (PowerShell)
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

files=("$@")
if [ ${#files[@]} -eq 0 ]; then
    for f in installer/Fid-Setup-*.exe installer/Fides-Setup-*.exe release/fid.exe release/fidesc.exe \
             release/ark.exe release/scribe.exe release/veil.exe; do
        [ -f "$f" ] && files+=("$f")
    done
fi
if [ ${#files[@]} -eq 0 ]; then
    echo "release-checksums: no files to checksum" >&2
    exit 1
fi

if command -v sha256sum &> /dev/null; then
    SUM="sha256sum"
elif command -v shasum &> /dev/null; then
    SUM="shasum -a 256"
else
    echo "release-checksums: need sha256sum or shasum" >&2
    exit 1
fi

out="SHA256SUMS.txt"
: > "$out"
for f in "${files[@]}"; do
    # name only, so the file checks against downloads in one folder
    (cd "$(dirname "$f")" && $SUM "$(basename "$f")") >> "$out"
done
echo "== $out"
cat "$out"
