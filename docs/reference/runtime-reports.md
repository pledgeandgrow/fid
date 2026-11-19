# Run-time reports of the C path

What a program built through C (`fid ccompile`, and `fid run` with one of
the flags below, which builds through C) reports about itself. The code is
in `compiler/codegen_c.fd`; `tests/memprof/check.sh` checks each report.

## Leaks with the allocating line (complaint CM8)

`fid run <file.fd> --leaks` (or `fid ccompile ... --leaks`) prints at exit
the blocks never freed, grouped by where they were allocated:

```
fid --leaks: 3 block(s), 120 bytes, never freed; by where they were allocated:
       120 bytes in      3 block(s): make_buf <- main, allocated at tests/memprof/leak_line.fd:8
```

The functions are the first Fides frames on the stack at the allocation.
The line is the input file's: each function of the input file stores
`"file.fd:LINE"` in a thread-local word (`__p_mc_at`) before the first
instruction of each line, at each block's start and after each call, and
the debug allocator copies it into the block's record. A block allocated
inside core (a string, a Vec buffer) gets the line of the input file's call
that led to it. The driver gives the input file with `set_c_mem_source`;
without it, `-g`'s file is used, and without either there is no line.

## Memory and allocation profile (roadmap L7C, complaint CO5)

`fid run <file.fd> --mem` (or `fid ccompile ... --mem`, `P_MEMPROF` in the
C) counts every allocation of the debug allocator and prints at exit:

```
fid --mem: 9 allocation(s), 1045 bytes; 6 freed (525 bytes); peak 612 bytes live
  by function:
         512 bytes in      2 allocation(s): keep
         500 bytes in      5 allocation(s): fill
  by line:
         500 bytes in      5 allocation(s): tests/memprof/mem_profile.fd:12
         512 bytes in      2 allocation(s): tests/memprof/mem_profile.fd:23
```

Bytes are the sizes asked for. "By function" is the innermost Fides
function on the stack (a core function such as `p_str_alloc` when the
allocation happens there); "by line" is the input file's line, as for
`--leaks`. Each list shows the 20 largest. The table holds 3072 (function,
line) places; bytes from more places are counted in the totals and
reported as not attributed.

## Task chain of a panic (complaint CA9)

A panic or crash inside a task prints, after the task's own stack trace,
the tasks it was running for:

```
Panic: b failed
stack trace:
    - panic
    -| called from b
task chain: b <- a <- main
```

Each step is the task awaiting the previous one, else the task that
started it and is running it (both are alive while they wait). The chain
ends at the code outside tasks (`main`, or `(thread)` on a thread of
`p_thread_spawn`), or at `(started by f)` for a task whose starter has gone
on without awaiting it yet. `spawn` runs its function on a thread, not as a
task, and has no chain.

## Loading a library at run time (roadmap L6C)

| Builtin | Gives |
|---------|-------|
| `p_dl_load(path: str) -> i64` | the OS's handle (`LoadLibraryA`, `dlopen(RTLD_NOW)`), 0 when it cannot be loaded |
| `p_dl_find(lib: i64, name: str) -> i64` | the symbol's address (`GetProcAddress`, `dlsym`), 0 when missing |
| `p_dl_unload(lib: i64) -> i64` | 0 when released, -1 otherwise |
| `p_dl_call(f: i64, arg: i64) -> i64` | calls `f` as `int64_t (*)(int64_t)` |

The C runtime has the loader only in a program that calls these (an old
glibc needs `-ldl` for it). On WASI loading always fails. `p_dl_open` and
`p_dl_sym` are still the old simulation (sequential handles, one fake
address per library; `test_dlopen.fd`), and `p_dl_call` on such a fake
address (below 64 KiB) returns its argument as before.
`tests/bootstrap/test_dl_find.fd` loads the host's C library, finds and
calls `llabs`, and gets 0 for a missing symbol and a missing library.
