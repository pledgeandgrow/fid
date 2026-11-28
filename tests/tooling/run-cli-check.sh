#!/usr/bin/env bash
# run-cli-check.sh — `fid run`'s command line (docs/roadmap/bugs-roadmap.md):
#   BG329  `fid run loop.fd --timeout 2` (and `fid --timeout 2 run ...`)
#          stops an endless program after about 2 s, exit 124 (the flag was
#          accepted and never enforced)
#   BG331  `fid run args.fd -- alpha beta` gives the program `alpha beta`
#          (the `--` restarted the dispatch: "Unknown command: alpha"), and
#          `fid watch f.fd run -- a` keeps its `--`
#   BG486  `fid run p.fd --crash-report` of a program that panics: the
#          program's panic, no "fid crash report" (it reported the user's
#          panic as a crash of fid)
#   BG460  `fid dump-ir` / `fid ir` of a program with `use string;` (it was
#          "undefined function ... --> <stdin>")
#
#   FID=release/fid.exe bash tests/tooling/run-cli-check.sh
#   OUT=<dir>   default build/run-cli-check
# Exit status 1 on a failure.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/run-cli-check}"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
run() {
    "$@" > "$OUT/.so" 2> "$OUT/.se" < /dev/null; rc=$?
    so=$(tr -d '\r' < "$OUT/.so"); se=$(tr -d '\r' < "$OUT/.se")
}

# --- BG329 -------------------------------------------------------------------
cat > "$OUT/loop.fd" <<'EOF'
fn main() {
    var i: i64 = 0;
    while true {
        i = i + 1;
    }
}
EOF
t0=$(date +%s)
run timeout 30 "$FID" run "$OUT/loop.fd" --timeout 2
t1=$(date +%s)
if [ "$rc" = 124 ] && [ $((t1 - t0)) -lt 25 ] && printf '%s' "$se" | grep -q "timeout of 2 seconds"; then
    ok "BG329 run --timeout 2 stops the program ($((t1 - t0)) s, exit 124)"
else
    bad "BG329 run --timeout 2: exit $rc after $((t1 - t0)) s, stderr [$se]"
fi
t0=$(date +%s)
run timeout 30 "$FID" --timeout 2 run "$OUT/loop.fd"
t1=$(date +%s)
if [ "$rc" = 124 ] && [ $((t1 - t0)) -lt 25 ]; then
    ok "BG329 the global --timeout 2 (before the command) stops it too"
else
    bad "BG329 fid --timeout 2 run: exit $rc after $((t1 - t0)) s, stdout [$so]"
fi

# --- BG331 -------------------------------------------------------------------
cat > "$OUT/args.fd" <<'EOF'
fn main() {
    var i: i64 = 1;
    while i < 3 {
        match arg_at(i) {
            Option::Some(a) => {
                println(a);
            }
            Option::None => {
                println("-");
            }
        }
        i = i + 1;
    }
}
EOF
run "$FID" run "$OUT/args.fd" alpha beta
plain="$so"
run "$FID" run "$OUT/args.fd" -- alpha beta
if [ "$rc" = 0 ] && [ "$so" = "$(printf 'alpha\nbeta')" ] && [ "$plain" = "$so" ]; then
    ok "BG331 fid run x.fd -- a b: the program gets a b"
else
    bad "BG331 fid run x.fd -- a b: exit $rc, stdout [$so] (without --: [$plain])"
fi
if ! printf '%s' "$so$se" | grep -q "Unknown command"; then
    ok "BG331 no \"Unknown command\" after --"
else
    bad "BG331 \"Unknown command\" after --"
fi

# --- BG486 -------------------------------------------------------------------
cat > "$OUT/p.fd" <<'EOF'
fn main() {
    println("start");
    val v: Vec<i64> = Vec::new();
    println(v[3]);
}
EOF
run "$FID" run "$OUT/p.fd" --crash-report
if [ "$rc" != 0 ] && printf '%s' "$so" | grep -q "start" && ! printf '%s' "$se" | grep -q "fid crash report" && ! printf '%s' "$se" | grep -q "Please report it"; then
    ok "BG486 a panicking program under --crash-report: no fid crash report"
else
    bad "BG486 --crash-report: exit $rc, stderr [$(printf '%s' "$se" | head -5 | tr '\n' '|')]"
fi
if printf '%s' "$se" | grep -qi "panic\|out of bounds"; then
    ok "BG486 the program's own panic is still shown"
else
    bad "BG486 the program's panic is missing: [$se]"
fi

# --- BG460 -------------------------------------------------------------------
cat > "$OUT/s.fd" <<'EOF'
use string;

fn main() {
    println(str_pad_left("7", 3, 48));
}
EOF
for c in dump-ir ir; do
    run "$FID" $c "$OUT/s.fd"
    if [ "$rc" = 0 ] && printf '%s' "$so" | grep -q "str_pad_left" && ! printf '%s' "$se" | grep -q "<stdin>"; then
        ok "BG460 fid $c of a program with use string;"
    else
        bad "BG460 fid $c: exit $rc, stderr [$(printf '%s' "$se" | head -3 | tr '\n' '|')]"
    fi
done

if [ "$fails" -gt 0 ]; then
    echo "run-cli-check: $fails failed"
    exit 1
fi
echo "run-cli-check: all passed"
