#!/bin/bash
# Import every stdlib module on its own: a module that only compiles when
# another program happens to import what it needs fails here.
# usage: scripts/check-stdlib.sh [compiler]   (default: target/release/fidesc)
compiler="${1:-target/release/fidesc}"
dir=$(mktemp -d)
ok=0
bad=0
for file in stdlib/*.fd; do
    name=$(basename "$file" .fd)
    # `use tls` is one of these two, by system
    case "$name" in tls_windows|tls_unix) name=tls ;; esac
    [ -f "$dir/u_$name.fd" ] && continue
    printf 'use %s;\nfn main() -> i64 { return 0; }\n' "$name" > "$dir/u_$name.fd"
    if "$compiler" emit "$dir/u_$name.fd" -o "$dir/u_$name.exe" --no-cache > "$dir/$name.log" 2>&1 \
        && [ -f "$dir/u_$name.exe" ]; then
        ok=$((ok + 1))
    else
        bad=$((bad + 1))
        echo "FAIL $name"
        grep -m3 -E 'rror' "$dir/$name.log"
    fi
done
echo "stdlib: $ok compiled, $bad failed"
[ "$bad" -eq 0 ]
