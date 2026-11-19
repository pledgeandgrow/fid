# Parallel code generation (SP5)

The machine layer (`compiler/nat_ir.fd`: Linux x86-64, AArch64 and RISC-V
64, macOS, and `--nat` Windows) can emit the functions of a program on
several threads. The image is the serial one byte for byte, whatever the
number of threads.

```
FID_NAT_PAR=8 fid emit prog.fd --target x86_64-linux -o prog
[par] FID_NAT_PAR: 1834 functions joined from worker buffers, 41 emitted again in place (8 threads; taint marks 12 ms, workers 310 ms, join 95 ms)
```

| `FID_NAT_PAR` | what runs |
|---|---|
| unset, empty, `0` | the serial emission (the default) |
| `1` | the workers' code on the main thread (one buffer per function, then the join) |
| `2` .. `16` | that many worker threads |
| anything else | 4 threads |

Not with the per-function cache (`FID_FNCACHE`), whose capture reads the
image as it is written.

## How it works

`ng_mir_finish` records, allocates and folds every function as before; with
the switch on it hands each function it would emit to `ng_par_add` instead.
`ng_par_finish` then:

1. makes, on the main thread and in placement order, each function's
   MI28 taint marks (`ng_tail_pre`): which calls may pass the frame's
   address and so must not become jumps. The analysis keeps callee
   summaries in module state, so the marks of a function depend on the
   functions analysed before it; making them in the serial order keeps
   them the serial ones. The serial emission makes them the same way, once
   per function before its first pass (`ng_mir_emit`), and both passes of
   branch relaxation use them;
2. splits the jobs into one contiguous chunk per thread of about the same
   number of machine instructions (`ng_par_chunks`), makes a generator per
   thread (`ng_par_gen`) and spawns `ng_par_thread` per chunk;
3. each thread emits its jobs one by one (`ng_par_emit_job`) into buffers
   of their own: code, fixups, line and frame rows, panic-line stubs, the
   labels it placed, with every label of the image unplaced and the
   function's own label at 0, so nothing it emits depends on another
   function's place;
4. the main thread joins the buffers in placement order (`ng_par_join`):
   bytes at the function's offset, fixups and rows moved there, the labels
   a job made renumbered into the image, a panic-line stub the image has
   already reused. A function whose bytes depend on where it starts is
   emitted again in place (`ng_par_redo`): on x86-64 a loop head padded to
   16 bytes when the function does not start on 16; on RISC-V a call to a
   function already placed (it may take the short JAL). Then its debug
   variable locations (`ng_dbg_vars_resolve`), its facts line and its line
   sequence's end, as the serial loop makes them.

## What a thread may touch

Owner counts of strings, Vecs and Boxes are not atomic (core.fd "Copies
for another thread"), and module globals are shared. So:

* **No module global is written** by anything a thread runs. What the
  emission of one function keeps is in `NatGen`: `sw_pro`/`sw_ep` (RA12's
  save subsets), `ldfwd_n` (TG17's count, added to `g_nat_ldfwd_n` by the
  main thread), `dbg_mpos`/`dbg_mark_on` (CT6's instruction positions,
  rebased at the join), `wipe_on`/`wipe_regs`/`wipe_keep1` (LF20),
  `tc_taint` (the marks of step 1), `line_fix` (the file of its line rows)
  and `par_log` (its `--stats` lines, printed at the join). Tables the
  emission reads are made before any emission (`ng_emit_tables`, called by
  `NatGen_new`: the TG1 selection table, the MI25 float-register
  switches); `ng_isel_mi` and `ng_fregs_on` only read them.
* **Each thread has a generator of its own**, made on the main thread
  (`ng_par_gen`): a `NatGen_new` of the target with the serial generator's
  numbers and switches, copies of the lists emission reads, and the raw
  handles of its maps, which emission only looks up. The one map emission
  writes, `pl_keys`, is made fresh for each job.
* **A job's function and allocation are touched by one thread**:
  `ng_par_add` copies the MIR, slots, name and allocation into buffers of
  their own (a recorded MIR may share its buffer with `g.recs`).
* The thread's label table (as many entries as the image has labels) moves
  into the job's generator copy and back out after it, so it is never
  copied per job; the labels a job placed are reset after it.

The type checker's race check (`TypeChecker_verify_no_races`) sees only
the spawns of a program's main file, so `tests/nattools/par_emit_globals.py`
checks the first rule: every function `ng_par_thread` reaches (by name,
over `compiler/*.fd` and the core stdlib modules) assigns no module
variable and changes no map of the generator but `pl_keys`. Its
`--self-test` plants a global write and a map insert into a copy of the
sources and must find both.

A field added to `NatGen` that the emission reads must be copied in
`ng_par_gen` (else the images differ, which `par_emit_check.sh` sees); a
field it writes and keeps must stay in `NatGen` (not a module global).

## Checks

* `bash tests/nattools/par_emit_check.sh $FID` — for every machine-layer
  target and four programs: the image with `FID_NAT_PAR=1` and with
  `FID_NAT_PAR=4` (threads) identical to the serial one; `test_par_emit.fd`
  five times on 8 threads, with `FID_VERIFY=1` on 4 threads, with `--stats`
  (the same `[shrinkwrap]` and `[ldfwd]` lines), with `FID_NAT_PAR=0` (off),
  `FID_NAT_MUTATE=par` (joining the functions that must be emitted again
  gives a different image, where there are any), and run where the image
  runs. `PAR_BIG=1` also builds `compiler/main.fd` for x86-64 Linux serially
  and on 8 threads and prints both times.
* `python3 tests/nattools/par_emit_globals.py . --self-test` — the static
  rule above.
* `tests/bootstrap/test_par_gen_threads.fd` — the ownership pattern on its
  own (generators per thread in a module Vec, jobs read by index, a label
  table moved in and out, buffers returned through futures): four threads
  give the buffers one thread gives, on every backend.
