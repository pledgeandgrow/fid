#!/bin/bash
# tier1.sh — the whole suite on this host, when it is one of the four tier-1
# platforms (competitive-roadmap PL2): Windows x64, Linux x64, macOS ARM64,
# Linux ARM64. .github/workflows/tier1.yml runs it on each platform for
# every commit.
#
# What "the suite passes" means here, per platform:
#   lanes     every test through the C path and through our own backend,
#             each compared with the interpreter (`fid run`: output, exit
#             code, panic message), by scripts/suite-all.sh with AGREE=1 (a
#             test that passes on one lane must pass on all):
#               windows-x64  c:x86_64-windows native:x86_64-windows-pe
#                            native:x86_64-windows (`emit --nat`)
#               linux-x64    c:x86_64-linux native:x86_64-linux
#               linux-arm64  c:aarch64-linux native:aarch64-linux
#               macos-arm64  c:aarch64-macos native:aarch64-macos
#   rejected  every expected-fail test (tests/bootstrap/*_fail.fd,
#             *_fail2.fd, and files whose first lines say `expect-error`;
#             not test_panic_*_fail.fd, which the lanes compare) is refused:
#             `fid run` exits non-zero
#   binaries  every binary a lane built is an executable of the lane's
#             target (scripts/bin-target-check.py: format, machine, OS);
#             on macos-arm64 also a well-formed, signed Mach-O
#             (tools/macho_check.fd, built here with `fid ccompile`)
#
# Another platform than this host's (PLATFORM=..., how the validation of
# PL2 runs all four from one Windows machine):
#   linux-x64    lanes cross-built here, run in WSL on Windows (natively on
#                a Linux x86-64 host)
#   linux-arm64  lanes cross-built here, run under qemu-aarch64 (in WSL on
#                Windows): scripts/cross-test.sh picks the runner
#   macos-arm64  lanes built here, not run (no Mac): every binary checked
#                by tools/macho_check.fd and bin-target-check.py; the run
#                is CI's macos-15 job (.github/workflows/tier1.yml)
#   windows-x64  only on Windows (the pe_ir lane is the host's `fid emit`)
# The summary says for each lane whether it ran or was built only.
#
# Usage: scripts/tier1.sh
#   FID=<fid>   default release/fid.exe on Windows, release/fid elsewhere
#   JOBS=<n>    parallel builds (default 4)
#   OUT=<dir>   default build/tier1
#   PLATFORM=   force one of the four names above (default: this host)
#   LANES="..." only these lanes; TESTS="a.fd b.fd" only these tests (a
#               smoke run; tier 1 is the whole suite, both unset)
#   NO_REJECT=1 skip the expected-fail check
#   NO_BINCHECK=1 skip the binary checks
#
# Output: $OUT/summary.md (also appended to $GITHUB_STEP_SUMMARY in CI).
# Exit status: 0 when every lane passes, the lanes agree and every
# expected-fail test is refused; 3 when this host is not tier 1; else 1.
set -u
cd "$(dirname "$0")/.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) OSN=windows ;; Darwin) OSN=macos ;; *) OSN=linux ;; esac
case "$(uname -m)" in x86_64|amd64) ARCHN=x64 ;; aarch64|arm64) ARCHN=arm64 ;; *) ARCHN="$(uname -m)" ;; esac
PLATFORM="${PLATFORM:-$OSN-$ARCHN}"
case "$PLATFORM" in
  windows-x64) DEF_LANES="c:x86_64-windows native:x86_64-windows-pe native:x86_64-windows" ;;
  linux-x64)   DEF_LANES="c:x86_64-linux native:x86_64-linux" ;;
  linux-arm64) DEF_LANES="c:aarch64-linux native:aarch64-linux" ;;
  macos-arm64) DEF_LANES="c:aarch64-macos native:aarch64-macos" ;;
  *) echo "tier1: $PLATFORM is not a tier-1 platform (windows-x64, linux-x64, linux-arm64, macos-arm64)"; exit 3 ;;
esac
HOSTP="$OSN-$ARCHN"
if [ "$PLATFORM" = windows-x64 ] && [ "$OSN" != windows ]; then
  echo "tier1: windows-x64 runs on Windows (this host is $HOSTP)"; exit 3
fi
# How the lanes run: on the platform itself, or from another host
case "$PLATFORM" in
  "$HOSTP") MODE="on $PLATFORM itself" ;;
  linux-x64) [ "$OSN" = windows ] && MODE="cross-built on $HOSTP, run in WSL" || MODE="cross-built on $HOSTP, run here" ;;
  linux-arm64) [ "$OSN" = windows ] && MODE="cross-built on $HOSTP, run under qemu-aarch64 in WSL" || MODE="cross-built on $HOSTP, run under qemu-aarch64" ;;
  macos-*) MODE="built on $HOSTP, not run here: binaries checked by tools/macho_check.fd" ;;
esac
if [ -z "${FID:-}" ]; then
  if [ "$OSN" = windows ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
[ -x "$FID" ] || { echo "tier1: no compiler at $FID (set FID)"; exit 1; }
# The runner of a Linux platform validated from another host: WSL on
# Windows, and qemu-aarch64 for linux-arm64 off an ARM64 machine. Without it
# every test would FAIL with exit code 127: say why instead
need_runner=""
case "$PLATFORM-$OSN" in
  linux-*-windows) need_runner=wsl ;;
esac
[ "$PLATFORM" = linux-arm64 ] && [ "$ARCHN" != arm64 ] && need_runner="$need_runner qemu-aarch64"
for r in $need_runner; do
  if [ "$r" = wsl ]; then
    command -v wsl > /dev/null 2>&1 || { echo "tier1: $PLATFORM from Windows runs its lanes in WSL, which is not installed"; exit 1; }
  elif [ "$OSN" = windows ]; then
    MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} -e sh -c "command -v $r" > /dev/null 2>&1 \
      || { echo "tier1: $PLATFORM needs $r in WSL (apt install qemu-user)"; exit 1; }
  else
    command -v "$r" > /dev/null 2>&1 || { echo "tier1: $PLATFORM needs $r (apt install qemu-user)"; exit 1; }
  fi
done
export FID
export JOBS="${JOBS:-4}"
OUT="${OUT:-build/tier1}"
mkdir -p "$OUT"
LANES="${LANES:-$DEF_LANES}"
st=0

is_reject_test() {  # path -> 0 for a test that must be refused
  case "$(basename "$1")" in
    test_panic_*_fail.fd) return 1 ;;
    *_fail.fd|*_fail2.fd) return 0 ;;
  esac
  grep -qxF "$1" "$OUT/expect-error.txt"
}
# files whose first three lines say expect-error (one pass, not a process per file)
awk 'FNR <= 3 && /expect-error/ { print FILENAME; nextfile }' tests/bootstrap/*.fd > "$OUT/expect-error.txt"
LANE_TESTS=""; REJ_TESTS=()
if [ -n "${TESTS:-}" ]; then
  for t in $TESTS; do
    case "$t" in */*) ;; *) t="tests/bootstrap/$t" ;; esac
    if is_reject_test "$t"; then REJ_TESTS+=("$t"); else LANE_TESTS="$LANE_TESTS $t"; fi
  done
else
  for t in tests/bootstrap/*.fd; do is_reject_test "$t" && REJ_TESTS+=("$t"); done
fi

echo "tier1: $PLATFORM ($MODE), lanes: $LANES"
# shellcheck disable=SC2086
if [ -n "${TESTS:-}" ] && [ -z "$LANE_TESTS" ]; then
  echo "lanes: none (TESTS names only expected-fail tests)" | tee "$OUT/lanes.txt"
  lanes_rc=0
else
  TESTS="$LANE_TESTS" OUT="$OUT/lanes" AGREE=1 bash scripts/suite-all.sh $LANES 2>&1 | tee "$OUT/lanes.txt"
  lanes_rc=${PIPESTATUS[0]}
fi
[ "$lanes_rc" = 0 ] || st=1

rej_line="skipped (NO_REJECT=1)"
# expect_missing <test> <log>: the `// expect-error: <text>` texts of the test
# its log does not contain, one per line (white space runs compared as one
# space and the [build]/[prof]/[time] progress lines left out, as
# scripts/diag-tests-check.py does); nothing when the test names no text
expect_missing() {
  awk '
    function ws(s) { gsub(/[ \t\r\n]+/, " ", s); sub(/^ /, "", s); sub(/ $/, "", s); return s }
    FNR == NR {
      if (match($0, /^[ \t]*\/\/[ \t]*expect-error[ \t]*:/)) {
        e = ws(substr($0, RSTART + RLENGTH)); if (length(e) >= 3) want[++ne] = e
      }
      next
    }
    !/^\[(build|prof|time)\]/ { out = out " " $0 }
    END { out = ws(out); for (i = 1; i <= ne; i++) if (index(out, want[i]) == 0) print want[i] }
  ' "$1" "$2"
}
if [ "${NO_REJECT:-}" != 1 ]; then
  : > "$OUT/rejected.txt"
  n=0; acc=0; wrong=0; sane=1
  # A compiler that cannot run anything refuses every test: check that it
  # runs hello first, or "N of N refused" is a false pass (the Nov 27
  # linux-arm64 lane: a fid that built none of the suite refused all 630)
  if ! timeout 120 "$FID" run tests/bootstrap/hello.fd < /dev/null > "$OUT/reject-hello-sanity.log" 2>&1; then
    rej_line="not checked: $FID cannot run tests/bootstrap/hello.fd (see reject-hello-sanity.log)"
    st=1; sane=0
    REJ_TESTS=()
  fi
  for f in "${REJ_TESTS[@]+"${REJ_TESTS[@]}"}"; do
    b=$(basename "$f")
    n=$((n + 1))
    log="$OUT/reject-${b%.fd}.log"
    if timeout 120 "$FID" run "$f" < /dev/null > "$log" 2>&1; then
      echo "ACCEPTED  $b" >> "$OUT/rejected.txt"; acc=$((acc + 1))
    else
      miss=$(expect_missing "$f" "$log")
      if [ -n "$miss" ]; then
        # refused, but not for the reason the test names
        echo "WRONG     $b (no: $(printf '%s' "$miss" | head -1))" >> "$OUT/rejected.txt"; wrong=$((wrong + 1))
      else
        echo "REJECTED  $b" >> "$OUT/rejected.txt"
      fi
    fi
  done
  if [ "$sane" = 1 ]; then
    rej_line="$((n - acc - wrong)) of $n refused with the error they expect"
    [ "$wrong" != 0 ] && rej_line="$rej_line, $wrong refused with another error"
    [ "$acc" != 0 ] && rej_line="$rej_line, $acc accepted"
  fi
  if [ "$acc" != 0 ] || [ "$wrong" != 0 ]; then
    st=1
    grep '^ACCEPTED\|^WRONG' "$OUT/rejected.txt" | sed 's/^/  /'
  fi
fi
echo "expected-fail tests: $rej_line"

# The binaries every lane built: an executable of the lane's target, and on
# macOS a Mach-O dyld would load (tools/macho_check.fd, the Fides port of
# scripts/macho-check.py)
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
      t="${lane#*:}"; t="${t%-pe}"
      bins=()   # (no mapfile: macOS's /bin/bash is 3.2)
      while IFS= read -r bin; do bins+=("$bin"); done < <(lane_bins "$lane")
      [ "${#bins[@]}" -gt 0 ] || continue
      nb=$((nb + ${#bins[@]}))
      # names on stdin: a lane's binaries overflow Windows' command line
      out=$(printf '%s\n' "${bins[@]}" | "$PY" scripts/bin-target-check.py "$t" - --quiet 2>&1 | tr -d '\r')
      echo "$out" >> "$OUT/bincheck.txt"
      bb=$((bb + $(echo "$out" | grep -c '^FAIL')))
    done
    bin_line="$nb built binaries checked, $bb not an executable of their target (or importing a runtime name)"
    [ "$bb" = 0 ] || { st=1; grep '^FAIL' "$OUT/bincheck.txt" | head -20 | sed 's/^/  /'; }
  fi
  case "$PLATFORM" in
    macos-*)
      x=""; [ "$OSN" = windows ] && x=".exe"
      MC="$OUT/macho_check$x"
      # the host's C compiler: CC, else zig cc (as the lanes), else fid's pick
      if [ -z "${CC:-}" ] && command -v zig > /dev/null 2>&1; then export CC="zig cc"; fi
      if timeout 900 "$FID" ccompile tools/macho_check.fd --no-cache -o "$MC" > "$OUT/macho_check.build.log" 2>&1 && [ -x "$MC" ]; then
        : > "$OUT/macho.txt"
        nm=0; bm=0
        for lane in $LANES; do
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
      fi ;;
  esac
fi
echo "binaries: $bin_line"
[ -n "$macho_line" ] && echo "$macho_line"

{
  echo "## Tier 1: $PLATFORM"
  echo
  echo "Lanes $MODE."
  echo
  echo '```'
  grep -aE '^(lanes:|c:|native:|suite-all:|  )' "$OUT/lanes.txt" | head -60
  echo "expected-fail tests: $rej_line"
  echo "binaries: $bin_line"
  [ -n "$macho_line" ] && echo "$macho_line"
  echo '```'
  [ "$st" = 0 ] && echo "PASS" || echo "FAIL"
} > "$OUT/summary.md"
[ -n "${GITHUB_STEP_SUMMARY:-}" ] && cat "$OUT/summary.md" >> "$GITHUB_STEP_SUMMARY"
echo "tier1: $PLATFORM $([ "$st" = 0 ] && echo PASS || echo FAIL)"
exit "$st"
