#!/bin/bash
# vf15_long_run.sh — the parser fuzzer's run of record (roadmap VF15).
#
#   FID=release/fid.exe scripts/fuzz/vf15_long_run.sh [out_dir]
#       TLS_N=5000 (inputs per tls seed)  ALL_N=20000 (inputs per seed over
#       every target)  SEEDS_TLS="1 2"  SEEDS_ALL="3 4"
#
# 1. the tls target (stdlib/tls_record.fd, binary seeds) alone, one run per
#    seed in SEEDS_TLS;
# 2. every target (json regex http csv toml xml url uuid tls), one run per
#    seed in SEEDS_ALL;
# 3. the repros of the earlier findings P1-P3 (tests/verify/bugs/p*.fd) and
#    tests/bootstrap/test_verify_vf15_parsers.fd, each through `run` and
#    `ccompile`: each must print what its //> lines say.
# Each run's findings stay in <out_dir>/<run>/ (input and output side by
# side); <out_dir>/summary.txt has one line per run and per repro.
# Exit status: 0 when no run found anything and every repro passed.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
export CC="${CC:-zig cc}"
OUT="${1:-build/vf15-long}"
mkdir -p "$OUT"
OUT="$(cd "$OUT" && pwd)"
PY="${PYTHON:-python}"
command -v "$PY" > /dev/null || PY=python3
SUM="$OUT/summary.txt"
{
  echo "VF15 parser fuzz, $(date -u +%Y-%m-%dT%H:%M:%SZ), $(uname -srm)"
  echo "  fid: $FID"
  echo "  commit $(git rev-parse --short HEAD 2>/dev/null)$(git diff --quiet 2>/dev/null || echo ' + uncommitted changes')"
} > "$SUM"
fail=0

fuzz() {  # name seed n parsers
  local name="$1" seed="$2" n="$3" parsers="$4" start log
  start=$(date +%s)
  log="$OUT/$name.log"
  "$PY" scripts/fuzz/fuzz_parsers.py --fid "$FID" -n "$n" --seed "$seed" \
      --parsers "$parsers" --out "$OUT/$name" > "$log" 2>&1
  local rc=$?
  [ $rc -eq 0 ] || fail=1
  echo "  $name: $(grep -E '^[0-9]+ inputs' "$log" | head -1 || echo "no summary (exit $rc)") ($(( $(date +%s) - start )) s)" >> "$SUM"
  grep -E '^  [a-z]+: ' "$log" | sed 's/^/    /' >> "$SUM"
  tail -3 "$log"
}

for s in ${SEEDS_TLS-1 2}; do
  fuzz "tls-s$s" "$s" "${TLS_N:-5000}" tls
done
for s in ${SEEDS_ALL-3 4}; do
  fuzz "all-s$s" "$s" "${ALL_N:-20000}" json,regex,http,csv,toml,xml,url,uuid,tls
done

# the repros: each file's //> lines are what it must print
check() {  # file backend
  local f="$1" b="$2" got want exe
  want=$(grep '^//> ' "$f" | sed 's#^//> ##')
  [ -n "$want" ] || want=$(sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$f" | grep '^// ' | sed 's#^// ##' | grep -v '^Expected output:$')
  if [ "$b" = run ]; then
    got=$(timeout 300 "$FID" run "$f" 2> /dev/null | tr -d '\r')
  else
    exe="$OUT/repro-$(basename "$f" .fd).exe"
    "$FID" ccompile "$f" -o "$exe" > /dev/null 2>&1
    got=$(timeout 300 "$exe" 2> /dev/null | tr -d '\r')
  fi
  if [ "$got" = "$want" ]; then
    echo "  pass  $b $f" >> "$SUM"
  else
    fail=1
    echo "  FAIL  $b $f: printed '$(echo "$got" | tail -1)'" >> "$SUM"
  fi
}
for f in tests/verify/bugs/p1_url_port_overflow.fd tests/verify/bugs/p2_regex_brace_overflow.fd \
         tests/verify/bugs/p3_xml_lt_oom.fd tests/bootstrap/test_verify_vf15_parsers.fd; do
  check "$f" run
  check "$f" ccompile
done
cat "$SUM"
exit "$fail"
