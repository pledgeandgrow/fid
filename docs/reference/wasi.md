# `fid wasm` and WASI preview 1

`fid wasm` (compiler/wasm.fd) builds a WebAssembly module that imports
WASI preview 1 (`wasi_snapshot_preview1`) and nothing else. Its runtime is
the Linux runtime (stdlib/linux_runtime/), unchanged: every builtin makes
Linux system calls (asm-generic numbers, `nat_syscall`), and
stdlib/linux_runtime/wasi.fd (`pe_wasi_syscall`) does each with WASI. This
page lists, for each group of builtins the standard library uses, the
system calls they make and the WASI functions those become (XT32).

Run a module with `node scripts/wasm-run.mjs prog.wasm [args...]`: the
current folder is the guest's `.` and `/`, the host's temporary folder its
`/tmp`, and the arguments and environment are the process's. wasmtime or
any WASI host works the same way (preopen `.`; `/` and `/tmp` as wanted).
On Windows, Node's WASI has no `fd_readdir`: scripts/wasm-run.mjs lists the
folder itself, so listings are checked against WASI itself only under
wasmtime. A directory is opened without the write rights (fd_datasync,
fd_write, fd_allocate, fd_filestat_set_size): wasmtime refuses a directory
opened with them (EISDIR). Nov 28: wasi-check.sh 10/10, Node and wasmtime
50.0.3.

Checks: `bash tests/wasm/wasi-check.sh` (one program per group, run in an
empty folder under Node, and under wasmtime when it is on PATH),
`bash tests/wasm/memory-check.sh` (XT31), `bash tests/wasm/size-check.sh`
(XT39).

## Builtins, system calls, WASI

| Builtins (stdlib) | Linux calls | WASI | Status, test |
|---|---|---|---|
| `p_file_read` (`read_file`, `fs_read_text`), `p_file_read_bytes`, `p_file_read_at`, `p_file_hash`, `p_file_size` | openat, lseek, read, close | `path_open`, `fd_seek`, `fd_read`, `fd_close` | works: wasi_files (r1-r3, rb, ra, rp, s1) |
| `p_file_write`, `p_file_write_bin`, `p_file_write_vec` (`fs_write_text`, `fs_write_bytes`) | openat (O_CREAT, O_TRUNC), write, close | `path_open` (oflags CREAT, TRUNC), `fd_write` | works: wasi_files (w1, w2, wb) |
| `p_file_append`, `p_file_append_vec` | openat (O_APPEND), write | `path_open` (fdflags APPEND), `fd_write` | works: wasi_files (a1) |
| `p_file_exists`, `p_is_dir`, `p_file_mtime_ms` (`fs_exists`, `fs_is_file`, `fs_is_dir`, `fs_modified_ms`) | openat, newfstatat | `path_open`, `path_filestat_get` (type, size, times) | works: wasi_files (e1, mt, no), wasi_dirs (m1) |
| `p_file_delete` | unlinkat | `path_unlink_file` | works: wasi_files (rm, nd) |
| `p_rename` | renameat2 | `path_rename` | works: wasi_files (mv) |
| `p_mkdir` (`fs_mkdir`, `fs_mkdir_all`) | mkdirat | `path_create_directory` | works: wasi_dirs (m1-m3) |
| `p_rmdir` (`fs_rmdir`, `fs_rmdir_all`) | unlinkat (AT_REMOVEDIR) | `path_remove_directory` | works: wasi_dirs (r1-r3) |
| `p_dir_list` (`fs_list_dir`, `fs_walk`, `fs_glob`) | openat (O_DIRECTORY), getdents64 | `path_open` (oflags DIRECTORY), `fd_readdir` | works: wasi_dirs (l1-l4, w1, g1) |
| `p_fs_copy_file` (`fs_copy_file`) | copy_file_range, sendfile, else read/write | `fd_read`/`fd_write` (the first two are ENOSYS) | works: wasi_files (cp) |
| `p_fs_map`, `p_fs_unmap` (a file's pages) | mmap of a descriptor, munmap | new pages filled with `fd_seek` + `fd_read` (a private copy; WASI has no mappings) | works since XT32: wasi_files (mm) |
| `p_mmap_anon`, `p_munmap`, the allocator | mmap, munmap | `memory.grow` and wasi.fd's free regions (XT31) | works: memory-check |
| `print`, `println`, `eprint` | write (1, 2) | `fd_write` | works: every test |
| `p_read_line`, `p_read_stdin` (`read_line`, `read_stdin`) | read (0) | `fd_read` | works: wasi_stdin |
| `p_clock_now`, `p_clock_now_ms`, `p_clock_now_us`, `p_clock_mono_ms`, CPU-time clocks | clock_gettime (0, 1, 2, 3) | `clock_time_get` (realtime, monotonic, process and thread CPU time) | works: wasi_clock_random (c1, c2, s1) |
| `task_sleep`, the scheduler's waits | nanosleep, sched_yield | `poll_oneoff` (one relative monotonic clock), `sched_yield` | works: wasi_clock_random (s1) |
| `p_os_random_u64` (stdlib/random.fd, crypto) | getrandom | `random_get` | works: wasi_clock_random (r1) |
| `p_argc`, `p_argv` (`os_argc`, `os_argv`, `arg_at`) | the process image at start | `args_sizes_get`, `args_get` | works: wasi_args_env |
| `p_getenv` (`env_var`, `os_getenv`) | the process image at start | `environ_sizes_get`, `environ_get` | works: wasi_args_env |
| `p_exit` (`os_exit`), returning from `main`, a panic | exit_group | `proc_exit` | works: wasi_args_env (exit code 7), every panic test |
| `p_getpid`, getppid, getuid, getgid | getpid, ... | none in WASI: 1, 0, 0, 0 | constants: wasi_clock_random (p1) |
| `sysconf` (page size, processors, memory pages) | sched_getaffinity, sysinfo | none in WASI: 4096, 1 processor; the linear memory (4 GB in all, free what it has not grown to) | works since XT32: wasi_clock_random (y1) |
| The preopened directories | — | `fd_prestat_get`, `fd_prestat_dir_name` | absolute paths go to the preopen with the longest matching name, relative ones to `.`: wasi_files (tm: `/tmp`) |

## What WASI preview 1 does not have

| Builtins | What happens |
|---|---|
| processes (`p_system`, `p_process_run`, `fid_proc_*`) | a panic at the call: `not available in WebAssembly: processes (...)` (tests/wasm/test_wasm_process.fd) |
| sockets (`p_socket_*`, `p_net_*`, `p_co_accept`, ...), `p_co_wait_fd` | socket, bind, connect, ppoll, epoll and fcntl are ENOSYS: the calls fail (WASI preview 1 only accepts on sockets the host opened; XT33/XT34 are WASI 0.2's sockets) |
| signals (`p_signal_*`), FID_CPU_LIMIT, FID_PROF | rt_sigaction does nothing, setitimer and prlimit are ENOSYS: no handler runs, no CPU limit, no profile |
| threads (`spawn` on an OS thread) | a thread runs to its end when it starts; tasks switch stacks (compiler/wasm.fd "Tasks") |
| `mprotect`, `madvise`, the allocator's decommit (MAP_FIXED) | nothing / ENOSYS: pages stay, a freed block keeps its bytes |

The WASI functions the runtime never calls (no Linux call of the stdlib
needs them): `fd_filestat_get`, `fd_filestat_set_size`, `fd_sync`,
`fd_datasync`, `fd_tell`, `fd_advise`, `fd_allocate`, `fd_renumber`,
`path_symlink`, `path_readlink`, `path_link`, `path_filestat_set_times`,
`fd_fdstat_get`/`set_flags`, `sock_*`.

## Memory (XT31)

The linear memory starts with the stack (8 MB), the data and a page, and
grows with `memory.grow` when the allocator maps more (its 4 MB chunks,
blocks over 1 MB, task stacks). WASM memory never shrinks, so wasi.fd keeps
what munmap gives back as free regions (whole 64 KB pages, sorted,
neighbours merged): mmap takes the smallest region that fits (the rest
stays free), zeroed, before it grows the memory, and when none fits and the
last region ends where the memory does it grows only by what that region
lacks. A block the memory cannot hold (more than wasm32's 4 GB, or what the
host refuses) is `Panic: out of memory (<n> bytes requested)`, exit code 1;
FID_MEM_LIMIT works as on Linux (exit code 125). tests/wasm/memory-check.sh:
200 MB held at once in three rounds of four block sizes peak at about
320 MB (1,828 MB before), 3 million runtime strings and 30,000 file reads
at 12 MB (332 MB before: the Linux runtime's `-> str` builtins never freed
their result, base.fd `p_str_give`).
