#!/bin/bash
# cc-one.sh — compile build/selfhost/fid-dev_wrapped.c into release/fid-dev.exe
# with a private zig cache and retries (zig cc dies silently with exit 5
# when Windows cannot spawn sub-processes: SystemResources, usually another
# build running at the same time).
# Generate the wrapper first with KEEP_BUILD_OUTPUTS=1 scripts/build-selfhost.sh.
set -u
cd "$(dirname "$0")/.." || exit 1
export ZIG_GLOBAL_CACHE_DIR="${ZIG_CACHE:-/tmp/zigcache-mine}"
CFLAGS=(-O0 -w -DP_NO_ENV_LIMITS -fno-sanitize=all -Wno-error -Wno-int-conversion
        -Wno-incompatible-pointer-types -Wno-implicit-function-declaration -Wno-return-type)
try() {
  for i in 1 2 3 4 5; do
    if zig cc "${CFLAGS[@]}" -c build/selfhost/fid-dev_wrapped.c -o /tmp/fid-dev.o 2>/tmp/cc-one-err.txt; then
      if zig cc /tmp/fid-dev.o -o release/fid-dev.exe -lws2_32 2>>/tmp/cc-one-err.txt; then
        echo "OK: release/fid-dev.exe ($(date +%H:%M:%S), wrapped.c $(stat -c %y build/selfhost/fid-dev_wrapped.c | cut -c1-19))"
        exit 0
      fi
    fi
    echo "attempt $i failed:"; head -2 /tmp/cc-one-err.txt
    sleep 15
  done
  exit 1
}
try
