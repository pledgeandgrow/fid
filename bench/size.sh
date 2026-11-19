#!/bin/bash
# bench/size.sh — binary size, startup time and peak memory, every language
# (competitive roadmap BM7).
#
#   ./bench/size.sh                 hello (bench/size/) and bench/fides/*.fd
#   ./bench/size.sh hello fib       some programs
#   RUNS=50 ./bench/size.sh         startup: best of 50 (default 20)
#   FID=path/to/fid ./bench/size.sh
#
# Three markdown tables on stdout:
#   Binary size   the executable as built (KB), the way each language
#                 ships by default: no strip, no compression; Fides native
#                 (`fid emit`, on Linux the machine layer) and through C
#   Startup       wall time of `hello`, best of RUNS (ms): process creation,
#                 runtime start-up and one line of output (timed by
#                 bench/peakmem.py around the child process alone)
#   Peak memory   the most each program had resident (MB), measured by
#                 bench/peakmem.py (ru_maxrss on Linux/macOS, the peak
#                 working set on Windows)
# The programs are built the same way as bench/run.sh (same flags); a
# program whose output differs from the Fides one is marked WRONG.
set -u
cd "$(dirname "$0")" || exit 1
RUNS="${RUNS:-20}"
X=""
EMIT_FLAGS=""
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) X=".exe" ;;
  Linux) case "$(uname -m)" in
           x86_64) EMIT_FLAGS="--target x86_64-linux" ;;
           aarch64|arm64) EMIT_FLAGS="--target aarch64-linux" ;;
         esac ;;
esac
if [ -z "${FID:-}" ]; then
  for c in ../release/stage0/fid$X ../release/fid$X ../release/fid-dev$X; do
    if [ -x "$c" ]; then FID="$c"; break; fi
  done
fi
[ -n "${FID:-}" ] && FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
if [ -z "${CC:-}" ] && command -v zig >/dev/null 2>&1; then export CC="zig cc"; fi
[ -d "/c/Program Files/Go/bin" ] && PATH="$PATH:/c/Program Files/Go/bin"
PY=python3; command -v python3 >/dev/null 2>&1 || PY=python
OUT=out
mkdir -p "$OUT"

if [ $# -gt 0 ]; then PROGS=("$@"); else
  PROGS=(hello); for f in fides/*.fd; do PROGS+=("$(basename "$f" .fd)"); done
fi

have() { command -v "$1" >/dev/null 2>&1; }

# dir_of <bench>: hello lives in bench/size/, the others in bench/
dir_of() { if [ "$1" = hello ]; then echo "size/"; else echo ""; fi; }

# build <lang> <bench> -> the executable, or nothing (as bench/run.sh)
build() {
  local lang=$1 b=$2 d; d=$(dir_of "$2")
  local exe="$OUT/size-$b-$lang$X" log="$OUT/size-$b-$lang.log"
  case $lang in
    # shellcheck disable=SC2086
    fides)   [ -n "${FID:-}" ] && (cd .. && "$FID" emit "bench/${d}fides/$b.fd" $EMIT_FLAGS -o "bench/$exe") >"$log" 2>&1 ;;
    fides-c) [ -n "${FID:-}" ] && (cd .. && "$FID" ccompile "bench/${d}fides/$b.fd" -o "bench/$exe") >"$log" 2>&1 ;;
    rust)    [ -f "${d}rust/$b.rs" ] && have rustc && rustc -C opt-level=3 -o "$exe" "${d}rust/$b.rs" >"$log" 2>&1 ;;
    zig)     [ -f "${d}zig/$b.zig" ] && have zig && zig build-exe -O ReleaseFast -femit-bin="$exe" "${d}zig/$b.zig" >"$log" 2>&1 ;;
    cpp)     [ -f "${d}cpp/$b.cpp" ] && have zig && zig c++ -O2 -o "$exe" "${d}cpp/$b.cpp" >"$log" 2>&1 ;;
    cpp-gcc) [ -f "${d}cpp/$b.cpp" ] && have g++ && g++ -O2 -o "$exe" "${d}cpp/$b.cpp" >"$log" 2>&1 ;;
    go)      [ -f "${d}go/$b.go" ] && have go && go build -o "$exe" "${d}go/$b.go" >"$log" 2>&1 ;;
    *) false ;;
  esac && [ -x "$exe" ] && echo "$exe"
}

fsize_kb() { local s; s=$(wc -c < "$1" | tr -d ' '); echo $(( (s + 512) / 1024 )); }

LANGS=(fides fides-c rust zig cpp cpp-gcc go)
HEAD="| Program | Fides native | Fides via C | Rust | Zig | C++ (Clang) | C++ (GCC) | Go |"
SEP="|---|---|---|---|---|---|---|---|"
declare -A EXE=()
for b in "${PROGS[@]}"; do
  for lang in "${LANGS[@]}"; do
    EXE["$b|$lang"]=$(build "$lang" "$b")
  done
done

echo "Binary size (KB)"
echo
echo "$HEAD"; echo "$SEP"
for b in "${PROGS[@]}"; do
  row="| $b"
  for lang in "${LANGS[@]}"; do
    e=${EXE["$b|$lang"]}
    if [ -n "$e" ]; then row="$row | $(fsize_kb "$e")"; else row="$row | -"; fi
  done
  echo "$row |"
done

echo
echo "Startup: hello, best of $RUNS (ms)"
echo
echo "$HEAD"; echo "$SEP"
row="| hello"
for lang in "${LANGS[@]}"; do
  e=${EXE["hello|$lang"]:-}
  if [ -z "$e" ]; then row="$row | -"; continue; fi
  # timed by peakmem.py around the child alone: `date` and a fork in Git
  # Bash cost ~130 ms on Windows, more than any hello
  bestms=""
  for _ in $(seq 1 "$RUNS"); do
    read -r _kb ms _out <<< "$("$PY" peakmem.py "$e" 2>/dev/null)"
    [ -n "${ms:-}" ] || continue
    if [ -z "$bestms" ] || awk "BEGIN{exit !($ms < $bestms)}"; then bestms=$ms; fi
  done
  row="$row | ${bestms:-failed}"
done
echo "$row |"

echo
echo "Peak memory (MB)"
echo
echo "$HEAD"; echo "$SEP"
for b in "${PROGS[@]}"; do
  row="| $b"
  ref=""
  for lang in "${LANGS[@]}"; do
    e=${EXE["$b|$lang"]}
    if [ -z "$e" ]; then row="$row | -"; continue; fi
    pm=$(timeout "${LIMIT:-120}" "$PY" peakmem.py "$e" 2> "$OUT/size-$b-$lang.mem.err"); pst=$?
    read -r kb _ms out <<< "$pm"
    if [ "$pst" = 124 ]; then row="$row | over ${LIMIT:-120} s"; continue; fi
    if [ "$pst" != 0 ] || [ -z "${kb:-}" ]; then row="$row | failed (see bench/$OUT/size-$b-$lang.mem.err)"; continue; fi
    [ -z "$ref" ] && ref="$out"
    if [ "$out" != "$ref" ]; then row="$row | WRONG ($out)"; continue; fi
    row="$row | $((kb / 1024)).$(( (kb * 10 / 1024) % 10 ))"
  done
  echo "$row |"
done
echo
echo "$(date +%F), $(uname -s) $(uname -m); fid=${FID:-none}. Sizes as built (not stripped)."
