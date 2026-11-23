# Compiler speed notes

Consolidated Nov 22: front-end and mid-end work that keeps compiles fast (SP9, SP11, SP12, WP19).

## Imports lexed in parallel, and the module graph (SP9)

Roadmap SP9 (llvm-roadmap, "Front end in parallel per module where modules
do not depend on each other"). This page says what runs in parallel today,
why the output cannot change, what was measured, and what is left.

### What it does

With `FID_PAR=N` (2 to 16, as for the parallel checks in
[parallel-checks.md](parallel-checks.md)), the import phase first finds the
import closure of the program and lexes its files, a breadth-first wave at
a time, each wave in up to N threads. The import loop that follows is the
sequential one: it parses the modules one by one, in the same order and
with the same struct and enum names as without `FID_PAR`, and finds their
tokens already in its token cache.

| Piece | Where |
|-------|-------|
| Lex texts in N threads, same tokens and messages as `Lexer::new(t).tokenize()` | `compiler/lexer.fd` `lex_texts_par`, `lex_par_worker`, `lex_text_alone` |
| Spread the texts over the workers (largest first, to the least loaded) | `lexer.fd` `lex_par_plan` |
| A module's imports from its tokens, without a parse | `lexer.fd` `token_use_paths` |
| The module graph and its levels | `lexer.fd` `ModGraph`, `modgraph_levels`, `modgraph_summary` |
| The prefetch in the import phase (driver) | `compiler/compiler.fd` `imp_prefetch_lex` |

The prefetch runs only when the import cache is not in use (it is on in the
first build of a one-shot command unless `FID_IMPCACHE=off`; a cache hit
neither lexes nor parses, so lexing ahead would only add work) and not in a
long-lived process with the module cache (`fid serve`, `fid fix`, the LSP).
With `--time` it adds a row just before the `imports` row, a part of that
phase (its form; the numbers are those of the program):

```
[time]   imports: lexed ahead, <n> modules, <l> levels, widest <w>: <ms> ms
```

### Why the output is the same

* Lexing a file depends on its text alone. The one piece of shared state
  the lexer had, the cache of words that are no keyword (`g_lex_not_kw`,
  CS4), is replaced in a worker's lexer by a map of its own
  (`Lexer.own_kw`, `keyword_or_ident_own`); the cache only remembers the
  answer of a pure function, so it gives the same tokens.
* Only a file that lexed without an error is put in the token cache. A file
  with a lex error is left to the import loop, which lexes it again when it
  reaches it and reports the error exactly where it did before.
* The import loop, its order, its deferrals, the names it gives the parser
  and every message are unchanged; a file the prefetch lexed but the loop
  never reaches (a `use` it drops) is simply not used. The prefetch records
  nothing for incremental builds (`inc_record_*`): only the loop does.

### Why the threads share nothing they change

As for the parallel checks (owner counts are not atomic): a worker gets
its number as its only spawn argument, reads its list of texts from
`g_lex_par_plan` and the texts from `g_lex_par_texts` (set before the
spawns, reset after the last await, never changed while the workers run),
lexes a copy of each text in a buffer of its own (`lex_own_str`), and
returns its tokens; the caller puts the results back in text order.

### The module graph

`token_use_paths` reads the `use` paths at brace depth 0 from a file's
tokens, as `parse_use` names them (`a::b` for `use a::b;`, `use a::b as c;`,
`use a::b::{x, y};`, `use a::b::*;`). The prefetch builds the graph of the
closure from them (`ModGraph`: names, files, the indexes each imports;
`core` left out as the import loop skips it). `modgraph_levels` gives each
module its level: 0 when it imports none of the others, else one more than
the highest level of those it imports; modules of one level do not depend
on each other, so they are the units a per-module front end can work on
together. A module on an import cycle, or behind one, gets -1.

### Measured

`tests/par/lex_par_bench.fd` lexes the 80 `.fd` files of `compiler/`
(12.4 MB, 2.2 million tokens) with `lex_texts_par` (built with ccompile,
`zig cc`, Windows, Nov 17, a machine shared with other builds):

| Threads | 1 | 2 | 4 | 8 |
|---------|---|---|---|---|
| ms (run 1) | 736 | 434 | 385 | 248 |
| ms (run 2) | 747 | 400 | 325 | 219 |

The same tokens in every case. The gain is bounded by the largest file
(`typechecker.fd`, about a fifth of the bytes, is lexed by one thread).
Lexing is a small part of the import phase: the rest is the parse, which
stays sequential (below). The import rows of `check compiler/main.fd
--time` with and without `FID_PAR=4` are the measure to take on the built
compiler (`SELF=1 bash tests/par/imports_par_check.sh`).

### Not parallel yet (what SP9 still needs)

* **Parsing.** A module's parse depends on the struct and enum names of the
  modules parsed before it in the loop's order (not only those it imports),
  and on parser globals: the `module::name` calls (`take_qualified_calls`,
  read in parse order by `cc_qualify_shadowed_imports`), turbofish
  numbering (`turbofish_all`, numbered across modules), `parser_saw_secret`,
  the process-wide import set, `g_pd_impl_selfs`. To parse modules of one
  level together, the names each module would have been given must be
  known first (a token scan of `struct`/`enum` at depth 0 gives them, and a
  parse can check them afterwards), and each of those globals must become
  per-parser state merged in the loop's order.
* **Type check.** It runs on the merged program, not per module, and the
  body check writes shared state for every function: the inferred types,
  generic instances (CE9), the RT6 records, the effect tables and
  `self.errors` with TL2 placement. A per-module check needs those split
  into per-worker parts merged in function order, as the borrow and escape
  checks' workers do (their units are functions: parallel-checks.md).
* **Escape and `@secret`.** Their reporting passes already run in
  `FID_PAR` threads per function; the summary fixpoint before them is
  sequential. With the levels above, the summaries of one level could be
  computed together, since a module's summaries depend only on its own and
  those of lower levels (cycles excepted).

## Arena windows for one function's compiler data (SP11)

Roadmap: docs/roadmap/llvm-roadmap.md, SP11, "Arena allocation for
compiler data per function, freed in one step".

### The mechanism (compiler/inc.fd)

`fn_arena_begin` takes the C runtime's allocator mark (`p_arena_save`).
`fn_arena_end` goes back to that mark (`p_arena_restore`), which hands back
in one step every arena chunk taken since the mark. A result outlives the
window by being copied into a hold block with `fn_arena_keep_*` and read
back after the window with `fn_arena_take_*`.

The switch is opt-in: `FID_FN_ARENA=1` in the compiler's environment.
Windows only work in a compiler built through C, the only runtime with
allocator marks; `release/fid.exe` is one. In any other runtime
`fn_arena_begin` returns 0 and the work runs as before.

A window is sound only when nothing made inside it is reachable after its
end. The full rule is in the comment above `fn_arena_on` in inc.fd. In
short, the work inside a window must not:
- change a container made outside the window;
- set a global to anything but a scalar;
- take an owner of an outside container, unless that container was pinned
  before the window.

### Where the compiler uses it

The machine layer's register allocation runs in a window: `ng_mir_alloc`
in compiler/nat_ir.fd. It does this work for:
- each function of `fid emit` on the Linux targets, macOS and `--nat`
  Windows;
- each trial allocation of live-range splitting (RA4).

`ng_mir_alloc` does the following:
1. Pins the containers the allocation takes an owner of:
   - `rec.mir` and `rec.slots`;
   - `g.td.int_callee` and `g.td.int_caller`;
   - RA4's `g_ng_split_base`, `g_ng_split_orig` and `g_ng_split_src`.
2. Opens the window.
3. Runs `ng_mir_alloc_body`, through `ng_alloc_keep`, which keeps the
   `NatAlloc` (its vectors, words and error string).
4. Closes the window and rebuilds the `NatAlloc` outside it.

Inside the window, `ng_mir_alloc_body` makes and drops everything it
uses:
- the liveness, intervals, segments, interference and copy classes;
- the spill weights;
- the checker's tables and its strings.

The rule was checked over the call graph of `ng_mir_alloc_body` (123
functions, Nov 17). It changes no container of the generator and no
global container. It sets only integers and flags, plus `g_ng_live_err`,
which it sets back to `""` before it returns. It builds no table lazily.

Emission stays outside the window. It grows containers that outlive the
function: the code buffer, labels and fixups in `g.e`, and the line and
frame rows. A window would hand those back with the rest.

### Measuring it

`bash tests/tooling/fn-arena-emit-check.sh [fid]` checks the following:
- `fid emit compiler/main.fd --target x86_64-linux --time` builds with and
  without `FID_FN_ARENA=1`, and the two images are byte for byte the same;
- with arenas, the build reports `[time] fn arenas: N windows, M MB given
  back at their ends, 0 not back at their mark`;
- small programs build with `FID_VERIFY=1` and arenas on, on x86-64,
  AArch64 and RISC-V, and each image equals the one built without arenas.

The script also prints the wall time and peak memory of both large builds.
With `FULL=1` it also runs the native suite with arenas and `FID_VERIFY=1`.

One known cost: the C runtime's `p_arena_restore` empties its free lists.
Blocks freed before a window are not reused after it. The peak memory
measured above shows how much that matters.

### Results

Not measured yet: the compiler with this change had not been built when it
was written. Add the table from fn-arena-emit-check.sh here.

Nov 21 (roadmap SP11 check, fid built from a self-hosted stage 0):
fn-arena-check passes (321 windows back at their mark), and `FID_VERIFY=1`
arena builds give the same images on 3 targets. fn-arena-emit-check fails
3: the native emit of compiler/main.fd does not finish (an iro_sccp
overflow panic, known-bugs 2g.89, or out of memory), so the table is still
missing.

## Lowering speed: work that grew with a function's length (SP12)

SP12 (llvm-roadmap, the same goal as competitive-roadmap CS4) is a
self-compile under 10 s. Lowering (`compiler/lower.fd`) and C generation
were about 22 s of the 25 s. This page records what was found to grow
faster than linearly in the lowering, what was changed, how it was checked,
and what is left.

### How it was found

`tests/perf/sp12_gen.py N` writes a program whose one function has N groups
of ordinary statements (a string kept in a second variable, a string copied
from a place, a local Vec, a Vec copied and only read). On the Nov 16
compiler, `dump-c --time` of it gave:

| N | statements | lower |
|---|-----------|-------|
| 300 | 2,700 | 65 s |
| 600 | 5,400 | 249 s |

Twice the statements took four times as long: every pass below was
quadratic in the length of a function or of a block. `tests/perf/
sp12_lower_bench.fd` runs `lower` alone on a file inside one process; a
stack sampler on it showed where the time went, and each fix was measured
again the same way.

### What changed (all in the frontend group's files)

Every change gives the same answer the old code gave; only how it is found
differs.

| Where | Was | Now |
|-------|-----|-----|
| `Lowerer_last_use` (lower.fd) | for each kept variable, `cow_uses_stmt` on every later statement of each enclosing list, and `lw_has_defer` of the whole body | one name index per statement list (`fr_lu`, filled once by cowcheck's `cow_names_stmt`, `Lowerer_used_after`); `cur_has_defer` worked out once per function |
| `Lowerer_rc_move_last` | the last-use walk first | the cheap checks first (an owned slot, not a loop's condition) |
| `Lowerer_readonly_copy_in` (a copy of a place) | the rest of the block copied, then walked six times | only the statements that mention the copy or its root, each alone (`Lowerer_readonly_rest`); the full walk when the index cannot tell |
| `Lowerer_lookup_alloca`, `Lowerer_lookup_var_ast_type` | scan of every variable of the function | name index with links down the stack (`lw_ix_link`, `lw_ix_last_alloca`), rebuilt when the tables are replaced whole |
| `lw_rc_cancel_pairs` | `lw_rc_value_used` / `lw_rc_use_count` per candidate: a walk of the function each | use counts once (`lw_rc_use_counts`), kept up to date by `lw_rc_rewrite_arg` |
| `lw_rc_literal_slot_counts` | `lw_rc_value_used` per candidate | counts gathered in its own pass |
| `lw_rc_empty_drops` | per candidate, the block searched from the load forward and back | per block, each slot's previous and next reference (`lw_rc_empty_block_refs`) |
| `lw_rc_stack_vecs_fn` | each stack Vec rewrote its drops by a walk of the function | one pass for all (`lw_rc_stack_drops`) |
| `lw_detached` | the block's instruction list copied on every pop (the block was still in `f.blocks`, two owners) | the block taken out first |
| escape.fd `esc_declare_fn` | a copy of the block's variable list held while pushing: the push copied the whole list | the list read in place |
| escape.fd `esc_lookup`, `esc_add_roots` | every variable copied while searching | names compared in place |

### Measured

`sp12_lower_bench` built with stage0 (`dump-c`, then `zig cc -O1`), the same
generated programs, lowering only:

| N | midway (part of the changes) | after | after, with the ir.fd hunk |
|---|-----|-----|-----|
| 100 | 0.39 s | 0.22 s | 0.022 s |
| 200 | 1.8 s | 0.81 s | 0.068 s |
| 400 | 8.3 s | 3.7 s | 0.16 s |
| 800 | 32 s | 11.6 s | 0.53 s |
| 1600 | | 39 s | 1.8 s |

(Another job ran on the machine at the same time: the numbers are rough.)
Without the ir.fd hunk below, copying the block's instruction list on every
emitted instruction is most of what is left; with it, the long function
lowers about 20x faster than before and close to linearly (3.3x for 2x).
The first measurement, on stage0 itself, is the table further up.

### Left

- `compiler/ir.fd` `IrFunction_emit` (another group's file): it took the
  block out of `self.blocks` with `get`, pushed the new instruction and set
  it back, so the push found the instruction list with two owners and
  copied it, once per instruction emitted: quadratic in a block's length,
  the largest single cost left (half the lowering time of the long
  function). Setting a fresh `BasicBlock_new(block_idx)` in its place right
  after the `get` makes the push change the list in place (checked on a
  stand-alone program: 20,000 pushes, 1,373 ms before, 2 ms after, same
  contents). Done since: `IrFunction_emit_line` does this (comment CS4),
  and so does `IrFunction_add_block_param`.
- Nov 21 (roadmap SP12 check, fid built from a self-hosted stage 0):
  lowering is still super-linear: `sp12_scaling.sh` at N=400 fails (twice
  the statements take 3.8–4.6x, limit 3.5); `check compiler/main.fd
  --time` takes 35 s.
- `lw_rc_cancel_param_slots` (`lw_ps_*`) and `Lowerer_lookup_alloca_type`
  still scan per candidate; they were 22% and 10% of the long-function
  bench after the changes above.
- C generation (codegen_c.fd, checks.fd: other groups) was not changed.
- Lowering or generating C per function in parallel, or from the
  per-function cache, is not done.
- Nothing here is measured on `dump-c compiler/main.fd`: the self-compile
  time with these changes is for the final build to record.

### Checking it

- `bash tests/perf/sp12_scaling.sh` (FID set): lower(2N) / lower(N) below
  `MAX_RATIO` (default 3.5), and the generated program's output equal to what `sp12_gen.py
  --expect` computes.
- `tests/selfhost/test_cow_names.fd`: `cow_names_stmt` agrees with
  `cow_uses_stmt` for every statement and name of its fixture.
- `tests/bootstrap/test_sp12_lowering.fd`: moves, defers, closures, stack
  Vecs, shared Vecs, emptied Vecs and string pairs keep their meaning.
- During development each new table was compared with the old scan at
  every query (a copy of lower.fd with assertions) while lowering every
  program under tests/bootstrap, tests/leaks, tests/conformance and several
  compiler modules: no difference.

## Sharing generic instances (WP19)

A generic function or method is compiled once per set of type arguments
(monomorphization, `compiler/mono.fd`). Two ways keep the compiler from
building or emitting the same body twice.

### At monomorphization: the key is the lowered type

`mono.fd` names an instance after its type arguments (`pick__i64`,
`Wrap__get__i64`). Since WP19 those arguments are first put into the form
lowering sees (`mono_share_type`, which mirrors `lower.fd`
`Lowerer_resolve_type`): a type alias is replaced by its target, also inside
a reference, a raw pointer, an array and a generic's arguments. So with

```fid
type Meters = i64;
fn count_of<T>(v: Vec<T>) -> i64 { return v.len(); }
```

`count_of(v)` for `v: Vec<Meters>` and for `v: Vec<i64>` both call
`count_of__i64`, built once. Lowering could not tell the two bodies apart:
the IR type, the ownership kind (`Lowerer_rc_kind`) and `type_name_of` all
go through `Lowerer_resolve_type`.

What is not shared here:

- an alias that has an `impl` or a trait impl of its own name (a method call
  on `T` may resolve through that name);
- the instances of a function or method with L8H specializations
  (`instantiate` and `mono_method_inst` choose among them by the spelled
  types);
- a tuple's or a function type's parts (lowering does not resolve aliases
  inside them either);
- distinct types with the same layout (two structs with the same fields):
  their bodies differ in names (field lookup, printing). The IR-level fold
  below merges what is still identical after lowering.

Switches: `FID_MONO_SHARE_OFF=1` keys instances by the spelled types (the
old behaviour); `FID_MONO_STATS=1` prints
`mono: N instance key(s), M built, K shared by lowered type`.

Check: `bash tests/mono_share/check.sh $FID`
(`tests/bootstrap/test_mono_share_alias.fd`).

### After lowering: identical functions folded (`icf`)

`compiler/iropt.fd` `iro_fold_identical` sends every direct call to a
function whose serialized IR equals an earlier function's to that earlier
one (opt-in `FID_IROPT_ON=icf` until a suite run shows the same output with
it on and off: `tests/tooling/midend-optin-check.sh` with `IME=icf`).

---

## PE88: `fid emit` of a typical test in under 1 second

Roadmap: docs/roadmap/pe-roadmap.md item 88 ("Native compile of a typical
test in under 1 second"). The measure is the default Windows build,
`fid emit <file.fd>` through pe_ir, with an -O2 compiler on a quiet machine
and the caches warm (D9's checked-library stamp, the import and AST
caches). `--no-cache` only skips the "up to date" answer for an unchanged
output, so a timed run always builds.

### How to measure

    FID=release/fid.exe bash tests/tooling/pe88-emit-time-check.sh

The script builds tests/bootstrap/hello.fd and test_collections.fd (a
mid-size test: Vec, HashMap, strings) once to warm the caches, then `RUNS`
times (default 5) with `--time`, and prints the stage table of the best run
for each, with and without the prune below. It fails when a best total is
not under `LIMIT_MS` (default 1000). It warns when other compiler processes
are running: several fid or zig processes at once make a stage 10-200x
slower (AGENTS.md), and such numbers are not the target's.

### Where the time went (Nov 17, a -O0 compiler, a busy machine)

`stage0.exe emit tests/bootstrap/hello.fd --no-cache --time`, before the
prune (absolute numbers are an -O0 compiler's on a loaded machine; the
shares are what matters):

| Stage | ms | share |
|---|---|---|
| inc check (the compiler hashes its own 55 MB executable) | 290 | 4% |
| front end (stdlib 350, typecheck 600, mono 370, lints 120) | 1650 | 21% |
| lower | 2800-3450 | 36-38% |
| iropt | 1350-1640 | 17% |
| native code (pe_ir) | 1600-1880 | 20% |
| total | 7700-9150 | |

With `FID_IROPT_STATS=1` the mid-end shows why it is that slow for a
12-line program: its per-function passes run on the 24 functions main
reaches, but its module-wide passes (hofspec 593 ms, licmcall 162, inline
154, gconst 87, closuredevirt 70) go over every function lowering made:
main's, all of core and the whole merged runtime. pe_ir likewise generates
only what main reaches but builds its string table, its list of needed
runtime functions and a label for every function of the module. iropt's
opt-in dead-function pass (`FID_IROPT_ON=deadf`) found 157 functions no
code can call in hello.fd, but took 1446 ms itself: it and
pe_ir_reachable_from look names up along a list.

### What changed: pe88_prune (compiler/main.fd)

Between lowering and the mid-end, a pe_ir build drops the functions no code
can call (`[build] pe88: N of M functions no code can call dropped before
the mid-end`, and a `prune` row in the `--time` table):

- Roots: main, main_entry, every function ir.fd `ir_dfe_root` names (p_*,
  __*, the whole Fides runtime pe_/pert_/pet_/ped_, nat_*, the Vec_* and
  vec_raw_* stand-ins, the collection printers), functions handed to C,
  interrupt handlers and kernels, externs, and functions some string of
  the module names (string table, global initializers).
- Edges, those of pe_ir_reachable_from: calls (the callee and what pe_ir
  resolves it to: a pert_c_ override, core's stand-in, a pert_ function),
  print and p_to_str with every formatter they may dispatch to, function
  references, strings naming a function, `dyn` calls (every
  `<type>_<method>`), autograd's builtins and the stubs' runtime functions
  (pe_stub_deps). The set is a hash map, one pass over each kept function.
- The runtime stays whole, so pe_ir resolves every builtin as before (a
  stub calls p_malloc and the like by name and takes pert_X when the module
  has it: dropping runtime functions could change which one it takes).
- Should pe_ir fail on the pruned module with an error naming a function
  the prune dropped (a pass called it by name, iropt's Vec_len for one;
  ir_dfe_root keeps those), the build is made again from every function
  and says so on stderr.
- `FID_PE_PRUNE=0` (or `FID_LOWER_ALL=1`) keeps every function. Only the
  pe_ir path prunes; the machine layer (`--nat`, Linux and macOS targets)
  and the C path are unchanged.

What it saves is the dropped functions' share of the module-wide passes
and of pe_ir's module-wide work (157 functions of hello.fd's module, by
iropt's count). The check script times both ways.

### What is left (not in the driver's files)

- Lowering, the largest stage, still turns all of core and the merged
  runtime into IR: compiler/reach.fd roots every function with a runtime
  prefix (pe_/pert_/pet_/ped_/p_/__). Rooting a `pert_X` only when a kept
  body or a pe_ir stub names `p_X` would leave out most of the 6,700 runtime
  lines for a small program; it needs the list of builtins pe_ir's stubs
  call by name (`PeirGen_call_builtin`, `PeirGen_call_runtime` and
  `func_labels` lookups: p_malloc, p_free, p_str_concat, p_print_str,
  p_eprint_str, p_panic, p_to_str*, p_argc, p_argv, p_tensor_to_str, ...)
  exported from pe_ir, so a stub never silently takes its machine version
  for a runtime function that was not lowered.
- pe_ir's own module-wide work is quadratic in places: pe_ir_reachable_from
  (`pe_strs_contains(seen, f)` for every edge), the string table
  (`pe_ir_strs_contains(all_strings, ...)`), and the `dyn` candidates
  (every function scanned for each DynCall). Hash sets there shorten
  `native code` for every build, pruned or not.
- `inc check`: the compiler hashes its whole executable once per build
  (compiler/inc.fd `inc_self_hash`, for the stamps and caches' keys): 30 ms
  for an 8 MB compiler, about 200 ms for a 41 MB one (measured through
  pe_ir's p_file_hash). A cache keyed by path, size and modification time
  needs `p_file_mtime_ms` in pe_runtime, linux_runtime and the
  interpreter (the C runtime has it).
- The front end's share (typecheck of the merged runtime, mono) belongs to
  the front end's roadmap items.

---

## Work stealing: per-worker lock-free run queues (competitive RT10)

Where: `stdlib/pe_runtime/conc.fd` "Run queues" (native Windows, pe_ir,
the default `fid emit`) and `stdlib/linux_runtime/conc.fd` "Worker rings"
(the machine layer: Linux AArch64 / RISC-V 64 / x86-64 and `--nat` Windows).

### Before

Every worker had a local queue, but all queues (local and global) were
linked lists under one lock (`pert_wq` in pe_runtime; one global list under
`pe_q_lock` in linux_runtime). A spawn, a yield, a pop and a steal each took
that lock, and a steal took one task.

### Now

- Each worker owns a bounded ring of 256 task slots, a Chase-Lev deque taken
  FIFO (as Go's `runq` and Tokio's local queue). Only its worker pushes, at
  `bottom`, with plain stores and one fenced store that publishes the slot
  (`pe_atomic_store` is `xchg`; `nat_atomic_store` is `stlr` + `dmb` / fenced
  on RISC-V). Its worker and thieves take at `top` by compare-and-swap, so a
  slot is claimed once; no lock is taken. FIFO rather than the paper's
  owner-LIFO: a task that yields goes behind the others on its ring.
- A worker whose ring is dry takes the global queue's head and its share of
  the rest (one worker's part, at most half a ring) in one lock hold, then
  steals half of a sibling's ring in one CAS: the slots are copied to its own
  ring's unpublished end, then claimed; a failed claim copies again. Victims
  are tried from a rotating start.
- A full ring moves its older half (claimed first, then read: only the owner
  writes slots) and the new task to the global queue in one lock hold.
- The global queue holds what threads that are not workers queue (the poller,
  the blocking pool, main), and in linux_runtime every sleeper (the scan
  that skips sleepers until they are due stays on the global queue) and
  everything in random mode (RL17).
- Parking: a worker counts itself idle (a locked add, a barrier), looks at
  the rings once more, and only then sleeps. A push publishes its slot before
  it reads the idle count, so either the pusher wakes the worker or the
  worker sees the task.
- Deadlock detector (pe_runtime): unchanged two-cycle rule; "queue empty" is
  now the global queue plus a sum over the rings. A task made but not yet
  queued, or run inline by `async` on a thread that is not a worker, no
  longer looks stuck (`pert_offq`): main spawning 200,000 tasks while the
  workers drained the queue panicked with "Tasks are deadlocked" before.
- Task registry (pe_runtime): a finished task's registry slot is in its
  block ([29]); finishing scanned the registry, which made tasks finishing
  out of spawn order quadratic (the tree phase below: 72 us a task at
  200,000 tasks, 4 us after).

The C runtime (`compiler/codegen_c.fd`) has no shared run queue to replace:
its tasks are a cooperative scheduler per OS thread (`P_THREAD_LOCAL`
`__p_cos`), so they never move between cores. Giving it M:N scheduling is a
separate, larger change.

### Tests and benchmark

- `tests/bootstrap/test_task_steal.fd`: fan-out from one task (5,000 tasks
  through a 256-slot ring: spills and steals), a spawn tree (8,191 tasks),
  3,000 tasks from main through the global queue, each yielding once.
  Prints counts and sums; a lost or duplicated task changes them, a lost
  wake-up hangs or panics.
- `tests/ctx_switch/steal_bench.fd [tasks] [work]`: the same three shapes
  with timings (ns per task) and a check of every result.

Run each many times and with several worker counts:

    $FID emit tests/bootstrap/test_task_steal.fd -o steal.exe
    for i in $(seq 50); do ./steal.exe | md5sum; done | sort | uniq -c
    for w in 1 2 3 16; do FIDES_WORKERS=$w ./steal.exe | md5sum; done
    FIDES_SCHED_RANDOM=7 ./steal.exe        # slow (random 1 ms sleeps), same output
    $FID emit tests/bootstrap/test_task_steal.fd --nat -o steal_nat.exe
    NATIVE=1 FID=$FID JOBS=4 bash scripts/cross-test.sh aarch64-linux

    $FID emit tests/ctx_switch/steal_bench.fd -o steal_bench.exe
    ./steal_bench.exe 200000 100
    FIDES_WORKERS=1 ./steal_bench.exe 200000 100

### Measurements (Nov 17, stage-0 compiler with today's stdlib)

Measured on a 16-thread Windows machine that was saturated by other builds
(several compilers at once, memory exhausted: some runs died with "out of
memory"), so these numbers are noisy and only show the order of magnitude.

`steal_bench.exe 200000 100`, pe_runtime, ns per task, three runs each:

| phase   | shared lock (before) | rings (now)          |
|---------|----------------------|----------------------|
| fan-out | 4538 / 5614 / 4988   | 5094 / 4350 / 4596   |
| tree    | 4057 / 3680 / 4724   | 4234 / 4000 / 4151   |
| global  | 15530 / 66765 / crash| 18124 / 36301 / 11501|

("before" here is the old queues with this change's registry and detector
fixes, so only the queues differ; without the registry fix the tree phase
was 72,000-108,000 ns per task, and the global phase always panicked with
a false deadlock.) At about 4-5 us a task the cost is dominated by making
and finishing a task (its stack, the registry lock `pert_reg`, the
`pert_done` broadcast at each finish), not by the run queues: the queue
change does not show above the noise here. To see the queues alone,
measure on a quiet machine, and then the next step is the per-task locks.

Correctness (pe_runtime): `test_task_steal` 20/20 runs identical, with
`FIDES_WORKERS` 1, 2, 3, 16 (5 runs each) identical, `FIDES_SCHED_RANDOM=7`
identical (112 s: random sleeps); test_tasks, test_task_handoff,
test_tasks_10k, test_runtime_sched_random, test_machine_tasks_once match
their expected output 5/5; the async tests and test_pe_task_order,
test_spawn_scope give the same output on 5 runs.

linux_runtime: built with `--nat` (the same machine layer on Windows
x86-64), `test_task_steal` 9/9 identical with the default and 1, 2, 3, 8
workers. That needed `FID_NAT_OFF=purecse`: the stage-0 compiler's pure-call
CSE (compiler/rules.fd) merges two `task_run()` calls (and two
`sync_atomic_load(g)` of the same global) in one function, so the second
`task_run()` returned at once, with the old queues as with the new ones.
The Linux targets were not run: stage 0 cannot build them with today's
runtime (`undefined symbol: nat_tls_set` from its x86-64 and AArch64 ELF).
