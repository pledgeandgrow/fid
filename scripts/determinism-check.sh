#!/bin/bash
# determinism-check.sh — deterministic output (docs/roadmap/llvm-roadmap.md
# RL13): no hash-map order reaches code generation, and two compiles of the
# same input give the same bytes.
#
# scripts/verify/repro.sh (VF12) compares two builds made in different
# directories and temp folders; scripts/repro-check.sh (SP20) compares
# builds with the caches on and off. This script adds what they do not:
#
#   1. source  scripts/determinism-scan.py over compiler/*.fd: every walk of
#              a hash map (HashMap_keys/values, their _i forms, HashMap_to_str,
#              p_map_keys/values, core's hm_*) and every random number must
#              be listed with its reason in scripts/determinism-allow.txt.
#   2. c       the same scan over the C of the compiler, which also sees a
#              `for k in map` (lowered to HashMap_keys) with the function it
#              is in: COMPILER_C=<file> (e.g. release/fid-dev.c of the
#              build), or SELF=1 to write it with `$FID dump-c
#              compiler/main.fd` twice, which also compares the two C texts.
#              Without either the step is skipped (and said so).
#   3. runs    each program built RUNS times (default 3) for every output
#              (dump-c C text; `emit` for the host; `emit --target` of each
#              of NAT_TARGETS; `emit --nat` on Windows), each run in another
#              process with another environment size (FID_DET_PAD: moves
#              the stack and the heap), another TEMP folder, run 2 with
#              `--time` and FID_EGRAPH_STATS=1 (what a run prints on stderr
#              must not change the output), and run 3 with address-space
#              randomization off where `setarch -R` exists (Linux): heap
#              addresses differ between the runs, so an order taken from
#              addresses (a map of unowned string keys sorts them by address)
#              shows as a difference. Every output must be byte-identical to
#              run 1's; a difference is shown with its first offsets.
#              Besides the given programs (default: a few tests), a
#              generated one with many functions, structs, enums, strings
#              and generic instances (the compiler's name tables) is built.
#
# Usage:
#   scripts/determinism-check.sh [program.fd ...]
#   FID=<fid>           the compiler (default release/fid.exe, else release/fid)
#   RUNS=<n>            builds of each output (default 3, at least 2)
#   NAT_TARGETS="..."   machine-layer targets (default x86_64-linux
#                       aarch64-linux riscv64-linux; "" for none)
#   COMPILER_C=<file>   the compiler's C for step 2;  SELF=1: make it
#   GEN_N=<n>           functions of the generated program (default 80; 0: none)
# Exit status: 0 when every check passed, 1 otherwise.
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
RUNS="${RUNS:-3}"
[ "$RUNS" -lt 2 ] && RUNS=2
NAT_TARGETS="${NAT_TARGETS-x86_64-linux aarch64-linux riscv64-linux}"
GEN_N="${GEN_N:-80}"
PY="${PYTHON:-python3}"
command -v "$PY" > /dev/null 2>&1 || PY=python
export FIDES_NO_CACHE=1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; *) HOST=posix ;; esac
DIR=build/determinism
mkdir -p "$DIR"
DIR="$(cd "$DIR" && pwd)"
fail=0
ok()  { echo "  ok    $*"; }
bad() { echo "  FAIL  $*"; fail=1; }

# 0. the scans find what they look for: tests/determinism/walks_map.fd walks
# a map in two functions (and names the walks in a comment and a string)
echo "== 0. the scans on a fixture that walks a map"
FIX=tests/determinism/walks_map.fd
"$PY" -I scripts/determinism-scan.py source tests/determinism > "$DIR/fix-source.txt" 2>&1
st=$?
if [ "$st" = 1 ] && grep -q "NEW .*walk_values: .values()" "$DIR/fix-source.txt" \
   && [ "$(grep -c "NEW" "$DIR/fix-source.txt")" = 1 ]; then
  ok "source scan reports walk_values only"
else
  bad "source scan of the fixture (status $st, see $DIR/fix-source.txt)"
fi
if "$FID" dump-c "$FIX" > "$DIR/fix.c" 2> "$DIR/fix.err" && [ -s "$DIR/fix.c" ]; then
  "$PY" -I scripts/determinism-scan.py c "$DIR/fix.c" > "$DIR/fix-c.txt" 2>&1
  st=$?
  if [ "$st" = 1 ] && grep -q "NEW .*walk_keys -> HashMap_keys" "$DIR/fix-c.txt" \
     && grep -q "NEW .*walk_values -> HashMap_values" "$DIR/fix-c.txt" \
     && ! grep -q "in_a_string" "$DIR/fix-c.txt"; then
    ok "C scan reports the for-in loop of walk_keys and walk_values"
  else
    bad "C scan of the fixture (status $st, see $DIR/fix-c.txt)"
  fi
else
  bad "dump-c of $FIX failed (see $DIR/fix.err)"
fi

echo "== 1. hash-map walks and random numbers in compiler/*.fd"
if "$PY" -I scripts/determinism-scan.py source compiler > "$DIR/scan-source.txt" 2>&1; then
  ok "$(tail -1 "$DIR/scan-source.txt")"
else
  grep -F "NEW" "$DIR/scan-source.txt" | sed 's/^/        /'
  bad "$(tail -1 "$DIR/scan-source.txt") (scripts/determinism-allow.txt)"
fi

echo "== 2. the same in the compiler's C"
CC_FILE="${COMPILER_C:-}"
if [ -n "${SELF:-}" ]; then
  "$FID" dump-c compiler/main.fd > "$DIR/self1.c" 2> "$DIR/self1.err"
  "$FID" dump-c compiler/main.fd > "$DIR/self2.c" 2> "$DIR/self2.err"
  if [ -s "$DIR/self1.c" ] && cmp -s "$DIR/self1.c" "$DIR/self2.c"; then
    ok "two dump-c of compiler/main.fd are identical ($(wc -c < "$DIR/self1.c") bytes)"
  else
    bad "two dump-c of compiler/main.fd differ or failed ($DIR/self1.c, $DIR/self2.c)"
  fi
  CC_FILE="$DIR/self1.c"
fi
if [ -n "$CC_FILE" ] && [ -s "$CC_FILE" ]; then
  if "$PY" -I scripts/determinism-scan.py c "$CC_FILE" > "$DIR/scan-c.txt" 2>&1; then
    ok "$(tail -1 "$DIR/scan-c.txt")"
  else
    grep -F "NEW" "$DIR/scan-c.txt" | sed 's/^/        /'
    bad "$(tail -1 "$DIR/scan-c.txt") (scripts/determinism-allow.txt)"
  fi
else
  echo "  skip  no C of the compiler (COMPILER_C=<file> or SELF=1)"
fi

# The generated program: many names for the compiler's maps
gen() { # gen <n> <file>
  local n="$1" i
  {
    echo "// generated by scripts/determinism-check.sh"
    echo "pub enum Shape { Dot, Line(i64), Box(i64, i64) }"
    echo ""
    echo "fn id<T>(x: T) -> T {"
    echo "    return x;"
    echo "}"
    echo ""
    i=0
    while [ "$i" -lt "$n" ]; do
      echo "pub struct Rec$i { pub a$i: i64, pub name$i: str }"
      echo ""
      echo "fn area_$i(s: Shape) -> i64 {"
      echo "    match s {"
      echo "        Shape::Dot => { return $i; }"
      echo "        Shape::Line(l) => { return l + $i; }"
      echo "        Shape::Box(w, h) => { return w * h + $i; }"
      echo "    }"
      echo "}"
      echo ""
      echo "fn f_$i(x: i64) -> i64 {"
      echo "    var r: Rec$i = Rec$i { a$i: x + $i, name$i: \"rec number $i\" };"
      echo "    var t: i64 = id(r.a$i) + area_$i(Shape::Box(x, $((i % 7 + 1)))) + area_$i(Shape::Line($i));"
      echo "    if str_len(r.name$i) > $((i % 13)) {"
      echo "        t = t + $((i * 3 + 1));"
      echo "    }"
      if [ "$i" -gt 0 ]; then echo "    t = t + f_$((i - 1))(x % $((i + 2)));"; fi
      echo "    return t;"
      echo "}"
      echo ""
      i=$((i + 1))
    done
    echo "fn main() -> i64 {"
    echo "    var label: str = id(\"generated\");"
    echo "    print(f_$((n - 1))(5) + id(str_len(label)));"
    echo "    print(\"\\n\");"
    echo "    return 0;"
    echo "}"
  } > "$2"
}

PROGS="$*"
if [ -z "$PROGS" ]; then
  PROGS="tests/bootstrap/hello.fd tests/bootstrap/test_hashmap.fd tests/bootstrap/test_closure.fd tests/bootstrap/test_struct_eq.fd tests/bootstrap/test_egraph_budget.fd"
  if [ "$GEN_N" -gt 0 ]; then
    gen "$GEN_N" "$DIR/gen_names.fd"
    PROGS="$PROGS $DIR/gen_names.fd"
  fi
fi

OUTS="out.c emit.bin"
for t in $NAT_TARGETS; do OUTS="$OUTS emit-$t.bin"; done
[ "$HOST" = windows ] && OUTS="$OUTS emit-nat.bin"
SETARCH=""
if [ "$HOST" = posix ] && command -v setarch > /dev/null 2>&1 && setarch "$(uname -m)" -R true 2> /dev/null; then
  SETARCH="setarch $(uname -m) -R"
fi

build_all() { # build_all <src> <out dir> <run>
  local src="$1" d="$2" r="$3" pad extra="" pre=""
  rm -rf "$d"; mkdir -p "$d/tmp"
  pad=$(printf '%*s' $((r * 1500)) '' | tr ' ' 'x')
  [ "$r" = 2 ] && extra="--time"
  [ "$r" = 3 ] && pre="$SETARCH"
  (
    export TEMP="$d/tmp" TMP="$d/tmp" TMPDIR="$d/tmp" FID_DET_PAD="$pad"
    [ "$r" = 2 ] && export FID_EGRAPH_STATS=1
    # shellcheck disable=SC2086
    $pre "$FID" dump-c "$src" $extra > "$d/out.c" 2> "$d/dump-c.err"
    # shellcheck disable=SC2086
    $pre "$FID" emit "$src" $extra -o "$d/emit.bin" > /dev/null 2> "$d/emit.err"
    for t in $NAT_TARGETS; do
      # shellcheck disable=SC2086
      $pre "$FID" emit "$src" --target "$t" $extra -o "$d/emit-$t.bin" > /dev/null 2> "$d/emit-$t.err"
    done
    if [ "$HOST" = windows ]; then
      # shellcheck disable=SC2086
      "$FID" emit "$src" --nat $extra -o "$d/emit-nat.bin" > /dev/null 2> "$d/emit-nat.err"
    fi
  )
}

echo "== 3. $RUNS builds of each output"
total=0
same=0
for src in $PROGS; do
  [ -f "$src" ] || { bad "missing $src"; continue; }
  name=$(basename "$src" .fd)
  echo "$name"
  r=1
  while [ "$r" -le "$RUNS" ]; do
    build_all "$src" "$DIR/run$r/$name" "$r"
    r=$((r + 1))
  done
  for out in $OUTS; do
    a="$DIR/run1/$name/$out"
    if [ ! -s "$a" ]; then
      echo "  skip  $out (no output in run 1: see $DIR/run1/$name/*.err)"
      continue
    fi
    r=2
    diff_runs=""
    while [ "$r" -le "$RUNS" ]; do
      b="$DIR/run$r/$name/$out"
      if [ ! -s "$b" ]; then
        diff_runs="$diff_runs $r(missing)"
      elif ! cmp -s "$a" "$b"; then
        diff_runs="$diff_runs $r"
        echo "        run $r: $(cmp -l "$a" "$b" 2> /dev/null | wc -l) bytes differ; first offsets (1-based, octal values):"
        cmp -l "$a" "$b" 2> /dev/null | head -6 | sed 's/^/          /'
      fi
      r=$((r + 1))
    done
    total=$((total + 1))
    if [ -z "$diff_runs" ]; then
      same=$((same + 1))
      ok "$out ($(wc -c < "$a") bytes, $RUNS runs)"
    else
      bad "$name/$out differs in run(s)$diff_runs"
    fi
  done
done
if [ "$total" = 0 ]; then
  bad "no output was built"
fi
echo "$same of $total outputs identical in all runs"

if [ "$fail" = 0 ]; then echo "determinism-check: all passed"; else echo "determinism-check: FAILED"; fi
exit "$fail"
