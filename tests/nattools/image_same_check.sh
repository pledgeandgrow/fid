#!/usr/bin/env bash
# TG13 (docs/roadmap/llvm-roadmap.md, docs/image-writer.md): every image
# writer goes through compiler/image_writer.fd. Until Nov 17 this script
# compared each format with its old body (FID_IMG_OLD=1); the old bodies
# are gone now (the unit tests tests/nattools/image_writer_unit.fd and
# image_writer_obj_unit.fd pin the writers' bytes by digest), so on real
# programs it checks that each image is the same on two builds and that the
# system tools read it without complaint.
#
#   FID=release/fid.exe bash tests/nattools/image_same_check.sh
#   FILES="tests/bootstrap/test_match.fd ..." to choose the programs
#   REF=<an older fid> to also compare with what that compiler writes
#     (equal only when nothing else in the backend changed in between)
#
# For each program and each format, the image is built twice (a difference
# is nondeterminism, a failure), then read back: `readelf -W -h -l -S` (or
# `wsl readelf` from Windows) for ELF, must print no "Warning"/"Error";
# `llvm-objdump --macho -h` for Mach-O when it is installed; the file's
# magic for PE and COFF (the PE ones run in the cross-test suites).
# Formats (the writer each one reaches):
#   elf-x64 / elf-a64 / elf-rv64   emit --target <arch>-linux: nat_elf_debug
#   elf-x64-nodebug                FID_NAT_OFF=debug: nat_elf
#   elf-x64-pie                    FID_NAT_ON=pie FID_NAT_OFF=debug: nat_elf (ET_DYN)
#   obj-x64 / obj-a64              emit --obj: nat_elf_object
#   pe-nat                         emit --nat: nat_pe, nat_pe_symbols
#   macho-a64 / macho-x64          emit --target <arch>-macos: nat_macho
#   pe                             emit (Windows default, pe_ir): pe_build_file
#   pe-g                           emit -g: pe_add_debug_sections, COFF symbols
#   coff                           codegen --coff: CoffEmitter_build_coff
# (ELF and Mach-O objects of `codegen --elf/--macho` are not compared: their
# new writers fix the old bodies' bytes on purpose; tests/nattools/
# image_writer_obj_unit.fd checks them. UEFI (pe_build_efi) and fat
# Mach-O (nat_macho_fat) have no command-line entry yet: the unit tests
# cover them.)
# Passing: the last line is "image_same_check: N same, 0 differ" (skipped
# formats, e.g. a target the build cannot reach, are listed, not counted;
# "differ" counts nondeterministic images, images a tool rejects, and with
# REF images that differ from the reference compiler's).
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/image_same_check}"
FILES="${FILES:-tests/bootstrap/hello.fd tests/bootstrap/test_match.fd tests/bootstrap/test_closure.fd tests/bootstrap/test_struct_eq.fd}"
FORMATS="${FORMATS:-elf-x64 elf-a64 elf-rv64 elf-x64-nodebug elf-x64-pie obj-x64 obj-a64 pe-nat macho-a64 macho-x64 pe pe-g coff}"
REF="${REF:-}"
mkdir -p "$OUT"
same=0
differ=0
skipped=""

# build <format> <file> <out> [env assignments...]: 0 when <out> was written
# (with the compiler BUILD_FID, else FID)
build() {
    local fmt="$1" f="$2" o="$3"
    local B="${BUILD_FID:-$FID}"
    shift 3
    rm -f "$o"
    case "$fmt" in
        elf-x64) env "$@" "$B" emit "$f" --target x86_64-linux -o "$o" ;;
        elf-a64) env "$@" "$B" emit "$f" --target aarch64-linux -o "$o" ;;
        elf-rv64) env "$@" "$B" emit "$f" --target riscv64-linux -o "$o" ;;
        elf-x64-nodebug) env "$@" FID_NAT_OFF=debug "$B" emit "$f" --target x86_64-linux -o "$o" ;;
        elf-x64-pie) env "$@" FID_NAT_ON=pie FID_NAT_OFF=debug "$B" emit "$f" --target x86_64-linux -o "$o" ;;
        obj-x64) env "$@" "$B" emit "$f" --target x86_64-linux --obj -o "$o" ;;
        obj-a64) env "$@" "$B" emit "$f" --target aarch64-linux --obj -o "$o" ;;
        pe-nat) env "$@" "$B" emit "$f" --nat -o "$o" ;;
        macho-a64) env "$@" "$B" emit "$f" --target aarch64-macos -o "$o" ;;
        macho-x64) env "$@" "$B" emit "$f" --target x86_64-macos -o "$o" ;;
        pe) env "$@" "$B" emit "$f" -o "$o" ;;
        pe-g) env "$@" "$B" emit "$f" -g -o "$o" ;;
        coff) env "$@" "$B" codegen "$f" --coff "$o" ;;
        *) echo "unknown format $fmt"; return 1 ;;
    esac >"$o.log" 2>&1
    [ -s "$o" ]
}

# readelf here, else in WSL (from Git Bash on Windows), else none
READELF=""
if command -v readelf >/dev/null 2>&1; then
    READELF=host
elif command -v wsl >/dev/null 2>&1 && wsl sh -c 'command -v readelf' >/dev/null 2>&1; then
    READELF=wsl
fi
[ -z "$READELF" ] && echo "note: no readelf (here or in WSL): ELF images get the magic check only"

elf_tool() {
    if [ "$READELF" = host ]; then
        readelf -W -h -l -S "$1" 2>&1
    else
        # (MSYS_NO_PATHCONV: Git Bash would turn /mnt/... back into C:/...)
        MSYS_NO_PATHCONV=1 wsl readelf -W -h -l -S "$(wsl wslpath -a "$(cygpath -m "$1")" | tr -d '\r')" 2>&1
    fi
}

magic() {
    head -c "$2" "$1" | od -An -tx1 | tr -d ' \n'
}

# check_image <format> <file>: prints why the system tools reject it, or
# nothing
check_image() {
    local fmt="$1" o="$2" r
    case "$fmt" in
        elf-*|obj-*)
            if [ "$(magic "$o" 4)" != "7f454c46" ]; then
                echo "not an ELF file"
                return
            fi
            if [ -n "$READELF" ]; then
                r=$(elf_tool "$o")
                if [ $? -ne 0 ] || echo "$r" | grep -qiE "warning|error"; then
                    echo "readelf: $(echo "$r" | grep -iE "warning|error" | head -1)"
                fi
            fi ;;
        macho-*)
            if [ "$(magic "$o" 4)" != "cffaedfe" ]; then
                echo "not a 64-bit Mach-O file"
                return
            fi
            if command -v llvm-objdump >/dev/null 2>&1; then
                r=$(llvm-objdump --macho -h "$o" 2>&1) || echo "llvm-objdump: $(echo "$r" | tail -1)"
            fi ;;
        pe|pe-g|pe-nat)
            [ "$(magic "$o" 2)" = "4d5a" ] || echo "not a PE file" ;;
        coff)
            [ "$(magic "$o" 2)" = "6486" ] || echo "not an AMD64 COFF object" ;;
    esac
}

for f in $FILES; do
    base=$(basename "$f" .fd)
    for fmt in $FORMATS; do
        a="$OUT/$base.$fmt.1"
        b="$OUT/$base.$fmt.2"
        c="$OUT/$base.$fmt.ref"
        if ! build "$fmt" "$f" "$a"; then
            skipped="$skipped $base/$fmt"
            echo "skip   $base $fmt (no output: $a.log)"
            continue
        fi
        build "$fmt" "$f" "$b"
        if ! cmp -s "$a" "$b"; then
            differ=$((differ + 1))
            echo "NONDET $base $fmt: two builds differ: $(cmp "$a" "$b" | head -1)"
            continue
        fi
        why=$(check_image "$fmt" "$a")
        if [ -n "$why" ]; then
            differ=$((differ + 1))
            echo "BAD    $base $fmt: $why"
            continue
        fi
        if [ -n "$REF" ]; then
            if BUILD_FID="$REF" build "$fmt" "$f" "$c" && ! cmp -s "$a" "$c"; then
                differ=$((differ + 1))
                echo "DIFF   $base $fmt: not what $REF writes: $(cmp "$a" "$c" | head -1)"
                continue
            fi
        fi
        same=$((same + 1))
        echo "same   $base $fmt ($(wc -c <"$a") bytes)"
    done
done
[ -n "$skipped" ] && echo "skipped:$skipped"
echo "image_same_check: $same same, $differ differ"
[ "$differ" -eq 0 ]
