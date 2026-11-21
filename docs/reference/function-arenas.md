# Arena windows for one function's compiler data (SP11)

Roadmap: docs/roadmap/llvm-roadmap.md, SP11, "Arena allocation for
compiler data per function, freed in one step".

## The mechanism (compiler/inc.fd)

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

## Where the compiler uses it

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

## Measuring it

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

## Results

Not measured yet: the compiler with this change had not been built when it
was written. Add the table from fn-arena-emit-check.sh here.

Nov 21 (roadmap SP11 check, fid built from a self-hosted stage 0):
fn-arena-check passes (321 windows back at their mark), and `FID_VERIFY=1`
arena builds give the same images on 3 targets. fn-arena-emit-check fails
3: the native emit of compiler/main.fd does not finish (an iro_sccp
overflow panic, known-bugs 2g.89, or out of memory), so the table is still
missing.
