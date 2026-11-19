# Socket soak (release-roadmap I6)

`scripts/verify/soak_http.py` builds `tests/verify/soak_http_service.fd`
(an HTTP service on a real socket, served with `http_serve_scoped`), starts
it, and loads it from other processes until `--duration` ends:

- its own client threads (`--loadgen builtin`, default): keep-alive
  connections of up to `--per-conn` requests, and a `--new-conn-share` of
  one-request connections, over `GET /`, `GET /item?id=N` and `POST /echo`;
  every status and body is checked;
- or `wrk` / `hey` (`--loadgen wrk|hey`), rerun in `--chunk`-second runs,
  with one body-checking client beside it.

It passes when the service still answers at the end, no request failed, and
the resident memory over the second half grows less than
`--max-growth-mb-per-hour` (default 1). Samples go to
`<out>/soak-http-<backend>.csv`, failures to `<out>/failures.txt`.

    python scripts/verify/soak_http.py --fid release/fid.exe --duration 86400   # I6: 24 hours
    python scripts/verify/soak_http.py --fid release/fid-dev --duration 1800 --loadgen wrk   # nightly

`verify-nightly.yml` (job `soak`) runs the 30-minute wrk run on Linux.
`scripts/verify/soak.py` stays the in-process soak (no socket, VF16).

Connection failures that come from the client running out of local ports
(Windows `WSAEADDRINUSE` 10048, Linux `EADDRNOTAVAIL`) are the load
generator's limit, not the service's: they are counted as "waits for a
client port" and retried, not as failures.

## First runs (Nov 16, Windows 11, a fid built on Nov 15 at 23:01)

Through `fid ccompile` (zig cc), 4 to 8 client threads on the same machine:

| run | requests | connections | failed | memory |
|-----|---------:|------------:|-------:|--------|
| 120 s, mixed kinds, 30% one-request connections (first version of the client) | 284,857 | ~15,000 | 4,396 (all client port exhaustion: WinError 10048) | 11.2 -> 59.9 MB |
| 40 s, keep-alive only, mixed kinds | 219,577 | 239 | 0 | 12.1 -> 49.0 MB |
| 41 s, one request per connection | 15,551 | 15,551 | 0 | 8.2 -> 9.9 MB |
| 30 s, keep-alive, `GET /` only | 185,832 | 176 | 0 | 9.8 -> 24.6 MB |
| 30 s, keep-alive, `GET /item` only | 184,294 | 176 | 0 | 12.9 -> 42.1 MB |
| 30 s, keep-alive, `POST /echo` only | 156,622 | 144 | 0 | 15.0 -> 56.5 MB |

Every request was answered with the right body. Memory grows linearly with
the number of requests, not of connections: about 95 bytes per `GET /`,
160 per `GET /item`, 265 per `POST /echo` (the one with a body), and it does
not level off. So the service leaks per request on a kept-alive
connection; the earlier "flat" reading (known-bugs 2g.35: 6,448 KB after
300 requests, 6,488 KB after 600) is the same rate, about 135 bytes per
request, over too few requests to see it. The 160,000-request figure of
release I6 (5.2 MB) predates the ownership rules of proposal 0004.

The leak was found and fixed on Nov 17: docs/verification/soak-http-leak.md
(a handler's result called through a function value, and `hc_text`'s
`return copy as str`, each kept a second owner of every response or body).
After the fix the soaks are flat: a 30-minute run took 10.1 million
requests with 0 failures at 6.0 MB (second-half slope -0.04 MB/hour). The
24-hour and week-long runs are what is left (soak-http-leak.md, "Left").
