#!/usr/bin/env bash
# MI27: jump tables for dense `match` on the machine layer
# (compiler/nat_ir.fd ng_jt_span / ng_gen_jtab / ng_jtab_emit, MO_JTAB and
# MO_JTE in compiler/mir.fd, NATFIX_SELF32 in compiler/nat_enc.fd), run
# after the compiler is built:
#   bash tests/nattools/machine_jtab_check.sh [fid]   (default release/fid.exe)
#
# For x86_64-linux, aarch64-linux, riscv64-linux and --nat Windows,
# tests/bootstrap/test_machine_jtab.fd and test_irswitch_match.fd are built
# with FID_VERIFY=1 (the MIR verifier checks each dispatch is followed by
# its entries; the allocation checker sees every table target as a
# successor) three ways: by default, with FID_NAT_OFF=jtab (the tree of
# compares, as before) and with FID_SWITCH=off (no Switch at all). All
# three must print the file's expected output where this host can run
# them. The default build's MIR (FID_NAT_DUMP='*') must hold a `jtab` in
# digit_name, holes, neg_base, apply, live_across and wide64 and none in
# sparse and bits3; the other two builds none at all. x86_64-linux is also
# built as a PIE (FID_NAT_ON=pie: the table needs no relocation) and run.
# With llvm-objdump / objdump, the default x86-64 binary must hold an
# indirect `jmp *%r10` and the FID_NAT_OFF=jtab one none.
#
# Linux executables run directly on Linux, through WSL on Windows
# (qemu-aarch64 / qemu-riscv64 for the other machines); a target whose
# runner is missing is built and checked only. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-jtab"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

# run_exe <exe name> <target>: the program's output; status 99 when this
# host cannot run it
run_exe() {
    local exe="$1" t="$2" q=""
    case "$t" in
        aarch64-linux) q=qemu-aarch64 ;;
        riscv64-linux) q=qemu-riscv64 ;;
    esac
    case "$t" in
        *-linux)
            if [ "$HOST" = windows ] && [ -z "$WSL" ]; then return 99; fi
            if [ -n "$q" ]; then
                if [ -n "$WSL" ]; then
                    (cd "$OUT" && $WSL sh -c "command -v $q >/dev/null") || return 99
                elif ! command -v "$q" >/dev/null 2>&1; then
                    if [ "$(uname -m)" = "${t%%-*}" ]; then q=""; else return 99; fi
                fi
            fi
            (cd "$OUT" && $WSL $q "./$exe" 2>&1 | tr -d '\r')
            ;;
        windows)
            [ "$HOST" = windows ] || return 99
            (cd "$OUT" && "./$exe" 2>&1 | tr -d '\r')
            ;;
    esac
}

# the functions whose MIR (last dump of each) holds a jtab
jtab_fns() {
    awk '/^\[mir\] [^ ]+ after .*: [0-9]+ operations/ { fn = $2; next }
         /^\[mir\] [0-9]+: jtab / { j[fn] = 1 }
         END { for (f in j) print f }' "$1" | sort
}

OBJDUMP=""
for c in llvm-objdump objdump; do
    if command -v "$c" >/dev/null 2>&1; then OBJDUMP="$c"; break; fi
done

for src in tests/bootstrap/test_machine_jtab.fd tests/bootstrap/test_irswitch_match.fd; do
    WANT="$(expected "$src")"
    base=$(basename "$src" .fd)
    for t in x86_64-linux aarch64-linux riscv64-linux windows pie; do
        for mode in default nojtab noswitch; do
            [ "$t" = pie ] && [ "$mode" != default ] && continue
            name="$base-$t-$mode"
            exe="$name"
            targ=(--target "$t")
            sw=(FID_NAT_DUMP='*')
            [ "$t" = windows ] && exe="$name.exe" && targ=(--nat)
            [ "$t" = pie ] && targ=(--target x86_64-linux) && sw+=(FID_NAT_ON=pie)
            [ "$mode" = nojtab ] && sw+=(FID_NAT_OFF=jtab)
            [ "$mode" = noswitch ] && sw+=(FID_SWITCH=off)
            if ! env FID_VERIFY=1 "${sw[@]}" "$FID" emit "$src" "${targ[@]}" -o "$OUT/$exe" >"$OUT/$name.out" 2>"$OUT/$name.log"; then
                bad "$base $t $mode: emit failed: $(grep -v '^\[' "$OUT/$name.log" | tail -2)"
                continue
            fi
            have="$(jtab_fns "$OUT/$name.log" | tr '\n' ' ')"
            if [ "$mode" = default ]; then
                if [ "$base" = test_machine_jtab ]; then
                    miss=""
                    for f in digit_name holes neg_base apply live_across wide64; do
                        case " $have " in *" $f "*) ;; *) miss="$miss $f" ;; esac
                    done
                    extra=""
                    for f in sparse bits3; do
                        case " $have " in *" $f "*) extra="$extra $f" ;; esac
                    done
                    if [ -z "$miss" ] && [ -z "$extra" ]; then
                        ok "$base $t: jump tables in the dense switches only ($have)"
                    else
                        bad "$base $t: jump tables missing in:$miss; unexpected in:$extra"
                    fi
                elif [ -n "$have" ]; then
                    ok "$base $t: jump tables in $have"
                else
                    bad "$base $t: no jump table (small has five dense keys)"
                fi
            elif [ -n "$have" ]; then
                bad "$base $t $mode: a jump table with the switch off: $have"
            else
                ok "$base $t $mode: no jump table"
            fi
            rt="$t"
            [ "$t" = pie ] && rt=x86_64-linux
            got=$(run_exe "$exe" "$rt")
            rc=$?
            if [ $rc -eq 99 ]; then
                ok "$base $t $mode: built with FID_VERIFY=1 (not run on this host)"
            elif [ "$got" = "$WANT" ]; then
                ok "$base $t $mode: output as expected"
            else
                bad "$base $t $mode: output differs"
                diff <(echo "$WANT") <(echo "$got") | head -10
            fi
        done
    done
done

# the indirect jump in the x86-64 code, only with the tables
if [ -n "$OBJDUMP" ]; then
    jon=$("$OBJDUMP" -d "$OUT/test_machine_jtab-x86_64-linux-default" 2>/dev/null | grep -cE 'jmp[q]?[[:space:]]+\*%r10')
    joff=$("$OBJDUMP" -d "$OUT/test_machine_jtab-x86_64-linux-nojtab" 2>/dev/null | grep -cE 'jmp[q]?[[:space:]]+\*%r10')
    if [ "$jon" -gt 0 ] && [ "$joff" -eq 0 ]; then
        ok "x86-64: $jon indirect jumps through a table, none with FID_NAT_OFF=jtab"
    else
        bad "x86-64: jmp *%r10: $jon by default, $joff with FID_NAT_OFF=jtab"
    fi
else
    echo "skip disassembly check (no objdump)"
fi

echo "$fails failure(s)"
[ "$fails" -eq 0 ]
