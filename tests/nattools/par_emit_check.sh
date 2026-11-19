#!/usr/bin/env bash
# SP5 (docs/roadmap/llvm-roadmap.md): code generation by workers
# (compiler/nat_ir.fd ng_par_finish, switch FID_NAT_PAR). Run after the
# compiler is built:
#   bash tests/nattools/par_emit_check.sh [fid]   (default release/fid.exe)
# For each program and machine-layer target, the image built with
# FID_NAT_PAR=1 (workers on the main thread) and with FID_NAT_PAR=4 (on
# worker threads) must be byte-identical to the one built without it, and
# the build must say it joined worker buffers ("[par] FID_NAT_PAR: N
# functions joined from worker buffers, M emitted again in place (K
# threads; ...)", N > 0, K > 1 for the threaded build).
# test_par_emit.fd is also built five times on 8 threads (a race would
# show as a crash or a different image), with FID_VERIFY=1 on 4 threads
# (the workers' label tables checked after each function), with --stats
# (the same [shrinkwrap] and [ldfwd] lines as serially), with
# FID_NAT_PAR=0 (off: no [par] line), and run where the image runs
# (natively, WSL, qemu-aarch64 / qemu-riscv64): its output must be the
# test's expected lines. PAR_BIG=1 also builds compiler/main.fd for x86-64
# Linux serially and on 8 threads (identical images, both times printed).
# A target whose serial build fails here is reported and skipped.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-par-emit"
mkdir -p "$OUT"
WSL=""
if command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

PROGS=(
    tests/bootstrap/test_par_emit.fd
    tests/bootstrap/hello.fd
    tests/bootstrap/test_machine_rules_random.fd
    tests/bootstrap/test_vec_builtins.fd
)

# target, extra arguments, name
CONFIGS=(
    "x86_64-linux||x64"
    "x86_64-linux|--size|x64size"
    "nat||nat"
    "aarch64-linux||a64"
    "riscv64-linux||rv"
    "x86_64-macos||macx64"
    "aarch64-macos||maca64"
)

# build <image> <target> <prog> [args...]: environment from the caller
build() {
    local img="$1" tgt="$2" prog="$3"
    shift 3
    if [ "$tgt" = nat ]; then
        "$FID" emit "$prog" --nat "$@" -o "$img" >"$img.log" 2>&1
    else
        "$FID" emit "$prog" --target "$tgt" "$@" -o "$img" >"$img.log" 2>&1
    fi
}

runner() {
    case "$1" in
        nat) if [ "$WIN" = 1 ]; then echo ""; else echo no; fi ;;
        x86_64-linux)
            if [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; then echo ""
            elif [ -n "$WSL" ]; then echo "$WSL"
            else echo no; fi ;;
        aarch64-linux)
            if [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = aarch64 ]; then echo ""
            elif command -v qemu-aarch64 >/dev/null 2>&1; then echo qemu-aarch64
            else echo no; fi ;;
        riscv64-linux)
            if command -v qemu-riscv64 >/dev/null 2>&1; then echo qemu-riscv64; else echo no; fi ;;
        *) echo no ;;
    esac
}

# The worker threads write no shared state (a static scan of what
# ng_par_thread reaches; the type checker's race check does not see a
# module's spawns)
PY=python3
command -v python3 >/dev/null 2>&1 || PY=python
if $PY tests/nattools/par_emit_globals.py . --self-test; then
    ok "par_emit_globals.py: the workers write no module variable or shared map"
else
    bad "par_emit_globals.py found shared state the worker threads write"
fi

joined_count() { sed -n 's/^\[par\] FID_NAT_PAR: \([0-9]*\) functions joined.*/\1/p' "$1" | tail -1; }
threads_count() { sed -n 's/^\[par\] FID_NAT_PAR: .*(\([0-9]*\) threads.*/\1/p' "$1" | tail -1; }

for cfg in "${CONFIGS[@]}"; do
    IFS='|' read -r tgt extra name <<< "$cfg"
    for prog in "${PROGS[@]}"; do
        base=$(basename "$prog" .fd)
        s="$OUT/$base.$name.serial"
        p="$OUT/$base.$name.par"
        # shellcheck disable=SC2086
        if ! (unset FID_NAT_PAR; build "$s" "$tgt" "$prog" $extra); then
            echo "skip $base $name: the serial build failed (see $s.log)"
            continue
        fi
        # shellcheck disable=SC2086
        if ! FID_NAT_PAR=1 build "$p" "$tgt" "$prog" $extra; then
            bad "$base $name: the FID_NAT_PAR=1 build failed: $(grep -m1 -i 'error\|native backend' "$p.log")"
            continue
        fi
        n=$(joined_count "$p.log")
        if [ -z "$n" ] || [ "$n" -le 0 ]; then
            bad "$base $name: no worker buffer joined ([par] line: $(grep -m1 '^\[par\]' "$p.log"))"
        elif cmp -s "$s" "$p"; then
            ok "$base $name: identical image ($(grep -m1 '^\[par\]' "$p.log" | sed 's/^\[par\] FID_NAT_PAR: //'))"
        else
            bad "$base $name: the images differ (cmp: $(cmp "$s" "$p" 2>&1 | head -1))"
        fi
        # Worker threads (FID_NAT_PAR=<n>): the same image with 4 threads,
        # and the build says it used more than one
        t="$OUT/$base.$name.par4"
        # shellcheck disable=SC2086
        if ! FID_NAT_PAR=4 build "$t" "$tgt" "$prog" $extra; then
            bad "$base $name: the FID_NAT_PAR=4 build failed: $(grep -m1 -i 'error\|native backend\|panic' "$t.log")"
            continue
        fi
        nt=$(threads_count "$t.log")
        if [ -z "$nt" ] || [ "$nt" -le 1 ]; then
            bad "$base $name: FID_NAT_PAR=4 ran no worker thread ([par] line: $(grep -m1 '^\[par\]' "$t.log"))"
        elif cmp -s "$s" "$t"; then
            ok "$base $name: identical image on $nt threads"
        else
            bad "$base $name: the FID_NAT_PAR=4 image differs from the serial one (cmp: $(cmp "$s" "$t" 2>&1 | head -1))"
        fi
    done
    # test_par_emit.fd on 8 threads, five times: a race between the
    # threads would show as a crash or as images that differ
    prog=tests/bootstrap/test_par_emit.fd
    s="$OUT/test_par_emit.$name.serial"
    if [ -f "$s" ]; then
        same=1
        for rep in 1 2 3 4 5; do
            r8="$OUT/test_par_emit.$name.par8.$rep"
            # shellcheck disable=SC2086
            if ! FID_NAT_PAR=8 build "$r8" "$tgt" "$prog" $extra || ! cmp -s "$s" "$r8"; then
                same=0
                bad "test_par_emit $name: FID_NAT_PAR=8 run $rep failed or differs ($(grep -m1 -i 'error\|native backend\|panic' "$r8.log"))"
                break
            fi
        done
        [ "$same" = 1 ] && ok "test_par_emit $name: FID_NAT_PAR=8 five times, the serial image each time"
    fi
    # test_par_emit.fd: verified, off, and run
    prog=tests/bootstrap/test_par_emit.fd
    v="$OUT/test_par_emit.$name.verify"
    s="$OUT/test_par_emit.$name.serial"
    [ -f "$s" ] || continue
    # shellcheck disable=SC2086
    if FID_VERIFY=1 FID_NAT_PAR=4 build "$v" "$tgt" "$prog" $extra; then
        # shellcheck disable=SC2086
        (unset FID_NAT_PAR; FID_VERIFY=1 build "$v.s" "$tgt" "$prog" $extra)
        if cmp -s "$v" "$v.s"; then ok "test_par_emit $name: FID_VERIFY=1 identical (4 threads)"; else bad "test_par_emit $name: FID_VERIFY=1 images differ"; fi
    else
        bad "test_par_emit $name: FID_VERIFY=1 FID_NAT_PAR=4 build failed: $(grep -m1 -i 'error\|native backend' "$v.log")"
    fi
    # --stats: the workers' lines ([shrinkwrap], kept per function and
    # printed at the join) and the total of forwarded loads ([ldfwd]) are
    # the serial build's
    st="$OUT/test_par_emit.$name.stats"
    # shellcheck disable=SC2086
    (unset FID_NAT_PAR; build "$st.s" "$tgt" "$prog" --stats $extra)
    # shellcheck disable=SC2086
    FID_NAT_PAR=4 build "$st.p" "$tgt" "$prog" --stats $extra
    if [ "$(grep '^\[shrinkwrap\]\|^\[ldfwd\]' "$st.s.log" | sort)" = "$(grep '^\[shrinkwrap\]\|^\[ldfwd\]' "$st.p.log" | sort)" ]; then
        ok "test_par_emit $name: --stats lines as the serial build's"
    else
        bad "test_par_emit $name: --stats lines differ (diff $st.s.log $st.p.log)"
    fi
    # Mutation: FID_NAT_MUTATE=par joins every buffer, also those that
    # ng_par_redo sends back for emission in place; where the par build
    # emitted any again (x86-64: loop heads off the 16-byte alignment), the
    # image must then differ from the serial one (the comparison above can
    # see a wrong join)
    redo=$(sed -n 's/^\[par\] FID_NAT_PAR: [0-9]* functions joined from worker buffers, \([0-9]*\) emitted again.*/\1/p' "$OUT/test_par_emit.$name.par.log" | tail -1)
    if [ -n "$redo" ] && [ "$redo" -gt 0 ]; then
        mu="$OUT/test_par_emit.$name.mutant"
        # shellcheck disable=SC2086
        if FID_NAT_PAR=1 FID_NAT_MUTATE=par build "$mu" "$tgt" "$prog" $extra && ! cmp -s "$mu" "$s"; then
            ok "test_par_emit $name: FID_NAT_MUTATE=par ($redo functions joined that must not be) gives a different image"
        else
            bad "test_par_emit $name: FID_NAT_MUTATE=par built the serial image (or failed): the redo check is not needed?"
        fi
    else
        echo "skip test_par_emit $name: no function emitted again in place (mutation not applicable)"
    fi
    o="$OUT/test_par_emit.$name.off"
    # shellcheck disable=SC2086
    FID_NAT_PAR=0 build "$o" "$tgt" "$prog" $extra
    if grep -q '^\[par\]' "$o.log"; then bad "test_par_emit $name: FID_NAT_PAR=0 still joined buffers"; else ok "test_par_emit $name: FID_NAT_PAR=0 is off"; fi
    r=$(runner "$tgt")
    if [ "$r" = no ]; then
        echo "skip test_par_emit $name: cannot run the image here"
        continue
    fi
    img="$OUT/test_par_emit.$name.par"
    if [ "$tgt" = nat ]; then
        cp "$img" "$img.exe"
        img="$img.exe"
    fi
    chmod +x "$img" 2>/dev/null
    run_img="$img"
    if [ -n "$r" ] && [ "$r" = "$WSL" ]; then
        run_img=$(MSYS_NO_PATHCONV=1 wsl.exe -e wslpath -a "$(cygpath -w "$img")" | tr -d '\r')
    fi
    got=$(MSYS_NO_PATHCONV=1 $r "$run_img" 2>&1 | tr -d '\r')
    if [ "$got" = "$(expected "$prog")" ]; then ok "test_par_emit $name: output"; else bad "test_par_emit $name: output differs: $(echo "$got" | head -3 | tr '\n' '|')"; fi
done

# PAR_BIG=1: the compiler itself (compiler/main.fd) for x86-64 Linux, built
# serially and on 8 threads: the same image, and the times of both
if [ "${PAR_BIG:-0}" = 1 ]; then
    bs="$OUT/fid.serial"
    bp="$OUT/fid.par8"
    t0=$(date +%s)
    if (unset FID_NAT_PAR; build "$bs" x86_64-linux compiler/main.fd); then
        t1=$(date +%s)
        if FID_NAT_PAR=8 build "$bp" x86_64-linux compiler/main.fd; then
            t2=$(date +%s)
            if cmp -s "$bs" "$bp"; then
                ok "compiler/main.fd x64: identical image on 8 threads (serial $((t1 - t0)) s, 8 threads $((t2 - t1)) s; $(grep -m1 '^\[par\]' "$bp.log" | sed 's/^\[par\] FID_NAT_PAR: //'))"
            else
                bad "compiler/main.fd x64: the FID_NAT_PAR=8 image differs (cmp: $(cmp "$bs" "$bp" 2>&1 | head -1))"
            fi
        else
            bad "compiler/main.fd x64: the FID_NAT_PAR=8 build failed: $(grep -m1 -i 'error\|native backend\|panic' "$bp.log")"
        fi
    else
        echo "skip compiler/main.fd: the serial build failed (see $bs.log)"
    fi
fi

if [ "$fails" -gt 0 ]; then
    echo "$fails failure(s)"
    exit 1
fi
echo "all passed"
