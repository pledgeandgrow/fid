#!/bin/bash
# stage0-build-all.sh — cross-build the pinned stage-0 fid for every CI host
# from one generated C file (fid_wrapped.c, the 256MB-stack wrapped dump-c).
# Usage: ./scripts/stage0-build-all.sh [max-parallel-targets] (default 2;
# the unit objects are checkpointed, so a restart resumes where it stopped).
# Generated C, objects, and cross-built seeds stay under ignored build/.
set -u
cd "$(dirname "$0")/.."
S=build/stage0
N="${1:-2}"
mk() {  # mk <zig-triple> <out> <extra-libs>
  local t="$1" out="$2" libs="${3:-}"
  echo "=== $out ($t)"
  local ok=1
  for i in 0 1 2 3 4 5 6 7; do
    if [ ! -f "$S/fid_w_$i.$t.o" ]; then
      zig cc -target "$t" -O2 -w -fno-strict-aliasing -fwrapv -D_GNU_SOURCE \
        -c "$S/fid_w_$i.c" -o "$S/fid_w_$i.$t.o" || { ok=0; break; }
    fi
  done
  [ $ok = 1 ] && zig cc -target "$t" -O2 -w -o "$S/$out" \
    "$S"/fid_w_*."$t".o $libs && echo "=== $out done"
}
running=0
for spec in \
  "x86_64-linux-musl   fid-linux-x64         -lm -lpthread -ldl -lunwind" \
  "aarch64-linux-musl  fid-linux-arm64       -lm -lpthread -ldl -lunwind" \
  "x86_64-macos        fid-macos-x64         -lm -lpthread" \
  "aarch64-macos       fid-macos-arm64       -lm -lpthread" \
  "aarch64-windows-gnu fid-windows-arm64.exe -lws2_32"; do
  set -- $spec
  mk "$1" "$2" "$3" &
  running=$((running+1))
  [ $running -ge $N ] && { wait -n 2>/dev/null || wait; running=$((running-1)); }
done
wait
ls -la "$S"/fid-linux-* "$S"/fid-macos-* "$S"/fid-windows-arm64.exe 2>/dev/null | grep -v '\.o$'
