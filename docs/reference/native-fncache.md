# The machine layer's per-function cache (CS6 / SP6)

`fid emit` on the machine layer (Linux AArch64 / RISC-V / x86-64, macOS,
`--nat` Windows; compiler/nat_ir.fd, section "CS6") keeps each function's
emitted bytes, its fixups by name and its debug rows in a cache file. A
later build replays a function whose content key matches instead of
allocating and emitting it again. A replayed image is the same bytes as a
fresh one (SP20).

## Where the cache is (nc_path_pick)

| Setting | Cache |
|---|---|
| `FID_FNCACHE=<file>` | that file |
| `FID_FNCACHE=mem` | this process's memory only (`fid serve`, SP7) |
| `FID_FNCACHE=0` / `off` / `no` / `none` / `false` | none |
| `fid serve` (g_nat_fncache_mem) | memory |
| `--no-cache`, `--no-fncache`, `FIDES_NO_CACHE=1` | none |
| an object file, `--stats` / `--remarks`, dump-mir, `FID_NAT_DUMP`, `FID_VERIFY`, `FID_NAT_PAR` | none by default (their output covers each function compiled; FID_NAT_PAR's workers do not run with the cache) |
| otherwise | `g_nat_fncache_path` when the driver set it, else `<output>.fides-fncache` beside the `-o` output of `fid emit` / `fid build` |

A debug build (`emit --debug`) already put it beside its output
(main.fd `debug_cache_setup`, SP13). pe_ir's own cache (B5) is
`<output>.fides-fncache.pe`.

## The key

An entry is used when the file's salt and the function's key match:

- salt (`nc_cfg_salt`, `nc_mod_salt`): the target, `--cpu`, the pass
  switches (FID_NAT_OFF / ON, the mid-end's), `--size`, the fast
  allocator, whether the build has line tables (not the input file's
  path), the libfacts file, **the compiler itself**
  (`inc_compiler_hash`: another fid emits other bytes) and **the
  `--profile-use` profile** (its block layout), plus the module's struct
  layouts, globals, externs, trait tables and dyn types;
- key (`nc_key_of`): the function's IR after `IrFunction_form_switches`,
  its source lines included (`nc_fn_hash`: its line rows and panic-line
  stubs are its recording's), and whether it gets line rows.

Since Nov 27 (salt `nc16`) the function set is not in the salt: an edit
that made the program reach one more runtime function, or one saved
under another file name, hit nothing. What a function's code takes from
the set is in its label log: each `ng_has_fn` answer (`H1:f` / `H0:f`)
and each callee's convention (`ng_internal`, `I1:f` / `I0:f`), checked
per entry with its refs (`nc_names_hold`). A function an edit moves to
other lines (one inserted above it) is compiled again; one whose lines
stay is replayed (making the key line-relative needs the fold keys and
the debug variables relative too: not done).

The cache file format is 8 (entries carry their deps, below).

## What a function's code takes for granted (SP6)

A function's bytes depend on more than its own IR. Each entry records
(`nc_note_deps`, at capture):

- for each function it refers to (`deps`), the clobber mask its register
  allocation saw for it (`ng_label_mask`, IPRA; -1: every register) and
  that function's struct result in registers (ME27 / LF24; 0: memory);
- its own struct result in registers (`rr_self`);
- the functions whose address it takes (`addrs`, noted by `ng_rr_plan`).

Before the first lookup of a build, `nc_validate` keeps the entries whose
key matches and whose deps still hold, a greatest fixpoint over them:

- a dep that is kept has the recorded mask (unless the code assumed every
  register) and the recorded result convention;
- a dep compiled again: it must not be able to take a result in registers
  the code did not expect (`nc_may_rr`, a superset of `ng_rr_plan`'s
  choice); its mask is known only after its allocation and is checked at
  the replay (`nc_masks_hold`; the post-order allocates callees first);
- a dep outside the program: a library's recorded mask (WP20).

What only the recorded program as a whole tells is checked once known:

- `ng_rr_plan`: a replayed function with its result in registers whose
  address is now taken, or whose name a string now names (`ng_named_fn`),
  cannot keep that convention;
- `nc_replay`: a short RISC-V call (TG6, `JAL ra`) whose callee is not
  placed before it within 1 MB any more;
- `nc_masks_hold`: a callee compiled again whose mask is not what the
  replayed code was allocated against.

Each gives up that entry alone (`nc_give_up_callers`; before Nov 27 it
gave up, transitively, every kept entry that recorded anything of it,
so one edited function took its callers up to `main` with it):
`nat_generate_image` makes the image again without it, and what its
callers took for granted of it is checked against the function compiled
again (its convention in `nc_validate`, its mask and escape summary at
their replay), so a caller is compiled again only when its assumption
changed, one level per attempt. Each attempt replays what the attempts
before it compiled (`g_nc_carry`, `nc_carry_keep`): a function is
compiled once per image. At most 8 attempts, then one with no hits.
`FID_NAT_OFF=ncnarrow`: the transitive give-up, two attempts.
`--stats` (with `FID_FNCACHE` set, or in `fid serve`) prints

```
[fncache] entries from file, H hits, N compiled, cache written
[fncache] attempts: 3 (4 functions compiled by an earlier one replayed)
[fncache] stale (a callee changed): f, g
[fncache] built again without: mk, use_mk, main
```

H counts the functions replayed from the cache only; N those compiled
in this image, by any attempt.

So struct results in registers (`ng_rr_plan`) and RISC-V short calls are
on under the cache, as without it. MI28's tail-call summaries too (SP20,
cache format 6): each entry keeps its own escape summary (`ft_self`,
`nc_note_ft`) and the one it read of each dep (`dep_ft`); a function
compiled again reads a replayed callee's from its entry
(`ng_ft_esc_map`); `nc_deps_ok` checks the kept deps' summaries,
`nc_ft_hold` (at the replay, as `nc_masks_hold`) the ones compiled again,
and `nc_give_up_callers` follows `dep_ft` too. Before, `ng_tail_taint`
took every call to escape under the cache, so a cache-on image had calls
where a cache-off image had jumps. Left as it is: a summary group past
64 functions or 32 rounds counts as escaping, and the group a function
falls in depends on the order the summaries are asked for, which the
cache changes (docs/reference/tail-calls.md). Check:
`tests/nattools/sp20_fncache_tail_check.sh` (an edit that changes a
callee's summary under a replayed caller).

## Checks

- `tests/nattools/machine_fncache_check.sh [fid]`: miss / hit identity for
  test_machine_fncache_deps.fd, test_machine_fncache.fd and
  test_repro_fncache.fd on each target; results in registers and short
  RISC-V calls in use under the cache; the default cache file; an edit
  (nat_fncache_edit_a.fd then nat_fncache_edit_b.fd on the same cache
  file: at most 10 functions compiled again, the image the one a build
  without the cache makes). Nov 27, on the -O2 build of 16:19: all passed
  (the edit reuses 273, 273 and 274 functions on x86_64, AArch64 and
  RISC-V and compiles 2; B on A's cache is B without the cache).
- `tests/nattools/machine_batch7_check.sh` (SP6/SP20, SP7) and
  `scripts/repro-check.sh` (cache miss / hit = off).
