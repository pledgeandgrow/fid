# VF18: thousands of interleavings, recorded (Nov 18)

competitive-roadmap VF18: sharded tests on thousands of task and thread
interleavings. The harness is `scripts/verify/interleave.py`; the programs
are `tests/verify/interleave.fd` and `tests/bootstrap/test_runtime_sched_random.fd`.

## What changed on Nov 18

- `tests/verify/interleave.fd` gained async work besides its threads
  (atomics, a mutex, a channel pipeline, parallel branches):
  - six green tasks (`task_go`) adding to one atomic with `task_yield`
    between, and their results summed;
  - a task pipeline: one task sends 300 values through a channel, another
    takes them with `sync_channel_try_recv` and yields while it is empty;
  - four `async fn`s started together and awaited in another order
    (each sleeps, adds to an atomic, yields);
  - a `select` over two channels, each fed by a spawned thread, with a
    `default` arm (1000 values, 500 from the first channel).
  Its `// Expected output` block (12 lines) holds every total.
- `interleave.py`:
  - a program with an `// Expected output` block is held to it (the
    reference run and every run), not only to its first run;
  - a failing run is saved as `<out>/<program>-<backend>-seed<N>.out` and
    the summary prints the failing seeds and the command that replays them;
  - `--seeds 17,42` runs only those seeds and prints each one's output;
  - `--record FILE` appends one Markdown row per program and backend (the
    table below was made with it);
  - output is line-buffered (a log shows each result when it is known).
- `tests/verify/interleave_racy.fd`: a lost-update race on purpose; the
  harness must report it.
- `tests/tooling/vf18-interleave-check.sh`: five checks of the above
  (interleave.fd on every backend, `fid run` of it, the racy program
  reported with seeds and a replay command, `--sched-random` on
  test_runtime_sched_random, `--seeds` and `--record`).

## The controlled scheduler

The seeded scheduler the roadmap line asks for is the runtimes' random-choice
mode, `FIDES_SCHED_RANDOM=<seed>` (docs/perf/rl17-sched-random.md): native
runtimes since Nov 16 (`stdlib/pe_runtime/conc.fd`, `stdlib/linux_runtime/conc.fd`,
"Random-choice scheduling"), the C runtime since Nov 17 (`compiler/codegen_c.fd`,
"Roadmap RL17": a random order each scheduler pass, a task gives up its turn
one time in two at atomics, mutexes and channels, threads sleep 0-1 ms one
time in four there). `interleave.py --sched-random` sets it to each run's seed.
Replay: on the C path a program whose tasks all run on one thread repeats its
schedule exactly from the seed (`tests/sched/sched-random-check.sh` checks
seeds 1-20 twice); with OS threads, or on pe_ir (a worker beside main), the
OS still decides part of it, so a seed makes a failure likely to come back.

## Runs

Compiler: the Nov 16 stage 0 (it has neither today's compiler changes nor
the C runtime's random mode), stdlib: HEAD's (Nov 15) with the working tree's
`stdlib/pe_runtime/` (the working tree's core.fd calls `p_region_word`, which
that compiler does not know). Windows 11, 16 logical CPUs, a busy machine
(other sessions' builds). Each run: a random CPU set (1, 2, half or all
CPUs), and half of the runs beside 1-3 busy-loop threads.

```bash
python scripts/verify/interleave.py --fid <stage0> -n 3000 --shards 4 \
  --backends ccompile,emit --record <file>
python scripts/verify/interleave.py --fid <stage0> -n 200 --shards 16 --sched-random \
  --timeout 300 --backends emit \
  --programs tests/bootstrap/test_runtime_sched_random.fd,tests/verify/interleave.fd
```

Every run printed the expected block; one distinct result per row.

| date | compiler | program | backend | runs | shards | mode | result | distinct | time |
|---|---|---|---|---|---|---|---|---|---|
| 2026-11-18 01:24 | stage0.exe | interleave | ccompile | 3000 | 4 | OS scheduler | PASS | 1 | 1314 s |
| 2026-11-18 01:46 | stage0.exe | interleave | emit | 3000 | 4 | OS scheduler | PASS | 1 | 1368 s |
| 2026-11-18 01:51 | stage0.exe | test_runtime_sched_random | emit | 200 | 16 | sched-random | PASS | 1 | 283 s |
| 2026-11-18 02:07 | stage0.exe | interleave | emit | 200 | 16 | sched-random | PASS | 1 | 901 s |

A seeded native run of interleave.fd takes about 60 s (the native random
mode sleeps 1 ms at one atomic in eight, and its threads do 29,000 atomics),
so the seeded runs are 200, not 3000. The racy program
(`tests/verify/interleave_racy.fd`, 12 runs, ccompile and emit) was reported
FAIL on both, 12 distinct totals of 165,582-198,451 of 400,000, with its
failing seeds and a replay command.

## Left

- The runs above are with the Nov 16 compiler; the validation phase repeats
  them with this run's compiler (below), and the C path's `--sched-random`
  has not been run on interleave.fd (the Nov 16 compiler's C runtime has no
  random mode).
- fid emits no loop safepoint (`p_co_tick`), so the runtimes' random
  preemption at loop safepoints never runs in a fid-built program: a task
  switches only at yields, awaits, sleeps, channel and socket waits and the
  random points (a lowering change in compiler/lower.fd, not this track).
- Every switch point enumerated, as CHESS or loom do, is not done: the
  scheduler samples interleavings from a seed, it does not enumerate them.
- The weekly job (`.github/workflows/verify-weekly.yml`, `interleave`) still
  runs ccompile only and does not record its result (`--record`).

## Validation

```bash
python scripts/verify/interleave.py --fid $FID -n 3000 --shards 4 \
  --backends ccompile,emit --record build/interleave/record.md   # rows to add above
python scripts/verify/interleave.py --fid $FID -n 200 --shards 16 --sched-random \
  --timeout 300 --backends ccompile,emit \
  --programs tests/bootstrap/test_runtime_sched_random.fd,tests/verify/interleave.fd
FID=$FID bash tests/tooling/vf18-interleave-check.sh
bash tests/sched/sched-random-check.sh      # FID=$FID: same seed, same schedule (C)
```
