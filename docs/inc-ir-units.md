# C per IR unit (`fid ccompile --inc`)

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
