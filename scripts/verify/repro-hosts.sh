#!/bin/bash
# repro-hosts.sh — the same bytes on two hosts (own-backend roadmap RL13):
# this Windows machine and Linux under WSL, each running scripts/verify/repro.sh
# with MANIFEST=, then `repro.sh --compare` of the two manifests.
#
#   FID=release/fid.exe FID_LINUX=<linux fid> bash scripts/verify/repro-hosts.sh [file.fd ...]
#
#   FID          the Windows fid (default release/fid.exe)
#   FID_LINUX    a Linux x86-64 fid built from the same source. Without it,
#                LINUX_BUILD makes one with $FID first, from a snapshot of
#                compiler/ and stdlib/ (a compiler build: minutes, GBs):
#                  cc    $FID ccompile compiler/main.fd --target x86_64-linux
#                        (zig cc, a static musl executable; the default)
#                  emit  $FID emit compiler/main.fd --target x86_64-linux
#                        (the machine layer, no C compiler)
#   COMPILER=1   both hosts also dump the C of compiler/main.fd (repro.sh
#                COMPILER=1 COMPILER_CC=0): fid/main.c in both manifests
#   OUT=<dir>    build/repro-hosts (the Windows side; the Linux side works in
#                WSL's own file system, ~/fid-repro-hosts, so its paths are
#                another host's, not /mnt/c)
#   DISTRO=<name> the WSL distribution (default: the default one)
#   HOSTSIM, PAR, NAT_TARGETS are passed on to both repro.sh runs.
#
# The manifests hold the host-independent outputs (repro.sh: the dump-c text
# and the emit --target <arch>-linux images). Two hosts agree when every
# output both have has the same hash.
#
# Writes $OUT/record.txt (date, hosts, the two compilers' hashes, each
# side's result and the comparison). Exit status: 0 when both hosts
# reproduce their own outputs and agree with each other; 1 otherwise; 2 when
# a host could not run at all.
set -u
cd "$(dirname "$0")/../.." || exit 2
ROOT=$(pwd)
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
OUT="${OUT:-build/repro-hosts}"
mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) ;; *)
  echo "repro-hosts: run this on Windows (Git Bash); the Linux side runs in WSL"; exit 2 ;;
esac
command -v wsl.exe > /dev/null || { echo "repro-hosts: no wsl.exe"; exit 2; }
[ -x "$FID" ] || { echo "repro-hosts: no Windows fid at $FID"; exit 2; }
if [ $# -eq 0 ]; then
  # repro.sh's default programs
  set -- tests/bootstrap/hello.fd tests/bootstrap/test_struct_eq.fd \
         tests/bootstrap/test_closure.fd tests/bootstrap/test_hashmap.fd \
         tests/bootstrap/strings.fd tests/bootstrap/test_opt_regalloc.fd
fi
WSLX=(wsl.exe)
[ -n "${DISTRO:-}" ] && WSLX=(wsl.exe -d "$DISTRO")
# Git Bash rewrites arguments that look like paths: not for wsl.exe's
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*'
inwsl() { "${WSLX[@]}" -e bash -c "$1" | tr -d '\r'; return "${PIPESTATUS[0]}"; }
# C:/x/y (pwd -W) -> /mnt/c/x/y
to_mnt() { local w; w=$(cd "$(dirname "$1")" && pwd -W)/$(basename "$1")
  echo "/mnt/$(echo "${w:0:1}" | tr 'A-Z' 'a-z')${w:2}"; }

REC="$OUT/record.txt"
{
  echo "repro-hosts.sh, $(date -u '+%Y-%m-%d %H:%M UTC')"
  echo "commit: $(git rev-parse --short HEAD 2> /dev/null) ($( [ -n "$(git status --porcelain 2> /dev/null | head -1)" ] && echo 'with changes' || echo clean))"
  echo "Windows: $(uname -sr), fid $(sha256sum < "$FID" | cut -c1-16)"
  echo "programs: $*"
} > "$REC"

# --- the Linux fid --------------------------------------------------------
FID_LINUX="${FID_LINUX:-}"
if [ -z "$FID_LINUX" ]; then
  how="${LINUX_BUILD:-cc}"
  echo "building a Linux fid with $FID ($how)"
  rm -rf "$OUT/src"; mkdir -p "$OUT/src"
  cp -r compiler stdlib "$OUT/src/"
  case "$how" in
    cc) ( cd "$OUT/src" && CC="${CC:-zig cc}" "$FID" ccompile compiler/main.fd --target x86_64-linux -o "$OUT/fid-linux" ) > "$OUT/linux-build.log" 2>&1 ;;
    emit) ( cd "$OUT/src" && "$FID" emit compiler/main.fd --target x86_64-linux -o "$OUT/fid-linux" ) > "$OUT/linux-build.log" 2>&1 ;;
    *) echo "repro-hosts: LINUX_BUILD must be cc or emit"; exit 2 ;;
  esac
  if [ ! -s "$OUT/fid-linux" ]; then
    echo "repro-hosts: the Linux fid did not build: $OUT/linux-build.log"
    tail -n 5 "$OUT/linux-build.log" | sed 's/^/  /'
    echo "linux fid: did not build ($how)" >> "$REC"
    exit 2
  fi
  FID_LINUX="$OUT/fid-linux"
  echo "linux fid: built here with LINUX_BUILD=$how" >> "$REC"
fi
case "$FID_LINUX" in /*|?:*) ;; *) FID_LINUX="$ROOT/$FID_LINUX" ;; esac
[ -f "$FID_LINUX" ] || { echo "repro-hosts: no Linux fid at $FID_LINUX"; exit 2; }
echo "linux fid: $(sha256sum < "$FID_LINUX" | cut -c1-16)" >> "$REC"

PASS="HOSTSIM=${HOSTSIM:-1} PAR='${PAR-4}'"
[ -n "${NAT_TARGETS+x}" ] && PASS="$PASS NAT_TARGETS='$NAT_TARGETS'"
CMP_ENV=""
[ -n "${COMPILER:-}" ] && CMP_ENV="COMPILER=1 COMPILER_CC=0"

# --- Windows --------------------------------------------------------------
echo "== Windows"
WENV=(HOSTSIM="${HOSTSIM:-1}" PAR="${PAR-4}" FID="$FID" WORK="$OUT/win" MANIFEST="$OUT/m-win.txt")
[ -n "${NAT_TARGETS+x}" ] && WENV+=(NAT_TARGETS="$NAT_TARGETS")
[ -n "${COMPILER:-}" ] && WENV+=(COMPILER=1 COMPILER_CC=0)
env "${WENV[@]}" bash scripts/verify/repro.sh "$@" 2>&1 | tee "$OUT/win.log"
win=${PIPESTATUS[0]}
echo "windows: repro.sh exit $win, $(tail -n 1 "$OUT/win.log")" >> "$REC"

# --- Linux (WSL) ----------------------------------------------------------
echo "== Linux (WSL)"
L='$HOME/fid-repro-hosts'
inwsl "rm -rf $L && mkdir -p $L/tree/scripts $L/tree/tests" || { echo "repro-hosts: WSL does not start"; exit 2; }
# the tree the Linux side builds from: its own copy, in WSL's file system
tar -cf "$OUT/tree.tar" stdlib scripts/verify "$@" ${COMPILER:+compiler}
inwsl "cd $L/tree && tar -xf '$(to_mnt "$OUT/tree.tar")' && cp '$(to_mnt "$FID_LINUX")' $L/fid && chmod +x $L/fid" \
  || { echo "repro-hosts: copying into WSL failed"; exit 2; }
lcc="gcc"
inwsl "command -v zig > /dev/null" && lcc="zig cc"
inwsl "ulimit -s unlimited 2> /dev/null; cd $L/tree && $PASS $CMP_ENV CC='$lcc' FID=$L/fid WORK=$L/work MANIFEST=$L/m-linux.txt bash scripts/verify/repro.sh $*" 2>&1 | tee "$OUT/linux.log"
lin=${PIPESTATUS[0]}
inwsl "cat $L/m-linux.txt" > "$OUT/m-linux.txt"
echo "linux: $(inwsl 'uname -sr'), CC=$lcc, repro.sh exit $lin, $(tail -n 1 "$OUT/linux.log")" >> "$REC"

# --- the two hosts --------------------------------------------------------
echo "== Windows against Linux"
if [ ! -s "$OUT/m-win.txt" ] || [ ! -s "$OUT/m-linux.txt" ]; then
  echo "repro-hosts: a manifest is empty ($OUT/m-win.txt, $OUT/m-linux.txt)"
  echo "compare: a manifest is empty" >> "$REC"
  exit 2
fi
bash scripts/verify/repro.sh --compare "$OUT/m-win.txt" "$OUT/m-linux.txt" 2>&1 | tee "$OUT/compare.log"
cmpst=${PIPESTATUS[0]}
# outputs only one host has (a build that failed on the other)
only=$(join -v1 -v2 -j 2 <(sort -k2 "$OUT/m-win.txt") <(sort -k2 "$OUT/m-linux.txt") | awk '{print $1}')
[ -n "$only" ] && echo "on one host only: $(echo $only)" | tee -a "$OUT/compare.log"
echo "compare: exit $cmpst, $(grep -m1 'hosts ' "$OUT/compare.log")" >> "$REC"
[ -n "$only" ] && echo "on one host only: $(echo $only)" >> "$REC"
echo "record: $REC"
[ "$win" = 0 ] && [ "$lin" = 0 ] && [ "$cmpst" = 0 ] && [ -z "$only" ]
