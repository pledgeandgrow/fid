#!/bin/bash
# SP19 (docs/roadmap/llvm-roadmap.md): `fid check` and the LSP share the
# state of a running `fid serve --port <n>` (compiler/main.fd serve_socket,
# compiler/compiler.fd serve_client_ask / check_text_errors).
# Checks:
#   1. `FID_SERVE=<n> fid check` of a clean file is answered by the server
#      (its log names the request) and says no errors;
#   2. the diagnostics of a file with errors are the same, line for line,
#      as a one-shot `fid check` prints them;
#   3. the second check through the server takes less time than a one-shot
#      check (the stdlib and module parses stay warm in the server);
#   4. a client with a wrong key is refused, and `fid check` then checks by
#      itself (same diagnostics);
#   5. the LSP with FID_SERVE set publishes the diagnostics the LSP alone
#      publishes, and the server's log shows its check-text requests
#      (tests/tooling/serve_lsp_probe.py; skipped without python3).
#   FID=release/fid.exe bash tests/tooling/serve-socket-check.sh
# Run from the repository root.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
PORT=${PORT:-$((47000 + RANDOM % 2000))}
TMP=$(mktemp -d)
cat > "$TMP/clean.fd" <<'EOF2'
fn twice(x: i64) -> i64 {
    return x * 2;
}
fn main() -> i64 {
    println(int_to_str(twice(21)));
    return 0;
}
EOF2
cat > "$TMP/bad.fd" <<'EOF2'
fn main() -> i64 {
    var x: i64 = "text";
    var y: bool = 3;
    return x;
}
EOF2
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
"$FID" serve --port "$PORT" 2> "$TMP/serve.log" &
SRV=$!
trap 'kill $SRV 2>/dev/null; rm -rf "$TMP"' EXIT
KEYDIR="${USERPROFILE:-$HOME}/.fid"
KEY="$KEYDIR/serve-$PORT.key"
for _ in $(seq 1 100); do
    [ -s "$KEY" ] && grep -q "listening" "$TMP/serve.log" && break
    sleep 0.1
done
if [ ! -s "$KEY" ]; then
    bad "the server wrote no key file ($KEY)"; cat "$TMP/serve.log"; exit 1
fi
ms() { date +%s%3N; }
# 1. a clean file through the server
O1=$(FID_SERVE=$PORT "$FID" check "$TMP/clean.fd" 2>&1)
if echo "$O1" | grep -q "no errors (fid serve on port $PORT)" && grep -q "fid serve: check -> 0" "$TMP/serve.log"; then ok "1. clean check answered by the server"; else bad "1. clean check: $O1"; fi
# 2. the same diagnostics as a one-shot check
"$FID" check "$TMP/bad.fd" > "$TMP/one.out" 2> "$TMP/one.err"; S1=$?
FID_SERVE=$PORT "$FID" check "$TMP/bad.fd" > "$TMP/srv.out" 2> "$TMP/srv.err"; S2=$?
grep -v '^\[' "$TMP/one.err" > "$TMP/one.diag"
grep -v '^\[' "$TMP/srv.err" > "$TMP/srv.diag"
if [ $S1 = 1 ] && [ $S2 = 1 ] && [ -s "$TMP/one.diag" ] && cmp -s "$TMP/one.diag" "$TMP/srv.diag" && grep -q "fid serve: check -> 1" "$TMP/serve.log"; then ok "2. diagnostics identical to a one-shot check"; else bad "2. diagnostics differ (status $S1 / $S2)"; diff "$TMP/one.diag" "$TMP/srv.diag" | head -20; fi
# 3. a warm check is faster than a one-shot check
t0=$(ms); "$FID" check "$TMP/clean.fd" > /dev/null 2>&1; t1=$(ms)
FID_SERVE=$PORT "$FID" check "$TMP/clean.fd" > /dev/null 2>&1; t2=$(ms)
one=$((t1 - t0)); warm=$((t2 - t1))
if [ $warm -lt $one ]; then ok "3. second check through the server faster ($warm ms vs $one ms one-shot)"; else bad "3. check through the server not faster ($warm ms vs $one ms)"; fi
# 4. a wrong key is refused; the client falls back to checking itself
cp "$KEY" "$TMP/key.saved"
echo "wrong-key" > "$KEY"
FID_SERVE=$PORT "$FID" check "$TMP/bad.fd" > /dev/null 2> "$TMP/fb.err"; S3=$?
cp "$TMP/key.saved" "$KEY"
grep -v '^\[' "$TMP/fb.err" > "$TMP/fb.diag"
if [ $S3 = 1 ] && cmp -s "$TMP/one.diag" "$TMP/fb.diag"; then ok "4. wrong key refused, client checked by itself"; else bad "4. wrong key: status $S3"; fi
# 5. the LSP through the server
if command -v python3 > /dev/null 2>&1; then
    if python3 tests/tooling/serve_lsp_probe.py "$FID" "$PORT" "$TMP/serve.log"; then ok "5. LSP diagnostics through the server"; else bad "5. LSP diagnostics through the server"; fi
else
    echo "skip 5. no python3"
fi
# stop the server: it removes its key file
printf 'key %s\nstop\n' "$(cat "$KEY")" > "$TMP/stop.req"
python3 - "$PORT" "$TMP/stop.req" <<'EOF2' 2>/dev/null || kill $SRV 2>/dev/null
import socket, sys
s = socket.create_connection(("127.0.0.1", int(sys.argv[1])))
s.sendall(open(sys.argv[2], "rb").read())
s.recv(100)
s.close()
EOF2
wait $SRV 2>/dev/null
if [ ! -e "$KEY" ]; then ok "stop: the server ended and removed its key"; else bad "stop: key file left"; rm -f "$KEY"; fi
if [ $fail = 0 ]; then echo "serve-socket-check: PASS"; else echo "serve-socket-check: FAIL"; echo "--- server log ---"; tail -20 "$TMP/serve.log"; fi
exit $fail
