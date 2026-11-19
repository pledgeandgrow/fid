#!/usr/bin/env bash
# CA10 (complaint roadmap): the effect system knows which calls block. A
# green task (p_co_go / task_go / task_scope_go / task_timeout /
# async_spawn) whose code reaches a call that blocks its worker thread
# (p_read_line, p_chan_recv, p_mutex_lock, p_thread_join, p_system, ...)
# gets a `fid lint` warning naming the task, the call and the function it
# is in; the same call run on the blocking pool (task_pool_call) does not.
#
# usage: tests/tooling/ca10-blocking-check.sh [fid]   (default:
# release/fid.exe, run from the repository root)
set -u
FID="${1:-release/fid.exe}"
fail=0
raw=$("$FID" lint tests/tooling/ca10/blocking_raw.fd 2>&1)
pool=$("$FID" lint tests/tooling/ca10/blocking_pool.fd 2>&1)
check() { # check <label> <text> <fixed string> <want: 1 present, 0 absent>
    if printf '%s\n' "$2" | grep -qF -- "$3"; then got=1; else got=0; fi
    if [ "$got" = "$4" ]; then
        echo "ok    $1"
    else
        echo "FAIL  $1"
        printf '%s\n' "$2" | sed 's/^/      /' | head -20
        fail=1
    fi
}
check "raw: p_read_line through a helper warns" "$raw" "task 'worker' calls 'p_read_line' (in 'wait_for_input', which it calls), which blocks the worker thread" 1
check "raw: p_chan_recv in the task warns" "$raw" "task 'worker' calls 'p_chan_recv', which blocks the worker thread" 1
check "pool: no blocking warning" "$pool" "blocks the worker thread" 0
exit $fail
