#!/bin/bash
# tier2.sh — the Tier 2 targets (competitive-roadmap PL3): Windows ARM64,
# macOS x64, FreeBSD x64 and Linux RISC-V 64. Tier 2 means the suite builds
# for the target on each push (docs/reference/platforms.md); it runs where
# this host can run it.
#
# Lanes (scripts/suite-all.sh, each test built by scripts/cross-test.sh and
# compared with the interpreter, `fid run`, wherever it can run):
#   c:aarch64-windows   C through `zig cc -target aarch64-windows-gnu`: a
#                       PE32+ ARM64 console executable. Runs on Windows
#                       ARM64 (CI: windows-11-arm, .github/workflows/tier2.yml)
#   c:x86_64-macos      C through `zig cc -target x86_64-macos` (a Mac's own
#                       clang on an Intel Mac). Runs on an Intel Mac, or on
#                       Apple silicon through Rosetta 2 (CI: macos-15-intel)
#   c:x86_64-freebsd    C through `zig cc -target x86_64-freebsd`: dynamic
#                       against FreeBSD's libc (zig ships its stubs). Runs on
#                       FreeBSD (CI: a FreeBSD VM, vmactions/freebsd-vm)
#   c:riscv64-linux     C through `zig cc -target riscv64-linux-musl`, static.
#                       Runs under qemu-riscv64 (in WSL on Windows)
#   WITH_NATIVE=1 adds our own backend's lanes of the same targets:
#   native:riscv64-linux (`fid emit --target riscv64-linux`, qemu) and
#   native:x86_64-macos (`fid emit --target x86_64-macos`).
#
# What passes:
#   every lane   every test builds (no BUILD line); on a lane this host can
#                run, every test that built matches the interpreter (no FAIL)
#   binaries     each binary is an executable of its lane's target
#                (scripts/bin-target-check.py: PE machine ARM64, ELF tagged
#                FreeBSD, Mach-O x86-64, ELF RISC-V); the Mach-O ones are
#                also read by tools/macho_check.fd (built here with
#                `fid ccompile`: load commands, dyld info, code signature)
# A lane this host cannot run is "built only": its results list BUILT per
# test, and $OUT/lanes/<lane>/run-all.sh runs it on the target. To compare
# there: copy the lane's directory to the target machine (same path under a
# checkout), run `bash <dir>/run-all.sh` there, bring the directory back and
#   COMPARE_ONLY=1 OUT=<dir> bash scripts/cross-test.sh <target>
# (tier2.yml does this for Windows ARM64 and FreeBSD).
#
# Usage: scripts/tier2.sh
#   FID=<fid>      default release/fid.exe on Windows, release/fid elsewhere
#   JOBS=<n>       parallel builds (default 4)
#   OUT=<dir>      default build/tier2
#   LANES="..."    only these lanes (default the four C lanes, plus the
#                  native ones with WITH_NATIVE=1)
#   TESTS="a.fd"   only these tests (a smoke run; Tier 2 is the whole suite)
#   NO_BINCHECK=1  skip the binary checks
#   NO_RUN_CHECK=1 do not stop when a riscv64 lane has no qemu-riscv64
#   AGREE=1       also fail when a test passes on one lane and fails on
#                  another (scripts/suite-all.sh)
#   CC             the C compiler for the cross builds (default zig cc, which
#                  the cross targets need)
#
# Output: $OUT/summary.md (also appended to $GITHUB_STEP_SUMMARY in CI).
# Exit status: 0 when every lane builds every test, every lane that ran
# passes and every binary checks; else 1.
set -u
cd "$(dirname "$0")/.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) OSN=windows ;; Darwin) OSN=macos ;; FreeBSD) OSN=freebsd ;; *) OSN=linux ;; esac
if [ -z "${FID:-}" ]; then
  if [ "$OSN" = windows ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
[ -x "$FID" ] || { echo "tier2: no compiler at $FID (set FID)"; exit 1; }
export FID
export JOBS="${JOBS:-4}"
export CC="${CC:-zig cc}"
# NATIVE picks cross-test.sh's backend: suite-all.sh sets it per lane, so a
# NATIVE=1 left in the environment must not turn the C lanes native
unset NATIVE
OUT="${OUT:-build/tier2}"
mkdir -p "$OUT"
DEF_LANES="c:aarch64-windows c:x86_64-macos c:x86_64-freebsd c:riscv64-linux"
if [ "${WITH_NATIVE:-}" = 1 ]; then
  DEF_LANES="$DEF_LANES native:riscv64-linux native:x86_64-macos"
fi
LANES="${LANES:-$DEF_LANES}"
case "$CC" in
  *zig*) command -v zig > /dev/null 2>&1 || { echo "tier2: zig is not on PATH (the cross targets build with zig cc)"; exit 1; } ;;
esac
# A riscv64-linux lane runs under qemu-riscv64 (in WSL on Windows) unless
# this is a RISC-V machine: without it every test would FAIL with exit code
# 127, so say why instead (NO_RUN_CHECK=1 skips this)
case " $LANES " in
  *riscv64-linux*)
    if [ "${NO_RUN_CHECK:-}" != 1 ] && [ "$(uname -m)" != riscv64 ]; then
      if [ "$OSN" = windows ]; then
        command -v wsl > /dev/null 2>&1 || { echo "tier2: the riscv64-linux lanes run in WSL, which is not installed"; exit 1; }
        MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} -e sh -c "command -v qemu-riscv64" > /dev/null 2>&1 \
          || { echo "tier2: the riscv64-linux lanes need qemu-riscv64 in WSL (apt install qemu-user)"; exit 1; }
      elif [ "$OSN" = linux ]; then
        command -v qemu-riscv64 > /dev/null 2>&1 || { echo "tier2: the riscv64-linux lanes need qemu-riscv64 (apt install qemu-user)"; exit 1; }
      fi
    fi ;;
esac
st=0

echo "tier2: lanes $LANES on $OSN-$(uname -m)"
# shellcheck disable=SC2086
TESTS="${TESTS:-}" OUT="$OUT/lanes" bash scripts/suite-all.sh $LANES 2>&1 | tee "$OUT/lanes.txt"
lanes_rc=${PIPESTATUS[0]}
[ "$lanes_rc" = 0 ] || st=1

lane_dir() {  # lane -> the directory scripts/cross-test.sh built it in
  local kind="${1%%:*}" t="${1#*:}" d
  d="$OUT/lanes/$kind-$t"
  [ "$kind" = native ] && d="$d-native"
  echo "$d"
}
lane_bins() {  # lane -> the binaries that built, one per line
  local t="${1#*:}" d ext=bin rc
  d=$(lane_dir "$1")
  case "$t" in wasm*) ext=wasm ;; *-windows*) ext=exe ;; esac
  for rc in "$d"/*.build_rc; do
    [ -f "$rc" ] && [ "$(cat "$rc")" = 0 ] && [ -f "${rc%.build_rc}.$ext" ] && echo "${rc%.build_rc}.$ext"
  done
}
# Which lanes ran here, and which were built only (their results say BUILT)
ran_lines=""
for lane in $LANES; do
  res="$(lane_dir "$lane")/results.txt"
  if [ -f "$res" ] && grep -aq '^BUILT' "$res"; then
    ran_lines="$ran_lines$lane: built only (run $(lane_dir "$lane")/run-all.sh on the target)
"
  elif [ -f "$res" ]; then
    ran_lines="$ran_lines$lane: built and run here
"
  else
    ran_lines="$ran_lines$lane: no results
"
  fi
done

bin_line="skipped (NO_BINCHECK=1)"
macho_line=""
if [ "${NO_BINCHECK:-}" != 1 ]; then
  : > "$OUT/bincheck.txt"
  PY=""
  for p in python3 python; do command -v "$p" > /dev/null 2>&1 && "$p" -c 'import sys; sys.exit(sys.version_info[0] < 3)' 2> /dev/null && { PY=$p; break; }; done
  nb=0; bb=0
  if [ -z "$PY" ]; then
    bin_line="skipped (no python3)"
  else
    for lane in $LANES; do
      t="${lane#*:}"
      bins=()   # (no mapfile: macOS's /bin/bash is 3.2)
      while IFS= read -r bin; do bins+=("$bin"); done < <(lane_bins "$lane")
      [ "${#bins[@]}" -gt 0 ] || continue
      nb=$((nb + ${#bins[@]}))
      # names on stdin: a lane's binaries overflow Windows' command line
      out=$(printf '%s\n' "${bins[@]}" | "$PY" scripts/bin-target-check.py "$t" - --quiet 2>&1 | tr -d '\r')
      echo "$out" >> "$OUT/bincheck.txt"
      bb=$((bb + $(echo "$out" | grep -c '^FAIL')))
    done
    bin_line="$nb built binaries checked, $bb not an executable of their target"
    [ "$bb" = 0 ] || { st=1; grep '^FAIL' "$OUT/bincheck.txt" | head -20 | sed 's/^/  /'; }
  fi
  # The Mach-O lanes: every binary through tools/macho_check.fd
  mlanes=""
  for lane in $LANES; do case "$lane" in *-macos*) mlanes="$mlanes $lane" ;; esac; done
  if [ -n "$mlanes" ]; then
    x=""; [ "$OSN" = windows ] && x=".exe"
    MC="$OUT/macho_check$x"
    # a host build of the tool: the host's C compiler (CC is zig cc here,
    # which builds for the host without -target)
    if timeout 900 "$FID" ccompile tools/macho_check.fd --no-cache -o "$MC" > "$OUT/macho_check.build.log" 2>&1 && [ -x "$MC" ]; then
      : > "$OUT/macho.txt"
      nm=0; bm=0
      for lane in $mlanes; do
        while IFS= read -r bin; do
          nm=$((nm + 1))
          if ! "$MC" "$bin" --quiet >> "$OUT/macho.txt" 2>&1; then
            bm=$((bm + 1)); echo "BAD Mach-O: $bin" >> "$OUT/macho.txt"
          fi
        done < <(lane_bins "$lane")
      done
      macho_line="Mach-O (tools/macho_check.fd): $nm checked, $bm bad"
      if [ "$bm" != 0 ] || [ "$nm" = 0 ]; then st=1; grep '^BAD Mach-O\|^FAIL' "$OUT/macho.txt" | head -20 | sed 's/^/  /'; fi
    else
      macho_line="Mach-O: tools/macho_check.fd did not build (see $OUT/macho_check.build.log)"
      st=1
    fi
  fi
fi
printf '%s' "$ran_lines"
echo "binaries: $bin_line"
[ -n "$macho_line" ] && echo "$macho_line"

{
  echo "## Tier 2"
  echo
  echo '```'
  grep -aE '^(lanes:|c:|native:|suite-all:|  )' "$OUT/lanes.txt" | head -60
  printf '%s' "$ran_lines"
  echo "binaries: $bin_line"
  [ -n "$macho_line" ] && echo "$macho_line"
  echo '```'
  [ "$st" = 0 ] && echo "PASS" || echo "FAIL"
} > "$OUT/summary.md"
[ -n "${GITHUB_STEP_SUMMARY:-}" ] && cat "$OUT/summary.md" >> "$GITHUB_STEP_SUMMARY"
echo "tier2: $([ "$st" = 0 ] && echo PASS || echo FAIL)"
exit "$st"
