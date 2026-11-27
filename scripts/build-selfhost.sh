#!/bin/bash
# build-selfhost.sh — build the self-hosted Fides compiler.
#
# Build workspace (OUT, default `build/selfhost`):
#   $OUT/fid-dev.c    — generated C source (fidesc dump-c compiler/main.fd)
#   $OUT/fid-dev.exe  — temporary compiled binary (with 256MB-stack wrapper)
# The default build publishes only fid-dev(.exe) into release/. Set
# KEEP_BUILD_OUTPUTS=1 to retain the generated C, logs, and temporary binary.
#
# Usage:
#   ./scripts/build-selfhost.sh            # dump-c + compile
#   OUT=/tmp/fid-dev-next ./scripts/build-selfhost.sh  # custom workspace
#   STAGE0=./build/bootstrap/fides-c3 ./scripts/build-selfhost.sh   # self-compile
#
# C compiler: $CC if set, else the first of tcc, gcc, clang, `zig cc`.
# The generated C is split into 8 units built at -O2 when the stage-0
# compiler has `split-c` (SPLIT, JOBS), else one ~70MB unit at -O0
# (optimizing it whole needs >16GB RAM with clang).
set -u
cd "$(dirname "$0")/.." || exit 1
PUBLISH_DEV=0
if [ -z "${OUT:-}" ]; then
  OUT=build/selfhost
  PUBLISH_DEV=1
fi
# As scripts/bootstrap.sh: prefer the pinned self-hosted stage 0 (which has
# `split-c` and builds 8 units at -O2) over a cold fidesc start.
STAGE0="${STAGE0:-}"
if [ -z "$STAGE0" ]; then
  for cand in "release/stage0/fid.exe" "release/stage0/fid"; do
    [ -x "$cand" ] && STAGE0="$cand" && break
  done
  # No pinned binary yet: fetch this host's (CI). A fidesc fallback cannot
  # self-compile the tree (2g.79); FID_STAGE0_FETCH=0 opts out.
  if [ -z "$STAGE0" ] && [ "${FID_STAGE0_FETCH:-1}" != 0 ]; then
    ./scripts/stage0-fetch.sh || true
    for cand in "release/stage0/fid.exe" "release/stage0/fid"; do
      [ -x "$cand" ] && STAGE0="$cand" && break
    done
  fi
  STAGE0="${STAGE0:-./target/release/fidesc}"
fi
mkdir -p "$OUT"

if [ -n "${CC:-}" ]; then
  CC_CMD=($CC)
elif command -v tcc >/dev/null; then CC_CMD=(tcc)
elif command -v gcc >/dev/null; then CC_CMD=(gcc)
elif command -v clang >/dev/null; then CC_CMD=(clang)
elif command -v zig >/dev/null; then CC_CMD=(zig cc)
else echo "error: no C compiler found (set CC)"; exit 1; fi
# -DP_NO_ENV_LIMITS: FID_MEM_LIMIT / FID_CPU_LIMIT in the environment limit
# the programs fid builds, not fid itself while it compiles
CFLAGS=(-O0 -w -DP_NO_ENV_LIMITS)
case "${CC_CMD[*]}" in
  *tcc*) ;;
  *) CFLAGS+=(-fno-sanitize=all -Wno-error -Wno-int-conversion -Wno-incompatible-pointer-types
              -Wno-implicit-function-declaration -Wno-return-type) ;;
esac
LIBS=()
OUT_EXE=fid-dev
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) LIBS=(-lws2_32); OUT_EXE=fid-dev.exe ;; Darwin) LIBS=(-lm -lpthread); CFLAGS+=(-D_XOPEN_SOURCE=600 -D_DARWIN_C_SOURCE) ;; *) LIBS=(-lm -lpthread -ldl); case "${CC_CMD[*]}${CC:-}" in *zig*) LIBS+=(-lunwind) ;; esac ;; esac

echo "[1/3] $STAGE0 dump-c compiler/main.fd > $OUT/fid-dev.c"
if ! "$STAGE0" dump-c compiler/main.fd > "$OUT/fid-dev.c" 2> "$OUT/fid-dev.log"; then
  echo "dump-c failed:"; grep -E "error" "$OUT/fid-dev.log" | head -20; exit 1
fi
echo "      ($(wc -c < "$OUT/fid-dev.c") bytes)"

# Rename main and run it on a thread with a 256MB stack (compiler recurses deeply).
echo "[2/3] wrapping main with 256MB-stack thread"
sed 's/^int main(int argc, char\*\* argv)/int fides_main(int argc, char** argv)/' "$OUT/fid-dev.c" > "$OUT/fid-dev_wrapped.c"
cat >> "$OUT/fid-dev_wrapped.c" <<'EOF'

// === Stack wrapper (added by scripts/build-selfhost.sh) ===
typedef struct { int argc; char** argv; int ret; } MainArgs;
#ifdef _WIN32
#include <windows.h>
static DWORD WINAPI thread_main(LPVOID p) { MainArgs* a = (MainArgs*)p; a->ret = fides_main(a->argc, a->argv); return 0; }
int main(int argc, char** argv) {
    MainArgs a = { argc, argv, 0 };
    HANDLE h = CreateThread(NULL, 256*1024*1024, thread_main, &a, 0, NULL);
    if (!h) return 1;
    WaitForSingleObject(h, INFINITE);
    CloseHandle(h);
    return a.ret;
}
#else
#include <pthread.h>
static void* thread_main(void* p) { MainArgs* a = (MainArgs*)p; a->ret = fides_main(a->argc, a->argv); return 0; }
int main(int argc, char** argv) {
    MainArgs a = { argc, argv, 0 };
    pthread_attr_t attr; pthread_t t;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, (size_t)256*1024*1024);
    if (pthread_create(&t, &attr, thread_main, &a) != 0) return 1;
    pthread_join(t, NULL);
    return a.ret;
}
#endif
EOF

echo "[3/3] ${CC_CMD[*]} -> $OUT/$OUT_EXE"
# As scripts/bootstrap.sh: when the stage-0 compiler has `fid split-c`, the C
# is split into SPLIT units (default 16) compiled at -O2, JOBS at a time
# (default 2: 4 at a time kills a memory-tight runner before the retry logic
# can help — the step is SIGTERMed whole, exit 143; override JOBS locally), and linked; the compiler is about 3x faster than one unit at
# -O0 (the Rust bootstrap, tcc or SPLIT=1: one unit at -O0). A unit that
# fails is retried alone, then the build stops with exit 1 (2g.86).
# The generated C is the same either way: only the C compiler's work differs.
SPLIT="${SPLIT:-16}"
JOBS="${JOBS:-2}"
# SPLIT_OPT: the optimization level of the first build of every unit (-O2).
# SPLIT_OPT=-O1 or -O0 when the machine cannot commit the memory the giant
# units take at -O2 (Nov 27: 44 GB of a 48 GB commit limit taken by other
# programs, the units died at once, exit 5): the generated C is the same.
SPLIT_OPT="${SPLIT_OPT:--O2}"
SPLIT_CFLAGS=("$SPLIT_OPT" -fno-strict-aliasing -fwrapv)
for f in "${CFLAGS[@]}"; do [ "$f" != "-O0" ] && SPLIT_CFLAGS+=("$f"); done
built=0
: > "$OUT/fid-dev.cc.log"
base="$OUT/fid-dev_wrapped_u"
case "${CC_CMD[*]}" in *tcc*) ;; *)
  if [ "$SPLIT" -gt 1 ] && "$STAGE0" split-c "$OUT/fid-dev_wrapped.c" "$base" "$SPLIT" >> "$OUT/fid-dev.cc.log" 2>&1 && [ -f "$base.h" ]; then
    # 2g.86: a unit that fails is named with its exit code and built again
    # alone (a C compiler killed for memory at JOBS at a time, exit 5 with
    # no message, usually builds by itself); one that still fails, or a
    # failed link, stops the build with exit 1. Before, any failure fell
    # back to one unit at -O0 (a ~17x slower fid) and the script exited 0.
    objs=(); pids=(); pid_units=(); failed=(); retried=()
    for k in $(seq 0 $((SPLIT - 1))); do
      "${CC_CMD[@]}" "${SPLIT_CFLAGS[@]}" -c "${base}_$k.c" -o "${base}_$k.o" >> "$OUT/fid-dev.cc.log" 2>&1 &
      pids+=($!)
      pid_units+=("$k")
      objs+=("${base}_$k.o")
      if [ ${#pids[@]} -ge "$JOBS" ]; then
        wait "${pids[0]}"; rc=$?
        [ "$rc" -ne 0 ] && failed+=("${pid_units[0]}:$rc")
        pids=("${pids[@]:1}")
        pid_units=("${pid_units[@]:1}")
      fi
    done
    for i in "${!pids[@]}"; do
      wait "${pids[$i]}"; rc=$?
      [ "$rc" -ne 0 ] && failed+=("${pid_units[$i]}:$rc")
    done
    if [ ${#failed[@]} -gt 0 ]; then
      for fu in "${failed[@]}"; do
        echo "      unit ${base}_${fu%%:*}.c failed at $SPLIT_OPT (exit ${fu##*:})"
      done
      echo "      building the failed units again, one at a time"
      still=()
      for fu in "${failed[@]}"; do
        k="${fu%%:*}"
        "${CC_CMD[@]}" "${SPLIT_CFLAGS[@]}" -c "${base}_$k.c" -o "${base}_$k.o" >> "$OUT/fid-dev.cc.log" 2>&1; rc=$?
        [ "$rc" -ne 0 ] && still+=("$k:$rc")
      done
      if [ ${#still[@]} -gt 0 ]; then
        # the giant typechecker units exceed llvm's memory at any
        # optimization level above -O0 even on a big machine (the stage0
        # cross-build measured this): fall to -O1, then -O0, per unit
        final=()
        retry_opts=()
        case "$SPLIT_OPT" in -O2) retry_opts=(-O1 -O0) ;; -O1) retry_opts=(-O0) ;; esac
        for fu in "${still[@]}"; do
          k="${fu%%:*}"
          ok=0
          for opt in "${retry_opts[@]}"; do
            echo "      unit ${base}_$k.c retrying at $opt (the giant-unit memory wall)"
            # the same flags as the first build with the optimization level
            # (before Nov 27 the retry had neither -w nor the -Wno-*: the
            # generated C's int-to-pointer conversions were errors then)
            cflags=("${SPLIT_CFLAGS[@]/$opt}")
            if "${CC_CMD[@]}" "${cflags[@]}" -c "${base}_$k.c" -o "${base}_$k.o" >> "$OUT/fid-dev.cc.log" 2>&1; then
              ok=1
              retried+=("$k$opt")
              break
            fi
          done
          [ "$ok" -eq 0 ] && final+=("$k")
        done
        if [ ${#final[@]} -gt 0 ]; then
          for k in "${final[@]}"; do
            echo "error: unit ${base}_$k.c unbuildable at $SPLIT_OPT and below (no memory to commit: see the commit limit)"
          done
          echo "      see $OUT/fid-dev.cc.log; SPLIT=1 builds one unit at -O0 (about 17x slower)"
          grep -E "error" "$OUT/fid-dev.cc.log" | head -20
          rm -f "${base}"_*.c "${base}"_*.o "$base.h"
          exit 1
        fi
      fi
    fi
    if "${CC_CMD[@]}" "${objs[@]}" -o "$OUT/$OUT_EXE" "${LIBS[@]}" >> "$OUT/fid-dev.cc.log" 2>&1; then
      if [ "${#retried[@]}" -gt 0 ]; then
        echo "      ($SPLIT units: $(( SPLIT - ${#retried[@]} )) at $SPLIT_OPT, ${#retried[@]} below it: unit ${retried[*]})"
      else
        echo "      ($SPLIT units at $SPLIT_OPT)"
      fi
      built=1
    else
      rc=$?
      echo "error: linking the $SPLIT units failed (exit $rc); see $OUT/fid-dev.cc.log"
      grep -E "error" "$OUT/fid-dev.cc.log" | head -20
      rm -f "${base}"_*.c "${base}"_*.o "$base.h"
      exit 1
    fi
    rm -f "${base}"_*.c "${base}"_*.o "$base.h"
  fi ;;
esac
if [ "$built" = 0 ] && ! "${CC_CMD[@]}" "${CFLAGS[@]}" "$OUT/fid-dev_wrapped.c" -o "$OUT/$OUT_EXE" "${LIBS[@]}" >> "$OUT/fid-dev.cc.log" 2>&1; then
  echo "C compile failed:"; grep -E "error" "$OUT/fid-dev.cc.log" | head -20; exit 1
fi
if [ "$PUBLISH_DEV" = 1 ]; then
  mkdir -p release
  cp "$OUT/$OUT_EXE" "release/$OUT_EXE"
  if [ "${KEEP_BUILD_OUTPUTS:-0}" != 1 ]; then
    rm -f "$OUT/fid-dev.c" "$OUT/fid-dev_wrapped.c"
    rm -f "$OUT/fid-dev.log" "$OUT/fid-dev.cc.log"
    rm -f "$OUT/fid-dev.pdb" "$OUT/$OUT_EXE"
  fi
  echo "OK: release/$OUT_EXE"
else
  echo "OK: $OUT/$OUT_EXE"
fi
