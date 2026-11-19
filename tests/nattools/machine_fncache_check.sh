#!/usr/bin/env bash
# SP6 (docs/roadmap/llvm-roadmap.md; docs/reference/native-fncache.md): the
# per-function native cache of the machine layer (compiler/nat_ir.fd CS6)
# on by default, with struct results in registers (ng_rr_plan) and RISC-V
# short calls under it, and hits that hold when what they call changes.
#  1. test_machine_fncache_deps.fd, test_machine_fncache.fd and
#     test_repro_fncache.fd, per target, with one FID_FNCACHE file: a miss
#     and a hit build are byte-identical, the hit build has hits
#     ("[fncache] ... N hits", N > 0), and the hit image prints the
#     expected lines.
#  2. Struct results in registers under the cache: the deps test built
#     with the cache differs from the same build with FID_NAT_OFF=retregs
#     (before SP6 the cache turned them off: the two were the same).
#  3. RISC-V short calls under the cache: the deps test's image has more
#     JAL ra words than with FID_NAT_OFF=relax (before SP6: the same, none).
#  4. On by default: `fid emit -o X` (no FID_FNCACHE, no --stats, no
#     FID_VERIFY) writes X.fides-fncache, and a second such build is the
#     same bytes; FID_FNCACHE=off and --no-fncache write no file.
#  5. An edit: tests/nattools/nat_fncache_edit_a.fd fills a cache file and
#     nat_fncache_edit_b.fd (same functions; pick names "mk" in a string,
#     inner keeps many values in registers) is built with it. The build
#     notices what no longer holds ("[fncache] built again without: ..."
#     names mk, whose result in registers its name now forbids), has hits
#     still, and prints B's expected lines, as a build without the cache.
# Images run where they can: x86-64 Linux natively or in WSL, AArch64 /
# RISC-V when QEMU_A64 / QEMU_RV name a qemu-user (e.g. "qemu-aarch64").
# Usage: tests/nattools/machine_fncache_check.sh [fid]   Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-fncache"
mkdir -p "$OUT"
PY=python3
command -v python3 >/dev/null 2>&1 || PY=python
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

# build <image> <program> <target> [flags...]: no cache variable of the
# caller's leaks in (FID_FNCACHE is set per call where wanted)
build() {
    local img="$1" t="$2" tgt="$3"
    shift 3
    "$FID" emit "$t" --target "$tgt" "$@" -o "$img" >"$img.log" 2>&1
}

run_check() {
    local r got
    r=$(runner "$4") || return 0
    local p="$2"
    if [ -n "$RUN" ] && [ "$r" = "$RUN" ]; then
        p=$(cd "$(dirname "$2")" && pwd)/$(basename "$2")
        p="/mnt/$(echo "$p" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
        got=$(MSYS_NO_PATHCONV=1 $r "$p" 2>&1 | tr -d '\r' | sed '/^wsl: /d')
    else
        got=$($r "$p" 2>&1)
    fi
    if [ "$got" = "$(expected "$3")" ]; then ok "$1 runs"; else bad "$1 runs (output differs)"; fi
}

hits() { sed -n 's/^\[fncache\] .*, \([0-9]*\) hits, .*/\1/p' "$1" | tail -1; }

# JAL ra words (RISC-V short calls, TG6): 32-bit words at any 2-byte offset
# (RVC) whose low 12 bits are 0x0EF
jal_ra() {
    $PY - "$1" <<'EOF'
import sys
b = open(sys.argv[1], 'rb').read()
n = 0
for i in range(0, len(b) - 3, 2):
    w = b[i] | (b[i + 1] << 8)
    if (w & 0xFFF) == 0x0EF:
        n += 1
print(n)
EOF
}

TARGETS="${TARGETS:-x86_64-linux aarch64-linux riscv64-linux}"
unset FID_FNCACHE FID_VERIFY FID_NAT_PAR FID_NAT_DUMP FIDES_NO_CACHE

# --- 1, 2, 3: miss / hit, the conventions under the cache --------------------
for T in tests/bootstrap/test_machine_fncache_deps.fd tests/bootstrap/test_machine_fncache.fd tests/bootstrap/test_repro_fncache.fd; do
    b=$(basename "$T" .fd)
    for tgt in $TARGETS; do
        img="$OUT/$b-$tgt"
        rm -f "$img.cache" "$img.rr.cache" "$img.relax.cache"
        FID_FNCACHE="$img.cache" build "$img-miss" "$T" "$tgt" --stats || { bad "$b $tgt builds (miss)"; continue; }
        FID_FNCACHE="$img.cache" build "$img-hit" "$T" "$tgt" --stats || { bad "$b $tgt builds (hit)"; continue; }
        h=$(hits "$img-hit.log")
        if [ -n "$h" ] && [ "$h" -gt 0 ]; then ok "$b $tgt: $h hits"; else bad "$b $tgt: no hits (${h:-no [fncache] line})"; fi
        if cmp -s "$img-miss" "$img-hit"; then ok "$b $tgt: hit image == miss image"
        else bad "$b $tgt: the hit image differs from the miss image"; fi
        run_check "$b $tgt (miss)" "$img-miss" "$T" "$tgt"
        run_check "$b $tgt (hit)" "$img-hit" "$T" "$tgt"
        if [ "$b" = test_machine_fncache_deps ]; then
            FID_FNCACHE="$img.rr.cache" FID_NAT_OFF=retregs build "$img-norr" "$T" "$tgt" || { bad "$b $tgt builds (retregs off)"; continue; }
            if cmp -s "$img-miss" "$img-norr"; then bad "$b $tgt: same image with FID_NAT_OFF=retregs: no struct result in registers under the cache"
            else ok "$b $tgt: struct results in registers under the cache"; fi
            run_check "$b $tgt (retregs off)" "$img-norr" "$T" "$tgt"
            if [ "$tgt" = riscv64-linux ]; then
                FID_FNCACHE="$img.relax.cache" FID_NAT_OFF=relax build "$img-norelax" "$T" "$tgt" || { bad "$b $tgt builds (relax off)"; continue; }
                a=$(jal_ra "$img-miss")
                z=$(jal_ra "$img-norelax")
                if [ "$a" -gt "$z" ]; then ok "$b $tgt: $a JAL ra words under the cache, $z without relaxation"
                else bad "$b $tgt: $a JAL ra words under the cache, $z without relaxation (no short calls)"; fi
            fi
        fi
    done
done

# --- 4: on by default --------------------------------------------------------
T=tests/bootstrap/test_machine_fncache_deps.fd
for tgt in $TARGETS; do
    img="$OUT/default-$tgt"
    rm -f "$img-1" "$img-1.fides-fncache" "$img-2" "$img-off" "$img-off.fides-fncache" "$img-nof" "$img-nof.fides-fncache"
    build "$img-1" "$T" "$tgt" || { bad "default $tgt builds"; continue; }
    if [ -s "$img-1.fides-fncache" ]; then ok "default $tgt: the cache file is beside the output"
    else bad "default $tgt: no $img-1.fides-fncache"; fi
    cp "$img-1.fides-fncache" "$img-2.fides-fncache" 2>/dev/null
    build "$img-2" "$T" "$tgt" || { bad "default $tgt builds (second)"; continue; }
    if cmp -s "$img-1" "$img-2"; then ok "default $tgt: a build from the default cache is the same bytes"
    else bad "default $tgt: the second default build differs"; fi
    run_check "default $tgt" "$img-2" "$T" "$tgt"
    FID_FNCACHE=off build "$img-off" "$T" "$tgt" || bad "default $tgt builds with FID_FNCACHE=off"
    if [ -e "$img-off.fides-fncache" ]; then bad "default $tgt: FID_FNCACHE=off wrote a cache file"; else ok "default $tgt: FID_FNCACHE=off: no cache file"; fi
    build "$img-nof" "$T" "$tgt" --no-fncache || bad "default $tgt builds with --no-fncache"
    if [ -e "$img-nof.fides-fncache" ]; then bad "default $tgt: --no-fncache wrote a cache file"; else ok "default $tgt: --no-fncache: no cache file"; fi
    run_check "default $tgt (off)" "$img-off" "$T" "$tgt"
done

# --- 5: an edit --------------------------------------------------------------
A=tests/nattools/nat_fncache_edit_a.fd
B=tests/nattools/nat_fncache_edit_b.fd
for tgt in $TARGETS; do
    img="$OUT/edit-$tgt"
    rm -f "$img.cache"
    FID_FNCACHE="$img.cache" build "$img-a" "$A" "$tgt" --stats || { bad "edit $tgt: A builds"; continue; }
    run_check "edit $tgt (A)" "$img-a" "$A" "$tgt"
    FID_FNCACHE="$img.cache" build "$img-b" "$B" "$tgt" --stats || { bad "edit $tgt: B builds with A's cache"; continue; }
    h=$(hits "$img-b.log")
    if [ -n "$h" ] && [ "$h" -gt 0 ]; then ok "edit $tgt: B has $h hits from A's cache"; else bad "edit $tgt: B has no hits (${h:-no [fncache] line})"; fi
    again=$(sed -n 's/^\[fncache\] built again without: //p' "$img-b.log" | tail -1)
    case ", $again," in
        *", mk,"*) ok "edit $tgt: mk given up (its name is now in a string): $again" ;;
        *) bad "edit $tgt: mk not given up (built again without: '${again}')" ;;
    esac
    run_check "edit $tgt (B on A's cache)" "$img-b" "$B" "$tgt"
    FID_FNCACHE=off build "$img-b-off" "$B" "$tgt" || { bad "edit $tgt: B builds without the cache"; continue; }
    run_check "edit $tgt (B without the cache)" "$img-b-off" "$B" "$tgt"
done

if [ "$fails" -gt 0 ]; then
    echo "$fails check(s) failed"
    exit 1
fi
echo "all function cache checks passed"
