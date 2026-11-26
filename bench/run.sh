#!/bin/bash
# bench/run.sh — the same programs in Fides, Rust, Zig, C++ and Go, built and
# timed with one command (competitive roadmap BM1).
#
#   ./bench/run.sh [bench ...]          default: every program in bench/fides/
#   RUNS=5 ./bench/run.sh fib           best of 5 instead of 3
#   FID=path/to/fid ./bench/run.sh      the Fides compiler to measure
#   SUITE=game ./bench/run.sh           another set: bench/game/ (the
#                                       Benchmarks Game, BM3); default the
#                                       programs of bench/fides/ etc.
#
# Each program prints one number; a language whose output differs from the
# Fides one is reported as WRONG, not timed. A language whose toolchain is
# missing is reported as "-". Fides runs two ways: native (`fid emit`; on
# Linux `--target x86_64-linux` or `aarch64-linux`, the machine layer: a plain
# `emit` there goes through C) and through C (`fid ccompile`, needs a C
# compiler; CC or zig cc).
#
# Rivals are built the way a good programmer of that language would ship
# them: rustc -C opt-level=3, zig -O ReleaseFast, clang++ -O2 (a real Clang
# when one is on the PATH, zig c++ otherwise), g++ -O2 (GCC, when on the
# PATH), go build.
#
# Each cell is the best of RUNS wall-clock times and the spread of the runs,
# (slowest - fastest) / fastest (BM8): "0.81 ±3%". A spread over 10% means
# the machine was not quiet; the footer gives the load average where the OS
# has one. bench/versions.txt pins the toolchains: a version that differs is
# named under the table (BM2). Results go to stdout as a markdown table.
set -u
cd "$(dirname "$0")" || exit 1
RUNS="${RUNS:-3}"
X=""
EMIT_FLAGS=""
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) X=".exe" ;;
  Linux) case "$(uname -m)" in
           x86_64) EMIT_FLAGS="--target x86_64-linux" ;;
           aarch64|arm64) EMIT_FLAGS="--target aarch64-linux" ;;
         esac ;;
esac
# the program set: bench/<lang>/ by default, bench/$SUITE/<lang>/ otherwise
SUITE="${SUITE:-}"
SD=""
[ -n "$SUITE" ] && SD="$SUITE/"
[ -z "$SUITE" ] || [ -d "$SUITE/fides" ] || { echo "no bench/$SUITE/fides/"; exit 2; }
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
  BENCHES=(); for f in "${SD}"fides/*.fd; do BENCHES+=("$(basename "$f" .fd)"); done
fi

have() { command -v "$1" >/dev/null 2>&1; }

# GCC (BM2): g++ on the PATH, else on Windows the g++ of WSL, which builds a
# Linux binary that is timed inside WSL (Linux on this machine, not Windows:
# the footer says so). GCC_WSL=0 turns the WSL fallback off.
GXX=""
if have g++; then GXX=native
elif [ -n "$X" ] && [ "${GCC_WSL:-1}" != 0 ] && have wsl.exe \
     && MSYS_NO_PATHCONV=1 wsl.exe -e sh -c 'command -v g++' >/dev/null 2>&1; then GXX=wsl
fi

# Clang (BM2): a real clang++ on the PATH. zig c++ is the fallback, not the
# preferred column: it is the same Clang front end but not the toolchain a
# Clang user installs.
CXXC=""
if have clang++; then CXXC=native
elif have zig; then CXXC=zig
fi

# build <lang> <bench> -> prints the executable path, or nothing
build() {
  local lang=$1 b=$2 exe="$OUT/${SUITE:+$SUITE-}$2-$1$X" d="$SD"
  local log="$OUT/${SUITE:+$SUITE-}$b-$lang.log"
  case $lang in
    # fid finds stdlib/ in the directory it runs in: build from the root
    # shellcheck disable=SC2086
    fides)   [ -n "${FID:-}" ] && (cd .. && "$FID" emit "bench/${d}fides/$b.fd" $EMIT_FLAGS -o "bench/$exe") >"$log" 2>&1 ;;
    fides-c) [ -n "${FID:-}" ] && (cd .. && "$FID" ccompile "bench/${d}fides/$b.fd" -o "bench/$exe") >"$log" 2>&1 ;;
    rust)    [ -f "${d}rust/$b.rs" ] && have rustc && rustc -C opt-level=3 -o "$exe" "${d}rust/$b.rs" >"$log" 2>&1 ;;
    zig)     [ -f "${d}zig/$b.zig" ] && have zig && zig build-exe -O ReleaseFast -femit-bin="$exe" "${d}zig/$b.zig" >"$log" 2>&1 ;;
    cpp)     [ -f "${d}cpp/$b.cpp" ] && case $CXXC in
               native) clang++ -O2 -o "$exe" "${d}cpp/$b.cpp" ;;
               zig) zig c++ -O2 -o "$exe" "${d}cpp/$b.cpp" ;;
               *) false ;;
             esac >"$log" 2>&1 ;;
    cpp-gcc) [ -f "${d}cpp/$b.cpp" ] && case $GXX in
               native) g++ -O2 -o "$exe" "${d}cpp/$b.cpp" ;;
               # a Linux ELF: best() times a *.elf inside WSL
               wsl) exe="${exe%.exe}.elf"; MSYS_NO_PATHCONV=1 wsl.exe -e g++ -O2 -o "$exe" "${d}cpp/$b.cpp" ;;
               *) false ;;
             esac >"$log" 2>&1 ;;
    go)      [ -f "${d}go/$b.go" ] && have go && go build -o "$exe" "${d}go/$b.go" >"$log" 2>&1 ;;
    *) false ;;
  esac && { [ -x "$exe" ] || { [[ $exe == *.elf ]] && [ -s "$exe" ]; }; } && echo "$exe"
}

# best_wsl <exe.elf> -> like best, the runs timed inside WSL (one wsl.exe
# start for all of them, so its start-up is not in the times)
best_wsl() {
  local exe=$1 bestns="" worstns="" out="no-output" ns o
  local lim="${LIMIT:-120}"
  while read -r ns o; do
    case $ns in ''|*[!0-9]*) continue ;; esac
    if [ $((ns / 1000000000)) -ge "$lim" ]; then echo "TIMEOUT 0 TIMEOUT"; return; fi
    out="$o"
    if [ -z "$bestns" ] || [ "$ns" -lt "$bestns" ]; then bestns=$ns; fi
    if [ -z "$worstns" ] || [ "$ns" -gt "$worstns" ]; then worstns=$ns; fi
  done < <(MSYS_NO_PATHCONV=1 wsl.exe -e bash -c 'for _ in $(seq 1 "$1"); do t0=$(date +%s%N); o=$(timeout "$2" "./$3" 2>&1 | tail -1); t1=$(date +%s%N); echo "$((t1 - t0)) $o"; done' run "$RUNS" "$lim" "$exe" 2>/dev/null | tr -d '\r')
  [ -z "$bestns" ] && { echo "0.00 0 $out"; return; }
  local spread=0
  [ "$bestns" -gt 0 ] && spread=$(( (worstns - bestns) * 100 / bestns ))
  printf '%d.%02d %d %s\n' $((bestns / 1000000000)) $(((bestns / 10000000) % 100)) "$spread" "$out"
}

# best <exe> -> best wall time in seconds over RUNS runs and the spread of
# the runs in percent (prints "secs spread out")
best() {
  local exe=$1 bestns="" worstns="" out=""
  case $exe in *.elf) best_wsl "$exe"; return ;; esac
  for _ in $(seq 1 "$RUNS"); do
    local t0 t1 o
    t0=$(date +%s%N)
    o=$(timeout "${LIMIT:-120}" "$exe" 2>&1 | tr -d '\r' | tail -1)
    t1=$(date +%s%N)
    # A run over the limit (LIMIT seconds, default 120) is reported, not waited for
    if [ $(( (t1 - t0) / 1000000000 )) -ge "${LIMIT:-120}" ]; then echo "TIMEOUT 0 TIMEOUT"; return; fi
    local ns=$((t1 - t0))
    out="$o"
    if [ -z "$bestns" ] || [ "$ns" -lt "$bestns" ]; then bestns=$ns; fi
    if [ -z "$worstns" ] || [ "$ns" -gt "$worstns" ]; then worstns=$ns; fi
  done
  local spread=0
  [ "$bestns" -gt 0 ] && spread=$(( (worstns - bestns) * 100 / bestns ))
  printf '%d.%02d %d %s\n' $((bestns / 1000000000)) $(((bestns / 10000000) % 100)) "$spread" "$out"
}

# load average (Linux, macOS), so a table says how quiet the machine was
load() {
  if [ -r /proc/loadavg ]; then cut -d' ' -f1-3 /proc/loadavg
  elif have sysctl; then sysctl -n vm.loadavg 2>/dev/null | tr -d '{}'
  else echo "n/a"; fi
}
LOAD_BEFORE=$(load)

LANGS=(fides fides-c rust zig cpp cpp-gcc go)
echo "| Benchmark | Fides native | Fides via C | Rust | Zig | C++ (Clang) | C++ (GCC) | Go |"
echo "|---|---|---|---|---|---|---|---|"
for b in "${BENCHES[@]}"; do
  row="| $b"
  ref=""
  for lang in "${LANGS[@]}"; do
    exe=$(build "$lang" "$b")
    if [ -z "$exe" ]; then row="$row | -"; continue; fi
    read -r secs spread out < <(best "$exe")
    if [ "$secs" = "TIMEOUT" ]; then row="$row | over ${LIMIT:-120} s"; continue; fi
    if [ -z "$ref" ]; then ref="$out"; fi
    if [ "$out" != "$ref" ]; then row="$row | WRONG ($out)"; else row="$row | $secs ±$spread%"; fi
  done
  echo "$row |"
done
echo
echo "Seconds, best of $RUNS, wall clock, ±spread of the runs; $(date +%F), $(uname -s) $(uname -m)${SUITE:+, suite $SUITE}."
echo "Load average before / after: $LOAD_BEFORE / $(load)."
V_RUST=$(have rustc && rustc --version | awk '{print $2}')
V_ZIG=$(have zig && zig version)
V_GO=$(have go && go version | awk '{print $3}' | sed 's/^go//')
V_GCC=""
case $GXX in
  native) V_GCC=$(g++ -dumpfullversion 2>/dev/null) ;;
  wsl) V_GCC=$(MSYS_NO_PATHCONV=1 wsl.exe -e g++ -dumpfullversion 2>/dev/null | tr -d '\r') ;;
esac
clang_version() { "$@" --version 2>/dev/null | sed -n 's/^clang version \([0-9][0-9.]*\).*/\1/p' | head -1; }
V_CLANG=""
case $CXXC in
  native) V_CLANG=$(clang_version clang++) ;;
  zig) V_CLANG=$(clang_version zig c++) ;;
esac
echo "Toolchains: fid=${FID:-none}; rustc ${V_RUST:--}; zig ${V_ZIG:--}; clang++ ${V_CLANG:--}${CXXC:+ ($CXXC)}; go ${V_GO:--}; g++ ${V_GCC:--}${GXX:+ ($GXX)}."
[ "$GXX" = wsl ] && echo "C++ (GCC): g++ of WSL, built and timed inside WSL (Linux on this machine); the other columns run on Windows."
# BM2: the pinned versions (bench/versions.txt: "tool version" lines; a
# version matches when it starts with the pinned one, so "15" pins any 15.x)
if [ -f versions.txt ]; then
  while read -r tool want; do
    case "$tool" in ''|\#*) continue ;; esac
    case "$tool" in
      rustc) got="$V_RUST" ;; zig) got="$V_ZIG" ;; clang) got="$V_CLANG" ;; go) got="$V_GO" ;; gcc) got="$V_GCC" ;; *) continue ;;
    esac
    [ -z "$got" ] && continue
    case "$got" in "$want"*) ;; *) echo "Note: $tool is $got, bench/versions.txt pins $want: numbers are not comparable with the published ones." ;; esac
  done < versions.txt
fi
