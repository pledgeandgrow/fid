#!/bin/bash
# run-a4-chain.sh — the night runners, serialized (each needs the machine):
#   1. wait for verify.sh --save (already running)          [A4 + baseline]
#   2. CB4: the compiler as a Linux ELF (quiet window now)  [retry]
#   3. A4 part 2: fidesc test --compiler <self-hosted fid>
#   4. B8: SUITE=1 inc-check.sh with the self-hosted fid
# Each appends to /tmp/night.log; the chain never stops on one failure.
cd "$(dirname "$0")/.." || exit 1
LOG=/tmp/night.log
say() { echo "[$(date +%H:%M:%S)] $*" >> "$LOG"; }

# 1. wait for the running verify.sh (its log stops growing + verify-exit line)
say "waiting for verify.sh --save"
while ! grep -q "verify-exit=" /tmp/verify_save2.log 2>/dev/null; do sleep 30; done
say "verify.sh done: $(grep 'verify-exit=' /tmp/verify_save2.log)"
grep -E "passed|failed|PASS|FAIL|results saved|baseline" /tmp/verify_save2.log | tail -8 >> "$LOG"

# 2. CB4 (the window is quiet now)
say "CB4: emit the compiler as a Linux ELF"
if ./release/fid-dev.exe emit compiler/main.fd --target x86_64-linux -o release/fid-linux > /tmp/cb4.log 2>&1; then
    say "CB4 OK: $(ls -la release/fid-linux 2>/dev/null | awk '{print $5}') bytes, magic $(head -c 4 release/fid-linux | od -An -tx1 | tr -d ' ')"
else
    say "CB4 FAILED again: $(tail -2 /tmp/cb4.log | tr '\n' ' ')"
fi

# 3. A4 part 2: the suite through the self-hosted compiler
say "A4-2: fidesc test --compiler release/fid-dev.exe"
if ./target/release/fidesc.exe test --compiler release/fid-dev.exe > /tmp/a4_selfhosted.log 2>&1; then
    say "A4-2 OK: $(tail -3 /tmp/a4_selfhosted.log | tr '\n' ' ')"
else
    say "A4-2 exit $?: $(tail -5 /tmp/a4_selfhosted.log | tr '\n' ' ')"
fi

# 4. B8: the incremental suite
say "B8: SUITE=1 inc-check.sh (hours)"
if FID=release/fid-dev.exe SUITE=1 bash scripts/inc-check.sh > /tmp/b8.log 2>&1; then
    say "B8 OK: $(tail -3 /tmp/b8.log | tr '\n' ' ')"
else
    say "B8 exit $?: $(tail -5 /tmp/b8.log | tr '\n' ' ')"
fi
say "night chain complete"
