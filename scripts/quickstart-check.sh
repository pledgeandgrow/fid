#!/usr/bin/env bash
# quickstart-check.sh — release-roadmap N2: follow the getting-started guide
# (docs/reference/quickstart.md) step by step in CI.
#
# The steps are the guide's own: scripts/guide-check.py saves each program
# the guide shows, runs each `fid`/`fides` command and each program it
# builds, and compares what they print with the guide's "Output:" blocks,
# so the guide cannot drift from the compiler. (This script used to keep
# its own copies of the guide's programs.)
#
# Usage: scripts/quickstart-check.sh [fid]     (default release/fid-dev)
#   CC=<cc>   the C compiler `fid ccompile` uses (e.g. "zig cc")
set -e
FID="${1:-}"
# a path given relative to where this was started
if [ -n "$FID" ] && { [ -e "$FID" ] || [ -e "$FID.exe" ]; }; then
  case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$(pwd)/$FID" ;; esac
fi
cd "$(dirname "$0")/.."
FID="${FID:-release/fid-dev}"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
# a native Windows python reads C:/..., not Git Bash's /c/...
if command -v cygpath > /dev/null 2>&1; then FID="$(cygpath -m "$FID")"; fi
exec "$PY" scripts/guide-check.py --fid "$FID"
