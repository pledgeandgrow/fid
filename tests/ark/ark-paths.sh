#!/bin/bash
# ark-paths.sh -- known bug 2g.63: path pacts on every host, Windows first.
#   1. a directory pact given with a trailing separator (`../lefty/`, and
#      `..\lefty\` in ark.toml) installs as ark_modules/lefty/;
#   2. a directory pact whose path has a space (`../my lib`) installs (the
#      unquoted xcopy/cp command failed on it);
#   3. a single-file pact installs as ark_modules/<name>.fd, also when it is
#      named without its extension (`--path ../righty` for ../righty.fd);
#   4. ark.toml keeps `/` separators (`path = "..\lefty\"` ended in an
#      escaped quote), and a TOML-escaped `"..\\righty.fd"` reads as one `\`;
#   5. a program `use`s all three pacts and runs (needs FID: fid run);
#   6. `ark lock`: with nothing installed it names each pact, says how many
#      are missing and fails; after install it writes 3 pacts and passes;
#      with no pacts it says so.
#
# Usage: tests/ark/ark-paths.sh [command that runs ark...]
#   default: release/fid.exe ark (else release/fid-dev.exe ark, ...)
#   FID=<fid> also runs the program (step 5); a fid outside the repository
#   needs FID_HOME=<repo> for its stdlib.
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
if [ $# -gt 0 ]; then
  ARK=("$@")
else
  ARK=()
  # prefer the newest working compiler: release/fid.exe can be weeks old
  # and predate the very features these scripts check (2g.63's lesson)
  for f in release/fid-dev.exe release/fid.exe release/fid-dev release/fid; do
    if [ -x "$f" ]; then ARK=("$ROOT/$f" ark); break; fi
  done
  if [ ${#ARK[@]} -eq 0 ]; then echo "ark-paths: no compiler (give the command)"; exit 1; fi
fi
case "${ARK[0]}" in
  /*|[A-Za-z]:*) ;;
  */*) ARK[0]="$ROOT/${ARK[0]}" ;;
esac
FID="${FID:-}"
if [ -n "$FID" ]; then
  case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
fi

W="$(mktemp -d 2>/dev/null || echo "$ROOT/build/ark-paths.$$")"
mkdir -p "$W"
fail=0
ok()  { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }

mkdir -p "$W/lefty" "$W/my lib" "$W/app" "$W/app2" "$W/empty"
printf 'pub fn lefty_one() -> i64 { return 1; }\n' > "$W/lefty/mod.fd"
printf 'pub fn spaced_four() -> i64 { return 4; }\n' > "$W/my lib/mod.fd"
printf 'pub fn righty_two() -> i64 { return 2; }\n' > "$W/righty.fd"

cd "$W/app" || exit 1
"${ARK[@]}" init app > /dev/null 2>&1 || bad "ark init"

echo "== lock with no pacts says so"
out=$("${ARK[@]}" lock 2>&1); st=$?
if [ $st -eq 0 ] && echo "$out" | grep -q "has no pacts"; then ok "empty lock: exit 0, 'has no pacts'"; else bad "empty lock: exit $st: $out"; fi

echo "== add: trailing separator, a space, a file without .fd"
"${ARK[@]}" add lefty --path '..\lefty\' > /dev/null 2>&1 || bad "add lefty"
"${ARK[@]}" add spaced --path "../my lib/" > /dev/null 2>&1 || bad "add spaced"
"${ARK[@]}" add righty --path ../righty > /dev/null 2>&1 || bad "add righty"
if grep -q 'lefty = { path = "../lefty/" }' ark.toml; then ok "ark.toml: lefty stored with / separators"; else bad "ark.toml lefty line: $(grep lefty ark.toml)"; fi
if grep -q '\\' ark.toml; then bad "ark.toml still holds a backslash: $(grep '\\' ark.toml)"; else ok "ark.toml has no backslash"; fi

echo "== lock before install names each pact and fails"
out=$("${ARK[@]}" lock 2>&1); st=$?
if [ $st -ne 0 ] && echo "$out" | grep -q "'lefty' is not installed" && echo "$out" | grep -q "3 pact(s) of ark.toml are not installed"; then
  ok "lock before install: exit $st, names the 3 missing pacts"
else
  bad "lock before install: exit $st: $out"
fi

echo "== install"
out=$("${ARK[@]}" install 2>&1); st=$?
if [ $st -eq 0 ]; then ok "install: exit 0"; else bad "install: exit $st: $out"; fi
[ -f ark_modules/lefty/mod.fd ] && ok "ark_modules/lefty/mod.fd" || bad "no ark_modules/lefty/mod.fd"
[ -f "ark_modules/spaced/mod.fd" ] && ok "ark_modules/spaced/mod.fd (path with a space)" || bad "no ark_modules/spaced/mod.fd"
[ -f ark_modules/righty.fd ] && ok "ark_modules/righty.fd (single file, named without .fd)" || bad "no ark_modules/righty.fd"
[ -e ark_modules/righty ] && bad "ark_modules/righty without .fd exists" || ok "no ark_modules/righty without .fd"

echo "== lock after install"
out=$("${ARK[@]}" lock 2>&1); st=$?
if [ $st -eq 0 ] && echo "$out" | grep -q "with 3 pact(s)"; then ok "lock: 3 pacts, exit 0"; else bad "lock after install: exit $st: $out"; fi
n=$(grep -c '^\[\[pact\]\]' ark.lock)
[ "$n" = "3" ] && ok "ark.lock holds 3 [[pact]]" || bad "ark.lock holds $n [[pact]]"

if [ -n "$FID" ]; then
  echo "== use the pacts"
  printf 'use core;\nuse lefty;\nuse righty;\nuse spaced;\nfn main() { println(int_to_str(lefty_one() + righty_two() + spaced_four())); }\n' > main.fd
  got=$("$FID" run main.fd 2> /dev/null | tr -d '\r' | tail -1)
  [ "$got" = "7" ] && ok "fid run: use lefty/righty/spaced prints 7" || bad "fid run printed '$got' (want 7)"
fi

echo "== a hand-written ark.toml: a TOML-escaped path and an old unescaped one"
cd "$W/app2" || exit 1
cat > ark.toml <<'EOF'
[package]
name = "app2"
version = "0.1.0"
edition = "2026"

[dependencies]
lefty = { path = "..\lefty\" }
righty = { path = "..\\righty.fd" }
empty = { path = "../empty" }
EOF
out=$("${ARK[@]}" install 2>&1); st=$?
if [ $st -eq 0 ] && [ -f ark_modules/lefty/mod.fd ] && [ -f ark_modules/righty.fd ]; then
  ok "install from hand-written ark.toml"
else
  bad "install from hand-written ark.toml: exit $st: $out"
fi
if echo "$out" | grep -q "holds no .fd file"; then ok "an empty directory pact is warned about"; else bad "no warning for the empty directory pact: $out"; fi

cd "$ROOT" || exit 1
rm -rf "$W"
if [ $fail -eq 0 ]; then echo "ark-paths: all passed"; else echo "ark-paths: FAILED"; fi
exit $fail
