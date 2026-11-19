#!/usr/bin/env bash
# SP11 (docs/roadmap/llvm-roadmap.md): the machine layer's register
# allocation of each function in an arena window (compiler/nat_ir.fd
# ng_mir_alloc -> ng_alloc_keep -> ng_mir_alloc_body, compiler/inc.fd
# fn_arena_*), on with FID_FN_ARENA=1 in a compiler built through C.
#   bash tests/tooling/fn-arena-emit-check.sh [fid]   (default release/fid.exe)
# Environment: BIG=<file.fd> (default compiler/main.fd), TARGET (default
# x86_64-linux), FULL=1 also runs scripts/cross-test.sh x86_64-linux with
# FID_FN_ARENA=1 FID_VERIFY=1 NATIVE=1.
#
#  1. `fid emit $BIG --target $TARGET --time` without and with
#     FID_FN_ARENA=1: both build, the two images are byte for byte the same,
#     and the build with arenas prints "[time] fn arenas: N windows, M MB
#     given back at their ends, 0 not back at their mark" with N > 0.
#     The wall time and peak memory of each are printed as a table; they
#     are measurements for the roadmap, not pass conditions.
#  2. Small programs with FID_VERIFY=1 and FID_FN_ARENA=1, on x86_64-linux,
#     aarch64-linux and riscv64-linux: each builds (the allocation checker
#     runs inside the window) and its image equals the one built without
#     arenas.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
BIG="${BIG:-compiler/main.fd}"
TARGET="${TARGET:-x86_64-linux}"
OUT="${TMPDIR:-/tmp}/fid-fn-arena-emit"
mkdir -p "$OUT"
cd "$(dirname "$0")/../.." || exit 1
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac

# measure <name> <out image> [env assignments...]: run `fid emit $BIG` with
# --time; writes $OUT/<name>.log, .ms (wall) and .peak (peak memory in KB,
# or "?" when it cannot be measured here)
measure() {
    local name="$1" img="$2"
    shift 2
    local t0 t1
    t0=$(date +%s%3N)
    if [ "$HOST" = windows ] && command -v powershell.exe >/dev/null 2>&1 && command -v cygpath >/dev/null 2>&1; then
        local fw lw ew iw bw
        fw=$(cygpath -w "$FID"); lw=$(cygpath -w "$OUT/$name.log"); ew=$(cygpath -w "$OUT/$name.err")
        iw=$(cygpath -w "$img"); bw=$(cygpath -w "$BIG")
        env "$@" powershell.exe -NoProfile -Command "\$p = Start-Process -FilePath '$fw' -ArgumentList 'emit','$bw','--target','$TARGET','--time','-o','$iw' -NoNewWindow -PassThru -RedirectStandardOutput '$lw' -RedirectStandardError '$ew'; \$peak = 0; while (-not \$p.HasExited) { try { \$p.Refresh(); if (\$p.PeakWorkingSet64 -gt \$peak) { \$peak = \$p.PeakWorkingSet64 } } catch {}; Start-Sleep -Milliseconds 100 }; \$p.WaitForExit(); [int64](\$peak / 1024)" | tr -d '\r' > "$OUT/$name.peak"
        cat "$OUT/$name.err" >> "$OUT/$name.log" 2>/dev/null
    elif [ -x /usr/bin/time ]; then
        env "$@" /usr/bin/time -f "%M" -o "$OUT/$name.peak" "$FID" emit "$BIG" --target "$TARGET" --time -o "$img" > "$OUT/$name.log" 2>&1
    else
        env "$@" "$FID" emit "$BIG" --target "$TARGET" --time -o "$img" > "$OUT/$name.log" 2>&1
        echo "?" > "$OUT/$name.peak"
    fi
    t1=$(date +%s%3N)
    echo $((t1 - t0)) > "$OUT/$name.ms"
}

measure plain "$OUT/big-plain" FID_FN_ARENA=0
measure arena "$OUT/big-arena" FID_FN_ARENA=1
if [ -s "$OUT/big-plain" ]; then ok "$BIG builds without arenas"; else bad "$BIG without arenas: $(tail -3 "$OUT/plain.log")"; fi
if [ -s "$OUT/big-arena" ]; then ok "$BIG builds with FID_FN_ARENA=1"; else bad "$BIG with FID_FN_ARENA=1: $(tail -3 "$OUT/arena.log")"; fi
if [ -s "$OUT/big-plain" ] && [ -s "$OUT/big-arena" ]; then
    if cmp -s "$OUT/big-plain" "$OUT/big-arena"; then ok "the two images are the same"; else bad "the image differs with FID_FN_ARENA=1"; fi
fi
far=$(grep -a '^\[time\] fn arenas:' "$OUT/arena.log" | tr -d '\r' | head -1)
nw=$(echo "$far" | sed -n 's/.*fn arenas: \([0-9]*\) windows.*/\1/p')
miss=$(echo "$far" | sed -n 's/.* \([0-9]*\) not back at their mark.*/\1/p')
if [ -n "$nw" ] && [ "$nw" -gt 0 ] && [ "$miss" = 0 ]; then ok "$far"; else bad "no sound window report with FID_FN_ARENA=1: '$far'"; fi
if grep -aq '^\[time\] fn arenas:' "$OUT/plain.log"; then bad "a window opened without FID_FN_ARENA"; else ok "no window without FID_FN_ARENA"; fi
echo "SP11 $BIG -> $TARGET:"
printf '  %-8s %10s %14s\n' build "wall ms" "peak KB"
printf '  %-8s %10s %14s\n' plain "$(cat "$OUT/plain.ms")" "$(cat "$OUT/plain.peak" 2>/dev/null)"
printf '  %-8s %10s %14s\n' arena "$(cat "$OUT/arena.ms")" "$(cat "$OUT/arena.peak" 2>/dev/null)"
grep -a '^\[time\] .*\(alloc\|native\|machine\)' "$OUT/arena.log" | head -5

SMALL="tests/bootstrap/test_machine_secret_regs.fd tests/bootstrap/test_natabi_wipe_secret.fd tests/bootstrap/test_machine_split_float.fd tests/bootstrap/test_machine_fcallee.fd"
for t in x86_64-linux aarch64-linux riscv64-linux; do
    for s in $SMALL; do
        [ -f "$s" ] || continue
        b=$(basename "$s" .fd)
        FID_VERIFY=1 FID_FN_ARENA=0 "$FID" emit "$s" --target "$t" -o "$OUT/$b-$t-plain" > "$OUT/$b-$t-plain.log" 2>&1
        if FID_VERIFY=1 FID_FN_ARENA=1 "$FID" emit "$s" --target "$t" -o "$OUT/$b-$t-arena" > "$OUT/$b-$t-arena.log" 2>&1 && [ -s "$OUT/$b-$t-arena" ]; then
            if cmp -s "$OUT/$b-$t-plain" "$OUT/$b-$t-arena"; then ok "$t $b: FID_VERIFY=1 with arenas, same image"; else bad "$t $b: image differs with FID_FN_ARENA=1"; fi
        else
            bad "$t $b: FID_VERIFY=1 FID_FN_ARENA=1 build: $(tail -3 "$OUT/$b-$t-arena.log")"
        fi
    done
done

if [ "${FULL:-0}" = 1 ]; then
    if FID_FN_ARENA=1 FID_VERIFY=1 NATIVE=1 FID="$FID" JOBS="${JOBS:-4}" bash scripts/cross-test.sh x86_64-linux > "$OUT/cross.log" 2>&1; then
        ok "cross-test x86_64-linux with FID_FN_ARENA=1 FID_VERIFY=1"
    else
        bad "cross-test x86_64-linux with FID_FN_ARENA=1 FID_VERIFY=1: $(tail -5 "$OUT/cross.log")"
    fi
fi

if [ "$fails" -gt 0 ]; then echo "$fails failure(s)"; exit 1; fi
echo "fn-arena-emit-check: all passed"
