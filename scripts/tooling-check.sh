#!/bin/bash
# tooling-check.sh — the command-line tools of docs/roadmap/llvm-roadmap.md
# tracks SP and DB, run over tests/bootstrap/test_tooling_explain.fd:
#   DB7   fid explain <file> --stages <fn>: the IR after each stage, then
#         the machine code
#   DB8   fid emit <file> --remarks: optimization remarks on stderr
#   SP2   --time on check, dump-c and emit: the per-stage table
#   SP18  fid serve: requests on stdin, answers ending in `done <status>`
#   SP20  scripts/repro-check.sh on the same file (caches on and off)
#
# Usage:
#   scripts/tooling-check.sh
#   FID=<fid>   the compiler (default release/fid.exe, else release/fid)
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
SRC=tests/bootstrap/test_tooling_explain.fd
DIR=build/tooling-check
mkdir -p "$DIR"
fail=0

has() { # has <label> <file> <fixed text>
  if grep -qF -- "$3" "$2"; then
    echo "  ok    $1"
  else
    echo "  FAIL  $1: no '$3' in $2"
    fail=1
  fi
}

echo "== DB7 explain --stages"
"$FID" explain "$SRC" --stages sum_to > "$DIR/explain.txt" 2> "$DIR/explain.err"
has "IR after lowering" "$DIR/explain.txt" "=== sum_to: IR after lowering ==="
has "operands shown" "$DIR/explain.txt" "fn sum_to(%"
has "IR after iropt" "$DIR/explain.txt" "=== sum_to: IR after iropt ==="
has "IR after the e-graph" "$DIR/explain.txt" "=== sum_to: IR after the e-graph ==="
has "machine code" "$DIR/explain.txt" "=== sum_to: machine code ==="

echo "== DB8 --remarks"
"$FID" emit "$SRC" --target x86_64-linux --remarks -o "$DIR/remarks.out" > /dev/null 2> "$DIR/remarks.err"
has "check remarks" "$DIR/remarks.err" "[remark] "
has "spill remarks" "$DIR/remarks.err" "[stats] "
has "inlining remarks" "$DIR/remarks.err" " not inlined: "
has "why a check stayed" "$DIR/remarks.err" "    kept "

echo "== SP2 --time"
"$FID" check "$SRC" --time > /dev/null 2> "$DIR/check-time.err"
has "check: typecheck row" "$DIR/check-time.err" "[time] typecheck:"
has "check: total row" "$DIR/check-time.err" "[time] total:"
"$FID" dump-c "$SRC" --time > "$DIR/dump.c" 2> "$DIR/dumpc-time.err"
has "dump-c: lower row" "$DIR/dumpc-time.err" "[time] lower:"
has "dump-c: C generation row" "$DIR/dumpc-time.err" "[time] C generation:"
has "dump-c: total row" "$DIR/dumpc-time.err" "[time] total:"
"$FID" emit "$SRC" --target x86_64-linux --time -o "$DIR/time.out" > /dev/null 2> "$DIR/emit-time.err"
has "emit: native code row" "$DIR/emit-time.err" "[time] native code:"
has "emit: iropt pass rows" "$DIR/emit-time.err" "[time]   iropt "
has "emit: allocation row" "$DIR/emit-time.err" "[time]   nat allocation:"
has "emit: total row" "$DIR/emit-time.err" "[time] total:"

echo "== SP13 emit --debug (no mid-end, the fast allocator)"
"$FID" emit "$SRC" --target x86_64-linux --debug --time -o "$DIR/debug.out" > /dev/null 2> "$DIR/debug-time.err"
if [ -s "$DIR/debug.out" ]; then
  echo "  ok    debug build written"
  if grep -qF "[time] iropt:" "$DIR/debug-time.err"; then
    echo "  FAIL  the debug build ran iropt"
    fail=1
  fi
  if [ "$(uname -s)" = "Linux" ] && [ "$(uname -m)" = "x86_64" ]; then
    chmod +x "$DIR/debug.out" "$DIR/time.out"
    if [ "$("$DIR/debug.out")" = "$("$DIR/time.out")" ]; then
      echo "  ok    debug and release builds print the same"
    else
      echo "  FAIL  debug and release builds print differently"
      fail=1
    fi
  fi
  # Compile time of the two modes, for SP13's 5x (reported, not a gate)
  echo "  time  debug:   $(grep -F '[time] total:' "$DIR/debug-time.err")"
  echo "  time  release: $(grep -F '[time] total:' "$DIR/emit-time.err")"
  # The ratio, whole build and after lowering (iropt, rules, e-graph and
  # native code: what --debug changes); the front end and lowering are the
  # same work in both modes
  ms_of() { grep -F "[time] $1:" "$2" | head -1 | grep -o '[0-9]* ms' | awk '{print $1}'; }
  td=$(ms_of total "$DIR/debug-time.err"); tr=$(ms_of total "$DIR/emit-time.err")
  bd=0; br=0
  for row in iropt rules egraph "native code"; do
    v=$(ms_of "$row" "$DIR/debug-time.err"); bd=$((bd + ${v:-0}))
    v=$(ms_of "$row" "$DIR/emit-time.err"); br=$((br + ${v:-0}))
  done
  if [ -n "$td" ] && [ -n "$tr" ] && [ "$td" -gt 0 ] && [ "$bd" -gt 0 ]; then
    echo "  ratio release/debug: whole build $(awk "BEGIN{printf \"%.2f\", $tr/$td}")x, after lowering $(awk "BEGIN{printf \"%.2f\", $br/$bd}")x ($br ms vs $bd ms; SP13 asks 5x)"
  fi
else
  echo "  FAIL  emit --debug wrote nothing (see $DIR/debug-time.err)"
  fail=1
fi
# SP13's debug caches: an edit-rebuild in debug mode, warm against cold
# (the ratio is reported; the check's own failures count)
if [ -f tests/tooling/sp13-debug-cache-check.sh ]; then
  if TIME=1 FID="$FID" bash tests/tooling/sp13-debug-cache-check.sh > "$DIR/sp13-cache.txt" 2>&1; then
    echo "  ok    debug caches (tests/tooling/sp13-debug-cache-check.sh)"
  else
    echo "  FAIL  debug caches (see $DIR/sp13-cache.txt)"
    fail=1
  fi
  grep -E '^(time|ratio) ' "$DIR/sp13-cache.txt" | sed 's/^/        /'
fi

echo "== SP18 serve"
cat > "$DIR/bad.fd" <<'EOF'
fn main() -> i64 {
    var x: i64 = "not a number";
    return x;
}
EOF
printf 'check %s\ncheck %s\ncheck %s\nbogus\nquit\n' "$SRC" "$DIR/bad.fd" "$SRC" | "$FID" serve > "$DIR/serve.txt" 2> "$DIR/serve.err"
n_ok=$(grep -c "no errors" "$DIR/serve.txt")
n_done0=$(grep -c "^done 0" "$DIR/serve.txt")
n_done1=$(grep -c "^done 1" "$DIR/serve.txt")
n_done2=$(grep -c "^done 2" "$DIR/serve.txt")
if [ "$n_ok" = 2 ] && [ "$n_done0" = 2 ] && [ "$n_done1" = 1 ] && [ "$n_done2" = 1 ]; then
  echo "  ok    serve: 2 clean checks, 1 with errors, 1 unknown request"
else
  echo "  FAIL  serve: ok=$n_ok done0=$n_done0 done1=$n_done1 done2=$n_done2 (see $DIR/serve.txt)"
  fail=1
fi

echo "== SP20 repro-check"
FID="$FID" bash scripts/repro-check.sh "$SRC" || fail=1

if [ "$fail" != 0 ]; then
  echo "tooling-check: FAIL"
  exit 1
fi
echo "tooling-check: PASS"
exit 0
