#!/bin/bash
# bench/idle/run.sh — idle connections against a Fides server (competitive
# roadmap RT11: the poller is IOCP/AFD, epoll or kqueue, not an O(n) loop;
# this measures it). Builds bench/idle/server.fd through C (`fid ccompile`)
# and natively (`fid emit`), then for each runs bench/idle/idle.py: steps of
# idle connections, the server's memory and the latency of fresh and idle
# connections at each step. Prints Markdown tables (bench/history.sh can add
# them to docs/benchmarks.md).
#
#   bash bench/idle/run.sh                        # steps 1k,10k,100k,1M
#   STEPS=1000,10000 bash bench/idle/run.sh       # a small run
#   BUILDS="c" bash bench/idle/run.sh             # only the C build
#
# FID (default release/fid, or release/fid.exe), CC for ccompile (default
# zig cc), PORTS/ADDRS/SAMPLES/SRCS passed to idle.py. On Linux the descriptor
# limit is raised as far as the hard limit allows; 1M connections also need
# the sysctls in idle.py's docstring (the client and the server are on one
# machine: about 2M sockets). Exit 1 when a step was not answered.
#
# Linux from a Windows fid (WSL: FID=/mnt/c/.../fid.exe):
#   CBUILD=dumpc   the C build is `fid dump-c` then $HOSTCC (default cc)
#                  here, instead of `fid ccompile` (which would run the
#                  Windows C compiler)
#   TARGET=x86_64-linux   the native build is `fid emit --target $TARGET`
#
# vm.max_map_count: a task's stack is one mapping on Linux 6.13+ (its guard
# installed with MADV_GUARD_INSTALL) and two before (an mprotect'ed guard
# page), so 1M tasks need about 2M mappings on older kernels. When the
# limit is below what the largest step needs, this script raises it as
# root, or through `sudo -n` when that needs no password; otherwise it
# says how far the run can go. MAXMAP=0: leave it alone.
set -u
cd "$(dirname "$0")/../.." || exit 2
FID="${FID:-release/fid}"
[ -x "$FID" ] || FID="release/fid.exe"
export CC="${CC:-zig cc}"
STEPS="${STEPS:-1000,10000,100000,1000000}"
BUILDS="${BUILDS:-c native}"
OUT="${OUT:-build/bench/idle}"
CBUILD="${CBUILD:-ccompile}"
HOSTCC="${HOSTCC:-cc}"
TARGET="${TARGET:-}"
mkdir -p "$OUT"
PY=python3; command -v python3 > /dev/null 2>&1 || PY=python
EXE=""; case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;; esac
ulimit -n "$(ulimit -Hn)" 2> /dev/null || true

# the largest step, and the mappings it needs (two per task, the rest of
# the process: 64k)
max_step=0
for s in ${STEPS//,/ }; do [ "$s" -gt "$max_step" ] && max_step=$s; done
if [ "$(uname -s)" = Linux ] && [ "${MAXMAP:-1}" != 0 ] && [ -r /proc/sys/vm/max_map_count ]; then
    need=$((2 * max_step + 65536))
    have=$(cat /proc/sys/vm/max_map_count)
    if [ "$have" -lt "$need" ]; then
        if [ "$(id -u)" = 0 ]; then
            sysctl -q -w vm.max_map_count="$need" > /dev/null 2>&1
        elif command -v sudo > /dev/null 2>&1; then
            sudo -n sysctl -q -w vm.max_map_count="$need" > /dev/null 2>&1
        fi
        have=$(cat /proc/sys/vm/max_map_count)
        if [ "$have" -lt "$need" ]; then
            echo "(vm.max_map_count is $have: on a kernel before 6.13 the tasks stop near $((have / 2 + have / 16)); raise it with sysctl -w vm.max_map_count=$need for $max_step)"
            echo
        fi
    fi
fi

status=0
echo "### Idle connections ($(date +%F), $(uname -sm), descriptor limit $(ulimit -n))"
echo
for b in $BUILDS; do
    exe="$OUT/server-$b$EXE"
    case "$b" in
        c)
            if [ "$CBUILD" = dumpc ]; then
                { "$FID" dump-c bench/idle/server.fd > "$OUT/server.c" \
                    && $HOSTCC -O2 -w -Wno-int-conversion -Wno-incompatible-pointer-types \
                        -o "$exe" "$OUT/server.c" -lm -lpthread; } > "$OUT/build-$b.log" 2>&1
            else
                "$FID" ccompile bench/idle/server.fd -o "$exe" > "$OUT/build-$b.log" 2>&1
            fi ;;
        native)
            if [ -n "$TARGET" ]; then
                "$FID" emit bench/idle/server.fd --target "$TARGET" -o "$exe" > "$OUT/build-$b.log" 2>&1
                chmod +x "$exe" 2> /dev/null
            else
                "$FID" emit bench/idle/server.fd -o "$exe" > "$OUT/build-$b.log" 2>&1
            fi ;;
        *) echo "unknown build $b"; continue ;;
    esac
    if [ ! -x "$exe" ]; then
        echo "($b: the server did not build, $OUT/build-$b.log)"; echo; status=1; continue
    fi
    # the built server answers its selftest (two pongs, counts 0 and 3)
    if [ "$("$exe" --selftest 2>&1 | tr -d '\r' | paste -sd, -)" != "pong 0,pong 3" ]; then
        echo "($b: the server's selftest failed)"; echo; status=1; continue
    fi
    label="fid ccompile"; [ "$CBUILD" = dumpc ] && label="fid dump-c, $HOSTCC -O2"
    [ "$b" = native ] && label="fid emit${TARGET:+ --target $TARGET}"
    args=(--server "$exe" --steps "$STEPS" --label "$label" --out "$OUT/idle-$b.md")
    [ -n "${PORTS:-}" ] && args+=(--ports "$PORTS")
    [ -n "${ADDRS:-}" ] && args+=(--addrs "$ADDRS")
    [ -n "${SAMPLES:-}" ] && args+=(--samples "$SAMPLES")
    [ -n "${SRCS:-}" ] && args+=(--srcs "$SRCS")
    "$PY" bench/idle/idle.py "${args[@]}" || status=1
    echo
done
exit $status
