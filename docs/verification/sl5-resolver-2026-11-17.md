# SL5: every address of a name, on every back end (Nov 17 2026)

Competitive roadmap SL5 asks that a connect by name try every address the
name has, IPv6 ones included, and that this be run against a real resolver.

## What changed

`--nat` Windows (the machine layer's PE, `stdlib/linux_runtime/win.fd`)
asked `gethostbyname`, which gives IPv4 addresses only, and connected to the
first one. Now:

- `win.fd` `pe_win_resolve_all` (pseudo system call 1003) asks
  `getaddrinfo` for any family and TCP, as pe_runtime's
  `pert_net_connect_sync` does, and returns up to 16 records
  `[family 2|10][IPv6 scope id][address]` in the order Windows ranks them
  (RFC 6724), then frees the list with `freeaddrinfo`.
- `sys.fd` `pert_net_connect` on Windows calls `pe_win_connect_all`, which
  tries each record until one connects (`pe_connect_sa`, task-aware), with
  the scope id kept for link-local IPv6. `pe_resolve6` on Windows uses the
  same resolver (`pe_win_resolve6`) instead of giving up.
- The two imports are `PE_W_GETADDRINFO` (75) and `PE_W_FREEADDRINFO` (76):
  `compiler/nat_ir.fd` `ng_win_imports` must list
  `ws2_32.dll|getaddrinfo` and `ws2_32.dll|freeaddrinfo` at those indexes
  (after `ntdll.dll|NtSetTimerResolution`). Without them a `--nat` program
  that connects by name reads the TLS index as an import and stops with
  "Invalid memory access ... at 0x2" (seen with the Nov 16 compiler).

## Runs (Nov 17, Nov 16 compiler `stage0`, today's runtime files)

| Back end | Program | Result |
|---|---|---|
| pe_ir (default `fid emit`, Windows) | `tests/net/real_connect.fd` | connected, HTTP status line, `.invalid` gives -1 |
| pe_ir | `tests/bootstrap/test_runtime_connect_retry.fd` | all five lines as expected (`::1: true`) |
| native Linux x86-64 (`emit --target x86_64-linux`, WSL, resolv.conf nameserver 10.255.255.254) | `tests/net/real_connect.fd` | connected, HTTP status line, `.invalid` gives -1 |
| native Linux x86-64 (WSL) | `test_runtime_connect_retry.fd` | all five lines as expected |
| `--nat` Windows | `test_runtime_connect_retry.fd` | `listening: true`, then the crash above: the Nov 16 compiler has no getaddrinfo import. To be rerun with the final compiler. |

The Nov 16 compiler's native Linux builds needed `FID_NAT_OFF=ldyn`: with
load-time binding on, it imported the runtime primitive `nat_tls_set` from
the C library ("undefined symbol: nat_tls_set"), a code generator matter
outside the runtime.

## How to check it again

```bash
FID_HOME=$PWD $FID emit tests/bootstrap/test_runtime_connect_retry.fd --nat -o r.exe && ./r.exe
FID_HOME=$PWD $FID emit tests/net/real_connect.fd --nat -o n.exe && ./n.exe
FID_HOME=$PWD $FID emit tests/net/real_connect.fd -o p.exe && ./p.exe
FID_HOME=$PWD $FID emit tests/net/real_connect.fd --target x86_64-linux -o n_lx && wsl ./n_lx
```

Each prints the "Expected output" lines of its file.
