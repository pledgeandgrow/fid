#!/bin/bash
# repro-check.sh — builds are the same byte for byte with the caches on and
# off (docs/roadmap/llvm-roadmap.md SP20).
#
# For each program:
#   1. `dump-c` with the binary stdlib artifact off (FID_ASTCACHE=off) twice
#      (two runs), on a fresh artifact (a miss: it is written) and again (a
#      hit: it is read): the four C files must be identical.
#   2. `emit` for each target the same way: the four executables must be
#      identical.
#   3. `emit` with the per-function native cache (FID_FNCACHE, CS6) on a
#      fresh file (a miss) and again (a hit: functions replayed from the
#      file) must equal the build without it. A hit used to differ: a
#      function folded away when the file was written was compiled again
#      and no longer folded onto its replayed copy, because the fold key
#      held label numbers (nat_ir.fd ng_fold_key_ids, SP20). Each
#      executable built on a hit is run when the host can run it, and its
#      output and exit status compared with the build without the cache.
#
# Usage:
#   scripts/repro-check.sh [program.fd ...]
#   FID=<fid>          the compiler (default release/fid.exe, else release/fid)
#   TARGETS="<t> ..."  the emit targets (default x86_64-linux; `host` is the
#                      host's default target, no --target: pe_ir on Windows,
#                      whose cache file is <FID_FNCACHE>.pe). TARGET=<t> is
#                      the same as TARGETS=<t>.
#   STRICT=1           also fail when no artifact or cache file was written
#                      (a vacuous compare)
#   On Windows the x86-64 Linux executables run in WSL (WSL_DISTRO).
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
TARGETS="${TARGETS:-${TARGET:-x86_64-linux}}"
PROGS="$*"
if [ -z "$PROGS" ]; then
  PROGS="tests/bootstrap/hello.fd tests/bootstrap/test_repro_fncache.fd tests/bootstrap/test_hashmap.fd tests/bootstrap/test_tooling_explain.fd"
fi
DIR=build/repro
mkdir -p "$DIR/run"
fail=0
vacuous=0
# WSL on Windows runs the x86-64 Linux executables (WSL_DISTRO=<name>, none: off)
WSL_D=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*)
  WSL_D="${WSL_DISTRO:-}"
  if [ -z "$WSL_D" ] && command -v wsl.exe > /dev/null 2>&1; then
    WSL_D=$(wsl.exe -l -q 2>/dev/null | tr -d '\0\r' | grep -v '^docker-desktop' | head -n 1)
  fi
  [ "$WSL_D" = none ] && WSL_D="" ;;
esac
run_linux() { # run_linux <exe>: here, or in WSL with the path as /mnt/<drive>/...
  if [ -z "$WSL_D" ]; then "$1"; return; fi
  local p
  p=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
  p="/mnt/$(echo "$p" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
  MSYS_NO_PATHCONV=1 wsl.exe -d "$WSL_D" -e "$p"   # no /mnt/... -> C:/... rewrite
}

same() { # same <label> <a> <b>: 0 when the files are identical
  if cmp -s "$2" "$3"; then
    echo "  ok    $1"
    return 0
  fi
  echo "  DIFF  $1 ($2 vs $3)"
  return 1
}

can_run() { # can_run <target>: 0 when an executable for it runs here
  case "$1" in
    x86_64-linux)
      { [ "$(uname -s)" = "Linux" ] && [ "$(uname -m)" = "x86_64" ]; } || [ -n "$WSL_D" ] ;;
    host) return 0 ;;
    *) return 1 ;;
  esac
}

run_exe() { # run_exe <target> <exe> <out>: runs a copy of the executable
  # under one fixed name, so a path the program prints (a panic, a loader
  # error) is the same for every build compared; prints the exit status
  local t="$DIR/run/prog.out"
  [ "$1" = host ] && t="$DIR/run/prog.exe"
  rm -f "$t"
  cp "$2" "$t" && chmod +x "$t" 2>/dev/null
  local s
  if [ "$1" = host ]; then "$t" > "$3" 2>&1; s=$?; else run_linux "$t" > "$3" 2>&1; s=$?; fi
  # WSL's own messages ("wsl: Failed to start the systemd user session")
  # come and go between runs: not the program's output. It can also land
  # spliced into the program's last line without its "wsl: " prefix
  # ("...big" + "e systemd user session for ..."): truncate that tail.
  [ -n "$WSL_D" ] && [ "$1" != host ] && sed -i -e '/^wsl: /d' -e 's/[a-z]* systemd user session.*$//' "$3"
  echo "$s"
}

emit_one() { # emit_one <target> <program> <out> [VAR=value ...]: one build
  local t="$1" p="$2" o="$3"
  shift 3
  if [ "$t" = host ]; then
    env "$@" "$FID" emit "$p" -o "$o" > /dev/null 2>&1
  else
    env "$@" "$FID" emit "$p" --target "$t" -o "$o" > /dev/null 2>&1
  fi
}

for p in $PROGS; do
  name=$(basename "$p" .fd)
  echo "== $name"
  art="$DIR/$name.ast.bin"
  rm -f "$art"
  FID_ASTCACHE=off "$FID" dump-c "$p" > "$DIR/$name.off.c" 2>/dev/null || { echo "  FAIL  dump-c (cache off)"; fail=1; continue; }
  FID_ASTCACHE=off "$FID" dump-c "$p" > "$DIR/$name.off2.c" 2>/dev/null || { echo "  FAIL  dump-c (cache off, second run)"; fail=1; continue; }
  FID_ASTCACHE="$art" "$FID" dump-c "$p" > "$DIR/$name.miss.c" 2>/dev/null || { echo "  FAIL  dump-c (miss)"; fail=1; continue; }
  FID_ASTCACHE="$art" "$FID" dump-c "$p" > "$DIR/$name.hit.c" 2>/dev/null || { echo "  FAIL  dump-c (hit)"; fail=1; continue; }
  same "C, two runs" "$DIR/$name.off.c" "$DIR/$name.off2.c" || fail=1
  if [ ! -s "$art" ]; then
    # this compiler keeps no artifact for dump-c: the two compares below are
    # between cache-off builds only, so they prove nothing about the cache
    echo "  note  no artifact was written ($art): the C compares are vacuous"
    vacuous=1
  fi
  same "C, artifact miss = off" "$DIR/$name.off.c" "$DIR/$name.miss.c" || fail=1
  same "C, artifact hit = off" "$DIR/$name.off.c" "$DIR/$name.hit.c" || fail=1

  for tg in $TARGETS; do
    b="$DIR/$name.$tg"
    art2="$b.ast.bin"
    rm -f "$art2"
    # the per-function cache is on by default (<out>.fides-fncache, SP6;
    # pe_ir's B5): off here, so "off" is a build with no cache at all and
    # the artifact builds differ from it only by the AST artifact
    emit_one "$tg" "$p" "$b.off.out" FID_ASTCACHE=off FID_FNCACHE=off || { echo "  FAIL  emit $tg (cache off)"; fail=1; continue; }
    emit_one "$tg" "$p" "$b.off2.out" FID_ASTCACHE=off FID_FNCACHE=off || { echo "  FAIL  emit $tg (cache off, second run)"; fail=1; continue; }
    emit_one "$tg" "$p" "$b.miss.out" FID_ASTCACHE="$art2" FID_FNCACHE=off || { echo "  FAIL  emit $tg (miss)"; fail=1; continue; }
    emit_one "$tg" "$p" "$b.hit.out" FID_ASTCACHE="$art2" FID_FNCACHE=off || { echo "  FAIL  emit $tg (hit)"; fail=1; continue; }
    same "$tg executable, two runs" "$b.off.out" "$b.off2.out" || fail=1
    if [ ! -s "$art2" ]; then
      echo "  note  no artifact was written ($art2): the emit compares are vacuous"
      vacuous=1
    fi
    same "$tg executable, artifact miss = off" "$b.off.out" "$b.miss.out" || fail=1
    same "$tg executable, artifact hit = off" "$b.off.out" "$b.hit.out" || fail=1

    fnc="$b.fncache"
    rm -f "$fnc" "$fnc.pe"
    emit_one "$tg" "$p" "$b.fmiss.out" FID_FNCACHE="$fnc" || { echo "  FAIL  emit $tg (fn cache miss)"; fail=1; continue; }
    emit_one "$tg" "$p" "$b.fhit.out" FID_FNCACHE="$fnc" || { echo "  FAIL  emit $tg (fn cache hit)"; fail=1; continue; }
    if [ ! -s "$fnc" ] && [ ! -s "$fnc.pe" ]; then
      echo "  note  no fn cache file was written ($fnc): the fn cache compares are vacuous"
      vacuous=1
    fi
    same "$tg executable, fn cache miss = off" "$b.off.out" "$b.fmiss.out" || fail=1
    same "$tg executable, fn cache hit = off" "$b.off.out" "$b.fhit.out" || fail=1
    # The hit's executable must also behave the same, where it can run here
    # (the host's, Linux x86-64 here or in WSL on Windows)
    if can_run "$tg"; then
      s1=$(run_exe "$tg" "$b.off.out" "$b.off.txt")
      s2=$(run_exe "$tg" "$b.fhit.out" "$b.fhit.txt")
      if [ "$s1" = "$s2" ] && cmp -s "$b.off.txt" "$b.fhit.txt"; then
        echo "  ok    $tg fn cache hit runs the same (exit $s1)"
      else
        echo "  DIFF  $tg fn cache hit runs differently (exit $s1 vs $s2)"
        fail=1
      fi
    fi
  done
done

if [ "$fail" != 0 ]; then
  echo "repro-check: FAIL"
  exit 1
fi
# STRICT=1: a vacuous compare (no artifact or cache file written) fails too
if [ "$vacuous" != 0 ]; then
  if [ "${STRICT:-}" = 1 ]; then
    echo "repro-check: FAIL (STRICT=1: an artifact or cache file was not written)"
    exit 1
  fi
  echo "repro-check: note: some compares were vacuous (no artifact or cache file written)"
fi
echo "repro-check: PASS"
exit 0
