#!/usr/bin/env bash
# TG9: RVC, RISC-V's 2-byte instructions (compiler/nat_enc.fd
# nat_rvc_compress, run by compiler/nat_ir.fd ng_rvc_pass; on by default
# since TG19, and with FID_NAT_ON=rvc or --size, FID_NAT_OFF=rvc wins; here
# "plain" is a FID_NAT_OFF=rvc build), run after the compiler is built:
#   bash tests/nattools/rvc_check.sh [fid]   (default release/fid.exe)
#
# 1. tests/nattools/rvc_unit.fd prints "rvc ok" (the 2-byte forms, their
#    expansion, a laid-out function compressed and resolved, the refusals).
# 2. For each program below, built for riscv64-linux with FID_VERIFY=1:
#    plain, with FID_NAT_ON=rvc, with --size, and with FID_NAT_ON=rvc
#    FID_NAT_OFF=rvc. The rvc and --size builds print an `[rvc] N of M
#    instructions in 2 bytes: code A -> B bytes` line with B < A and N > 0;
#    the plain build none; the FID_NAT_OFF=rvc build is byte for byte the
#    plain one. The ELF's e_flags are 5 (double-float ABI + EF_RISCV_RVC)
#    in the rvc build, 4 in the plain one; its .debug_frame CIE has code
#    alignment 2 (4 in the plain one). Every build runs (qemu-riscv64,
#    through WSL on Windows) and prints the file's expected output (or, for
#    a file without one, what the plain build prints).
# 3. The sizes: each program's file and code bytes plain and with RVC, and
#    the total saving.
# Exit 1 on a failure. Afterwards, the whole suite on RISC-V with RVC:
#   NATIVE=1 FID_NAT_ON=rvc FID=release/fid.exe JOBS=4 bash scripts/cross-test.sh riscv64-linux
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-rvc"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi
RUNNER=""
if [ -n "$WSL" ]; then
    if $WSL sh -c "command -v qemu-riscv64 >/dev/null"; then RUNNER="qemu-riscv64"; fi
elif [ "$HOST" = linux ]; then
    if [ "$(uname -m)" = riscv64 ]; then RUNNER="native"
    elif command -v qemu-riscv64 >/dev/null 2>&1; then RUNNER="qemu-riscv64"; fi
fi

# run <exe>: its output; status 99 when this host cannot run it
run() {
    [ -n "$RUNNER" ] || return 99
    local q="$RUNNER"
    [ "$q" = native ] && q=""
    (cd "$OUT" && $WSL $q "./$1" 2>&1 | tr -d '\r')
}

# the ELF's e_flags (offset 48, 4 bytes)
eflags() { od -An -tu4 -j48 -N4 "$1" | tr -d ' '; }

# the code alignment factor of the first CIE of .debug_frame ("-": none)
cie_align() {
    python3 - "$1" <<'EOF' 2>/dev/null || echo "-"
import struct, sys
d = open(sys.argv[1], 'rb').read()
shoff = struct.unpack_from('<Q', d, 0x28)[0]
ent, num, stx = struct.unpack_from('<HHH', d, 0x3A)
sh = lambda i: struct.unpack_from('<IIQQQQIIQQ', d, shoff + i * ent)
st = sh(stx)
for i in range(num):
    s = sh(i)
    no = st[4] + s[0]
    if d[no:d.index(b'\0', no)] == b'.debug_frame':
        p = s[4] + 4 + 4 + 1
        while d[p] != 0:
            p += 1
        print(d[p + 1])
        sys.exit(0)
print('-')
EOF
}

# 1. the unit check
got=$("$FID" run tests/nattools/rvc_unit.fd 2>/dev/null | tr -d '\r' | grep '^rvc ')
if [ "$got" = "rvc ok" ]; then ok "rvc_unit: rvc ok"; else bad "rvc_unit: ${got:-no output}"; fi

PROGS="tests/bootstrap/test_machine_jtab.fd tests/bootstrap/test_irswitch_match.fd
tests/bootstrap/test_machine_memop.fd tests/bootstrap/test_vectorize.fd
tests/bootstrap/test_machine_tailcall_frame.fd tests/bootstrap/test_machine_bparams.fd
tests/bootstrap/test_machine_isel_tg1b.fd tests/bootstrap/test_machine_rules_random.fd"

tot_plain=0
tot_rvc=0
printf '%-34s %10s %10s %10s %10s\n' program file file+rvc code code+rvc > "$OUT/sizes.txt"
for src in $PROGS; do
    [ -f "$src" ] || { echo "skip $src (missing)"; continue; }
    base=$(basename "$src" .fd)
    WANT="$(expected "$src")"
    built=1
    for mode in plain rvc size off; do
        exe="$base-$mode"
        sw=(FID_VERIFY=1)
        extra=()
        # TG19: RVC is the default now, so "plain" (the uncompressed
        # reference) says FID_NAT_OFF=rvc; tests/nattools/tg19_size_check.sh
        # checks the default build
        [ "$mode" = plain ] && sw+=(FID_NAT_OFF=rvc)
        [ "$mode" = rvc ] && sw+=(FID_NAT_ON=rvc)
        [ "$mode" = size ] && extra=(--size)
        [ "$mode" = off ] && sw+=(FID_NAT_ON=rvc FID_NAT_OFF=rvc)
        if ! env "${sw[@]}" "$FID" emit "$src" --target riscv64-linux "${extra[@]+"${extra[@]}"}" -o "$OUT/$exe" >"$OUT/$exe.out" 2>"$OUT/$exe.log"; then
            bad "$base $mode: emit failed: $(grep -v '^\[' "$OUT/$exe.log" | tail -2)"
            built=0
            continue
        fi
        line=$(grep '^\[rvc\]' "$OUT/$exe.log" | tail -1)
        case "$mode" in
            plain|off)
                if [ -n "$line" ]; then bad "$base $mode: $line"; else ok "$base $mode: no compression"; fi ;;
            *)
                a=$(echo "$line" | sed -n 's/.*code \([0-9]*\) -> \([0-9]*\) bytes.*/\1/p')
                b=$(echo "$line" | sed -n 's/.*code \([0-9]*\) -> \([0-9]*\) bytes.*/\2/p')
                n=$(echo "$line" | sed -n 's/^\[rvc\] \([0-9]*\) of.*/\1/p')
                if [ -n "$a" ] && [ -n "$b" ] && [ "$b" -lt "$a" ] && [ "${n:-0}" -gt 0 ]; then
                    ok "$base $mode: $line"
                    if [ "$mode" = rvc ]; then
                        echo "$a $b" > "$OUT/$base.code"
                    fi
                else
                    bad "$base $mode: ${line:-no [rvc] line}"
                fi ;;
        esac
        out=$(run "$exe")
        rc=$?
        want="$WANT"
        [ -z "$want" ] && [ "$mode" != plain ] && want="$(cat "$OUT/$base-plain.run" 2>/dev/null)"
        if [ $rc -eq 99 ]; then
            ok "$base $mode: built (no qemu-riscv64 here to run it)"
        elif [ -z "$want" ] && [ "$mode" = plain ]; then
            echo "$out" > "$OUT/$base-plain.run"
            ok "$base $mode: ran (no expected output in the file)"
        elif [ "$out" = "$want" ]; then
            [ "$mode" = plain ] && echo "$out" > "$OUT/$base-plain.run"
            ok "$base $mode: output as expected"
        else
            bad "$base $mode: output differs"
            diff <(echo "$want") <(echo "$out") | head -8
        fi
    done
    [ $built -eq 1 ] || continue
    if cmp -s "$OUT/$base-plain" "$OUT/$base-off"; then
        ok "$base: FID_NAT_OFF=rvc gives the plain image"
    else
        bad "$base: FID_NAT_OFF=rvc image differs from the plain one"
    fi
    fp=$(eflags "$OUT/$base-plain")
    fr=$(eflags "$OUT/$base-rvc")
    if [ "$fp" = 4 ] && [ "$fr" = 5 ]; then
        ok "$base: e_flags 4 plain, 5 (RVC) compressed"
    else
        bad "$base: e_flags $fp plain, $fr compressed (want 4, 5)"
    fi
    cp=$(cie_align "$OUT/$base-plain")
    cr=$(cie_align "$OUT/$base-rvc")
    if [ "$cr" = 2 ] && [ "$cp" = 4 ]; then
        ok "$base: .debug_frame code alignment 4 plain, 2 compressed"
    elif [ "$cr" = "-" ] && [ "$cp" = "-" ]; then
        ok "$base: no .debug_frame (not checked)"
    else
        bad "$base: .debug_frame code alignment $cp plain, $cr compressed (want 4, 2)"
    fi
    sp=$(wc -c < "$OUT/$base-plain" | tr -d ' ')
    sr=$(wc -c < "$OUT/$base-rvc" | tr -d ' ')
    ca="-"
    cb="-"
    if [ -f "$OUT/$base.code" ]; then
        read -r ca cb < "$OUT/$base.code"
        tot_plain=$((tot_plain + ca))
        tot_rvc=$((tot_rvc + cb))
    fi
    printf '%-34s %10s %10s %10s %10s\n' "$base" "$sp" "$sr" "$ca" "$cb" >> "$OUT/sizes.txt"
done

cat "$OUT/sizes.txt"
if [ "$tot_plain" -gt 0 ]; then
    echo "code bytes: $tot_plain -> $tot_rvc ($(( (tot_plain - tot_rvc) * 100 / tot_plain ))% smaller)"
fi
echo "$fails failure(s)"
[ "$fails" -eq 0 ]
