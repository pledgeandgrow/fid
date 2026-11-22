# Runtime notes

Consolidated Nov 22: runtime features and profiling.

## Waits that respect cancellation, and the task chain of a panic

Complaint-roadmap CA7 (with the slot of CA8) and CA9, in the runtimes.

### The token slot

Every task has a token slot, and so does the code outside tasks on each
thread. `p_task_token()` reads it and `p_task_set_token(t)` replaces it,
giving back what it held. A `timeout(ms) { }` block (CA8,
docs/reference/timeout-blocks.md) puts its own token there for the length of
the block. A new task starts with an empty slot, whatever its starter's
holds.

A token is three words: `[cancelled (0/1)][deadline in ms on
p_clock_now_ms's clock, 0 for none][the token the slot held before]`. A
token counts as cancelled when its first word is set, when its deadline has
passed, or when either holds for a token further down the chain (word 2).
So an inner block also stops at an outer block's deadline.

| Runtime | Slot of a task | Slot outside tasks |
|---|---|---|
| C (compiler/codegen_c.fd) | `__p_co.tok` | `__p_tok_outside` (thread-local) |
| pe_ir (stdlib/pe_runtime/conc.fd) | word [30] of the task block | TLS slot `pert_tls_tok`, made on first use |
| machine layer (stdlib/linux_runtime/conc.fd) | word [28] of the task block | word [10] of the thread block (base.fd `pe_tls`) |
| interpreter (compiler/interp.fd) | `ITask.token` | `g_itask_tok_outside` |

The interpreter never passes these calls on to the runtime it runs on. That
runtime's slot belongs to the interpreter itself.

### The waits

When the slot holds a token, the builtin waits wait in slices: at most
10 ms, and never past the first deadline on the chain. Between slices they
check the token and give up once it is cancelled:

| Wait | Result when the token stops it |
|---|---|
| `p_co_sleep(ms)` (`task_sleep`) | returns 1 (0: the whole time passed) |
| `await f` (`p_future_poll` of a task) | returns 0; the awaited task goes on, and nothing waits for it any more |
| `p_chan_recv` (`sync_channel_recv`) | the pending value, as for a closed, empty channel |
| `p_chan_send` (`sync_channel_send`) | false, as for a closed channel |
| `p_co_wait_fd` (`task_wait_read/_write`, `p_co_recv`, `p_co_accept`) | 0, not ready |

With an empty slot every wait behaves as it did before: one load and one
test.

While a wait sleeps in slices, the other tasks keep running. A task gives
up its turn. Code outside tasks runs the tasks queued on its thread in the
C runtime and the machine layer, and sleeps in pe_ir, where workers run the
tasks.

A cancelled `await` gives the zero word. That word means nothing for a
future whose result is a string, a struct or a Vec. Check `timeout`'s `else`
(or the token) before using such a result.

Thread joins (`p_thread_join`, or awaiting a `spawn` that runs on an OS
thread) still wait to the end.

The library forms in stdlib/task.fd ("Cancellation tokens":
`task_sleep_cancellable` and the others) take a token as an argument and
work the same way.

Tests: `tests/bootstrap/test_task_wait_cancel.fd`, which runs on every
backend; `tests/cancel/send_full.fd`, compiled backends only, because the
interpreter's channels have no capacity; and `tests/cancel/check.sh`, which
runs both on each backend.

### The task chain of a panic (CA9)

After a panic's trace, the runtime prints the tasks the panicking one was
running for:

    Panic: b failed
    ...
    task chain: b <- a <- main

To build the chain, it starts at the running task and takes the task that
awaits it. If no task awaits it, it takes the task whose turn it runs in, as
with an `async` call that has not suspended yet. It stops at the code
outside tasks: `main`, or `(thread)` off the main thread (the C runtime and
the machine layer). If neither task exists, it ends with `(started by f)`,
the task that started it. A trailing `__task` (an adapted task function) is
left out of the names.

| Runtime | Records | Prints |
|---|---|---|
| C | `__p_co` name, waiter, parent_name | `__p_co_print_chain`, through `__p_trace_tasks` |
| machine layer | task block [25] name, [26] waiter, [27] starter's name (1: a thread), [29] the task it runs inside | `pe_co_print_chain(2)` from base.fd `pert_panic` and `pe_on_signal` |
| pe_ir | task block [31] name, [32] starter's name, [33] waiter; [15] the task it runs inside | `pert_task_chain()`, called by pe_ir's `p_panic` stub (compiler/pe_ir_stubs.fd `PeirGen_gen_panic`) |

Test: `tests/bootstrap/test_task_chain_fail.fd`. The C path is checked by
tests/memprof/check.sh and the native paths by tests/cancel/check.sh.

## Child processes with pipes, and Unix-domain sockets

Two runtime additions of Nov 17 (roadmap L4H `process::Command`, L4B
`UnixStream`).

### `Command::spawn` and `process_spawn_io` (stdlib/process.fd)

`Command::spawn()` starts the program directly, without `sh` or `cmd.exe`:
every argument reaches it as one word whatever it holds, and `%NAME%` is not
expanded. It returns at once with a `Child`:

```fid
use process;

var c: Command = Command::new("sort");
c.pipe_stdin();
c.pipe_stdout();
match c.spawn() {
    Option::Some(ch) => {
        var child: Child = ch;
        child.write("pear\napple\n");
        child.close_stdin();
        val text: str = child.read_all_stdout();   // "apple\npear\n"
        val code: i64 = child.wait();              // 0
        child.release();
    }
    Option::None => { }
}
```

- `pipe_stdin` / `pipe_stdout` / `pipe_stderr` choose the streams that are
  pipes to this process; the others are this process's own.
  `stderr_to_stdout()` sends stderr where stdout goes. `stdin(data)` writes
  the data at the start and then closes the input (unless `pipe_stdin`).
  `env`, `env_remove` and `cwd` apply; `pipe(&next)` stages do not (a
  pipeline runs through the shell: `output()`), and `spawn` returns None.
- `Child`: `write`, `close_stdin`, `read_stdout` / `read_stderr` (the next
  chunk, at most 64 KB, None at the end), `read_all_stdout` /
  `read_all_stderr`, `wait` (closes stdin first), `wait_timeout`, `kill`,
  `id`, `release` (closes the pipes still open).
- Under it: `process_spawn_io(argv, env, dir, io)` with `PROC_PIPE_STDIN |
  PROC_PIPE_STDOUT | PROC_PIPE_STDERR | PROC_STDERR_TO_STDOUT`,
  `process_write`, `process_read(h, PROC_STDOUT | PROC_STDERR)`,
  `process_read_all`, `process_close_stdin`, and AD4's `process_wait`,
  `process_kill`, `process_release`.
- Reading one pipe to its end while the child fills the other can block
  both: read the two in turn or merge them.

The runtimes give `fid_proc_spawn_io`, `fid_proc_write`, `fid_proc_read` and
`fid_proc_close_in` (declared `extern "C"` in process.fd):

| Runtime | Pipes |
|---|---|
| C (compiler/codegen_c.fd `__p_child_spawn`) | `CreatePipe` + `STARTF_USESTDHANDLES` on Windows; `pipe` + `dup2` between `fork` and `execvp` on POSIX (our ends close-on-exec; SIGPIPE ignored during a write). WASI: none. |
| native Windows (stdlib/pe_runtime/base.fd `pe_px_spawn`) | `CreatePipe`, `SetHandleInformation`, `ReadFile` / `WriteFile`. |
| machine layer Linux / macOS (stdlib/linux_runtime/sys.fd `pe_px_spawn`) | a connected AF_UNIX stream pair per stream (`pe_px_pairs`; nat_ir's x86-64 system call map has no `pipe2`/`dup3`), placed at descriptor 0/1/2 with `fcntl(F_DUPFD)` in the child before it execs `/bin/sh`'s launcher. |
| `--nat` Windows | not yet: a request for a pipe gives no child (needs `CreatePipe` and `SetHandleInformation` in nat_ir's `ng_win_imports`). |
| `fid run` (compiler/interp.fd "Pipes") | no live pipes: the child starts at its first read, wait, kill or pid, with what was written before as its input file; reads hand over its output once it has ended. |

Test: `tests/bootstrap/test_process_pipes.fd`.

### Unix-domain sockets (stdlib/unix_socket.fd)

`UnixListener::bind(path)`, `accept`, `close` (removes the path) and
`UnixStream::connect(path)`, `write_str`, `read_str`, `close`, over the
builtins `p_unix_listen(path)` and `p_unix_connect(path)` (-1 on failure)
and the TCP socket builtins, which work on any stream socket.

| Runtime | |
|---|---|
| C | `socket(AF_UNIX, SOCK_STREAM)`; Windows 10 1803+ through ws2_32 (SOCKADDR_UN). |
| native Windows (pe_runtime/conc.fd `pert_unix_listen`) | ws2_32 AF_UNIX. |
| machine layer (linux_runtime/sys.fd `pert_unix_listen`) | Linux; macOS through darwin.fd's sockaddr translation; `--nat` Windows through win.fd. |
| `fid run` (interp.fd `iux_*`) | simulated inside the one process, like its other sockets: no file at the path, no other process can connect. |

Test: `tests/bootstrap/test_unix_socket.fd` (run it from a directory on a
local file system: a WSL `/mnt/c` directory may not hold socket files).

## Memory flame graphs

`fid run <file.fd> --mem` (and `fid ccompile ... --mem`) prints a memory
profile at exit: see runtime-reports.md, "Memory and allocation profile".
If the environment variable `FID_MEM_FOLDED` names a file, the program also
writes there the bytes allocated under each whole stack of Fides functions,
as collapsed stacks, one line per stack, outermost function first:

```
main;keep 512
main;fill 500
main;greet;p_str_alloc 48
```

Flame graph tools read this format as it is:

```bash
FID_MEM_FOLDED=mem.folded fid run app.fd --mem
flamegraph.pl --countname=bytes --title="bytes allocated" mem.folded > mem.svg
# or: inferno-flamegraph < mem.folded > mem.svg; or open it in speedscope
```

A width is the bytes asked for, not the bytes live at exit. Up to 32 Fides
frames are kept per stack (the innermost ones in deeper recursion); a stack
without any Fides frame is `(no Fides frame)`; the table holds 3072 stacks,
and bytes from further stacks are one `(other stacks)` line. Without
`FID_MEM_FOLDED` nothing is written and the stack is not walked a second
time. The code is `__p_ms_note` / `__p_ms_write` in `compiler/codegen_c.fd`
(under `P_MEMPROF`); `tests/memprof/check_folded.sh` checks it.

The CPU profile has the same output: `fid profile` (competitive TL7).
