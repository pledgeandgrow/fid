#!/usr/bin/env bash
# SP11 (docs/roadmap/llvm-roadmap.md): arenas for one function's compiler
# data, given back in one step (compiler/inc.fd fn_arena_*).
#   bash tests/tooling/fn-arena-check.sh [fid]   (default release/fid.exe)
# Builds tests/tooling/fn_arena_unit.fd through C and checks:
# - without FID_FN_ARENA: the expected lines, no window opens;
# - with FID_FN_ARENA=1: the same lines, every window (321) opened and back
#   at its mark, and the --time style report line;
# - `fid run` (no allocator marks in the interpreter): the same lines;
# - the machine layer (`fid emit`: pe_ir on Windows, x86_64-linux
#   elsewhere): the same lines with FID_FN_ARENA=1, no window opens.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-fn-arena"
mkdir -p "$OUT"
cd "$(dirname "$0")/../.." || exit 1
T=tests/tooling/fn_arena_unit.fd
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
expected "$T" > "$OUT/expected.txt"

EXT=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) EXT=.exe ;; esac

if "$FID" ccompile "$T" -o "$OUT/fa$EXT" > "$OUT/build.log" 2>&1 && [ -s "$OUT/fa$EXT" ]; then
    env -u FID_FN_ARENA "$OUT/fa$EXT" > "$OUT/plain.txt" 2> "$OUT/plain.err"
    if diff "$OUT/expected.txt" <(tr -d '\r' < "$OUT/plain.txt") > /dev/null && grep -q "^windows: 0 opened" "$OUT/plain.err"; then
        ok "C path, arenas off: expected lines, no window"
    else
        bad "C path, arenas off: $(head -c 300 "$OUT/plain.txt") $(head -c 200 "$OUT/plain.err")"
    fi
    FID_FN_ARENA=1 "$OUT/fa$EXT" > "$OUT/arena.txt" 2> "$OUT/arena.err"
    if diff "$OUT/expected.txt" <(tr -d '\r' < "$OUT/arena.txt") > /dev/null \
        && grep -q "^windows: 321 opened, 321 back at their mark" "$OUT/arena.err" \
        && grep -q "^fn arenas: 321 windows, .* 0 not back at their mark" "$OUT/arena.err"; then
        ok "C path, FID_FN_ARENA=1: same lines, 321 windows back at their mark"
    else
        bad "C path, FID_FN_ARENA=1: $(head -c 300 "$OUT/arena.txt") $(head -c 300 "$OUT/arena.err")"
    fi
else
    bad "fn_arena_unit does not build through C: $(tail -3 "$OUT/build.log")"
fi

FID_FN_ARENA=1 "$FID" run "$T" > "$OUT/run.txt" 2> "$OUT/run.err"
if diff "$OUT/expected.txt" <(tr -d '\r' < "$OUT/run.txt" | grep -v -e '^Built native executable' -e '^Reused cached artifact') > /dev/null; then
    ok "fid run: same lines"
else
    bad "fid run: $(head -c 300 "$OUT/run.txt") $(tail -c 200 "$OUT/run.err")"
fi

if [ -n "$EXT" ]; then EARGS=(); else EARGS=(--target x86_64-linux); fi
if [ -n "$EXT" ] || [ "$(uname -m)" = x86_64 ]; then
    if "$FID" emit "$T" "${EARGS[@]}" -o "$OUT/fa_nat$EXT" > "$OUT/nat.log" 2>&1 && [ -s "$OUT/fa_nat$EXT" ]; then
        FID_FN_ARENA=1 "$OUT/fa_nat$EXT" > "$OUT/nat.txt" 2> "$OUT/nat.err"
        if diff "$OUT/expected.txt" <(tr -d '\r' < "$OUT/nat.txt") > /dev/null && grep -q "^windows: 0 opened" "$OUT/nat.err"; then
            ok "emit ${EARGS[*]}: same lines, no window (no allocator marks)"
        else
            bad "emit ${EARGS[*]}: $(head -c 300 "$OUT/nat.txt") $(head -c 200 "$OUT/nat.err")"
        fi
    else
        bad "fn_arena_unit does not build with emit ${EARGS[*]}: $(tail -3 "$OUT/nat.log")"
    fi
fi

echo "fn-arena-check: $fails failure(s)"
[ $fails = 0 ]
