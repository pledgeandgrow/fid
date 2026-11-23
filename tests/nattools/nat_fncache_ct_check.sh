#!/usr/bin/env bash
# BG44 (docs/roadmap/bugs-roadmap.md): the machine layer's function cache
# (compiler/nat_ir.fd nc_skel_fn) replayed a @constant_time function
# without putting its label in the set of @constant_time labels
# (ng_gen_function's ct_labels), so a @constant_time caller compiled again
# in the same build failed the machine-level check: "call to a function
# that is not @constant_time".
#  1. tests/nattools/nat_fncache_ct_a.fd fills a cache file (FID_FNCACHE);
#  2. tests/nattools/nat_fncache_ct_b.fd (ct_sum the same, ct_check changed)
#     is built with it: the build succeeds, has hits ("[fncache] ...
#     N hits", N > 0), and does not say "not @constant_time";
#  3. where the image can run (x86-64 Linux natively or in WSL; AArch64 /
#     RISC-V when QEMU_A64 / QEMU_RV name a qemu-user), it prints B's
#     expected lines.
# Usage: tests/nattools/nat_fncache_ct_check.sh [fid]   Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-nat-fncache-ct"
mkdir -p "$OUT"
RUN=""
if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

runner() {
    case "$1" in
        x86_64-linux)
            if [ -n "$RUN" ]; then echo "$RUN"; return 0; fi
            if [ "$(uname -m)" = x86_64 ] && [ "$(uname -s)" = Linux ]; then echo "env"; return 0; fi ;;
        aarch64-linux) if [ -n "${QEMU_A64:-}" ]; then echo "$QEMU_A64"; return 0; fi ;;
        riscv64-linux) if [ -n "${QEMU_RV:-}" ]; then echo "$QEMU_RV"; return 0; fi ;;
    esac
    return 1
}

run_check() {
    local r got p
    r=$(runner "$4") || return 0
    p="$2"
    if [ -n "$RUN" ] && [ "$r" = "$RUN" ]; then
        # a Windows path (pwd -W: /tmp is not under /c/ in Git Bash) -> /mnt/<drive>/
        p=$(cd "$(dirname "$2")" && (pwd -W 2>/dev/null || pwd))/$(basename "$2")
        p=$(echo "$p" | sed -E 's|^([a-zA-Z]):/|/mnt/\L\1/|; s|^/([a-zA-Z])/|/mnt/\L\1/|')
        got=$(MSYS_NO_PATHCONV=1 $r "$p" 2>&1 | tr -d '\r' | sed '/^wsl: /d')
    else
        got=$($r "$p" 2>&1)
    fi
    if [ "$got" = "$(expected "$3")" ]; then ok "$1 runs"; else bad "$1 runs (output differs)"; fi
}

hits() { sed -n 's/^\[fncache\] .*, \([0-9]*\) hits, .*/\1/p' "$1" | tail -1; }

A=tests/nattools/nat_fncache_ct_a.fd
B=tests/nattools/nat_fncache_ct_b.fd
TARGETS="${TARGETS:-x86_64-linux aarch64-linux riscv64-linux}"
unset FID_FNCACHE FID_VERIFY FID_NAT_PAR FID_NAT_DUMP FIDES_NO_CACHE

# The cache's salt covers the input file's name (nc_cfg_salt "lines:"), as
# for one program edited in place: both versions are built from one path.
SRC="$OUT/ct.fd"

for tgt in $TARGETS; do
    img="$OUT/ct-$tgt"
    rm -f "$img.cache"
    cp "$A" "$SRC"
    if FID_FNCACHE="$img.cache" "$FID" emit "$SRC" --target "$tgt" --stats -o "$img-a" >"$img-a.log" 2>&1; then
        ok "$tgt: version A builds"
    else
        bad "$tgt: version A builds"
        continue
    fi
    run_check "$tgt version A" "$img-a" "$A" "$tgt"
    cp "$B" "$SRC"
    if FID_FNCACHE="$img.cache" "$FID" emit "$SRC" --target "$tgt" --stats -o "$img-b" >"$img-b.log" 2>&1; then
        ok "$tgt: version B builds with A's cache"
    else
        bad "$tgt: version B builds with A's cache ($(grep -m1 'constant_time' "$img-b.log"))"
        continue
    fi
    if grep -q "not @constant_time" "$img-b.log"; then
        bad "$tgt: version B: a cached @constant_time callee counted as not @constant_time"
    else
        ok "$tgt: version B: no @constant_time complaint"
    fi
    h=$(hits "$img-b.log")
    if [ -n "$h" ] && [ "$h" -gt 0 ]; then ok "$tgt: version B: $h hits"; else bad "$tgt: version B: no hits (${h:-no [fncache] line})"; fi
    run_check "$tgt version B" "$img-b" "$B" "$tgt"
done

if [ "$fails" -gt 0 ]; then
    echo "$fails failure(s)"
    exit 1
fi
echo "all passed"
