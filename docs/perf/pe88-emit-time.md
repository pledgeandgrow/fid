# PE88: `fid emit` of a typical test in under 1 second

Roadmap: docs/roadmap/pe-roadmap.md item 88 ("Native compile of a typical
test in under 1 second"). The measure is the default Windows build,
`fid emit <file.fd>` through pe_ir, with an -O2 compiler on a quiet machine
and the caches warm (D9's checked-library stamp, the import and AST
caches). `--no-cache` only skips the "up to date" answer for an unchanged
output, so a timed run always builds.

## How to measure

    FID=release/fid.exe bash tests/tooling/pe88-emit-time-check.sh

The script builds tests/bootstrap/hello.fd and test_collections.fd (a
mid-size test: Vec, HashMap, strings) once to warm the caches, then `RUNS`
times (default 5) with `--time`, and prints the stage table of the best run
for each, with and without the prune below. It fails when a best total is
not under `LIMIT_MS` (default 1000). It warns when other compiler processes
are running: several fid or zig processes at once make a stage 10-200x
slower (AGENTS.md), and such numbers are not the target's.

## Where the time went (Nov 17, a -O0 compiler, a busy machine)

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

## What changed: pe88_prune (compiler/main.fd)

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

## What is left (not in the driver's files)

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
