#!/bin/bash
# build-units.sh — compile the units of a split C file within the runner's
# memory. Sourced by scripts/build-selfhost.sh and scripts/bootstrap.sh once
# they have set CC_CMD, SPLIT_CFLAGS (arrays), JOBS and SPLIT:
#
#   . scripts/build-units.sh
#   if units_compile <base> <log>; then "${CC_CMD[@]}" "${UNITS_OBJS[@]}" -o ...
#   units_summary          the line that says what was built
#
# <base>_0.c .. <base>_$((SPLIT - 1)).c become <base>_N.o; UNITS_OBJS holds
# them in unit order. units_compile returns 0 when every unit is there.
#
#   SPLIT_OPT (default -O2)     the level of the first build of every unit
#   FID_BUILD_TIGHT (auto)      auto: tight when the machine has under 24 GB
#                               of memory (a hosted runner); 1 or 0 forces it
#   FID_BUILD_MEM_MB            the memory to assume instead of the machine's
#                               (16 GB of a hosted runner, to try its path)
#   FID_HEAVY_KB (default 500)  a unit with one function of more KB of C than
#                               this is heavy
#   FID_UNIT_MEM_MB             (tight or not) every compile gets this many MB
#
# Tight mode. LLVM's memory at -O2 grows faster than a function's size, and
# two heavy units together take a 16 GB hosted runner away ("The hosted
# runner lost communication with the server", "The runner has received a
# shutdown signal", exit 143: every Linux and macOS workflow that builds
# fid, from Nov 23 to Nov 27; the Windows runners, which have a page file,
# built it two at a time). So there the heavy units are compiled one at a
# time. On Linux every compile also gets a memory limit (ulimit -v): a unit
# over it fails, and the ladder catches it, instead of the runner dying.
# macOS and Windows get the order only: the limit is not measured there,
# and a limit that bites where it should not would turn a good unit into
# "unbuildable". On Nov 27 the units with a function of 660 KB to 1.4 MB
# were the ones that failed first under memory pressure; unit 0, the largest
# file (13.7 MB), has a 4 MB function and is cheap, but counts as heavy: it
# costs its turn alone, no more.
# A unit that fails is built again alone at SPLIT_OPT, then at -O1, then at
# -O0, with the same flags otherwise: the generated C is the same at every
# level, only the compiler's speed differs.

# The machine's memory in MB, 0 when it cannot be read (never a failure: a
# reader that is missing only means "not tight")
units_mem_total_mb() {
    local v=""
    v=$(units_mem_raw_mb) || v=""
    case "$v" in ''|*[!0-9]*) echo 0 ;; *) echo "$v" ;; esac
    return 0
}

units_mem_raw_mb() {
    case "$(uname -s)" in
        Linux*) awk '/^MemTotal:/ { print int($2 / 1024); exit }' /proc/meminfo 2>/dev/null ;;
        Darwin*) echo $(( $(sysctl -n hw.memsize 2>/dev/null || echo 0) / 1048576 )) ;;
        # Git Bash on the Windows runners: the physical memory (the page
        # file behind it is not what a compile fills first)
        MINGW*|MSYS*) powershell.exe -NoProfile -NonInteractive -Command \
            '[int]((Get-CimInstance Win32_ComputerSystem).TotalPhysicalMemory/1MB)' 2>/dev/null \
            | tr -d '\r' | head -1 ;;
        *) echo 0 ;;
    esac
}

# The largest top-level function of a C unit, in KB. A function starts at a
# line that begins in column 1, has a "(" and no ";" at its end, and ends
# where the next one starts (data at column 1 are counted as functions too:
# that only makes a unit heavier than it is).
units_largest_fn_kb() {
    awk '!/^[ \t#\/{*]/ && /\(/ && !/;[ \t\r]*$/ {
            if (start >= 0 && pos - start > best) best = pos - start
            start = pos
         }
         { pos += length($0) + 1 }
         END {
            if (start >= 0 && pos - start > best) best = pos - start
            printf "%d\n", int(best / 1024)
         }' "$1"
}

# units_compile_one <unit> <level> <limit MB>: in the background by the
# callers (`&`); the limit is a ulimit -v where that exists (none: 0). The
# level replaces SPLIT_OPT in the flags: `${SPLIT_CFLAGS[@]/$SPLIT_OPT}`
# alone would delete it, and a compile with no -O flag is -O0 (the ladder
# ran at -O0 at every level until Nov 27).
units_compile_one() {
    ( if [ "$3" -gt 0 ]; then
          # a shell that cannot set the limit says so and compiles without it
          ulimit -v $(( $3 * 1024 )) 2>/dev/null || echo "      (no memory limit can be set in this shell: unit $1 is compiled without one)"
      fi
      exec "${CC_CMD[@]}" "${SPLIT_CFLAGS[@]/$SPLIT_OPT/$2}" -c "${UNITS_BASE}_$1.c" -o "${UNITS_BASE}_$1.o" ) >> "$UNITS_LOG" 2>&1
}

units_summary() {
    if [ ${#UNITS_RETRIED[@]} -gt 0 ]; then
        echo "      ($SPLIT units: $(( SPLIT - ${#UNITS_RETRIED[@]} )) at $SPLIT_OPT, ${#UNITS_RETRIED[@]} below it: unit ${UNITS_RETRIED[*]})"
    else
        echo "      ($SPLIT units at $SPLIT_OPT)"
    fi
}

units_compile() {
    UNITS_BASE="$1"
    UNITS_LOG="$2"
    UNITS_OBJS=()
    UNITS_RETRIED=()
    SPLIT_OPT="${SPLIT_OPT:--O2}"
    local total tight=0 heavy_kb=500 cap_heavy=0 cap_light=0 k rc can_cap=0
    total="${FID_BUILD_MEM_MB:-$(units_mem_total_mb)}"
    # the limit per compile where it is measured to work: Linux
    case "$(uname -s)" in Linux*) can_cap=1 ;; esac
    case "${FID_BUILD_TIGHT:-auto}" in
        auto) if [ "$total" -gt 0 ] && [ "$total" -lt 24000 ]; then tight=1; fi ;;
        1 | yes | true) tight=1 ;;
    esac
    if [ -n "${FID_HEAVY_KB:-}" ]; then heavy_kb="$FID_HEAVY_KB"; fi
    if [ "$tight" = 1 ]; then
        # the system and the runner agent keep 2.5 GB
        local budget=$(( total - 2500 ))
        if [ "$budget" -lt 1500 ]; then budget=1500; fi
        if [ "$can_cap" = 1 ]; then
            cap_heavy=$budget
            cap_light=$(( budget / JOBS ))
            if [ "$cap_light" -lt 700 ]; then cap_light=700; fi
        fi
    fi
    if [ -n "${FID_UNIT_MEM_MB:-}" ]; then cap_heavy="$FID_UNIT_MEM_MB"; cap_light="$FID_UNIT_MEM_MB"; fi
    local heavy=()
    local light=()
    for k in $(seq 0 $(( SPLIT - 1 ))); do
        UNITS_OBJS+=("${UNITS_BASE}_$k.o")
        if [ "$tight" = 1 ] && [ "$(units_largest_fn_kb "${UNITS_BASE}_$k.c")" -ge "$heavy_kb" ]; then heavy+=("$k"); else light+=("$k"); fi
    done
    if [ "$tight" = 1 ]; then
        local limits="no limit per compile on this system"
        if [ "$cap_heavy" -gt 0 ]; then
            limits="with ${cap_heavy} MB each; the others $JOBS at a time, with ${cap_light} MB each (a unit over its limit fails and goes down the ladder)"
        else
            limits="$limits; the others $JOBS at a time"
        fi
        echo "      memory: ${total} MB; unit ${heavy[*]:-none} have a function over $heavy_kb KB of C and are compiled alone, $limits"
    elif [ "$cap_heavy" -gt 0 ]; then
        echo "      memory: every unit is compiled with a limit of ${cap_heavy} MB"
    fi
    local failed=()
    # 1. the heavy units, one at a time
    for k in ${heavy[@]+"${heavy[@]}"}; do
        units_compile_one "$k" "$SPLIT_OPT" "$cap_heavy"
        rc=$?
        if [ "$rc" -ne 0 ]; then failed+=("$k:alone:$rc"); fi
    done
    # 2. the others, JOBS at a time
    local pids=()
    local pid_units=()
    for k in ${light[@]+"${light[@]}"}; do
        units_compile_one "$k" "$SPLIT_OPT" "$cap_light" &
        pids+=($!)
        pid_units+=("$k")
        if [ ${#pids[@]} -ge "$JOBS" ]; then
            wait "${pids[0]}"
            rc=$?
            if [ "$rc" -ne 0 ]; then failed+=("${pid_units[0]}:shared:$rc"); fi
            pids=("${pids[@]:1}")
            pid_units=("${pid_units[@]:1}")
        fi
    done
    if [ ${#pids[@]} -gt 0 ]; then
        local i
        for i in "${!pids[@]}"; do
            wait "${pids[$i]}"
            rc=$?
            if [ "$rc" -ne 0 ]; then failed+=("${pid_units[$i]}:shared:$rc"); fi
        done
    fi
    # 3. a unit that failed beside another one is built again alone, at
    #    SPLIT_OPT (a compiler killed for memory at JOBS at a time usually
    #    builds by itself); one that failed alone goes down the ladder
    local still=()
    if [ ${#failed[@]} -gt 0 ]; then
        for rc in "${failed[@]}"; do
            echo "      unit ${UNITS_BASE}_${rc%%:*}.c failed at $SPLIT_OPT (exit ${rc##*:})"
        done
        echo "      building the failed units again, one at a time"
        for rc in "${failed[@]}"; do
            k="${rc%%:*}"
            case "$rc" in
                *:alone:*) still+=("$k") ;;
                *)
                    units_compile_one "$k" "$SPLIT_OPT" "$cap_heavy"
                    rc=$?
                    if [ "$rc" -ne 0 ]; then still+=("$k"); fi
                    ;;
            esac
        done
    fi
    # 4. the ladder: -O1, then -O0 (the giant units exceed LLVM's memory at
    #    any level above -O0 on a small machine)
    if [ ${#still[@]} -gt 0 ]; then
        local retry_opts=()
        case "$SPLIT_OPT" in -O2) retry_opts=(-O1 -O0) ;; -O1) retry_opts=(-O0) ;; esac
        local final=()
        local opt ok
        for k in "${still[@]}"; do
            ok=0
            for opt in ${retry_opts[@]+"${retry_opts[@]}"}; do
                echo "      unit ${UNITS_BASE}_$k.c retrying at $opt (the giant-unit memory wall)"
                units_compile_one "$k" "$opt" "$cap_heavy"
                rc=$?
                if [ "$rc" -eq 0 ]; then
                    ok=1
                    UNITS_RETRIED+=("$k$opt")
                    break
                fi
            done
            if [ "$ok" -eq 0 ]; then final+=("$k"); fi
        done
        if [ ${#final[@]} -gt 0 ]; then
            for k in "${final[@]}"; do
                echo "error: unit ${UNITS_BASE}_$k.c unbuildable at $SPLIT_OPT and below (no memory to commit: see the commit limit, FID_UNIT_MEM_MB)"
            done
            return 1
        fi
    fi
    return 0
}
