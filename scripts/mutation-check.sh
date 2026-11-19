#!/bin/bash
# mutation-check.sh — mutation testing of the backend (own-backend roadmap
# RL11): each deliberately broken rule or pass of the machine layer must
# make the suite fail. No compiler is rebuilt: every mutant is a run-time
# switch of the compiler under test, FID_NAT_MUTATE=<mutant> (never set in
# a real build; it is part of nat_ir's cache key):
#   rule<N>   compiler/rules.fd: rule N (the order `fid test-rules` lists)
#             gives a wrong result where it applies (`x + 0 = 0`)
#   rulec<N>  the same rule matching the wrong constant (`x + 1 = x`); only
#             for the `x op c` / `c op x` rules
#   eg<fam>   compiler/egraph.fd: an e-graph rule family broken (egcomm,
#             egflip, egfold, egident, egzero, egdouble, egsame)
#   <name>    compiler/nat_ir.fd ng_mutant("<name>"): a selection rule, an
#             encoding or a machine pass broken (TG2: isel, iselimm,
#             iselcc, shi, addi, tbz, brgap, opw, andn, memop, ldop, par,
#             arghint, ...) — read from the source, so a new one is
#             covered at once
# (scripts/verify/variants.py `mutate` is the other half: mutants that are
# source edits, each a compiler build.)
#
# Usage: scripts/mutation-check.sh [test.fd ...]
#   FID=<fid>        the compiler (release/fid.exe, else release/fid)
#   TARGET=<t>       nat (default on Windows: `emit --nat`), x86_64-linux
#                    (default elsewhere), aarch64-linux, riscv64-linux;
#                    a Linux image runs natively, in WSL, or under
#                    qemu-aarch64 / qemu-riscv64
#   MUTANTS="a b"    which mutants (default: every one the tree has)
#   TESTS=quick      the killer tests only (test_mutation_kill,
#                    test_egraph_rule_families, test_machine_rules_random,
#                    ...); default every test of tests/bootstrap/ that is
#                    not expected to fail and not in scripts/long-tests.txt
#   SAMPLE=N         N tests of those (fixed seed), after the killers
#   JOBS=4  TIMEOUT=30 (seconds per run)  OUT=build/mutation-check
#   VERIFY=1         build with FID_VERIFY=1 (the IR verifier and the
#                    allocation checker): a mutant they reject is killed
#                    "by a checker"
#   STRICT=1         an inert mutant (no image changed) fails too
#
# 1. `fid test-rules` passes, and fails with FID_NAT_MUTATE=rule<N> on rule N
#    alone (the switch works; no image is built for this).
# 2. Base: every test built and run without a mutant; a test that does not
#    build or run here is left out (listed).
# 3. Each mutant: the tests are built with it, the killers first; an image
#    byte-identical to the base one is not run (the mutant did nothing
#    there); otherwise its output and exit status are compared with the
#    base. The first difference (or a build that fails) kills the mutant.
# Verdicts: killed (by which test, how), SURVIVED (some image changed, no
# test noticed), inert (no image changed: the suite never reaches the
# rule on this target).
# Exit status: 0 when no mutant survived (and, with STRICT=1, none was
# inert) and step 1 passed; 1 otherwise; 2 when no test could be built.
# Report: $OUT/report.txt.
set -u
cd "$(dirname "$0")/.." || exit 2
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
if [ -z "${TARGET:-}" ]; then
  if [ $WIN = 1 ]; then TARGET=nat; else TARGET=x86_64-linux; fi
fi
OUT="${OUT:-build/mutation-check}"
JOBS="${JOBS:-4}"
TIMEOUT="${TIMEOUT:-30}"
export FIDES_NO_CACHE=1
PY=python3; command -v python3 > /dev/null 2>&1 || PY=python
mkdir -p "$OUT/base" "$OUT/m"
REPORT="$OUT/report.txt"
: > "$REPORT"
say() { echo "$*"; echo "$*" >> "$REPORT"; }

# --- the runner of an image of $TARGET -----------------------------------
RUNNER=""
case "$TARGET" in
  nat)
    [ $WIN = 1 ] || { echo "TARGET=nat needs a Windows host"; exit 2; } ;;
  x86_64-linux)
    if [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; then RUNNER=""
    elif command -v wsl.exe > /dev/null 2>&1; then RUNNER="wsl.exe -e"
    else echo "no way to run x86_64-linux images here"; exit 2; fi ;;
  aarch64-linux|riscv64-linux)
    q="qemu-${TARGET%%-*}"
    if [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = "${TARGET%%-*}" ]; then RUNNER=""
    elif command -v "$q" > /dev/null 2>&1; then RUNNER="$q"
    elif command -v wsl.exe > /dev/null 2>&1 && wsl.exe -e sh -c "command -v $q" > /dev/null 2>&1; then RUNNER="wsl.exe -e $q"
    else echo "no way to run $TARGET images here (no $q)"; exit 2; fi ;;
  *) echo "unknown TARGET $TARGET"; exit 2 ;;
esac
EXT=""
[ "$TARGET" = nat ] && EXT=".exe"

# build_img <test> <image> <mutant or ""> : 0 when the image was written
build_img() {
  local t="$1" img="$2" mu="$3" vf=""
  [ "${VERIFY:-}" = 1 ] && vf=1
  rm -f "$img"
  if [ "$TARGET" = nat ]; then
    FID_VERIFY="$vf" FID_NAT_MUTATE="$mu" timeout 600 "$FID" emit "$t" --nat -o "$img" > "$img.build.log" 2>&1
  else
    FID_VERIFY="$vf" FID_NAT_MUTATE="$mu" timeout 600 "$FID" emit "$t" --target "$TARGET" -o "$img" > "$img.build.log" 2>&1
  fi
  local rc=$?
  [ $rc = 0 ] && [ -s "$img" ]
}

# run_img <image> <out file>: the output (at most 1 MB) and the exit status
# (in <out file>.rc; "timeout" when it ran too long)
run_img() {
  local img="$1" o="$2" p="$1"
  if [ -n "$RUNNER" ] && [ "${RUNNER#wsl.exe}" != "$RUNNER" ]; then
    # WSL: the image by its /mnt path
    p=$(cd "$(dirname "$img")" && pwd)/$(basename "$img")
    p="/mnt/$(echo "$p" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
    MSYS_NO_PATHCONV=1 timeout "$TIMEOUT" $RUNNER "$p" < /dev/null 2>&1 | head -c 1048576 | tr -d '\r' > "$o"
  else
    timeout "$TIMEOUT" $RUNNER "$p" < /dev/null 2>&1 | head -c 1048576 | tr -d '\r' > "$o"
  fi
  local rc=${PIPESTATUS[0]}
  [ "$rc" = 124 ] && rc=timeout
  echo "$rc" > "$o.rc"
}

sha() { sha256sum "$1" | cut -c1-16; }

# --- 1. the rules' switch, on values ---------------------------------------
say "mutation-check: $FID, target $TARGET"
step1=0
RULES=$("$FID" test-rules 2>/dev/null)
rrc=$?
RULES=$(echo "$RULES" | tr -d '\r')
NRULES=$(echo "$RULES" | grep -cE '^(ok|FAIL) ')
if [ $rrc != 0 ] || [ "$NRULES" = 0 ] || echo "$RULES" | grep -q '^FAIL '; then
  say "FAIL fid test-rules does not pass without a mutant"
  step1=1
fi
RULE_MUTANTS=""
i=0
while [ "$i" -lt "$NRULES" ]; do
  name=$(echo "$RULES" | grep -E '^(ok|FAIL) ' | sed -n "$((i + 1))p" | sed -E 's/^(ok|FAIL) +//')
  RULE_MUTANTS="$RULE_MUTANTS rule$i"
  # `x op x` rules have no constant to shift
  if ! echo "$name" | grep -qE '^x [^ ]+ x( |$)'; then RULE_MUTANTS="$RULE_MUTANTS rulec$i"; fi
  got=$(FID_NAT_MUTATE="rule$i" "$FID" test-rules 2>/dev/null | tr -d '\r')
  nfail=$(echo "$got" | grep -c '^FAIL ')
  if [ "$nfail" != 1 ] || ! echo "$got" | grep -qF -e "FAIL  MUTANT $name"; then
    say "FAIL FID_NAT_MUTATE=rule$i: fid test-rules should fail on \"$name\" alone (failed: $nfail)"
    step1=1
  fi
  i=$((i + 1))
done
[ $step1 = 0 ] && say "ok   fid test-rules: $NRULES rules pass; each rule<N> mutant fails its rule alone"

if [ -n "${MUTANTS:-}" ]; then
  LIST="$MUTANTS"
else
  NAT_MUTANTS=$(grep -ohE 'ng_mutant\("[a-z0-9_]+"\)' compiler/nat_ir.fd compiler/nat_enc.fd 2>/dev/null | sed -E 's/.*"(.*)".*/\1/' | sort -u | tr '\n' ' ')
  LIST="$RULE_MUTANTS egcomm egflip egfold egident egzero egdouble egsame $NAT_MUTANTS"
fi

# --- the tests --------------------------------------------------------------
KILLERS="tests/bootstrap/test_mutation_kill.fd tests/bootstrap/test_egraph_rule_families.fd tests/bootstrap/test_machine_rules_random.fd tests/bootstrap/test_opt_fold.fd tests/bootstrap/test_verify_opt_dse_sccp_iv.fd tests/bootstrap/test_wrapping.fd"
TLIST=()
for k in $KILLERS; do [ -f "$k" ] && TLIST+=("$k"); done
if [ $# -gt 0 ]; then
  TLIST=("$@")
elif [ "${TESTS:-}" != quick ]; then
  long=$(sed -E 's/[[:space:]].*//' scripts/long-tests.txt 2>/dev/null | grep -v '^#' | tr '\n' ' ')
  rest=()
  for f in tests/bootstrap/*.fd; do
    b=$(basename "$f" .fd)
    case "$b" in *_fail) continue ;; esac
    case " $KILLERS " in *" $f "*) continue ;; esac
    case " $long " in *" $b "*) continue ;; esac
    rest+=("$f")
  done
  if [ -n "${SAMPLE:-}" ]; then
    mapfile -t rest < <(printf '%s\n' "${rest[@]}" | "$PY" -c 'import random,sys; l=[x.strip() for x in sys.stdin if x.strip()]; random.Random(1).shuffle(l); print("\n".join(sorted(l[:int(sys.argv[1])])))' "$SAMPLE")
  fi
  TLIST+=("${rest[@]}")
fi
key() { echo "$1" | sed -E 's|^tests/||; s|\.fd$||; s|/|_|g'; }

# --- 2. base ----------------------------------------------------------------
base_one() {
  local t="$1" k
  k=$(key "$t")
  local img="$OUT/base/$k$EXT"
  if ! build_img "$t" "$img" ""; then echo "nobuild" > "$OUT/base/$k.state"; return; fi
  run_img "$img" "$OUT/base/$k.out"
  if [ "$(cat "$OUT/base/$k.out.rc")" = timeout ]; then echo "timeout" > "$OUT/base/$k.state"; return; fi
  sha "$img" > "$OUT/base/$k.sha"
  echo ok > "$OUT/base/$k.state"
}
for t in "${TLIST[@]}"; do
  while [ "$(jobs -rp | wc -l)" -ge "$JOBS" ]; do wait -n; done
  base_one "$t" &
done
wait
USE=()
dropped=""
for t in "${TLIST[@]}"; do
  k=$(key "$t")
  if [ "$(cat "$OUT/base/$k.state" 2>/dev/null)" = ok ]; then USE+=("$t"); else dropped="$dropped $k($(cat "$OUT/base/$k.state" 2>/dev/null))"; fi
done
say "base: ${#USE[@]} of ${#TLIST[@]} tests built and ran${dropped:+; left out:$dropped}"
[ ${#USE[@]} = 0 ] && { say "no test could be built: nothing to compare"; exit 2; }
# Same input, same bytes? Otherwise an unchanged image cannot be told apart
# and every mutant's images are run (slower; "inert" is not reported)
DET=1
t0="${USE[0]}"; k0=$(key "$t0")
if build_img "$t0" "$OUT/base/det$EXT" "" && [ "$(sha "$OUT/base/det$EXT")" != "$(cat "$OUT/base/$k0.sha")" ]; then
  DET=0
  say "note: two builds of $k0 differ: images are not compared byte for byte"
fi

# --- 3. each mutant ---------------------------------------------------------
mutant_one() {
  local mu="$1" d="$OUT/m/$1" changed=0 t k img
  mkdir -p "$d"
  for t in "${USE[@]}"; do
    k=$(key "$t")
    img="$d/$k$EXT"
    if ! build_img "$t" "$img" "$mu"; then
      local why="build failed"
      grep -qiE 'TG[0-9]|verif|check' "$img.build.log" 2>/dev/null && why="build failed (a checker: $(grep -m1 -iE 'TG[0-9]|verif|check' "$img.build.log" | cut -c1-80))"
      echo "killed|$k: $why|$changed" > "$d.result"
      return
    fi
    if [ $DET = 1 ] && [ "$(sha "$img")" = "$(cat "$OUT/base/$k.sha")" ]; then
      rm -f "$img"
      continue
    fi
    changed=$((changed + 1))
    run_img "$img" "$d/$k.out"
    if [ "$(cat "$d/$k.out.rc")" != "$(cat "$OUT/base/$k.out.rc")" ]; then
      echo "killed|$k: exit $(cat "$OUT/base/$k.out.rc") -> $(cat "$d/$k.out.rc")|$changed" > "$d.result"
      return
    fi
    if ! cmp -s "$d/$k.out" "$OUT/base/$k.out"; then
      echo "killed|$k: output differs ($(diff "$OUT/base/$k.out" "$d/$k.out" | grep -m1 '^[<>]' | cut -c1-60))|$changed" > "$d.result"
      return
    fi
    rm -f "$img"
  done
  if [ $changed = 0 ] && [ $DET = 1 ]; then
    echo "inert|no image changed|0" > "$d.result"
  else
    echo "SURVIVED|$changed images changed, every output the same|$changed" > "$d.result"
  fi
}
for mu in $LIST; do
  rm -f "$OUT/m/$mu.result"
  while [ "$(jobs -rp | wc -l)" -ge "$JOBS" ]; do wait -n; done
  mutant_one "$mu" &
done
wait

killed=0; survived=0; inert=0
for mu in $LIST; do
  r=$(cat "$OUT/m/$mu.result" 2>/dev/null || echo "SURVIVED|no result|0")
  v="${r%%|*}"; rest="${r#*|}"; why="${rest%|*}"
  case "$v" in
    killed) killed=$((killed + 1)) ;;
    inert) inert=$((inert + 1)) ;;
    *) survived=$((survived + 1)) ;;
  esac
  say "$(printf '%-10s %-9s %s' "$mu" "$v" "$why")"
done
total=$((killed + survived + inert))
say "mutation-check ($TARGET): $total mutants, $killed killed, $survived survived, $inert inert; ${#USE[@]} tests"
bad=$step1
[ $survived -gt 0 ] && bad=1
[ "${STRICT:-}" = 1 ] && [ $inert -gt 0 ] && bad=1
exit $bad
