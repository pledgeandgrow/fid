#!/bin/bash
# Competitive roadmap OP23: the runtime's copy, byte search, equality,
# ordering and hashing go a word (or a vector) at a time on every back end:
# the C runtime (compiler/codegen_c.fd fast_core_vec_body: libc's
# memmove/memchr/memcmp/strlen, 8-byte words), native Windows
# (stdlib/pe_runtime/base.fd: msvcrt's routines, word hash) and native
# Linux (stdlib/linux_runtime/base.fd and sys.fd: aligned words).
#   FID=release/fid.exe bash tests/tooling/op23-memops-bench.sh
# Run from the repository root. Checks, for the C build (ccompile, needs a C
# compiler) and the native build of this host (emit: Windows .exe, or
# x86_64-linux on Linux):
#   1. tests/bootstrap/test_runtime_memops.fd, test_runtime_strcmp.fd and
#      test_mem_routines.fd print what their "Expected output" says;
#   2. tests/tooling/op23_bench.fd gives the byte loops' results, and, in
#      the native build, each runtime routine (eq, find, compare, copy,
#      hash) is at least twice as fast as its byte loop on 1 MB (best of 3
#      runs; time on a quiet machine). The C build's ratios are printed
#      only: a C compiler may vectorize the byte loops themselves (LLVM's
#      early-exit vectorizer), so they are no "before" there;
#   3. natively, tests/tooling/op23_hash_check.fd finds the word-at-a-time
#      hash equal to its byte-by-byte definition everywhere ("... 0").
# With QEMU=1 on Linux (or WSL=1 on Windows: the Linux builds run in WSL),
# the aarch64-linux and riscv64-linux builds are checked too (1 and 3 only:
# emulated timings mean nothing). Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
W=$(mktemp -d)
fail=0
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) host=windows; exe=.exe ;;
    *) host=linux; exe= ;;
esac

expected() {
    sed -n '/Expected output/,/^[^/]/p' "$1" | grep '^// ' | grep -v 'Expected output' | sed 's|^// ||'
}

# run <label> <binary> [runner]
run_tests() {
    local label=$1 build=$2 runner=$3
    for t in test_runtime_memops test_runtime_strcmp test_mem_routines; do
        local bin="$W/$t.$label$exe"
        if ! $build "tests/bootstrap/$t.fd" "$bin" > "$W/$t.$label.log" 2>&1; then
            echo "FAIL $t ($label): build"; tail -5 "$W/$t.$label.log"; fail=1; continue
        fi
        if diff <($runner "$bin" 2>&1 | tr -d '\r') <(expected "tests/bootstrap/$t.fd") > "$W/$t.$label.diff"; then
            echo "ok   $t ($label)"
        else
            echo "FAIL $t ($label)"; head -10 "$W/$t.$label.diff"; fail=1
        fi
    done
}

# bench <label> <binary> <enforce: 1 = fail under 2x>
bench() {
    local label=$1 bin=$2 enforce=$3
    for k in 1 2 3; do
        "$bin" | tr -d '\r' > "$W/bench.$label.$k" || { echo "FAIL op23_bench ($label): exit"; fail=1; return; }
    done
    if grep -q " false" "$W/bench.$label.1"; then
        echo "FAIL op23_bench ($label): a runtime routine differs from its byte loop"; cat "$W/bench.$label.1"; fail=1
    fi
    for op in eq find compare copy hash; do
        # the best ratio of the three runs
        local r
        r=$(grep -h "^time $op " "$W"/bench.$label.? | awk '{ rt = $3; if (rt < 1) rt = 1; x = $4 / rt; if (x > b) b = x } END { printf "%.1f", b }')
        if awk -v r="$r" 'BEGIN { exit !(r >= 2.0) }'; then
            echo "ok   $op ($label): ${r}x the byte loop"
        elif [ "$enforce" != 1 ]; then
            echo "info $op ($label): ${r}x the byte loop"
        else
            echo "FAIL $op ($label): ${r}x the byte loop (want 2x)"; fail=1
        fi
    done
}

hash_check() {
    local label=$1 bin=$2 runner=$3
    local out
    out=$($runner "$bin" | tr -d '\r')
    if [ "$out" = "$(printf 'lengths: 656 0\npage edges: 840 0')" ]; then
        echo "ok   op23_hash_check ($label)"
    else
        echo "FAIL op23_hash_check ($label): $out"; fail=1
    fi
}

c_build() { "$FID" ccompile "$1" -o "$2"; }
native_build() {
    if [ $host = windows ]; then
        "$FID" emit "$1" -o "$2"
    else
        "$FID" emit "$1" --target x86_64-linux -o "$2" && chmod +x "$2"
    fi
}

# C
run_tests c c_build ""
if c_build tests/tooling/op23_bench.fd "$W/op23_bench.c$exe" > "$W/bench.c.log" 2>&1; then
    bench c "$W/op23_bench.c$exe" 0
else
    echo "FAIL op23_bench (c): build"; tail -5 "$W/bench.c.log"; fail=1
fi

# native
run_tests native native_build ""
if native_build tests/tooling/op23_bench.fd "$W/op23_bench.n$exe" > "$W/bench.n.log" 2>&1; then
    bench native "$W/op23_bench.n$exe" 1
else
    echo "FAIL op23_bench (native): build"; tail -5 "$W/bench.n.log"; fail=1
fi
if native_build tests/tooling/op23_hash_check.fd "$W/hc.n$exe" > "$W/hc.n.log" 2>&1; then
    hash_check native "$W/hc.n$exe" ""
else
    echo "FAIL op23_hash_check (native): build"; tail -5 "$W/hc.n.log"; fail=1
fi

# other Linux targets under qemu (and x86-64 Linux from a Windows host)
lin() {
    if [ $host = windows ]; then MSYS_NO_PATHCONV=1 wsl -e "$@"; else "$@"; fi
}
if [ "${QEMU:-}" = 1 ] || [ "${WSL:-}" = 1 ]; then
    for tg in x86_64-linux aarch64-linux riscv64-linux; do
        [ $host = linux ] && [ $tg = x86_64-linux ] && continue
        case $tg in
            x86_64*) q="" ;;
            aarch64*) q=qemu-aarch64 ;;
            riscv64*) q=qemu-riscv64 ;;
        esac
        for t in test_runtime_memops test_runtime_strcmp test_mem_routines; do
            "$FID" emit "tests/bootstrap/$t.fd" --target $tg -o "$W/$t.$tg" > "$W/$t.$tg.log" 2>&1 || { echo "FAIL $t ($tg): build"; fail=1; continue; }
            p="$W/$t.$tg"; chmod +x "$p"
            [ $host = windows ] && p=$(MSYS_NO_PATHCONV=1 wsl -e wslpath -a "$(cygpath -w "$p")" 2>/dev/null | tr -d '\r')
            if diff <(lin $q "$p" 2>&1 | tr -d '\r') <(expected "tests/bootstrap/$t.fd") > /dev/null; then
                echo "ok   $t ($tg)"
            else
                echo "FAIL $t ($tg)"; fail=1
            fi
        done
        "$FID" emit tests/tooling/op23_hash_check.fd --target $tg -o "$W/hc.$tg" > "$W/hc.$tg.log" 2>&1 || { echo "FAIL op23_hash_check ($tg): build"; fail=1; continue; }
        p="$W/hc.$tg"; chmod +x "$p"
        [ $host = windows ] && p=$(MSYS_NO_PATHCONV=1 wsl -e wslpath -a "$(cygpath -w "$p")" 2>/dev/null | tr -d '\r')
        hash_check $tg "$p" "lin $q"
    done
fi

rm -rf "$W"
if [ $fail = 0 ]; then echo "PASS op23-memops-bench"; else echo "FAILED op23-memops-bench"; fi
exit $fail
