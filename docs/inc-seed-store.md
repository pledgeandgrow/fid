# The seeded store and globals in per-module builds

`fid ccompile --inc` builds a program per module: each unit (one per source
module, compiler/compiler.fd `ccompile_inc_prepare`) is compiled to an
object keyed by its input, kept in `<output dir>/.fidcache`, and linked.
This note covers two things done for inc-roadmap B6 and B8.

## B6: a seeded store shipped with the toolchain

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

### Filling it

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

### Why core's object fits a new program

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

## B8: globals in a per-module build

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
