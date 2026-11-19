#!/bin/bash
# rl13-repro-check.sh — scripts/verify/repro.sh and repro-hosts.sh (RL13)
# give the right verdicts, checked with stand-in compilers instead of fid:
# each stand-in is a shell script that answers `dump-c`, `emit` and
# `ccompile` with output made from the source, plus whatever its mode puts
# in. Runs in about a minute and builds nothing.
#
#   clean    the source only                       -> reproducible
#   home     dump-c names the stdlib path FID_HOME  -> caught by HOSTSIM=1 only
#   user     the images name the user and the zone  -> caught by HOSTSIM=1 only
#   par      the images name FID_NAT_PAR            -> caught by PAR=4 only
#   clock    every output has the time in it        -> caught always
#   located  a DIFFERENT line is followed by bin-diff-locate.py's location
#   --compare of equal and of different manifests  -> exit 0 / exit 1
#   repro-hosts.sh (when wsl.exe is there): a clean stand-in on Windows and
#            in WSL agrees (exit 0); one that writes `uname -s` into its
#            dump-c output does not (exit 1, "hosts differ")
#
# Usage: bash tests/tooling/rl13-repro-check.sh     (OUT=<dir>: a temp dir)
# Exit status: the number of wrong verdicts.
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
OUT="${OUT:-$(mktemp -d 2>/dev/null || echo build/rl13-repro-check)}"
mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"
bad=0

fake() {  # fake <mode> <path>
  {
    echo '#!/bin/bash'
    echo "# a stand-in compiler for rl13-repro-check.sh (mode $1)"
    echo "MODE=$1"
    cat <<'EOF'
cmd="$1"; shift
src=""; out=""; tgt="pe"; prev=""
for a in "$@"; do
  [ "$prev" = -o ] && out="$a"
  [ "$prev" = --target ] && tgt="$a"
  [ "$a" = --nat ] && tgt="nat"
  case "$a" in *.fd) src="$a" ;; esac
  prev="$a"
done
[ -f "$src" ] || { echo "no source $src" >&2; exit 1; }
body() {
  echo "/* $cmd $tgt */"
  cat "$src"
  case "$MODE" in
    home) [ "$cmd" = dump-c ] && echo "/* stdlib: $FID_HOME */" ;;
    user) [ "$cmd" = emit ] && echo "built by ${USERNAME:-${USER:-}} in ${TZ:-}" ;;
    par) [ "$cmd" = emit ] && echo "workers ${FID_NAT_PAR:-0}" ;;
    clock) date +%s%N ;;
    host) [ "$cmd" = dump-c ] && echo "/* host $(uname -s) */" ;;
  esac
}
case "$cmd" in
  dump-c) body ;;
  emit|ccompile) [ -n "$out" ] || exit 1; body > "$out" ;;
  *) exit 1 ;;
esac
exit 0
EOF
  } > "$2"
  chmod +x "$2"
}

PROG=tests/bootstrap/hello.fd
# run <name> <want-exit> <grep-or-""> <env...>: repro.sh with stand-in <mode>
run() {
  local name="$1" mode="$2" want="$3" pat="$4"; shift 4
  fake "$mode" "$OUT/fid-$mode"
  env NAT_TARGETS=x86_64-linux "$@" FID="$OUT/fid-$mode" WORK="$OUT/w-$name" \
      bash scripts/verify/repro.sh "$PROG" > "$OUT/$name.log" 2>&1
  local got=$?
  if [ "$got" != "$want" ]; then
    echo "WRONG $name: exit $got, want $want ($OUT/$name.log)"
    tail -n 6 "$OUT/$name.log" | sed 's/^/      /'
    bad=$((bad + 1))
  elif [ -n "$pat" ] && ! grep -q -- "$pat" "$OUT/$name.log"; then
    echo "WRONG $name: exit $got but no \"$pat\" ($OUT/$name.log)"
    bad=$((bad + 1))
  else
    echo "ok    $name: exit $got${pat:+, \"$pat\"}"
  fi
}

run clean        clean 0 "outputs reproducible"
run home         home  1 "DIFFERENT hello/out.c:"         HOSTSIM=1 PAR=
run home-nosim   home  0 ""                                 HOSTSIM=0 PAR=
run user         user  1 "DIFFERENT hello/emit-x86_64-linux.bin:" HOSTSIM=1 PAR=
run user-nosim   user  0 ""                                 HOSTSIM=0 PAR=
run par          par   1 "DIFFERENT hello/emit-x86_64-linux.bin with 4 workers" HOSTSIM=0 PAR=4
run par-off      par   0 ""                                 HOSTSIM=0 PAR=
run clock        clock 1 "DIFFERENT"                        HOSTSIM=0 PAR=
# the location follows a difference (text: the first differing lines)
if command -v python3 > /dev/null || command -v python > /dev/null; then
  if grep -q "first differing lines" "$OUT/home.log"; then echo "ok    located: home.log names the lines"
  else echo "WRONG located: no location in $OUT/home.log"; bad=$((bad + 1)); fi
fi
# a clean run puts its host-independent outputs in the manifest
env NAT_TARGETS=x86_64-linux PAR= FID="$OUT/fid-clean" WORK="$OUT/w-m1" MANIFEST="$OUT/m1.txt" \
    bash scripts/verify/repro.sh "$PROG" > "$OUT/m1.log" 2>&1
if [ "$(wc -l < "$OUT/m1.txt")" = 2 ] && grep -q " hello/out.c$" "$OUT/m1.txt" \
   && grep -q " hello/emit-x86_64-linux.bin$" "$OUT/m1.txt"; then
  echo "ok    manifest: out.c and the Linux image"
else
  echo "WRONG manifest: $OUT/m1.txt"; bad=$((bad + 1))
fi

# --compare
cp "$OUT/m1.txt" "$OUT/m2.txt"
bash scripts/verify/repro.sh --compare "$OUT/m1.txt" "$OUT/m2.txt" > "$OUT/cmp-same.log" 2>&1
st=$?
if [ $st = 0 ] && grep -q "hosts agree on all 2 outputs" "$OUT/cmp-same.log"; then echo "ok    compare equal: exit 0"
else echo "WRONG compare equal: exit $st ($OUT/cmp-same.log)"; bad=$((bad + 1)); fi
# the same names, the first hash changed in its first digit
awk 'NR == 1 { c = substr($0, 1, 1); $0 = (c == "0" ? "1" : "0") substr($0, 2) } { print }' \
    "$OUT/m1.txt" > "$OUT/m3.txt"
bash scripts/verify/repro.sh --compare "$OUT/m1.txt" "$OUT/m3.txt" > "$OUT/cmp-diff.log" 2>&1
st=$?
if [ $st = 1 ] && grep -q "hosts differ on 1 of 2 outputs" "$OUT/cmp-diff.log"; then echo "ok    compare different: exit 1"
else echo "WRONG compare different: exit $st ($OUT/cmp-diff.log)"; bad=$((bad + 1)); fi

# repro-hosts.sh, on Windows with WSL
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) win=1 ;; *) win=0 ;; esac
if [ $win = 1 ] && command -v wsl.exe > /dev/null && MSYS_NO_PATHCONV=1 wsl.exe -e true 2> /dev/null; then
  hosts() {  # hosts <mode> <want> <pattern>
    fake "$2" "$OUT/fid-$2"
    NAT_TARGETS=x86_64-linux PAR= FID="$OUT/fid-$2" FID_LINUX="$OUT/fid-$2" OUT="$OUT/h-$1" \
      bash scripts/verify/repro-hosts.sh "$PROG" > "$OUT/hosts-$1.log" 2>&1
    local got=$?
    if [ "$got" = "$3" ] && grep -q -- "$4" "$OUT/hosts-$1.log"; then echo "ok    repro-hosts $1: exit $got, \"$4\""
    else echo "WRONG repro-hosts $1: exit $got, want $3 and \"$4\" ($OUT/hosts-$1.log)"
      tail -n 8 "$OUT/hosts-$1.log" | sed 's/^/      /'; bad=$((bad + 1)); fi
  }
  hosts clean clean 0 "hosts agree on all 2 outputs"
  hosts host  host  1 "hosts differ on 1 of 2 outputs"
else
  echo "skip  repro-hosts: not Windows with WSL"
fi

echo "rl13-repro-check: $bad wrong"
exit "$bad"
