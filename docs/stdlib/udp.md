# udp — UDP sockets over IPv4 and IPv6

`stdlib/udp.fd` calls the operating system's socket API directly: Winsock
(ws2_32) on Windows, the C library's BSD sockets on Linux, macOS and
FreeBSD, picked with `@cfg`. Nothing has to be linked by hand, so a program
using it builds:

| Build | |
|-------|---|
| `fid ccompile app.fd` | through C, on Windows, Linux, macOS (and `--target` cross builds) |
| `fid emit app.fd` | natively for Windows (pe_ir imports ws2_32.dll) |
| `fid emit app.fd --target x86_64-linux` | natively for Linux (the C library's functions bound at load time) |

The interpreter (`fid run`) cannot call the socket API (it stops at the
first socket call); the address functions work there too.
(`net.fd`'s older UDP functions need `runtime/pledge_net.c` linked by
hand; new code should use this module.)

## Addresses

```fides
use udp;

val a: UdpAddr = udp_addr("2001:db8::1", 443);   // family UDP_V6
val b: UdpAddr = udp_addr("192.0.2.1", 443);     // family UDP_V4
udp_addr_text(& a)                               // "[2001:db8::1]:443"
```

| Call | |
|------|---|
| `udp_addr(text, port)` | dotted IPv4 or IPv6 (RFC 4291 text: `::`, a dotted tail, brackets, a `%zone`); family 0 when it is neither (`udp_addr_ok`) |
| `udp_addr_any(family, port)` / `udp_addr_loopback(family, port)` | `0.0.0.0` / `::`, `127.0.0.1` / `::1` |
| `udp_addr_text(& a)`, `udp_addr_ip_text(& a)` | `"[::1]:4433"`, `"::1"` (RFC 5952 form; v4-mapped as `::ffff:a.b.c.d`) |
| `udp_addr_same(& a, & b)`, `udp_addr_key(& a)` | compare; bytes naming it (QUIC binds tokens and paths to them) |
| `udp_addr_mapped(& a)` / `udp_addr_unmapped(& a)` | IPv4 to and from `::ffff:a.b.c.d` |

Host names are not resolved here.

## Sockets

| Call | |
|------|---|
| `udp_open(family)` | a socket, -1 on failure; an IPv6 one is dual-stack (IPV6_V6ONLY off) |
| `udp_bind(sock, & a)`, `udp_listen(& a)` | bind (port 0: the system picks; `udp_local_port(sock)` tells) |
| `udp_send(sock, bytes, & to)` | send a datagram on a socket of `to`'s family; `udp_send_on(sock, UDP_V6, bytes, & to)` sends to an IPv4 peer from a dual-stack socket |
| `udp_recv(sock, max, timeout_ms, & mut from)` | one datagram ([] after the timeout); `from` gets the sender (v4-mapped senders as IPv4) |
| `udp_wait(sock, timeout_ms)` | whether a datagram is waiting |
| `udp_close(sock)` | |

Bytes are `Vec<i64>` of 0..255. On Windows, `SIO_UDP_CONNRESET` is turned
off, so an ICMP "port unreachable" for an earlier datagram does not fail
later receives.

## Tests

- `tests/bootstrap/test_stdlib_udp_addr.fd`: address text, every backend
- `tests/net/udp_loopback.fd`: IPv4, IPv6 and a dual-stack socket on the
  loopback interface (C, `emit` for Windows and Linux)
- QUIC over it: [quic.md](quic.md)
