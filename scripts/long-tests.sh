#!/bin/bash
# long-tests.sh — release-roadmap A12: the suite's whole-compiler tests,
# rerun with a longer time limit than the harness's.
#
# `fidesc test` (src/main.rs, frozen) runs every compiled test binary with a
# fixed 30 s limit (run_with_timeout). A test that runs the self-hosted front
# end and its interpreter inside itself (tests/selfhost/test_spawn_gt.fd:
# ~19 s alone on the native Windows path) can pass that limit on a loaded
# machine and is then reported as a failure although its output is right.
# The tests named in scripts/long-tests.txt are such tests: verify.sh sends
# each C or PE failure of one of them here, and drops it from the results
# when this run, with LIMIT seconds, gives the interpreter's output.
#
# Usage:
#   scripts/long-tests.sh <name> <c|pe>   one test on one backend
#   scripts/long-tests.sh --all           every listed test on C (and PE on
#                                         Windows)
#   scripts/long-tests.sh --is-long <name>  exit 0 when <name> is listed
#   FIDESC=<fidesc>   default ./target/release/fidesc
#   LIMIT=<s>         the time limit per run (default 300)
# Exit status: 0 when every run matched the interpreter within LIMIT.
set -u
cd "$(dirname "$0")/.." || exit 1
FIDESC="${FIDESC:-./target/release/fidesc}"
LIMIT="${LIMIT:-300}"
LIST=scripts/long-tests.txt
EXE=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;; esac

listed() {
  [ -f "$LIST" ] && grep -v '^#' "$LIST" | awk '{print $1}' | grep -qx "$1"
}

find_test() {
  local d
  for d in tests/bootstrap tests/selfhost; do
    if [ -f "$d/$1.fd" ]; then echo "$d/$1.fd"; return 0; fi
  done
  return 1
}

# Run with a limit: `timeout` where there is one (coreutils, Git Bash),
# else without.
limited() {
  if command -v timeout > /dev/null 2>&1; then timeout "$LIMIT" "$@"; else "$@"; fi
}

check_one() {
  local name="$1" backend="$2" src dir out want got rc
  src=$(find_test "$name") || { echo "long-tests: no test $name"; return 1; }
  dir="build/long-tests/$name"
  mkdir -p "$dir"
  # The reference is the interpreter's output, as in the harness.
  if ! limited "$FIDESC" run "$src" > "$dir/interp.out" 2> "$dir/interp.err"; then
    echo "long-tests: $name: the interpreter run failed (see $dir/interp.err)"
    return 1
  fi
  out="$dir/$name-$backend$EXE"
  rm -f "$out"
  case "$backend" in
    c)  "$FIDESC" ccompile "$src" -o "$out" > "$dir/build-c.log" 2>&1 ;;
    pe) "$FIDESC" emit "$src" -o "$out" --no-cache > "$dir/build-pe.log" 2>&1 ;;
    *)  echo "long-tests: unknown backend $backend"; return 2 ;;
  esac
  if [ ! -f "$out" ]; then
    echo "long-tests: $name: the $backend build failed (see $dir/build-$backend.log)"
    return 1
  fi
  local t0 t1
  t0=$(date +%s)
  limited "./$out" > "$dir/$backend.out" 2> "$dir/$backend.err" < /dev/null
  rc=$?
  t1=$(date +%s)
  want=$(tr -d '\r' < "$dir/interp.out")
  got=$(tr -d '\r' < "$dir/$backend.out")
  if [ "$rc" = 124 ]; then
    echo "long-tests: $name ($backend): over the ${LIMIT} s limit"
    return 1
  fi
  if [ "$got" != "$want" ]; then
    echo "long-tests: $name ($backend): output differs from the interpreter's (exit $rc; $dir)"
    return 1
  fi
  echo "long-tests: $name ($backend): passed in $((t1 - t0)) s (limit ${LIMIT} s)"
  return 0
}

case "${1:-}" in
  --is-long)
    listed "${2:-}"; exit $? ;;
  --all)
    bad=0
    for name in $(grep -v '^#' "$LIST" | awk '{print $1}'); do
      check_one "$name" c || bad=$((bad + 1))
      if [ -n "$EXE" ]; then check_one "$name" pe || bad=$((bad + 1)); fi
    done
    exit $bad ;;
  ""|-h|--help)
    sed -n '2,22p' "$0"; exit 2 ;;
  *)
    check_one "$1" "${2:-c}"; exit $? ;;
esac
