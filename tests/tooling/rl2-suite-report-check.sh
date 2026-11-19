#!/bin/bash
# rl2-suite-report-check.sh — RL2 (llvm-roadmap: the suite on every backend
# and target on every change). Checks the parts that need no compiler, one
# line each (OK / FAIL); the exit status is the number of FAILs:
#
#  1. scripts/verify/suite-all-report.py --selftest: made-up lane folders;
#     every cause (a runtime name bound from libc, output, crash, panic, a
#     compiler panic, a zig cache race), the lane patterns and the Linux
#     binaries importing runtime names are recognised, a clean run is 0.
#  2. scripts/suite-all.sh knows ten lanes (LIST_LANES=1) and calls the
#     report at the end of every run ($OUT/report.md).
#  3. scripts/suite-lanes-check.sh: every lane runs in a workflow on push
#     and pull request; SELFTEST=1: a dropped lane and a push-only workflow
#     are both caught.
#  4. The WASI SO_ERROR gap stays closed: compiler/codegen_c.fd defines
#     SO_ERROR in its __wasi__ block (wasi-libc has no SO_ERROR; every C
#     program for wasm32-wasi failed to build, Nov 16).
#  5. FID=<compiler> given: hello.fd through C for wasm32-wasi builds and,
#     with node, prints what `fid run` prints (the lane's first test).
#  6. FID given: native executables for the three Linux targets import no
#     runtime name from a shared library (scripts/bin-target-check.py; the
#     riscv64 lane's "undefined symbol: nat_tls_set").
#
# Usage: tests/tooling/rl2-suite-report-check.sh   (FID=..., CC as for
#        scripts/cross-test.sh; OUT=<dir>, default build/rl2-check)
set -u
cd "$(dirname "$0")/../.." || exit 1
OUT="${OUT:-build/rl2-check}"
mkdir -p "$OUT"
bad=0
ok() { echo "OK    $1"; }
no() { echo "FAIL  $1"; bad=$((bad + 1)); }
PY=""
for p in python3 python; do
  if command -v "$p" > /dev/null 2>&1 && "$p" -c 'import sys; sys.exit(sys.version_info[0] < 3)' 2> /dev/null; then PY=$p; break; fi
done

# 1
if [ -z "$PY" ]; then
  no "no python 3 for scripts/verify/suite-all-report.py"
else
  r=$("$PY" scripts/verify/suite-all-report.py --selftest 2>&1)
  if echo "$r" | grep -q "selftest: ok"; then ok "report selftest: $r"; else no "report selftest: $r"; fi
fi

# 2
n=$(LIST_LANES=1 bash scripts/suite-all.sh | grep -cE '^(c|native):')
if [ "$n" = 10 ]; then ok "suite-all.sh knows 10 lanes"; else no "suite-all.sh knows $n lanes, want 10"; fi
if grep -q 'scripts/verify/suite-all-report.py "\$OUT" --lanes' scripts/suite-all.sh; then
  ok "suite-all.sh writes \$OUT/report.md"
else
  no "suite-all.sh does not call scripts/verify/suite-all-report.py"
fi

# 3
r=$(bash scripts/suite-lanes-check.sh 2>&1); st=$?
if [ $st = 0 ]; then ok "suite-lanes-check: $r"; else no "suite-lanes-check: $r"; fi
r=$(SELFTEST=1 bash scripts/suite-lanes-check.sh 2>&1); st=$?
if [ $st = 0 ]; then ok "suite-lanes-check SELFTEST=1"; else no "suite-lanes-check SELFTEST=1: $(echo "$r" | tail -3)"; fi

# 4
if grep -A 40 'wln(self, "#ifdef __wasi__");' compiler/codegen_c.fd | grep -q '#define SO_ERROR'; then
  ok "codegen_c.fd defines SO_ERROR for __wasi__"
else
  no "codegen_c.fd has no SO_ERROR for __wasi__ (every wasm32-wasi C build fails)"
fi

# 5
if [ -n "${FID:-}" ]; then
  export CC="${CC:-zig cc}"
  if "$FID" ccompile tests/bootstrap/hello.fd --target wasm32-wasi --cc-opt 0 --no-cache -o "$OUT/hello.wasm" > "$OUT/hello.wasm.log" 2>&1 && [ -f "$OUT/hello.wasm" ]; then
    ok "hello.fd builds for wasm32-wasi"
    if command -v node > /dev/null 2>&1; then
      "$FID" run tests/bootstrap/hello.fd < /dev/null 2> /dev/null | tr -d '\r' > "$OUT/hello.expected"
      node scripts/wasm-run.mjs "$OUT/hello.wasm" < /dev/null 2> /dev/null | tr -d '\r' > "$OUT/hello.actual"
      if [ -s "$OUT/hello.actual" ] && cmp -s "$OUT/hello.expected" "$OUT/hello.actual"; then
        ok "hello.wasm under node prints what fid run prints"
      else
        no "hello.wasm under node: $(head -c 200 "$OUT/hello.actual")"
      fi
    else
      echo "SKIP  node not found: hello.wasm not run"
    fi
  else
    no "hello.fd for wasm32-wasi did not build: $(grep -m1 -i error "$OUT/hello.wasm.log")"
  fi
else
  echo "SKIP  FID not set: the wasm32-wasi build (check 5) not run"
fi

# 6. FID given: the native Linux lanes' executables import none of the
#    runtime's own names. On Nov 18 the stage-0 compiler's hello.fd for
#    x86_64/aarch64/riscv64-linux imported nat_tls_set and 16 more nat_*
#    names and p_sensitive_check/_free/_remark from libc.so.6 (the CI2
#    branch of compiler/nat_ir.fd ng_gen_call_std): every test of those
#    lanes then stops with "symbol lookup error". hello.fd and
#    test_verify_rl3_rule_identities.fd (Vec code: core's p_sensitive_*
#    calls) are built only, and read by scripts/bin-target-check.py.
if [ -n "${FID:-}" ] && [ -n "$PY" ]; then
  for tg in x86_64-linux aarch64-linux riscv64-linux; do
    for src in tests/bootstrap/hello.fd tests/bootstrap/test_verify_rl3_rule_identities.fd; do
      b="$OUT/$(basename "$src" .fd)-$tg.bin"
      rm -f "$b"
      if "$FID" emit "$src" --target "$tg" -o "$b" > "$b.log" 2>&1 && [ -f "$b" ]; then
        r=$("$PY" scripts/bin-target-check.py "$tg" "$b" 2>&1)
        if echo "$r" | grep -q ", 0 bad"; then
          ok "$(basename "$src") for $tg imports no runtime name"
        else
          no "$(basename "$src") for $tg: $(echo "$r" | grep -m1 FAIL | sed 's/.*: imports/imports/')"
        fi
      else
        no "$(basename "$src") for $tg did not build: $(grep -m1 -i 'error\|panic' "$b.log")"
      fi
    done
  done
fi
echo "rl2-suite-report-check: $bad failed"
exit $bad
