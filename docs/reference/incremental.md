# Incremental compilation

Consolidated Nov 22 from docs/inc-ir-units.md, docs/inc-instances.md, docs/inc-seed-store.md, docs/import-cache.md, docs/incremental-check.md and docs/reference/incremental-relink.md: per-module `ccompile --inc` builds, the serve-side caches (CB1, CB2) and in-memory native relinking (SP7).

## C per IR unit (`fid ccompile --inc`)

inc-roadmap B1 (with B2 at the IR level). Before, `ccompile --inc`
generated the whole program's C on every build and cut the text into one
translation unit per source module (`split_c_units`); only the C compile of
an unchanged unit was saved.

Now the lowered program is cut into lower.fd's `IrUnit`s
(`ir_units_split`: one per source module) and each unit's C is generated
from that unit alone:

- its own functions;
- a stub of every function it imports: name, parameters and return type, an
  empty body. The C generator writes the stub's prototype and casts calls
  by it; the stub's own C is dropped. Imports are the calls and function
  references of `IrUnit.imports`, the targets of `__c_fn_` adapters, every
  `<Type>_<method>` a dynamic call can dispatch to, and the C callbacks;
- only the tables the C generator reads: extern declarations, C callbacks,
  globals, and only the struct layouts the unit names (found in the unit's
  ir_ser bytes, then through the layouts' fields).

The unit owning `main` also writes the tail (C `main`, the runtime, the
function table of panic traces and lookups by name). It has a stub of
every function and lists and registers all of them
(`set_c_register_all`), so its C depends on the other units' signatures,
not on their bodies. A function calling the library loader (`p_dl_*`) keeps
its body there, since the tail writes the loader only for such a program.

Each object is stored under the hash of the unit's generator input (its
ir_ser bytes, the bytes of its layouts) and the build's flags
(`inc_build_key`, `FID_SWITCH`, `FID_C_CT_OFF`, the `FIDES_INC_*`
switches). A unit whose key has an object is neither generated nor
compiled. A generated unit whose C already has an object (the key the text
path uses) is not compiled either. A build whose keys all match the last
build's is "C translation unchanged".

Generic instances are placed at their callers at the IR level
(`incu_place_instances`, as `c_place_instances_at_callers` does on the
text; `FIDES_INC_INSTANCES=home` keeps them home). The static per-caller
copies of `c_copy_instances_to_callers` are not made on this path.

The text of one unit is still passed through `split_c_units`, with the
units' assignment (`c_unit_of_fn`) and only that unit kept
(`g_split_only`). It moves the prelude's definitions to main's unit, keeps
globals' definitions there (`extern` elsewhere), and scopes prototypes,
literals and layouts to what the unit's text names.

Code: compiler/compiler.fd, section "B1 (inc-roadmap): C per IrUnit"
(`ccompile_inc_prepare`, `incu_unit_c`); compiler/main.fd
`build_c_units_ir`, `cmd_ccompile`.

Switches: `FIDES_INC_IR=off` (or `FIDES_INC_FLAT=1`) brings back the
old path. `FIDES_INC_DEBUG=1` prints `[inc] ir unit <name>: f functions,
s stubs, l of L layouts` for each unit, `[inc] ir instances: ...`, and
`[inc] ir units: N units, g generated (names; ms), r reused by their IR key`.

Checks: `bash tests/tooling/inc-ir-units-check.sh`, `scripts/inc-check.sh`
(`SUITE=1`, `FIXPOINT=1`), tests/tooling/inc-layouts-check.sh,
tests/tooling/inc-instance-check.sh.

Known bug (Nov 21, 2026, known-bugs 2g.92): on this path a `dyn` call whose
impl is in another unit gets no dispatch candidates, and the program aborts
(inc-ir-units-check.sh exits 127 after its third line); `FIDES_INC_IR=off`
works. Also on this path: no `[inc] layouts` debug line, and a new struct in
a library unit rebuilds main's unit.

## Where `--inc` puts generic instances and per-type helpers

`fid ccompile --inc` splits the generated C into one unit per source module
(`compiler/compiler.fd` `split_c_units`). Each unit is compiled to an object
keyed by its text, so a unit is rebuilt only when its text changes. This page
records where the functions that belong to no single module go
(docs/roadmap/inc-roadmap.md B2 and B3).

### Generic instances (B3)

An instance (`f__T`, or a method instance `S__m__T`) starts in the unit of
the module that defines the generic (`func_module_of`). Then two passes move
it:

1. `c_place_instances_at_callers`: when every function that names the
   instance is in one unit other than the generic's own, the instance moves
   to that unit.
2. `c_copy_instances_to_callers`: when two or more units call it and the
   generic's own unit does not, its one external definition goes to the
   calling unit whose name comes first. Every other calling unit gets a
   `static` copy. `split_c_units` adds `static` to the copy's definition line
   and to that unit's prototype of it. The instances a copy calls are placed
   the same way: a call from a copied function counts for every unit that
   holds it. This repeats until nothing changes (16 rounds at most).

The result: a new instantiation of a generic of `gen` (or core) from `lib1`
and `lib2` rebuilds `lib1`, `lib2` and main's unit, but not `gen`'s or
core's unit. Main's unit is rebuilt because its tail lists every live
function in `__p_fns`.

Instances that are never copied:

- An instance whose address is taken. That means its name appears anywhere
  in the C other than in a call, a declaration or its own `__p_fns` row, for
  example as a function value or in a table. Two copies would have two
  addresses, and a comparison of function values could then differ.
- An instance that the generic's own unit calls. It stays there, and other
  units call that one.
- Any instance when prototypes are not scoped per unit
  (`FIDES_INC_EFFECTS=off`, `FIDES_INC_FLAT`), because every unit would then
  carry its external prototype.

Each instance still has exactly one external definition, and `__p_fns` names
that one. A panic trace therefore names a frame in the external definition.
A frame in a `static` copy shows no name in a trace. This is the cost of the
copies.

Switches:

- `FIDES_INC_INSTANCES=move`: only pass 1 (no copies).
- `FIDES_INC_INSTANCES=home`: neither pass; every instance stays in its
  generic's unit.
- `FIDES_INC_DEBUG=1`: prints `[inc] instances: ...` (pass 1) and
  `[inc] instance copies: N static copies of M of K instances (A
  address-taken, never copied)`.

Check: `bash tests/tooling/inc-instance-copies-check.sh` (copies and the
`move`/`home` controls) and `bash tests/tooling/inc-instance-check.sh` (pass
1).

### Struct layouts, FFI externs and `==` helpers (B2)

- Struct definitions and FFI extern declarations are scoped per unit like
  prototypes and string literals. A unit keeps a block when its text, a
  prototype it carries, the shared prelude or another kept block names it.
  `FIDES_INC_LAYOUTS=off` turns this off. `FIDES_INC_DEBUG=1` prints
  `[inc] layouts <unit>: a of b structs, c of d externs`.
- The `==` helpers that the compiler synthesizes for each non-generic struct
  and enum (`__eq_<S>`, `__eq_<Enum>`) go to the unit of their type's
  module. `record_func_modules` registers them. Before this they went to
  main's unit, so adding a struct anywhere rebuilt main's unit.
  `FIDES_INC_EQ=main` restores the old placement.
- What still goes to main's unit: the numbered ownership helpers
  (`__own_dup_N`, numbered across the whole program), and the tail's
  `__p_fns` table, which changes when a live function is added or removed.
- An `#include` or `// PLINK:` line is shared by every unit. Adding the
  first extern of a header or library to the program therefore still
  rebuilds every unit. A second extern of a header the program already
  includes does not.

Checks: `bash tests/tooling/inc-layouts-check.sh` (structs) and
`bash tests/tooling/inc-layouts-ffi-check.sh` (externs and `__eq_`).

## The seeded store and globals in per-module builds

`fid ccompile --inc` builds a program per module: each unit (one per source
module, compiler/compiler.fd `ccompile_inc_prepare`) is compiled to an
object keyed by its input, kept in `<output dir>/.fidcache`, and linked.
This note covers two things done for inc-roadmap B6 and B8.

### B6: a seeded store shipped with the toolchain

An installed toolchain reads a second, read-only store of unit objects:
`cache/` beside `fid.exe` (compiler.fd `fid_seed_store_dir`). A build looks
an object up in this order (inc.fd `inc_artifact_get`):

1. the local `.fidcache` next to the output;
2. the seeded store (`inc_seed_get`);
3. the remote store named by `FIDES_CAS`.

A build never writes into the seeded store and never evicts from it.
`FIDES_SEED_STORE=<dir>` names another store; `FIDES_SEED_STORE=off` turns
it off. With `FIDES_INC_DEBUG=1` each hit prints `[inc] seeded store:
<name>`; a build that took objects from it prints `Took N of M units from
the seeded store`.

#### Filling it

```
fid ccompile <file.fd> --inc -o <out> --seed-store <dir>
```

builds the program as usual and also copies every unit's object except
main's into `<dir>`, named as in a `.fidcache` (main.fd
`build_c_units_ir`, inc.fd `inc_seed_put`). It prints `Seeded N units
into <dir>: <units>`. The installer runs it on `tests/bootstrap/hello.fd`
and the examples with `release/fid.exe`, the compiler it ships (an
object's name holds the compiler's hash, the C compiler's name and its
flags).

#### Why core's object fits a new program

A unit's key is the hash of what its C is generated from. For core's unit
to have the same key in every program, nothing of the rest of the program
may be in it:

- the unit key holds the build's arguments but not the input's and
  output's names, `--inc`, `--seed-store <dir>`, CC (the C compiler is
  part of the object's name already) or FID_HOME (inc.fd
  `inc_unit_build_key`). `-g`, `--leaks` and `--mem` write the input's
  name into the C: then the names stay in the key;
- a unit other than main's carries only the globals its functions address
  and the C functions it calls (`ccompile_inc_prepare`;
  `FIDES_INC_SCOPE_DATA=off` gives every unit all of them, as before). In
  its C text the same holds: another unit declares only the globals it
  names, with only their types' layouts (compiler.fd `split_c_units`);
- core's dispatches over the program's struct and enum types
  (`__p_elem_drop`, `__p_elem_dup`, `__p_mem_drop`, `__p_mem_dup`: lower.fd
  writes their bodies for each program) live in main's unit; core keeps a
  prototype (`FIDES_INC_DISPATCH=core` leaves them in core).

What still makes a unit differ between programs: a generic instance that
units of two different modules call stays in the unit of its generic
(inc-roadmap B3), so a program whose modules share an instance of a core
generic has its own core unit. A program that edits core never hits.

Check: `bash tests/tooling/inc-seed-store-check.sh` (FID=<fid>, CC=<cc>).
Known bug (Nov 21, 2026, known-bugs 2g.91): a fresh project never takes
core's unit from the seeded store (steps 3 and 4 of that check fail: core's
unit key differs between the seed build and the fresh project). Seeding and
`FIDES_SEED_STORE=off` work.

### B8: globals in a per-module build

Two bugs of `split_c_units` broke every program with a module-level global
when built with `--inc`:

- main's unit defined each global without its initializer: the line
  `static int64_t _g_x = 7LL;` became `int64_t _g_x;`, so `var x: i64 = 7`
  read 0 and a `str` global read "". Main's unit now keeps the
  initializer; the other units declare the global `extern`.
- the function that runs the other initializers (`__init_globals`:
  `Vec::new()`, a map, a computed value) is `static`, and the split kept a
  copy of every `static` function in every unit's prelude, before the
  globals and the prototypes it names. Every program with such a global,
  or importing a stdlib module with one, failed to compile ("use of
  undeclared identifier '_g_...'", "call to undeclared function
  'Vec_new'"). Now only main's unit has it, after the globals' definitions,
  with the prototypes and string literals it names.

Checks: `bash tests/tooling/inc-globals-check.sh` and
`tests/bootstrap/test_inc_global_inits.fd` (built both ways by
`SUITE=1 scripts/inc-check.sh`).

## Incremental checks in `fid serve` (CB2)

A long-lived `fid serve` (and an LSP process, which turns the module
cache on) checks a program again after an edit without checking every
function body again. Only the bodies the edit can reach are checked. The
code is in `compiler/compiler.fd`, section "CB2: incremental body checks in
`fid serve`". It is driven from `compiler/main.fd` `serve_request` and the
socket `check-text` request, and from `check_text_errors` when the module
cache is on.

### What is kept

After a check request that passes (type check, reference escapes, secrecy,
borrow check), the server keeps the modules of the program: the file of
each one, its text, and its top-level items. An item is a function, an
`impl` or trait-impl method, a declaration, or an import or module line or
`#[..]` attribute. Each item is kept with:

- a hash of its tokens without their positions, so adding a line above it
  leaves it unchanged;
- the names it defines: the function or method name, the name after
  `struct`, `enum`, `cap`, `type`, `var`, `const` and so on, and the variants
  of an enum;
- the words it mentions: its identifiers, and the words of its f-strings.

### What the next request checks

The next request compares each module with that clean state:

1. An item whose hash changed, a new item and a removed item are
   *changed*. The names they define go into a pool.
2. An item that mentions a pooled name is *affected*: a signature, a
   struct, or a fact about a callee (its effects, whether it allocates or
   reaches C code, an escape summary) may have changed what its check sees.
   The names it defines go into the pool too, and this repeats to a
   fixpoint, because those facts flow up the call graph. `main` counts only
   where it is called or passed (`main(`, `(main)`), because `main` is a
   common word in programs.
3. A changed import or module line, or a `#[..]` attribute, changes every
   item of its module.
4. Every body of the program is checked when any of these changed: the
   program's own import lines or attributes (its capabilities), the
   settings (core and runtime, the borrow-check mode, the `@cfg` target and
   features, the edition), or a module without a file (a C header).
5. When any module spawns threads and the program's module is registered
   as `main`, every body of that module is checked. The race check reads
   the global writes the checker records for `main` while it checks those
   bodies.

Every other body whose key (`fn:f`, `impl:T:m`, `trait:T:m`) the program
defines only once goes into the skip set. The checker, the escape and
secrecy passes and the borrow checker already take this skip set for core
(release D9). The passes over the whole program still run on every request:
effects, the `verify_*` passes, lock order, races, escape summaries and
lifetime ties. Only the per-body checks are skipped.

A check request ends after the borrow check. Comptime substitution and
monomorphization run in a build but report no errors, so they are skipped.
Lints are skipped too, because the answer holds errors only. The request's
note says what was checked:

```
main.fd: no errors (41 ms, 2 module parses reused, 2 of 7 bodies checked)
main.fd: 1 error(s) (39 ms, 2 module parses reused, 2 of 7 bodies checked)
main.fd: no errors (812 ms, all bodies checked: first check)
```

### Switches

- `FID_CB2=off`: every request is a full build, as before CB2.
- `FID_CB2_DEBUG=1`: the server prints the changed, removed and checked
  keys on stderr (`cb2: changed main.fd fn:other`).
- `FID_SERVE_TIME=1`: the server prints each check's stage times on
  stderr (`[time] typecheck: 12 ms`). The `cb2 plan` row is the comparison
  itself.

### Checking it

`tests/tooling/cb2-incremental-check.sh` (with `FID=<compiler>`):

- **Part A** runs a three-module program through twelve edits, once with
  `FID_CB2=off` and once with CB2 on. Each request must give the same
  status and errors in both runs. The notes must show what was checked
  again. The edits include a signature change and a struct field rename in
  `geo.fd`, which make the unchanged `calc.fd` and `main.fd` report
  errors, and a callee that starts printing, which makes an unchanged
  `sandbox` block report an IO effect.
- **Part B** times `compiler/main.fd` on a snapshot of `compiler/`: a first
  check, an unchanged check, a statement added in one function, and a
  type error there. It prints each request's time, the bodies it checked
  and the stage times. The CB2 target is under 50 ms from the edit to the
  diagnostics. The script reports whether the target is met;
  `STRICT_TIME=1` turns a miss into a failure.

### Limits

- The passes over the whole program still run on every request: imports
  and the merge, type registration, the effect and `verify_*` passes, and
  escape summaries. On a program as large as the compiler they are what
  is left of a request's time. Making them incremental needs state kept
  across requests inside the type checker.
- An edit to a widely used function's body checks all of its transitive
  callers, because their facts may have changed. The checker's facts are
  not visible from outside it, so the server cannot see that they did not
  change.
- Warnings (lints, deprecated calls) are not part of a check request's
  answer.

## The import cache (CB1)

A compile of a large program spends much of its front end reading the
modules it imports: `fid check compiler/main.fd --time` reported 29 s of
`imports` out of 137 s (Nov 9, compiler built at -O0), nearly all of it
lexing and parsing the 60 compiler modules. The in-process modcache
(CB2) only helps a long-lived process (`fid serve`, `fid fix`, the LSP).
The import cache keeps each imported module's parse on disk, so a later
one-shot compile (`check`, `run`, `ccompile`, `dump-c`, `emit`, `build`)
neither lexes nor parses a module whose text has not changed.

### What is stored

`compiler/compiler.fd`, section "CB1: import cache on disk". When
`cached_parse` (the import loop's parse of a module) misses, the module is
parsed as before and its `Program` is written with the AST serializer
(`compiler/ast_ser.fd`, the one the core artifact uses) to
`fid-imp-<hash of path and compiler>.bin` in `FID_IMPCACHE_DIR`, else
`TEMP`/`TMPDIR`/`/tmp`. The file is written under another name and renamed,
so a reader never sees half a file. One file per module path: a new key
overwrites it.

### The key

A module's parse depends on its text and on what the parser knows when it
starts: the struct and enum names (`Name { .. }` is a literal or a block)
and the names modules are imported under (`name::f(..)` is a call of an
imported module's function or a method call). Only names that occur as
words of the module's text can change its parse. The key is the hash of:

- the text (FNV-1a, as `hash_source`),
- for each distinct word of the text, in order, whether it is a known
  struct, enum or import name (names that are not one word are all folded
  in),
- the compiler executable's hash (`inc_compiler_hash`): a rebuilt compiler
  starts over.

### Parser state

A parse also changes parser globals that later parses and passes read. On a
hit they are put back:

- the import-name set: the module's `use` names are parsed again as
  `use <name>;` lines (`imp_replay_names`);
- the `module::name` calls the parse recorded: kept with the entry and
  added to `g_imp_qcalls`, which `cc_qualify_shadowed_imports` reads before
  the parser's own list.

A parse that numbered a turbofish call (`f::<T>(..)`, global numbering),
saw `@secret`/`@sensitive` (the secrecy pass's fast skip) or an `@import`
of a C header is not kept. Because the parser's import-name set lives as
long as the process, the cache is used only by the first build of a
process, and never with the modcache. A failed parse turns it off for the
rest of the build.

### Switches and output

- `FID_IMPCACHE=off`: no read, no write (compare output with and without).
- `FID_IMPCACHE_DIR=<dir>`: where the files go.
- stderr: `[build] import cache: H hit(s), P parsed, S saved` after the
  imports of a build that used it.

The core artifact (CS2) is version 3 now: when nothing is merged into core
(`check`, `run`, `ccompile`, `dump-c`) it stores core once instead of the
same Program twice, so a hit decodes one Program.

### Checking it

`FID=<fid> bash tests/tooling/cb1-impcache-check.sh` (fixture
`tests/projects/cb1_impcache`): same output and same C with the cache off,
cold and warm; a warm build hits every module; a program knowing another
name that occurs in a module parses it again; an edited module is parsed
again; damaged files are ignored; the core artifact is written once and
read. `TIME=1` also times `fid check compiler/main.fd --time` cold and warm.

### Not done (CB1 is larger)

Type checking, comptime substitution, lowering and the C generator are not
cached or parallel, and builds are not incremental per function
(competitive CS1, CS4-CS6, complaint CB3). Decoding is not yet as fast as
it could be: `ar_i64` in `compiler/ast_ser.fd` allocates a Vec for every
integer it reads.

## Incremental relinking in memory (SP7)

A long-lived `fid` process (`fid serve`, or any driver that builds the same
program more than once) keeps the machine layer's per-function cache in
memory between builds (`g_nc_mem`, docs/reference/native-fncache.md). SP7
keeps the layout of the last build there too, so that the next build after
an edit does not lay every function out again: the functions that did not
change keep their bytes at their offsets, the changed ones go back into
their old places when they still fit, and only the references (fixups) are
resolved again.

Code: compiler/nat_ir.fd, section "SP7: incremental relinking in memory"
(`nl_*`), with its hooks in `ng_mir_finish` (the loop that places the
functions), `nc_replay` (its `at` argument), `nc_start` (`nl_reset`) and
`nat_generate_image1` (`nl_commit`, and the fresh retry).

### What a build keeps

After the loop that places the functions (`nl_loop_end`), a build whose
fixups then resolve keeps (`nl_commit`):

- what it was: the cache path, the cache salt (target, configuration,
  program layout: `nc_cfg_salt`, `nc_mod_salt`), the architecture and
  object format;
- where its functions start (`pos0`: the code in front of them, the
  primitives, is that long) and end;
- per placed function its start and its slot: from its start to the next
  function's start (the last one's to the end of the functions), so the
  alignment padding and any gap after it belong to it;
- the slot of a function of the layout before that this build does not
  place (an edit's smaller version was inlined away, or nothing reaches it
  any more): it stays the zeros it left. Before Nov 27 the function in
  front of it got that room, and a later build that had the function again
  (the edit undone) had to place it after the code
  (tests/nattools/relink_check.sh, third build).

### What the next build does with it

`nl_begin` uses the kept layout when this build has the cache on, the same
cache, salt, architecture and format, the same code in front of the
functions, and at least half of the kept functions. It then reserves the
kept code (zeros, as alignment padding is) and places each function, in
the usual order:

| Function | Where it goes |
|---|---|
| a cache hit whose bytes fit its slot | its recorded bytes replayed at its old offset (`nc_replay(g, e, at)`), its fixups registered against that offset |
| compiled again, fits its slot | emitted after the reserved code as in any build, then moved into its slot (`nl_move`: bytes, the labels placed in it, its fixups, line and frame rows); on x86-64 a function with a padded loop head (MI30) moves only by a multiple of 16, else it stays after the code |
| no slot (new), or outgrew it | after the reserved code, as in a fresh build; its old slot stays zeros (never executed) |
| in the layout, not in the image of the build before | its slot is still there: a hit is replayed into it, and one compiled again moves into it when it fits |
| folded onto identical code | as before: its label points at that code |

Every fixup is then resolved by `NatEnc_resolve` as in a fresh build: a call
to a function that moved reaches it at its new place, and the unchanged
functions' bytes are not touched except where they refer to something that
moved.

The panic-trace table (`[address, name]` pairs a panic searches), the
symbols, the DWARF line program and the frame rows (.debug_frame,
.eh_frame, PE unwind data) want address order. With a kept layout used,
`nl_loop_end` sorts the placed names by start and the line and frame rows
by the start of the function that made them (a function's rows keep their
order; a hit replayed into its slot ends its line sequence at its own end,
`nl_line_close_at`).

### When it starts afresh

- RISC-V: its short calls (TG6) and RVC (TG9/TG19) depend on where every
  function is; object files; the parallel emission (never with the cache);
  `FID_NAT_OFF=relink`. `--stats` says why: `[relink] a fresh layout (...)`.
- A reference out of reach in the kept layout (a short branch to code that
  is now further away): `NatEnc_resolve` fails, the layout is dropped and
  the same build runs again laid out afresh (`nat_generate_image1`).
- A layout whose code is less than half used (many functions grew or went
  away) is not kept: the next build starts afresh and compact.

An image built this way is not the same bytes as a fresh build (functions
sit at other offsets, with gaps), but it runs the same. When nothing
changed, every function is a hit in its own slot and the image is the same
bytes as the build before (tests/nattools/machine_batch7_check.sh, SP7).

### `--stats`

```
[relink] a fresh layout (no earlier build in this process)
[relink] the last build's layout: 131 functions in place, 3 compiled again into their places, 1 placed after them
[relink] less than half of the kept layout is code: the next build starts afresh
[relink] the kept layout does not resolve (...): laid out afresh
```

`nat_relink_counts()` gives the last build's numbers ([used, in place,
moved into place, placed after]) and `nat_relink_start(name)` a function's
start in the kept layout, for a driver or a test.

### Checks

`bash tests/nattools/relink_check.sh [fid]`: one `fid serve` builds
tests/bootstrap/test_machine_relink.fd, then two edits of it
(tests/nattools/relink_edit_b.fd, relink_edit_c.fd) and the program again
from the same path, with `FID_VERIFY=1`, for x86-64 Linux, AArch64 Linux
and `--nat` Windows. It checks the `[relink]` counts of each build, that
each image runs as a fresh build of its source (output and exit code),
that the unchanged functions keep their addresses
(tests/nattools/relink_syms.py), that readelf decodes the line program,
and that `FID_NAT_OFF=relink` lays the code out afresh.

Nov 21 (roadmap SP7 check, fid built from a self-hosted stage 0): relinking
in memory works when the layout is kept (AArch64, build 4: 241 functions in
place, 1 compiled again), but `relink_check.sh` fails 10 checks: an edited
program never reuses the kept layout ("a fresh layout (the last build was
of another target, cache or program)"), 149 of 241 unchanged functions
move, and on x86-64 and `--nat` the changed function goes after the
others.
