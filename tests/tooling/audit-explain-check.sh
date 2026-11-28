#!/usr/bin/env bash
# audit-explain-check.sh — the reports about a program say what it does
# (docs/roadmap/bugs-roadmap.md):
#   BG459  `fid audit --strict` lists an `unsafe{` block written without a
#          space (and with a tab) — it reported "0 unsafe site(s)", exit 0
#   BG481  `fid explain --alloc` lists a string `+` (p_str_concat /
#          p_str_concat_int) — it said "no heap allocations"
#   BG328  `fid veil --format html` paints only the user's own lines: a
#          never-called function's lines are not "covered", a `}` line is
#          not "uncovered" because core ran its line number
#   BG326  `fid profile p.fd -o out` with `out` already there reports this
#          run's samples, not the old file's (needs a C compiler; skipped
#          without one)
#
#   FID=release/fid.exe bash tests/tooling/audit-explain-check.sh
#   OUT=<dir>   default build/audit-explain-check
# Exit status 1 on a failure.
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
OUT="${OUT:-build/audit-explain-check}"
mkdir -p "$OUT"
case "$OUT" in /*|[A-Za-z]:*) ;; *) OUT="$ROOT/$OUT" ;; esac
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
run() {
    "$@" > "$OUT/.so" 2> "$OUT/.se" < /dev/null; rc=$?
    so=$(tr -d '\r' < "$OUT/.so"); se=$(tr -d '\r' < "$OUT/.se")
}

# --- BG459 -------------------------------------------------------------------
printf 'fn main() {\n    var m: i64 = 0;\n    unsafe{\n        m = p_malloc(8);\n    }\n    var r: i64 = 0;\n    unsafe\t{\n        val q: *mut i64 = m;\n        *q = 41;\n        r = *q;\n        p_free(m);\n    }\n    println(r + 1);\n}\n' > "$OUT/a.fd"
run "$FID" audit "$OUT/a.fd" --strict
if [ "$rc" = 1 ] && printf '%s' "$so$se" | grep -q "2 unsafe site(s) outside the stdlib"; then
    ok "BG459 unsafe{ and unsafe<tab>{ are both listed, --strict exits 1"
else
    bad "BG459 audit: exit $rc, [$(printf '%s' "$so$se" | grep -m2 "unsafe site" | tr '\n' '|')]"
fi
printf 'fn main() {\n    val is_unsafe = 1;\n    println(is_unsafe);\n}\n' > "$OUT/a2.fd"
run "$FID" audit "$OUT/a2.fd" --strict
if [ "$rc" = 0 ]; then
    ok "BG459 a name holding \"unsafe\" is not a block"
else
    bad "BG459 is_unsafe: exit $rc, [$(printf '%s' "$so$se" | grep -m2 "unsafe" | tr '\n' '|')]"
fi

# --- BG481 -------------------------------------------------------------------
cat > "$OUT/s.fd" <<'EOF'
fn greet(name: str, n: i64) -> str {
    val a = "hello " + name;
    val b = f"{a} #{n}";
    return b + "!";
}

fn main() {
    println(greet("bob", 3));
}
EOF
run "$FID" explain "$OUT/s.fd" --alloc greet
if printf '%s' "$so$se" | grep -q "string concatenation" && ! printf '%s' "$so$se" | grep -q "no heap allocations"; then
    ok "BG481 explain --alloc lists the string concatenation"
else
    bad "BG481 explain --alloc greet: [$(printf '%s' "$so$se" | head -4 | tr '\n' '|')]"
fi

# --- BG328 -------------------------------------------------------------------
{
    printf 'fn main() {\n    var v: Vec<i64> = Vec::new();\n    v.push(1);\n    v.push(2);\n    var m: HashMap<str, i64> = HashMap::new();\n    m.insert("a", 1);\n    println(v.len() + m.len());\n}\n'
    i=0
    while [ $i -lt 300 ]; do
        printf 'fn never_%d(x: i64) -> i64 {\n    return x + %d;\n}\n' $i $i
        i=$((i + 1))
    done
} > "$OUT/v2.fd"
run "$FID" veil "$OUT/v2.fd" --format html --output "$OUT/cov"
h="$OUT/cov/coverage.html"
if [ -f "$h" ]; then
    cov_never=$(grep 'class="line covered"' "$h" | grep -c "never_\|return x +")
    unc_brace=$(grep 'class="line uncovered"' "$h" | grep -cE '</span> \}</span>')
    if [ "$cov_never" = 0 ] && [ "$unc_brace" -le 1 ]; then
        ok "BG328 no never-called line is covered, no stray } is uncovered"
    else
        bad "BG328 $cov_never never-called lines covered, $unc_brace } lines uncovered"
    fi
    if grep 'class="line covered"' "$h" | grep -q "v.push(1)"; then
        ok "BG328 main's own lines are still covered"
    else
        bad "BG328 main's v.push(1) is not covered"
    fi
else
    bad "BG328 no report: exit $rc, [$(printf '%s' "$so$se" | head -3 | tr '\n' '|')]"
fi

# --- BG326 -------------------------------------------------------------------
if [ -z "${CC:-}" ]; then
    for c in gcc clang cc tcc; do
        if command -v $c > /dev/null 2>&1; then export CC=$c; break; fi
    done
    if [ -z "${CC:-}" ] && command -v zig > /dev/null 2>&1; then export CC="zig cc"; fi
fi
if [ -z "${CC:-}" ]; then
    echo "skip BG326: no C compiler (set CC)"
else
    cat > "$OUT/fib.fd" <<'EOF'
fn fib(n: i64) -> i64 {
    if n < 2 {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

fn main() {
    println(fib(32));
}
EOF
    echo "stale_old_profile;main 999" > "$OUT/prof.txt"
    ( cd "$OUT" && "$FID" profile fib.fd -o prof.txt > "$OUT/prof.out" 2>&1 < /dev/null ); rc=$?
    if [ "$rc" = 0 ] && ! grep -q "stale_old_profile" "$OUT/prof.txt" && grep -q "fib" "$OUT/prof.txt"; then
        ok "BG326 profile -o over an existing file: this run's stacks"
    else
        bad "BG326 profile: exit $rc, prof.txt [$(head -2 "$OUT/prof.txt" | tr '\n' '|')]"
    fi
    if [ ! -e "$OUT/_fides_profile_tmp.c" ] && [ ! -e "$OUT/_fides_profile_tmp.exe" ] && [ ! -e "$OUT/_fides_profile_tmp" ]; then
        ok "BG326 the profile build's files are removed"
    else
        bad "BG326 _fides_profile_tmp.* left in the directory"
    fi
fi

if [ "$fails" -gt 0 ]; then
    echo "audit-explain-check: $fails failed"
    exit 1
fi
echo "audit-explain-check: all passed"
