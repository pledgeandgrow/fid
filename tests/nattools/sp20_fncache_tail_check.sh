#!/usr/bin/env bash
# SP20 (docs/roadmap/llvm-roadmap.md): the per-function cache (FID_FNCACHE,
# compiler/nat_ir.fd CS6) gives the bytes a build without it gives, also
# where MI28's escape summaries decide the tail jumps. Before, a build with
# the cache used no summaries (every frame address escaped), so a cache-on
# image of a function that hands its frame's address to a clean callee had
# a call where the cache-off image had a jump. Now a replayed function's
# summary comes from its entry (ng_ft_esc_map, nc_note_ft) and a replayed
# caller whose callee's summary changed is compiled again (nc_deps_ok,
# nc_ft_hold, nc_give_up_callers).
#   bash tests/nattools/sp20_fncache_tail_check.sh [fid]   (default release/fid.exe)
# Per target (x86_64, aarch64, riscv64 Linux):
# 1. tests/nattools/sp20_tail_a.fd and test_machine_tailcall_frame.fd: a
#    cache miss and a cache hit are byte-identical to the build without
#    the cache.
# 2. sp20_tail_b.fd (only `look` differs: it keeps the address) is not
#    the same image as A (else the test proves nothing), and B built with
#    the cache A left (walk's entry read a clean summary of look, which no
#    longer holds) is byte-identical to B without the cache.
# 3. The images run and print their expected lines where they can (WSL /
#    qemu-user).
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-sp20-tail"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

WSL=""
if command -v wsl.exe >/dev/null 2>&1 && wsl.exe -e true >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

runner() {
    case "$1" in
        x86_64-linux)
            if [ -n "$WSL" ]; then echo "$WSL"
            elif [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; then echo ""
            else echo no; fi ;;
        aarch64-linux)
            if [ -n "$WSL" ]; then echo "$WSL qemu-aarch64"
            elif command -v qemu-aarch64 >/dev/null 2>&1; then echo qemu-aarch64
            else echo no; fi ;;
        riscv64-linux)
            if [ -n "$WSL" ]; then echo "$WSL qemu-riscv64"
            elif command -v qemu-riscv64 >/dev/null 2>&1; then echo qemu-riscv64
            else echo no; fi ;;
        *) echo no ;;
    esac
}

runs() {   # runs <image> <target>: its output, or return 2 when it cannot run here
    local r
    r=$(runner "$2")
    [ "$r" = no ] && return 2
    timeout 300 $r "$1" 2>&1 | tr -d '\r' | sed '/^wsl: /d'
}

emit() {   # emit <image> <program> <target> [VAR=value ...]
    # FID_IROPT_OFF=tailcall in every build: else the IR optimizer turns
    # walk's self-recursion into a loop and no machine-layer jump is left
    # to depend on look's summary
    local o="$1" p="$2" t="$3"
    shift 3
    env FID_IROPT_OFF=tailcall "$@" "$FID" emit "$p" --target "$t" -o "$o" > "$o.log" 2>&1
}

A=tests/nattools/sp20_tail_a.fd
B=tests/nattools/sp20_tail_b.fd
F=tests/bootstrap/test_machine_tailcall_frame.fd

for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    d="$OUT/$tgt"
    mkdir -p "$d"
    # 1. miss and hit = off
    for P in "$A" "$F"; do
        n=$(basename "$P" .fd)
        c="$d/$n.fncache"
        rm -f "$c"
        emit "$d/$n.off" "$P" "$tgt" || { bad "$tgt $n: build without the cache failed: $(tail -1 "$d/$n.off.log")"; continue; }
        emit "$d/$n.miss" "$P" "$tgt" FID_FNCACHE="$c" || { bad "$tgt $n: cache miss build failed"; continue; }
        emit "$d/$n.hit" "$P" "$tgt" FID_FNCACHE="$c" FID_VERIFY=1 || { bad "$tgt $n: cache hit build failed: $(tail -1 "$d/$n.hit.log")"; continue; }
        [ -s "$c" ] || bad "$tgt $n: no cache file was written (the compares prove nothing)"
        if cmp -s "$d/$n.off" "$d/$n.miss"; then ok "$tgt $n: cache miss = no cache"; else bad "$tgt $n: cache miss differs from no cache"; fi
        if cmp -s "$d/$n.off" "$d/$n.hit"; then ok "$tgt $n: cache hit = no cache"; else bad "$tgt $n: cache hit differs from no cache"; fi
        got=$(runs "$d/$n.hit" "$tgt")
        if [ $? -ne 2 ]; then
            if [ "$got" = "$(expected "$P")" ]; then ok "$tgt $n: the hit image runs"; else bad "$tgt $n: the hit image prints other lines"; fi
        fi
    done
    # 2. B with A's cache
    c="$d/ab.fncache"
    rm -f "$c"
    emit "$d/b.off" "$B" "$tgt" || { bad "$tgt B: build without the cache failed"; continue; }
    # the summaries matter here: without MI28's analysis (the old rule, no
    # jump in a function that takes its frame's address) A is another image
    emit "$d/a.notf" "$A" "$tgt" FID_NAT_OFF=tailframe || { bad "$tgt: A with FID_NAT_OFF=tailframe failed"; continue; }
    if cmp -s "$d/a.notf" "$d/sp20_tail_a.off"; then
        bad "$tgt: A is the same image with FID_NAT_OFF=tailframe (no jump depends on a summary: the test proves nothing)"
    else
        ok "$tgt: walk's jump in A depends on look's summary"
    fi
    emit "$d/a.seed" "$A" "$tgt" FID_FNCACHE="$c" || { bad "$tgt: A with the cache failed"; continue; }
    emit "$d/b.fromA" "$B" "$tgt" FID_FNCACHE="$c" FID_VERIFY=1 || { bad "$tgt: B with A's cache failed: $(tail -1 "$d/b.fromA.log")"; continue; }
    if cmp -s "$d/b.off" "$d/b.fromA"; then
        ok "$tgt: B with A's cache = B without the cache (walk compiled again)"
    else
        bad "$tgt: B with A's cache differs from B without it (a stale summary replayed?)"
    fi
    # and B again on its own cache: a hit
    emit "$d/b.hit" "$B" "$tgt" FID_FNCACHE="$c" || { bad "$tgt: B's hit build failed"; continue; }
    if cmp -s "$d/b.off" "$d/b.hit"; then ok "$tgt: B's cache hit = no cache"; else bad "$tgt: B's cache hit differs"; fi
    got=$(runs "$d/b.fromA" "$tgt")
    if [ $? -ne 2 ]; then
        if [ "$got" = "$(expected "$B")" ]; then ok "$tgt: B with A's cache runs"; else bad "$tgt: B with A's cache prints other lines"; fi
    fi
done

if [ "$fails" -gt 0 ]; then
    echo "$fails check(s) failed"
    exit 1
fi
echo "all SP20 cache / tail-summary checks passed"
