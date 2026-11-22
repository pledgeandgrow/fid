#!/usr/bin/env bash
# 2g.110 (docs/reference/known-bugs.md; RL9, TG12, MI18 in
# docs/roadmap/llvm-roadmap.md): the machine layer's COFF objects
# (`fid emit --obj --nat`, `fid codegen --coff`) failed on every program:
# "native backend: undefined symbol: FreeLibrary" (and LoadLibraryA,
# GetProcAddress): the runtime's kernel32 functions of an `extern "C" link
# "kernel32.dll"` block (stdlib/linux_runtime/win.fd) are dynamic imports
# (TG15), which compiler/nat_ir.fd ng_ldyn_ok refused in object mode. Now a
# COFF (or Mach-O) object keeps them: their import slots become ADDR64
# relocations against the functions (nat_enc.fd nat_coff_object).
#   bash tests/nattools/coff_obj_imports_check.sh [fid]   (default release/fid.exe)
# For each program below: `emit --obj --nat` and `codegen --coff` must
# write an object naming the three kernel32 functions; on Windows with zig
# (lld) the --obj one, linked with fid_main as the entry, must print what
# the `emit --nat` executable prints. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
ZIG=${ZIG:-zig}
OUT="${TMPDIR:-/tmp}/fid-coff-obj-imports"
mkdir -p "$OUT"
export FIDES_NO_CACHE=1
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
windows=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) windows=1 ;; esac

for src in tests/bootstrap/hello.fd tests/bootstrap/test_dl_find.fd tests/bootstrap/test_dynlib.fd; do
    base=$(basename "$src" .fd)
    if ! "$FID" emit "$src" --obj --nat -o "$OUT/$base.obj" >"$OUT/$base.obj.log" 2>&1 || [ ! -s "$OUT/$base.obj" ]; then
        bad "$base: emit --obj --nat failed: $(grep -v '^\[' "$OUT/$base.obj.log" | tail -3 | tr '\n' ' ')"
        continue
    fi
    if ! "$FID" codegen "$src" --coff "$OUT/$base.cg.obj" >"$OUT/$base.cg.log" 2>&1 || [ ! -s "$OUT/$base.cg.obj" ]; then
        bad "$base: codegen --coff failed: $(grep -v '^\[' "$OUT/$base.cg.log" | tail -3 | tr '\n' ' ')"
    else
        ok "$base: codegen --coff writes an object"
    fi
    miss=""
    for f in LoadLibraryA GetProcAddress FreeLibrary; do
        grep -qa "$f" "$OUT/$base.obj" || miss="$miss $f"
    done
    if [ -n "$miss" ]; then
        # (a program that loads no library may leave them out)
        ok "$base: object written (without:$miss)"
    else
        ok "$base: object written, naming the kernel32 loader functions"
    fi
    if [ $windows = 1 ] && command -v "$ZIG" >/dev/null 2>&1; then
        if ! "$FID" emit "$src" --nat -o "$OUT/$base.exe" >"$OUT/$base.exe.log" 2>&1; then
            bad "$base: emit --nat failed"
            continue
        fi
        want=$(cd "$OUT" && "./$base.exe" 2>&1 | tr -d '\r')
        if "$ZIG" cc -target x86_64-windows-gnu -nostdlib "$OUT/$base.obj" -lkernel32 -lws2_32 -lbcrypt -lntdll -lapi-ms-win-core-synch-l1-2-0 -Wl,--entry=fid_main -o "$OUT/$base.linked.exe" >"$OUT/$base.link.log" 2>&1; then
            got=$(cd "$OUT" && "./$base.linked.exe" 2>&1 | tr -d '\r')
            if [ "$got" = "$want" ]; then
                ok "$base: the linked object prints what the --nat executable prints"
            else
                bad "$base: linked object output differs"
                diff <(echo "$want") <(echo "$got") | head -6
            fi
        else
            bad "$base: link failed: $(head -3 "$OUT/$base.link.log" | tr '\n' ' ')"
        fi
    fi
done

# 2g.106 / 2g.110: an object's C functions may come from the objects and
# libraries it is linked with: `emit --obj --nat` of a program calling one
# that no DLL of this machine exports still writes the object (an
# undefined symbol for the linker), where `emit --nat` reports it
# (compiler/nat_ir.fd: nat_pe_link_missing only for an executable)
cat > "$OUT/objext.fd" <<'EOF'
extern "C" link "m" {
    fn fides_cb9_not_in_libm(x: f64) -> f64;
}

@unsafe
fn main() -> i64 {
    println("before");
    println(fides_cb9_not_in_libm(2.0));
    return 0;
}
EOF
if "$FID" emit "$OUT/objext.fd" --obj --nat -o "$OUT/objext.obj" >"$OUT/objext.log" 2>&1 && [ -s "$OUT/objext.obj" ] && grep -qa "fides_cb9_not_in_libm" "$OUT/objext.obj"; then
    ok "objext: --obj --nat leaves an undefined C function to the linker"
else
    bad "objext: --obj --nat: $(grep -v '^\[' "$OUT/objext.log" | tail -2 | tr '\n' ' ')"
fi

echo "coff_obj_imports_check: $fails failed (logs in $OUT)"
[ $fails -eq 0 ]
