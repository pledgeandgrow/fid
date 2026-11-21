#!/bin/bash
# wp-defaults-check.sh — llvm-roadmap WP7 (envtrim), WP14 (spec) and WP15
# (deadf), on by default in compiler/iropt.fd since Nov 17 (FID_IROPT_OFF
# names each off). WP10 (readcse, compiler/rules.fd) is checked by
# tests/tooling/op25-check.sh.
# What it checks, one line each (OK / FAIL), exit status the number of FAILs:
#
#  1. The passes do something (FID_IROPT_STATS=1, inline and inline2 off so
#     the calls they act on stay calls):
#     - envtrim: "iropt: envtrim N calls" with N > 0 for
#       test_irswitch_envtrim.fd, 0 with FID_IROPT_OFF=envtrim;
#     - spec: the "spec N" of the "iropt: fold ..." line > 0 for
#       test_midend_spec_hot.fd (scale is cloned for its hot constants),
#       0 with FID_IROPT_OFF=spec;
#     - deadf: "iropt: dead functions N, dead globals M" with N > 0 and
#       M > 0 for test_irswitch_envtrim.fd (g_written is only stored), both
#       0 with FID_IROPT_OFF=deadf.
#  2. The tests print their "Expected output:" lines (FID_VERIFY=1) by
#     default and with envtrim,spec,deadf named off, through pe_ir and
#     `--nat`: test_irswitch_envtrim, test_midend_spec_hot,
#     test_midend_wp_named (task entries started by name: envtrim keeps the
#     env of one that is also called directly, deadf keeps one nothing
#     calls).
#  3. Programs whose bugs these passes once caused, default vs all three
#     off, same output and exit status: test_brotli (pe_ir and --nat; the
#     envtrim miscompile of Nov 9), test_simd, test_simd_i64,
#     test_machine_simd (--nat: nat_ir's vector loops call core's
#     vec_buf_alloc by name with a null env first).
#  4. SUITE=1: every program of tests/bootstrap through
#     tests/tooling/midend-pass-suite-check.sh with envtrim,spec,deadf named
#     off and on (JOBS passed on; NAT=1: again with --nat). Must end
#     "0 differ" (exit 0).
#  5. TIME=1: the iropt time of envtrim, spec and deadf on
#     test_collections.fd (FID_IROPT_STATS=1 "iropt: pass <name>: N ms"),
#     printed (INFO), not judged.
#
# Usage: tests/tooling/wp-defaults-check.sh
#   FID=<compiler> (default release/fid.exe), OUT=<dir> (default
#   build/wp-defaults), SUITE=1, NAT=1, TIME=1, JOBS=<n>
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/wp-defaults}"
SUITE="${SUITE:-0}"
NAT="${NAT:-0}"
TIME="${TIME:-0}"
JOBS="${JOBS:-1}"
mkdir -p "$OUT"
bad=0
ok() { echo "OK    $1"; }
no() { echo "FAIL  $1"; bad=$((bad + 1)); }

E=tests/bootstrap/test_irswitch_envtrim.fd
S=tests/bootstrap/test_midend_spec_hot.fd
N=tests/bootstrap/test_midend_wp_named.fd
OFF3=envtrim,spec,deadf

# build <exe> <env...> -- <emit args...>: 0 when the executable was made
build() {
  local exe="$1"; shift
  local envs=()
  while [ "$1" != "--" ]; do envs+=("$1"); shift; done
  shift
  rm -f "$exe"
  env "${envs[@]}" "$FID" emit "$@" -o "$exe" > "$exe.log" 2>&1 && [ -f "$exe" ]
}

# The "Expected output:" lines of a test's header comment
want() {
  awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$1"
}

# stat <label> <sed expression printing the number> <env...> -- <emit args>
stat() {
  local label="$1" expr="$2"; shift 2
  local exe="$OUT/$label.exe"
  if build "$exe" FID_IROPT_STATS=1 "$@"; then
    sed -n "$expr" "$exe.log" | tr -d '\r' | head -1
  else
    echo "-1"
  fi
}
want_pos() {  # <what> <count>
  if [ -n "$2" ] && [ "$2" -gt 0 ] 2>/dev/null; then ok "stats $1: $2"; else no "stats $1: '$2' (expected > 0)"; fi
}
want_zero() {
  if [ "$2" = 0 ]; then ok "stats $1: 0"; else no "stats $1: '$2' (expected 0)"; fi
}

# 1. counters
ET='s/^iropt: envtrim \([0-9]*\) calls.*/\1/p'
SP='s/^iropt: fold .*, spec \([0-9]*\),.*/\1/p'
DF='s/^iropt: dead functions \([0-9]*\), dead globals [0-9]*.*/\1/p'
DG='s/^iropt: dead functions [0-9]*, dead globals \([0-9]*\).*/\1/p'
want_pos "envtrim" "$(stat et "$ET" FID_IROPT_OFF=inline,inline2 -- "$E")"
want_zero "envtrim off" "$(stat et_off "$ET" FID_IROPT_OFF=inline,inline2,envtrim -- "$E")"
want_pos "spec" "$(stat sp "$SP" FID_IROPT_OFF=inline,inline2 -- "$S")"
want_zero "spec off" "$(stat sp_off "$SP" FID_IROPT_OFF=inline,inline2,spec -- "$S")"
# Nov 21: the emit path lowers only what compiler/reach.fd finds reachable
# (PE88), so functions the passes made dead are gone before iropt counts
# them (155 with FID_PE_PRUNE=0, 0 without); the deadf counters are read
# with the prune off, the outputs below stay checked with it on.
want_pos "deadf functions" "$(stat df "$DF" FID_PE_PRUNE=0 FID_IROPT_OFF=inline,inline2 -- "$E")"
want_pos "deadf globals" "$(sed -n "$DG" "$OUT/df.exe.log" | tr -d '\r' | head -1)"
want_zero "deadf off functions" "$(stat df_off "$DF" FID_PE_PRUNE=0 FID_IROPT_OFF=inline,inline2,deadf -- "$E")"
want_zero "deadf off globals" "$(sed -n "$DG" "$OUT/df_off.exe.log" | tr -d '\r' | head -1)"

# 2. expected output, default and with the three off
for t in "$E" "$S" "$N"; do
  n=$(basename "$t" .fd)
  want "$t" > "$OUT/$n.want"
  for tgt in pe_ir nat; do
    extra=""
    [ "$tgt" = nat ] && extra="--nat"
    for cfg in default off; do
      case "$cfg" in
        default) envs=(X=1) ;;
        off) envs=(FID_IROPT_OFF=$OFF3) ;;
      esac
      exe="$OUT/$n.$tgt.$cfg.exe"
      if build "$exe" "${envs[@]}" FID_VERIFY=1 -- "$t" $extra; then
        timeout 60 "$exe" < /dev/null > "$exe.out" 2>&1
        if cmp -s <(tr -d '\r' < "$exe.out") "$OUT/$n.want"; then
          ok "output $n $tgt $cfg"
        else
          no "output $n $tgt $cfg (see $exe.out)"
        fi
      else
        no "build $n $tgt $cfg: $(grep -m1 -i -E 'error|panic|invalid|verif' "$exe.log")"
      fi
    done
  done
done

# 3. default vs off, same output and exit status
same() {  # <test> <tgt>
  local t="$1" tgt="$2" extra="" n
  n=$(basename "$t" .fd)
  [ "$tgt" = nat ] && extra="--nat"
  local a="$OUT/$n.$tgt.d.exe" b="$OUT/$n.$tgt.o.exe" ra rb
  if ! build "$a" FID_VERIFY=1 -- "$t" $extra; then
    no "build $n $tgt default: $(grep -m1 -i -E 'error|panic|invalid|verif' "$a.log")"
    return
  fi
  if ! build "$b" FID_IROPT_OFF=$OFF3 -- "$t" $extra; then
    no "build $n $tgt off: $(grep -m1 -i -E 'error|panic|invalid' "$b.log")"
    return
  fi
  timeout 120 "$a" < /dev/null > "$a.out" 2>&1; ra=$?
  timeout 120 "$b" < /dev/null > "$b.out" 2>&1; rb=$?
  if [ "$ra" = "$rb" ] && cmp -s "$a.out" "$b.out"; then
    ok "same $n $tgt (rc $ra)"
  else
    no "same $n $tgt (rc default $ra, off $rb): $(diff "$b.out" "$a.out" | head -4 | tr '\n' '|')"
  fi
}
same tests/bootstrap/test_brotli.fd pe_ir
same tests/bootstrap/test_brotli.fd nat
for t in test_simd test_simd_i64 test_machine_simd; do
  [ -f "tests/bootstrap/$t.fd" ] && same "tests/bootstrap/$t.fd" nat
done

# 4. the suite, on and off
if [ "$SUITE" = 1 ]; then
  suite() {  # <extra>
    local tag="pe_ir"
    [ -n "$1" ] && tag="nat"
    local log="$OUT/suite-$tag.txt"
    PASS="$OFF3" FID="$FID" JOBS="$JOBS" EXTRA="$1" OUT="$OUT/suite-$tag" \
      bash tests/tooling/midend-pass-suite-check.sh > "$log" 2>&1
    local rc=$?
    if [ "$rc" = 0 ]; then
      ok "suite $OFF3 $tag: $(tail -1 "$log" | tr -d '\r')"
    else
      no "suite $OFF3 $tag (exit $rc; see $log): $(grep -E '^(DIFF|BUILD|VERIFY)' "$log" | head -5 | tr '\n' '|')"
    fi
  }
  suite ""
  if [ "$NAT" = 1 ]; then
    suite --nat
  fi
fi

# 5. what the three cost
if [ "$TIME" = 1 ]; then
  exe="$OUT/time.exe"
  if build "$exe" FID_IROPT_STATS=1 -- tests/bootstrap/test_collections.fd; then
    for p in envtrim spec deadf; do
      echo "INFO  iropt time $p: $(grep -m1 "^iropt: pass $p:" "$exe.log" | tr -d '\r' | sed 's/^iropt: pass [a-z]*: //')"
    done
  fi
fi

echo "wp-defaults-check: $bad failed"
exit "$bad"
