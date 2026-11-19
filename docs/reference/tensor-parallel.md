# Tensor parallelism over several ranks

Roadmap P6 ("Multi-rank TP"). The communication builtins of `stdlib/shard.fd`
(`all_reduce`, `broadcast`, `all_gather`, `scatter`, `send_tensor`,
`recv_tensor`) move tensors between ranks in programs built with
`fid ccompile` or `fid emit` (native Windows, and the machine layer's Linux
targets). Each rank is a separate process running the same program
(SPMD). With one rank, the default, every collective returns a copy of its
argument, as it did before.

## Starting the ranks

```fid
use tensor;
use shard;

fn main() {
    val rank = p_tp_init(2) as i64;     // ranks 0 and 1; this one's rank
    val world = p_tp_world_size() as i64;
    ...
}
```

- `p_tp_init(n)` starts ranks 1..n-1 and returns the caller's rank (rank 0
  is the process that was started). `n <= 1` keeps one rank. At most 32
  ranks.
- `FID_TP_WORLD=n` in the environment starts the ranks before `main` runs
  and overrides the program's `n`: `FID_TP_WORLD=4 ./prog`.
- `p_tp_rank()` and `p_tp_world_size()` give the rank and the number of
  ranks.

On Linux and macOS the ranks are forked at the call, so nothing before it
runs twice. On Windows each rank runs the program again from the start; its
output is held back until it reaches `p_tp_init`, but anything else the
program does before that call (reading stdin, writing files) happens once per
rank: call `p_tp_init` first. Start the ranks before starting threads or
tasks. On WebAssembly (WASI, no processes) the program runs on one rank.

## The collectives

Every rank must call the same collectives in the same order. Each goes
through rank 0, which works on the ranks' tensors in rank order, so results
(sums of floating-point numbers included) are the same on every run.

| Call | Result on every rank |
|------|----------------------|
| `all_reduce(t)` | the element-wise sum of every rank's `t` (same shape on all ranks) |
| `broadcast(t)` | rank 0's `t` |
| `all_gather(t)` | every rank's `t` joined along dimension 0, rank 0's first (dimensions past 0 must agree) |
| `scatter(t)` | rank r's share of rank 0's `t` cut along dimension 0: `rows / n` rows, one more for the first `rows % n` ranks |
| `send_tensor(t)` | sends `t` to rank `(r + 1) % n` |
| `recv_tensor(shape)` | the tensor rank `(r - 1) % n` sent; it must have `shape` |

A rank that calls another collective than rank 0, or sends a tensor of
another shape, stops the run with a panic naming both. A rank that exits or
fails while another waits on it stops that one too ("all_reduce on rank 0:
rank 1 is gone"). When `main` ends, rank 0 waits for the other ranks; the run
fails (exit 1) when one of them did, with "tp: rank r exited with status s"
on stderr. `send_tensor` writes into a pipe of 1 MB on Windows and the
system's size elsewhere: in a ring where every rank sends a larger tensor
before receiving, ranks wait for each other for ever (as with unbuffered
sends in MPI). Receive on alternate ranks first in that case.

## Implementation

`compiler/codegen_c.fd` `cg_emit_tp_runtime`: one pipe each way between every
pair of ranks (created before the ranks start; on Windows each child gets only
its own pipe ends, made inheritable for its start alone and named in
`FID_TP_LINKS`, so a rank sees another one's exit as the end of a pipe). A
message is a 12-word header (magic, operation, ndim, len, shape) and the
`len` doubles. The C `main` calls `__p_tp_boot` before the globals are set and
`__p_tp_finish` after `main` returns.

`shard.fd` also has `tp_init(n)`, `tp_rank()` and `tp_world_size()`, the
same as the builtins with `i64` results.

### Native backends (`fid emit`)

`stdlib/pe_runtime/tensor.fd` ("Tensor parallelism over several ranks") has
the same protocol, collectives and messages as the C runtime; it is merged
into native Windows builds (pe_ir) and into every machine-layer build
(nat_ir). Starting the ranks and the links are the OS layer's
(`pe_tp_os_attach`, `pe_tp_os_spawn`, `pe_tp_os_write`, `pe_tp_os_read`,
`pe_tp_os_close`, `pe_tp_os_wait`):

- Windows (`stdlib/pe_runtime/conc.fd`, "Tensor-parallel ranks"): as the C
  runtime's Windows half. Rank 0 runs the program again with
  `CreateProcessA`; one pipe each way between every pair of ranks, a rank's
  own ends made inheritable for its start alone and named in
  `FID_TP_LINKS`. A started rank writes to `NUL` until it reaches
  `p_tp_init`, where it takes rank 0's standard output and error
  (`FID_TP_STDIO`). `FID_TP_RANK`, `FID_TP_WORLD`, `FID_TP_LINKS` and
  `FID_TP_STDIO` are removed from the environment once read.
- Linux AArch64, RISC-V and x86-64, macOS (`stdlib/linux_runtime/sys.fd`,
  "Tensor-parallel ranks"): rank 0 forks at `p_tp_init`, so nothing before
  the call runs twice. Every pair of ranks shares one connected `AF_UNIX`
  stream socket (Linux: abstract namespace; macOS: a file in the temp
  directory, removed at once), since nat_ir's x86-64 system call map has no
  `pipe2`. Once there are ranks, SIGPIPE is ignored: a write to a rank that
  is gone fails with "is gone" instead of killing the writer.
- `FID_TP_WORLD=n` is read at `p_tp_init` (not before `main` as in C): the
  ranks start there in every case, and what `main` printed before is printed
  by rank 0 alone.
- When `main` ends, `pert_tp_exit` makes rank 0 wait for the other ranks
  (exit status 1 and "tp: rank r exited with status s" when one failed); the
  machine layer calls it from `pe_rt_exit` and `pert_exit`, pe_ir from
  `pert_exit` (an explicit exit) and, once its entry point calls it after
  `main`, from `main`'s return (until then rank 0 does not wait for the
  others when `main` returns: tests/tp/check.sh's "rank 0 waits" fails on
  pe_ir).
- One rank: `--nat` Windows (the Linux runtime over kernel32 has no fork)
  and WASI (no sockets, no processes) keep one rank whatever `n` asks.
- A tensor's values (its stored length) travel; its shape is checked
  dimension by dimension, not against the number of values (a native tensor
  made by `from_vec` keeps the Vec's length).

Not covered: the interpreter (`fid run`) runs one rank (`p_tp_init` gives 0,
`p_tp_world_size` 1, every collective a copy). There is no transport across
machines and no GPU collectives.

## Check

`bash tests/tp/check.sh [FID]` builds `tests/tp/multirank.fd` (A x B with A's
columns and B's rows split over the ranks; the all-reduce of the partial
products must equal the product computed on one rank) and runs it on 2 ranks
(`p_tp_init(2)`) and on 3 (`FID_TP_WORLD=3`), then `tests/tp/rank_fail.fd`
(rank 1 panics before the all-reduce: rank 0 must stop with an error, not
hang) and `tests/tp/rank_exit.fd` (rank 1 ends with status 3 after the
collective: rank 0 must print, wait for it and exit with 1). `TP_TARGET`
picks the backend: unset `fid ccompile` (needs `CC`), `native` `fid emit`,
`x86_64-linux`, `aarch64-linux` or `riscv64-linux` `fid emit --target`
(from Windows the programs run in WSL, AArch64 and RISC-V under qemu-user):

```bash
CC="zig cc" bash tests/tp/check.sh release/fid.exe
TP_TARGET=native bash tests/tp/check.sh release/fid.exe
TP_TARGET=x86_64-linux bash tests/tp/check.sh release/fid.exe
```

Nov 17, with the Nov 16 compiler reading today's runtime files: C 5/5,
native Windows 4/5 ("rank 0 waits" needs pe_ir's entry point to call
`pert_tp_exit`); x86_64-linux not run (that compiler's Linux output does
not start: "undefined symbol: nat_tls_set").
