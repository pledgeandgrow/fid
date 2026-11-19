#!/usr/bin/env bash
# stdlib/mysql + stdlib/mysql_db against a real server (competitive SL12), on
# Linux or in WSL, as an ordinary user (no root): a throwaway MariaDB data
# directory from the distribution's packages (mariadb-server: needs
# mariadb-install-db and mariadbd; MySQL's mysqld --initialize-insecure is
# used when MariaDB is absent), on 127.0.0.1:53306, user "fides" with the
# password "fides-pw" (mysql_native_password on MariaDB, caching_sha2 fast
# authentication on MySQL 8 after a first login has cached it), database
# "fides". Runs tests/db/mysql_live.fd and a wrong-password login.
#
#   BIN=<mysql_live built for this machine> bash tests/db/mysql_live.sh
#   FID=<fid for this machine> bash tests/db/mysql_live.sh   # builds it
#
# Exit 0 and "mysql live: all ok" when every run printed what it should.
set -u
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
DIR=$(mktemp -d /tmp/fides-my.XXXXXX)
PORT=${MYT_PORT:-53306}
SOCK="$DIR/mysql.sock"
PID=""
cleanup() {
    [ -n "$PID" ] && kill "$PID" 2>/dev/null && wait "$PID" 2>/dev/null
    rm -rf "$DIR"
}
trap cleanup EXIT

if [ -z "${BIN:-}" ]; then
    if [ -z "${FID:-}" ]; then echo "mysql live: set BIN or FID"; exit 2; fi
    (cd "$ROOT" && "$FID" ccompile tests/db/mysql_live.fd -o "$DIR/mysql_live") || exit 2
    BIN="$DIR/mysql_live"
fi

if command -v mariadb-install-db >/dev/null 2>&1; then
    SERVER=$(command -v mariadbd || echo /usr/sbin/mariadbd)
    CLIENT=$(command -v mariadb || command -v mysql)
    mariadb-install-db --no-defaults --datadir="$DIR/data" --auth-root-authentication-method=normal \
        --skip-test-db >"$DIR/install.log" 2>&1 || { cat "$DIR/install.log"; exit 2; }
elif command -v mysqld >/dev/null 2>&1; then
    SERVER=$(command -v mysqld)
    CLIENT=$(command -v mysql)
    "$SERVER" --no-defaults --initialize-insecure --datadir="$DIR/data" >"$DIR/install.log" 2>&1 \
        || { cat "$DIR/install.log"; exit 2; }
else
    echo "mysql live: no MariaDB or MySQL server (apt install mariadb-server)"; exit 2
fi
"$SERVER" --no-defaults --datadir="$DIR/data" --port="$PORT" --bind-address=127.0.0.1 \
    --socket="$SOCK" --pid-file="$DIR/mysqld.pid" --skip-name-resolve >"$DIR/server.log" 2>&1 &
PID=$!
for i in $(seq 1 60); do
    "$CLIENT" --no-defaults -uroot --socket="$SOCK" -e "SELECT 1" >/dev/null 2>&1 && break
    sleep 0.5
done
"$CLIENT" --no-defaults -uroot --socket="$SOCK" -e "CREATE DATABASE fides; CREATE USER 'fides'@'127.0.0.1' IDENTIFIED BY 'fides-pw'; CREATE USER 'fides'@'localhost' IDENTIFIED BY 'fides-pw'; GRANT ALL ON fides.* TO 'fides'@'127.0.0.1'; GRANT ALL ON fides.* TO 'fides'@'localhost';" \
    || { cat "$DIR/server.log"; exit 2; }

fails=0
out=$(env MYT_PORT="$PORT" "$BIN" 2>&1)
want="connected
created
inserted 3
rows [{\"id\":1,\"name\":\"Ada\",\"note\":null},{\"id\":2,\"name\":\"O'Brien\",\"note\":\"x\"},{\"id\":3,\"name\":\"Robert'); DROP TABLE fides_live;--\",\"note\":null}]
prepared [{\"id\":2,\"name\":\"O'Brien\",\"note\":\"x\"}]
one {\"n\":3}
error 1146 42S02
database 2
ok"
if [ "$out" == "$want" ]; then
    echo "  session: ok"
else
    echo "  session: FAILED"; echo "$out" | sed 's/^/    /'; fails=$((fails + 1))
fi
out=$(env MYT_PORT="$PORT" MYT_PASSWORD=wrong "$BIN" 2>&1)
case "$out" in
    "FAILED: connect: "*1045*) echo "  wrong password refused: ok" ;;
    *) echo "  wrong password: FAILED"; echo "$out" | sed 's/^/    /'; fails=$((fails + 1)) ;;
esac
if [ $fails -eq 0 ]; then echo "mysql live: all ok"; exit 0; fi
echo "mysql live: $fails failed"; exit 1
