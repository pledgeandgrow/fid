#!/bin/bash
# fmt-check.sh — release-roadmap N4: `fid fmt` in CI.
#   0. fixtures: tests/fmt/<name>_in.fd formats to tests/fmt/<name>.fd,
#      which passes --check, and both print the same (tests/fmt/README.md);
#   1. formatted: `fid fmt --check` passes on every file of compiler/ and
#      stdlib/ (RUNTIMES=1: also stdlib/pe_runtime/ and stdlib/linux_runtime/);
#   2. stable: a formatted file formats to itself (a formatted copy of a
#      test program passes --check);
#   3. comments kept: the formatted copy has every comment of the original,
#      in the same order and with the same words (scripts/fmt-comments.py);
#   4. behaviour kept: the formatted copy prints what the original prints
#      under `fid run`, with the same exit status.
# Steps 2-4 run on copies of test programs (tests/bootstrap/test_*.fd, not
# the expected-fail ones) that formatting changes: an unchanged file proves
# nothing. A program whose two runs of the original differ (time, random,
# addresses) or that fails on its own is skipped. `fid fmt` itself refuses to
# write a file whose tokens (comments included) would change
# (compiler/fmt.fd fmt_same_tokens); this checks the same from outside.
#
# Usage:
#   scripts/fmt-check.sh [fid]   (default: release/fid-dev, else release/fid.exe,
#                                 else release/fid)
#   LIMIT=<n>    test programs for steps 2-4 (default 30; 0: none)
#   SKIP_TREE=1  skip step 1
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${1:-${FID:-}}"
if [ -z "$FID" ]; then
  for f in release/fid-dev release/fid-dev.exe release/fid.exe release/fid; do
    if [ -x "$f" ]; then FID="$f"; break; fi
  done
fi
if [ -z "$FID" ]; then echo "fmt-check: no compiler (give its path)"; exit 1; fi
LIMIT="${LIMIT:-30}"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
WORK=build/fmt-check
rm -rf "$WORK"
mkdir -p "$WORK"
fail=0

# run_fd <file>: fid run, no input, at most two minutes, stdout only
run_fd() {
  if command -v timeout > /dev/null 2>&1; then
    timeout 120 "$FID" run "$1" < /dev/null 2> /dev/null
  else
    "$FID" run "$1" < /dev/null 2> /dev/null
  fi
}

echo "== 0. fixtures (tests/fmt): <name>_in.fd formats to <name>.fd, which is stable"
for f in tests/fmt/*_in.fd; do
  [ -f "$f" ] || continue
  name=$(basename "$f" _in.fd)
  want="tests/fmt/$name.fd"
  cp "$f" "$WORK/fx_$name.fd"
  if ! "$FID" fmt "$WORK/fx_$name.fd" > "$WORK/fx_$name.log" 2>&1; then
    echo "  FAIL  $f: fid fmt failed: $(tr -d '\r' < "$WORK/fx_$name.log" | head -1)"
    fail=1
    continue
  fi
  if [ "$(tr -d '\r' < "$WORK/fx_$name.fd")" != "$(tr -d '\r' < "$want")" ]; then
    echo "  FAIL  $f: formats differently from $want:"
    diff <(tr -d '\r' < "$want") <(tr -d '\r' < "$WORK/fx_$name.fd") | head -10 | sed 's/^/        /'
    fail=1
  else
    echo "  ok    $f formats to $want"
  fi
  # stable: the expected file is already formatted (LF copy: git may check
  # it out with CRLF)
  tr -d '\r' < "$want" > "$WORK/fx_${name}_want.fd"
  if "$FID" fmt "$WORK/fx_${name}_want.fd" --check > /dev/null 2>&1; then
    echo "  ok    $want passes fmt --check"
  else
    echo "  FAIL  $want: fmt --check would reformat it (formatting is not stable)"
    fail=1
  fi
  if [ "$(run_fd "$f")" = "$(run_fd "$want")" ]; then
    echo "  ok    $name: both print the same"
  else
    echo "  FAIL  $name: $f and $want print different things"
    fail=1
  fi
done

if [ -z "${SKIP_TREE:-}" ]; then
  echo "== 1. fid fmt --check on the compiler and the stdlib"
  files="compiler/*.fd stdlib/*.fd"
  [ -n "${RUNTIMES:-}" ] && files="$files stdlib/pe_runtime/*.fd stdlib/linux_runtime/*.fd"
  n=0
  bad=0
  for f in $files; do
    n=$((n + 1))
    if ! "$FID" fmt "$f" --check > "$WORK/check.log" 2>&1; then
      echo "  FAIL  $f: $(tr -d '\r' < "$WORK/check.log" | head -1)"
      bad=$((bad + 1))
    fi
  done
  if [ "$bad" = 0 ]; then echo "  ok    $n files formatted"; else echo "  FAIL  $bad of $n files"; fail=1; fi
fi

if [ "$LIMIT" -gt 0 ]; then
  echo "== 2-4. formatted copies of test programs: stable, comments and behaviour kept"
  changed=0
  checked=0
  skipped=0
  for f in tests/bootstrap/test_*.fd; do
    [ "$changed" -ge "$LIMIT" ] && break
    case "$f" in *_fail*.fd) continue ;; esac
    head -1 "$f" | grep -q "expect-error" && continue
    name=$(basename "$f" .fd)
    # the copy sits beside nothing: a program importing a sibling file is
    # recognised by its run failing, and skipped
    cp "$f" "$WORK/$name.fd"
    if ! "$FID" fmt "$WORK/$name.fd" > "$WORK/$name.fmt.log" 2>&1; then
      echo "  FAIL  $f: fid fmt failed: $(tr -d '\r' < "$WORK/$name.fmt.log" | head -1)"
      fail=1
      continue
    fi
    if cmp -s "$f" "$WORK/$name.fd"; then continue; fi
    changed=$((changed + 1))
    # 2. stable
    if ! "$FID" fmt "$WORK/$name.fd" --check > /dev/null 2>&1; then
      echo "  FAIL  $f: formatting the formatted copy changes it again"
      fail=1
    fi
    # 3. comments
    if ! "$PY" scripts/fmt-comments.py "$f" "$WORK/$name.fd"; then
      fail=1
    fi
    # 4. behaviour: the original twice (to see it is deterministic), the copy once
    run_fd "$f" > "$WORK/$name.a"; sa=$?
    run_fd "$f" > "$WORK/$name.b"; sb=$?
    if [ "$sa" != 0 ] || [ "$sa" != "$sb" ] || ! cmp -s "$WORK/$name.a" "$WORK/$name.b"; then
      skipped=$((skipped + 1))
      continue
    fi
    run_fd "$WORK/$name.fd" > "$WORK/$name.c"; sc=$?
    checked=$((checked + 1))
    if [ "$sc" != "$sa" ] || ! cmp -s "$WORK/$name.a" "$WORK/$name.c"; then
      echo "  FAIL  $f: the formatted copy behaves differently (exit $sc, was $sa; outputs in $WORK/$name.a, .c)"
      fail=1
    fi
  done
  echo "  ok    $changed changed by formatting, $checked run and compared, $skipped skipped (nondeterministic or failing alone)"
fi

if [ "$fail" = 0 ]; then echo "fmt-check: PASS"; else echo "fmt-check: FAIL"; fi
exit "$fail"
