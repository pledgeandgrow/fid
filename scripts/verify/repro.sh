#!/bin/bash
# repro.sh — reproducible builds (roadmap VF12): the same input must give the
# same bytes, whatever the directory, the temp folder, the time or the order.
#
#   FID=release/fid.exe scripts/verify/repro.sh [file.fd ...]
#
# Each program (default: a few tests of tests/bootstrap/) is copied into two
# directories with different paths, then built in each (from that
# directory, by its file name; the stdlib from FID_HOME, default the project
# root) with a different TEMP/TMP and a pause between the builds:
#   dump-c   the generated C text
#   emit     the native executable (fid's own PE writer, no C compiler)
#   ccompile the executable through the C compiler (CC, default `zig cc`)
#   emit-<target>  the machine layer's image for each of NAT_TARGETS
#            (default x86_64-linux aarch64-linux riscv64-linux; "" for none),
#            and emit-nat (`emit --nat`) on Windows
# and the two results are compared byte for byte. A difference is reported
# with the first differing offsets (cmp -l), so a timestamp, a path or a
# random value in the output can be found. COMPILER=1 also builds fid
# itself twice, from copies of compiler/ and stdlib/ in the two directories:
# `dump-c compiler/main.fd` and `ccompile compiler/main.fd` (COMPILER_CC=0:
# the C only). The fixpoint check compares stages; this compares one
# compiler with itself, and its C goes into MANIFEST as fid/main.c.
#
# Same bytes on every host (own-backend roadmap RL13): MANIFEST=<file>
# writes one "sha256  program/output" line per host-independent output
# (dump-c text, the emit-<target> images); run it on two hosts and compare:
#   MANIFEST=build/m-win.txt scripts/verify/repro.sh     (on Windows)
#   MANIFEST=build/m-linux.txt scripts/verify/repro.sh   (on Linux)
#   scripts/verify/repro.sh --compare build/m-win.txt build/m-linux.txt
# scripts/verify/repro-hosts.sh does the three steps on Windows and WSL.
# On one host, HOSTSIM=1 (default) gives the second build another stdlib
# path, user, host name, home, time zone and locale, and PAR="4" (default)
# compares builds with worker threads (FID_NAT_PAR, FID_PAR) against the
# serial ones; a difference is located by scripts/verify/bin-diff-locate.py.
#
# Exit status: 0 when every output is reproducible, 1 otherwise.
set -u
cd "$(dirname "$0")/../.." || exit 1
if [ "${1:-}" = "--compare" ]; then
  # two manifests of different hosts: every output both have must agree
  shift
  [ $# = 2 ] || { echo "usage: repro.sh --compare <manifest> <manifest>"; exit 2; }
  d=$(join -j 2 <(sort -k2 "$1") <(sort -k2 "$2") | awk '$2 != $3 { print "  DIFFERENT " $1 }')
  n=$(join -j 2 <(sort -k2 "$1") <(sort -k2 "$2") | wc -l)
  if [ -n "$d" ]; then echo "$d"; echo "hosts differ on $(echo "$d" | wc -l) of $n outputs"; exit 1; fi
  echo "hosts agree on all $n outputs"
  exit 0
fi
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
export CC="${CC:-zig cc}"
export FIDES_NO_CACHE=1
ROOT="$(pwd)"
WORK="${WORK:-build/repro}"
mkdir -p "$WORK"
WORK="$(cd "$WORK" && pwd)"
if [ $# -eq 0 ]; then
  set -- tests/bootstrap/hello.fd tests/bootstrap/test_struct_eq.fd \
         tests/bootstrap/test_closure.fd tests/bootstrap/test_hashmap.fd \
         tests/bootstrap/strings.fd tests/bootstrap/test_opt_regalloc.fd
fi
bad=0
total=0
NAT_TARGETS="${NAT_TARGETS-x86_64-linux aarch64-linux riscv64-linux}"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows; X_EXE=".exe" ;; *) HOST=posix; X_EXE="" ;; esac
OUTS="out.c emit.exe cc.exe"
for t in $NAT_TARGETS; do OUTS="$OUTS emit-$t.bin"; done
[ "$HOST" = windows ] && OUTS="$OUTS emit-nat.exe"
[ -n "${MANIFEST:-}" ] && : > "$MANIFEST"

# Another host, as far as one machine can stand in for it (RL13): with
# HOSTSIM=1 (the default) the second build also reads the stdlib from
# another, longer path (both sides get a copy of the same snapshot, so an
# edit to stdlib/ during the run cannot make them differ), and runs with
# another user, host name, home, time zone and locale. PAR="<n> ..."
# (default "4") builds side a's host-independent outputs again with
# FID_NAT_PAR=n and FID_PAR=n (the machine layer's and the checks' worker
# threads) and compares them with the serial build: a host with more cores
# must not get other bytes. PAR="" leaves that out.
HOSTSIM="${HOSTSIM:-1}"
PAR="${PAR-4}"
if [ "$HOSTSIM" != 0 ]; then
  HOME_A="$WORK/a/_home"
  HOME_B="$WORK/bb_longer_path/_another_host/opt/fides-install"
  rm -rf "$HOME_A" "$HOME_B"
  mkdir -p "$HOME_A" "$HOME_B"
  cp -r "${FID_HOME:-$ROOT}/stdlib" "$HOME_A/"
  cp -r "$HOME_A/stdlib" "$HOME_B/"
else
  HOME_A="${FID_HOME:-$ROOT}"
  HOME_B="$HOME_A"
fi
PY=$(command -v python3 || command -v python || true)
# TREE=<dir>: programs that `use compiler::<m>` (tests/nattools/*_unit.fd)
# need compiler/ beside them: <dir>/compiler is copied once into the work
# folder and from there next to each copy (a snapshot, as for the stdlib)
if [ -n "${TREE:-}" ]; then
  [ -d "$TREE/compiler" ] || { echo "repro: no $TREE/compiler"; exit 1; }
  rm -rf "$WORK/_tree"; mkdir -p "$WORK/_tree"
  cp -r "$TREE/compiler" "$WORK/_tree/"
fi

compare() {  # name a b
  total=$((total + 1))
  if cmp -s "$2" "$3"; then
    echo "  same      $1 ($(wc -c < "$2") bytes)"
  else
    bad=$((bad + 1))
    n=$(cmp -l "$2" "$3" 2>/dev/null | wc -l)
    echo "  DIFFERENT $1: $n bytes differ; first offsets (1-based, octal values):"
    cmp -l "$2" "$3" 2>/dev/null | head -8 | sed 's/^/      /'
    # the sections (and Linux functions, or C lines) that hold them
    if [ -n "$PY" ] && [ -f "$ROOT/scripts/verify/bin-diff-locate.py" ]; then
      "$PY" "$ROOT/scripts/verify/bin-diff-locate.py" "$2" "$3" --max 4 2>&1 | head -16 | sed 's/^/      /'
    fi
  fi
}

for f in "$@"; do
  [ -f "$f" ] || { echo "missing: $f"; continue; }
  name=$(basename "$f" .fd)
  echo "$name"
  for side in a bb_longer_path; do
    d="$WORK/$side/$name"
    rm -rf "$d"; mkdir -p "$d/tmp"
    cp "$f" "$d/$name.fd"
    [ -n "${TREE:-}" ] && cp -r "$WORK/_tree/compiler" "$d/"
    (
      # built where the copy is, by its name, as a user builds it: the
      # path given to fid goes into the Linux images' DWARF (R2), so an
      # absolute one makes them differ per directory. The stdlib comes from
      # FID_HOME (the project root unless set). ABSPATH=1 builds from the
      # project root with the absolute path instead (shows R2).
      export TEMP="$d/tmp" TMP="$d/tmp" TMPDIR="$d/tmp" FID_HOME="$HOME_A"
      if [ "$side" != a ]; then
        export FID_HOME="$HOME_B"
        if [ "$HOSTSIM" != 0 ]; then
          mkdir -p "$d/home"
          export USERNAME=repro-b USER=repro-b LOGNAME=repro-b \
                 COMPUTERNAME=REPRO-HOST-B HOSTNAME=repro-host-b \
                 HOME="$d/home" USERPROFILE="$d/home" \
                 TZ=Asia/Kolkata LANG=de_DE.UTF-8 LC_ALL=de_DE.UTF-8
        fi
      fi
      src="$name.fd"
      if [ -n "${ABSPATH:-}" ]; then cd "$ROOT"; src="$d/$name.fd"; else cd "$d"; fi
      "$FID" dump-c "$src" > "$d/out.c" 2> /dev/null
      "$FID" emit "$src" -o "$d/emit.exe" > /dev/null 2>&1
      "$FID" ccompile "$src" -o "$d/cc.exe" > /dev/null 2>&1
      for t in $NAT_TARGETS; do
        "$FID" emit "$src" --target "$t" -o "$d/emit-$t.bin" > /dev/null 2>&1
      done
      if [ "$HOST" = windows ]; then
        "$FID" emit "$src" --nat -o "$d/emit-nat.exe" > /dev/null 2>&1
      fi
    )
    sleep 2
  done
  A="$WORK/a/$name"; B="$WORK/bb_longer_path/$name"
  for out in $OUTS; do
    if [ -f "$A/$out" ] && [ -f "$B/$out" ]; then
      compare "$name/$out" "$A/$out" "$B/$out"
      # outputs that do not depend on the host: C text, Linux images
      case "$out" in out.c|emit-*-linux.bin)
        [ -n "${MANIFEST:-}" ] && echo "$(sha256sum < "$A/$out" | cut -d' ' -f1)  $name/$out" >> "$MANIFEST" ;;
      esac
    else
      echo "  missing   $name/$out (build failed)"
    fi
  done
  # PAR: side a's host-independent outputs again with worker threads
  for n in $PAR; do
    (
      export TEMP="$A/tmp" TMP="$A/tmp" TMPDIR="$A/tmp" FID_HOME="$HOME_A"
      export FID_NAT_PAR="$n" FID_PAR="$n"
      src="$name.fd"
      if [ -n "${ABSPATH:-}" ]; then cd "$ROOT"; src="$A/$name.fd"; else cd "$A"; fi
      "$FID" dump-c "$src" > "$A/out.par$n.c" 2> /dev/null
      for t in $NAT_TARGETS; do
        "$FID" emit "$src" --target "$t" -o "$A/emit-$t.par$n.bin" > /dev/null 2>&1
      done
      if [ "$HOST" = windows ]; then
        "$FID" emit "$src" --nat -o "$A/emit-nat.par$n.exe" > /dev/null 2>&1
      fi
    )
    for out in $OUTS; do
      case "$out" in
        out.c) p="out.par$n.c" ;;
        emit-nat.exe) p="emit-nat.par$n.exe" ;;
        emit-*.bin) p="${out%.bin}.par$n.bin" ;;
        *) continue ;;
      esac
      if [ -f "$A/$out" ] && [ -f "$A/$p" ]; then
        compare "$name/$out with $n workers" "$A/$out" "$A/$p"
      else
        echo "  missing   $name/$p (build failed)"
      fi
    done
  done
done

if [ -n "${COMPILER:-}" ]; then
  # fid itself, built twice: compiler/ and stdlib/ are copied into two
  # directories with different paths (a snapshot: another session editing
  # the tree cannot make the two differ) and each copy is built there with
  # its own TEMP, FID_HOME pointing at that copy's stdlib, and a pause
  # between: `dump-c compiler/main.fd` (the C text, host-independent, so it
  # also goes into MANIFEST) and `ccompile compiler/main.fd` (the
  # executable through CC; VF12 R1 makes it the same bytes). COMPILER_CC=0
  # leaves the executable out (it needs several GB and minutes per build).
  echo "compiler/main.fd"
  for side in a bb_longer_path; do
    d="$WORK/$side/fid-self"
    rm -rf "$d"; mkdir -p "$d/tmp"
    cp -r "$ROOT/compiler" "$ROOT/stdlib" "$d/"
    (
      cd "$d"
      export TEMP="$d/tmp" TMP="$d/tmp" TMPDIR="$d/tmp" FID_HOME="$d"
      "$FID" dump-c compiler/main.fd > "$d/main.c" 2> "$d/dump-c.log"
      if [ "${COMPILER_CC:-1}" != 0 ]; then
        "$FID" ccompile compiler/main.fd -o "$d/fid-built$X_EXE" > "$d/ccompile.log" 2>&1
      fi
    )
    sleep 2
  done
  A="$WORK/a/fid-self"; B="$WORK/bb_longer_path/fid-self"
  if [ -s "$A/main.c" ] && [ -s "$B/main.c" ]; then
    compare "fid/dump-c compiler/main.fd" "$A/main.c" "$B/main.c"
    [ -n "${MANIFEST:-}" ] && echo "$(sha256sum < "$A/main.c" | cut -d' ' -f1)  fid/main.c" >> "$MANIFEST"
  else
    total=$((total + 1)); bad=$((bad + 1))
    echo "  missing   fid/main.c (dump-c failed: $A/dump-c.log)"
  fi
  if [ "${COMPILER_CC:-1}" != 0 ]; then
    if [ -f "$A/fid-built$X_EXE" ] && [ -f "$B/fid-built$X_EXE" ]; then
      compare "fid/ccompile compiler/main.fd" "$A/fid-built$X_EXE" "$B/fid-built$X_EXE"
    else
      total=$((total + 1)); bad=$((bad + 1))
      echo "  missing   fid/fid-built$X_EXE (ccompile failed: $A/ccompile.log)"
    fi
  fi
fi

echo "$((total - bad)) of $total outputs reproducible"
[ "$bad" -eq 0 ]
