# inc.fd — build caching: stamps, artifact store, checked core, in-memory IR

> **Re-audited 2026-11-09** against the working tree (inc.fd has a small
> uncommitted edit: FID_IROPT_*/FID_NAT_OFF added to the build key).

**Path:** `compiler/inc.fd` — **889 lines** (166 on Oct 22). Imports `ir`.

**Role:** four caches plus helpers.

1. **Build stamps** (`<output>.fides-stamp`, format `fides-stamp-fd 2`):
   the key (`inc_build_key`: compiler hash, command, arguments except
   `--no-cache`, `CC`, `FIDES_CC_OPT`, `FID_HOME`, `FID_IROPT_OFF`,
   `FID_IROPT_ON`, `FID_NAT_OFF`), every file read (`inc_record_file`) with
   its hash, every import candidate found missing (`inc_record_absent`),
   the C hash and the output hash. `inc_up_to_date` skips the whole build;
   `inc_c_unchanged` skips the C compiler.
2. **Artifact store** (`<dir>/.fidcache/<key>.exe`): keyed on a canonical
   hash of the generated C (`inc_canon_hash`: comments and whitespace gone,
   defined functions renamed by position, unreferenced ones dropped) plus
   toolchain and flags. Used for whole binaries and per-unit `.o` files of
   split C. LRU eviction past 64 files or 1 GB (`inc_cache_evict`).
3. **Checked core** (release D9): an empty file in TEMP/TMPDIR named by
   core's hash and the compiler's hash; when present, core's bodies are not
   checked again. `FIDES_CHECK_CORE` forces a check.
4. **In-memory IR cache** (`g_cache`): `IrModule` by path and source hash,
   now also valid only while the recorded inputs are unchanged.

Helpers: `hash_source` (FNV-1a), `env_or_empty` (null-safe `p_getenv`, used
across the compiler), `inc_compiler_hash` (hash of argv[0] or the PATH
entry), `inc_c_fn_defs` (C definition spans for `split_c_units`).

## Callers

| Caller | Site | What |
|--------|------|------|
| compiler.fd | :1116, :1120, :1171, :1339, :1344 | record the program, core, runtime files, imports, missing candidates |
| compiler.fd | :1653, :1775 | checked-core stamp (only marked when borrowck is `on`) |
| compiler.fd `ccompile_source_with_path` | :1807, :1841 | in-memory IR cache |
| compiler.fd `split_c_units` | :4371 | `inc_c_fn_defs` |
| main.fd `cmd_ccompile` | :1842-1875, :2053-2142 | stamp read/commit, artifact get/put |
| main.fd split-C link | :1679-1688 | per-unit `.o` artifacts |
| main.fd `cmd_build` (`emit`/`build`) | :3123-3184 | stamps for the pe_ir path (C hash 0) |
| main.fd | :2487 | compiler hash in a JSON report |
| pe.fd, pe_ir*, hotreload | many | `env_or_empty`, `hash_str` only |

Reached from `ccompile`, `compile`, `dump-c`/`split-c` (recording),
`build`/`emit`, `run` (front end records inputs and uses the core stamp).

## Findings — verified live

### MEDIUM — env vars that change the build are missing from the key

`FID_VERIFY` is not in `inc_build_key`: with `FID_VERIFY=1`, `emit` to an
output built before printed "a9.exe is up to date" and ran no verifier.
By reading: `FIDES_CFLAGS` (used in the artifact key, main.fd:2061, but not
in the stamp key), `FID_RULES_ON`, `FID_NAT_ON`, `FID_LIBFACTS`,
`FID_LIBFACTS_IN` and `FIDES_COW_CHECK` also change output or checks
without changing the key, so a stale output is reported up to date.

## Findings — old doc re-checked

- F1 cache blind to imports: **FIXED**. Stamps and the in-memory cache check
  every recorded file and missing path (`inc_deps_unchanged`).
- F1 cache blind to flags: CHANGED. Stamps include the command line (so
  `--borrowck=...`) and some env vars (see MEDIUM). The in-memory cache is
  still keyed on path and source hash only (compiler.fd:1807), so two
  in-process `ccompile` calls with different `borrowck_mode` share a module.
  LOW, since one process compiles once.
- F2 per-process only: CHANGED. Stamps, artifacts and the core stamp persist
  on disk; only `g_cache` is per process.
- F3 adaptations: STILL TRUE. Lazy `g_cache` init (`ensure_cache_init`),
  `IncrementalCache_clear` pops in a loop, `g_cache` entries are never
  evicted.

## Findings — code inspection

### LOW — canonical hash ignores identifiers inside preprocessor lines

`inc_lex_c` makes a `#...` line one non-identifier token, and liveness
starts from `main` and identifiers outside definitions. A function used
only from a macro body is dropped from the hash, so a change to it alone
would reuse a stale artifact. Today codegen prototypes every function in
the prelude, which keeps them live. The artifact key does not include the
compiler hash either; it relies on identical canonical C meaning an
identical binary.

### LOW — `inc_stamp_read` trusts `p_file_read`

:241 reads the stamp after `p_file_exists` but does not test the result for
null (AGENTS.md item 18/25); an unreadable stamp would crash in
`str_split`. `inc_cache_evict` does test it.

### INFO

- `inc_canon_hash` leaks two HashMaps per call (`def_index`, `numbers`).
- `inc_artifact_path` always uses `.exe`, also for `.o` units and non-Windows
  targets (name only).
- `inc_self_hash` returns 0 when argv[0] cannot be found; then the core stamp
  is never used and every key shares compiler hash 0.

## Fix order

1. Put every output-changing env var (or a list read from one place) into
   `inc_build_key`; add `FID_VERIFY`.
2. Null check in `inc_stamp_read`.
3. Add the compiler hash to the artifact key.

## Verification state

All 889 lines read. Live: the stale "up to date" with `FID_VERIFY` (s18.exe,
emit). Stamps and artifacts were produced by the probes; eviction and split-C
`.o` reuse not exercised.
