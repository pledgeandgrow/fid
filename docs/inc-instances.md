# Where `--inc` puts generic instances and per-type helpers

`fid ccompile --inc` splits the generated C into one unit per source module
(`compiler/compiler.fd` `split_c_units`). Each unit is compiled to an object
keyed by its text, so a unit is rebuilt only when its text changes. This page
records where the functions that belong to no single module go
(docs/roadmap/inc-roadmap.md B2 and B3).

## Generic instances (B3)

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

## Struct layouts, FFI externs and `==` helpers (B2)

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
