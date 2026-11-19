#!/bin/bash
# native-fixpoint-check.sh — scripts/native-fixpoint.sh (RL14) gives the
# right verdict, checked with stand-in compilers instead of fid, so the check
# runs in seconds and needs no build (ci-wiring-check.sh runs it in the
# gates job). Each stand-in is a shell script that answers `run hello.fd`
# and `emit <src> -o <out>` the way a compiler at a fixpoint would, or not:
#
#   fixed    emit writes a copy of itself: stage 2 == stage 3   -> exit 0
#   drift    each emit adds a line: stage 2 != stage 3           -> exit 2
#   broken   emit fails                                          -> exit 1
#   norun    stage 2 cannot run hello.fd                         -> exit 1
#   badhello stage 3's native hello prints something else        -> exit 1
#   settle   stage 1 adds a line the built stages leave out: stage 2 !=
#            stage 3 == stage 4 (STAGE4=1 says so)                -> exit 2
# and, with STAGE4=1, that drift says stage 3 != stage 4 too and that the
# drift is located in drift.txt (scripts/verify/bin-diff-locate.py).
#
# Usage: scripts/native-fixpoint-check.sh    (OUT=<dir>, default a temp dir)
# Exit status: the number of cases whose verdict is wrong.
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT=$(pwd)
OUT="${OUT:-$(mktemp -d 2>/dev/null || echo build/native-fixpoint-check)}"
mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"
want_hello="$(cat tests/bootstrap/hello.fd > /dev/null 2>&1 && echo 'Hello, World!')"

fake() {  # fake <mode> <path>
  cat > "$2" <<EOF
#!/bin/bash
# a stand-in compiler for native-fixpoint-check.sh (mode $1)
MODE=$1
EOF
  cat >> "$2" <<'EOF'
if [ $# -eq 0 ]; then
  # a program this "compiler" built: hello
  [ "$MODE" = badhello ] && [ -n "${FAKE_STAGE3:-}" ] && { echo "goodbye"; exit 0; }
  echo "Hello, World!"; exit 0
fi
case "$1" in
  run)
    [ "$MODE" = norun ] && [ -n "${FAKE_BUILT:-}" ] && exit 1
    echo "Hello, World!"; exit 0 ;;
  emit)
    [ "$MODE" = broken ] && { echo "error: emit failed" >&2; exit 1; }
    out=""; src=""; prev=""
    for a in "$@"; do
      [ "$prev" = -o ] && out="$a"
      case "$a" in *.fd) src="$a" ;; esac
      prev="$a"
    done
    [ -n "$out" ] && [ -n "$src" ] || exit 1
    if [ "$src" = compiler/main.fd ]; then
      # the next stage: this compiler, marked as one that was built
      if grep -q '^FAKE_BUILT=1$' "$0"; then
        # settle: a built stage leaves out what stage 1 put in
        if [ "$MODE" = settle ]; then grep -v '^# stage1$' "$0" > "$out"; else cp "$0" "$out"; fi
      else
        { head -n 3 "$0"; echo "FAKE_BUILT=1"; tail -n +4 "$0"; } > "$out"
        [ "$MODE" = settle ] && echo "# stage1" >> "$out"
      fi
      [ "$MODE" = drift ] && echo "# built $RANDOM$RANDOM $(date +%s%N)" >> "$out"
    else
      # a program: a copy too, which prints hello when run with no arguments
      if [ "$MODE" = badhello ]; then { head -n 3 "$0"; echo "FAKE_STAGE3=1"; tail -n +4 "$0"; } > "$out"
      else cp "$0" "$out"; fi
    fi
    chmod +x "$out"; exit 0 ;;
esac
exit 1
EOF
  chmod +x "$2"
}

bad=0
expect() {  # expect <mode> <status>
  local mode="$1" want="$2" got
  fake "$mode" "$OUT/fake-$mode"
  # BACKEND=linux: extensionless stages, which Git Bash runs as scripts too
  local cc="$OUT/fake-$mode"
  OUT="$OUT/run-$mode" FID="$cc" BACKEND=linux \
    bash scripts/native-fixpoint.sh > "$OUT/$mode.log" 2>&1
  got=$?
  if [ "$got" = "$want" ]; then
    echo "ok    $mode: exit $got"
  else
    echo "WRONG $mode: exit $got, want $want ($OUT/$mode.log)"
    sed 's/^/      /' "$OUT/$mode.log" | tail -n 6
    bad=$((bad + 1))
  fi
}
expect fixed 0
expect drift 2
expect broken 1
expect norun 1
expect badhello 1
# STAGE4=1 says whether a drift settles; the drift is located (drift.txt)
says() {  # says <mode> <text>: the last run of <mode> printed <text>
  if grep -qF "$2" "$OUT/$1.log"; then echo "ok    $1: says \"$2\""
  else echo "WRONG $1: does not say \"$2\" ($OUT/$1.log)"; bad=$((bad + 1)); fi
}
STAGE4=1 expect settle 2
says settle "stage 3 == stage 4"
STAGE4=1 expect drift 2
says drift "stage 3 != stage 4 too"
if command -v python3 > /dev/null || command -v python > /dev/null; then
  if [ -s "$OUT/run-drift/drift.txt" ] && grep -q "first differing lines" "$OUT/run-drift/drift.txt"; then
    echo "ok    drift: located in drift.txt"
  else
    echo "WRONG drift: no location in $OUT/run-drift/drift.txt"; bad=$((bad + 1))
  fi
fi
[ -n "$want_hello" ] || true
echo "native-fixpoint-check: $bad wrong"
exit "$bad"
