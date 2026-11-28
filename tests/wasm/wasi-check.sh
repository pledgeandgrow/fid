#!/bin/bash
# tests/wasm/wasi-check.sh — XT32: the WASI preview 1 calls of `fid wasm`'s
# runtime (stdlib/linux_runtime/wasi.fd), one program per group, each built
# with `fid wasm` and run in an empty folder (the preopened "." and "/";
# the host's temporary folder is /tmp) under Node (scripts/wasm-run.mjs),
# and under wasmtime as well when it is on PATH (WASMTIME=0: not). Checked:
# the output (tests/wasm/<name>.expected), the exit code, and that the
# folder is empty again afterwards.
#
#   wasi_files         path_open, fd_read/write/seek/close,
#                      path_filestat_get, path_rename, path_unlink_file,
#                      a file's pages (mmap of a descriptor), /tmp
#   wasi_dirs          path_create_directory, fd_readdir,
#                      path_remove_directory, walks and globs
#   wasi_clock_random  clock_time_get, poll_oneoff (a sleep), random_get,
#                      sysconf / sysinfo
#   wasi_args_env      args_get, environ_get, proc_exit (exit code 7)
#   wasi_stdin         fd_read on standard input
#
#   FID=release/fid.exe bash tests/wasm/wasi-check.sh
#   OUT=<dir>   default build/wasm-wasi
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) DEF=release/fid.exe ;; *) DEF=release/fid ;; esac
FID="${FID:-$DEF}"
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
OUT="${OUT:-build/wasm-wasi}"
mkdir -p "$OUT"
OUT=$(cd "$OUT" && pwd)
HOSTTMP="${TMPDIR:-${TMP:-/tmp}}"
WT=""
if [ "${WASMTIME:-1}" != 0 ] && command -v wasmtime > /dev/null 2>&1; then WT=wasmtime; fi
pass=0; fail=0

# one <runner> <name> <want rc> <stdin text> <args...> (env from ENVS)
one() {
  local runner="$1" name="$2" want_rc="$3" input="$4"
  shift 4
  local tag="$name.$runner" dir="$OUT/run_$name.$runner"
  rm -rf "$dir"; mkdir -p "$dir"
  local rc
  if [ "$runner" = node ]; then
    (cd "$dir" && printf '%b' "$input" | env "${ENVS[@]}" timeout 120 node "$ROOT/scripts/wasm-run.mjs" "$OUT/$name.wasm" "$@" \
        > "$OUT/$tag.out" 2> "$OUT/$tag.err")
    rc=$?
  else
    local wenv=() e
    for e in "${ENVS[@]}"; do wenv+=(--env "$e"); done
    (cd "$dir" && printf '%b' "$input" | timeout 120 wasmtime run --dir .::. --dir .::/ --dir "$HOSTTMP::/tmp" "${wenv[@]}" \
        "$OUT/$name.wasm" "$@" > "$OUT/$tag.out" 2> "$OUT/$tag.err")
    rc=$?
  fi
  local ok=1 why=""
  [ "$rc" = "$want_rc" ] || { ok=0; why="rc $rc, want $want_rc"; }
  cmp -s <(tr -d '\r' < "tests/wasm/$name.expected") <(tr -d '\r' < "$OUT/$tag.out") || { ok=0; why="$why; output differs"; }
  [ -z "$(ls -A "$dir")" ] || { ok=0; why="$why; left in its folder: $(ls -A "$dir" | tr '\n' ' ')"; }
  if [ $ok = 1 ]; then
    echo "PASS  $name ($runner)"; pass=$((pass+1))
  else
    echo "FAIL  $name ($runner: $why)"
    diff <(tr -d '\r' < "tests/wasm/$name.expected") <(tr -d '\r' < "$OUT/$tag.out") | head -10
    head -3 "$OUT/$tag.err"
    fail=$((fail+1))
  fi
}

# check <name> <want rc> <stdin text> <args...>
check() {
  local name="$1"
  if ! timeout 600 "$FID" wasm "tests/wasm/$name.fd" -o "$OUT/$name.wasm" > "$OUT/$name.build.log" 2>&1; then
    echo "BUILD $name: $(grep -a -m1 -i 'error\|not available' "$OUT/$name.build.log" | cut -c1-160)"
    fail=$((fail+1)); return
  fi
  one node "$@"
  if [ -n "$WT" ]; then one wasmtime "$@"; fi
}

ENVS=(XT32_NONE=1)
check wasi_files 0 ""
check wasi_dirs 0 ""
check wasi_clock_random 0 ""
check wasi_stdin 0 'one\n  two  \nthree'
ENVS=(XT32_VAR=value-1 XT32_EQ=a=b=c)
check wasi_args_env 7 "" alpha "b c" ""
[ -n "$WT" ] || echo "(wasmtime not on PATH: Node only)"
echo "fid wasm WASI checks: $pass passed, $fail failed"
[ $fail = 0 ]
