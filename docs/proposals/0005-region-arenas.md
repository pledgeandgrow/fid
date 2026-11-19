# 0005: Regions freed in one step

- **Status:** accepted; written for the C backend behind
  `FID_REGION_ARENA=1` (Nov 16). Answers competitive-roadmap OP29 and
  llvm-roadmap LF21, which were blocked on this design. The other backends
  keep regions as scopes (`p_region_word` is 0 there).
- **Decision:** accepted on 2026-11-16 with the recommended answers:
  D1 copy out automatically, D2 the region state saved per task (a region
  whose body awaits or yields gets no arena for now), D3 the C backend
  first.

### As built (what differs from the draft below)

Two parts of the draft were simplified while writing it; the invariant and
R1–R3, R6, C1–C3 and C5 are as described.

- **R4 and R5 became one flag.** Instead of a release log, each arena has
  a *dirty* flag. Keeping a block from outside the current region in it
  (`Str_dup`, `Vec_share`, `HashMap_share`, `Box_dup_t` on a block of a
  shallower arena or the heap) marks the region dirty, and so does any code
  that runs in parent mode inside it. A drop of a block whose region is
  still clean returns at once without visiting what it holds: nothing in
  that region can hold anything from outside it. In a dirty region drops
  run as before, and `p_free` of an arena block does nothing. Outside
  objects are counted exactly as without regions, so no log is needed.
- **C4 (adopting results) is not needed.** Parent mode covers a whole
  statement (`Lowerer_rg_stmt_out`), so the temporaries it makes are
  dropped in parent mode too. A value the statement keeps goes through
  `Lowerer_rg_keep_out`: an owner of its own (copied out of a deeper
  region by core), then the temporary is dropped.
- **Exits.** A `return` or `?` inside a region leaves its function's
  regions (p_region_word op 6) before its value is made. A pass after
  lowering (`lw_rg_finish`) saves the region state at the entry of each
  function with a region and restores it before every return, which ends
  the regions left open. `break` and `continue` end the regions opened
  inside the loop.
- **Calls** (C3) are wrapped after lowering: in each function region code
  can reach, a call to a function that may keep a value where no region's
  end reaches (a global, persistent storage, a call through a function
  value or trait object, a C function, an unknown builtin, or a function
  that does) runs with no region current. `FID_REGION_DEBUG=1` prints each
  such call.
- **Stores follow the container** (added to R3). While a region deeper
  than a container's is current, an element stored into it (`Vec_push`,
  `Vec_set`, `Vec_insert`, the `_mem` forms, `HashMap_insert`, and the C
  backend's inline `Vec_push`/`Vec_set`) gets an owner of its own made
  where the container is (`rg_fit`: a copy when the element is a deeper
  region's). Without it, a map made outside the region and filled inside
  it would hold the region's strings after its end. Functions with a
  region keep their Vec headers off the stack (LF13 is skipped there), so
  a region's Vec is the region's.
- **What counts as a change outside.** A method call on a type name
  (`HashMap::new()`) changes no variable. Assigning a number or a bool to
  a variable declared outside keeps nothing, so it stays in region mode.
- **Builtin.** One builtin, `p_region_word(op, x)`, carries everything:
  0 regions open, 1 the current one, 2 make one current, 3/6 leave regions
  (marking them dirty or not), 4 mark dirty, 5 clean?, 7 begin, 8 end,
  9/10 restore/save the state.
- **Also done (Nov 16, later):** a `with budget(N)` region gets a first
  chunk of N bytes and a quarter more (p_region_word(7, N)); `fid explain
  --alloc` names each region's start and end and each statement or call
  that runs in parent mode (checks.fd `explain_region_note`); a `?` on a
  `Result` whose type the lowering does not know keeps its regions' memory
  instead of releasing it (op 11: each block becomes a never-freed block of
  no region), so the error stays valid at the cost of a leak on that path.
- **Not done yet:** the native runtimes (nat_ir, pe_ir), waiting on the
  other session's allocator rewrite there; `--checked-alloc` closing a
  region's pages after its end. Raw pointers and `as i64` handles are not
  followed (§8.5).

## Problem

A `region` is a scope today (reference.md §8.6). Its locals are freed one
object at a time by the ownership rule (proposal 0004, release C6:
`lw_drop_function_vecs` at `Inst::RegionEnd`). Every backend treats
`RegionBegin`/`RegionEnd` as no-ops.

```fides
var names: Vec<str> = Vec::new();
for line in lines {
    region r {
        val fields = line.split(",");            // header, buffer, n strings
        val key = fields.get(0) + ":" + fields.get(1);
        names.push(key);                         // kept outside the region
    }
}
```

At each end of `r`, the drops walk `fields` and free its n strings, the buffer
and the header, one free-list push each. A region should cost one bump per
allocation and one release at its end, the way an arena does in C or Zig.

The C runtime already has an arena mark (`p_arena_save`/`p_arena_restore`):
it frees every chunk allocated after the mark. Wiring `RegionBegin`/`End` to
it is unsound in three ways:

1. **An outside value growing inside the region.** `names.push(key)` can
   reallocate `names`' buffer. The new buffer comes from the arena and is
   freed at the region's end while `names` still points at it.
2. **A value the region hands out.** `key` is made inside `r` and kept by
   `names`. Nothing checks that no value leaves a region (§8.6 says so).
3. **Other threads.** Their allocations during the region land in the same
   chunks (the mark is per-thread in C, but core's and the native allocators
   are shared).

## Change

Programs mean the same thing. Only where memory comes from and when it goes
back changes. One invariant does all the work:

> **While a region's arena is current on a task, every value made or kept
> belongs to a place declared inside the region.**

The compiler keeps the invariant by choosing, for each piece of code in a
region, which arena is current: the region's own (*region mode*) or the
enclosing one (*parent mode*: the next region out, or the heap). The runtime
does the rest. A backend that ignores regions (the interpreter, or any
backend before it is done) still frees object by object and prints the same.

### Runtime rules

- **R1. One arena stack per task.** `RegionBegin` pushes an arena (a chunk
  list and its depth); `RegionEnd` pops it. A task switch saves and restores
  the top of the stack in the task. A new thread or task starts with no
  region. Other threads never allocate in it, which fixes case 3.
- **R2. Fresh allocations bump.** In region mode `p_malloc` takes the next
  bytes of the current arena: an 8-byte header with its own magic
  (`P_ARENA_MAGIC | depth`) and the payload. There is no size class and no
  free list. A block bigger than a chunk gets a mapping of its own on the
  arena's list.
- **R3. Growth follows the object.** A buffer that replaces an existing
  object's buffer (a Vec or HashMap growing, a StringBuilder, a copy-on-write
  copy) comes from the arena that holds the object's header, not from the
  current arena. An outside Vec grown inside a region therefore stays on the
  heap, which fixes case 1. To find a block's arena in O(1), arena chunks are
  4 MB-aligned and keep their arena's record in their first word.
- **R4. Releases inside a region are deferred.** A drop or count decrement of
  a block in an open arena returns at once, without visiting what the block
  holds. That is the one-step part: dropping `fields` costs one compare, not
  a walk over its strings. In region mode, a release of an outside object is
  skipped too. R5 makes sure it still happens, once.
- **R5. Keeping an outside object logs it.** `Str_dup`, `Vec_share`,
  `HashMap_share` and `Box_dup` in region mode, on a block outside the
  current arena, add one owner as today. They also append the block to the
  arena's release log, which lives in the arena. `RegionEnd` pops the arena,
  releases each logged block once with no region current, and then frees the
  chunks.
- **R6. Keeping a region's value outside copies it out.** If the current
  arena is shallower than the arena holding the block, the same four
  functions copy it into the current arena instead of sharing it. That
  happens when code running for an outer place keeps a value of a deeper
  region that is still open. The copy is deep and follows the element type
  code, as `Vec_share_deep` already does; struct dups (`__own_dup_N`) recurse
  through them. This fixes case 2 with no new type rule.

R4–R6 live in core.fd (`Str_drop`, `Vec_drop`, `Vec_share`, `Str_dup`, …), so
they are written once for every backend. Each runtime adds two builtins:

- `p_region_depth()`, the current depth;
- `p_arena_depth(ptr)`, the depth of the block's arena, or 0 for the heap.

Where a backend has no arenas, both return 0 and every check falls through to
today's code.

### Compiler rules

- **C1. Every exit ends the region.** `RegionEnd` goes on every edge out of
  the region: `return`, `break`, `continue`, `?`, and a labelled break. Today
  it is emitted only on fall-through (`lower.fd`, `Stmt::RegionBlock`). An
  embedded `fides_call` that longjmps on a panic, and a cancelled task,
  unwind the arena stack to the depth recorded at entry.
- **C2. Escaping keeps run in parent mode.** Inside a region, a keep (let,
  assignment, field, element, return, payload: 0004's `Lowerer_rc_own_value`
  points) is escaping when its target is rooted outside the region:
  - a variable declared outside, a parameter, or a global;
  - a field or element of one of those;
  - a `return` or `break` value;
  - a channel send, `spawn` arguments, or a closure that escapes.

  Method calls on such a place with `&mut self` (`names.push(key)`,
  `insert`, `set`, `clear`) count too. The value expression is evaluated in
  parent mode and its value passes through R6. A handed-over temporary goes
  through `__p_keep_out(x)`, which copies it when it lies in a deeper arena.
  Allocas already carry `Region::Named` (`Lowerer_current_region`), so
  whether a place is rooted outside is known in the IR.
- **C3. Calls get a "retains" summary.** A function *retains* if it may keep
  a heap value somewhere other than its own locals and its result:
  - a global, a channel, or a spawned task;
  - through a given `&mut` parameter;
  - by calling an FFI function, a function value, or an unknown callee.

  The summary is computed bottom-up over the merged program, as iropt's
  effect summaries are. A call in a region runs in region mode unless it
  retains into a global, channel, task or FFI, or retains through a `&mut`
  parameter whose argument is rooted outside the region. Those calls run in
  parent mode. Parsing, formatting and arithmetic helpers stay in region
  mode, and that is where the gain is.
- **C4. Results of parent-mode calls are adopted.** A heap result of a
  parent-mode call that a region place keeps goes through
  `__p_region_adopt(x)`, which logs it as R5 does (no extra owner: it is
  handed over).
- **C5. Drops are lowered as today.** R4 makes them cheap at run time, so
  the drop lowering and every pass after it stay as they are.

### Raw pointers

A raw pointer or an `as i64` handle kept past its region is not seen (§8.5:
the programmer's responsibility). `fid run --checked-alloc` makes a closed
arena's pages inaccessible, so such a use faults instead of reading freed
memory.

### What the user sees

`fid explain <file> --alloc` lists each escaping keep (a copy-out) and each
parent-mode call, with the reason (a `&mut` argument rooted outside, a
global, FFI). That way a region that does not pay off can be fixed by
reading one report. A `with budget(N)` region can allocate its arena once,
at N bytes, and never grow it (a later step).

## What it breaks

Nothing compiles differently and nothing prints differently. Memory is
released at other times:

- A region's own values go back at its end, not at their last use. A region
  around a long loop holds every temporary of the loop until the loop ends,
  so its peak is higher. That is what a region means; reference.md §8.6
  says so. A region inside the loop body keeps the peak flat.
- An outside value that a region kept is released at the region's end, not
  at the region's own drop.

## Alternatives

- **The time-based arena** (`p_arena_save`/`restore` around the region) is
  unsound in all three cases above.
- **Reject every escaping keep at compile time** instead of copying out
  (R6/C2). That is a simpler runtime and the cost is visible. But it rejects
  programs that compile today (`names.push(key)` in a region), so it would
  need an edition. It can come back as a lint: `fid lint --perf` warns at
  each copy-out.
- **Pass the arena by owner**: allocation sites take an arena argument and a
  Vec header records its arena, with no mode. Callees' allocations then stay
  on the heap, a region container's children come from anywhere, and the
  drop walk cannot be skipped. Little is gained.
- **Region inference** (Tofte–Talpin, MLKit): regions in types and
  signatures. It is the most precise, and far too large a change for the
  type checker and every signature.

## Implementation

Behind `FID_REGION_ARENA=1` (off by default) until the suite and the leak
gate pass with it on.

1. **core.fd**: R4–R6 in the drop, share and dup functions, and R3 in
   `vec_buf_alloc` and the HashMap and StringBuilder growth paths, all over
   `p_region_depth`/`p_arena_depth`. Add `__p_keep_out` and
   `__p_region_adopt`.
2. **C runtime** (`codegen_c.fd`): the arena stack (thread-local), aligned
   chunks, `P_ARENA_MAGIC` blocks in `p_malloc`, the release log, and
   `RegionBegin`/`End` calls instead of the no-ops. Accept the arena magic
   in `p_free` and `p_str_adopt`, which today mask only the low byte of
   `P_BLOCK_MAGIC`. Under `P_LEAKS`, the leak count still ends at "every
   block was freed".
3. **Lowering** (`lower.fd`): C1 on every exit, C2 and C4 at the 0004 keep
   points, and a new instruction `Inst::RegionMode(name, parent)` around
   parent-mode code. Every backend must accept it (a no-op where regions are
   no-ops).
4. **New module `compiler/region.fd`**: the C3 summaries over the IR call
   graph, and parent-mode switches inserted around retaining calls.
   `fid explain --alloc` reads its results.
5. **Native runtimes**: `stdlib/linux_runtime/base.fd` (`pert_malloc`, the
   task switch) for nat_ir, then pe_ir's allocator stub and
   `stdlib/pe_runtime`. The interpreter keeps regions as no-ops.
6. **Tests** (`tests/bootstrap/regions/`), each run with the flag on and off,
   with the same output, through C and the leak gate:
   - an outside Vec grown in a region (case 1);
   - copy-out by push, assignment, `return`, `?`, `break` value and
     `spawn` (case 2);
   - nested regions;
   - a region in a loop with a flat peak;
   - a callee retaining through a `&mut` parameter, a global, and FFI;
   - a task switch inside a region;
   - a panic through `fides_call`;
   - `--checked-alloc` faulting on a raw pointer used after the region.

   A benchmark compares the split/parse loop above with and without the
   region.

## Decisions to make

- **D1.** At an escaping keep: copy out automatically (R6, recommended,
  nothing breaks) or reject it at compile time (needs an edition)?
- **D2.** A task switch (`await`, `yield`) inside a region: save the arena
  per task (R1, recommended: one field in the task) or reject it inside
  regions for now?
- **D3.** Order of backends: C first (recommended: it has the chunks and
  the debug allocator to test with), then nat_ir, then pe_ir?
