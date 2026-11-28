#!/bin/bash
# BG467 (docs/roadmap/bugs-roadmap.md): under `fid run`, p_file_read_at
# reads only the bytes asked for (compiler/interp.fd, through the host's
# p_file_read_at). Before, it read the whole file into an i64 per byte:
# 10 bytes near the end of a 3 GiB file panicked "out of memory". The file
# is sparse (truncate), so it takes no disk space where the file system
# supports that; the check is skipped without truncate.
#   FID=release/fid.exe bash tests/tooling/bg467-read-at-large-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
if ! command -v truncate > /dev/null 2>&1; then echo "skip no truncate"; exit 0; fi
D=$(mktemp -d ./tests/_bg467_check.XXXXXX)
D=${D#./}
trap 'rm -rf "$D"' EXIT
fail=0
truncate -s 3G "$D/big.bin" || { echo "skip cannot make a 3 GiB file"; exit 0; }
cat > "$D/p.fd" <<FD
fn main() {
    val raw: i64 = p_file_read_at("$D/big.bin", 3221225470, 10) as i64;
    println(f"{Vec_len(raw)}");
    val raw2: i64 = p_file_read_at("$D/big.bin", 1073741824, 4) as i64;
    println(f"{Vec_len(raw2)} {Vec_get(raw2, 0)}");
}
FD
out=$(timeout 120 "$FID" run "$D/p.fd" 2>&1 | tr -d '\r')
if echo "$out" | grep -qx "2"; then echo "ok   2 bytes at the end of 3 GiB"; else echo "FAIL end of 3 GiB: $out"; fail=1; fi
if echo "$out" | grep -qx "4 0"; then echo "ok   4 bytes at 1 GiB"; else echo "FAIL 1 GiB: $out"; fail=1; fi
if echo "$out" | grep -qi "out of memory"; then echo "FAIL out of memory"; fail=1; fi
exit $fail
