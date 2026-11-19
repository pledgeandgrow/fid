# Child processes with pipes, and Unix-domain sockets

Two runtime additions of Nov 17 (roadmap L4H `process::Command`, L4B
`UnixStream`).

## `Command::spawn` and `process_spawn_io` (stdlib/process.fd)

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

## Unix-domain sockets (stdlib/unix_socket.fd)

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
