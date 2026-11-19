#!/bin/bash
# rl3-matrix-all.sh — the pass on/off matrix (scripts/pass-matrix.sh) on the
# whole suite for every native target this host can run, resumable, then
# one record of all of them (own-backend roadmap RL3).
#
# Usage: scripts/verify/rl3-matrix-all.sh [target ...]
#   target: x86_64-linux aarch64-linux riscv64-linux x86_64-windows
#           (default: every one this host runs: the Linux ones here, in WSL
#           on Windows, under qemu-user for AArch64/RISC-V; x86_64-windows,
#           `emit --nat`, on Windows)
#   FID, JOBS, WSL_DISTRO as for scripts/cross-test.sh; IROPT, NAT, IME,
#   EGRAPH, RULES as for scripts/pass-matrix.sh (default: every switch the
#   compiler's source reads)
#   OUT=<dir>   default build/pm (one folder per target)
#   FRESH=1     start again (default: RESUME=1, the runs that finished
#               before are kept: the whole matrix is ~95 suite runs per
#               target, so it is meant to be done in several sittings with
#               the same FID)
#   TESTS="a.fd b.fd"  only these tests (a smoke run)
#
# Output: each target's $OUT/<target>/matrix.txt, and $OUT/report.md
# (scripts/verify/pass-matrix-report.py: the switches that differ, their
# tests, tests differing under many switches, switches differing on every
# target). Copy report.md to docs/verification/ to record the run.
# Exit status: that of pass-matrix-report.py (0: every matrix finished and
# no switch differs).
set -u
cd "$(dirname "$0")/../.." || exit 1
OUT="${OUT:-build/pm}"
mkdir -p "$OUT"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; *) HOST=linux ;; esac
ARCH=$(uname -m)

runs_here() {  # target -> 0 when this host runs its native binaries
  local q=""
  case "$1" in
    x86_64-windows) [ "$HOST" = windows ]; return ;;
    aarch64-linux) q=qemu-aarch64 ;;
    riscv64-linux) q=qemu-riscv64 ;;
    x86_64-linux) q="" ;;
    *) return 1 ;;
  esac
  if [ "$HOST" = windows ]; then
    command -v wsl > /dev/null 2>&1 || return 1
    [ -z "$q" ] && return 0
    MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} -e sh -c "command -v $q" > /dev/null 2>&1
  elif [ "$HOST" = linux ]; then
    [ "${1%%-linux}" = "$ARCH" ] && return 0
    [ "$ARCH-${1%%-linux}" = arm64-aarch64 ] && return 0
    [ -n "$q" ] && command -v "$q" > /dev/null 2>&1
  else
    return 1
  fi
}

TARGETS=("$@")
if [ ${#TARGETS[@]} -eq 0 ]; then
  for t in x86_64-linux aarch64-linux riscv64-linux x86_64-windows; do
    runs_here "$t" && TARGETS+=("$t")
  done
fi
[ ${#TARGETS[@]} -gt 0 ] || { echo "rl3-matrix-all: this host runs none of the native targets"; exit 2; }
echo "rl3-matrix-all: ${TARGETS[*]} ($([ "${FRESH:-}" = 1 ] && echo fresh || echo resuming))"
resume=1
[ "${FRESH:-}" = 1 ] && resume=""
for t in "${TARGETS[@]}"; do
  echo "== $t"
  # shellcheck disable=SC2086
  RESUME=$resume OUT="$OUT/$t" bash scripts/pass-matrix.sh "$t" ${TESTS:-} | tail -n 1
done
PY=""
for p in python3 python; do
  if command -v "$p" > /dev/null 2>&1 && "$p" -c 'import sys; sys.exit(sys.version_info[0] < 3)' 2> /dev/null; then PY=$p; break; fi
done
[ -n "$PY" ] || { echo "rl3-matrix-all: no python 3 for the report; see $OUT/<target>/matrix.txt"; exit 1; }
args=()
for t in "${TARGETS[@]}"; do args+=("$OUT/$t/matrix.txt"); done
"$PY" scripts/verify/pass-matrix-report.py "${args[@]}" --md "$OUT/report.md" \
  --compiler "${FID:-default fid} ($(date -u +%Y-%m-%d))"
st=$?
echo "report: $OUT/report.md"
exit $st
