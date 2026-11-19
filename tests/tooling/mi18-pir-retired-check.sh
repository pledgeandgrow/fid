#!/usr/bin/env bash
# mi18-pir-retired-check.sh — the PIR path is retired (llvm-roadmap MI18).
#
# compiler/main.fd no longer has a command that prints or builds through
# PIR (pir.fd, opt.fd, regalloc.fd, codegen_x64/arm64/riscv.fd,
# peephole.fd); compiler.fd build_executable is gone. What the old commands
# do now:
#   fid pir | dump-pir <f>          exit 1, name what replaced them
#   fid opt <f> [--fn <name>]       the IR after the IR optimizer (iropt_native)
#   fid codegen <f> [--target t]    the machine layer's code as instructions
#                                   (fid disasm), x86-64/arm64/riscv -> Linux
#   fid codegen <f> --elf|--coff|--macho <o>
#                                   the machine layer's object file (emit --obj)
#   fid repl  :pir                  says it was removed; :opt <fn> replaced it
#                                   (tests/tooling/repl-*-check.sh)
# and no module that main.fd or compiler.fd imports for these commands is
# imported any more (static check of the `use` lines). The last two
# importers of pir.fd (elf64.fd, pe.fd: other owners) are reported, not
# failed, until they drop it; then compiler/pir.fd and the rest can be
# deleted (STRICT=1 fails while any import is left).
#
#   FID=release/fid.exe bash tests/tooling/mi18-pir-retired-check.sh
# Run from the repository root. Exit 0 when every check passes.
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
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }

# --- static: the `use` lines --------------------------------------------
echo "imports:"
RETIRED="pir opt regalloc codegen_x64 codegen_arm64 codegen_riscv peephole"
for f in compiler/main.fd compiler/compiler.fd; do
  for m in $RETIRED pe coff macho; do
    if grep -qE "use compiler::$m;" "$f"; then bad "$f still imports compiler::$m"; imp_bad=1; fi
  done
done
[ "${imp_bad:-0}" = 0 ] && ok "main.fd and compiler.fd import none of: $RETIRED pe coff macho"
if grep -qE 'fn build_executable\b|build_executable\(' compiler/compiler.fd compiler/main.fd; then bad "build_executable is still defined or called"; else ok "build_executable is gone"; fi
if grep -qE 'fn (cmd_pir|repl_fn_pir)\b' compiler/main.fd compiler/compiler.fd; then bad "cmd_pir / repl_fn_pir still defined"; else ok "cmd_pir and repl_fn_pir are gone"; fi
left=""
for m in $RETIRED; do
  for f in compiler/*.fd; do
    case "$f" in compiler/pir.fd|compiler/opt.fd|compiler/regalloc.fd|compiler/codegen_x64.fd|compiler/codegen_arm64.fd|compiler/codegen_riscv.fd|compiler/peephole.fd|compiler/pe.fd) continue ;; esac
    [ -f "$f" ] && grep -qE "use compiler::$m;" "$f" && left="$left $f:$m"
  done
done
if [ -z "$left" ]; then ok "no live module imports a retired one: compiler/{${RETIRED// /,}}.fd can be deleted"
elif [ "${STRICT:-0}" = 1 ]; then bad "still imported:$left"
else echo "  note  still imported (other owners; STRICT=1 fails):$left"; fi

# --- the commands -----------------------------------------------------------
cat > "$TMP/p.fd" <<'EOF'
fn twice(x: i64) -> i64 {
    return x * 2;
}

fn main() -> i64 {
    println(int_to_str(twice(21)));
    return 0;
}
EOF
P="$TMP/p.fd"
echo "pir / dump-pir:"
for c in pir dump-pir; do
  "$FID" "$c" "$P" > "$TMP/$c.out" 2> "$TMP/$c.err"; rc=$?
  if [ "$rc" = 1 ] && grep -qF "the PIR path was removed" "$TMP/$c.err" && grep -qF "fid opt" "$TMP/$c.err" && grep -qF "fid dump-mir" "$TMP/$c.err"; then ok "fid $c says it was removed and what replaced it"
  else bad "fid $c (exit $rc): $(head -2 "$TMP/$c.err" | tr '\n' ' ')"; fi
done

echo "opt:"
if "$FID" opt "$P" > "$TMP/opt.out" 2> "$TMP/opt.err" && grep -q '^fn main(' "$TMP/opt.out"; then ok "fid opt prints the optimized module"
else bad "fid opt: $(tail -3 "$TMP/opt.err" | tr '\n' ' ')"; fi
if "$FID" opt "$P" --fn main > "$TMP/optf.out" 2> "$TMP/optf.err" && head -1 "$TMP/optf.out" | grep -q '^fn main(' && [ "$(grep -c '^fn ' "$TMP/optf.out")" = 1 ]; then ok "fid opt --fn main prints that function alone"
else bad "fid opt --fn main: $(head -2 "$TMP/optf.out" | tr '\n' ' ')"; fi
# the optimizer runs: main after `fid opt` is not main as lowered (dump-ir:
# twice's call is inlined), and with the opt-in deadf pass the inlined
# twice is removed as well
"$FID" dump-ir "$P" > "$TMP/ir.out" 2>/dev/null
fnbody() { awk -v f="^fn $2[(]" '$0 ~ f {p=1; print; next} /^fn / {p=0} p' "$1"; }
fnbody "$TMP/ir.out" main > "$TMP/ir.main"
fnbody "$TMP/opt.out" main > "$TMP/opt.main"
FID_IROPT_ON=deadf "$FID" opt "$P" > "$TMP/optd.out" 2>/dev/null
if [ -s "$TMP/ir.main" ] && [ -s "$TMP/opt.main" ] && ! cmp -s "$TMP/ir.main" "$TMP/opt.main"; then ok "fid opt runs iropt (main differs from dump-ir's)"
else bad "fid opt --fn main is main as lowered: no optimizer ran"; fi
if grep -q '^fn twice(' "$TMP/ir.out" && ! grep -q '^fn twice(' "$TMP/optd.out" && grep -q '^fn main(' "$TMP/optd.out"; then ok "FID_IROPT_ON=deadf fid opt: twice inlined and removed"
else echo "  note  FID_IROPT_ON=deadf kept twice (in dump-ir: $(grep -c '^fn twice(' "$TMP/ir.out"), after: $(grep -c '^fn twice(' "$TMP/optd.out")): inlining declined it"; fi
"$FID" opt "$P" --fn nosuch > /dev/null 2> "$TMP/optn.err"; rc=$?
if [ "$rc" = 1 ] && grep -qF "no function 'nosuch'" "$TMP/optn.err"; then ok "fid opt --fn of an unknown function fails"
else bad "fid opt --fn nosuch (exit $rc)"; fi

echo "codegen:"
for t in x86-64 arm64 riscv; do
  if "$FID" codegen "$P" --target "$t" > "$TMP/cg-$t.out" 2> "$TMP/cg-$t.err" && grep -q '^[0-9a-f]* <main>:' "$TMP/cg-$t.out"; then ok "fid codegen --target $t lists main's instructions"
  else bad "fid codegen --target $t: $(tail -3 "$TMP/cg-$t.err" | tr '\n' ' ')"; fi
done
if "$FID" codegen "$P" --fn main > "$TMP/cg-fn.out" 2>/dev/null && grep -q '<main>:' "$TMP/cg-fn.out" && ! grep -q '<twice>:' "$TMP/cg-fn.out"; then ok "fid codegen --fn main lists main only"
else bad "fid codegen --fn main"; fi
# object files: magic, and the machine of the target
hex() { od -An -tx1 -N "$2" -j "$3" "$1" 2>/dev/null | tr -d ' \n'; }
obj() {  # name, flag, target, file, magic-offset, magic-len, magic, mach-offset, mach-len, mach
  local name=$1 flag=$2 t=$3 o="$TMP/$4"
  rm -f "$o"
  if ! "$FID" codegen "$P" --target "$t" "$flag" "$o" > "$TMP/$4.log" 2>&1 || [ ! -s "$o" ]; then bad "$name: no object ($(tail -2 "$TMP/$4.log" | tr '\n' ' '))"; return; fi
  if [ "$(hex "$o" "$6" "$5")" = "$7" ] && [ "$(hex "$o" "$9" "$8")" = "${10}" ]; then ok "$name"
  else bad "$name: magic $(hex "$o" "$6" "$5") machine $(hex "$o" "$9" "$8")"; fi
}
#   ELF: 7f 45 4c 46, e_type 1 (REL) at 16, e_machine at 18 (3e x86-64, b7 AArch64, f3 RISC-V)
obj "codegen --elf (x86-64): ELF relocatable"  --elf x86-64 a.o   0 4 7f454c46 16 4 01003e00
obj "codegen --elf (arm64): ELF AArch64"       --elf arm64  b.o   0 4 7f454c46 16 4 0100b700
obj "codegen --elf (riscv): ELF RISC-V"        --elf riscv  c.o   0 4 7f454c46 16 4 0100f300
#   COFF: machine 0x8664 in the first two bytes
obj "codegen --coff: COFF x86-64"              --coff x86-64 d.obj 0 2 6486 0 2 6486
#   Mach-O 64: cf fa ed fe, cputype 0x01000007 (x86-64) / 0x0100000c (arm64)
obj "codegen --macho (x86-64): Mach-O x86-64"  --macho x86-64 e.o 0 4 cffaedfe 4 4 07000001
obj "codegen --macho (arm64): Mach-O arm64"    --macho arm64 f.o  0 4 cffaedfe 4 4 0c000001
"$FID" codegen "$P" --target sparc > /dev/null 2> "$TMP/cg-bad.err"; rc=$?
if [ "$rc" = 1 ] && grep -qF "Unknown target: sparc" "$TMP/cg-bad.err"; then ok "an unknown target is an error"; else bad "fid codegen --target sparc (exit $rc)"; fi

if [ "$fail" = 0 ]; then echo "mi18-pir-retired-check: PASS"; else echo "mi18-pir-retired-check: FAIL"; fi
exit "$fail"
