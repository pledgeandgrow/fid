#!/usr/bin/env bash
# TG1 (docs/roadmap/llvm-roadmap.md): instruction selection as rules
# (compiler/nat_ir.fd ng_isel, the table ng_isel_rows). Run after the
# compiler is built:
#   bash tests/nattools/isel_table_check.sh [fid]   (default release/fid.exe)
# 1. Byte identity: for tests/bootstrap/test_machine_isel.fd and a few
#    suite programs, the image of each Linux target (aarch64, riscv64,
#    x86_64) and of --nat Windows is the same with the table (default) and
#    with FID_NAT_OFF=isel (the hand-written selection).
# 2. FID_VERIFY=1 builds pass: nat_sel_check runs every case of
#    nat_enc.fd nat_sel_cases through the table, on values, and compares
#    its bytes with the hand-written selection's.
# 3. Mutation: FID_NAT_MUTATE=isel (MO_SUB's rules take their operands
#    the wrong way round) makes the FID_VERIFY=1 build fail with the
#    selection check's error, on every target: the check sees the table.
# 4. The table drives the code: with FID_NAT_MUTATE=isel and no
#    FID_VERIFY the image differs from the default one (it would not if
#    the table were never consulted).
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-isel-table"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

PROGS="tests/bootstrap/test_machine_isel.fd tests/bootstrap/hello.fd tests/bootstrap/test_machine_ldop.fd tests/bootstrap/test_machine_rules_random.fd"

emit() {   # emit <prog> <target|nat> <out> [env...]
    local p="$1" t="$2" o="$3"
    shift 3
    if [ "$t" = nat ]; then
        env "$@" "$FID" emit "$p" --nat -o "$o" >"$o.log" 2>&1
    else
        env "$@" "$FID" emit "$p" --target "$t" -o "$o" >"$o.log" 2>&1
    fi
}

for t in aarch64-linux riscv64-linux x86_64-linux nat; do
    for p in $PROGS; do
        [ -f "$p" ] || continue
        b=$(basename "$p" .fd)
        if ! emit "$p" "$t" "$OUT/$b-$t-table" FID_NAT_OFF=; then
            bad "TG1 $t $b: build with the table failed ($(tail -1 "$OUT/$b-$t-table.log"))"
            continue
        fi
        if ! emit "$p" "$t" "$OUT/$b-$t-hand" FID_NAT_OFF=isel; then
            bad "TG1 $t $b: build with FID_NAT_OFF=isel failed"
            continue
        fi
        if cmp -s "$OUT/$b-$t-table" "$OUT/$b-$t-hand"; then
            ok "TG1 $t $b: table and hand-written selection give the same image"
        else
            bad "TG1 $t $b: the images differ"
        fi
    done
    T=tests/bootstrap/test_machine_isel.fd
    if emit "$T" "$t" "$OUT/v-$t" FID_VERIFY=1; then
        ok "TG1 $t: FID_VERIFY=1 (nat_sel_check, table vs hand-written) clean"
    else
        bad "TG1 $t: FID_VERIFY=1 build failed: $(grep -m1 'selection' "$OUT/v-$t.log")"
    fi
    if emit "$T" "$t" "$OUT/m-$t" FID_VERIFY=1 FID_NAT_MUTATE=isel; then
        bad "TG1 $t: FID_NAT_MUTATE=isel passed the selection check"
    elif grep -q "instruction selection check" "$OUT/m-$t.log"; then
        ok "TG1 $t: the mutated table is caught by nat_sel_check"
    else
        bad "TG1 $t: the mutated build failed for another reason: $(tail -1 "$OUT/m-$t.log")"
    fi
    if emit "$T" "$t" "$OUT/mm-$t" FID_NAT_MUTATE=isel && emit "$T" "$t" "$OUT/d-$t"; then
        if cmp -s "$OUT/mm-$t" "$OUT/d-$t"; then
            bad "TG1 $t: the mutated table changed nothing (table not used?)"
        else
            ok "TG1 $t: the image comes from the table"
        fi
    else
        bad "TG1 $t: mutated or default build failed"
    fi
done

if [ "$fails" -ne 0 ]; then
    echo "$fails failure(s)"
    exit 1
fi
echo "all TG1 checks passed"
