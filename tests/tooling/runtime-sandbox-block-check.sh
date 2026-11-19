#!/bin/bash
# Roadmap Phase 12 "Runtime sandbox": `sandbox { }` revokes capabilities
# while the program runs. compiler/lower.fd (Lowerer_lower_sandbox) puts
# p_sandbox_enter()/p_sandbox_exit() around the block when core defines
# them; the interpreter (compiler/interp.fd) revokes every capability in
# between. Compiled programs call core's no-ops.
#   FID=release/fid.exe bash tests/tooling/runtime-sandbox-block-check.sh
# Run from the repository root. Exit 0 when every check passes.
# Set CC (e.g. CC="zig cc") to also check the C path.
FID=${FID:-release/fid.exe}
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }

# 1. What the checker cannot see is stopped at run time.
P=tests/tooling/sandbox_block_probe.fd
OUT=$(mktemp)
ERR=$(mktemp)
"$FID" run "$P" > "$OUT" 2> "$ERR"
code=$?
if [ "$(tr -d '\r' < "$OUT")" = "before 0" ]; then ok "probe: output up to the sandbox"; else bad "probe: output: $(tr -d '\r' < "$OUT" | tr '\n' '|')"; fi
if [ $code -ne 0 ]; then ok "probe: the run fails"; else bad "probe: the run fails: exit 0"; fi
if grep -qF "Sandbox error: 'p_file_exists' needs capability 'FS', which was revoked at run time" "$ERR"; then ok "probe: sandbox error names the call and the capability"; else bad "probe: no sandbox error"; fi

# 2. Every way out of a sandbox block lifts the revocation (interpreter).
T=tests/bootstrap/test_runtime_sandbox_block.fd
expected=$(printf 'end 7\nearly 10\nlate 2\nloop 8\ntry 21\ntry none\nnested 9\ndone')
got=$("$FID" run "$T" 2>/dev/null | tr -d '\r')
if [ "$got" = "$expected" ]; then ok "block: run, every exit lifts the sandbox"; else bad "block: run: $(echo "$got" | tr '\n' '|')"; fi

# 3. Core defines the entry points (lowering emits the calls only then).
if "$FID" dump-ir "$T" 2>/dev/null | grep -q "Function: p_sandbox_exit"; then ok "block: core's p_sandbox_enter/exit are in the IR"; else bad "block: no p_sandbox_exit in the IR (core lacks p_sandbox_enter/exit: see stdlib/core.fd)"; fi

# 4. Compiled: same output, links (core's no-ops).
if [ -n "$CC" ]; then
    EXE=$(mktemp -u).exe
    if "$FID" ccompile "$T" -o "$EXE" > /dev/null 2>&1 && [ "$("$EXE" | tr -d '\r')" = "$expected" ]; then ok "block: C output"; else bad "block: C build or output"; fi
    rm -f "$EXE"
fi

if [ $fail -ne 0 ]; then
    echo "--- probe stdout ---"; cat "$OUT"
    echo "--- probe stderr ---"; cat "$ERR"
fi
rm -f "$OUT" "$ERR"
exit $fail
