#!/bin/bash
# pass-matrix.sh — every optimization switch off in turn must give the same
# results (own-backend roadmap RL3).
#
# Usage: scripts/pass-matrix.sh <target> [test.fd ...]
#   target: aarch64-linux | riscv64-linux (the native backend, NATIVE=1)
#   FID, JOBS, WSL_DISTRO as for scripts/cross-test.sh
#
# For each switch the suite (or the files given) is built and run through
# scripts/cross-test.sh with that switch off: iropt's passes through
# FID_IROPT_OFF, the native backend's through FID_NAT_OFF. A line per
# switch with its pass count; a switch whose count differs from the
# all-on run is reported as a regression to look at.
set -u
cd "$(dirname "$0")/.." || exit 1
TARGET="${1:-}"
if [ -z "$TARGET" ]; then sed -n '2,13p' "$0"; exit 1; fi
shift
IROPT="inline fold dce tailcall gvn licm"
NAT="mir promote ipra fold hint rules purecse weights vecinline unchecked coalesce"
mkdir -p build/pm
base=$(env NATIVE=1 OUT=build/pm/all bash scripts/cross-test.sh "$TARGET" "$@" 2>&1 | grep -E "passed" | tail -n 1)
echo "all on:            $base"
for p in $IROPT; do
  r=$(env FID_IROPT_OFF="$p" NATIVE=1 OUT="build/pm/iropt-$p" bash scripts/cross-test.sh "$TARGET" "$@" 2>&1 | grep -E "passed" | tail -n 1)
  flag=""; [ "${r%% passed*}" != "${base%% passed*}" ] && flag="   <- differs"
  printf 'iropt %-12s %s%s\n' "$p off:" "$r" "$flag"
done
for p in $NAT; do
  r=$(env FID_NAT_OFF="$p" NATIVE=1 OUT="build/pm/nat-$p" bash scripts/cross-test.sh "$TARGET" "$@" 2>&1 | grep -E "passed" | tail -n 1)
  flag=""; [ "${r%% passed*}" != "${base%% passed*}" ] && flag="   <- differs"
  printf 'nat   %-12s %s%s\n' "$p off:" "$r" "$flag"
done
