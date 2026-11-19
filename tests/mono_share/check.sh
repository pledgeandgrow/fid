#!/usr/bin/env bash
# WP19 (llvm-roadmap): generic instances are keyed by their type arguments
# as lowering sees them (compiler/mono.fd mono_share_key), so types that
# lower alike (an alias and its target) share one instance.
#
# Checks on tests/bootstrap/test_mono_share_alias.fd:
#   1. dump-ir has no instance for the alias Meters (count_of__Meters,
#      name_of__Meters, Wrap__get__Meters, pick__Meters): their calls go to
#      the i64 instances, which exist.
#   2. keep__Score is still there: Score has an impl of its own name, so it
#      is not resolved.
#   3. FID_MONO_SHARE_OFF=1 builds the instances as spelled (count_of__Meters
#      is back): the test does exercise sharing.
#   4. FID_MONO_STATS=1 reports at least 3 instances shared.
#   5. `run` prints the expected output with sharing on and off.
#
# usage: tests/mono_share/check.sh [fid]   (default: release/fid.exe, run
# from the repository root)
set -u
FID="${1:-release/fid.exe}"
T=tests/bootstrap/test_mono_share_alias.fd
fail=0
ok() { echo "ok    $1"; }
bad() { echo "FAIL  $1"; fail=1; }

ir=$("$FID" dump-ir "$T" 2>/dev/null)
if printf '%s\n' "$ir" | grep -qE '(pick|count_of|name_of|Wrap__get)__Meters'; then
    bad "an instance for the alias Meters was built:"
    printf '%s\n' "$ir" | grep -oE '(pick|count_of|name_of|Wrap__get)__Meters' | sort -u | sed 's/^/      /'
else
    ok "no instance for the alias Meters"
fi
for f in count_of__i64 name_of__i64 Wrap__get__i64 pick__i64 keep__i64 keep__Score; do
    if printf '%s\n' "$ir" | grep -q "fn $f("; then
        ok "instance $f"
    else
        bad "instance $f missing"
    fi
done

off=$(FID_MONO_SHARE_OFF=1 "$FID" dump-ir "$T" 2>/dev/null)
if printf '%s\n' "$off" | grep -q "fn count_of__Meters("; then
    ok "FID_MONO_SHARE_OFF=1 builds count_of__Meters"
else
    bad "FID_MONO_SHARE_OFF=1 did not build count_of__Meters"
fi

stats=$(FID_MONO_STATS=1 "$FID" dump-ir "$T" 2>&1 >/dev/null | grep 'shared by lowered type' | tail -1)
n=$(printf '%s\n' "$stats" | sed -n 's/.* built, \([0-9]*\) shared by lowered type.*/\1/p')
if [ -n "$n" ] && [ "$n" -ge 3 ]; then
    ok "stats: $stats"
else
    bad "stats: expected at least 3 shared, got '$stats'"
fi

want=$(sed -n '/^\/\/ Expected output:/,/^$/p' "$T" | sed -n 's,^// ,,p' | grep -v '^Expected output:')
got_on=$("$FID" run "$T" 2>/dev/null)
got_off=$(FID_MONO_SHARE_OFF=1 "$FID" run "$T" 2>/dev/null)
if [ "$got_on" = "$want" ]; then ok "run output (sharing on)"; else bad "run output (sharing on) differs"; diff <(printf '%s\n' "$want") <(printf '%s\n' "$got_on") | sed 's/^/      /'; fi
if [ "$got_off" = "$want" ]; then ok "run output (sharing off)"; else bad "run output (sharing off) differs"; fi
exit $fail
