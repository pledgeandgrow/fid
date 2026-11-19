#!/bin/bash
# cross-from-linux.sh — TL6 (competitive roadmap): a Linux fid builds Windows
# executables with no C compiler, no linker and no download, and they run
# on Windows. `fid build/emit --target windows-x64` goes to pe_ir (machine
# code and the PE writer are Fides) on any host; `ccompile --target` is the
# C route and needs zig by design (docs/reference/cross-compilation.md).
#
# On a Windows host (Git Bash) with WSL:
#   1. a Linux fid: compiler/main.fd built by this host's fid with
#      `fid emit --target x86_64-linux` (the machine layer: no C compiler;
#      VIA_C=1: `fid ccompile --target x86_64-linux`, which needs zig), from
#      a snapshot of compiler/ and stdlib/ (AGENTS.md 26). LINUX_FID=<path>
#      uses a Linux fid built before instead.
#   2. in WSL, the Linux fid runs hello.fd and builds it with
#      `fid build --target windows-x64`; the PE is run here on Windows and
#      must print what `fid run` prints (here and in WSL).
#   3. in WSL, tests/tooling/build-no-toolchain-check.sh with the Linux fid
#      and a PATH with no cc/gcc/clang/zig/ld (its steps 1-3).
#   4. SUITE=1 (default): in WSL, `NATIVE=1 BUILD_ONLY=1 cross-test.sh <lane>`
#      for each of LANES (default x86_64-windows-pe: pe_ir; also
#      x86_64-windows: `emit --nat`) with the Linux fid; the PEs then run
#      here on Windows (the lane's run-all.sh) and are compared with
#      `fid run` (COMPARE_ONLY=1). WSL's interop is not used to run them: it
#      does not pass the exit code back. COMPARE_HOST=1 (default) also builds
#      the lane with this host's fid: every test that passes built on
#      Windows must pass built on Linux.
# On a Linux host: FID is a Linux fid; steps 2-4 run here, the PEs under
# wine when it is installed (WINE=<command>; else they are built only).
#
# Usage: scripts/cross-from-linux.sh
#   FID=<fid>        this host's fid (default release/fid.exe, release/fid)
#   OUT=<dir>        build/tl6 (relative to the root: run-all.sh goes back)
#   JOBS=<n>         4;  WSL_DISTRO=<name>: the WSL distribution
# Exit status: the number of failed steps (0: every step passed).
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT=$(pwd)
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; *) HOST=linux ;; esac
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
OUT="${OUT:-build/tl6}"
case "$OUT" in /*|?:*) echo "cross-from-linux: OUT must be relative to the root ($OUT)"; exit 1 ;; esac
OUT="${OUT%/}"
JOBS="${JOBS:-4}"
LANES="${LANES:-x86_64-windows-pe}"
SUITE="${SUITE:-1}"
COMPARE_HOST="${COMPARE_HOST:-1}"
mkdir -p "$OUT"
fails=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fails=$((fails + 1)); }

# The reference: what `fid run` prints on this host
REF=$("$FID" run tests/bootstrap/hello.fd 2>/dev/null | tr -d '\r')
if [ -z "$REF" ]; then echo "cross-from-linux: $FID does not run hello.fd"; exit 1; fi

if [ "$HOST" = windows ]; then
  # C:/x/y -> /mnt/c/x/y; MSYS must not rewrite the paths given to wsl
  wsl_path() { cygpath -m "$1" | sed -E 's#^([A-Za-z]):#/mnt/\L\1#'; }
  WROOT=$(wsl_path "$ROOT")
  if ! MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} true > /dev/null 2>&1; then
    echo "cross-from-linux: no WSL here (wsl --install, or run this on Linux)"; exit 1
  fi

  # 1. The Linux fid
  if [ -n "${LINUX_FID:-}" ]; then
    cp "$LINUX_FID" "$OUT/fid-linux" || exit 1
    rm -rf "$OUT/tree"; mkdir -p "$OUT/tree"; cp -r compiler stdlib "$OUT/tree/"
    ok "1. Linux fid given: $LINUX_FID"
  else
    rm -rf "$OUT/tree"; mkdir -p "$OUT/tree"; cp -r compiler stdlib "$OUT/tree/"
    if [ "${VIA_C:-}" = 1 ]; then BUILD1=(ccompile compiler/main.fd --target x86_64-linux); else BUILD1=(emit compiler/main.fd --target x86_64-linux); fi
    echo "1. ${BUILD1[*]} (in $OUT/tree)"
    t0=$(date +%s)
    ( cd "$OUT/tree" && "$FID" "${BUILD1[@]}" -o "$ROOT/$OUT/fid-linux" ) > "$OUT/build-linux-fid.log" 2>&1
    rc=$?
    t1=$(date +%s)
    if [ $rc != 0 ] || [ ! -s "$OUT/fid-linux" ]; then
      bad "1. the Linux fid did not build (rc $rc): $OUT/build-linux-fid.log"
      tail -n 8 "$OUT/build-linux-fid.log" | sed 's/^/    /'
      echo "cross-from-linux: $fails step(s) failed"; exit "$fails"
    fi
    ok "1. Linux fid built: $(wc -c < "$OUT/fid-linux") bytes in $((t1 - t0)) s"
  fi
  if [ "$(head -c 4 "$OUT/fid-linux" | tail -c 3)" != "ELF" ]; then
    bad "1. $OUT/fid-linux is not an ELF executable"
    echo "cross-from-linux: $fails step(s) failed"; exit "$fails"
  fi
  WFID="$WROOT/$OUT/fid-linux"
  WHOME="$WROOT/$OUT/tree"
  # in_wsl <commands>: run in WSL at the root, with the Linux fid's home
  # (the snapshot's stdlib) and an unlimited stack (the compiler recurses);
  # OS unset: fid takes OS=Windows_NT for a Windows host
  in_wsl() {
    MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} bash -c "cd '$WROOT' || exit 1; ulimit -s unlimited 2> /dev/null; chmod +x '$WFID' 2> /dev/null; unset OS; export FID_HOME='$WHOME'; $1"
  }

  # 2. hello: run by the Linux fid, built for Windows by it, run here
  rm -f "$OUT/hello_win.exe"
  in_wsl "'$WFID' run tests/bootstrap/hello.fd > '$OUT/hello-linux-run.txt' 2>&1; '$WFID' build tests/bootstrap/hello.fd --target windows-x64 -o '$OUT/hello_win.exe' > '$OUT/hello-build.log' 2>&1"
  got_run=$(tr -d '\r' < "$OUT/hello-linux-run.txt" 2>/dev/null)
  if [ "$got_run" = "$REF" ]; then ok "2a. the Linux fid runs hello.fd in WSL"; else bad "2a. the Linux fid's run prints '$got_run'"; fi
  if [ -s "$OUT/hello_win.exe" ] && [ "$(head -c 2 "$OUT/hello_win.exe")" = "MZ" ]; then
    got=$("$OUT/hello_win.exe" < /dev/null 2>&1 | tr -d '\r')
    if [ "$got" = "$REF" ]; then ok "2b. fid build --target windows-x64 in WSL: the PE runs on Windows"
    else bad "2b. the PE built in WSL prints '$got' on Windows, fid run prints '$REF'"; fi
    # The same compiler source on two hosts: the PEs should be the same bytes
    if "$FID" build tests/bootstrap/hello.fd --target windows-x64 -o "$OUT/hello_host.exe" > /dev/null 2>&1; then
      if cmp -s "$OUT/hello_win.exe" "$OUT/hello_host.exe"; then echo "  note  2c. the PE is the same bytes as this host's fid writes"
      else echo "  note  2c. the PE differs from this host's (FID may be built from other sources than the snapshot)"; fi
    fi
  else
    bad "2b. fid build --target windows-x64 in WSL wrote no PE: $OUT/hello-build.log"
    tail -n 8 "$OUT/hello-build.log" 2>/dev/null | sed 's/^/    /'
  fi

  # 3. build-no-toolchain-check.sh in WSL with the Linux fid
  # (logs are written inside WSL: wsl's own console output can garble them)
  if in_wsl "FID='$WFID' bash tests/tooling/build-no-toolchain-check.sh > '$OUT/no-toolchain.log' 2>&1"; then
    ok "3. build-no-toolchain-check.sh in WSL with the Linux fid"
  else
    bad "3. build-no-toolchain-check.sh in WSL: $OUT/no-toolchain.log"
  fi
  sed 's/^/    /' "$OUT/no-toolchain.log"

  # 4. The suite: built in WSL by the Linux fid, run here
  if [ "$SUITE" = 1 ]; then
    for lane in $LANES; do
      LOUT="$OUT/suite-$lane"
      rm -rf "$LOUT-native"
      echo "4. $lane: building the suite in WSL with the Linux fid (BUILD_ONLY=1)"
      in_wsl "NATIVE=1 BUILD_ONLY=1 FID='$WFID' OUT='$LOUT' JOBS='$JOBS' bash scripts/cross-test.sh $lane > '$OUT/suite-$lane-build.log' 2>&1"
      tail -n 3 "$OUT/suite-$lane-build.log" | sed 's/^/    /'
      if [ ! -f "$LOUT-native/run-all.sh" ]; then bad "4. $lane: nothing built ($OUT/suite-$lane-build.log)"; continue; fi
      echo "   running the PEs on Windows"
      bash "$LOUT-native/run-all.sh"
      COMPARE_ONLY=1 NATIVE=1 FID="$FID" OUT="$LOUT" bash scripts/cross-test.sh "$lane" > "$OUT/suite-$lane-compare.log" 2>&1
      summary=$(grep -m1 "^$lane: " "$OUT/suite-$lane-compare.log")
      echo "    built on Linux, run on Windows: $summary"
      if [ "$COMPARE_HOST" = 1 ]; then
        HOUT="$OUT/host-$lane"
        rm -rf "$HOUT-native"
        NATIVE=1 FID="$FID" OUT="$HOUT" JOBS="$JOBS" bash scripts/cross-test.sh "$lane" > "$OUT/host-$lane.log" 2>&1
        echo "    built on Windows:               $(grep -m1 "^$lane: " "$OUT/host-$lane.log")"
        # every test that passes built here must pass built on Linux
        worse=$(awk '$1 == "PASS" { print $2 }' "$HOUT-native/results.txt" | while read -r t; do
                  grep -q "^PASS  $t\$" "$LOUT-native/results.txt" || echo "$t"; done)
        if [ -z "$worse" ]; then ok "4. $lane: every test that passes built on Windows passes built on Linux"
        else bad "4. $lane: pass built on Windows, not built on Linux: $(echo $worse)"; fi
      elif grep -q "^$lane: .* 0 failed, 0 did not build" "$OUT/suite-$lane-compare.log"; then
        ok "4. $lane: the suite built on Linux passes on Windows"
      else
        bad "4. $lane: $summary ($LOUT-native/results.txt)"
      fi
    done
  fi
else
  # A Linux (or macOS) host: FID is this host's fid; PEs run under wine
  # when it is installed
  ulimit -s unlimited 2> /dev/null
  rm -f "$OUT/hello_win.exe"
  if "$FID" build tests/bootstrap/hello.fd --target windows-x64 -o "$OUT/hello_win.exe" > "$OUT/hello-build.log" 2>&1 \
     && [ "$(head -c 2 "$OUT/hello_win.exe")" = "MZ" ]; then
    wine_cmd="${WINE:-}"
    if [ -z "$wine_cmd" ]; then
      if command -v wine > /dev/null 2>&1; then wine_cmd=wine
      elif command -v wine64 > /dev/null 2>&1; then wine_cmd=wine64; fi
    fi
    if [ -n "$wine_cmd" ]; then
      got=$(WINEDEBUG="${WINEDEBUG:--all}" "$wine_cmd" "$OUT/hello_win.exe" < /dev/null 2>/dev/null | tr -d '\r')
      if [ "$got" = "$REF" ]; then ok "2. fid build --target windows-x64: the PE runs under $wine_cmd"
      else bad "2. the PE prints '$got' under $wine_cmd, fid run prints '$REF'"; fi
    else
      ok "2. fid build --target windows-x64 wrote a PE (no wine: copy $OUT/hello_win.exe to Windows)"
    fi
  else
    bad "2. fid build --target windows-x64 failed: $OUT/hello-build.log"
  fi
  if FID="$FID" bash tests/tooling/build-no-toolchain-check.sh > "$OUT/no-toolchain.log" 2>&1; then
    ok "3. build-no-toolchain-check.sh"
  else
    bad "3. build-no-toolchain-check.sh: $OUT/no-toolchain.log"
  fi
  sed 's/^/    /' "$OUT/no-toolchain.log"
  if [ "$SUITE" = 1 ]; then
    for lane in $LANES; do
      NATIVE=1 FID="$FID" OUT="$OUT/suite-$lane" JOBS="$JOBS" bash scripts/cross-test.sh "$lane" > "$OUT/suite-$lane.log" 2>&1
      rc=$?
      summary=$(grep -m1 "^$lane: " "$OUT/suite-$lane.log")
      if [ $rc = 0 ]; then ok "4. $lane: $summary"; else bad "4. $lane: $summary ($OUT/suite-$lane-native/results.txt)"; fi
    done
  fi
fi

if [ "$fails" = 0 ]; then echo "cross-from-linux: PASS"; else echo "cross-from-linux: $fails step(s) failed"; fi
exit "$fails"
