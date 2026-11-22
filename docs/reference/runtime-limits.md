# Run-time resource limits (roadmap P12)

A program built through C (`fid ccompile`, `fid build` on the C path)
reads two limits from its environment when it starts:

| Variable | Value | Past the limit |
|----------|-------|----------------|
| `FID_MEM_LIMIT` | bytes, or with `K`, `M`, `G` (`64M`, `1.5G`) | `fid: memory limit of N bytes exceeded (FID_MEM_LIMIT): U bytes in use, A more asked for`, a stack trace, exit code 125 |
| `FID_CPU_LIMIT` | seconds of CPU time (`2`, `0.5`) | `fid: CPU time limit of S seconds exceeded (FID_CPU_LIMIT)`, exit code 124 (as `timeout`) |

Unset, empty or `0` means no limit. A value that is not a number is an
error at start-up (exit code 2), so a typo does not run the program
unlimited.

What is counted:

- Memory: the live bytes of the Fides heap. Every block `p_malloc` hands
  out counts its size class while it is alive (freed blocks are taken off,
  so a program that allocates and frees in a loop does not hit the limit);
  a block over 1 MiB counts what the C library gave for it (`_msize`,
  `malloc_usable_size`, `malloc_size`). Memory the C runtime takes with
  `malloc` itself (thread stacks, the C library's own buffers) is not
  counted, nor are blocks dropped in bulk by `p_arena_restore`.
- CPU time: user plus system time of the whole process, every thread
  included. On Linux and macOS through `RLIMIT_CPU` and `SIGXCPU` (whole
  seconds, rounded up); on Windows a watcher thread reads
  `GetProcessTimes` every 10 ms. WebAssembly (WASI) has no CPU limit.

With no limit set the allocator pays one test of a global per call.

The implementation is `emit_limits_runtime` in `compiler/codegen_c.fd`
(called by `main` through `__p_limits_init`). The check is
`tests/limits/check.sh` (`tests/limits/check-run.sh` for `fid run`).

Limits baked in when the C is compiled: `-DP_LIM_MEM_DEFAULT=<bytes>` and
`-DP_LIM_CPU_DEFAULT_MS=<milliseconds>` are the limits when the variable is
not set (a set variable wins). `-DP_NO_ENV_LIMITS` makes the program ignore
both variables (for a program such as `fid` itself, whose user sets the
limits for the programs it builds and runs).

### Natively built programs (`fid emit`)

| Backend | Memory | CPU time |
|---------|--------|----------|
| machine layer, Linux targets (`--target x86_64-linux`, `aarch64-linux`, `riscv64-linux`) | yes | yes: `RLIMIT_CPU` (whole seconds, rounded up) and `SIGXCPU` |
| machine layer, `--nat` Windows | yes | yes: a thread reads `GetProcessTimes` every 10 ms (win.fd) |
| machine layer, macOS, WASI | yes | not enforced (their system-call layers have no CPU-time limit yet) |
| pe_ir (default Windows `emit`) | once `p_malloc`/`p_free` call the runtime (below) | once the entry point calls `pert_limits_boot` (below): a thread reads `GetProcessTimes` every 10 ms |

Same variables, messages and exit codes as the C path. The machine layer's
runtime (`stdlib/linux_runtime/sys.fd`, section "Run-time resource
limits") reads them in `pe_lim_init`, which `pe_rt_init` calls before
`main`; `pert_malloc` counts each block's size class (2^k bytes, a large
block its mapping) and `pert_free` takes it off, under the heap lock.
pe_ir's runtime (`stdlib/pe_runtime/conc.fd`, same section name) has
`pert_limits_boot`, `pert_lim_take(bytes)` and `pert_lim_give(bytes)`;
pe_ir's machine-code entry point and allocator (`compiler/pe_ir_stubs.fd`)
must call them for the limits to apply there. The check is
`tests/limits/check-native.sh [FID] [TARGET]`.

### Interpreted programs (`fid run`)

The interpreter (`compiler/interp.fd`, section "Run-time resource limits")
reads the same two variables when the program starts (after compiling it),
with the same messages and exit codes; a malformed value stops `fid run`
with exit code 2.

- Memory: the live heap of the interpreted program, counted as a compiled
  program counts it: each block of the interpreter's memory that the
  program's `p_malloc` takes, or that the interpreter takes for one of the
  program's values (a Vec's header and buffer, a map, a string stored in
  memory), counts what `p_malloc` of that size counts in the C runtime (its
  size class; past 1 MiB its pages) until it is freed. A call's allocas are
  its stack and are not counted, nor are the literals laid out before the
  program starts. The program stops once the instruction that allocated is
  done, with the message, the interpreted stack trace and exit code 125.
  Building a string (`+`, `str_repeat`) larger than what is left of the
  limit stops it too. A string the interpreter holds as its own value is
  counted once it is stored in memory (a `Vec<str>` element, a field); such
  a copy is uncounted for core (`Str_drop` leaves it), so it stays counted
  while the run lasts: a program that keeps storing new strings reaches the
  limit sooner than compiled.
- CPU time: the CPU time of the `fid` process from the start of the
  program (compiling it is not counted), read every 4096 interpreted
  instructions (GetProcessTimes on Windows, the process CPU-time clock of
  clock_gettime on Linux and macOS; a natively built Linux `fid` reads it
  through linux_runtime's `pert_c_clock_gettime`).
- `fid run <file> --max-mem <bytes[K|M|G]> --cpu-limit <seconds>`: the limits
  when the variable is not set, as `fid ccompile` takes them (a set
  variable wins; `fid run --leaks` passes them on to its C build).

This is not INTERP_MEM_LIMIT, the interpreter's own simulated memory (127 MB
of program bytes, 1 GB of the host's): everything in it counts there,
allocas and literals too, and past it an allocation panics ("p_malloc:
OOM ..."). A FID_MEM_LIMIT above about 127 MB is therefore never reached
under `fid run`. The older `fid run --max-memory <MB>` compares the top of
that memory with its limit when an alloca is made.

`fid` itself is built with `-DP_NO_ENV_LIMITS` (scripts/bootstrap.sh,
build-selfhost.sh), so the variables limit the program it runs, not the
compiler. The check is `tests/limits/check-run.sh`.

## Run-time type names (roadmap L8F)

`type_name_of(x)` is the name of x's type as a `str` (`i64`, `str`,
`Vec<f64>`, a struct's or enum's name). It is fixed when the program is
compiled, after monomorphization, so inside a generic function it names the
type of that instance, and every backend has it with no run-time support:
the lowering (`Lowerer_type_name_of`, `compiler/lower.fd`) turns the call
into a string constant. As C's `sizeof`, the argument is not evaluated.
`p_type_name(x)` is the same. Test: `tests/bootstrap/test_type_name_of.fd`.
