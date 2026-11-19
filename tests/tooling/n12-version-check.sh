#!/usr/bin/env bash
# N12 (docs/roadmap/release-roadmap.md): `fid version` and `fid --version`
# name the version from compiler/build_info.fd (fid_version: a release
# build's exact version, else "<fid_version_base>+dev"), never the old
# literal "0.8.0-unstable"; scripts/build-info.sh writes the release's
# version (FIDES_VERSION, a VERSION file or a tag v<version> on HEAD) and
# --reset puts the file back byte for byte.
#   FID=release/fid.exe bash tests/tooling/n12-version-check.sh
# Run from the repository root. Exit 0 when every check passes.
set -u
FID=${FID:-release/fid.exe}
ROOT=$(pwd)
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }

# --- the version the source names ------------------------------------------
BI=compiler/build_info.fd
base=$(awk '/^pub fn fid_version_base\(\)/ { getline; print; exit }' "$BI" | sed -n 's/.*return "\(.*\)";.*/\1/p')
rel=$(awk '/^pub fn fid_build_release\(\)/ { getline; print; exit }' "$BI" | sed -n 's/.*return "\(.*\)";.*/\1/p')
if [ -n "$base" ]; then ok "build_info.fd: base $base"; else bad "build_info.fd has no fid_version_base"; fi
if [ -n "$rel" ]; then want="$rel"; else want="$base+dev"; fi

# --- fid version / --version / -V --------------------------------------------
v1=$("$FID" version 2>&1 | tr -d '\r')
c1=$?
first=$(printf '%s\n' "$v1" | head -1)
if [ $c1 -eq 0 ]; then ok "fid version exits 0"; else bad "fid version exit $c1"; fi
case "$first" in
  "fid $want (self-hosted Fides compiler)") ok "fid version: $first" ;;
  *) bad "fid version first line '$first', wanted 'fid $want (self-hosted Fides compiler)'" ;;
esac
case "$v1" in *0.8.0-unstable*) bad "fid version still prints 0.8.0-unstable" ;; *) ok "no 0.8.0-unstable" ;; esac
case "$v1" in *"build: "*) ok "names the build" ;; *) bad "no build: line" ;; esac
v2=$("$FID" --version 2>&1 | tr -d '\r')
if [ "$v1" = "$v2" ]; then ok "fid --version = fid version"; else bad "fid --version differs: $v2"; fi
v3=$("$FID" -V 2>&1 | tr -d '\r')
if [ "$v1" = "$v3" ]; then ok "fid -V = fid version"; else bad "fid -V differs: $v3"; fi

# --- every place that names the compiler's version ---------------------------
left=$(grep -ln '0\.8\.0-unstable' compiler/*.fd 2>/dev/null)
if [ -z "$left" ]; then ok "no 0.8.0-unstable literal in compiler/"; else bad "0.8.0-unstable still in: $left"; fi

# --- scripts/build-info.sh on a copy ------------------------------------------
D=$(mktemp -d ./_n12_check.XXXXXX)
D=${D#./}
trap 'rm -rf "$D"' EXIT
mkdir -p "$D/compiler"
cp "$BI" "$D/compiler/build_info.fd"
cp compiler/token.fd "$D/compiler/token.fd"
cp "$BI" "$D/orig.fd"
(cd "$D" && FIDES_VERSION=v9.8.7 bash "$ROOT/scripts/build-info.sh" > out.txt 2>&1)
got=$(awk '/^pub fn fid_build_release\(\)/ { getline; print; exit }' "$D/compiler/build_info.fd")
case "$got" in *'return "9.8.7";'*) ok "build-info.sh writes FIDES_VERSION (v stripped)" ;; *) bad "build-info.sh wrote '$got'"; cat "$D/out.txt" ;; esac
if grep -q '^pub fn fid_build_commit()' "$D/compiler/build_info.fd" && awk '/^pub fn fid_build_commit\(\)/ { getline; print; exit }' "$D/compiler/build_info.fd" | grep -q 'return ".\+";'; then ok "build-info.sh writes a commit"; else bad "no commit written"; fi
got=$(awk '/^pub fn fid_build_source\(\)/ { getline; print; exit }' "$D/compiler/build_info.fd")
case "$got" in *'return "src-'[0-9a-f]*'";'*) ok "build-info.sh writes the source hash (D9)" ;; *) bad "source hash line '$got'" ;; esac
(cd "$D" && bash "$ROOT/scripts/build-info.sh" --reset > /dev/null 2>&1)
if cmp -s "$D/orig.fd" "$D/compiler/build_info.fd"; then ok "--reset gives back the same file"; else bad "--reset left a different file"; diff "$D/orig.fd" "$D/compiler/build_info.fd"; fi
# a VERSION file
printf '2.3.4\n' > "$D/VERSION"
(cd "$D" && bash "$ROOT/scripts/build-info.sh" > /dev/null 2>&1)
got=$(awk '/^pub fn fid_build_release\(\)/ { getline; print; exit }' "$D/compiler/build_info.fd")
case "$got" in *'return "2.3.4";'*) ok "build-info.sh reads VERSION" ;; *) bad "VERSION gave '$got'" ;; esac
(cd "$D" && bash "$ROOT/scripts/build-info.sh" --reset > /dev/null 2>&1)
rm -f "$D/VERSION"
# text that is not a version never reaches the string literal
(cd "$D" && FIDES_VERSION='1"; panic("x' bash "$ROOT/scripts/build-info.sh" > /dev/null 2>&1)
got=$(awk '/^pub fn fid_build_release\(\)/ { getline; print; exit }' "$D/compiler/build_info.fd")
case "$got" in *'return "";'*) ok "a bad FIDES_VERSION is ignored" ;; *) bad "a bad FIDES_VERSION reached the file: $got" ;; esac
(cd "$D" && bash "$ROOT/scripts/build-info.sh" --reset > /dev/null 2>&1)
if cmp -s "$D/orig.fd" "$D/compiler/build_info.fd"; then ok "the copy is back as it was"; else bad "the copy differs after --reset"; fi
if [ $fail -eq 0 ]; then echo "N12 version check: all passed"; else echo "N12 version check: FAILED"; fi
exit $fail
