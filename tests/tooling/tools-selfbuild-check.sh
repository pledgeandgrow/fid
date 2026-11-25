#!/bin/bash
# tools-selfbuild-check.sh -- AD1 (competitive-roadmap), release N8: the
# companion tools written in Fides (tools/{ark,scribe,veil}.fd over
# compiler/{ark,scribe,veil}.fd) built with `fid ccompile` -- the way
# installer/build-installer.sh builds what it ships -- and their output
# compared with the Rust tools (target/release/{ark,scribe,veil}) on the
# same inputs.
#
# Checked for each tool built by fid:
#   - it builds through C and runs (--version / version);
#   - the command line of the Rust tool: --help and -h exit 0, no argument
#     prints the help and exits 1, a missing input exits 1 with an error on
#     stderr, a second input is refused, -o/-f short forms;
#   - scribe: Markdown of stdlib/math.fd and stdlib/semver.fd, written into
#     a directory that does not exist yet: the same sections (## / ###
#     headings, in order) as the Rust scribe's;
#   - veil: coverage of tests/bootstrap/hello.fd and struct.fd: the
#     program's own output identical to the Rust veil's, the same
#     "Functions:" line and the same per-function names, coverage.txt and
#     (--format html) coverage.html written.
#   - ark: tests/ark/ark-selfbuild.sh (every tests/ark check against it).
# Without target/release/<tool>(.exe) the comparison part is skipped (said
# so), the rest still runs.
#
# Usage: tests/tooling/tools-selfbuild-check.sh [fid]
#   default fid: release/fid.exe (else release/fid-dev.exe, release/fid, ...)
#   CC: the C compiler ccompile uses (default "zig cc" when zig is on PATH)
#   TOOLS: which to check (default "scribe veil ark")
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
FID="${1:-}"
if [ -z "$FID" ]; then
  for f in release/fid.exe release/fid-dev.exe release/fid release/fid-dev; do
    if [ -x "$f" ]; then FID="$ROOT/$f"; break; fi
  done
fi
if [ -z "$FID" ]; then echo "tools-selfbuild-check: no fid (give its path)"; exit 1; fi
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
if [ -z "${CC:-}" ] && command -v zig > /dev/null 2>&1; then export CC="zig cc"; fi
TOOLS="${TOOLS:-scribe veil ark}"

WORK="$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/tools-selfbuild.$$")"
mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT
EXT=""
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) EXT=".exe" ;; esac

fail=0
ok()   { echo "  ok    $*"; }
bad()  { echo "  FAIL  $*"; fail=1; }
note() { echo "  --    $*"; }

echo "== tools-selfbuild-check ($FID)"

build() {
  local tool="$1"
  if ! "$FID" ccompile "tools/$tool.fd" -o "$WORK/$tool$EXT" > "$WORK/$tool-build.log" 2>&1 \
      || [ ! -f "$WORK/$tool$EXT" ]; then
    bad "fid ccompile tools/$tool.fd"
    grep -v '^\[' "$WORK/$tool-build.log" | tail -20 | sed 's/^/        /'
    return 1
  fi
  ok "fid ccompile tools/$tool.fd -> $tool$EXT ($(wc -c < "$WORK/$tool$EXT") bytes)"
  return 0
}

rust_tool() {
  local t="target/release/$1$EXT"
  if [ -x "$t" ]; then echo "$ROOT/$t"; fi
}

# The command line both tools share with their Rust versions.
check_cli() {
  local tool="$1" exe="$2"
  local out code
  out=$("$exe" --version 2>&1); code=$?
  case "$out" in "$tool 0.1.0"*) [ $code -eq 0 ] && ok "$tool --version: $out" || bad "$tool --version exit $code" ;;
    *) bad "$tool --version: '$out'" ;; esac
  for h in --help -h; do
    out=$("$exe" $h 2>&1); code=$?
    if [ $code -eq 0 ] && printf '%s' "$out" | grep -q "USAGE:"; then ok "$tool $h (exit 0, usage)";
    else bad "$tool $h: exit $code, '$(printf '%s' "$out" | head -1)'"; fi
  done
  out=$("$exe" 2>&1); code=$?
  if [ $code -eq 1 ] && printf '%s' "$out" | grep -q "USAGE:"; then ok "$tool with no argument (help, exit 1)";
  else bad "$tool with no argument: exit $code"; fi
  "$exe" "$WORK/no-such-file.fd" > "$WORK/$tool-missing.out" 2> "$WORK/$tool-missing.err"; code=$?
  if [ $code -eq 1 ] && grep -q "error: Cannot" "$WORK/$tool-missing.err"; then ok "$tool on a missing input: exit 1, error on stderr";
  else bad "$tool on a missing input: exit $code, stderr '$(head -1 "$WORK/$tool-missing.err")'"; fi
  "$exe" tests/bootstrap/hello.fd tests/bootstrap/hello.fd -o "$WORK/two" > /dev/null 2> "$WORK/$tool-two.err"; code=$?
  if [ $code -eq 1 ] && grep -q "Unknown argument" "$WORK/$tool-two.err"; then ok "$tool refuses a second input";
  else bad "$tool with two inputs: exit $code"; fi
}

# ── scribe ────────────────────────────────────────────────────────────────
if [[ " $TOOLS " == *" scribe "* ]] && build scribe; then
  S="$WORK/scribe$EXT"
  check_cli scribe "$S"
  RS=$(rust_tool scribe)
  for src in stdlib/math.fd stdlib/semver.fd; do
    name=$(basename "$src" .fd)
    out="$WORK/sc-fd-$name/deep/dir"
    if ! "$S" "$src" -f markdown -o "$out" > "$WORK/sc-$name.out" 2>&1; then
      bad "scribe $src -f markdown: exit $? ($(head -1 "$WORK/sc-$name.out"))"; continue
    fi
    md=$(ls "$out"/*.md 2>/dev/null | head -1)
    if [ -z "$md" ]; then bad "scribe $src: no .md written in a new directory"; continue; fi
    ok "scribe $src -f markdown -o <new dir>: $(basename "$md")"
    if [ -z "$RS" ]; then note "no target/release/scribe$EXT: Rust comparison skipped"; continue; fi
    rout="$WORK/sc-rs-$name"
    if ! "$RS" "$src" --format markdown --output "$rout" > /dev/null 2>&1; then
      note "Rust scribe failed on $src: comparison skipped"; continue
    fi
    rmd=$(ls "$rout"/*.md 2>/dev/null | head -1)
    grep -E '^#{2,3} ' "$md"  | tr -d '\r' > "$WORK/sc-$name.fd.h"
    grep -E '^#{2,3} ' "$rmd" | tr -d '\r' > "$WORK/sc-$name.rs.h"
    # The frozen Rust scribe still has BG104 (src/scribe.rs parse_impl
    # counts a line's braces before testing the depth, so a method whose
    # body spans lines is never documented); the Fides scribe has it fixed.
    # Extra "### name" headings are accepted when nothing of the Rust
    # scribe's is missing and each one is a method of an impl block in the
    # source (an indented "fn name(").
    extra_ok=0
    if ! cmp -s "$WORK/sc-$name.fd.h" "$WORK/sc-$name.rs.h"; then
      diff "$WORK/sc-$name.fd.h" "$WORK/sc-$name.rs.h" | grep -E '^[<>] ' > "$WORK/sc-$name.diff"
      if ! grep -q '^> ' "$WORK/sc-$name.diff" && ! grep -qv '^< ### ' "$WORK/sc-$name.diff"; then
        extra_ok=1
        for m in $(sed 's/^< ### //' "$WORK/sc-$name.diff" | sort -u); do
          if ! tr -d '\r' < "$src" | grep -qE "^[[:space:]]+(pub )?fn $m[(<]"; then extra_ok=0; fi
        done
      fi
    fi
    if cmp -s "$WORK/sc-$name.fd.h" "$WORK/sc-$name.rs.h"; then
      ok "scribe $src: same $(wc -l < "$WORK/sc-$name.fd.h") sections as the Rust scribe"
    elif [ $extra_ok -eq 1 ]; then
      ok "scribe $src: the Rust scribe's $(wc -l < "$WORK/sc-$name.rs.h") sections, plus $(wc -l < "$WORK/sc-$name.diff") impl methods it misses (BG104, fixed in the Fides scribe only)"
    else
      bad "scribe $src: sections differ from the Rust scribe (< fid, > Rust)"
      diff "$WORK/sc-$name.fd.h" "$WORK/sc-$name.rs.h" | head -20 | sed 's/^/        /'
    fi
  done
fi

# ── veil ──────────────────────────────────────────────────────────────────
if [[ " $TOOLS " == *" veil "* ]] && build veil; then
  V="$WORK/veil$EXT"
  check_cli veil "$V"
  RV=$(rust_tool veil)
  # A program that does not compile: no report, the errors on stderr, exit 1
  printf 'fn main() -> i64 {\n    val x: i64 = "no";\n    return 0;\n}\n' > "$WORK/bad.fd"
  "$V" "$WORK/bad.fd" -o "$WORK/vl-bad" > "$WORK/vl-bad.out" 2> "$WORK/vl-bad.err"; code=$?
  if [ $code -eq 1 ] && grep -q "^error: .*cannot assign str" "$WORK/vl-bad.err" && [ ! -f "$WORK/vl-bad/coverage.txt" ]; then
    ok "veil on a program that does not compile: exit 1, the error on stderr, no report"
  else
    bad "veil on a program that does not compile: exit $code, stderr '$(grep -v '^\[' "$WORK/vl-bad.err" | head -1)'"
  fi
  for src in tests/bootstrap/hello.fd tests/bootstrap/struct.fd; do
    name=$(basename "$src" .fd)
    out="$WORK/vl-fd-$name"
    if ! "$V" "$src" -o "$out" > "$WORK/vl-$name.fd.out" 2> "$WORK/vl-$name.fd.err"; then
      bad "veil $src: exit $? ($(head -1 "$WORK/vl-$name.fd.err"))"; continue
    fi
    if [ ! -f "$out/coverage.txt" ]; then bad "veil $src: no coverage.txt"; continue; fi
    ok "veil $src -o <dir>: coverage.txt"
    if "$V" --format html "$src" --output "$out-html" > /dev/null 2>&1 && [ -f "$out-html/coverage.html" ]; then
      ok "veil --format html $src: coverage.html"
    else
      bad "veil --format html $src: no coverage.html"
    fi
    if [ -z "$RV" ]; then note "no target/release/veil$EXT: Rust comparison skipped"; continue; fi
    if ! "$RV" "$src" --output "$WORK/vl-rs-$name" > "$WORK/vl-$name.rs.out" 2> /dev/null; then
      note "Rust veil failed on $src: comparison skipped"; continue
    fi
    # The program's own output: everything before the report's title,
    # which follows it on the same line when the output ends without a
    # newline (both veils print what the interpreter collected as it is)
    prog() { tr -d '\r' < "$1" | sed -n '/veil .* Coverage Report$/{s/veil .* Coverage Report$//;p;q;};p' | sed '/^$/d'; }
    prog "$WORK/vl-$name.fd.out" > "$WORK/vl-$name.fd.prog"
    prog "$WORK/vl-$name.rs.out" > "$WORK/vl-$name.rs.prog"
    if cmp -s "$WORK/vl-$name.fd.prog" "$WORK/vl-$name.rs.prog"; then ok "veil $src: the program's output is the Rust veil's";
    else bad "veil $src: the program's output differs from the Rust veil's"; diff "$WORK/vl-$name.fd.prog" "$WORK/vl-$name.rs.prog" | head -10 | sed 's/^/        /'; fi
    ff=$(tr -d '\r' < "$out/coverage.txt" | grep '^Functions:')
    rf=$(tr -d '\r' < "$WORK/vl-rs-$name/coverage.txt" | grep '^Functions:')
    if [ "$ff" = "$rf" ]; then ok "veil $src: $ff (as Rust)"; else bad "veil $src: '$ff' vs Rust '$rf'"; fi
    # Per-function rows: names (first column), sorted
    rows() { tr -d '\r' < "$1" | sed -n '/^Per-Function Coverage/,/^$/p' | sed '1,4d' | awk 'NF >= 4 {print $1}' | sort; }
    rows "$out/coverage.txt" > "$WORK/vl-$name.fd.fn"
    rows "$WORK/vl-rs-$name/coverage.txt" > "$WORK/vl-$name.rs.fn"
    if cmp -s "$WORK/vl-$name.fd.fn" "$WORK/vl-$name.rs.fn"; then ok "veil $src: the same $(wc -l < "$WORK/vl-$name.fd.fn") functions listed";
    else bad "veil $src: per-function rows differ (< fid, > Rust)"; diff "$WORK/vl-$name.fd.fn" "$WORK/vl-$name.rs.fn" | head -10 | sed 's/^/        /'; fi
    fl=$(tr -d '\r' < "$out/coverage.txt" | grep '^Lines:')
    rl=$(tr -d '\r' < "$WORK/vl-rs-$name/coverage.txt" | grep '^Lines:')
    if [ "$fl" = "$rl" ]; then ok "veil $src: $fl (as Rust)"; else note "veil $src: '$fl' vs Rust '$rl' (line attribution of the two lowerings; not a failure)"; fi
  done
fi

# ── ark ───────────────────────────────────────────────────────────────────
if [[ " $TOOLS " == *" ark "* ]]; then
  if bash tests/ark/ark-selfbuild.sh "$FID" > "$WORK/ark.log" 2>&1; then
    ok "ark: tests/ark/ark-selfbuild.sh PASS"
  else
    bad "ark: tests/ark/ark-selfbuild.sh"
    grep -F 'FAIL' "$WORK/ark.log" | head -10 | sed 's/^/        /'
  fi
  RA=$(rust_tool ark)
  if [ -n "$RA" ] && "$FID" ccompile tools/ark.fd -o "$WORK/ark$EXT" > /dev/null 2>&1; then
    fa=$("$WORK/ark$EXT" version 2>&1 | head -1 | tr -d '\r')
    ra=$("$RA" version 2>&1 | head -1 | tr -d '\r')
    note "ark version: fid '$fa', Rust '$ra'"
  fi
fi

if [ $fail -ne 0 ]; then echo "tools-selfbuild-check: FAILED"; exit 1; fi
echo "tools-selfbuild-check: PASS"
