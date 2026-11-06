#!/bin/bash
# bench/run.sh — the same programs in Fides, Rust, Zig, C++ and Go, built and
# timed with one command (competitive roadmap BM1).
#
#   ./bench/run.sh [bench ...]          default: every program in bench/fides/
#   RUNS=5 ./bench/run.sh fib           best of 5 instead of 3
#   FID=path/to/fid ./bench/run.sh      the Fides compiler to measure
#
# Each program prints one number; a language whose output differs from the
# Fides one is reported as WRONG, not timed. A language whose toolchain is
# missing is reported as "-". Fides runs two ways: native (`fid emit`) and
# through C (`fid ccompile`, needs a C compiler; CC or zig cc).
#
# Rivals are built the way a good programmer of that language would ship
# them: rustc -C opt-level=3, zig -O ReleaseFast, zig c++ -O2, go build.
# Results go to stdout as a markdown table, with the toolchain versions.
set -u
cd "$(dirname "$0")" || exit 1
RUNS="${RUNS:-3}"
X=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
if [ -z "${FID:-}" ]; then
  for c in ../release/stage0/fid$X ../release/fid$X ../release/fid-dev$X; do
    if [ -x "$c" ]; then FID="$c"; break; fi
  done
fi
[ -n "${FID:-}" ] && FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
OUT=out
mkdir -p "$OUT"
# `fid ccompile` needs a C compiler; zig cc is the one every platform has
if [ -z "${CC:-}" ] && command -v zig >/dev/null 2>&1; then export CC="zig cc"; fi
# Go installs to a fixed place on Windows and may not be on PATH yet
[ -d "/c/Program Files/Go/bin" ] && PATH="$PATH:/c/Program Files/Go/bin"

if [ $# -gt 0 ]; then BENCHES=("$@"); else
  BENCHES=(); for f in fides/*.fd; do BENCHES+=("$(basename "$f" .fd)"); done
fi

have() { command -v "$1" >/dev/null 2>&1; }

# build <lang> <bench> -> prints the executable path, or nothing
build() {
  local lang=$1 b=$2 exe="$OUT/$2-$1$X"
  case $lang in
    # fid finds stdlib/ in the directory it runs in: build from the root
    fides)   [ -n "${FID:-}" ] && (cd .. && "$FID" emit "bench/fides/$b.fd" -o "bench/$exe") >"$OUT/$b-$lang.log" 2>&1 ;;
    fides-c) [ -n "${FID:-}" ] && (cd .. && "$FID" ccompile "bench/fides/$b.fd" -o "bench/$exe") >"$OUT/$b-$lang.log" 2>&1 ;;
    rust)    [ -f "rust/$b.rs" ] && have rustc && rustc -C opt-level=3 -o "$exe" "rust/$b.rs" >"$OUT/$b-$lang.log" 2>&1 ;;
    zig)     [ -f "zig/$b.zig" ] && have zig && zig build-exe -O ReleaseFast -femit-bin="$exe" "zig/$b.zig" >"$OUT/$b-$lang.log" 2>&1 ;;
    cpp)     [ -f "cpp/$b.cpp" ] && have zig && zig c++ -O2 -o "$exe" "cpp/$b.cpp" >"$OUT/$b-$lang.log" 2>&1 ;;
    go)      [ -f "go/$b.go" ] && have go && go build -o "$exe" "go/$b.go" >"$OUT/$b-$lang.log" 2>&1 ;;
    *) false ;;
  esac && [ -x "$exe" ] && echo "$exe"
}

# best <exe> -> best wall time in seconds over RUNS runs (prints "s out")
best() {
  local exe=$1 bestns="" out=""
  for _ in $(seq 1 "$RUNS"); do
    local t0 t1 o
    t0=$(date +%s%N)
    o=$(timeout "${LIMIT:-120}" "$exe" 2>&1 | tr -d '\r' | tail -1)
    t1=$(date +%s%N)
    # A run over the limit (LIMIT seconds, default 120) is reported, not waited for
    if [ $(( (t1 - t0) / 1000000000 )) -ge "${LIMIT:-120}" ]; then echo "TIMEOUT TIMEOUT"; return; fi
    local ns=$((t1 - t0))
    out="$o"
    if [ -z "$bestns" ] || [ "$ns" -lt "$bestns" ]; then bestns=$ns; fi
  done
  printf '%d.%02d %s\n' $((bestns / 1000000000)) $(((bestns / 10000000) % 100)) "$out"
}

LANGS=(fides fides-c rust zig cpp go)
echo "| Benchmark | Fides native | Fides via C | Rust | Zig | C++ | Go |"
echo "|---|---|---|---|---|---|---|"
for b in "${BENCHES[@]}"; do
  row="| $b"
  ref=""
  for lang in "${LANGS[@]}"; do
    exe=$(build "$lang" "$b")
    if [ -z "$exe" ]; then row="$row | -"; continue; fi
    read -r secs out < <(best "$exe")
    if [ "$secs" = "TIMEOUT" ]; then row="$row | over ${LIMIT:-120} s"; continue; fi
    if [ -z "$ref" ]; then ref="$out"; fi
    if [ "$out" != "$ref" ]; then row="$row | WRONG ($out)"; else row="$row | $secs"; fi
  done
  echo "$row |"
done
echo
echo "Seconds, best of $RUNS, wall clock; $(date +%F), $(uname -s)."
echo "Toolchains: fid=${FID:-none}; $(have rustc && rustc --version); zig $(have zig && zig version); $(have go && go version)"
