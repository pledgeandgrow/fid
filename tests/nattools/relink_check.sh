#!/usr/bin/env bash
# SP7 (docs/roadmap/llvm-roadmap.md; docs/reference/incremental-relink.md):
# incremental relinking in memory (compiler/nat_ir.fd nl_*). One `fid
# serve` builds tests/bootstrap/test_machine_relink.fd (A), then, from the
# same path, tests/nattools/relink_edit_b.fd (B), relink_edit_c.fd (C) and
# A again, each with --stats and FID_VERIFY=1, the server started with
# --relink (relinking is asked for since Nov 27: an image in the kept
# layout is not the one-shot build's, which SP8 wants by default). Run
# after the compiler is built:
#   bash tests/nattools/relink_check.sh [fid]     (default release/fid.exe)
# Per target (x86-64 Linux; AArch64 Linux; `--nat` Windows on Windows):
#  1. the first build lays its code out afresh ("[relink] a fresh layout
#     (no earlier build in this process)"); each later one uses the layout
#     of the one before ("[relink] the last build's layout: N functions in
#     place, M compiled again into their places, K placed after them"):
#     B with N > 0, M >= 2 (rl_shrink, rl_text), K >= 1 (rl_grow); C and
#     A again with N > 0, M >= 1 and K = 0 (everything fits its place);
#  2. every image that can run here prints what a fresh build of its source
#     (FID_FNCACHE=off, its own process) prints, with the same exit code,
#     and A's images print the test's expected lines;
#  3. ELF targets: the functions of A's image keep their addresses in B's
#     (tests/nattools/relink_syms.py), but for the ones the edit grew
#     (rl_grow; rl_mid and main may be compiled again); a fresh build of B
#     moves more of them (printed, for comparison);
#  4. x86-64: readelf decodes B's line program without a warning (the line
#     and frame rows went back into address order, nl_loop_end);
#  5. FID_NAT_OFF=relink: the second build lays its code out afresh and
#     still runs as a fresh build.
# Images run where they can: x86-64 Linux natively or in WSL, AArch64 when
# QEMU_A64 names a qemu-user (e.g. "qemu-aarch64"), Windows natively.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-relink"
# The paths go to `fid serve` on its stdin, where no shell converts them:
# on Windows a Git Bash path (/c/..., /tmp/...) is one the compiler cannot
# read, so the output folder is named the Windows way
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) OUT=$(cygpath -m "$OUT") ;; esac
mkdir -p "$OUT"
PY=python3
command -v python3 >/dev/null 2>&1 || PY=python
RUN=""
# (WSL with a Linux distribution: `wsl.exe -e true` fails without one)
if command -v wsl.exe >/dev/null 2>&1 && wsl.exe -e true >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

A=tests/bootstrap/test_machine_relink.fd
B=tests/nattools/relink_edit_b.fd
C=tests/nattools/relink_edit_c.fd

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

# runner <target>: the command prefix that runs its images here, or fail
runner() {
    case "$1" in
        x86_64-linux)
            if [ -n "$RUN" ]; then echo "$RUN"; return 0; fi
            if [ "$(uname -m)" = x86_64 ] && [ "$(uname -s)" = Linux ]; then echo "env"; return 0; fi ;;
        aarch64-linux) if [ -n "${QEMU_A64:-}" ]; then echo "$QEMU_A64"; return 0; fi ;;
        nat-windows)
            case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) echo "env"; return 0 ;; esac ;;
    esac
    return 1
}

flags_of() {
    case "$1" in
        nat-windows) echo "--nat" ;;
        *) echo "--target $1" ;;
    esac
}

# run_img <target> <image> <out file>: its output, then "exit N"
run_img() {
    local r
    r=$(runner "$1") || return 1
    local dir base
    dir=$(dirname "$2")
    base=$(basename "$2")
    (cd "$dir" && $r "./$base") >"$3" 2>&1
    echo "exit $?" >>"$3"
    return 0
}

# session <tag> <target> [VAR=value...]: one `fid serve` building A, B, C
# and A from one path ($OUT/<tag>-prog.fd) into $OUT/<tag>-img1..4
session() {
    local tag="$1" tgt="$2"
    shift 2
    local work="$OUT/$tag-prog.fd" log="$OUT/$tag.log" fl ext=""
    fl=$(flags_of "$tgt")
    [ "$tgt" = nat-windows ] && ext=".exe"
    rm -f "$log" "$OUT/$tag"-img*
    : >"$log"
    {
        local n=0 src t
        for src in "$A" "$B" "$C" "$A"; do
            n=$((n + 1))
            cp "$src" "$work"
            printf 'emit %s %s --stats -o %s\n' "$work" "$fl" "$OUT/$tag-img$n$ext"
            # this request's answer before the next edit
            t=0
            while [ "$(grep -c '^done ' "$log")" -lt "$n" ] && [ "$t" -lt 3600 ]; do
                sleep 1
                t=$((t + 1))
            done
        done
        printf 'quit\n'
    } | env FID_VERIFY=1 "$@" "$FID" serve --relink >>"$log" 2>&1
}

# relink_line <log> <n>: the n-th build's "[relink]" verdict
relink_line() {
    grep -E "^\[relink\] (a fresh layout|the last build's layout)" "$1" | sed -n "${2}p"
}

# counts <line>: "N M K" of a "the last build's layout" line
counts() {
    echo "$1" | sed -n "s/.*layout: \([0-9]*\) functions in place, \([0-9]*\) compiled again into their places, \([0-9]*\) placed after them.*/\1 \2 \3/p"
}

check_target() {
    local tgt="$1" tag ext=""
    tag="relink-$tgt"
    [ "$tgt" = nat-windows ] && ext=".exe"
    session "$tag" "$tgt"
    local log="$OUT/$tag.log"
    if [ "$(grep -c '^done 0' "$log")" -ne 4 ]; then
        bad "SP7 $tgt: not every request of the serve session built (see $log)"
        return
    fi
    # 1. the verdicts
    local l1 l2 l3 l4 c
    l1=$(relink_line "$log" 1)
    case "$l1" in
        *"a fresh layout (no earlier build in this process)"*) ok "SP7 $tgt: the first build lays its code out afresh" ;;
        *) bad "SP7 $tgt: first build: '$l1'" ;;
    esac
    l2=$(relink_line "$log" 2)
    c=$(counts "$l2")
    if [ -n "$c" ]; then
        set -- $c
        if [ "$1" -gt 0 ] && [ "$2" -ge 2 ] && [ "$3" -ge 1 ]; then
            ok "SP7 $tgt: edit B: $1 in place, $2 compiled again into their places, $3 placed after them"
        else
            bad "SP7 $tgt: edit B: '$l2' (want N > 0, M >= 2, K >= 1)"
        fi
    else
        bad "SP7 $tgt: edit B did not use the last build's layout: '$l2'"
    fi
    local k
    for k in 3 4; do
        l3=$(relink_line "$log" "$k")
        c=$(counts "$l3")
        if [ -n "$c" ]; then
            set -- $c
            if [ "$1" -gt 0 ] && [ "$2" -ge 1 ] && [ "$3" -eq 0 ]; then
                ok "SP7 $tgt: build $k: $1 in place, $2 compiled again into their places, nothing after them"
            else
                bad "SP7 $tgt: build $k: '$l3' (want N > 0, M >= 1, K = 0)"
            fi
        else
            bad "SP7 $tgt: build $k did not use the last build's layout: '$l3'"
        fi
    done
    # 2. each image runs as a fresh build of its source
    local srcs=("$A" "$B" "$C" "$A") i
    for i in 1 2 3 4; do
        local src="${srcs[$((i - 1))]}" fresh="$OUT/$tag-fresh$i$ext"
        FID_FNCACHE=off "$FID" emit "$src" $(flags_of "$tgt") -o "$fresh" >"$fresh.log" 2>&1 || { bad "SP7 $tgt: fresh build $i"; continue; }
        if run_img "$tgt" "$OUT/$tag-img$i$ext" "$OUT/$tag-img$i.out"; then
            run_img "$tgt" "$fresh" "$fresh.out"
            if cmp -s "$OUT/$tag-img$i.out" "$fresh.out"; then
                ok "SP7 $tgt: build $i runs as a fresh build"
            else
                bad "SP7 $tgt: build $i differs from a fresh build (diff $OUT/$tag-img$i.out $fresh.out)"
            fi
            if [ "$src" = "$A" ]; then
                expected "$A" >"$OUT/$tag.want"
                if diff <(sed '$d' "$OUT/$tag-img$i.out" | tr -d '\r') "$OUT/$tag.want" >/dev/null; then
                    ok "SP7 $tgt: build $i prints the expected lines"
                else
                    bad "SP7 $tgt: build $i output (see $OUT/$tag-img$i.out)"
                fi
            fi
        else
            echo "skip SP7 $tgt: build $i not run (no way to run $tgt here)"
        fi
    done
    # 3. addresses kept (ELF)
    if [ "$tgt" != nat-windows ]; then
        local cmpo moved name
        cmpo=$($PY tests/nattools/relink_syms.py "$OUT/$tag-img1" "$OUT/$tag-img2")
        echo "     $tgt A -> B in one process: $(echo "$cmpo" | head -1)"
        moved=$(echo "$cmpo" | sed -n 's/^moved \([^ ]*\) .*/\1/p')
        local extra=""
        for name in $moved; do
            case "$name" in rl_grow|rl_mid|main) ;; *) extra="$extra $name" ;; esac
        done
        if [ -z "$(echo "$cmpo" | head -1 | grep '^common [1-9]')" ]; then
            bad "SP7 $tgt: no function symbols to compare ($cmpo)"
        elif [ -z "$extra" ]; then
            ok "SP7 $tgt: every unchanged function kept its address"
        else
            bad "SP7 $tgt: functions moved that the edit did not change:$extra"
        fi
        echo "     $tgt A -> B fresh: $($PY tests/nattools/relink_syms.py "$OUT/$tag-fresh1" "$OUT/$tag-fresh2" | head -1)"
    fi
    # 4. the line program of B's image (x86-64 through WSL's readelf)
    if [ "$tgt" = x86_64-linux ] && [ -n "$RUN" ]; then
        local rd
        rd=$(cd "$OUT" && wsl.exe -e readelf --debug-dump=decodedline "$tag-img2" 2>&1)
        if [ $? -eq 0 ] && ! echo "$rd" | grep -qi "warning"; then
            ok "SP7 $tgt: readelf decodes the line program of B's image"
        else
            bad "SP7 $tgt: readelf on B's image: $(echo "$rd" | grep -i warning | head -3)"
        fi
    fi
}

check_off() {
    local tgt=x86_64-linux tag="relink-off"
    session "$tag" "$tgt" FID_NAT_OFF=relink
    local l2
    l2=$(relink_line "$OUT/$tag.log" 2)
    case "$l2" in
        *"a fresh layout (FID_NAT_OFF=relink)"*) ok "SP7 FID_NAT_OFF=relink: the second build lays its code out afresh" ;;
        *) bad "SP7 FID_NAT_OFF=relink: second build: '$l2'" ;;
    esac
    if run_img "$tgt" "$OUT/$tag-img2" "$OUT/$tag-img2.out"; then
        FID_FNCACHE=off "$FID" emit "$B" --target "$tgt" -o "$OUT/$tag-fresh2" >/dev/null 2>&1
        run_img "$tgt" "$OUT/$tag-fresh2" "$OUT/$tag-fresh2.out"
        if cmp -s "$OUT/$tag-img2.out" "$OUT/$tag-fresh2.out"; then ok "SP7 FID_NAT_OFF=relink: runs as a fresh build"
        else bad "SP7 FID_NAT_OFF=relink: differs from a fresh build"; fi
    fi
}

check_target x86_64-linux
check_target aarch64-linux
if runner nat-windows >/dev/null; then
    check_target nat-windows
fi
check_off

if [ "$fails" -gt 0 ]; then
    echo "$fails check(s) failed"
    exit 1
fi
echo "all SP7 relink checks passed"
