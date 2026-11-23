#!/usr/bin/env bash
# DB6 (docs/roadmap/llvm-roadmap.md), run after the compiler is built:
#   bash tests/nattools/db6_eh_check.sh [fid]      (default release/fid.exe)
# - tests/nattools/eh_frame_unit.fd prints its expected lines (nat_enc.fd
#   nat_elf_eh_frame / nat_eh_frame / nat_eh_frame_hdr on a stand-in image,
#   and a restore row (n = 0) as DW_CFA_restore), and the executable it
#   writes passes tests/nattools/eh_frame_check.py --readelf;
# - for x86_64-linux, aarch64-linux and riscv64-linux,
#   tests/bootstrap/test_machine_cfi_saves.fd and test_machine_shrinkwrap.fd:
#   * eh_frame_check.py --readelf: PT_GNU_EH_FRAME, the header and its
#     search table, every FDE the same rules as .debug_frame's, every
#     function found by a binary search; FID_NAT_OFF=ehframe: none;
#   * cfi_saves_check.py, elf_segments_check.py (still one R+X and one R+W
#     PT_LOAD), scripts/image-lint.py;
#   * the program prints its expected lines (WSL / qemu-user);
#   * libgcc finds every FDE (tests/nattools/eh_frame_libgcc.c, built with
#     gcc where there is one: Linux or WSL);
#   * shrink wrapping: sw_cfi_check.py against the FID_NAT_OFF=shrinkwrap
#     build (every deferred save has its rule), and FID_NAT_OFF=swcfi
#     leaves deferred registers without one (--expect-missing);
# - scripts/dbg-check.py still passes.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-db6"
mkdir -p "$OUT"
PY=python3
command -v python3 >/dev/null 2>&1 || PY=python
HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

wslpath_of() {
    local p
    p=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
    echo "/mnt/$(echo "$p" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
}

# run_on <img> <target>: the program's output (empty when it cannot run here)
run_on() {
    local img="$1" tgt="$2" q=""
    case "$tgt" in aarch64-linux) q=qemu-aarch64 ;; riscv64-linux) q=qemu-riscv64 ;; esac
    if [ -n "$WSL" ]; then
        MSYS_NO_PATHCONV=1 $WSL sh -c "chmod +x '$(wslpath_of "$img")'; $q '$(wslpath_of "$img")'" 2>&1
    elif [ "$HOST" = linux ]; then
        chmod +x "$img"
        if [ -n "$q" ]; then $q "$img" 2>&1; else "$img" 2>&1; fi
    fi
}

# --- the unit test ---------------------------------------------------------
U=tests/nattools/eh_frame_unit.fd
got=$(FID_EH_UNIT_OUT="$OUT/eh_unit.elf" "$FID" run "$U" 2>&1 | grep -v '^\[')
if [ "$got" = "$(expected "$U")" ]; then ok "DB6 eh_frame_unit prints its expected lines"; else bad "DB6 eh_frame_unit: $(diff <(echo "$got") <(expected "$U") | head -5)"; fi
if [ -f "$OUT/eh_unit.elf" ]; then
    if r=$($PY tests/nattools/eh_frame_check.py --readelf "$OUT/eh_unit.elf"); then ok "DB6 unit image: $(echo "$r" | tail -1)"; else bad "DB6 unit image: $(echo "$r" | head -3)"; fi
fi

# --- libgcc's reader (gcc on Linux, or in WSL) ------------------------------
GCC_BIN=""
if [ -n "$WSL" ]; then
    if MSYS_NO_PATHCONV=1 $WSL sh -c "gcc -O1 -o '$(wslpath_of "$OUT")/eh_frame_libgcc' '$(wslpath_of tests/nattools/eh_frame_libgcc.c)'" >/dev/null 2>&1; then GCC_BIN="$OUT/eh_frame_libgcc"; fi
elif [ "$HOST" = linux ] && command -v gcc >/dev/null 2>&1; then
    if gcc -O1 -o "$OUT/eh_frame_libgcc" tests/nattools/eh_frame_libgcc.c >/dev/null 2>&1; then GCC_BIN="$OUT/eh_frame_libgcc"; fi
fi
[ -z "$GCC_BIN" ] && echo "skip libgcc check: no gcc here"

build() {
    local img="$1" t="$2" tgt="$3"
    shift 3
    FID_VERIFY=1 "$FID" emit "$t" --target "$tgt" "$@" -o "$img" >"$img.log" 2>&1
}

for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    for T in tests/bootstrap/test_machine_cfi_saves.fd tests/bootstrap/test_machine_shrinkwrap.fd; do
        base=$(basename "$T" .fd)
        img="$OUT/$base-$tgt"
        build "$img" "$T" "$tgt" --stats || { bad "DB6 $base $tgt builds: $(tail -2 "$img.log")"; continue; }
        if r=$($PY tests/nattools/eh_frame_check.py --readelf "$img"); then ok "DB6 $base $tgt: $(echo "$r" | tail -1 | sed 's|.*: ||')"; else bad "DB6 $base $tgt .eh_frame: $(echo "$r" | head -3)"; fi
        if r=$($PY tests/nattools/cfi_saves_check.py "$img"); then ok "DB6 $base $tgt: $r"; else bad "DB6 $base $tgt CFI: $(echo "$r" | head -3)"; fi
        if r=$($PY tests/nattools/elf_segments_check.py "$img"); then ok "DB6 $base $tgt segments"; else bad "DB6 $base $tgt segments: $r"; fi
        if [ -f scripts/image-lint.py ]; then
            if r=$($PY scripts/image-lint.py "$img" 2>&1); then ok "DB6 $base $tgt image-lint"; else bad "DB6 $base $tgt image-lint: $(echo "$r" | head -3)"; fi
        fi
        got=$(run_on "$img" "$tgt")
        if [ -n "$WSL" ] || [ "$HOST" = linux ]; then
            if [ "$got" = "$(expected "$T")" ]; then ok "DB6 $base $tgt runs"; else bad "DB6 $base $tgt runs (output differs): $(echo "$got" | head -3)"; fi
        fi
        if [ -n "$GCC_BIN" ]; then
            if [ -n "$WSL" ]; then
                r=$(MSYS_NO_PATHCONV=1 $WSL "$(wslpath_of "$GCC_BIN")" "$(wslpath_of "$img")" 2>&1)
            else
                r=$("$GCC_BIN" "$img" 2>&1)
            fi
            case "$r" in ok:*) ok "DB6 $base $tgt libgcc: $r" ;; *) bad "DB6 $base $tgt libgcc: $r" ;; esac
        fi
        FID_NAT_OFF=ehframe build "$img-noeh" "$T" "$tgt" || bad "DB6 $base $tgt builds with FID_NAT_OFF=ehframe"
        if r=$($PY tests/nattools/eh_frame_check.py --none "$img-noeh"); then ok "DB6 $base $tgt FID_NAT_OFF=ehframe: none"; else bad "DB6 $base $tgt: $r"; fi
        if [ "$base" = test_machine_shrinkwrap ]; then
            FID_NAT_OFF=shrinkwrap build "$img-off" "$T" "$tgt" || { bad "DB6 $tgt builds with FID_NAT_OFF=shrinkwrap"; continue; }
            if r=$($PY tests/nattools/sw_cfi_check.py "$img" "$img-off" "$img.log"); then ok "DB6 $tgt shrink wrapping: $r"; else bad "DB6 $tgt shrink wrapping: $(echo "$r" | head -3)"; fi
            FID_NAT_OFF=swcfi build "$img-nosw" "$T" "$tgt" --stats || { bad "DB6 $tgt builds with FID_NAT_OFF=swcfi"; continue; }
            if r=$($PY tests/nattools/sw_cfi_check.py "$img-nosw" "$img-off" "$img-nosw.log" --expect-missing); then ok "DB6 $tgt FID_NAT_OFF=swcfi: $r"; else bad "DB6 $tgt FID_NAT_OFF=swcfi: $(echo "$r" | head -3)"; fi
        fi
    done
done

# --- the existing debug-information check -----------------------------------
if r=$($PY scripts/dbg-check.py --fid "$FID" 2>&1); then ok "DB6 scripts/dbg-check.py"; else bad "DB6 scripts/dbg-check.py: $(echo "$r" | tail -3)"; fi

echo "$fails failure(s)"
[ "$fails" = 0 ]
