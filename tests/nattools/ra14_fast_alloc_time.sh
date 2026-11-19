#!/usr/bin/env bash
# RA14 (docs/roadmap/llvm-roadmap.md): how much of a build the register
# allocation of the machine layer takes, with the fast allocator of debug
# builds (`fid emit --fast-alloc`, `--debug`: compiler/nat_ir.fd
# g_nat_fast, ng_mir_alloc's `fast` path) and with the default one. Run
# after the compiler is built, on a quiet machine (other fid processes make
# every number meaningless):
#   bash tests/nattools/ra14_fast_alloc_time.sh [fid] [--self] [--runs N]
# For each program (below; --self adds compiler/main.fd, the self-compile)
# and each mode (default, --fast-alloc, --debug) it runs
# `fid emit <prog> --target x86_64-linux --time` N times (default 3) and
# keeps the fastest run, reading `[prof] native allocation: <ms>ms` (the
# time in ng_mir_alloc_split, nat_ir.fd nat_generate) and `[time] total:
# <ms> ms`. Prints one row per program and mode: allocation ms, total ms,
# the share, and the fast allocator's speed-up over the default one.
# The goal holds when the allocation of the fast modes (--fast-alloc and
# --debug) is under 10% of the total for every program; exit 1 when it is
# not, 2 when a build fails or a row is missing.
set -u
FID="release/fid.exe"
SELF=0
RUNS=3
while [ $# -gt 0 ]; do
    case "$1" in
        --self) SELF=1 ;;
        --runs) shift; RUNS="$1" ;;
        *) FID="$1" ;;
    esac
    shift
done
OUT="${TMPDIR:-/tmp}/fid-ra14"
mkdir -p "$OUT"
PROGS="tests/bootstrap/test_compiler_sized.fd tests/bootstrap/test_machine_probe.fd tests/bootstrap/test_toml_spec.fd tests/bootstrap/test_stdlib_calendar_lunar.fd tests/bootstrap/test_machine_shrinkwrap2.fd"
if [ "$SELF" = 1 ]; then PROGS="$PROGS compiler/main.fd"; fi
status=0
printf '%-48s %-12s %10s %10s %7s\n' program mode alloc_ms total_ms share
for p in $PROGS; do
    [ -f "$p" ] || { echo "missing $p"; status=2; continue; }
    base=$(basename "$p" .fd)
    def_al=""
    for mode in default fast-alloc debug; do
        best_al=""
        best_tot=""
        r=0
        while [ "$r" -lt "$RUNS" ]; do
            log="$OUT/$base-$mode-$r.log"
            case "$mode" in
                default) "$FID" emit "$p" --target x86_64-linux --time -o "$OUT/$base.elf" >"$log" 2>&1 ;;
                fast-alloc) "$FID" emit "$p" --target x86_64-linux --time --fast-alloc -o "$OUT/$base.elf" >"$log" 2>&1 ;;
                # (--no-cache: a cold build each time, not one replayed
                # from the debug build's function cache, main.fd
                # debug_cache_setup)
                debug) "$FID" emit "$p" --target x86_64-linux --time --debug --no-cache -o "$OUT/$base.elf" >"$log" 2>&1 ;;
            esac
            if [ $? -ne 0 ]; then
                echo "FAIL $base $mode: the build failed ($(tail -2 "$log" | tr '\n' ' '))"
                status=2
                break
            fi
            al=$(sed -n 's/^\[prof\] native allocation: \([0-9]*\)ms.*/\1/p' "$log" | tail -1)
            tot=$(sed -n 's/^\[time\] total: \([0-9]*\) ms.*/\1/p' "$log" | tail -1)
            if [ -z "$al" ] || [ -z "$tot" ]; then
                echo "FAIL $base $mode: no allocation or total row in $log"
                status=2
                break
            fi
            if [ "$mode" != default ] && ! grep -q '^\[prof\] native allocation: .*fast allocator' "$log"; then
                echo "FAIL $base $mode: the fast allocator did not run"
                status=1
            fi
            if [ -z "$best_tot" ] || [ "$tot" -lt "$best_tot" ]; then
                best_tot="$tot"
                best_al="$al"
            fi
            r=$((r + 1))
        done
        [ -n "$best_tot" ] || continue
        share=$(awk -v a="$best_al" -v t="$best_tot" 'BEGIN { if (t > 0) printf "%.1f%%", 100 * a / t; else print "-" }')
        extra=""
        if [ "$mode" = default ]; then
            def_al="$best_al"
        elif [ -n "$def_al" ] && [ "$best_al" -gt 0 ]; then
            extra=$(awk -v d="$def_al" -v a="$best_al" 'BEGIN { printf "  (%.1fx faster than the default allocator)", d / a }')
        fi
        printf '%-48s %-12s %10s %10s %7s%s\n' "$base" "$mode" "$best_al" "$best_tot" "$share" "$extra"
        if [ "$mode" != default ] && [ $((best_al * 10)) -ge "$best_tot" ]; then
            echo "FAIL $base $mode: allocation is not under 10% of the build"
            status=1
        fi
    done
done
if [ "$status" = 0 ]; then echo "RA14: the fast allocator takes under 10% of every build"; fi
exit $status
