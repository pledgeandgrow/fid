#!/usr/bin/env bash
# DB10 (docs/roadmap/llvm-roadmap.md), run after the compiler is built:
#   bash tests/nattools/db10_check.sh [fid]      (default release/fid.exe)
# - `fid wasm` modules carry a "name" section naming every function
#   (compiler/wasm.fd wb_name_section; tests/wasm/check_names.py, also
#   checks FID_NAT_OFF=debug leaves it out), and the module still runs
#   under Node (scripts/wasm-run.mjs) when node is here;
# - Mach-O images (aarch64-macos, x86_64-macos) have line information: a
#   __DWARF segment with __debug_info / __debug_line (and __debug_frame)
#   from the rows nat_elf_debug uses (compiler/nat_enc.fd
#   nat_macho_debug_cfi, nat_macho_dwarf, nat_macho_dwarf_seg; nat_ir.fd's
#   mac branch): scripts/macho-check.py (structure, code signature over
#   the DWARF), tests/nattools/macho_dwarf_check.py (its own decoder; and
#   llvm-dwarfdump --verify when on PATH), tests/nattools/
#   macho_dwarf_readelf.py (GNU readelf decodes the same sections);
#   FID_NAT_OFF=debug: no __DWARF;
# - the stand-in Mach-O unit tests print their expected lines
#   (tests/nattools/macho_dwarf_unit.fd, macho_dbg_types_unit.fd);
# - scripts/dbg-check.py (symbol tables of every image) still passes.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-db10"
mkdir -p "$OUT"
PY=python3
command -v python3 >/dev/null 2>&1 || PY=python
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

# --- wasm: the name section -------------------------------------------------
for src in tests/bootstrap/hello.fd tests/bootstrap/test_machine_cfi_saves.fd; do
    if r=$($PY tests/wasm/check_names.py --fid "$FID" "$src" 2>&1); then ok "DB10 wasm $src: $r"; else bad "DB10 wasm $src: $r"; fi
done
if command -v node >/dev/null 2>&1; then
    T=tests/bootstrap/test_machine_cfi_saves.fd
    if "$FID" wasm "$T" -o "$OUT/cfi.wasm" >"$OUT/cfi.wasm.log" 2>&1; then
        got=$(node scripts/wasm-run.mjs "$OUT/cfi.wasm" 2>&1)
        if [ "$got" = "$(expected "$T")" ]; then ok "DB10 wasm module with names runs under node"; else bad "DB10 wasm run: $(echo "$got" | head -3)"; fi
    else
        bad "DB10 fid wasm $T: $(tail -2 "$OUT/cfi.wasm.log")"
    fi
fi

# --- Mach-O: line information ----------------------------------------------
for tgt in aarch64-macos x86_64-macos; do
    for T in tests/bootstrap/test_machine_cfi_saves.fd tests/bootstrap/test_nattools_lines_files.fd; do
        base=$(basename "$T" .fd)
        img="$OUT/$base-$tgt"
        if ! "$FID" emit "$T" --target "$tgt" -o "$img" >"$img.log" 2>&1; then bad "DB10 $base $tgt builds: $(tail -2 "$img.log")"; continue; fi
        if r=$($PY scripts/macho-check.py "$img" --quiet 2>&1); then ok "DB10 $base $tgt macho-check"; else bad "DB10 $base $tgt macho-check: $(echo "$r" | head -3)"; fi
        if r=$($PY tests/nattools/macho_dwarf_check.py "$img" --fn main --file "$base.fd" 2>&1); then ok "DB10 $base $tgt: $(echo "$r" | grep 'line table' | sed 's/^ *//')"; else bad "DB10 $base $tgt macho_dwarf_check: $(echo "$r" | grep -i fail | head -3)"; fi
        r=$($PY tests/nattools/macho_dwarf_readelf.py "$img" --fn main --file "$base.fd" 2>&1)
        case $? in
            0) ok "DB10 $base $tgt: $r" ;;
            2) echo "skip DB10 $base $tgt readelf: $r" ;;
            *) bad "DB10 $base $tgt readelf: $(echo "$r" | head -3)" ;;
        esac
        FID_NAT_OFF=debug "$FID" emit "$T" --target "$tgt" -o "$img-nodbg" >"$img-nodbg.log" 2>&1 || bad "DB10 $base $tgt builds with FID_NAT_OFF=debug"
        if $PY - "$img-nodbg" <<'EOF'
import struct, sys
d = open(sys.argv[1], "rb").read()
n = struct.unpack_from("<I", d, 16)[0]
at = 32
for _ in range(n):
    cmd, size = struct.unpack_from("<II", d, at)
    if cmd == 0x19 and d[at + 8:at + 24].rstrip(b"\0") == b"__DWARF":
        sys.exit(1)
    at += size
EOF
        then ok "DB10 $base $tgt FID_NAT_OFF=debug: no __DWARF"; else bad "DB10 $base $tgt FID_NAT_OFF=debug still writes __DWARF"; fi
    done
done

# --- the stand-in Mach-O units ------------------------------------------------
for U in tests/nattools/macho_dwarf_unit.fd tests/nattools/macho_dbg_types_unit.fd; do
    got=$("$FID" run "$U" 2>&1)
    if [ "$got" = "$(expected "$U")" ]; then ok "DB10 $(basename "$U") prints its expected lines"; else bad "DB10 $(basename "$U"): $(diff <(echo "$got") <(expected "$U") | head -5)"; fi
done

# --- symbol tables of every image (DB2 / DB10) --------------------------------
if r=$($PY scripts/dbg-check.py --fid "$FID" 2>&1); then ok "DB10 scripts/dbg-check.py"; else bad "DB10 scripts/dbg-check.py: $(echo "$r" | tail -3)"; fi

echo "$fails failure(s)"
[ "$fails" = 0 ]
