#!/bin/bash
# bench/compile-speed.sh — how fast each compiler builds programs of growing
# size, clean and after a one-line change (competitive roadmap BM6).
#
#   ./bench/compile-speed.sh                       hello, 10k, 100k lines
#   SIZES="hello 10000 100000 1000000" ./bench/compile-speed.sh
#   FID=path/to/fid RUNS=3 ./bench/compile-speed.sh
#
# The programs come from bench/gen_lines.py (the same program in Fides,
# Rust, Go, C++ and Zig; `hello` is bench/size/). For each size:
#   clean    the first build of that program: Fides with FIDES_NO_CACHE=1
#            and no stamp beside the output; Rust with a new incremental
#            directory; Go, Zig and C++ on content their caches never saw
#            (the standard libraries stay cached, as on a developer's
#            machine)
#   change   one line of the program changed (gen_lines.py --touch: the
#            constant of the first function), then built again with every
#            cache the clean build left: Fides's build cache and stamps,
#            rustc -C incremental, Go's and Zig's caches; C++ has none
#   check    `fid check` alone (parse, types, borrows, effects): the time an
#            editor waits for errors
# Fides builds natively (`fid emit`; on Linux the machine layer) and
# through C (`fid ccompile`). Rivals build in their debug mode (rustc
# -C opt-level=0, go build, zig build-exe -O Debug, zig c++ -O0, g++ -O0):
# the mode used while editing. Times are wall clock in seconds, best of
# RUNS (default 1: the large sizes take minutes); each executable is run
# once and must print what the Fides one prints, or the cell says WRONG.
set -u
cd "$(dirname "$0")" || exit 1
RUNS="${RUNS:-1}"
SIZES="${SIZES:-hello 10000 100000}"
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
OUT="$(pwd)/out/compile-speed"
mkdir -p "$OUT"
have() { command -v "$1" >/dev/null 2>&1; }

# load average (Linux, macOS; Git Bash's /proc/loadavg on Windows), so a
# table says how quiet the machine was (as in run.sh)
load() {
  if [ -r /proc/loadavg ]; then cut -d' ' -f1-3 /proc/loadavg
  elif have sysctl; then sysctl -n vm.loadavg 2>/dev/null | tr -d '{}'
  else echo "n/a"; fi
}
LOAD_BEFORE=$(load)

# now_ns; secs <ns> -> "1.23"
now_ns() { date +%s%N; }
secs() { printf '%d.%02d' $(($1 / 1000000000)) $((($1 / 10000000) % 100)); }

# compile <lang> <dir> <phase> -> prints "<ns> <exe>" ("" on failure)
compile() {
  local lang=$1 d=$2 phase=$3 exe="$2/prog-$1$X" log="$2/$1-$3.log" t0 t1 rc
  t0=$(now_ns)
  case $lang in
    fides|fides-c)
      local cmd=emit flags="$EMIT_FLAGS"
      [ "$lang" = fides-c ] && { cmd=ccompile; flags=""; }
      if [ "$phase" = clean ]; then
        rm -f "$exe" "$exe.fides-stamp"
        # shellcheck disable=SC2086
        (cd .. && FIDES_NO_CACHE=1 "$FID" $cmd "$d/prog.fd" $flags -o "$exe") > "$log" 2>&1
      else
        # shellcheck disable=SC2086
        (cd .. && "$FID" $cmd "$d/prog.fd" $flags -o "$exe") > "$log" 2>&1
      fi ;;
    check) (cd .. && "$FID" check "$d/prog.fd") > "$log" 2>&1; rc=$?
           t1=$(now_ns); [ $rc -eq 0 ] && echo "$((t1 - t0)) -"; return ;;
    rust)
      [ "$phase" = clean ] && rm -rf "$d/rust-inc"
      rustc -C opt-level=0 -C incremental="$d/rust-inc" -o "$exe" "$d/prog.rs" > "$log" 2>&1 ;;
    go)      (cd "$d" && go build -o "prog-go$X" prog.go) > "$log" 2>&1 ;;
    zig)     zig build-exe -O Debug -femit-bin="$exe" "$d/prog.zig" > "$log" 2>&1 ;;
    cpp)     zig c++ -O0 -o "$exe" "$d/prog.cpp" > "$log" 2>&1 ;;
    cpp-gcc) g++ -O0 -o "$exe" "$d/prog.cpp" > "$log" 2>&1 ;;
  esac
  rc=$?
  t1=$(now_ns)
  [ $rc -eq 0 ] && [ -x "$exe" ] && echo "$((t1 - t0)) $exe"
}

available() {
  case $1 in
    fides|fides-c|check) [ -n "${FID:-}" ] ;;
    rust) have rustc ;; go) have go ;; zig|cpp) have zig ;; cpp-gcc) have g++ ;;
  esac
}

LANGS=(check fides fides-c rust go zig cpp cpp-gcc)
echo "| Program | Build | fid check | Fides native | Fides via C | Rust | Go | Zig | C++ (Clang) | C++ (GCC) |"
echo "|---|---|---|---|---|---|---|---|---|---|"
for size in $SIZES; do
  d="$OUT/$size"
  rm -rf "$d"; mkdir -p "$d"
  if [ "$size" = hello ]; then
    cp size/fides/hello.fd "$d/prog.fd"; cp size/rust/hello.rs "$d/prog.rs"
    cp size/go/hello.go "$d/prog.go"; cp size/zig/hello.zig "$d/prog.zig"
    cp size/cpp/hello.cpp "$d/prog.cpp"
    lines=$(wc -l < "$d/prog.fd" | tr -d ' ')
  else
    "$PY" gen_lines.py "$size" "$d" > /dev/null || { echo "gen_lines.py failed"; exit 2; }
    lines=$(wc -l < "$d/prog.fd" | tr -d ' ')
  fi
  phases="clean change"; [ "$size" = hello ] && phases="clean"
  for phase in $phases; do
    if [ "$phase" = change ]; then "$PY" gen_lines.py --touch "$d" || exit 2; fi
    row="| $size ($lines lines) | $phase"
    ref=""
    for lang in "${LANGS[@]}"; do
      if ! available "$lang" || { [ "$lang" = check ] && [ "$phase" = change ]; }; then
        row="$row | -"; continue
      fi
      best=""; exe=""
      for r in $(seq 1 "$RUNS"); do
        # a repeated clean build must be clean again; a repeated change
        # build rebuilds the same change (caches warm)
        read -r ns exe < <(compile "$lang" "$d" "$phase")
        [ -n "${ns:-}" ] || break
        if [ -z "$best" ] || [ "$ns" -lt "$best" ]; then best=$ns; fi
        [ "$r" -lt "$RUNS" ] && [ "$phase" = clean ] && [ "$lang" = rust ] && rm -rf "$d/rust-inc"
      done
      if [ -z "$best" ]; then row="$row | failed"; continue; fi
      cell="$(secs "$best")"
      if [ "$exe" != "-" ] && [ -n "$exe" ]; then
        out=$("$exe" 2>&1 | tr -d '\r' | tail -1)
        [ -z "$ref" ] && ref="$out"
        [ "$out" != "$ref" ] && cell="WRONG ($out)"
      fi
      row="$row | $cell"
    done
    echo "$row |"
  done
done
echo
echo "Seconds, wall clock, best of $RUNS; $(date +%F), $(uname -s) $(uname -m); fid=${FID:-none}."
echo "Load average before / after: $LOAD_BEFORE / $(load)."
echo "Rivals in debug mode; the programs are bench/gen_lines.py's (BM6)."
