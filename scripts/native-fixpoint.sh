#!/bin/bash
# native-fixpoint.sh — the self-hosted compiler built by our own backend, no
# C compiler anywhere, to a fixpoint (own-backend roadmap RL14): stage 2 is
# compiler/main.fd built by `fid emit`, stage 3 is the same source built by
# stage 2 with `emit`; the two executables must be equal byte for byte, and
# stage 3 must still run and compile a program.
#
# Usage: scripts/native-fixpoint.sh
#   FID=<fid>    stage 1: any working fid (default release/fid.exe, else
#                release/fid), e.g. the one scripts/bootstrap.sh made
#   BACKEND      what builds stages 2 and 3 (default: the host's):
#                  pe     `fid emit` (pe_ir, the default on Windows)
#                  nat    `fid emit --nat` (the machine layer, Windows x64)
#                  linux  `fid emit --target <arch>-linux` (the machine layer;
#                         the default on Linux; stages run on this host)
#   OUT=<dir>    build/native-fixpoint
#   FID_VERIFY=1 is passed through: the IR verifier and the allocation
#                checker on both stage builds
#   STAGE4=1     when stage 2 != stage 3, build stage 4 with stage 3 and say
#                whether the drift settles (stage 3 == stage 4)
# A drift is located by scripts/verify/bin-diff-locate.py (sections, and
# functions on Linux) into $OUT/drift.txt.
#
# The source is copied into $OUT/tree first and built there (AGENTS.md 26:
# another session may be editing compiler/ or stdlib/ between the stages).
# A compiler is a deep recursion: on Linux the stack limit is raised
# (ulimit -s unlimited) for the stages.
#
# Exit status: 0 at the fixpoint; 1 when a stage does not build or run;
# 2 when stage 2 and stage 3 differ (the first differing offsets are shown).
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT=$(pwd)
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; *) HOST=linux ;; esac
BACKEND="${BACKEND:-}"
[ -n "$BACKEND" ] || { [ "$HOST" = windows ] && BACKEND=pe || BACKEND=linux; }
case "$BACKEND" in
  # fid takes the file first: the flags go after it (BACKEND=nat passed
  # `emit --nat compiler/main.fd` until Nov 27 and the stage read "--nat"
  # as the file)
  pe) EFLAGS=(); EXT=.exe ;;
  nat) EFLAGS=(--nat); EXT=.exe ;;
  linux)
    case "$(uname -m)" in
      x86_64|amd64) T=x86_64-linux ;; aarch64|arm64) T=aarch64-linux ;; riscv64) T=riscv64-linux ;;
      *) echo "native-fixpoint: no native Linux target for $(uname -m)"; exit 1 ;;
    esac
    EFLAGS=(--target "$T"); EXT="" ;;
  *) echo "native-fixpoint: BACKEND must be pe, nat or linux"; exit 1 ;;
esac
OUT="${OUT:-build/native-fixpoint}"
mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"
[ "$HOST" = linux ] && ulimit -s unlimited 2> /dev/null

rm -rf "$OUT/tree"
mkdir -p "$OUT/tree"
cp -r compiler stdlib "$OUT/tree/"
cp tests/bootstrap/hello.fd "$OUT/tree/hello.fd"

stage() {  # n compiler: build stage n of compiler/main.fd with that compiler
  local n="$1" cc="$2" t0 t1
  echo "stage $n: emit compiler/main.fd ${EFLAGS[*]:-} with $(basename "$cc")"
  t0=$(date +%s)
  ( cd "$OUT/tree" && "$cc" emit compiler/main.fd ${EFLAGS[@]+"${EFLAGS[@]}"} -o "$OUT/fid$n$EXT" ) > "$OUT/stage$n.log" 2>&1
  local rc=$?
  t1=$(date +%s)
  if [ $rc != 0 ] || [ ! -s "$OUT/fid$n$EXT" ]; then
    echo "  stage $n did not build (rc $rc, $((t1 - t0)) s): $OUT/stage$n.log"
    tail -n 5 "$OUT/stage$n.log" | sed 's/^/    /'
    exit 1
  fi
  echo "  $(wc -c < "$OUT/fid$n$EXT") bytes in $((t1 - t0)) s"
}

stage 2 "$FID"
# stage 2 must work before it builds stage 3
if ! ( cd "$OUT/tree" && "$OUT/fid2$EXT" run hello.fd ) > "$OUT/hello2.txt" 2>&1; then
  echo "stage 2 does not run hello.fd: $OUT/hello2.txt"; exit 1
fi
stage 3 "$OUT/fid2$EXT"

if cmp -s "$OUT/fid2$EXT" "$OUT/fid3$EXT"; then
  echo "fixpoint: stage 2 == stage 3 ($BACKEND)"
else
  echo "NOT a fixpoint: stage 2 and stage 3 differ ($(wc -c < "$OUT/fid2$EXT") and $(wc -c < "$OUT/fid3$EXT") bytes, $(cmp -l "$OUT/fid2$EXT" "$OUT/fid3$EXT" 2> /dev/null | wc -l) differing in the common part); first offsets:"
  cmp -l "$OUT/fid2$EXT" "$OUT/fid3$EXT" | head -8 | sed 's/^/    /'
  # where: the sections, and on Linux the functions, that hold the bytes
  PY=$(command -v python3 || command -v python)
  if [ -n "$PY" ] && [ -f "$ROOT/scripts/verify/bin-diff-locate.py" ]; then
    "$PY" "$ROOT/scripts/verify/bin-diff-locate.py" "$OUT/fid2$EXT" "$OUT/fid3$EXT" > "$OUT/drift.txt" 2>&1
    sed 's/^/  /' "$OUT/drift.txt" | head -24
  fi
  # STAGE4=1: one stage more tells a drift that settles from one that does
  # not. Stage 3 == stage 4: the emit-built compilers (stages 2, 3) agree
  # and stage 1 (built another way, usually through C) compiles differently
  # from them: a miscompile in one of the two paths, so build a small
  # program with stage 1 and stage 2 and compare. Stage 3 != stage 4: the
  # output depends on more than the source (nondeterminism: run
  # scripts/verify/repro.sh with stage 2).
  if [ -n "${STAGE4:-}" ]; then
    stage 4 "$OUT/fid3$EXT"
    if cmp -s "$OUT/fid3$EXT" "$OUT/fid4$EXT"; then
      echo "  stage 3 == stage 4: stages 2 and 3 compile alike; stage 1 compiles differently from them"
    else
      echo "  stage 3 != stage 4 too: the output does not depend on the source alone"
    fi
  fi
  exit 2
fi
# stage 3 runs a program, and compiles one natively that runs. What is
# compared is the program's own output: the interpreter's stdout (the
# compiler's diagnostic lines go to stderr, kept in hello3.err), without
# CR, against the native program's. Before Nov 27 the interpreter's stderr
# was compared too, so this check could not pass.
( cd "$OUT/tree" && "$OUT/fid3$EXT" run hello.fd ) > "$OUT/hello3.txt" 2> "$OUT/hello3.err" || {
  echo "stage 3 does not run hello.fd"; tail -3 "$OUT/hello3.err"; exit 1; }
tr -d '\r' < "$OUT/hello3.txt" > "$OUT/hello3.tmp" && mv "$OUT/hello3.tmp" "$OUT/hello3.txt"
( cd "$OUT/tree" && "$OUT/fid3$EXT" emit hello.fd ${EFLAGS[@]+"${EFLAGS[@]}"} -o "$OUT/hello3$EXT" ) > /dev/null 2>&1 \
  && "$OUT/hello3$EXT" > "$OUT/hello3-native.txt" 2>&1 || {
  echo "stage 3 does not compile hello.fd natively, or it does not run"; exit 1; }
tr -d '\r' < "$OUT/hello3-native.txt" > "$OUT/hello3-native.tmp" && mv "$OUT/hello3-native.tmp" "$OUT/hello3-native.txt"
cmp -s "$OUT/hello3.txt" "$OUT/hello3-native.txt" || {
  echo "stage 3's native hello differs from its interpreted one"; diff "$OUT/hello3.txt" "$OUT/hello3-native.txt" | head -6; exit 1; }
echo "stage 3 runs and compiles hello.fd"
exit 0
