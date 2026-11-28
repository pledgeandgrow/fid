#!/usr/bin/env bash
# Target roadmap XT44, XT50, XT54, XT64 (docs/roadmap/target-roadmap.md):
# the ELF branding of the machine layer's FreeBSD, OpenBSD and Android
# images (compiler/nat_enc.fd g_nat_elf_os, nat_elf_img) and the routing of
# those targets to the machine layer (compiler/main.fd nat_elf_target_os).
#   bash tests/nattools/xt_elf_notes_check.sh [fid]   (default release/fid.exe)
# Structure only: nothing runs (no BSD, no Android here). Per image:
# - x86_64-freebsd / aarch64-freebsd (FID_IMAGE_ONLY=1: there is no FreeBSD
#   runtime yet, XT43): EI_OSABI 9 (FreeBSD), a PT_NOTE inside the first
#   PT_LOAD holding the note "FreeBSD", type 1 (NT_FREEBSD_ABI_TAG), desc
#   1400097 (__FreeBSD_version), a .note.tag SHT_NOTE section at it.
# - x86_64-openbsd / aarch64-openbsd (FID_IMAGE_ONLY=1: OpenBSD needs libc
#   system calls, XT53): EI_OSABI 0, the note "OpenBSD", type 1, desc 0
#   (.note.openbsd.ident), PT_OPENBSD_NOBTCFI, no PT_OPENBSD_WXNEEDED and
#   no PT_LOAD both writable and executable (W^X holds); with
#   FID_NAT_OFF=wxsplit (one RWX segment) PT_OPENBSD_WXNEEDED appears.
# - aarch64-android (API 34 by default), aarch64-android29, the NDK triple
#   aarch64-linux-android31, android-arm64: ET_DYN (a static PIE), no
#   PT_INTERP, the note "Android", type 1, desc = the API level
#   (.note.android.ident).
# - x86_64-linux (control): EI_OSABI 0, no PT_NOTE.
# - every image passes scripts/image-lint.py; llvm-readelf -h -l -n names
#   the OS/ABI and the notes.
# - refusals: x86_64-freebsd without FID_IMAGE_ONLY names XT43,
#   x86_64-openbsd names XT53, aarch64-android --no-pie, aarch64-android5
#   and riscv64-freebsd are errors; none writes a Windows PE (before XT50
#   these targets fell through to pe_ir).
# READELF= names llvm-readelf (default: on PATH, else the LLVM 21 install
# under %LOCALAPPDATA%\Programs). Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-xt-elf-notes"
mkdir -p "$OUT"
cd "$(dirname "$0")/../.." || exit 1
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
READELF="${READELF:-}"
if [ -z "$READELF" ]; then
    if command -v llvm-readelf > /dev/null 2>&1; then
        READELF=llvm-readelf
    elif [ -x "/c/Users/$USERNAME/AppData/Local/Programs/llvm-21.1.0/bin/llvm-readelf.exe" ]; then
        READELF="/c/Users/$USERNAME/AppData/Local/Programs/llvm-21.1.0/bin/llvm-readelf.exe"
    elif [ -n "${LOCALAPPDATA:-}" ] && [ -x "$(cygpath -u "$LOCALAPPDATA" 2>/dev/null)/Programs/llvm-21.1.0/bin/llvm-readelf.exe" ]; then
        READELF="$(cygpath -u "$LOCALAPPDATA")/Programs/llvm-21.1.0/bin/llvm-readelf.exe"
    fi
fi
[ -n "$READELF" ] || echo "note: no llvm-readelf: the readelf rows are skipped (the byte checks still run)"

HELLO=tests/bootstrap/hello.fd

# elf_facts <file>: one line per fact, from the bytes (no readelf needed)
elf_facts() {
    "$PY" - "$1" <<'PYEOF'
import struct, sys
d = open(sys.argv[1], "rb").read()
if d[:4] != b"\x7fELF":
    print("magic", d[:2].hex()); sys.exit(0)
print("osabi", d[7])
e_type, = struct.unpack_from("<H", d, 16)
print("etype", e_type)
phoff, = struct.unpack_from("<Q", d, 32)
shoff, = struct.unpack_from("<Q", d, 40)
phnum, = struct.unpack_from("<H", d, 56)
shnum, shstrndx = struct.unpack_from("<HH", d, 60)
loads = []
for i in range(phnum):
    t, f, off, va, pa, fs, ms, al = struct.unpack_from("<IIQQQQQQ", d, phoff + 56 * i)
    print("phdr %#x" % t)
    if t == 1:
        loads.append((off, fs, f))
        if (f & 2) and (f & 1):
            print("wx_load")
    if t == 4:
        inload = any(lo <= off and off + fs <= lo + lfs for lo, lfs, _ in loads)
        print("note_in_load", int(inload))
        p = off
        while p + 12 <= off + fs:
            nsz, dsz, nt = struct.unpack_from("<III", d, p)
            name = d[p + 12:p + 12 + nsz].rstrip(b"\0").decode("latin-1")
            q = p + 12 + ((nsz + 3) & ~3)
            desc = struct.unpack_from("<I", d, q)[0] if dsz >= 4 else -1
            print("note %s %d %d %d" % (name, nt, dsz, desc))
            p = q + ((dsz + 3) & ~3)
if shnum and shoff:
    s_off = struct.unpack_from("<Q", d, shoff + 64 * shstrndx + 24)[0]
    for i in range(shnum):
        nm, typ = struct.unpack_from("<II", d, shoff + 64 * i)
        e = d.index(b"\0", s_off + nm)
        if typ == 7:
            print("notesec %s" % d[s_off + nm:e].decode("latin-1"))
PYEOF
}

has() { grep -qx -- "$2" "$1"; }

# build <name> <target> [env...]: the image of hello.fd, its facts in
# $OUT/<name>.facts, its readelf in $OUT/<name>.readelf
build() {
    local name="$1" tgt="$2"
    shift 2
    rm -f "$OUT/$name" "$OUT/$name.exe"
    if ! env "$@" "$FID" emit "$HELLO" --target "$tgt" -o "$OUT/$name" > "$OUT/$name.log" 2>&1; then
        bad "$name: emit --target $tgt failed: $(tail -n 3 "$OUT/$name.log" | tr '\n' ' ')"
        return 1
    fi
    if [ -f "$OUT/$name.exe" ] || [ ! -f "$OUT/$name" ]; then
        bad "$name: no ELF at $OUT/$name (a PE?)"
        return 1
    fi
    elf_facts "$OUT/$name" > "$OUT/$name.facts"
    if "$PY" scripts/image-lint.py "$OUT/$name" > "$OUT/$name.lint" 2>&1; then
        ok "$name: image-lint clean"
    else
        bad "$name: image-lint: $(head -n 5 "$OUT/$name.lint" | tr '\n' ' ')"
    fi
    if [ -n "$READELF" ]; then
        "$READELF" -h -l -n "$OUT/$name" > "$OUT/$name.readelf" 2>&1 || bad "$name: llvm-readelf failed"
    fi
    return 0
}

# expect <name> <fact> <what>
expect() {
    if has "$OUT/$1.facts" "$2"; then ok "$1: $3"; else bad "$1: $3 (no '$2' in $OUT/$1.facts)"; fi
}
expect_not() {
    if has "$OUT/$1.facts" "$2"; then bad "$1: $3 ('$2' in $OUT/$1.facts)"; else ok "$1: $3"; fi
}
expect_readelf() {
    [ -n "$READELF" ] || return 0
    if grep -q -- "$2" "$OUT/$1.readelf"; then ok "$1: readelf: $2"; else bad "$1: readelf has no '$2' ($OUT/$1.readelf)"; fi
}

# --- XT44/XT50: FreeBSD --------------------------------------------------
for a in x86_64 aarch64; do
    n="$a-freebsd"
    if build "$n" "$n" FID_IMAGE_ONLY=1; then
        expect "$n" "osabi 9" "EI_OSABI ELFOSABI_FREEBSD"
        expect "$n" "phdr 0x4" "PT_NOTE"
        expect "$n" "note_in_load 1" "the note is inside a PT_LOAD"
        expect "$n" "note FreeBSD 1 4 1400097" "note FreeBSD, NT_FREEBSD_ABI_TAG, __FreeBSD_version 1400097"
        expect "$n" "notesec .note.tag" ".note.tag section"
        expect_not "$n" "phdr 0x3" "no PT_INTERP (static)"
        expect_not "$n" "wx_load" "no W+X PT_LOAD"
        expect_readelf "$n" "OS/ABI:.*FreeBSD"
        expect_readelf "$n" "FreeBSD"
    fi
done

# --- XT54: OpenBSD -------------------------------------------------------
for a in x86_64 aarch64; do
    n="$a-openbsd"
    if build "$n" "$n" FID_IMAGE_ONLY=1; then
        expect "$n" "osabi 0" "EI_OSABI System V (as OpenBSD's linker)"
        expect "$n" "note_in_load 1" "the note is inside a PT_LOAD"
        expect "$n" "note OpenBSD 1 4 0" "note OpenBSD, type 1, desc 0"
        expect "$n" "notesec .note.openbsd.ident" ".note.openbsd.ident section"
        expect "$n" "phdr 0x65a3dbe8" "PT_OPENBSD_NOBTCFI"
        expect "$n" "phdr 0x6" "PT_PHDR (OpenBSD refuses an ET_DYN without one: EINVAL)"
        expect_not "$n" "phdr 0x65a3dbe7" "no PT_OPENBSD_WXNEEDED (W^X holds)"
        expect_not "$n" "phdr 0x65a3dbe6" "no PT_OPENBSD_RANDOMIZE (no random data section)"
        expect_not "$n" "wx_load" "no W+X PT_LOAD"
        expect_readelf "$n" "OpenBSD"
        expect_readelf "$n" "OPENBSD_NOBTCFI"
    fi
done
n=x86_64-openbsd-rwx
if build "$n" x86_64-openbsd FID_IMAGE_ONLY=1 FID_NAT_OFF=wxsplit; then
    expect "$n" "wx_load" "one RWX PT_LOAD (FID_NAT_OFF=wxsplit)"
    expect "$n" "phdr 0x65a3dbe7" "PT_OPENBSD_WXNEEDED with the RWX segment"
    expect "$n" "phdr 0x6" "PT_PHDR"
fi

# --- XT64: Android -------------------------------------------------------
for spec in "aarch64-android:aarch64-android:34" "aarch64-android29:aarch64-android29:29" "ndk31:aarch64-linux-android31:31" "android-arm64:android-arm64:34"; do
    n="${spec%%:*}"; rest="${spec#*:}"; tgt="${rest%%:*}"; api="${rest#*:}"
    if build "$n" "$tgt"; then
        expect "$n" "etype 3" "ET_DYN (PIE)"
        expect_not "$n" "phdr 0x3" "no PT_INTERP (static PIE)"
        expect "$n" "osabi 0" "EI_OSABI System V"
        expect "$n" "note_in_load 1" "the note is inside a PT_LOAD"
        expect "$n" "note Android 1 4 $api" "note Android, type 1, API level $api"
        expect "$n" "notesec .note.android.ident" ".note.android.ident section"
        expect_not "$n" "wx_load" "no W+X PT_LOAD"
        expect_readelf "$n" "Android"
    fi
done

# --- control: Linux is unchanged ----------------------------------------
n=x86_64-linux
if build "$n" "$n"; then
    expect "$n" "osabi 0" "EI_OSABI System V"
    expect_not "$n" "phdr 0x4" "no PT_NOTE"
    expect_not "$n" "phdr 0x6" "no PT_PHDR (static, unchanged)"
fi

# --- refusals ------------------------------------------------------------
# refuse <name> <target> <text the error names> <VAR=value> [fid flags...]
refuse() {
    local name="$1" tgt="$2" want="$3" envset="$4"
    shift 4
    rm -f "$OUT/r_$name" "$OUT/r_$name.exe"
    if env "$envset" "$FID" emit "$HELLO" --target "$tgt" -o "$OUT/r_$name" "$@" > "$OUT/r_$name.log" 2>&1; then
        bad "refuse $name: --target $tgt built"
    elif ! grep -q -- "$want" "$OUT/r_$name.log"; then
        bad "refuse $name: no '$want' in the error: $(head -n 2 "$OUT/r_$name.log" | tr '\n' ' ')"
    elif [ -f "$OUT/r_$name" ] || [ -f "$OUT/r_$name.exe" ]; then
        bad "refuse $name: an image was written"
    else
        ok "refuse $name: $want"
    fi
}
refuse freebsd-noruntime x86_64-freebsd "XT43" FID_IMAGE_ONLY=
refuse openbsd-noruntime x86_64-openbsd "XT53" FID_IMAGE_ONLY=
refuse android-nopie aarch64-android "position-independent" XT_UNUSED=1 --no-pie
refuse android-api5 aarch64-android5 "API level" XT_UNUSED=1
refuse riscv-freebsd riscv64-freebsd "x86_64 and aarch64" FID_IMAGE_ONLY=1

if [ "$fails" -gt 0 ]; then
    echo "xt_elf_notes_check: $fails failure(s) (outputs in $OUT)"
    exit 1
fi
echo "xt_elf_notes_check: all passed (outputs in $OUT)"
