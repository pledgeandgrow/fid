# Idle connections (RT11), Nov 16 2026

Competitive roadmap RT11 replaced the O(n) poll loop with IOCP/AFD on
Windows, epoll on Linux and kqueue on macOS. This page measures it with many
idle connections: `bench/idle/server.fd` (one task per connection, parked on
its socket with no time limit; a line gets back `pong <open connections>`)
and `bench/idle/idle.py` (opens the connections in steps; after each step
the server's resident memory, and the latency of 100 fresh connections and
of 100 pings on idle connections picked at random). `bench/idle/run.sh`
builds the server both ways and runs the steps; `scripts/idle-bench-check.sh`
is the small check; the weekly bench job runs it to 250k on a Linux runner.

Client and server share the machine in every run below, and the client is
Python: a latency includes Python's own time.

## Linux (WSL2, 16 cores, 15 GB), C path

`fid dump-c bench/idle/server.fd` on Windows, gcc 15 -O2 in WSL, descriptor
limit 1048576, 16 addresses x 16 ports.

| held | server count | rss MB | KB/conn | open/s | new p50 ms | new p99 ms | idle p50 ms | idle p99 ms |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1000 | 1001 | 6.6 | 4.74 | 20102 | 0.17 | 0.30 | 0.16 | 0.29 |
| 10000 | 10000 | 47.1 | 4.62 | 18716 | 0.37 | 0.55 | 0.24 | 0.48 |
| 100000 | 100001 | 451.6 | 4.60 | 15574 | 5.33 | 8.93 | 1.87 | 3.88 |
| 300000 | 300001 | 1353.0 | 4.61 | 2513 | 21.40 | 9704.41 | 8.72 | 16.50 |
| 500000 | 500001 | 2248.9 | 4.60 | 1035 | 35.46 | 45.83 | 14.08 | 21.82 |
| 589808 | — | — | — | — | — | — | — | — |

(300k is from a second run with the same build; the other rows from the
first. Both stopped at 589,808.)

- **Memory is flat per connection: 4.6 KB** resident (a task, its touched
  stack pages, the receive buffer), from 1k to 500k.
- **1M was not reached.** Twice the server stopped at 589,808 connections
  with `Panic: task: out of memory` at 2.7 GB resident. Each task's stack is
  an `mmap` of 260 KB with its lowest page `mprotect`ed as a guard: two
  mappings per task, so `vm.max_map_count` (1048576 here) is the limit, not
  memory. A stock Linux has 65530: about 32k tasks. Raise it
  (`sysctl -w vm.max_map_count=4194304`) or allocate stacks so that they do
  not each need two mappings (compiler/codegen_c.fd, `P_CO_STACK`).
- **Latency grows with the number held**: an idle connection's ping is
  0.16 ms at 1k, 1.9 ms at 100k, 14 ms at 500k; opening slows from ~18k/s
  to ~1k/s. The 9.7 s p99 at 300k is one fresh connection retried by
  TCP (SYN timeout) while the accept backlog was full. The cause was the
  C runtime's scheduler, not epoll: every pass (`__p_co_round`) and every
  wait (`__p_co_wait`) walked the list of all tasks, parked ones included.
  Fixed Nov 17: see below.

## Nov 17: parked tasks out of the scheduler's list, guard pages

Two changes to the C runtime (compiler/codegen_c.fd):

- A task parked on the poller with no deadline (epoll, kqueue, IOCP/AFD:
  an idle connection) leaves `__p_cos` at the end of its turn
  (`__p_co_round`) and comes back at its place by creation order when its
  socket is ready (`__p_co_unpark`, called by the poller's wake). A pass
  and a wait now cost the tasks that can run, as RT12 did for sleepers.
  `tests/bootstrap/test_task_park_wake.fd` checks the scheduler still waits
  for parked tasks; `scripts/idle-bench-check.sh` checks the idle ping's
  median at 100k connections against 1k (Linux).
- A task stack's guard page is installed with `MADV_GUARD_INSTALL` on
  Linux 6.13+ (no mapping of its own: one mapping per stack, adjacent
  stacks merge), and `mprotect`ed as before on older kernels and other
  systems. The native Linux runtime (stdlib/linux_runtime/conc.fd,
  `pe_task_stack_new`) does the same.

Same machine and setup as above (WSL2 kernel 6.6.87, gcc 15 -O2, `fid
dump-c` of the Nov 16 compiler with the change applied to its C, 16
addresses x 16 ports, 200 samples per step, `vm.max_map_count` 1048576):

| held | server count | rss MB | KB/conn | open/s | new p50 ms | new p99 ms | idle p50 ms | idle p99 ms |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1000 | 1000 | 6.8 | 4.61 | 18264 | 0.10 | 0.26 | 0.10 | 0.16 |
| 10000 | 10000 | 47.4 | 4.62 | 21037 | 0.10 | 0.35 | 0.10 | 0.41 |
| 100000 | 100000 | 455.6 | 4.64 | 24422 | 0.10 | 0.20 | 0.09 | 0.17 |
| 250000 | 250000 | 1134.9 | 4.64 | 22105 | 0.10 | 0.18 | 0.10 | 0.19 |
| 500000 | 500000 | 2267.9 | 4.64 | 19436 | 0.11 | 0.24 | 0.10 | 0.19 |
| 589808 | — | — | — | — | — | — | — | — |

- **Latency is flat**: an idle ping takes 0.10 ms at 1k and at 500k (was
  14 ms at 500k), a fresh connection 0.11 ms (was 35 ms), and opening stays
  near 20k/s (was ~1k/s at 500k). The same build without the change, same
  run: idle p50 0.93 ms at 50k, 3.98 ms at 100k.
- **Memory per connection: 4.64 KB** resident, flat from 1k to 500k.
- **1M not reached on this kernel**: at 589,808 the server stops with
  `Panic: task: out of memory (no mapping for its stack: memory, or
  vm.max_map_count on Linux)`. WSL2's 6.6 kernel has no
  `MADV_GUARD_INSTALL`, so each stack is still two mappings and 1048576
  mappings hold ~590k tasks. On it, 1M needs
  `sysctl -w vm.max_map_count=2200000` (bench/idle/run.sh now raises it
  when it runs as root or `sudo -n` works, and says how far the run can go
  otherwise); on Linux 6.13+ the stacks need one mapping each. At
  4.64 KB per connection 1M is ~4.6 GB for the server.

## Windows 11 (busy shared machine), both builds

`bench/idle/run.sh`, `STEPS=1000,10000,50000`, 16 ports, 8 source
addresses. Both stop at ~15,790 connections: the client gets WSAENOBUFS
(10055), Windows's dynamic port range (16,384 ports by default) being used
up; the server was still answering. Raising it needs an administrator
(`netsh int ipv4 set dynamicport tcp start=1025 num=64511`).

| build | held | server count | rss MB | KB/conn | open/s | new p50 ms | idle p50 ms | idle p99 ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| ccompile | 1000 | 1000 | 10.3 | 5.30 | 410 | 0.15 | 0.18 | 0.37 |
| ccompile | 10000 | 10000 | 55.8 | 5.19 | 347 | 0.32 | 0.34 | 1.05 |
| ccompile | 15791 | 15791 | 84.5 | 5.15 | 315 | — | 0.22 | 0.55 |
| emit | 1000 | 1000 | 11.0 | 5.55 | 396 | 0.18 | 0.19 | 0.42 |
| emit | 10000 | 10001 | 58.2 | 5.39 | 362 | 0.15 | 0.21 | 0.37 |
| emit | 15790 | 15790 | 87.8 | 5.34 | 348 | — | 0.12 | 0.24 |

- 5.2-5.5 KB per connection on both backends; idle pings stay at ~0.2 ms up
  to 15.8k: no growth with the number held at this size.
- Accepting is slow on Windows, ~350/s against ~18k/s on Linux. The C
  runtime's `p_co_accept` always waits for readiness before it accepts
  (`p_co_wait_fd(fd, 0, -1)` then `p_socket_accept`), one poller round trip
  per connection even when the backlog is full; trying the accept first and
  waiting only when it would block would drain a backlog in one turn. (The
  first version of the server also called `task_wait_read` before each
  accept: two round trips, ~250/s.)

## Left

- 1M on Linux: with `vm.max_map_count` raised (root:
  `STEPS=1000,10000,100000,500000,1000000 bash bench/idle/run.sh`), or on a
  6.13+ kernel where a stack is one mapping.
- The native Linux backend (`TARGET=x86_64-linux BUILDS=native bash
  bench/idle/run.sh`, or `fid emit --target x86_64-linux` on Linux). Not
  measured Nov 17: the Nov 16 compiler's x86_64-linux and aarch64-linux
  executables asked for a dynamic loader (`undefined symbol: nat_tls_set`
  / no `ld-linux-aarch64.so.1`) with the current runtime, so the native run
  waits for a compiler built from this tree.
- The client in a compiled language, or several processes, to tell the
  server's latency growth from the client's.
- macOS (kqueue).
