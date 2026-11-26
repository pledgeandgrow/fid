#!/usr/bin/env bash
# tools/fuzz_nightly.fd (roadmap AD4) does what scripts/fuzz/nightly.sh
# does: both run with the same environment, one after the other, into the
# same OUT folder, and their output, exit status and kept files are
# compared ("\r\n" from Python on Windows counts as "\n"; the compiler's
# timings are blurred). nightly.sh runs the Python tools
# (scripts/fuzz/gen_programs.py, triage.py), the port their Fides ports
# (tools/fuzz_gen.fd, tools/fuzz_triage.fd), built here first.
#
#   bash tests/tools/fuzz_nightly/check.sh
#
# FUZZ_BACKENDS=run,check: the `check` backend builds no executable, so
# every program is kept and triage has something to print. The sanitizer
# part runs only on Linux and macOS with clang. FID: the compiler both
# drive and that builds the tools (default release/fid.exe; on Windows a
# path Python can start, not /c/...). Python runs in UTF-8 mode
# (PYTHONUTF8=1), as on Linux. Run from the repository root.
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-fuzz-nightly-check}"
mkdir -p "$OUT"
ext=""
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) ext=".exe" ;; esac
for t in fuzz_nightly fuzz_gen fuzz_triage; do
    if ! "$FID" ccompile "tools/$t.fd" -o "$OUT/$t$ext" > "$OUT/build-$t.txt" 2>&1; then
        echo "FAIL: tools/$t.fd does not build"; tail -5 "$OUT/build-$t.txt"; exit 1
    fi
done
export FUZZ_GEN="$OUT/fuzz_gen$ext" FUZZ_TRIAGE="$OUT/fuzz_triage$ext"
pass=0
fail=0
norm() {   # dir -> dir.n
    rm -rf "$1.n"; mkdir -p "$1.n"
    (cd "$1" && find . -type f) | while read -r f; do
        mkdir -p "$(dirname "$1.n/$f")"
        sed -E 's/[0-9]+ ?ms/N ms/g' "$1/$f" > "$1.n/$f"
    done
}
compare() {   # name, then VAR=value settings both get
    local name="$1"; shift
    local k="$OUT/keep-$name"
    rm -rf "$k" "$k.sh" "$k.fd" "$k.sh.n" "$k.fd.n"
    env "$@" FID="$FID" OUT="$k" PYTHONUTF8=1 timeout 3600 bash scripts/fuzz/nightly.sh > "$OUT/$name.sh" 2>&1
    local pc=$?
    mv "$k" "$k.sh"
    env "$@" FID="$FID" OUT="$k" timeout 3600 "$OUT/fuzz_nightly$ext" > "$OUT/$name.fd" 2>&1
    local fc=$?
    mv "$k" "$k.fd"
    norm "$k.sh"; norm "$k.fd"
    sed -i -E 's/[0-9]+ ?ms/N ms/g' "$OUT/$name.sh" "$OUT/$name.fd"
    if diff --strip-trailing-cr "$OUT/$name.sh" "$OUT/$name.fd" > "$OUT/$name.diff" \
        && diff -r "$k.sh.n" "$k.fd.n" >> "$OUT/$name.diff" && [ $pc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$OUT/$name.fd") lines, $(find "$k.fd" -type f | wc -l) kept files)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
        # which step: the generator's lines (and the kept files) alone
        local gen_re='^(seed |batch |[0-9]+ programs, |sanitizer finding: )'
        if diff --strip-trailing-cr <(grep -aE "$gen_re" "$OUT/$name.sh") <(grep -aE "$gen_re" "$OUT/$name.fd") > /dev/null \
            && diff -r "$k.sh.n" "$k.fd.n" > /dev/null; then
            echo "     (the runs and kept files are the same: only the triage step's lines differ)"
        fi
    fi
}
compare kept N=2 SEED=7 FUZZ_BACKENDS=run,check
compare agree N=2 SEED=40 FUZZ_BACKENDS=run BATCH=2
echo "fuzz_nightly port: $pass same, $fail differ"
[ $fail = 0 ]
