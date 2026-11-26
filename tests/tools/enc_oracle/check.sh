#!/usr/bin/env bash
# tools/enc_oracle.fd (roadmap AD4) writes and prints what
# scripts/enc-oracle.py does: both run with the same arguments, each in a
# folder of its own (build/enc-oracle/ is relative to the current
# directory), and stdout, stderr, the exit status and every file under
# build/enc-oracle/ (the generated oracle.fd, the .s files, the objects
# zig made) are compared ("\r\n" from Python on Windows counts as "\n").
#
#   bash tests/tools/enc_oracle/check.sh        # about a minute
#
# Each side runs in a copy of compiler/ (the generated program has `use
# compiler::nat_enc`; an empty Cargo.toml marks the copy as the project
# root and FID_HOME points the compiler at stdlib/), so nothing is written
# into the tree. Cases: the real run (every form agrees: "1200 forms, 0
# mismatches"); a copy of nat_enc.fd with three encoders broken (a64_ret
# a wrong word, rv_ecall two words, x64_ret two bytes: three MISMATCH
# lines, exit 3); a --zig that fails (`false`: "the assembler refused");
# a --fid and a --zig that do not exist (Python's traceback ends in the
# port's one line: only that line is compared); -h; a bad option.
#
# FID: the compiler that builds the tool and runs the generated program
# (default release/fid.exe; a path Python can start too, so on Windows not
# a /c/... one). ZIG: the zig both call (default zig).
# Run from the repository root; needs python3 (or python) and zig.
set -u
FID="${FID:-release/fid.exe}"
ZIG="${ZIG:-zig}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-enc-oracle-check}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
"$PY" -c "import sys" > /dev/null 2>&1 || PY=python
REPO="$(pwd)"
command -v cygpath > /dev/null 2>&1 && REPO="$(cygpath -m "$REPO")"
case "$FID" in /*|?:*) ;; *) FID="$REPO/$FID" ;; esac
exe="$OUT/enc_oracle"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/enc_oracle.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/enc_oracle.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
# the two roots: compiler/ as it is, and with three encoders broken
for r in root broken; do
    rm -rf "$OUT/$r"
    mkdir -p "$OUT/$r"
    cp -r compiler "$OUT/$r/compiler"
    : > "$OUT/$r/Cargo.toml"
done
"$PY" - "$OUT/broken/compiler/nat_enc.fd" <<'PY'
import sys
p = sys.argv[1]
s = open(p, encoding="utf-8").read()
for a, b in [("NatEnc_word(e, 0xD65F03C0);", "NatEnc_word(e, 0xD65F03C1);"),
             ("pub fn rv_ecall(e: &mut NatEnc) {\n    NatEnc_word(e, 0x00000073);",
              "pub fn rv_ecall(e: &mut NatEnc) {\n    NatEnc_word(e, 0x00000073);\n    NatEnc_word(e, 0x00000073);"),
             ("pub fn x64_ret(e: &mut NatEnc) {\n    NatEnc_byte(e, 0xC3);",
              "pub fn x64_ret(e: &mut NatEnc) {\n    NatEnc_byte(e, 0xC3);\n    NatEnc_byte(e, 0xC3);")]:
    assert a in s, a
    s = s.replace(a, b, 1)
open(p, "w", encoding="utf-8", newline="\n").write(s)
PY
export FID_HOME="$REPO"
pass=0
fail=0
# compare <name> <root> <last-line-of-stderr-only: 0|1> <args...>
compare() {
    local name="$1" root="$OUT/$2" lastline="$3"; shift 3
    local d pc fc bad=0
    for d in py fd; do
        rm -rf "$root/build" "$OUT/$name.$d.d"
        if [ $d = py ]; then
            (cd "$root" && "$PY" "$REPO/scripts/enc-oracle.py" "$@") > "$OUT/$name.$d.out" 2> "$OUT/$name.$d.err"; pc=$?
        else
            (cd "$root" && "$exe" "$@") > "$OUT/$name.$d.out" 2> "$OUT/$name.$d.err"; fc=$?
        fi
        [ -d "$root/build" ] && mv "$root/build" "$OUT/$name.$d.d"
        sed -i 's/\r$//' "$OUT/$name.$d.out" "$OUT/$name.$d.err"
        if [ "$lastline" = 1 ]; then tail -1 "$OUT/$name.$d.err" > "$OUT/$name.$d.e1"; mv "$OUT/$name.$d.e1" "$OUT/$name.$d.err"; fi
    done
    diff "$OUT/$name.py.out" "$OUT/$name.fd.out" > "$OUT/$name.diff" || bad=1
    diff "$OUT/$name.py.err" "$OUT/$name.fd.err" >> "$OUT/$name.diff" || bad=1
    [ $pc = $fc ] || bad=1
    if [ -d "$OUT/$name.py.d" ] || [ -d "$OUT/$name.fd.d" ]; then
        diff -r "$OUT/$name.py.d" "$OUT/$name.fd.d" >> "$OUT/$name.diff" 2>&1 || { bad=1; echo "  (build/enc-oracle differs)" >> "$OUT/$name.diff"; }
    fi
    if [ $bad = 0 ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(cat "$OUT/$name.fd.out" "$OUT/$name.fd.err" | wc -l) lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
compare real root 0 --fid "$FID" --zig "$ZIG"
compare broken broken 0 --fid "$FID" --zig "$ZIG"
compare zig-fails root 0 --fid "$FID" --zig false
compare no-zig root 1 --fid "$FID" --zig "$OUT/no-such-zig"
compare no-fid root 1 --fid "$OUT/no-such-fid"
compare help root 0 -h
compare bad-option root 0 --fid
echo "enc_oracle port: $pass same, $fail differ"
[ $fail = 0 ]
