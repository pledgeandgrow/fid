#!/bin/bash
# ircache-check.sh — the lowered-IR cache (compiler/ir_ser.fd irc_*,
# used by compiler/lower.fd). Off by default (measured Nov 22: replaying
# a lowered IrFunction costs about what lowering it does, so a warm hit
# saves nothing — release-roadmap D9, competitive CS4). Opt-in:
# FID_IRCACHE=<file>, or FID_IRCACHE=auto for <TEMP>/fid-irc[p]-<hash>.bin
# (p = `fid emit`'s pruned builds; run/dump-c/ccompile are full builds —
# a finish rewrites the file with only what the build used, so the two
# modes keep separate files). Per-function key: the function's whole AST;
# the file's salt: every declaration of the program. off/no/0/none/false,
# FIDES_NO_CACHE=1 and --no-cache turn it off. FID_IRCACHE_STATS=1 counts
# (stderr). Ver 3: a check word per entry, so a torn or damaged file stops
# loading at the first bad entry instead of replaying silently wrong IR.
# `fid serve` keeps its in-memory variant (FID_SERVE_IRCACHE=1).
#
# Usage: tests/tooling/ircache-check.sh
#   FID=<compiler>   default release/fid.exe (release/fid off Windows)
#   TARGET=<flags>   default `--nat` on Windows, `--target x86_64-linux`
#                    elsewhere
#   OUT=<dir>        default build/ircache
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. default (nothing set): no cache file is written, emit works.
#  2. FID_IRCACHE=<file>: the file is written, a warm emit gives the same
#     bytes, and FID_IRCACHE_STATS=1 counts hits on the warm run.
#  3. off by value, FIDES_NO_CACHE=1, --no-cache: nothing written even
#     with FID_IRCACHE=auto; the executable is the same bytes.
#  4. FID_IRCACHE=auto: emit makes the pruned file (fid-ircp-), `run`
#     makes the full one (fid-irc-) and leaves the pruned one alone.
#  5. a damaged cache file (a byte flipped, then cut short): emit works
#     and gives the same executable.
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
if [ $WIN = 1 ]; then
  FID="${FID:-release/fid.exe}"; TARGET="${TARGET:---nat}"
else
  FID="${FID:-release/fid}"; TARGET="${TARGET:---target x86_64-linux}"
fi
OUT="${OUT:-build/ircache}"
mkdir -p "$OUT"
OUT_ABS="$(cd "$OUT" && pwd)"
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

# The temp directory the compiler itself sees (not Git Bash's /tmp)
if [ $WIN = 1 ]; then
  TMPD=$(cmd //c "echo %TEMP%" | tr -d '\r')
else
  TMPD="${TMPDIR:-/tmp}"
fi
unset FID_IRCACHE FID_IRCACHE_STATS FIDES_NO_CACHE || true
rm -f "$TMPD"/fid-irc*.bin "$OUT"/mine.bin

# 1: default — off, no file
if "$FID" emit tests/bootstrap/hello.fd $TARGET -o "$OUT/hello0.exe" 2> "$OUT/e0.log" \
   && [ -z "$(ls "$TMPD"/fid-irc*.bin 2>/dev/null)" ] \
   && [ ! -e "$OUT/mine.bin" ]; then
  pass "1 default writes no cache"
else
  fail "1 default"
fi

# 2: explicit file, warm hit, stats
if FID_IRCACHE="$OUT_ABS/mine.bin" "$FID" emit tests/bootstrap/hello.fd $TARGET -o "$OUT/hello1.exe" 2> "$OUT/e1.log" \
   && [ -s "$OUT/mine.bin" ]; then
  pass "2 explicit file written"
else
  fail "2 explicit file"
fi
if FID_IRCACHE="$OUT_ABS/mine.bin" FID_IRCACHE_STATS=1 "$FID" emit tests/bootstrap/hello.fd $TARGET -o "$OUT/hello2.exe" 2> "$OUT/e2.log" \
   && cmp -s "$OUT/hello1.exe" "$OUT/hello2.exe" \
   && grep -q "\[ircache\] [1-9][0-9]* hits" "$OUT/e2.log"; then
  pass "2 warm hit, same bytes, counted"
else
  fail "2 warm"
fi

# 3: every off switch
rm -f "$TMPD"/fid-irc*.bin
ok3=0
FID_IRCACHE=off "$FID" emit tests/bootstrap/hello.fd $TARGET -o "$OUT/h3a.exe" 2> /dev/null && ok3=$((ok3+1))
FID_IRCACHE=auto FIDES_NO_CACHE=1 "$FID" emit tests/bootstrap/hello.fd $TARGET -o "$OUT/h3b.exe" 2> /dev/null && ok3=$((ok3+1))
FID_IRCACHE=auto "$FID" emit tests/bootstrap/hello.fd $TARGET --no-cache -o "$OUT/h3c.exe" 2> /dev/null && ok3=$((ok3+1))
if [ $ok3 = 3 ] && [ -z "$(ls "$TMPD"/fid-irc*.bin 2>/dev/null)" ] \
   && cmp -s "$OUT/hello1.exe" "$OUT/h3a.exe" && cmp -s "$OUT/hello1.exe" "$OUT/h3b.exe" \
   && cmp -s "$OUT/hello1.exe" "$OUT/h3c.exe"; then
  pass "3 off switches: no file, same bytes"
else
  fail "3 off switches"
fi

# 4: auto — pruned for emit, full for run, side by side
rm -f "$TMPD"/fid-irc*.bin
if FID_IRCACHE=auto "$FID" emit tests/bootstrap/hello.fd $TARGET -o "$OUT/hello4.exe" 2> "$OUT/e4.log"; then
  if [ -n "$(ls "$TMPD"/fid-ircp-*.bin 2>/dev/null)" ] && cmp -s "$OUT/hello1.exe" "$OUT/hello4.exe"; then
    pass "4 auto emit makes the pruned cache"
  else
    fail "4 auto emit"
  fi
else
  fail "4 auto emit"
fi
# hello prints several lines; a non-empty run output is enough
if FID_IRCACHE=auto "$FID" run tests/bootstrap/hello.fd > "$OUT/run1.txt" 2> "$OUT/run4.log" \
   && [ -s "$OUT/run1.txt" ] \
   && [ -n "$(ls "$TMPD"/fid-irc-*.bin 2>/dev/null)" ] \
   && [ -n "$(ls "$TMPD"/fid-ircp-*.bin 2>/dev/null)" ]; then
  pass "4 auto run makes the full cache, keeps the pruned one"
else
  fail "4 auto run"
fi

# 5: a damaged cache file is ignored, not fatal
M="$OUT/mine.bin"
python3 - "$M" <<'PY'
import sys
p = sys.argv[1]
d = bytearray(open(p, 'rb').read())
d[len(d) // 2] ^= 0xFF
open(p, 'wb').write(d)
PY
if FID_IRCACHE="$M" "$FID" emit tests/bootstrap/hello.fd $TARGET -o "$OUT/hello5.exe" 2> "$OUT/e5.log" \
   && cmp -s "$OUT/hello1.exe" "$OUT/hello5.exe"; then
  pass "5 flipped-byte cache ignored"
else
  fail "5 flipped-byte cache"
fi
cp "$OUT/mine.bin" "$OUT/mine.keep"
head -c 40 "$OUT/mine.keep" > "$OUT/mine.bin"   # header, records cut short
if FID_IRCACHE="$OUT_ABS/mine.bin" "$FID" emit tests/bootstrap/hello.fd $TARGET -o "$OUT/hello6.exe" 2> "$OUT/e6.log" \
   && cmp -s "$OUT/hello1.exe" "$OUT/hello6.exe"; then
  pass "5 cut-short cache ignored"
else
  fail "5 cut-short cache"
fi
echo "$bad FAIL(s)"
exit $bad
