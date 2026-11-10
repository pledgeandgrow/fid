# reach.fd — which functions `emit` lowers

- **Path:** `compiler/reach.fd`
- **LOC:** 305 (2026-11-10)
- **Role:** compile time (competitive-roadmap CS4). Computes on the AST the
  set of functions a native build can reach, so lower.fd lowers only those.
- **Added:** 2026-11-10 by the native-async session (uncommitted at the
  time of writing).

## Callers

| File | Line | Mechanism |
|------|------|-----------|
| `compiler/main.fd` | :52 | `use compiler::reach` |
| `compiler/main.fd` | :2856 | `lw_keep_set` wraps `reach_keep_set(program, core_names)` |
| `compiler/main.fd` | :3137, :3235 | sets `g_lower_keep` for nat_ir emits and the Windows pe_ir emit; reset to 0 afterwards |
| `compiler/lower.fd` | :2831-2835 | `g_lower_keep` (0 = lower everything) and the `lw_keep` test in the function and impl loops |

Reached from: `fid emit` / `fid build` to Windows PE (pe_ir), the Linux
targets and `--nat`. Not from the C path (`ccompile`, `dump-c`) or `run`,
which lower every function. `FID_LOWER_ALL=1` turns it off.

## What it keeps

- Roots: `main`; every core function; every runtime function (`pe_`, `pert_`,
  `ped_`, `pet_`, `nat_`, `p_`, `__` prefixes, called by name by lowering,
  the backends and the C runtime); trait impl methods (dynamic dispatch);
  comptime functions; any function with an annotation.
- A kept body marks every name it mentions: calls (also `A::b` as `A_b`),
  method names (every impl method of that name), identifiers (function
  values) and string literals (functions started by name, such as
  `task_go("f")` and spawns).
- The ownership helpers (`__own_*`, `__p_mem_*`) are generated after the
  bodies from what was lowered, so they follow the kept set.

## Findings

- **INFO (reported by its author):** 24 of 30 compared binaries are
  byte-identical with and without it; the other 6 differ only in naming and
  give the same output. Hello world compiles in 0.53 s instead of 11 s,
  together with the other compile-time changes of Nov 10.
- **LOW (code inspection):** the set is conservative by design (any method
  name, any string literal that names a function). A function reached only
  through a name built at run time (a computed string) would be missed;
  none was found.

## Verification state

Not re-checked by this audit beyond reading the file header and its callers.
