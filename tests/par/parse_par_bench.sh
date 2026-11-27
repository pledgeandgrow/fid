#!/usr/bin/env bash
# SP9: how long the import phase of `check compiler/main.fd` takes with the
# closure lexed and parsed ahead in N threads, and without (import cache
# off, so every module is parsed; and a cold import cache, the first build
# after a new compiler). Time on a quiet machine: other fid processes make
# every number meaningless.
#
#   FID=release/fid-dev.exe bash tests/par/parse_par_bench.sh [file.fd]
#
# Prints the `imports` rows of --time (imports; lexed ahead; parsed ahead;
# parses ahead used) and the total for FID_PAR unset, 2, 4, 8 and 16, then
# the parse phase alone from tests/par/parse_ahead_probe.fd.
set -u
FID="${FID:-release/fid.exe}"
F="${1:-compiler/main.fd}"
OUT="${OUT:-$(mktemp -d)}"
export FID_HOME="${FID_HOME:-$(pwd)}"

row() {
    grep -E '^\[time\] ((  )?imports|total)' "$1" | sed 's/^\[time\] /    /'
}

echo "== $F, import cache off"
for n in 1 2 4 8 16; do
    if [ $n -eq 1 ]; then
        FID_IMPCACHE=off "$FID" check "$F" --time > "$OUT/b$n.log" 2>&1
    else
        FID_IMPCACHE=off FID_PAR=$n "$FID" check "$F" --time > "$OUT/b$n.log" 2>&1
    fi
    echo "  FID_PAR=$n:"; row "$OUT/b$n.log"
done

echo "== $F, cold import cache"
for n in 1 8; do
    rm -rf "$OUT/ic$n"; mkdir -p "$OUT/ic$n"
    if [ $n -eq 1 ]; then
        FID_IMPCACHE_DIR="$OUT/ic$n" "$FID" check "$F" --time > "$OUT/c$n.log" 2>&1
    else
        FID_IMPCACHE_DIR="$OUT/ic$n" FID_PAR=$n "$FID" check "$F" --time > "$OUT/c$n.log" 2>&1
    fi
    echo "  FID_PAR=$n:"; row "$OUT/c$n.log"
done

echo "== the parse phase alone (parse_ahead_probe)"
if "$FID" ccompile tests/par/parse_ahead_probe.fd -o "$OUT/probe.exe" > "$OUT/probe.build" 2>&1; then
    for n in 1 2 4 8 16; do
        echo "  $("$OUT/probe.exe" "$F" $n | grep '^time')"
    done
else
    echo "  parse_ahead_probe does not build (see $OUT/probe.build)"
fi
