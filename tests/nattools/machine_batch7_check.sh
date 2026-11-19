#!/usr/bin/env bash
# Checks of the machine-layer goals TG14, TG17, SP6/SP20 and SP7
# (docs/roadmap/llvm-roadmap.md), run after the compiler is built:
#   bash tests/nattools/machine_batch7_check.sh [fid]   (default release/fid.exe)
# - TG14: tests/bootstrap/test_machine_pie.fd for x86-64, AArch64 and
#   RISC-V Linux: by default the image is two PT_LOADs, R+X (headers and
#   code) and R+W (data, from a page), with a non-executable PT_GNU_STACK
#   (tests/nattools/elf_segments_check.py); FID_NAT_ON=pie the same as
#   ET_DYN; FID_NAT_OFF=wxsplit one RWX PT_LOAD as before.
# - TG17: tests/bootstrap/test_machine_ldfwd.fd with --stats: a
#   "[ldfwd] N slot loads ..." line with N > 0 on every target, N = 0 with
#   FID_NAT_OFF=ldfwd; FID_VERIFY=1 on every build.
# - SP6/SP20: tests/bootstrap/test_machine_fncache.fd emitted twice with one
#   FID_FNCACHE file: the second build has hits ("[fncache] ... N hits")
#   and its image is byte-identical to the first's.
# - SP7: `fid serve` emitting the same program twice with FID_FNCACHE=mem:
#   the second build takes its entries from memory and writes the same
#   bytes as a fresh build.
# - every image that can run here (x86-64 Linux through WSL on Windows or
#   natively; AArch64 / RISC-V through QEMU_A64 / QEMU_RV, e.g.
#   "qemu-aarch64", when set) prints the test's expected lines.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-batch7"
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

build() {
    local img="$1" t="$2" tgt="$3"
    shift 3
    FID_VERIFY=1 "$FID" emit "$t" --target "$tgt" "$@" -o "$img" >"$img.log" 2>&1
}

run_check() {
    local r got
    r=$(runner "$4") || return 0
    local p="$2"
    if [ -n "$RUN" ] && [ "$r" = "$RUN" ]; then
        p=$(cd "$(dirname "$2")" && pwd)/$(basename "$2")
        p="/mnt/$(echo "$p" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
        got=$(MSYS_NO_PATHCONV=1 $r "$p" 2>&1)
    else
        got=$($r "$p" 2>&1)
    fi
    if [ "$got" = "$(expected "$3")" ]; then ok "$1 runs"; else bad "$1 runs (output differs)"; fi
}

TARGETS="x86_64-linux aarch64-linux riscv64-linux"

# --- TG14 -------------------------------------------------------------------
T=tests/bootstrap/test_machine_pie.fd
for tgt in $TARGETS; do
    img="$OUT/pie-$tgt"
    build "$img-split" "$T" "$tgt" || { bad "TG14 $tgt builds"; continue; }
    FID_NAT_ON=pie build "$img-pie" "$T" "$tgt" || { bad "TG14 $tgt builds (pie)"; continue; }
    FID_NAT_OFF=wxsplit build "$img-rwx" "$T" "$tgt" || { bad "TG14 $tgt builds (wxsplit off)"; continue; }
    if $PY tests/nattools/elf_segments_check.py --exec "$img-split" >/dev/null; then ok "TG14 $tgt: R+X code, R+W data, ET_EXEC"
    else bad "TG14 $tgt: $($PY tests/nattools/elf_segments_check.py --exec "$img-split" | head -1)"; fi
    if $PY tests/nattools/elf_segments_check.py --dyn "$img-pie" >/dev/null; then ok "TG14 $tgt (pie): R+X code, R+W data, ET_DYN"
    else bad "TG14 $tgt (pie): $($PY tests/nattools/elf_segments_check.py --dyn "$img-pie" | head -1)"; fi
    if $PY tests/nattools/elf_segments_check.py --rwx "$img-rwx" >/dev/null; then ok "TG14 $tgt (wxsplit off): one RWX segment"
    else bad "TG14 $tgt (wxsplit off): $($PY tests/nattools/elf_segments_check.py --rwx "$img-rwx" | head -1)"; fi
    run_check "TG14 $tgt" "$img-split" "$T" "$tgt"
    run_check "TG14 $tgt (pie)" "$img-pie" "$T" "$tgt"
    run_check "TG14 $tgt (wxsplit off)" "$img-rwx" "$T" "$tgt"
done

# --- TG17 -------------------------------------------------------------------
T=tests/bootstrap/test_machine_ldfwd.fd
ldfwd_n() { sed -n 's/^\[ldfwd\] \([0-9]*\) .*/\1/p' "$1" | tail -1; }
for tgt in $TARGETS; do
    img="$OUT/ldfwd-$tgt"
    build "$img-on" "$T" "$tgt" --stats || { bad "TG17 $tgt builds"; continue; }
    FID_NAT_OFF=ldfwd build "$img-off" "$T" "$tgt" --stats || { bad "TG17 $tgt builds (off)"; continue; }
    a=$(ldfwd_n "$img-on.log")
    b=$(ldfwd_n "$img-off.log")
    if [ -n "$a" ] && [ "$a" -gt 0 ]; then ok "TG17 $tgt: $a slot loads after a store became moves"
    else bad "TG17 $tgt: no load after store removed (${a:-no [ldfwd] line})"; fi
    if [ "${b:-x}" = 0 ]; then ok "TG17 $tgt (off): none"; else bad "TG17 $tgt (off): ${b:-no [ldfwd] line}"; fi
    if cmp -s "$img-on" "$img-off"; then bad "TG17 $tgt: the rule changed no byte"; fi
    run_check "TG17 $tgt" "$img-on" "$T" "$tgt"
    run_check "TG17 $tgt (off)" "$img-off" "$T" "$tgt"
done

# --- SP6 / SP20 -------------------------------------------------------------
T=tests/bootstrap/test_machine_fncache.fd
for tgt in $TARGETS; do
    img="$OUT/fnc-$tgt"
    rm -f "$img.cache"
    FID_FNCACHE="$img.cache" build "$img-miss" "$T" "$tgt" --stats || { bad "SP6 $tgt builds (miss)"; continue; }
    FID_FNCACHE="$img.cache" build "$img-hit" "$T" "$tgt" --stats || { bad "SP6 $tgt builds (hit)"; continue; }
    hits=$(sed -n 's/^\[fncache\] .*, \([0-9]*\) hits, .*/\1/p' "$img-hit.log" | tail -1)
    if [ -n "$hits" ] && [ "$hits" -gt 0 ]; then ok "SP6 $tgt: $hits hits"; else bad "SP6 $tgt: no hits (${hits:-no [fncache] line})"; fi
    if cmp -s "$img-miss" "$img-hit"; then ok "SP20 $tgt: hit image == miss image"
    else bad "SP20 $tgt: the hit image differs from the miss image"; fi
    run_check "SP6 $tgt (hit)" "$img-hit" "$T" "$tgt"
done

# --- SP7 --------------------------------------------------------------------
tgt=x86_64-linux
img="$OUT/serve-$tgt"
rm -f "$img-1" "$img-2" "$img.fresh.cache"
# (a fresh build under the cache: the reference for the server's builds,
# which run under the cache too; SP6 keeps struct results in registers,
# ng_rr_plan, under the cache, but a build without it may still lay the
# image out differently, SP20)
FID_FNCACHE="$img.fresh.cache" "$FID" emit "$T" --target "$tgt" -o "$img-fresh" >/dev/null 2>&1
printf 'emit %s --target %s --stats -o %s\nemit %s --target %s --stats -o %s\nquit\n' \
    "$T" "$tgt" "$img-1" "$T" "$tgt" "$img-2" | FID_FNCACHE=mem "$FID" serve >"$img.log" 2>&1
srcs=$(sed -n 's/^\[fncache\] entries from \([a-z]*\),.*/\1/p' "$img.log" | tr '\n' ' ')
case "$srcs" in
    "none memory "*) ok "SP7: the second build in one process took its entries from memory" ;;
    *) bad "SP7: entry sources '$srcs' (want: none memory)" ;;
esac
if [ -f "$img-2" ] && cmp -s "$img-2" "$img-fresh"; then ok "SP7: in-memory hit image == fresh build"
else bad "SP7: the in-memory hit image differs from a fresh build (or was not written)"; fi

if [ "$fails" -gt 0 ]; then
    echo "$fails check(s) failed"
    exit 1
fi
echo "all machine batch-7 checks passed"
