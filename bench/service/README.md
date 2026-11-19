# Service benchmarks (competitive roadmap BM5)

TechEmpower-style HTTP tests: the same small service written in Fides, Go
and Node, driven by a load client, requests per second for each test type.

| Test | Request | Reply |
|---|---|---|
| plaintext | `GET /plaintext` | `Hello, World!` |
| json | `GET /json` | `{"message":"Hello, World!"}` |
| db | `GET /db` | one random row of the World table, `{"id":N,"randomNumber":M}` |
| queries | `GET /queries?queries=20` | 20 random rows, a JSON array (`queries` clamped to 1..500, a non-number is 1) |

| Server | Source | Built with |
|---|---|---|
| fides | `fides/server.fd`, `http_serve_scoped` | `fid emit` (native; the machine layer on Linux) |
| fides-c | the same | `fid ccompile` |
| go | `go/server.go`, `net/http` | `go build` |
| node | `node/server.mjs`, `node:http` | - |

## Running

```bash
bash bench/service/run.sh                        # every server, here, in turn
SERVERS="fides go" DURATION=30 bash bench/service/run.sh
# client on another machine (what TechEmpower does):
bash bench/service/run.sh serve fides 8080                  # on the server
HOST=server-ip PORT=8080 bash bench/service/run.sh load fides  # on the client
```

The load client is `wrk` when installed (`LOADGEN=wrk`), else `load.py`
(`LOADGEN=py`): Python's asyncio, keep-alive connections, every reply's
status and body checked. Before measuring, each test's reply is checked once
by `load.py` (wrk does not read bodies), so a server that answers wrong gets
`WRONG`, not a number. `CONNECTIONS` (64), `DURATION` (10 s), `WARMUP`
(2 s), `THREADS` (wrk, 4), `TESTS`. Every run's numbers go to
`bench/out/service/results.jsonl` (with p50/p99 latency under `load.py`).

`scripts/service-bench-check.sh` checks that every server's `--selftest`
gives the same bodies (routes, table and row picker agree) and that a short
run answers every test right. `.github/workflows/verify-weekly.yml` runs both
on Linux under wrk.

## How this differs from TechEmpower

- **No database server.** The World table (10,000 rows, row i's
  `randomNumber` is `(i * 7919 + 13) % 10000 + 1`) is held in each server's
  memory; TechEmpower reads it from PostgreSQL over the network. The db and
  queries tests here measure routing, row lookup and JSON building, not a
  database driver. Rows are picked by the same 31-bit LCG in every server
  (seed 42), so the client checks every row against the table.
- **Same machine by default.** `run.sh` without `serve`/`load` runs the
  client beside the server, and its footer says so; the client then takes
  CPU from the server. With `load.py` as the client, the client is the limit
  at a few ten thousand requests a second (the Nov 16 run below).
- No `Date` header, no fortunes or updates tests, no pipelining.

## Results

`docs/benchmarks.md` keeps the tables (Nov 16, 2026: Fides via C, Go and
Node on a busy Windows machine, client `load.py` beside the server, so the
client was the limit). On Windows the native Fides server does not build yet:
`pe_ir` lacks `p_signal_check` / `p_signal_register`, which
`stdlib/http.fd` calls.
