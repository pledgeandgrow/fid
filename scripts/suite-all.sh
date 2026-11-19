#!/bin/bash
# suite-all.sh — the suite on every backend and target this host can run,
# each compared with the interpreter (own-backend roadmap RL2), and the two
# independent paths, the C path and our own backend, compared test by test
# (RL22: a release ships only when both agree on the suite).
#
# Usage: scripts/suite-all.sh [lane ...]
#   lane  c:<target>       the C path: scripts/cross-test.sh <target>
#         native:<target>  our backend: NATIVE=1 scripts/cross-test.sh <target>
#   Default: every lane this host can run:
#     c:x86_64-linux c:aarch64-linux c:riscv64-linux c:wasm32-wasi
#     native:x86_64-linux native:aarch64-linux native:riscv64-linux
#     and on Windows c:x86_64-windows native:x86_64-windows-pe (pe_ir, the
#     default `fid emit`) native:x86_64-windows (`emit --nat`)
#   A Linux lane needs qemu-user (or the host's own architecture); on
#   Windows it runs in WSL (WSL_DISTRO). A wasm lane needs node.
#   FID, JOBS, CC as for scripts/cross-test.sh. OUT=<dir> (build/suite-all).
#   TESTS="a.fd b.fd"  only these tests on every lane (a smoke run; the
#            release check is the whole suite, TESTS unset)
#   LIST_LANES=1  print every lane this script knows and exit (CI runs each
#            on every change: .github/workflows/suite-all.yml, tier1.yml,
#            checked by scripts/suite-lanes-check.sh)
#   AGREE=1  also fail when a test passes on one lane and fails or does not
#            build on another (the release check, docs/reference/releasing.md)
#
# Output: one line per lane (passed, failed, did not build), then each test
# whose verdict differs between lanes. Exit status: the number of failing
# lanes, plus one when AGREE=1 and some test's lanes disagree.
set -u
cd "$(dirname "$0")/.." || exit 1
OUT="${OUT:-build/suite-all}"
mkdir -p "$OUT"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; *) HOST=linux ;; esac
ARCH=$(uname -m)

can_run() {  # target -> 0 if its binaries run here
  case "$1" in
    *-windows*) [ "$HOST" = windows ] ;;
    wasm32-wasi) command -v node > /dev/null 2>&1 ;;
    x86_64-linux|aarch64-linux|riscv64-linux)
      local a="${1%%-linux}" q
      [ "$a" = aarch64 ] && q=qemu-aarch64
      [ "$a" = riscv64 ] && q=qemu-riscv64
      [ "$a" = x86_64 ] && q=""
      if [ "$HOST" = windows ]; then
        command -v wsl > /dev/null 2>&1 || return 1
        [ -z "$q" ] && return 0
        MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} -e sh -c "command -v $q" > /dev/null 2>&1
      elif [ "$HOST" = linux ]; then
        { [ "$ARCH" = "$a" ] || [ "$ARCH-$a" = "arm64-aarch64" ]; } && return 0
        [ -n "$q" ] && command -v "$q" > /dev/null 2>&1
      else
        return 1
      fi ;;
    *) return 1 ;;
  esac
}

# LIST_LANES=1: every lane this script knows, whatever this host can run
# (scripts/suite-lanes-check.sh checks CI runs each of them, RL2)
ALL_LANES="c:x86_64-linux c:aarch64-linux c:riscv64-linux c:wasm32-wasi native:x86_64-linux native:aarch64-linux native:riscv64-linux c:x86_64-windows native:x86_64-windows-pe native:x86_64-windows"
if [ "${LIST_LANES:-}" = 1 ]; then
  printf '%s\n' $ALL_LANES
  exit 0
fi

LANES=("$@")
if [ ${#LANES[@]} -eq 0 ]; then
  for t in x86_64-linux aarch64-linux riscv64-linux wasm32-wasi; do
    can_run "$t" && LANES+=("c:$t")
  done
  for t in x86_64-linux aarch64-linux riscv64-linux; do
    can_run "$t" && LANES+=("native:$t")
  done
  if [ "$HOST" = windows ]; then
    LANES+=(c:x86_64-windows native:x86_64-windows-pe native:x86_64-windows)
  fi
fi
echo "lanes: ${LANES[*]}"

failing=0
RES=()   # results.txt of each lane, in LANES order
for lane in "${LANES[@]}"; do
  kind="${lane%%:*}"; target="${lane#*:}"
  dir="$OUT/${kind}-${target}"
  if [ "$kind" = native ]; then
    line=$(NATIVE=1 OUT="$dir" bash scripts/cross-test.sh "$target" ${TESTS:-} 2>&1 | grep -aE "passed," | tail -n 1)
    res="$dir-native/results.txt"
  else
    line=$(OUT="$dir" bash scripts/cross-test.sh "$target" ${TESTS:-} 2>&1 | grep -aE "passed," | tail -n 1)
    res="$dir/results.txt"
  fi
  RES+=("$res")
  printf '%-28s %s\n' "$lane" "${line:-no summary (see $dir)}"
  # a lane fails when any test failed or did not build, or nothing ran
  # (a lane this host cannot run lists BUILT per test: built, not run)
  if [ ! -s "$res" ] || grep -aq '^FAIL\|^BUILD' "$res"; then
    failing=$((failing + 1))
  fi
done

# Test by test: a test PASS on one lane and FAIL/BUILD on another
disagree=$(
  for i in "${!LANES[@]}"; do
    [ -f "${RES[$i]}" ] || continue
    sed -E "s/^([A-Z]+) +([^ ]+).*/\2 \1 ${LANES[$i]//\//_}/" "${RES[$i]}"
  done | awk '
    $2 == "SKIP" || $2 == "BUILT" { next }
    { v[$1] = v[$1] " " $3 "=" $2; if ($2 == "PASS") p[$1] = 1; else f[$1] = 1 }
    END { for (t in v) if (p[t] && f[t]) print t ":" v[t] }' | sort)
if [ -n "$disagree" ]; then
  echo "tests whose lanes disagree:"
  echo "$disagree" | head -100 | sed 's/^/  /'
  echo "$disagree" > "$OUT/disagree.txt"
fi
st=$failing
if [ "${AGREE:-}" = 1 ] && [ -n "$disagree" ]; then st=$((st + 1)); fi
# The record and first triage of this run (each failure's cause, the tests
# grouped by the lanes they fail on, Linux binaries importing runtime
# names): $OUT/report.md, scripts/verify/suite-all-report.py
for py in python3 python; do
  if command -v "$py" > /dev/null 2>&1 && "$py" -c 'import sys; sys.exit(sys.version_info[0] < 3)' 2> /dev/null; then
    "$py" scripts/verify/suite-all-report.py "$OUT" --lanes "${LANES[*]}" --md "$OUT/report.md" \
      --compiler "${FID:-default fid} ($(date -u +%Y-%m-%d))" > "$OUT/report.txt" 2>&1
    echo "report: $OUT/report.md"
    break
  fi
done
echo "suite-all: ${#LANES[@]} lanes, $failing failing$([ -n "$disagree" ] && echo ", $(echo "$disagree" | wc -l) tests disagree")"
exit $st
