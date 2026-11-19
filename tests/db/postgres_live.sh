#!/usr/bin/env bash
# stdlib/postgres against a real PostgreSQL server (competitive SL12), on
# Linux or in WSL, as an ordinary user (no root): a throwaway cluster of the
# distribution's PostgreSQL in a temporary directory, on 127.0.0.1:55432,
# user "fides" with a SCRAM-SHA-256 password, TLS on with a self-signed
# certificate for "localhost" / 127.0.0.1. Runs tests/db/postgres_live.fd
# four times: sslmode disable (in clear), prefer and require (encrypted,
# the certificate trusted through SSL_CERT_FILE), and require without that
# trust (must be refused: the certificate is checked).
#
#   BIN=<postgres_live built for this machine> bash tests/db/postgres_live.sh
#   FID=<fid for this machine> bash tests/db/postgres_live.sh   # builds it
#
# Exit 0 and "postgres live: all ok" when every run printed what it should.
set -u
PGBIN=${PGBIN:-$(ls -d /usr/lib/postgresql/*/bin 2>/dev/null | sort -V | tail -1)}
if [ -z "$PGBIN" ] || [ ! -x "$PGBIN/initdb" ]; then
    echo "postgres live: no PostgreSQL server binaries (set PGBIN)"; exit 2
fi
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
DIR=$(mktemp -d /tmp/fides-pg.XXXXXX)
PORT=${PGT_PORT:-55432}
cleanup() {
    "$PGBIN/pg_ctl" -D "$DIR/data" -m fast stop >/dev/null 2>&1
    rm -rf "$DIR"
}
trap cleanup EXIT

if [ -z "${BIN:-}" ]; then
    if [ -z "${FID:-}" ]; then echo "postgres live: set BIN or FID"; exit 2; fi
    (cd "$ROOT" && "$FID" ccompile tests/db/postgres_live.fd -o "$DIR/postgres_live") || exit 2
    BIN="$DIR/postgres_live"
fi

echo fides-pw > "$DIR/pw"
"$PGBIN/initdb" -D "$DIR/data" -U fides --auth=scram-sha-256 --pwfile="$DIR/pw" >"$DIR/initdb.log" 2>&1 \
    || { cat "$DIR/initdb.log"; exit 2; }
openssl req -x509 -newkey rsa:2048 -nodes -days 2 -subj /CN=localhost \
    -addext "subjectAltName=DNS:localhost,IP:127.0.0.1" \
    -keyout "$DIR/data/server.key" -out "$DIR/data/server.crt" >/dev/null 2>&1 || exit 2
chmod 600 "$DIR/data/server.key"
cat >> "$DIR/data/postgresql.conf" <<EOF
port = $PORT
listen_addresses = '127.0.0.1'
unix_socket_directories = '$DIR'
ssl = on
EOF
"$PGBIN/pg_ctl" -D "$DIR/data" -l "$DIR/server.log" -w start >/dev/null || { cat "$DIR/server.log"; exit 2; }

fails=0
expect() {  # expect <name> <expected tls> <env...>
    local name=$1 tls=$2; shift 2
    local out
    out=$(env PGT_PORT="$PORT" "$@" "$BIN" 2>&1)
    local want="connected auth SCRAM-SHA-256 tls $tls
server ssl $tls
created
inserted 3
rows [{\"id\":1,\"name\":\"Ada\",\"note\":null},{\"id\":2,\"name\":\"O'Brien\",\"note\":\"x\"},{\"id\":3,\"name\":\"Robert'); DROP TABLE fides_live;--\",\"note\":null}]
one {\"n\":3}
null param [{\"is_null\":true}]
error 42P01
database 2
ok"
    if [ "$out" == "$want" ]; then
        echo "  $name: ok"
    else
        echo "  $name: FAILED"; echo "$out" | sed 's/^/    /'; fails=$((fails + 1))
    fi
}
expect disable false PGT_SSLMODE=disable
expect prefer true PGT_SSLMODE=prefer SSL_CERT_FILE="$DIR/data/server.crt" PGT_HOST=localhost
expect require true PGT_SSLMODE=require SSL_CERT_FILE="$DIR/data/server.crt" PGT_HOST=localhost
# Without trusting the certificate, require must not connect
out=$(env PGT_PORT="$PORT" PGT_SSLMODE=require PGT_HOST=localhost SSL_CERT_FILE=/nonexistent "$BIN" 2>&1)
if [ "$out" == "FAILED: connect: TLS handshake with localhost failed (certificate or name not accepted)" ]; then
    echo "  untrusted certificate refused: ok"
else
    echo "  untrusted certificate: FAILED"; echo "$out" | sed 's/^/    /'; fails=$((fails + 1))
fi
# A wrong password is refused by SCRAM
out=$(env PGT_PORT="$PORT" PGT_PASSWORD=wrong "$BIN" 2>&1)
case "$out" in
    "FAILED: connect: FATAL 28P01: password authentication failed for user \"fides\"") echo "  wrong password refused: ok" ;;
    *) echo "  wrong password: FAILED"; echo "$out" | sed 's/^/    /'; fails=$((fails + 1)) ;;
esac
if [ $fails -eq 0 ]; then echo "postgres live: all ok"; exit 0; fi
echo "postgres live: $fails failed"; exit 1
