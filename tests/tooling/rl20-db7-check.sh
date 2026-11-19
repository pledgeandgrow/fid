#!/usr/bin/env bash
# rl20-db7-check.sh -- what is left of two driver goals of
# docs/roadmap/llvm-roadmap.md, done on Nov 17 (compiler/main.fd):
#   RL20  --crash-report names the mid-end pass and the function it ran on
#         ("[iropt] <pass> <fn>" lines of iropt.fd/iropt_me.fd under
#         FID_CRASH_TRACE; FID_CRASH_TEST=iropt:<pass>[:<fn>] crashes there),
#         and `fid reduce --timeout <s>` stops a try that runs too long
#         (reduce_system: a watchdog that kills the process tree; Windows:
#         PowerShell WaitForExit and taskkill /T).
#   DB7   `fid explain <file> --stages <fn>` prints the machine code as
#         instructions (disasm.fd disasm_bytes), found through the ELF
#         .symtab, the PE COFF symbols (--nat) and the Mach-O LC_SYMTAB;
#         `fid disasm <Mach-O> --fn <fn>` reads the same symbols.
#
# Usage (from anywhere):
#   bash tests/tooling/rl20-db7-check.sh
#   FID=<fid>   the compiler (default release/fid.exe, else release/fid)
# Exit 0 when every check passes. Needs bash and grep; the reducer's
# predicate runs `fid run`.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
# relative paths: Git Bash would rewrite a /tmp path given to a Windows
# program
DIR=build/rl20-db7-check
rm -rf "$DIR"
mkdir -p "$DIR"
T=tests/bootstrap
SRC=$T/test_tooling_explain.fd
fail=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }
has() { # has <label> <file> <fixed text>
  if grep -qF -- "$3" "$2"; then ok "$1"; else bad "$1: no '$3' in $2"; fi
}
hasnt() {
  if grep -qF -- "$3" "$2"; then bad "$1: '$3' in $2"; else ok "$1"; fi
}

echo "== DB7 explain --stages: the machine code as instructions"
for t in x86_64-linux aarch64-linux riscv64-linux x86_64-macos aarch64-macos; do
  "$FID" explain "$SRC" --stages sum_to --target "$t" > "$DIR/ex-$t.txt" 2> "$DIR/ex-$t.err"
  has "$t: the machine code heading" "$DIR/ex-$t.txt" "=== sum_to: machine code ==="
  has "$t: the function found by its symbol" "$DIR/ex-$t.txt" "<sum_to>:"
  hasnt "$t: not only ELF" "$DIR/ex-$t.txt" "only ELF executables are read here"
  hasnt "$t: not a missing symbol" "$DIR/ex-$t.txt" "(no symbol sum_to"
  # a disassembled line: address, bytes, then an instruction
  if grep -qE '^  [0-9a-f]{8,}:  ([0-9a-f]{2} )+ *[a-z]' "$DIR/ex-$t.txt"; then ok "$t: instructions, not hex rows"; else bad "$t: no instruction lines in $DIR/ex-$t.txt"; fi
done
has "x86-64 Linux: the prologue" "$DIR/ex-x86_64-linux.txt" "push rbp"
has "x86-64 Linux: the return" "$DIR/ex-x86_64-linux.txt" " ret"
has "x86-64 macOS: the prologue" "$DIR/ex-x86_64-macos.txt" "push rbp"
has "AArch64 Linux: the return" "$DIR/ex-aarch64-linux.txt" " ret"
has "AArch64 macOS: the return" "$DIR/ex-aarch64-macos.txt" " ret"
"$FID" explain "$SRC" --stages sum_to --nat --target x86_64-windows > "$DIR/ex-win.txt" 2> "$DIR/ex-win.err"
has "PE (--nat): the function found by its COFF symbol" "$DIR/ex-win.txt" "<sum_to>:"
has "PE (--nat): the prologue" "$DIR/ex-win.txt" "push rbp"
hasnt "PE (--nat): not only ELF" "$DIR/ex-win.txt" "only ELF executables are read here"
"$FID" explain "$SRC" --stages no_such_function_xyz > "$DIR/ex-none.txt" 2> "$DIR/ex-none.err"
has "a missing function is said" "$DIR/ex-none.txt" "(no symbol no_such_function_xyz"
# fid disasm of a Mach-O image the machine layer wrote
if "$FID" emit "$SRC" --target x86_64-macos -o "$DIR/m-x64" > "$DIR/m-x64.log" 2>&1; then
  "$FID" disasm "$DIR/m-x64" --fn sum_to > "$DIR/dis-m-x64.txt" 2>&1
  has "disasm of a Mach-O (x86-64)" "$DIR/dis-m-x64.txt" "<sum_to>:"
  hasnt "disasm reads Mach-O" "$DIR/dis-m-x64.txt" "a Mach-O: use a Linux --target"
else
  bad "emit --target x86_64-macos (see $DIR/m-x64.log)"
fi
if "$FID" emit "$SRC" --target aarch64-macos -o "$DIR/m-a64" > "$DIR/m-a64.log" 2>&1; then
  "$FID" disasm "$DIR/m-a64" > "$DIR/dis-m-a64.txt" 2>&1
  has "disasm of a Mach-O (AArch64), every function" "$DIR/dis-m-a64.txt" "<main>:"
  has "disasm of a Mach-O (AArch64): sum_to too" "$DIR/dis-m-a64.txt" "<sum_to>:"
else
  bad "emit --target aarch64-macos (see $DIR/m-a64.log)"
fi

echo "== RL20 the mid-end pass and function of a crash"
FID_CRASH_TRACE=1 "$FID" emit "$SRC" --target x86_64-linux -o "$DIR/tr" > "$DIR/tr.out" 2> "$DIR/tr.err"
has "the trace names a pass and main" "$DIR/tr.err" "[iropt] fold main"
has "the trace names a pass and sum_to" "$DIR/tr.err" " sum_to"
FID_CRASH_TEST=iropt:dce:main "$FID" emit "$SRC" --target x86_64-linux -o "$DIR/cr" --crash-report > "$DIR/cr.out" 2> "$DIR/cr.err"
has "the report names the stage" "$DIR/cr.err" "stage:    iropt"
has "the report names the pass" "$DIR/cr.err" "pass:     dce (the mid-end)"
has "the report names the function" "$DIR/cr.err" "function: main (the pass was running on it)"
has "the report has the panic" "$DIR/cr.err" "FID_CRASH_TEST: a deliberate crash in iropt pass dce on main"
has "the report says how to reduce" "$DIR/cr.err" "fid reduce"
hasnt "the trace lines are not echoed" "$DIR/cr.err" "[iropt] "
FID_CRASH_TEST=iropt:fold "$FID" emit "$SRC" --target x86_64-linux -o "$DIR/cr2" --crash-report > "$DIR/cr2.out" 2> "$DIR/cr2.err"
has "any function: the pass" "$DIR/cr2.err" "pass:     fold (the mid-end)"
has "any function: a function" "$DIR/cr2.err" "function: "
# the machine layer's own function names still work
FID_CRASH_TEST=native "$FID" emit "$SRC" --target x86_64-linux -o "$DIR/cr3" --crash-report > "$DIR/cr3.out" 2> "$DIR/cr3.err"
has "a crash in the machine layer: its stage" "$DIR/cr3.err" "stage:    native code"
hasnt "a crash in the machine layer: no mid-end pass" "$DIR/cr3.err" "pass:     "

echo "== RL20 fid reduce --timeout: a try that loops forever is stopped"
cat > "$DIR/pred.sh" << EOF
#!/usr/bin/env bash
# fails (status 3) while the program still prints KEEP 42 and then 2
out=\$("$FID" run "\$1" 2>/dev/null | tr -d '\r')
if printf '%s\n' "\$out" | grep -q "^KEEP 42\$" && printf '%s\n' "\$out" | grep -qx "2"; then exit 3; fi
exit 0
EOF
cp "$T/test_driver_reduce_hang.fd" "$DIR/hang_in.fd"
t0=$(date +%s)
"$FID" reduce "$DIR/hang_in.fd" -o "$DIR/hang_out.fd" --cmd "bash $DIR/pred.sh {}" --timeout 5 > "$DIR/hang.log" 2>&1
rc=$?
t1=$(date +%s)
if [ $rc -eq 0 ]; then ok "fid reduce ended (status 0)"; else bad "fid reduce: status $rc (see $DIR/hang.log)"; fi
if [ $((t1 - t0)) -lt 600 ]; then ok "in $((t1 - t0)) s"; else bad "took $((t1 - t0)) s"; fi
has "the timeout is said" "$DIR/hang.log" "each try is stopped after 5 s"
has "a try was stopped" "$DIR/hang.log" "were stopped"
if [ -f "$DIR/hang_out.fd" ]; then
  has "the reduced program keeps KEEP 42" "$DIR/hang_out.fd" "KEEP 42"
  has "the reduced program keeps the loop's step" "$DIR/hang_out.fd" "t = t - 10;"
  hasnt "the helper went" "$DIR/hang_out.fd" "fn helper_a"
else
  bad "no reduced program (see $DIR/hang.log)"
fi
# no leftovers of the watchdog
for f in _fid_reduce.timeout _fid_reduce_try.sh _fid_reduce_cmd.sh _fid_reduce_try.ps1 _fid_reduce_cmd.bat _fid_reduce.out; do
  if [ -e "$f" ]; then bad "left behind: $f"; fi
done
if [ -e "$DIR/_fid_reduce_cand.fd" ]; then bad "left behind: $DIR/_fid_reduce_cand.fd"; else ok "the candidate file is removed"; fi

if [ $fail -eq 0 ]; then echo "rl20-db7-check: all passed"; else echo "rl20-db7-check: FAILED"; fi
exit $fail
