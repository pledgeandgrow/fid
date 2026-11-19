# The keep-alive leak of the socket soak (VF16, release I6)

`docs/verification/soak-http.md` measured, on Nov 16, memory that grew with
the number of requests on kept-alive connections: about 95 bytes per
`GET /`, 160 per `GET /item`, 265 per `POST /echo`. This page is what it
was and how it was checked (Nov 17).

## Finding it without a socket

`tests/verify/http_keepalive_leak.fd` runs the real connection task
(`stdlib/http.fd` `http_conn_task`, with `http_serve_scoped`'s dispatch)
in one process: the connection's TLS hooks (`http_set_tls`) hand it the
requests of `tests/verify/soak_http_service.fd` from memory and take its
responses, so `fid run --leaks` reports what a request leaves behind.
Before the fix, 900 requests left 1,212 blocks:

| blocks | allocated in | what |
|-------:|--------------|------|
| 300 × 3 | `build_response_headers <- build_response <- soak_route <- http_scoped_dispatch` | every response |
| 300 | `p_str_buf <- hc_text <- hc_take` | every POST body |
| 12 | `main` | the function values globals hold (one-time) |

## The two causes

Both are a value handed over without being taken as a temporary, so the
place that kept it took a second owner (proposal 0004: "keeping a value
takes an owner: a temporary is handed over, anything else gets
`Str_dup`"), and one owner was never dropped:

1. **A call through a function value** (`val h: fn(..) -> str = ...;
   return h(req)`) is lowered as `CallPtr`, and the lowering notes only
   direct calls' results as temporaries (`Lowerer_rc_note_call`). So
   `return h(sreq)` in `http_scoped_dispatch` duplicated the response the
   handler had just made: one count was dropped after the write, one
   stayed. The same in `http_health_handler`, in `http_conn_task`'s
   `response = handler(req)` (the `http_serve` path), in `router_dispatch`,
   `router_dispatch_request`, and in http2's `h2_run_handlers`.
2. **`return copy as str`** in `hc_text`: a cast is no temporary either,
   so every string the connection read (heads, bodies, chunk lines) was
   returned with two owners. The head was freed by hand
   (`hs_free(head)`), which hid it there and left the variable's drop
   reading a freed header; a body kept one owner forever.

## The fix (stdlib only)

- A handler's result goes through `p_str_adopt(h(req) as i64)`: as a raw
  word it has the one owner the handler gave it, and `p_str_adopt` hands
  that over (a counted string or a literal as it is). This stays right
  once the lowering takes `CallPtr` results as temporaries: `as i64` then
  claims the temporary, as it does for direct calls.
- `hc_text` returns `p_str_adopt(copy)`; `http_read_message` no longer
  frees `head` by hand (its variable drops it).

After it, the report is the same for 90 and for 900 requests (12 blocks,
the one-time function values; 13 with `HTTP_KA_PLAIN=1`, the `http_serve`
path), and both soaks are flat (fid of Nov 16 with this stdlib, Windows 11,
zig cc):

| run | requests | connections | failed | memory |
|-----|---------:|------------:|-------:|--------|
| `soak_http.py --duration 90 --clients 4 --new-conn-share 0` | 396,127 | 7,783 | 0 | 6.3 -> 6.3 MB, slope +0.07 MB/h |
| `soak_http.py --duration 120` (defaults: 8 clients, 10% one-request connections) | 496,027 | 10,859 | 0 | 6.4 -> 6.4 MB, slope -0.00 MB/h |
| `soak.py --duration 90` (in-process service, 862 heartbeats) | 8,620,000 iterations | - | - | 4.9 MB at first, peak 6.0, slope +0.00 MB/h |

Before: 40 s of keep-alive load took the same service from 12.1 to 49.0 MB.

## The 30-minute run (release I6, Nov 17 afternoon)

Same fid (Nov 16) with the stdlib of Nov 17 16:15 (after the TLS/ALPN,
QUIC and `trace` span changes to `http.fd`), Windows 11, zig cc, the
default load (8 client threads, 10% one-request connections, `GET /`,
`GET /item`, `POST /echo`, every body checked), on a machine busy with
other compiler builds:

    python scripts/verify/soak_http.py --fid <fid> --duration 1800 --port 8297

| requests | connections | failed | rate | memory |
|---------:|------------:|-------:|-----:|--------|
| 10,110,046 | 221,738 | 0 | 5,617/s | 6.0 MB at the first sample, 6.0 at the last, peak 6.1; second-half slope -0.04 MB/hour |

`PASS`. Around minute 12 three of the eight Python client threads ended
with `MemoryError` (the machine's commit limit, reached by the other
builds, not by the service: it stayed at 6.0 MB); the other five carried
the load to the end, so the rate above is a lower bound. Before the fix
this service grew about 1 MB every 5,000 requests: 10 million requests
would have been about 2 GB.

The in-process check with the same stdlib, `fid run
tests/verify/http_keepalive_leak.fd --leaks`, reports the same blocks for
90 and for 900 requests (33 blocks / 872 bytes; 34 / 880 with
`HTTP_KA_PLAIN=1`): all allocated once in `main` (the globals the stdlib
modules it imports now hold, and the function values), none in the request
path; `--checked-alloc` is clean.

## The 24-hour run

Started Nov 17 at 16:46 (+01:00) on the same machine, same build, two
client threads, port 8298, output in `build/soak-http-24h/` (gitignored):
`run.log` gets the summary line and `PASS`/`FAIL` when it ends (Nov 18
around 16:46), `soak-http-ccompile.csv` a sample every 10 s
(`seconds,rss_mb,ok,failed,external_requests`), `started.txt` how it was
started. First samples: 5.8 MB, about 6,000 requests a second.

    python scripts/verify/soak_http.py --fid <fid> --duration 86400 --port 8298 --clients 2 --out build/soak-http-24h

It passes when `run.log` ends in `PASS` with 0 failed and a second-half
slope under 1 MB/hour. Check also that the `ok` column of the csv keeps
growing to the end (client threads that die of the machine's memory
limit, as above, stop the load without counting a failure).

## Left

- The 24-hour run of release I6 is running (above); repeat it with the
  release compiler of this tree (`--fid release/fid.exe`) once it is built.
  The week-long run of VF16:
  `python scripts/verify/soak_http.py --fid release/fid.exe --duration 604800`,
  on a machine left running that long.
- `scripts/verify/soak_http.py`: a client thread that raises anything but
  the errors it expects (here `MemoryError`) ends silently; it should count
  it, restart the thread, and fail the run if the request count stops
  growing.
- In the compiler (not stdlib): note a `CallPtr` result of a string, Vec,
  map or Box type as a temporary, as direct calls are; then the
  `p_str_adopt(.. as i64)` wrappers are no longer needed (they stay
  correct). Function values without captures could be static instead of an
  8-byte block each that is never freed (the 12 one-time blocks above).
