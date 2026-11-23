#!/bin/bash
# BG51 (docs/roadmap/bugs-roadmap.md): a build's stamp (<output>.fides-stamp,
# compiler/inc.fd) records the inputs read besides the sources: the
# `--profile-use` profile (compiler/pgo.fd), ark_modules/.features and
# ark.toml (compiler/compiler.fd cfg_features_file, cc_manifest_edition).
# Before, a build after changing one of them was "up to date" and kept the
# old output.
#   1. ark_modules/.features turns on another feature: `fid emit` builds
#      again and the program takes the other @cfg(feature) function;
#   2. ark.toml changes: not up to date;
#   3. the --profile-use profile changes: not up to date.
#   FID=release/fid.exe bash tests/tooling/stamp-inputs-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
unset FIDES_NO_CACHE
P="$TMP/sp"
mkdir -p "$P/ark_modules"
printf '[package]\nname = "sp"\nversion = "0.1.0"\n' > "$P/ark.toml"
printf 'sp = ["slow"]\n' > "$P/ark_modules/.features"
cat > "$P/main.fd" <<'EOF2'
@cfg(feature = "fast")
fn mode() -> str {
    return "fast";
}

@cfg(feature = "slow")
fn mode() -> str {
    return "slow";
}

fn main() -> i64 {
    println(mode());
    return 0;
}
EOF2
printf 'fides-profile 1\n5 main\n' > "$TMP/a.prof"
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
emit() { "$FID" emit "$P/main.fd" -o "$P/main.exe" "$@" 2>&1; }
emit > /dev/null
got=$("$P/main.exe" | tr -d '\r')
if [ "$got" = "slow" ]; then ok "the first build takes the slow feature"; else bad "the first build: '$got'"; fi
out=$(emit)
if echo "$out" | grep -q "is up to date"; then ok "nothing changed: up to date"; else bad "nothing changed, but built again: $out"; fi
# 1. the features
printf 'sp = ["fast"]\n' > "$P/ark_modules/.features"
out=$(emit)
got=$("$P/main.exe" | tr -d '\r')
if ! echo "$out" | grep -q "is up to date" && [ "$got" = "fast" ]; then ok "1. changed features: built again, fast"; else bad "1. changed features: '$got' / $out"; fi
# 2. ark.toml
printf '[package]\nname = "sp"\nversion = "0.2.0"\n' > "$P/ark.toml"
out=$(emit)
if echo "$out" | grep -q "is up to date"; then bad "2. a changed ark.toml was up to date"; else ok "2. a changed ark.toml builds again"; fi
# 3. the profile
emit --profile-use "$TMP/a.prof" > /dev/null
out=$(emit --profile-use "$TMP/a.prof")
if echo "$out" | grep -q "is up to date"; then ok "3. the same profile: up to date"; else bad "3. the same profile built again: $out"; fi
printf 'fides-profile 1\n9 main\n' > "$TMP/a.prof"
out=$(emit --profile-use "$TMP/a.prof")
if echo "$out" | grep -q "is up to date"; then bad "3. a changed profile was up to date"; else ok "3. a changed profile builds again"; fi
if [ $fail = 0 ]; then echo "stamp-inputs-check: PASS"; else echo "stamp-inputs-check: FAIL"; fi
exit $fail
