#!/bin/bash
# ark-safety.sh -- what a manifest can make `ark install` do, and what
# `ark remove` leaves (docs/roadmap/bugs-roadmap.md):
#   BG285  a git branch, tag, commit or URL in an ark.toml is not shell
#          syntax: `branch = "x & echo INJECTED> inj.txt & rem"` (cmd.exe) or
#          `"x; echo INJECTED > inj.txt"` (sh) is refused, nothing runs
#   BG411  a pact name that is not a plain name (`../../pwned`, from a
#          dependency's own ark.toml) is refused before anything is written;
#          `ark add ../x` too
#   BG482  `ark remove <name>` removes ark_modules/<name>.fd (and its
#          ark.lock entry): `use <name>;` no longer resolves
#   and a plain git dependency (a local repository, a branch with / and .)
#   still installs.
# Everything is local (directories and a git repository under a temporary
# directory).
#
# Usage: tests/ark/ark-safety.sh [command that runs ark...]
#   default: release/fid-dev.exe ark (else release/fid.exe ark, ...)
#   e.g.     tests/ark/ark-safety.sh release/fid.exe run tools/ark.fd
#            (the tree's ark.fd under the interpreter)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
if [ $# -gt 0 ]; then
  ARK=("$@")
else
  ARK=()
  for f in release/fid-dev.exe release/fid.exe release/fid-dev release/fid; do
    if [ -x "$f" ]; then ARK=("$ROOT/$f" ark); break; fi
  done
  if [ ${#ARK[@]} -eq 0 ]; then echo "ark-safety: no compiler (give the command)"; exit 1; fi
fi
case "${ARK[0]}" in
  /*|[A-Za-z]:*) ;;
  */*) ARK[0]="$ROOT/${ARK[0]}" ;;
esac
# `fid run tools/ark.fd`: the path of the program made absolute too
i=1
while [ $i -lt ${#ARK[@]} ]; do
  case "${ARK[$i]}" in
    tools/*.fd) ARK[$i]="$ROOT/${ARK[$i]}" ;;
  esac
  i=$((i + 1))
done
export FID_HOME="${FID_HOME:-$(cygpath -m "$ROOT" 2>/dev/null || echo "$ROOT")}"
WORK="$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/ark-safety.$$")"
mkdir -p "$WORK"
export ARK_HOME="$(cygpath -m "$WORK/home" 2>/dev/null || echo "$WORK/home")"
export ARK_CACHE="$(cygpath -m "$WORK/cache" 2>/dev/null || echo "$WORK/cache")"
mkdir -p "$WORK/home"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
ark() { "${ARK[@]}" "$@" < /dev/null 2>&1 | tr -d '\r' | grep -v '^\[\(build\|prof\|time\)\]'; }
new_app() {  # new_app <dir>: an `ark init` project there
  mkdir -p "$1" && cd "$1" && ark init app > /dev/null
}

# --- BG285: a ref or URL is not shell syntax ---------------------------------
new_app "$WORK/inj1"
cat >> ark.toml <<'EOF'

[dependencies]
evil = { git = "https://invalid.example/none.git", branch = "x & echo INJECTED> inj.txt & rem" }
EOF
out=$(ark install)
if [ ! -e inj.txt ] && printf '%s' "$out" | grep -q "not a branch or tag name"; then
  ok "BG285 a branch holding & is refused, nothing runs (cmd.exe form)"
else
  bad "BG285 branch with &: inj.txt $( [ -e inj.txt ] && echo made || echo absent ), [$(printf '%s' "$out" | head -3 | tr '\n' '|')]"
fi
new_app "$WORK/inj2"
cat >> ark.toml <<'EOF'

[dependencies]
evil = { git = "https://invalid.example/none.git", branch = "x; echo INJECTED > inj.txt" }
EOF
out=$(ark install)
if [ ! -e inj.txt ]; then
  ok "BG285 a branch holding ; runs nothing (sh form)"
else
  bad "BG285 branch with ;: inj.txt made"
fi
new_app "$WORK/inj3"
cat >> ark.toml <<'EOF'

[dependencies]
evil = { git = "https://x.example/a.git & echo INJECTED> inj.txt & rem $(touch inj2.txt)" }
EOF
out=$(ark install)
if [ ! -e inj.txt ] && [ ! -e inj2.txt ] && printf '%s' "$out" | grep -q "not a git URL"; then
  ok "BG285 a URL holding shell syntax is refused"
else
  bad "BG285 URL: [$(printf '%s' "$out" | head -3 | tr '\n' '|')]"
fi
new_app "$WORK/inj4"
cat >> ark.toml <<'EOF'

[dependencies]
evil = { git = "--upload-pack=touch inj.txt" }
EOF
out=$(ark install)
if [ ! -e inj.txt ]; then
  ok "BG285 a URL that is a git option is refused"
else
  bad "BG285 --upload-pack URL: inj.txt made"
fi

# --- a plain git dependency still installs -----------------------------------
mkdir -p "$WORK/libg"
( cd "$WORK/libg" && git init -q . && printf '[package]\nname = "libg"\nversion = "0.1.0"\n' > ark.toml \
  && printf 'pub fn lg() -> i64 {\n    return 7;\n}\n' > libg.fd && git add . \
  && git -c user.email=t@example.invalid -c user.name=t commit -qm init && git branch rel/1.0 ) > /dev/null 2>&1
new_app "$WORK/okgit"
ark add libg --git "$(cygpath -m "$WORK/libg" 2>/dev/null || echo "$WORK/libg")" --branch rel/1.0 > /dev/null
out=$(ark install)
if printf '%s' "$out" | grep -q "Installing libg... OK (git" && [ -e ark_modules/libg ]; then
  ok "a git pact at branch rel/1.0 installs"
else
  bad "git pact: [$(printf '%s' "$out" | head -4 | tr '\n' '|')]"
fi

# --- BG411: plain pact names ---------------------------------------------------
mkdir -p "$WORK/n/evil" "$WORK/n/app"
printf '[package]\nname = "evil"\nversion = "0.1.0"\n\n[dependencies]\n../../pwned = { path = "payload.fd" }\n' > "$WORK/n/evil/ark.toml"
printf 'pub fn pw() -> i64 {\n    return 1;\n}\n' > "$WORK/n/evil/payload.fd"
printf 'pub fn ev() -> i64 {\n    return 2;\n}\n' > "$WORK/n/evil/evil.fd"
cd "$WORK/n/app" && ark init app > /dev/null
ark add evil --path ../evil > /dev/null
out=$(ark install)
if [ ! -e "$WORK/pwned.fd" ] && [ ! -e "$WORK/n/pwned.fd" ] && [ ! -e "$WORK/pwned" ] && printf '%s' "$out" | grep -q "invalid pact name '../../pwned'"; then
  ok "BG411 a dependency's pact named ../../pwned is refused, nothing written"
else
  bad "BG411 ../../pwned: [$(printf '%s' "$out" | head -3 | tr '\n' '|')], $(ls "$WORK" "$WORK/n" | tr '\n' ' ')"
fi
out=$("${ARK[@]}" add ../x --path ../evil/payload.fd < /dev/null 2>&1); rc=$?
if [ "$rc" != 0 ] && ! grep -q '\.\./x' ark.toml; then
  ok "BG411 ark add ../x is refused"
else
  bad "BG411 ark add ../x: [$out]"
fi

# --- BG482: remove takes the installed copy away -----------------------------
mkdir -p "$WORK/r/libx"
printf 'pub fn lx() -> i64 {\n    return 8;\n}\n' > "$WORK/r/libx/libx.fd"
new_app "$WORK/r/app"
ark add libx --path ../libx/libx.fd > /dev/null
ark install > /dev/null
had=no; [ -e ark_modules/libx.fd ] && had=yes
ark remove libx > /dev/null
ark install > /dev/null
if [ "$had" = yes ] && [ ! -e ark_modules/libx.fd ] && ! grep -q 'name = "libx"' ark.lock 2>/dev/null; then
  ok "BG482 ark remove deletes ark_modules/libx.fd and its ark.lock entry"
else
  bad "BG482 installed: $had, after remove: $(ls ark_modules 2>/dev/null | tr '\n' ' ')"
fi

rm -rf "$WORK"
if [ "$fails" -gt 0 ]; then
  echo "ark-safety: $fails failed"
  exit 1
fi
echo "ark-safety: all passed"
