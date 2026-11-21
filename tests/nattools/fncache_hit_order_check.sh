#!/usr/bin/env bash
# 2g.82 (docs/reference/known-bugs.md; SP20, SP13, SP8, SP6 in
# docs/roadmap/llvm-roadmap.md): an image built from function-cache hits
# must be byte-identical to one built without the cache. A hit
# (compiler/nat_ir.fd nc_skel_fn) asked for its callees' labels (the call
# edges of its recorded machine IR, in layout order) before it replayed
# the labels its recording asked for (nc_log_replay, in recording order),
# so callees were queued, and placed, in another order: hello's hit image
# differed from the cache-off one in 25,528 bytes, from pe_dbg_free on.
# The replay now comes first.
#   bash tests/nattools/fncache_hit_order_check.sh [fid]   (default release/fid.exe)
# For each program and target: a build filling a fresh cache (miss), one
# reading it (hit, every function a hit) and one with FID_FNCACHE=off must
# be the same bytes; also with --debug (the debug sections). Exit 1 on a
# failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-fncache-order"
mkdir -p "$OUT"
unset FIDES_NO_CACHE FID_NAT_DUMP FID_VERIFY FID_NAT_PAR
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
PROGS="${PROGS:-tests/bootstrap/hello.fd tests/bootstrap/test_hashmap.fd tests/bootstrap/test_machine_fncache.fd tests/bootstrap/test_repro_fncache.fd}"
for src in $PROGS; do
    [ -f "$src" ] || { bad "$src: missing"; continue; }
    base=$(basename "$src" .fd)
    for tgt in x86_64-linux aarch64-linux riscv64-linux; do
        for mode in plain debug; do
            extra=()
            [ "$mode" = debug ] && extra=(--debug)
            img="$OUT/$base-$tgt-$mode"
            rm -f "$img.cache"
            if ! FID_FNCACHE="$img.cache" "$FID" emit "$src" --target "$tgt" "${extra[@]}" --stats -o "$img-miss" >"$img-miss.log" 2>&1; then
                bad "$base $tgt $mode: miss build failed"; continue
            fi
            if ! FID_FNCACHE="$img.cache" "$FID" emit "$src" --target "$tgt" "${extra[@]}" --stats -o "$img-hit" >"$img-hit.log" 2>&1; then
                bad "$base $tgt $mode: hit build failed"; continue
            fi
            if ! FID_FNCACHE=off "$FID" emit "$src" --target "$tgt" "${extra[@]}" -o "$img-off" >"$img-off.log" 2>&1; then
                bad "$base $tgt $mode: cache-off build failed"; continue
            fi
            hits=$(sed -n 's/^\[fncache\] entries from [a-z]*, \([0-9]*\) hits.*/\1/p' "$img-hit.log" | tail -1)
            if [ -z "$hits" ] || [ "$hits" -eq 0 ]; then
                bad "$base $tgt $mode: the second build has no hits (${hits:-no [fncache] line})"
            fi
            if cmp -s "$img-miss" "$img-off"; then ok "$base $tgt $mode: miss = off"; else bad "$base $tgt $mode: miss differs from off"; fi
            if cmp -s "$img-hit" "$img-off"; then
                ok "$base $tgt $mode: hit ($hits hits) = off"
            else
                bad "$base $tgt $mode: hit differs from off ($(cmp -l "$img-hit" "$img-off" 2>/dev/null | wc -l) bytes)"
            fi
        done
    done
done
echo "fncache_hit_order_check: $fails failed (logs in $OUT)"
[ $fails -eq 0 ]
