#!/bin/bash
# switch-check.sh — the pass switches the compiler reads, found in its source
# (own-backend roadmap RL19 and RL3).
#
#   scripts/switch-check.sh                 check that docs/verification/kill-switches.md
#                                           names every switch (exit 1 if not)
#   scripts/switch-check.sh --list <group>  print one group, space separated:
#       iropt   FID_IROPT_OFF names of the default-on mid-end passes
#               (iro_pass_on in compiler/iropt.fd, minus the opt-in ones)
#       ime     FID_IROPT_ON names of the opt-in mid-end passes
#               (ime_on in compiler/iropt_me.fd / iropt.fd)
#       nat     FID_NAT_OFF names of the backend passes and rule groups
#               (ng_on in nat_ir.fd, ru_on in rules.fd, eg_on in egraph.fd)
#       rules   FID_RULES_ON names (ru_opt_in in rules.fd)
#
# Reads the source only (no compiler is run), so it is cheap enough for every
# push (.github/workflows/ci.yml, job `gates`). scripts/pass-matrix.sh takes
# its lists from here, so a new switch is tried by the matrix as soon as the
# compiler reads it.
set -u
cd "$(dirname "$0")/.." || exit 1
export LC_ALL=C   # sort and comm must agree on the order
DOC=docs/verification/kill-switches.md

names() {  # grep pattern of the reader function -> sorted unique names
  grep -oh "$1(\"[a-z0-9_]*\")" compiler/*.fd 2>/dev/null \
    | sed -E 's/.*\("([a-z0-9_]*)"\)/\1/' | sort -u
}

group() {
  case "$1" in
    iropt)
      # default-on: read by iro_pass_on and not gated by ime_on as well
      comm -23 <(names iro_pass_on) <(names ime_on) ;;
    ime) names ime_on ;;
    nat) { names ng_on; names ru_on; names eg_on; } | sort -u ;;
    rules) names ru_opt_in ;;
    *) echo "unknown group: $1 (iropt, ime, nat, rules)" >&2; return 2 ;;
  esac
}

if [ "${1:-}" = "--list" ]; then
  group "${2:-}" | tr '\n' ' ' | sed 's/ $//'
  echo
  exit 0
fi

# Check: each name appears in the doc's table of its variable, written as
# `VAR=name` (one table row per switch).
bad=0
check() {  # var group
  local n
  for n in $(group "$2"); do
    if ! grep -qF "\`$1=$n\`" "$DOC"; then
      echo "switch-check: $1=$n is read by the compiler but not documented in $DOC"
      bad=$((bad + 1))
    fi
  done
}
[ -f "$DOC" ] || { echo "switch-check: $DOC missing"; exit 1; }
check FID_IROPT_OFF iropt
check FID_IROPT_ON ime
check FID_NAT_OFF nat
check FID_RULES_ON rules
# FID_NAT_ON is read with its own test in nat_ir.fd (ng_panic_if)
if grep -q '",crashfree,"' compiler/nat_ir.fd && ! grep -qF '`FID_NAT_ON=crashfree`' "$DOC"; then
  echo "switch-check: FID_NAT_ON=crashfree is not documented in $DOC"; bad=$((bad + 1))
fi
# The other way: a documented switch the compiler no longer reads
for row in $(grep -oE '`FID_(IROPT_OFF|IROPT_ON|NAT_OFF|RULES_ON)=[a-z0-9_]+`' "$DOC" | tr -d '`' | sort -u); do
  var="${row%%=*}"; n="${row#*=}"
  [ "$row" = FID_IROPT_ON=all ] && continue   # every opt-in pass, not a name
  case "$var" in
    FID_IROPT_OFF) g="iropt ime" ;;   # FID_IROPT_OFF also turns an opt-in pass off
    FID_IROPT_ON) g=ime ;;
    FID_NAT_OFF) g=nat ;;
    FID_RULES_ON) g=rules ;;
  esac
  found=0
  for gg in $g; do group "$gg" | grep -qx "$n" && found=1; done
  if [ $found = 0 ]; then
    echo "switch-check: $DOC documents $row, which the compiler does not read"
    bad=$((bad + 1))
  fi
done
if [ $bad = 0 ]; then
  echo "switch-check: ok ($(group iropt | wc -l) iropt, $(group ime | wc -l) opt-in, $(group nat | wc -l) backend, $(group rules | wc -l) rules switches)"
fi
[ $bad = 0 ]
