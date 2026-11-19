#!/usr/bin/env bash
# build-info.sh — the commit and date `fid version` names (release N12).
#
#   scripts/build-info.sh           write the current commit and today's date
#                                   (UTC) into compiler/build_info.fd
#   scripts/build-info.sh --reset   put the empty values back (a development
#                                   build names itself by its executable hash)
#
# RELEASE=1 scripts/bootstrap.sh runs the first before the stages and the
# second when it exits, so the tree is left as it was. The commit gets a
# "+changes" suffix when the tree has uncommitted changes. The version
# (fid_build_release) comes from FIDES_VERSION, a VERSION file or a tag
# v<version> on HEAD; without one a build calls itself "<base>+dev".
set -u
F="compiler/build_info.fd"
if [ ! -f "$F" ]; then
  echo "build-info: $F not found (run from the repository root)"; exit 1
fi
commit=""
date=""
version=""
if [ "${1:-}" != "--reset" ]; then
  commit="$(git rev-parse --short=12 HEAD 2>/dev/null || echo unknown)"
  if ! git diff --quiet 2>/dev/null; then
    commit="$commit+changes"
  fi
  date="$(date -u +%F)"
  # N12: the release's version: FIDES_VERSION (release.yml, the installer),
  # else a VERSION file, else a tag v<version> on HEAD; none of them leaves
  # "" (fid then names itself "<fid_version_base>+dev")
  version="${FIDES_VERSION:-}"
  if [ -z "$version" ] && [ -f VERSION ]; then
    version="$(head -1 VERSION | tr -d ' \r\n')"
  fi
  if [ -z "$version" ]; then
    version="$(git describe --tags --exact-match HEAD 2>/dev/null || true)"
  fi
  version="${version#v}"
  # only a version-like text goes into a string literal
  case "$version" in
    *[!0-9A-Za-z.+-]*) echo "build-info: ignoring version '$version'"; version="" ;;
  esac
  # D9: the hash of the source this build compiles (every .fd file under
  # compiler/ and stdlib/ but build_info.fd, in byte order of their names,
  # line endings ignored). fid keys its caches on it with the executable's
  # size (inc.fd inc_compiler_hash) instead of reading the executable.
  hasher="sha256sum"
  if ! command -v sha256sum > /dev/null 2>&1; then
    if command -v shasum > /dev/null 2>&1; then hasher="shasum -a 256"; else hasher="cksum"; fi
  fi
  # (one cat and one tr for all files: a process per file takes minutes in
  # Git Bash)
  src_list="$(find compiler stdlib -name '*.fd' -type f 2>/dev/null | LC_ALL=C sort | grep -v '^compiler/build_info\.fd$')"
  source_hash=""
  if [ -n "$src_list" ]; then
    source_hash="$( { printf '%s\n' "$src_list"; printf '%s\n' "$src_list" | tr '\n' '\0' | xargs -0 cat; } | tr -d '\r' | $hasher | tr -cd '0-9a-f' | cut -c1-16)"
  fi
  if [ -n "$source_hash" ]; then
    source_hash="src-$source_hash"
  fi
fi
tmp="$F.tmp.$$"
awk -v c="$commit" -v d="$date" -v v="$version" -v s="${source_hash:-}" '
  /^pub fn fid_build_commit\(\)/ { print; getline; print "    return \"" c "\";"; next }
  /^pub fn fid_build_date\(\)/ { print; getline; print "    return \"" d "\";"; next }
  /^pub fn fid_build_release\(\)/ { print; getline; print "    return \"" v "\";"; next }
  /^pub fn fid_build_source\(\)/ { print; getline; print "    return \"" s "\";"; next }
  { print }
' "$F" > "$tmp" && mv "$tmp" "$F"
if [ -n "$commit" ]; then
  echo "build-info: version ${version:-(development)}, commit $commit, built $date, source ${source_hash:-?}"
else
  echo "build-info: reset"
fi
