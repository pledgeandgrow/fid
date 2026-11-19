#!/usr/bin/env bash
# irfuzz-targets.sh — RL10 (docs/roadmap/llvm-roadmap.md): random IR
# (`fid ir-fuzz`, scripts/fuzz/fuzz_ir.sh) on every machine-layer target,
# without and with the mid-end, with each failing seed's IR kept as a
# repro.
#
#   tests/tooling/irfuzz-targets.sh [count] [first_seed]
#     FID=<fid>       the compiler (default release/fid.exe, else release/fid)
#     TARGETS="..."   default: x86_64-linux aarch64-linux riscv64-linux
#                     x86_64-windows (the last only on a Windows host)
#     MIDEND=0        skip the --mid-end round
#
# On Windows the Linux targets run in WSL (fid.exe through interop; x86-64
# images natively, the others under qemu-<arch>), x86_64-windows here.
# A target with no way to run its images is checked as builds only (the
# log says "checking the builds only"); the table says so.
# For each failing seed, build/fuzz-ir/fail-<seed>.fir is copied to
# tests/verify/bugs/irfuzz/<target>[-mid]-<seed>.fir (replay:
# `fid emit-ir <file> --target <target> [--mid-end]`; record it in
# docs/verification/miscompile-log.md). Prints one table row per round,
# for docs/verification/rl10-irfuzz-2026-11-17.md. Exit 1 when a round
# fails.
set -u
cd "$(dirname "$0")/../.." || exit 1
COUNT="${1:-200}"
FIRST="${2:-1}"
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
on_windows=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) on_windows=1 ;; esac
if [ -z "${TARGETS:-}" ]; then
  TARGETS="x86_64-linux aarch64-linux riscv64-linux"
  [ "$on_windows" = 1 ] && TARGETS="$TARGETS x86_64-windows"
fi
OUT=build/irfuzz-targets
KEEP=tests/verify/bugs/irfuzz
mkdir -p "$OUT"
wsl_dir=""
if [ "$on_windows" = 1 ] && command -v wsl.exe > /dev/null; then
  wsl_dir="$(wsl.exe wslpath -a "$(pwd -W)" 2>/dev/null | tr -d '\r')"
fi
rounds="0"
[ "${MIDEND:-1}" != 0 ] && rounds="0 1"
status=0
echo "| target | mid-end | seeds | result |"
echo "|--------|---------|-------|--------|"
for t in $TARGETS; do
  for mid in $rounds; do
    tag="$t"
    midflag=""
    if [ "$mid" = 1 ]; then tag="$t-mid"; midflag=1; fi
    log="$OUT/$tag.log"
    if [ "$on_windows" = 1 ] && [ "${t%windows}" = "$t" ]; then
      if [ -z "$wsl_dir" ]; then
        echo "| $t | $mid | $FIRST..$((FIRST + COUNT - 1)) | not run: no WSL |"
        status=1
        continue
      fi
      wsl.exe bash -lc "cd '$wsl_dir' && FID='$FID' TARGET='$t' MIDEND='$midflag' bash scripts/fuzz/fuzz_ir.sh $COUNT $FIRST" > "$log" 2>&1
    else
      FID="$FID" TARGET="$t" MIDEND="$midflag" bash scripts/fuzz/fuzz_ir.sh "$COUNT" "$FIRST" > "$log" 2>&1
    fi
    rc=$?
    summary="$(tr -d '\r' < "$log" | grep '^fuzz_ir: [0-9]' | tail -1 | sed 's/^fuzz_ir: //')"
    [ -z "$summary" ] && summary="no summary (see $log)"
    if tr -d '\r' < "$log" | grep -q 'checking the builds only'; then
      summary="$summary; builds only"
    fi
    for seed in $(tr -d '\r' < "$log" | sed -n 's/^  FAIL  seed \([0-9][0-9]*\):.*/\1/p'); do
      if [ -f "build/fuzz-ir/fail-$seed.fir" ]; then
        mkdir -p "$KEEP"
        cp "build/fuzz-ir/fail-$seed.fir" "$KEEP/$tag-$seed.fir"
        summary="$summary; kept $KEEP/$tag-$seed.fir"
      fi
    done
    echo "| $t | $mid | $FIRST..$((FIRST + COUNT - 1)) | $summary |"
    [ "$rc" -ne 0 ] && status=1
  done
done
exit "$status"
