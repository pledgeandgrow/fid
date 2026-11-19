#!/usr/bin/env bash
# competitive-roadmap PL5: the Cortex-M (ARMv7-M, Thumb-2) code generator,
# compiler/nat_thumb.fd.
#
#  1. tests/bootstrap/test_thumb_core.fd: `fid run` and `fid emit --target
#     thumbv7m-linux` under qemu-arm both print the file's expected lines
#     (exit status 0).
#  2. tests/nattools/thumb_panic.fd: an i64 overflow panics ("Panic:
#     Integer overflow on addition", exit 1) after the line before it.
#  3. tests/nattools/thumb_exit.fd: main's result (3) is the exit status.
#  4. `--target thumbv7m-none`: the ELF of a board (two segments: code at
#     0 with the vector table, .data in RAM with its image in flash), for
#     mps2-an385 and lm3s6965evb; when qemu-system-arm is installed, the
#     images run on the board (UART on stdio, semihosting exit) and print
#     the same lines with the same exit status.
#
# Usage: FID=release/fid.exe bash tests/nattools/thumb_check.sh
# On Windows the Linux side runs in WSL (qemu-arm, qemu-system-arm there).
# Exit status: the number of failed checks.
set -u
FID=${FID:-release/fid.exe}
OUT=${OUT:-build/thumb-check}
PY=${PY:-python3}
command -v "$PY" >/dev/null 2>&1 || PY=python
mkdir -p "$OUT"
fails=0
pass=0

ok() { pass=$((pass + 1)); echo "ok   $1"; }
bad() { fails=$((fails + 1)); echo "FAIL $1"; }

# Run a Linux command: here on Linux, else in WSL (paths converted)
on_linux() {
    if [ "$(uname -s)" = Linux ]; then
        "$@"
    else
        local args=() a
        for a in "$@"; do
            if [ -e "$a" ]; then
                a=$(wsl.exe wslpath -u "$(cygpath -m "$(realpath "$a")")" | tr -d '\r')
            fi
            args+=("$a")
        done
        wsl.exe -e "${args[@]}"
    fi
}

have_linux() {
    on_linux sh -c "command -v $1" >/dev/null 2>&1
}

expected() {
    sed -n '/^\/\/ Expected output:/,/^[^\/]/p' "$1" | sed -n 's#^// \{0,1\}##p' | sed '1d'
}

emit() {
    # emit <file> <target> <out> [env...]
    local f=$1 t=$2 o=$3
    shift 3
    env "$@" "$FID" emit "$f" --target "$t" -o "$o" >"$o.log" 2>&1
}

# 1. the core test: run and qemu-arm agree with the expected lines
CORE=tests/bootstrap/test_thumb_core.fd
expected "$CORE" >"$OUT/core.expected"
"$FID" run "$CORE" >"$OUT/core.run" 2>/dev/null
if diff -q "$OUT/core.expected" "$OUT/core.run" >/dev/null; then ok "fid run $CORE"; else bad "fid run $CORE (diff $OUT/core.expected $OUT/core.run)"; fi
if ! have_linux qemu-arm; then
    bad "qemu-arm is not installed (apt install qemu-user)"
else
    if emit "$CORE" thumbv7m-linux "$OUT/core.elf"; then
        on_linux qemu-arm "$OUT/core.elf" >"$OUT/core.out" 2>"$OUT/core.err"
        st=$?
        if [ $st -eq 0 ] && diff -q "$OUT/core.expected" "$OUT/core.out" >/dev/null; then
            ok "thumbv7m-linux $CORE"
        else
            bad "thumbv7m-linux $CORE (status $st; diff $OUT/core.expected $OUT/core.out)"
        fi
    else
        bad "emit --target thumbv7m-linux $CORE ($OUT/core.elf.log)"
    fi
    # 2. an overflow panics
    if emit tests/nattools/thumb_panic.fd thumbv7m-linux "$OUT/panic.elf"; then
        on_linux qemu-arm "$OUT/panic.elf" >"$OUT/panic.out" 2>"$OUT/panic.err"
        st=$?
        if [ $st -eq 1 ] && [ "$(cat "$OUT/panic.out")" = "before" ] && grep -q "^Panic: Integer overflow on addition" "$OUT/panic.err"; then
            ok "thumbv7m-linux overflow panic"
        else
            bad "thumbv7m-linux overflow panic (status $st, $OUT/panic.out, $OUT/panic.err)"
        fi
    else
        bad "emit thumb_panic.fd ($OUT/panic.elf.log)"
    fi
    # 3. the exit status
    if emit tests/nattools/thumb_exit.fd thumbv7m-linux "$OUT/exit.elf"; then
        on_linux qemu-arm "$OUT/exit.elf" >"$OUT/exit.out" 2>&1
        st=$?
        if [ $st -eq 3 ] && [ "$(cat "$OUT/exit.out")" = "half 42" ]; then
            ok "thumbv7m-linux exit status"
        else
            bad "thumbv7m-linux exit status ($st, $OUT/exit.out)"
        fi
    else
        bad "emit thumb_exit.fd ($OUT/exit.elf.log)"
    fi
fi

# 4. no OS: the image's shape, then (with qemu-system-arm) a run per board
check_image() {
    "$PY" - "$1" "$2" <<'PYEOF'
import struct, sys
path, board = sys.argv[1], sys.argv[2]
d = open(path, 'rb').read()
assert d[:4] == b'\x7fELF' and d[4] == 1 and d[5] == 1, 'not an ELF32 LE'
etype, mach = struct.unpack_from('<HH', d, 16)
entry, phoff = struct.unpack_from('<II', d, 24)
phnum = struct.unpack_from('<H', d, 44)[0]
assert etype == 2 and mach == 40, 'not an ARM executable'
assert entry & 1, 'entry is not a Thumb address'
segs = [struct.unpack_from('<IIIIIIII', d, phoff + 32 * i) for i in range(phnum)]
assert phnum == 2, 'expected 2 segments, got %d' % phnum
code, data = segs
assert code[2] == 0 and code[3] == 0, 'code not at 0'
ram = 0x20000000
assert data[2] >= ram and data[3] == code[3] + code[4], 'data: vaddr %x paddr %x' % (data[2], data[3])
sp, reset = struct.unpack_from('<II', d, code[1])
top = {'mps2-an385': 0x20100000, 'lm3s6965evb': 0x20004000}[board]
assert sp == top, 'initial sp %x, want %x' % (sp, top)
assert reset & 1 and reset == entry, 'reset vector %x, entry %x' % (reset, entry)
for v in struct.unpack_from('<14I', d, code[1] + 8):
    assert v & 1, 'an exception vector is not a Thumb address'
print('image ok: %d bytes of code, %d of data' % (code[4], data[4]))
PYEOF
}

# 4a. the no-OS image's own code (reset handler, .data copied from flash,
#     the heap, the system calls it implements: write, mmap, exit) on the
#     test board FID_THUMB_BOARD=qemu-user, which qemu-arm runs (the UART
#     a write(2), the semihosting exit an exit_group)
if have_linux qemu-arm; then
    for prog in "$CORE" tests/nattools/thumb_exit.fd tests/nattools/thumb_panic.fd; do
        name=$(basename "$prog" .fd)-qemu-user
        if ! emit "$prog" thumbv7m-none "$OUT/$name.elf" FID_THUMB_BOARD=qemu-user; then
            bad "emit --target thumbv7m-none $prog (qemu-user, $OUT/$name.elf.log)"
            continue
        fi
        on_linux qemu-arm "$OUT/$name.elf" >"$OUT/$name.out" 2>"$OUT/$name.err"
        st=$?
        case "$prog" in
            "$CORE") want=0; cp "$OUT/core.expected" "$OUT/$name.want" ;;
            *exit.fd) want=3; echo "half 42" >"$OUT/$name.want" ;;
            # (stderr goes to the UART too: one stream)
            *) want=1; printf 'before\nPanic: Integer overflow on addition\n' >"$OUT/$name.want" ;;
        esac
        if [ $st -eq $want ] && diff -q "$OUT/$name.want" "$OUT/$name.out" >/dev/null; then
            ok "thumbv7m-none (qemu-user board) $prog"
        else
            bad "thumbv7m-none (qemu-user board) $prog (status $st, want $want; diff $OUT/$name.want $OUT/$name.out)"
        fi
    done
fi

QSYS=0
if have_linux qemu-system-arm; then QSYS=1; else echo "note: qemu-system-arm is not installed (apt install qemu-system-arm): the no-OS images are checked but not run"; fi
for board in mps2-an385 lm3s6965evb; do
    for prog in "$CORE" tests/nattools/thumb_exit.fd; do
        name=$(basename "$prog" .fd)-$board
        if [ "$board" = lm3s6965evb ] && [ "$prog" = "$CORE" ]; then
            continue    # 64 KB of RAM: the small program only
        fi
        if ! emit "$prog" thumbv7m-none "$OUT/$name.elf" FID_THUMB_BOARD=$board; then
            bad "emit --target thumbv7m-none $prog ($board, $OUT/$name.elf.log)"
            continue
        fi
        if check_image "$OUT/$name.elf" "$board" >"$OUT/$name.shape" 2>&1; then
            ok "thumbv7m-none image $name"
        else
            bad "thumbv7m-none image $name ($(tail -1 "$OUT/$name.shape"))"
            continue
        fi
        if [ $QSYS -eq 1 ]; then
            on_linux timeout 60 qemu-system-arm -M "$board" -cpu cortex-m3 -nographic -monitor none \
                -semihosting-config enable=on,target=native -kernel "$OUT/$name.elf" >"$OUT/$name.out" 2>"$OUT/$name.err"
            st=$?
            tr -d '\r' <"$OUT/$name.out" >"$OUT/$name.txt"
            if [ "$prog" = "$CORE" ]; then
                want=0
                cp "$OUT/core.expected" "$OUT/$name.want"
            else
                want=3
                echo "half 42" >"$OUT/$name.want"
            fi
            if [ $st -eq $want ] && diff -q "$OUT/$name.want" "$OUT/$name.txt" >/dev/null; then
                ok "qemu-system-arm -M $board $prog"
            else
                bad "qemu-system-arm -M $board $prog (status $st, want $want; diff $OUT/$name.want $OUT/$name.txt)"
            fi
        fi
    done
done

echo "$pass passed, $fails failed"
exit $fails
