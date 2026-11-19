#!/usr/bin/env bash
# driver-goals-check.sh -- the compiler-driver goals done on Nov 16
# (compiler/main.fd, ir_ser.fd, disasm.fd, gpu.fd):
#   RA14  emit --fast-alloc: the fast allocator with the mid-end on
#   TG19  emit --size: the same output, an image no larger; RISC-V images compressed (RVC) by default
#   MI11  fid dump-mir <file> --target <t> [--fn f]: MIR on stderr, no image
#   TG5   fid disasm <file.fd|exe> [--fn f]: instructions by function
#   RL10  emit --save-ir / fid emit-ir: the machine layer on saved IR;
#         fid ir-fuzz <seed>: random IR, its value known (scripts/fuzz/fuzz_ir.sh)
#   RL20  --crash-report: the stage of a crash; fid reduce
#   MI18  build --native: the machine layer (x86-64 Linux), fid gpu from IR
#   CI7   ccompile --lib (and build --lib): a shared library and its header
#   SL13  fid test -run <test>/<row> (FIDES_ROW) and --row; fid test --fuzz
#         coverage-guided, failing inputs saved under testdata/fuzz/
#         (scripts/tooling/sl13-fuzz-check.sh)
#   N12   fid version: build line (commit and date in a release build)
#   VF12  ccompile: the same executable from two directories (Windows)
#   L7G   fid repl: `:type <expr>` prints the expression's type
#
# Usage:
#   scripts/driver-goals-check.sh
#   FID=<fid>   the compiler (default release/fid.exe, else release/fid)
# Images of Linux targets are run only on a Linux x86-64 host (or with
# qemu-<arch>); elsewhere their builds are checked.
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT="$(pwd)"
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in
  /*|[A-Za-z]:*) ;;
  *) FID="$ROOT/$FID" ;;
esac
DIR="$ROOT/build/driver-goals-check"
mkdir -p "$DIR"
T="$ROOT/tests/bootstrap"
fail=0
ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }
has() { # has <label> <file> <fixed text>
  if grep -qF -- "$3" "$2"; then ok "$1"; else bad "$1: no '$3' in $2"; fi
}
hasnt() {
  if grep -qF -- "$3" "$2"; then bad "$1: '$3' in $2"; else ok "$1"; fi
}
expected() { # the "// Expected output:" block of a test
  awk '/^\/\/ Expected output:/{on=1; next} on && /^\/\/ /{sub(/^\/\/ /, ""); print; next} on{exit}' "$1"
}
linux_x64=0
if [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; then linux_x64=1; fi
run_x64() { # run_x64 <image> <out>: 0 when it ran
  if [ "$linux_x64" = 1 ]; then "$1" > "$2" 2>&1; return 0; fi
  if command -v qemu-x86_64 > /dev/null; then qemu-x86_64 "$1" > "$2" 2>&1; return 0; fi
  return 1
}
same_as_expected() { # same_as_expected <label> <test.fd> <output file>
  if [ "$(tr -d '\r' < "$3")" = "$(expected "$2")" ]; then ok "$1"; else bad "$1 (see $3)"; fi
}

echo "== RA14 --fast-alloc, TG19 --size"
F="$T/test_driver_fast_alloc.fd"
"$FID" emit "$F" --target x86_64-linux -o "$DIR/fa-plain" > "$DIR/fa-plain.log" 2>&1 || bad "emit (see $DIR/fa-plain.log)"
"$FID" emit "$F" --target x86_64-linux --fast-alloc -o "$DIR/fa-fast" > "$DIR/fa-fast.log" 2>&1 || bad "emit --fast-alloc (see $DIR/fa-fast.log)"
"$FID" emit "$F" --target x86_64-linux --size -o "$DIR/fa-size" > "$DIR/fa-size.log" 2>&1 || bad "emit --size (see $DIR/fa-size.log)"
if [ -f "$DIR/fa-plain" ] && [ -f "$DIR/fa-size" ]; then
  p=$(wc -c < "$DIR/fa-plain"); s=$(wc -c < "$DIR/fa-size")
  if [ "$s" -le "$p" ]; then ok "--size image $s bytes <= $p"; else bad "--size image $s bytes > $p"; fi
fi
for v in plain fast size; do
  if [ -f "$DIR/fa-$v" ] && run_x64 "$DIR/fa-$v" "$DIR/fa-$v.out"; then
    same_as_expected "$v output" "$F" "$DIR/fa-$v.out"
  fi
done
# TG19: RVC on riscv64 by default (smaller than FID_NAT_OFF=rvc); --size no larger
"$FID" emit "$F" --target riscv64-linux -o "$DIR/rv-plain" > "$DIR/rv-plain.log" 2>&1 || bad "emit riscv64 (see $DIR/rv-plain.log)"
FID_NAT_OFF=rvc "$FID" emit "$F" --target riscv64-linux -o "$DIR/rv-norvc" > "$DIR/rv-norvc.log" 2>&1 || bad "emit riscv64 FID_NAT_OFF=rvc (see $DIR/rv-norvc.log)"
"$FID" emit "$F" --target riscv64-linux --size -o "$DIR/rv-size" > "$DIR/rv-size.log" 2>&1 || bad "emit riscv64 --size (see $DIR/rv-size.log)"
if [ -f "$DIR/rv-plain" ] && [ -f "$DIR/rv-norvc" ] && [ -f "$DIR/rv-size" ]; then
  n=$(wc -c < "$DIR/rv-norvc"); p=$(wc -c < "$DIR/rv-plain"); s=$(wc -c < "$DIR/rv-size")
  if [ "$p" -lt "$n" ]; then ok "riscv64 image $p bytes < $n without RVC"; else bad "riscv64 image $p bytes, $n without RVC"; fi
  if [ "$s" -le "$p" ]; then ok "riscv64 --size image $s bytes <= $p"; else bad "riscv64 --size image $s bytes > $p"; fi
fi

echo "== MI11 dump-mir"
F="$T/test_driver_disasm.fd"
"$FID" dump-mir "$F" --target x86_64-linux --fn mix > "$DIR/mir.out" 2> "$DIR/mir.err"
has "dump-mir names mix" "$DIR/mir.err" "[mir] mix:"
hasnt "dump-mir prints only --fn's MIR" "$DIR/mir.err" "[mir] main:"
has "dump-mir writes no image" "$DIR/mir.err" "not written"
"$FID" dump-mir "$F" --target aarch64-linux > "$DIR/mir-a64.out" 2> "$DIR/mir-a64.err"
has "dump-mir every function (aarch64)" "$DIR/mir-a64.err" "[mir] main:"

echo "== TG5 disasm"
for t in x86_64-linux aarch64-linux riscv64-linux; do
  "$FID" disasm "$F" --target "$t" --fn mix > "$DIR/dis-$t.out" 2> "$DIR/dis-$t.err"
  has "disasm $t: mix" "$DIR/dis-$t.out" "<mix>:"
  has "disasm $t: ret" "$DIR/dis-$t.out" "ret"
  hasnt "disasm $t: no undecoded words" "$DIR/dis-$t.out" ".word 0x"
done
"$FID" emit "$F" --target x86_64-linux -o "$DIR/dis-img" > /dev/null 2>&1
"$FID" disasm "$DIR/dis-img" --fn main > "$DIR/dis-img.out" 2>&1
has "disasm of an image: main" "$DIR/dis-img.out" "<main>:"
if command -v llvm-objdump > /dev/null; then
  FID="$FID" bash scripts/disasm-check.sh "$F" > "$DIR/disasm-check.out" 2>&1 && ok "disasm-check.sh (llvm-objdump)" || bad "disasm-check.sh (see $DIR/disasm-check.out)"
fi

echo "== RL10 save-ir, emit-ir, ir-fuzz"
F="$T/test_driver_save_ir.fd"
"$FID" emit "$F" --target x86_64-linux --save-ir "$DIR/t.fir" -o "$DIR/ir-direct" > "$DIR/ir-direct.log" 2>&1 || bad "emit --save-ir (see $DIR/ir-direct.log)"
[ -s "$DIR/t.fir" ] && ok "IR saved" || bad "no $DIR/t.fir"
"$FID" emit-ir "$DIR/t.fir" -o "$DIR/ir-replay" > "$DIR/ir-replay.log" 2>&1 || bad "emit-ir (see $DIR/ir-replay.log)"
if [ -f "$DIR/ir-direct" ] && [ -f "$DIR/ir-replay" ]; then
  # (the images need not be byte-identical: emit also writes the line
  # table of the source, which a saved module does not carry)
  if run_x64 "$DIR/ir-direct" "$DIR/ir-direct.out"; then same_as_expected "emit --save-ir output" "$F" "$DIR/ir-direct.out"; fi
  if run_x64 "$DIR/ir-replay" "$DIR/ir-replay.out"; then same_as_expected "emit-ir output" "$F" "$DIR/ir-replay.out"; fi
fi
printf 'not an IR module' > "$DIR/bad.fir"
"$FID" emit-ir "$DIR/bad.fir" -o "$DIR/bad" > "$DIR/bad.log" 2>&1 && bad "emit-ir took a bad file" || has "emit-ir rejects a bad file" "$DIR/bad.log" "is not a saved IR module"
"$FID" ir-fuzz 7 --expect > "$DIR/fz7a.out" 2>&1
"$FID" ir-fuzz 7 --expect > "$DIR/fz7b.out" 2>&1
if cmp -s "$DIR/fz7a.out" "$DIR/fz7b.out" && grep -q '^expected: ' "$DIR/fz7a.out"; then ok "ir-fuzz: one value per seed"; else bad "ir-fuzz --expect (see $DIR/fz7a.out)"; fi
FID="$FID" bash scripts/fuzz/fuzz_ir.sh 20 1 > "$DIR/fuzz_ir.out" 2>&1 && ok "fuzz_ir.sh: 20 seeds" || bad "fuzz_ir.sh (see $DIR/fuzz_ir.out)"

echo "== RL20 crash report, reduce"
FID_CRASH_TEST=lowering "$FID" emit "$T/test_driver_native.fd" --target x86_64-linux -o "$DIR/cr" --crash-report > "$DIR/cr.out" 2> "$DIR/cr.err"
has "the report names the stage" "$DIR/cr.err" "stage:    lowering"
has "the report has the panic" "$DIR/cr.err" "FID_CRASH_TEST"
has "the report says how to reduce" "$DIR/cr.err" "fid reduce"
"$FID" check "$T/test_driver_native.fd" --crash-report > "$DIR/cr-ok.out" 2> "$DIR/cr-ok.err"
hasnt "no report without a crash" "$DIR/cr-ok.err" "crash report"
cat > "$DIR/pred.sh" << EOF
#!/usr/bin/env bash
# fails (status 3) while the program still prints KEEP 42
if "$FID" run "\$1" 2>/dev/null | grep -q "KEEP 42"; then exit 3; fi
exit 0
EOF
cp "$T/test_driver_reduce.fd" "$DIR/reduce_in.fd"
"$FID" reduce "$DIR/reduce_in.fd" -o "$DIR/reduce_out.fd" --cmd "bash $DIR/pred.sh {}" > "$DIR/reduce.log" 2>&1 || bad "fid reduce (see $DIR/reduce.log)"
if [ -f "$DIR/reduce_out.fd" ]; then
  n_in=$(grep -cv '^\s*$\|^\s*//' "$DIR/reduce_in.fd"); n_out=$(grep -cv '^\s*$\|^\s*//' "$DIR/reduce_out.fd")
  if [ "$n_out" -lt "$n_in" ] && [ "$n_out" -le 6 ]; then ok "reduced $n_in -> $n_out lines"; else bad "reduce: $n_in -> $n_out lines (see $DIR/reduce_out.fd)"; fi
  has "the reduced program keeps the line" "$DIR/reduce_out.fd" "KEEP 42"
fi

echo "== MI18 build --native, fid gpu"
F="$T/test_driver_native.fd"
"$FID" build "$F" --native -o "$DIR/native" > "$DIR/native.log" 2>&1 || bad "build --native (see $DIR/native.log)"
hasnt "build --native is not the PIR path" "$DIR/native.log" "Using native backend"
if [ -f "$DIR/native" ]; then
  magic="$(od -An -tx1 -N4 "$DIR/native" | tr -d ' \n')"
  mach="$(od -An -tu2 -j18 -N2 "$DIR/native" | tr -d ' \n')"
  if [ "$magic" = 7f454c46 ] && [ "$mach" = 62 ]; then ok "an x86-64 ELF"; else bad "not an x86-64 ELF ($magic, machine $mach)"; fi
  if run_x64 "$DIR/native" "$DIR/native.out"; then same_as_expected "build --native output" "$F" "$DIR/native.out"; fi
fi
F="$T/test_driver_gpu.fd"
"$FID" gpu "$F" --target ptx > "$DIR/gpu.ptx" 2> "$DIR/gpu.err"
has "ptx: the kernel" "$DIR/gpu.ptx" ".visible .entry k_sum("
has "ptx: the loop's add" "$DIR/gpu.ptx" "add.s64"
hasnt "ptx: the slot loads are translated" "$DIR/gpu.ptx" "not translated: load"
"$FID" gpu "$F" --target spirv -o "$DIR/gpu.spv" > /dev/null 2> "$DIR/gpu-spv.err"
if [ -f "$DIR/gpu.spv" ] && command -v python3 > /dev/null; then
  python3 scripts/spirv-check.py "$DIR/gpu.spv" > "$DIR/spirv-check.out" 2>&1 && ok "spirv-check.py" || bad "spirv-check.py (see $DIR/spirv-check.out)"
fi

echo "== CI7 ccompile --lib (tests/projects/embed)"
if [ -n "${CC:-}" ] || command -v cc > /dev/null || command -v gcc > /dev/null; then
  rm -rf "$DIR/embed"; cp -r "$ROOT/tests/projects/embed" "$DIR/embed"
  if FID="$FID" bash "$DIR/embed/build.sh" > "$DIR/embed.log" 2>&1; then ok "a library, its header, a C host calling it"; else bad "embed (see $DIR/embed.log)"; fi
else
  echo "  skip  no C compiler"
fi

echo "== SL13 fid test -run <test>/<row>"
S="$ROOT/scripts/tooling/sl13_rows_tests.fd"
"$FID" test "$S" -run test_parse/neg > "$DIR/rows1.out" 2>&1
has "-run test/row: the row" "$DIR/rows1.out" "table parse: 1 passed, 2 skipped"
hasnt "-run test/row: the other test does not run" "$DIR/rows1.out" "other ran"
"$FID" test "$S" --row big > "$DIR/rows2.out" 2>&1
has "--row: one row" "$DIR/rows2.out" "table parse: 1 passed, 2 skipped"
has "--row: every test" "$DIR/rows2.out" "other ran"
"$FID" test "$S" -run test_parse > "$DIR/rows3.out" 2>&1
has "-run test: every row" "$DIR/rows3.out" "table parse: 3 passed"

echo "== SL13 fid test --fuzz: coverage-guided, failures saved and replayed"
FID="$FID" bash scripts/tooling/sl13-fuzz-check.sh > "$DIR/sl13-fuzz.out" 2>&1 && ok "sl13-fuzz-check.sh" || bad "sl13-fuzz-check.sh (see $DIR/sl13-fuzz.out)"

echo "== N12 fid version"
"$FID" version > "$DIR/version.out" 2>&1
has "version: the build" "$DIR/version.out" "build: "
"$FID" -V > "$DIR/version2.out" 2>&1
has "-V" "$DIR/version2.out" "build: "

echo "== VF12 ccompile from two directories"
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*)
    if [ -n "${CC:-}" ] || command -v zig > /dev/null; then
      mkdir -p "$DIR/r1" "$DIR/r2"
      cp "$T/hello.fd" "$DIR/r1/hello.fd"; cp "$T/hello.fd" "$DIR/r2/hello.fd"
      (cd "$DIR/r1" && CC="${CC:-zig cc}" FIDES_NO_CACHE=1 "$FID" ccompile hello.fd -o hello.exe > build.log 2>&1)
      (cd "$DIR/r2" && CC="${CC:-zig cc}" FIDES_NO_CACHE=1 "$FID" ccompile hello.fd -o hello.exe > build.log 2>&1)
      if cmp -s "$DIR/r1/hello.exe" "$DIR/r2/hello.exe"; then ok "the same bytes"; else bad "the executables differ ($(cmp -l "$DIR/r1/hello.exe" "$DIR/r2/hello.exe" 2>/dev/null | wc -l) bytes)"; fi
    else
      echo "  skip  no C compiler"
    fi ;;
  *) echo "  skip  (Windows only: scripts/verify/repro.sh covers Linux)" ;;
esac

echo "== L7G fid repl :type"
printf ':type 1 + 2\n:type "hi"\n:quit\n' | "$FID" repl > "$DIR/l7g.out" 2> "$DIR/l7g.err"
has "int" "$DIR/l7g.out" "1 + 2 : i64"
has "string" "$DIR/l7g.out" '"hi" : str'

if [ "$fail" -eq 0 ]; then echo "driver goals: all checks passed"; else echo "driver goals: some checks FAILED"; fi
exit "$fail"
