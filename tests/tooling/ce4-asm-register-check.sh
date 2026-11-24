#!/usr/bin/env bash
# ce4-asm-register-check.sh -- the two asm! register errors of the bare
# native build (compiler/pe_ir.fd, Inst::Asm in bare mode): an in() or out()
# operand that names no 64-bit general register is an error of
# `fid emit --bare`, with the function and the register named
# (docs/roadmap/complaint-roadmap.md CE4: a test for each error message).
#
# They come from code generation, not from the checks `fid check`/`fid run`
# make, so they cannot be expected-fail tests of tests/bootstrap (tier1
# refuses those through `fid run`, which runs asm! as a no-op).
#
# Checks:
#   1. in("xmm0")  -> asm! in main: in("xmm0") is not a 64-bit register
#   2. out("st0")  -> asm! in main: out("st0") is not a 64-bit register
#   3. in("rcx") / out("rax") build (the registers it accepts)
#
# Usage: tests/tooling/ce4-asm-register-check.sh   (FID=<fid> to choose)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
export FID_HOME="${FID_HOME:-$ROOT}"
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
bad() { echo "  FAIL  $1"; fail=1; }
ok() { echo "  ok    $1"; }
has() {
  if grep -qF -- "$3" "$TMP/$2"; then ok "$1"
  else bad "$1: no '$3' in $2"; grep -v '^\[' "$TMP/$2" | head -10; fi
}

cat > "$TMP/in_xmm.fd" <<'EOF'
@unsafe
fn main() -> i64 {
    val x: i64 = 1;
    asm!("nop", in("xmm0") x);
    return 0;
}
EOF
cat > "$TMP/out_st0.fd" <<'EOF'
@unsafe
fn main() -> i64 {
    var x: i64 = 1;
    val y: i64 = asm!("nop", out("st0") x);
    return y;
}
EOF
cat > "$TMP/good.fd" <<'EOF'
@unsafe
fn main() -> i64 {
    var x: i64 = 1;
    val y: i64 = asm!("mov rax, rcx", in("rcx") x, out("rax") x);
    return y;
}
EOF

# 1. in() of a register that is not a 64-bit general one
if "$FID" emit "$TMP/in_xmm.fd" --bare -o "$TMP/in_xmm.bin" > "$TMP/in.log" 2>&1; then
  bad "1. emit --bare accepted in(\"xmm0\")"
else
  ok "1. emit --bare refuses in(\"xmm0\")"
fi
has "1. the message" in.log 'asm! in main: in("xmm0") is not a 64-bit register'

# 2. out() of one
if "$FID" emit "$TMP/out_st0.fd" --bare -o "$TMP/out_st0.bin" > "$TMP/out.log" 2>&1; then
  bad "2. emit --bare accepted out(\"st0\")"
else
  ok "2. emit --bare refuses out(\"st0\")"
fi
has "2. the message" out.log 'asm! in main: out("st0") is not a 64-bit register'

# 3. the general registers build
if "$FID" emit "$TMP/good.fd" --bare -o "$TMP/good.bin" > "$TMP/good.log" 2>&1; then
  ok "3. in(\"rcx\") and out(\"rax\") build"
else
  bad "3. in(\"rcx\")/out(\"rax\") refused"; grep -v '^\[' "$TMP/good.log" | head -10
fi

if [ "$fail" = 0 ]; then echo "ce4-asm-register-check: ok"; else echo "ce4-asm-register-check: FAILED"; fi
exit "$fail"
