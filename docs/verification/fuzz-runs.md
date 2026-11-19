# Fuzz runs of record (VF1)

The runs of `scripts/fuzz/nightly.sh` / `scripts/fuzz/gen_programs.py`:
random programs that `run` (the interpreter), `ccompile` (C), `emit` (the
native Windows backend, pe_ir) and `checked` (the C path under
`--checked-alloc`) must agree on. A seed names one program (`--rich` for
the rich kind), so every seed below is a repro:

    python scripts/fuzz/gen_programs.py -n 1 --seed <s> [--rich] --fid <fid> \
        --backends run,ccompile,emit,checked --keep out/

Failures are filed in [miscompile-log.md](miscompile-log.md) with repros in
`tests/verify/bugs/`.

## Nov 17, 2026 — first runs and the first long run

**Compiler**: the stage 0 of Nov 16 (`fid` built from the tree of that day;
it does not hold the work of Nov 17), on Windows x64, `CC="zig cc"`, with
the stdlib of `HEAD` beside it (`FID_HOME`). The machine ran several other
fid builds at the same time (load 8-44): see "Machine-side failures".

| Run | Seeds | Kind | Backends | Batch | Programs | Kept | Time |
|-----|-------|------|----------|-------|----------|------|------|
| A1 | 20261117-20261266 | plain | run, ccompile, emit, checked | 1 | 150 | 1 | 14:41-16:05 |
| A2 | 20261117-20261266 | rich | run, ccompile, emit, checked | 1 | 150 | 19 | 16:05-18:52 |
| B1 | 1000000-1000199 | plain | run, ccompile, emit, checked | 50 | 200 | 3 | 19:15-19:44 |
| B2 | 1000200-1000999 | plain | run, ccompile, emit, checked | 50 | 800 | 18 | 20:06-21:37 |
| C1 | 1000000-1000549 | rich | run, ccompile, emit, checked | 50 | 550 | 21 | 21:37-23:01 |
| C2 | 1000550-1000999 | rich | ccompile, emit, checked | 50 | 450 | 17 | 23:01-23:17 |

The long run kept 59 of 2,000 programs (3 + 18 + 21 + 17): 11 are M24, 37
M25 and 11 the `q4` name the stage 0 does not take; none is anything else.
(C1 also ran seed 1000555 alone, from the batch that was stopped; C2 kept
it again.)

The long run is B1+B2+C1+C2: 2,000 programs (1,000 plain, 1,000 rich), of
which 1,550 also ran on the interpreter. C2 leaves `run` out: one batch of
49 rich programs had run on the interpreter for 25 minutes (1.9 GB) when it
was stopped (finding I1), and the rest of the rich seeds would have taken
another five hours or more. Run A ran each program alone; the long run
compiled 50 programs as one file (`--batch 50`) and ran a seed alone only
when its batch disagreed.

### What the kept programs are

Every kept seed was triaged with `scripts/fuzz/triage.py` and the ones it
could not name were run again alone (`--retries 2`); the M24 ones were
checked with `FID_IROPT_OFF=promote` (right output then) and the M25 ones by
the size of their Vec (all over 512 elements).

| Finding | Seeds | What |
|---------|-------|------|
| **M24** (new, open) | A2: 20261189 (reduced, 655 tries of `reduce.py`), 20261217; B1: 1000119; B2: 1000311, 1000525, 1000803, 1000915; C1: 1000250, 1000372, 1000525; C2: 1000570, 1000645, 1000744 | `emit` prints a wrong value: `x = x;` between two `if`s and the IR optimizer's `promote`. `tests/verify/bugs/m24_promote_self_assign.fd`, `m24_promote_self_assign_on.fd` |
| **M25** (new, open) | A2: 20261164; B1: 1000122, 1000170; B2: 14 seeds (1000381, 1000429, 1000461, 1000466, 1000492, 1000504, 1000521, 1000530, 1000586, 1000596, 1000615, 1000635, 1000833, 1000919); C1: 11 seeds (1000047, 1000070, 1000088, 1000122, 1000201, 1000427, 1000434, 1000466, 1000492, 1000497, 1000521); C2: 10 seeds (1000578, 1000584, 1000596, 1000628, 1000670, 1000776, 1000936, 1000961, 1000976, 1000983) | `checked` panics in `Vec_push` when a Vec grows past 4 KiB: the buffer tag probe reads 24 bytes before an untagged buffer. `tests/verify/bugs/m25_checked_vec_grow_page.fd` |
| `q4` as a name (fixed in the tree) | A2: 20261147; C1: 1000015, 1000058, 1000107, 1000131, 1000271, 1000380, 1000493; C2: 1000555, 1000793, 1000934, 1000981 | the rich programs name their fourth struct copy `q4`; the stage 0 parses `q4` as the type `i8` ("expected identifier but got 'I8'"). Fixed since (stdlib-agent-bugs 13, `tests/bootstrap/test_fix_type_word_idents.fd`): not a finding for the current compiler, but it cost the stage-0 run a split batch each time |
| generator (fixed) | A1: 20261220 | the program set its own loop counter back and never ended (every backend timed out). Fixed in `Gen.target` before the long run (`test_gen_programs.py` checks no program assigns a counter) |
| I1: interpreter speed (open, not a miscompile) | A2: 20261134, 20261228; B1: 1000122; B2: 1000504, 1000833 (the last three are M25 seeds too) | `run` passes the 60-second limit where the compiled program takes 0.6 s: 20261134 prints the right answer after 8 min 14 s. A loop calling a function that makes a `Vec`, pushes and reads one element costs ~9 ms per call on `fid run` (4,000 calls: 34.8 s; with a string concatenation instead: 13.4 s), linearly. A rich batch of 49 programs ran 25 minutes and grew to 1.9 GB. `--run-timeout` (default 60) sets the limit |

No other disagreement was found: nothing differed between `run` and
`ccompile`, and every `emit` difference was M24.

### Machine-side failures

Run A2 kept 19 programs; 13 of them agreed when run again alone later
(20261127, 129, 169, 170, 175, 182, 185, 186, 187, 188, 194, 196, 203), and
the other 6 are the findings above (M24 ×2, M25, `q4`, I1 ×2). What the 13
had shown: `zig cc` ran out of memory ("LLVM ERROR: out of memory") or
could not be started ("failed to spawn zig clang"), `p_malloc: OOM` in the
checked build, a C compilation that failed with no message, a timeout, and
`fid` itself stopping with a null access in `wln` while writing the C
(20261129). The harness now keeps these apart:

- `gen_programs.py --retries k` runs again only the backends that failed
  or gave the minority's result, up to k times, before a program is kept;
  a batch's backend that could not build it is tried again before the
  batch is split.
- A failed build's stderr ends with `(compiler exit N)`, so a crash of fid
  is told from a rejection.
- `triage.py` groups failures that point at the machine (out of memory, a
  C compiler that could not start, a timeout) under "environment?" and
  lists their seeds to run again; hex addresses no longer split a
  signature; a signature that looks like a logged bug gets a hint
  (`[likely M25]`, `[maybe M24]`, `[q4/q8 as a variable name]`); when no
  backend result has a majority, the most common result of the backends
  that finished is the expected one.
- A batch splits less: when one seed stops a backend (a panic), the first
  differing seed runs alone and the seeds after it run again as a smaller
  batch (before: 29 of 50 ran alone); when the compiler rejects a batch,
  the seed whose lines hold the error runs alone and the others as a batch
  (before: all 50 alone). A program that no backend builds no longer counts
  as agreeing.

The harness is checked without a compiler by
`python scripts/fuzz/test_gen_programs.py` (fake backends for the retries
and the splits).

### Reproducing

    # one finding (M24): emit prints garbage, promote off makes it right
    python scripts/fuzz/gen_programs.py -n 1 --seed 20261189 --rich \
        --fid release/fid.exe --backends run,ccompile,emit --keep out/
    # the long run
    N=1000 SEED=1000000 BATCH=50 RETRIES=2 FID=release/fid.exe \
        bash scripts/fuzz/nightly.sh
    # the repros
    python scripts/verify/expect.py --fid release/fid.exe tests/verify/bugs/m24_promote_self_assign.fd \
        tests/verify/bugs/m24_promote_self_assign_on.fd tests/verify/bugs/m25_checked_vec_grow_page.fd

Not run yet: the Linux backends of `nightly.sh` (`emit:x86_64-linux`, the
ASan/UBSan sample) and a run with a compiler built from the tree of Nov 17
(the validation of this run's build should repeat B1 with it).
