#!/bin/bash
# tests/wasm/memory-check.sh — XT31: `fid wasm`'s linear memory and the
# allocator under Node (scripts/wasm-run.mjs). Each program is built with
# `fid wasm`, run in an empty folder, and checked: its output
# (tests/wasm/<name>.expected), its exit code, its panic or limit message,
# and the memory at exit (FID_WASM_MEMSTAT=1: WASM memory never shrinks,
# so that is the program's peak) against a ceiling.
#
#   mem_grow_reuse  200 MB held at once in three rounds of four block
#                   sizes: freed pages reused (stdlib/linux_runtime/wasi.fd
#                   free regions), peak under 360 MB (was 1,828 MB)
#   mem_str_free    3 million runtime strings and 30,000 file reads: each
#                   freed (base.fd p_str_give), peak under 32 MB (was 332)
#   mem_oom         a block memory.grow refuses: "out of memory (...)",
#                   exit 1
#   test_alloc_oom_panic  2^40 bytes (tests/bootstrap): the same
#   mem_hog         FID_MEM_LIMIT=4M (tests/limits): exit 125 and the
#                   limit's message
#
#   FID=release/fid.exe bash tests/wasm/memory-check.sh
#   OUT=<dir>   default build/wasm-memory
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) DEF=release/fid.exe ;; *) DEF=release/fid ;; esac
FID="${FID:-$DEF}"
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
OUT="${OUT:-build/wasm-memory}"
mkdir -p "$OUT"
OUT=$(cd "$OUT" && pwd)
pass=0; fail=0

# check <name> <source> <want rc> <max pages> <stderr line or ""> [VAR=value ...]
check() {
  local name="$1" src="$2" want_rc="$3" max_pages="$4" want_err="$5"
  shift 5
  if ! timeout 600 "$FID" wasm "$src" -o "$OUT/$name.wasm" > "$OUT/$name.build.log" 2>&1; then
    echo "BUILD $name: $(grep -a -m1 -i 'error\|not available' "$OUT/$name.build.log" | cut -c1-160)"
    fail=$((fail+1)); return
  fi
  rm -rf "$OUT/run_$name"; mkdir -p "$OUT/run_$name"
  (cd "$OUT/run_$name" && env FID_WASM_MEMSTAT=1 "$@" timeout 300 node "$ROOT/scripts/wasm-run.mjs" "$OUT/$name.wasm" \
      < /dev/null > "$OUT/$name.out" 2> "$OUT/$name.err")
  local rc=$? ok=1 why=""
  [ "$rc" = "$want_rc" ] || { ok=0; why="rc $rc, want $want_rc"; }
  local exp="tests/wasm/$name.expected"
  if [ -f "$exp" ] && ! cmp -s <(tr -d '\r' < "$exp") <(tr -d '\r' < "$OUT/$name.out"); then
    ok=0; why="$why; output differs"
  fi
  if [ -n "$want_err" ]; then
    local got_err
    got_err=$(grep -a -m1 -F "$want_err" "$OUT/$name.err" | tr -d '\r')
    [ -n "$got_err" ] || { ok=0; why="$why; no '$want_err' on stderr"; }
  fi
  local pages
  pages=$(sed -n 's/^\[wasm-run\] memory: \([0-9]*\) pages.*/\1/p' "$OUT/$name.err" | tail -1)
  if [ -z "$pages" ]; then
    ok=0; why="$why; no memory size"
  elif [ "$pages" -gt "$max_pages" ]; then
    ok=0; why="$why; $pages pages, at most $max_pages"
  fi
  if [ $ok = 1 ]; then
    echo "PASS  $name ($pages pages, $((pages / 16)) MB)"; pass=$((pass+1))
  else
    echo "FAIL  $name ($why)"
    [ -f "$exp" ] && diff <(tr -d '\r' < "$exp") <(tr -d '\r' < "$OUT/$name.out") | head -10
    grep -av '^\[wasm-run\]' "$OUT/$name.err" | head -3
    fail=$((fail+1))
  fi
}

check mem_grow_reuse tests/wasm/mem_grow_reuse.fd 0 5760 ""
check mem_str_free tests/wasm/mem_str_free.fd 0 512 ""
check mem_oom tests/wasm/mem_oom.fd 1 512 "Panic: out of memory (4294705152 bytes requested)"
check test_alloc_oom_panic tests/bootstrap/test_alloc_oom_panic.fd 1 512 "Panic: out of memory (1099511627776 bytes requested)"
check mem_hog tests/limits/mem_hog.fd 125 512 "fid: memory limit of 4194304 bytes exceeded (FID_MEM_LIMIT)" FID_MEM_LIMIT=4M
echo "fid wasm memory checks: $pass passed, $fail failed"
[ $fail = 0 ]
