#!/bin/bash
# region-check.sh [FID] [FILE...]: the gate of proposal 0005 (regions freed
# in one step). Runs the leak gate (leak-check.sh: each program's output as
# its <name>.expected, every block freed at exit) with the region arenas on
# (FID_REGION_ARENA=1, a C build), so every program prints the same with
# arenas as without them and leaks nothing. With no files, tests/leaks/*.fd.
#
#   ./scripts/region-check.sh release/fid-dev.exe
#   ./scripts/region-check.sh release/fid-dev.exe tests/leaks/regions.fd
set -u
FID="${1:-release/fid.exe}"
shift || true
export FID_REGION_ARENA=1
exec bash "$(dirname "$0")/leak-check.sh" "$FID" "$@"
