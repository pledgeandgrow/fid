#!/usr/bin/env bash
# Allocator speed (competitive roadmap RT8): fid's allocator against the C
# library's malloc and mimalloc on mimalloc-bench's shapes (bench/alloc/
# README.md). Each program of bench/alloc/c is built several ways, only the
# allocator differing (alloc.h):
#   libc     the C library's malloc/free
#   libc-z   calloc/free (zeroed blocks, as fid hands out)
#   mi       mimalloc's mi_malloc/mi_free
#   mi-z     mi_zalloc/mi_free
#   fid      fid's p_malloc/p_free: the C runtime `fid dump-c c/rt.fd` writes
# and the Fides version of each program (bench/alloc/fides) is built with
# `fid ccompile` (fides-c) and `fid emit` (fides-native).
#
#   bash bench/alloc/run.sh                    # FID=release/fid.exe
#   FID=path/to/fid CC="zig cc" RUNS=5 bash bench/alloc/run.sh
#   PROGS="small churn" VARIANTS="mi-z fid" bash bench/alloc/run.sh
#
# mimalloc's source: MIMALLOC_DIR (a checkout: include/ and src/static.c),
# else the copy cargo keeps for the mimalloc crate (~/.cargo/registry/src/
# */libmimalloc-sys-*/c_src/mimalloc/v2), else MIMALLOC_FETCH=1 clones
# github.com/microsoft/mimalloc (tag MIMALLOC_TAG, default v2.1.7) into
# $OUT/mimalloc. Without any of them the mi variants are left out.
#
# Prints, per program, the best of RUNS wall-clock times (ms) of each
# variant and fid's time over mi-z's and mi's; every variant of a program
# must print the same number (a different one is reported as WRONG and
# makes the exit code 1). The table is also written to $OUT/results.txt.
# Times are wall clock: on a busy machine compare the ratios, best of a few
# runs.
set -u
cd "$(dirname "$0")/../.."
FID="${FID:-release/fid.exe}"
OUT="${OUT:-bench/out/alloc}"
RUNS="${RUNS:-3}"
PROGS="${PROGS:-small churn sizes threads large}"
VARIANTS="${VARIANTS:-libc libc-z mi mi-z fid fides-c fides-native}"
MIMALLOC_TAG="${MIMALLOC_TAG:-v2.1.7}"
export FID_HOME="${FID_HOME:-$(pwd)}"
mkdir -p "$OUT"

X=""
WIN=0
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) X=".exe"; WIN=1 ;;
esac
if [ -z "${CC:-}" ]; then
  if command -v zig >/dev/null 2>&1; then CC="zig cc"
  elif command -v clang >/dev/null 2>&1; then CC=clang
  else CC=cc; fi
fi
export CC
CFLAGS="${CFLAGS:--O2 -w}"
if [ "$WIN" = 1 ]; then
  SYSLIBS="-lws2_32"
  MILIBS="-ladvapi32"
  THRLIBS=""
else
  SYSLIBS="-lm -lpthread -ldl"
  case "$CC" in *zig*) SYSLIBS="$SYSLIBS -lunwind" ;; esac
  MILIBS="-lpthread"
  THRLIBS="-lpthread"
fi
SRC=bench/alloc/c

now_ms() {
  local t
  t=$(date +%s%N 2>/dev/null)
  case "$t" in
    *N|"") python3 -c 'import time; print(int(time.time() * 1000))' ;;
    *) echo $((t / 1000000)) ;;
  esac
}

has() { case " $VARIANTS " in *" $1 "*) return 0 ;; esac; return 1; }

# --- fid's C runtime
FID_OK=0
if has fid; then
  echo "== fid's C runtime ($FID dump-c $SRC/rt.fd)"
  if "$FID" dump-c "$SRC/rt.fd" >"$OUT/rt.c" 2>"$OUT/rt.log" &&
     $CC $CFLAGS -c -Dmain=__fid_rt_main "$OUT/rt.c" -o "$OUT/rt.o" 2>>"$OUT/rt.log"; then
    FID_OK=1
  else
    echo "   failed: see $OUT/rt.log"
  fi
fi

# --- mimalloc
MI_OK=0
if has mi || has mi-z; then
  MI="${MIMALLOC_DIR:-}"
  if [ -z "$MI" ]; then
    for d in "$HOME"/.cargo/registry/src/*/libmimalloc-sys-*/c_src/mimalloc/v2; do
      [ -f "$d/src/static.c" ] && MI="$d"
    done
  fi
  if [ -z "$MI" ] && [ -f "$OUT/mimalloc/src/static.c" ]; then MI="$OUT/mimalloc"; fi
  if [ -z "$MI" ] && [ "${MIMALLOC_FETCH:-0}" = 1 ]; then
    echo "== fetching mimalloc $MIMALLOC_TAG"
    git clone --quiet --depth 1 --branch "$MIMALLOC_TAG" https://github.com/microsoft/mimalloc "$OUT/mimalloc" && MI="$OUT/mimalloc"
  fi
  if [ -n "$MI" ] && [ -f "$MI/src/static.c" ]; then
    echo "== mimalloc ($MI)"
    if $CC $CFLAGS -c -DNDEBUG -I"$MI/include" "$MI/src/static.c" -o "$OUT/mi.o" 2>"$OUT/mi.log"; then
      MI_OK=1
    else
      echo "   failed: see $OUT/mi.log"
    fi
  else
    echo "== no mimalloc source: set MIMALLOC_DIR, or MIMALLOC_FETCH=1 to clone it; mi variants left out"
  fi
fi

# build <variant> <prog> -> the executable's path on stdout, or nothing
build() {
  local v="$1" p="$2" exe="$OUT/$2-$1$X"
  local log="$OUT/$2-$1.log"
  case "$v" in
    libc)   $CC $CFLAGS -I"$SRC" "$SRC/$p.c" -o "$exe" $THRLIBS >"$log" 2>&1 ;;
    libc-z) $CC $CFLAGS -DZERO -I"$SRC" "$SRC/$p.c" -o "$exe" $THRLIBS >"$log" 2>&1 ;;
    mi)     [ "$MI_OK" = 1 ] && $CC $CFLAGS -DUSE_MIMALLOC -I"$SRC" -I"$MI/include" "$SRC/$p.c" "$OUT/mi.o" -o "$exe" $MILIBS >"$log" 2>&1 ;;
    mi-z)   [ "$MI_OK" = 1 ] && $CC $CFLAGS -DUSE_MIMALLOC -DZERO -I"$SRC" -I"$MI/include" "$SRC/$p.c" "$OUT/mi.o" -o "$exe" $MILIBS >"$log" 2>&1 ;;
    fid)    [ "$FID_OK" = 1 ] && $CC $CFLAGS -DUSE_FID -I"$SRC" "$SRC/$p.c" "$OUT/rt.o" -o "$exe" $SYSLIBS >"$log" 2>&1 ;;
    fides-c)      "$FID" ccompile "bench/alloc/fides/$p.fd" -o "$exe" >"$log" 2>&1 ;;
    fides-native) "$FID" emit "bench/alloc/fides/$p.fd" -o "$exe" >"$log" 2>&1 ;;
    *) false ;;
  esac || return 0
  [ -f "$exe" ] && echo "$exe"
}

status=0
{
  printf "%-8s" "program"
  for v in $VARIANTS; do printf " %12s" "$v"; done
  printf " %10s %10s\n" "fid/mi-z" "fid/mi"
} | tee "$OUT/results.txt"
for p in $PROGS; do
  declare -A T=()
  ref=""
  line=$(printf "%-8s" "$p")
  for v in $VARIANTS; do
    exe=$(build "$v" "$p")
    if [ -z "$exe" ]; then line="$line$(printf " %12s" "-")"; continue; fi
    case "$exe" in /*|?:/*) ;; *) exe="./$exe" ;; esac
    best=""
    out=""
    for _ in $(seq 1 "$RUNS"); do
      t0=$(now_ms)
      out=$("$exe" 2>/dev/null)
      rc=$?
      t1=$(now_ms)
      if [ $rc -ne 0 ]; then out="exit $rc"; best=""; break; fi
      dt=$((t1 - t0))
      if [ -z "$best" ] || [ "$dt" -lt "$best" ]; then best=$dt; fi
    done
    if [ -z "$best" ]; then
      echo "FAILED: $p $v ($out)" >&2
      status=1
      line="$line$(printf " %12s" "FAILED")"
      continue
    fi
    if [ -z "$ref" ]; then ref="$out"; fi
    if [ "$out" != "$ref" ]; then
      echo "WRONG: $p $v printed '$out', the first variant '$ref'" >&2
      status=1
      line="$line$(printf " %12s" "WRONG")"
      continue
    fi
    T[$v]="$best"
    line="$line$(printf " %12s" "$best")"
  done
  ratio() {
    if [ -n "${T[fid]:-}" ] && [ -n "${T[$1]:-}" ] && [ "${T[$1]}" -gt 0 ]; then
      awk -v a="${T[fid]}" -v b="${T[$1]}" 'BEGIN { printf "%.2fx", a / b }'
    else
      echo "-"
    fi
  }
  line="$line$(printf " %10s %10s" "$(ratio mi-z)" "$(ratio mi)")"
  echo "$line" | tee -a "$OUT/results.txt"
  unset T
done
echo "(ms, best of $RUNS; CC=$CC)" | tee -a "$OUT/results.txt"
exit $status
