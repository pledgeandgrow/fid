#!/bin/bash
# crater.sh — competitive-roadmap ST6: before a release, build every known
# Fides project with the new compiler, as Rust's crater does, and fail on
# any file that built with the last release and does not build now.
#
# The projects are listed in scripts/crater/projects.txt (its header gives
# the format). Each is copied (local:) or cloned (git:) into
# build/crater/src/<name>; a project with an ark.toml gets `fid ark install`;
# then each file is built with the project's command (check, run or
# ccompile). Results go to build/crater/results.txt, one
# `<name>|<file>|pass|fail` line each, and are compared with
# scripts/crater/baseline.txt, the results of the last release:
#   pass -> fail   a regression: the run fails
#   fail -> pass   fixed (reported)
#   not in the baseline: new (reported; it must pass to be added)
#
# Usage:
#   scripts/crater.sh [--save] [fid]
#   FID=<fid>       the compiler (default: release/fid.exe, release/fid,
#                   release/fid-dev)
#   CC=<cc>         the C compiler for ccompile
#   ONLY=<name>     one project
#   TIMEOUT=<s>     per file (default 300)
#   --save          record this run as the baseline (do it with the
#                   release's compiler, after the release)
#   FID_HOME=<dir>  where the stdlib comes from (<dir>/stdlib; the copies
#                   build outside the repository, so without it the stdlib
#                   beside the compiler). An old compiler with today's
#                   stdlib fails on builtins it does not know: record the
#                   baseline with the release's own stdlib, e.g.
#                   git archive <release commit> stdlib | tar -x -C <dir>
#   CRATER_STDLIB_NOTE  what the baseline's header says of the stdlib
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT="$(pwd)"
SAVE=0
if [ "${1:-}" = "--save" ]; then SAVE=1; shift; fi
FID="${1:-${FID:-}}"
if [ -z "$FID" ]; then
  for f in release/fid.exe release/fid release/fid-dev release/fid-dev.exe; do
    if [ -e "$f" ]; then FID="$f"; break; fi
  done
fi
if [ -z "$FID" ]; then echo "crater: no compiler (give its path)"; exit 2; fi
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
LIST=scripts/crater/projects.txt
BASE=scripts/crater/baseline.txt
OUT=build/crater
TIMEOUT="${TIMEOUT:-300}"
EXE=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;; esac
mkdir -p "$OUT/src" "$OUT/logs"
: > "$OUT/results.txt"

with_timeout() {
  if command -v timeout > /dev/null 2>&1; then timeout "$TIMEOUT" "$@"; else "$@"; fi
}

n_pass=0; n_fail=0
while read -r name source command files; do
  case "$name" in ''|'#'*) continue ;; esac
  if [ -n "${ONLY:-}" ] && [ "$name" != "$ONLY" ]; then continue; fi
  dir="$OUT/src/$name"
  rm -rf "$dir"
  case "$source" in
    local:*)
      mkdir -p "$dir"
      cp -R "${source#local:}/." "$dir/"
      ;;
    git:*)
      spec="${source#git:}"
      url="${spec%%#*}"
      ref=""
      [ "$spec" != "$url" ] && ref="${spec#*#}"
      if ! git clone --quiet "$url" "$dir" > "$OUT/logs/$name.clone.log" 2>&1; then
        echo "  FAIL  $name: cannot clone $url"
        echo "$name|(clone)|fail" >> "$OUT/results.txt"
        n_fail=$((n_fail + 1))
        continue
      fi
      if [ -n "$ref" ] && ! git -C "$dir" checkout --quiet "$ref" >> "$OUT/logs/$name.clone.log" 2>&1; then
        echo "  FAIL  $name: no $ref in $url"
        echo "$name|(clone)|fail" >> "$OUT/results.txt"
        n_fail=$((n_fail + 1))
        continue
      fi
      ;;
    *)
      echo "  FAIL  $name: unknown source '$source'"
      continue
      ;;
  esac
  if [ -f "$dir/ark.toml" ]; then
    (cd "$dir" && with_timeout "$FID" ark install) > "$OUT/logs/$name.ark.log" 2>&1 < /dev/null \
      || echo "  note  $name: ark install failed (see $OUT/logs/$name.ark.log)"
  fi
  for pat in $files; do
    # the files of the pattern, relative to the project
    for f in $(cd "$dir" && ls -d $pat 2> /dev/null); do
      log="$OUT/logs/$name.$(echo "$f" | tr '/' '_').log"
      case "$command" in
        check)    (cd "$dir" && with_timeout "$FID" check "$f") > "$log" 2>&1 < /dev/null ;;
        run)      (cd "$dir" && with_timeout "$FID" run "$f") > "$log" 2>&1 < /dev/null ;;
        ccompile) (cd "$dir" && with_timeout "$FID" ccompile "$f" -o "crater_out$EXE") > "$log" 2>&1 < /dev/null ;;
        *)        echo "unknown command $command" > "$log"; false ;;
      esac
      if [ $? -eq 0 ]; then
        echo "$name|$f|pass" >> "$OUT/results.txt"; n_pass=$((n_pass + 1))
      else
        echo "$name|$f|fail" >> "$OUT/results.txt"; n_fail=$((n_fail + 1))
      fi
    done
  done
done < <(tr -d '\r' < "$LIST")
echo "crater: $n_pass built, $n_fail failed ($OUT/results.txt, logs in $OUT/logs)"

if [ "$SAVE" = 1 ]; then
  # the header says what made it: a compiler is only as good as the
  # stdlib it was run with (FID_HOME, else the one beside it)
  {
    echo "# crater baseline (scripts/crater.sh --save), $(date -u '+%Y-%m-%d %H:%M UTC')"
    echo "# compiler: ${FID#"$ROOT"/} ($("$FID" version 2> /dev/null | head -1 | tr -d '\r'))"
    echo "# stdlib: ${CRATER_STDLIB_NOTE:-${FID_HOME:-the one beside the compiler}}"
    cat "$OUT/results.txt"
  } > "$BASE"
  echo "crater: baseline saved to $BASE"
  exit 0
fi
if [ ! -f "$BASE" ]; then
  echo "crater: no baseline ($BASE): every file must build"
  [ "$n_fail" = 0 ] && { echo "crater: PASS"; exit 0; }
  grep '|fail$' "$OUT/results.txt" | sed 's/^/  FAIL  /'
  echo "crater: FAIL"; exit 1
fi
bad=0
while IFS='|' read -r name f state; do
  was=$(grep -F "$name|$f|" "$BASE" | head -1 | cut -d'|' -f3)
  if [ -z "$was" ]; then
    echo "  new   $name $f: $state"
  elif [ "$was" = pass ] && [ "$state" = fail ]; then
    echo "  FAIL  $name $f: built with the last release, fails now (log: $OUT/logs/)"; bad=1
  elif [ "$was" = fail ] && [ "$state" = pass ]; then
    echo "  fixed $name $f"
  fi
done < "$OUT/results.txt"
# a file the baseline had that is gone (the project moved or was renamed)
while IFS='|' read -r name f state; do
  case "$name" in ''|'#'*) continue ;; esac
  if [ -n "${ONLY:-}" ] && [ "$name" != "$ONLY" ]; then continue; fi
  grep -qF "$name|$f|" "$OUT/results.txt" || echo "  gone  $name $f (was $state)"
done < <(tr -d '\r' < "$BASE")
if [ "$bad" = 0 ]; then echo "crater: PASS"; else echo "crater: FAIL"; fi
exit "$bad"
