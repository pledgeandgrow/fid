#!/bin/bash
# pgo-bench.sh — OP30 (competitive roadmap): what profile-guided
# optimization buys. tests/tooling/pgo_bench.fd is profiled through the
# interpreter with a small count (`fid run --profile`, block counts
# included), built twice with `fid emit` (without and with `--profile-use`)
# and each build timed with a large count, best of RUNS.
#
# Usage: tests/tooling/pgo-bench.sh
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/pgobench
#   TARGET=<flags>   emit flags; default `--nat` on Windows, the machine
#                    layer's Linux target on Linux (`--target x86_64-linux`)
#   N=<iterations>   timed count, default 100000000
#   PN=<iterations>  profiled count, default 5000
#   RUNS=<n>         default 5
#   MIN=<ratio>      the speedup (time without / time with) to pass,
#                    default 1.00 (the profile build must not be slower)
#
# Prints the best time of each build and the speedup. Exit status: 1 when
# a build fails, the two builds print different checksums, or the speedup
# is under MIN; else 0. Time on a quiet machine (AGENTS.md: other fid
# processes make timings meaningless).
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/pgobench}"
N="${N:-100000000}"
PN="${PN:-5000}"
RUNS="${RUNS:-5}"
MIN="${MIN:-1.00}"
if [ -z "${TARGET+x}" ]; then
  TARGET="--nat"
  case "$(uname -s)" in
    Linux) case "$(uname -m)" in
             x86_64) TARGET="--target x86_64-linux" ;;
             aarch64|arm64) TARGET="--target aarch64-linux" ;;
           esac ;;
  esac
fi
mkdir -p "$OUT"
B=tests/tooling/pgo_bench.fd
P="$OUT/bench.prof"
rm -f "$P" "$OUT/plain.exe" "$OUT/pgo.exe"
if ! PGO_BENCH_N="$PN" "$FID" run "$B" --profile "$P" > "$OUT/run.out" 2> "$OUT/run.log" || [ ! -s "$P" ]; then
  echo "FAIL profiling run (see $OUT/run.log)"; exit 1
fi
blocks=$(grep -c "@" "$P")
echo "profile: $(grep -vc "@" "$P") lines of functions, $blocks of blocks"
"$FID" emit "$B" $TARGET -o "$OUT/plain.exe" > "$OUT/plain.log" 2>&1 < /dev/null
"$FID" emit "$B" $TARGET --profile-use "$P" -o "$OUT/pgo.exe" > "$OUT/pgo.log" 2>&1 < /dev/null
if [ ! -f "$OUT/plain.exe" ] || [ ! -f "$OUT/pgo.exe" ]; then
  echo "FAIL build (see $OUT/plain.log, $OUT/pgo.log)"; exit 1
fi
best() { # best <exe> -> "<ms> <checksum>"
  local b="" sum="" r=0
  while [ "$r" -lt "$RUNS" ]; do
    local s e out
    s=$(date +%s%N)
    out=$(PGO_BENCH_N="$N" "$1" | tr -d '\r')
    e=$(date +%s%N)
    local ms=$(( (e - s) / 1000000 ))
    if [ -z "$b" ] || [ "$ms" -lt "$b" ]; then b=$ms; fi
    sum="$out"
    r=$((r + 1))
  done
  echo "$b $sum"
}
read -r t0 s0 < <(best "$OUT/plain.exe")
read -r t1 s1 < <(best "$OUT/pgo.exe")
echo "without profile: $t0 ms (checksum $s0)"
echo "with profile:    $t1 ms (checksum $s1)"
if [ "$s0" != "$s1" ] || [ -z "$s0" ]; then
  echo "FAIL the builds print different checksums"; exit 1
fi
ratio=$(awk -v a="$t0" -v b="$t1" 'BEGIN { if (b <= 0) b = 1; printf "%.2f", a / b }')
echo "speedup: ${ratio}x (MIN ${MIN})"
if awk -v r="$ratio" -v m="$MIN" 'BEGIN { exit !(r + 0 >= m + 0) }'; then
  echo "PASS"
  exit 0
fi
echo "FAIL speedup under ${MIN}"
exit 1
