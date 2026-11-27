[All modules](index.md)

# compiler::inc -- Fides Documentation

## Contents

- [hash_source](#hash_source) (function)
- [hash_str](#hash_str) (function)
- [inc_deps_reset](#inc_deps_reset) (function)
- [inc_record_file](#inc_record_file) (function)
- [inc_record_absent](#inc_record_absent) (function)
- [inc_deps_unchanged](#inc_deps_unchanged) (function)
- [IncStamp](#IncStamp) (struct)
- [env_or_empty](#env_or_empty) (function)
- [env_override_set](#env_override_set) (function)
- [env_override_clear](#env_override_clear) (function)
- [argv_at](#argv_at) (function)
- [argv_request_set](#argv_request_set) (function)
- [argv_count](#argv_count) (function)
- [argv_request_clear](#argv_request_clear) (function)
- [inc_compiler_identity](#inc_compiler_identity) (function)
- [inc_compiler_hash](#inc_compiler_hash) (function)
- [inc_exe_hash](#inc_exe_hash) (function)
- [inc_core_checked](#inc_core_checked) (function)
- [inc_core_mark_checked](#inc_core_mark_checked) (function)
- [inc_build_key](#inc_build_key) (function)
- [inc_unit_build_key](#inc_unit_build_key) (function)
- [inc_stamp_read](#inc_stamp_read) (function)
- [inc_up_to_date](#inc_up_to_date) (function)
- [inc_c_unchanged](#inc_c_unchanged) (function)
- [inc_stamp_commit](#inc_stamp_commit) (function)
- [inc_bytes_hash](#inc_bytes_hash) (function)
- [inc_stamp_commit_hash](#inc_stamp_commit_hash) (function)
- [inc_deps_files](#inc_deps_files) (function)
- [inc_deps_hashes](#inc_deps_hashes) (function)
- [inc_deps_absent](#inc_deps_absent) (function)
- [IncCacheEntry](#IncCacheEntry) (struct)
- [IncrementalCache](#IncrementalCache) (struct)
- [IncrementalCache_new](#IncrementalCache_new) (function)
- [IncrementalCache_get](#IncrementalCache_get) (function)
- [IncrementalCache_store](#IncrementalCache_store) (function)
- [IncrementalCache_clear](#IncrementalCache_clear) (function)
- [IncrementalCache_len](#IncrementalCache_len) (function)
- [IncrementalCache_is_empty](#IncrementalCache_is_empty) (function)
- [global_cache_store](#global_cache_store) (function)
- [global_cache_get](#global_cache_get) (function)
- [global_cache_clear](#global_cache_clear) (function)
- [CTokens](#CTokens) (struct)
- [inc_lex_c](#inc_lex_c) (function)
- [CDefs](#CDefs) (struct)
- [inc_find_c_defs](#inc_find_c_defs) (function)
- [inc_canon_hash](#inc_canon_hash) (function)
- [inc_unit_hash](#inc_unit_hash) (function)
- [inc_unit_file_name](#inc_unit_file_name) (function)
- [CDefSpans](#CDefSpans) (struct)
- [inc_c_fn_defs](#inc_c_fn_defs) (function)
- [inc_artifact_path](#inc_artifact_path) (function)
- [inc_artifact_get](#inc_artifact_get) (function)
- [inc_artifact_put](#inc_artifact_put) (function)
- [inc_set_seed_dir](#inc_set_seed_dir) (function)
- [inc_seed_hits](#inc_seed_hits) (function)
- [inc_seed_put](#inc_seed_put) (function)
- [inc_cache_evict](#inc_cache_evict) (function)
- [fn_arena_on](#fn_arena_on) (function)
- [fn_arena_begin](#fn_arena_begin) (function)
- [fn_arena_end](#fn_arena_end) (function)
- [fn_arena_keep_word](#fn_arena_keep_word) (function)
- [fn_arena_keep_words](#fn_arena_keep_words) (function)
- [fn_arena_keep_str](#fn_arena_keep_str) (function)
- [fn_arena_take_word](#fn_arena_take_word) (function)
- [fn_arena_take_words](#fn_arena_take_words) (function)
- [fn_arena_take_str](#fn_arena_take_str) (function)
- [fn_arena_windows](#fn_arena_windows) (function)
- [fn_arena_released](#fn_arena_released) (function)
- [fn_arena_missed](#fn_arena_missed) (function)
- [fn_arena_report](#fn_arena_report) (function)
- [fn_arena_close](#fn_arena_close) (function)

## hash_source

```fides
pub fn hash_source(source: str) -> i64 {
```

**Parameters:**

- `source`: `str`

**Returns:** `i64`

*Defined at line 31*

---

## hash_str

Simple hash function for strings (FNV-1a 64-bit). Alias for hash_source.

```fides
pub fn hash_str(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 48*

---

## inc_deps_reset

Start recording the inputs of a new build.

```fides
pub fn inc_deps_reset() {
```

*Defined at line 79*

---

## inc_record_file

Record that the build read the file at `path`.

```fides
pub fn inc_record_file(path: str) {
```

**Parameters:**

- `path`: `str`

*Defined at line 96*

---

## inc_record_absent

Record that the build looked for `path` and found nothing there.

```fides
pub fn inc_record_absent(path: str) {
```

**Parameters:**

- `path`: `str`

*Defined at line 106*

---

## inc_deps_unchanged

Every file still has its recorded hash and every missing path is
still missing.

```fides
pub fn inc_deps_unchanged(files: Vec<str>, hashes: Vec<i64>, absent: Vec<str>) -> bool {
```

**Parameters:**

- `files`: `Vec<str>`
- `hashes`: `Vec<i64>`
- `absent`: `Vec<str>`

**Returns:** `bool`

*Defined at line 116*

---

## IncStamp

A recorded build: what it read, under which key, and its C's hash.

```fides
pub struct IncStamp {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `key` | `i64` |
| `has_c` | `bool` |
| `c_hash` | `i64` |
| `has_out` | `bool` |
| `out_hash` | `i64` |
| `files` | `Vec<str>` |
| `hashes` | `Vec<i64>` |
| `absent` | `Vec<str>` |

*Defined at line 135*

---

## env_or_empty

An environment variable's value, "" when it is not set (p_getenv
returns null then, which crashes string concatenation and str_len).

```fides
pub fn env_or_empty(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 158*

---

## env_override_set

Give `name` the value `value` for env_or_empty (replacing an earlier one).

```fides
pub fn env_override_set(name: str, value: str) {
```

**Parameters:**

- `name`: `str`
- `value`: `str`

*Defined at line 184*

---

## env_override_clear

Forget every value env_override_set gave.

```fides
pub fn env_override_clear() {
```

*Defined at line 198*

---

## argv_at

argv[i]: the process's, or, while a `fid serve` request runs, the
request's words (index 0 stays the executable, as for a one-shot run).

```fides
pub fn argv_at(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 212*

---

## argv_request_set

SP18: the words of a request ([exe, command, file, flags...]) stand for
argv until argv_request_clear.

```fides
pub fn argv_request_set(words: Vec<str>) {
```

**Parameters:**

- `words`: `Vec<str>`

*Defined at line 224*

---

## argv_count

argc to go with argv_at: the request's word count while one runs.

```fides
pub fn argv_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 235*

---

## argv_request_clear

```fides
pub fn argv_request_clear() {
```

*Defined at line 242*

---

## inc_compiler_identity

D9 (release-roadmap): what inc_compiler_hash, the key of every build
cache, is made of:
"src"  a release build (scripts/build-info.sh wrote fid_build_source,
the hash of the compiler's and the stdlib's source): that hash
with the executable's size. Nothing of the executable is read,
where "exe" read all of it (50 MB, 0.3 s on a -O0 build) on
every command that consults a cache.
"exe"  a development build, or FIDES_SELF_HASH=exe: the hash of the
executable's bytes (inc_self_hash), as before.
The size tells apart the stages of a bootstrap (each built from the same
source by another compiler); FIDES_SELF_HASH=exe keeps them strictly
apart.

```fides
pub fn inc_compiler_identity() -> str {
```

**Returns:** `str`

*Defined at line 379*

---

## inc_compiler_hash

inc_compiler_hash, computed once per run.

```fides
pub fn inc_compiler_hash() -> i64 {
```

**Returns:** `i64`

*Defined at line 389*

---

## inc_exe_hash

inc_exe_hash, computed once per run.
N12: the hash of this executable's bytes, whatever inc_compiler_hash is
made of (`fid version` names the exact build by it).

```fides
pub fn inc_exe_hash() -> i64 {
```

**Returns:** `i64`

*Defined at line 411*

---

## inc_core_checked

Whether this compiler already checked this core clean.

```fides
pub fn inc_core_checked(core_hash: i64) -> bool {
```

**Parameters:**

- `core_hash`: `i64`

**Returns:** `bool`

*Defined at line 446*

---

## inc_core_mark_checked

Record that this compiler checked this core clean. Best-effort.

```fides
pub fn inc_core_mark_checked(core_hash: i64) {
```

**Parameters:**

- `core_hash`: `i64`

*Defined at line 457*

---

## inc_build_key

The key of a build: compiler identity, command, arguments, CC,
FIDES_CC_OPT and FID_HOME (which stdlib imports resolve to).

```fides
pub fn inc_build_key(command: str, n: i64, start: i64) -> i64 {
```

**Parameters:**

- `command`: `str`
- `n`: `i64`
- `start`: `i64`

**Returns:** `i64`

*Defined at line 466*

---

## inc_unit_build_key

B6: the key of a build as far as one IR unit's object goes: what
inc_build_key holds but the input's and the output's names (argument
`start + 1` and `-o <output>`), `--inc`, `--seed-store <dir>`, CC and
FID_HOME (the other arguments in their order).
The unit's key holds its functions, so where they came from does not
matter, and a unit of the stdlib gets the same key in every program.
`keep_paths`: the build writes the input's name into the C (-g,
--leaks, --mem): keep the names.

```fides
pub fn inc_unit_build_key(command: str, n: i64, start: i64, keep_paths: bool) -> i64 {
```

**Parameters:**

- `command`: `str`
- `n`: `i64`
- `start`: `i64`
- `keep_paths`: `bool`

**Returns:** `i64`

*Defined at line 488*

---

## inc_stamp_read

Read `<output>.fides-stamp` (ok = false when missing or unreadable).

```fides
pub fn inc_stamp_read(path: str) -> IncStamp {
```

**Parameters:**

- `path`: `str`

**Returns:** `IncStamp`

*Defined at line 532*

---

## inc_up_to_date

Nothing changed since the build that wrote `output`.

```fides
pub fn inc_up_to_date(st: IncStamp, key: i64, output: str) -> bool {
```

**Parameters:**

- `st`: `IncStamp`
- `key`: `i64`
- `output`: `str`

**Returns:** `bool`

*Defined at line 593*

---

## inc_c_unchanged

The previous build produced `output` from exactly this C code.

```fides
pub fn inc_c_unchanged(st: IncStamp, key: i64, output: str, c_hash: i64) -> bool {
```

**Parameters:**

- `st`: `IncStamp`
- `key`: `i64`
- `output`: `str`
- `c_hash`: `i64`

**Returns:** `bool`

*Defined at line 604*

---

## inc_stamp_commit

Record a successful build. Nothing is written unless the build recorded
`main_input`: a pipeline that does not record its inputs must never be
reported up to date.

```fides
pub fn inc_stamp_commit(path: str, key: i64, c_hash: i64, main_input: str, output: str) {
```

**Parameters:**

- `path`: `str`
- `key`: `i64`
- `c_hash`: `i64`
- `main_input`: `str`
- `output`: `str`

*Defined at line 614*

---

## inc_bytes_hash

```fides
pub fn inc_bytes_hash(bytes: Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 623*

---

## inc_stamp_commit_hash

inc_stamp_commit with the output's hash already known.

```fides
pub fn inc_stamp_commit_hash(path: str, key: i64, c_hash: i64, main_input: str, out_hash: i64) {
```

**Parameters:**

- `path`: `str`
- `key`: `i64`
- `c_hash`: `i64`
- `main_input`: `str`
- `out_hash`: `i64`

*Defined at line 634*

---

## inc_deps_files

Snapshot of the recorded inputs (for the in-memory cache).

```fides
pub fn inc_deps_files() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 656*

---

## inc_deps_hashes

```fides
pub fn inc_deps_hashes() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 660*

---

## inc_deps_absent

```fides
pub fn inc_deps_absent() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 664*

---

## IncCacheEntry

```fides
pub struct IncCacheEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `hash` | `i64` |
| `module` | `IrModule` |
| `dep_files` | `Vec<str>` |
| `dep_hashes` | `Vec<i64>` |
| `dep_absent` | `Vec<str>` |

*Defined at line 671*

---

## IncrementalCache

Incremental compilation cache. Stores optimized IrModules keyed by
(source_path, source_hash). In-memory only.

```fides
pub struct IncrementalCache {
```

**Fields:**

| Name | Type |
|------|------|
| `entries` | `Vec<IncCacheEntry>` |

*Defined at line 686*

---

## IncrementalCache_new

```fides
pub fn IncrementalCache_new() -> IncrementalCache {
```

**Returns:** `IncrementalCache`

*Defined at line 690*

---

## IncrementalCache_get

Look up a cached IrModule by source path + hash.
Returns OptIrModule::Some(module) on hit, OptIrModule::None on miss.

```fides
pub fn IncrementalCache_get(self: &IncrementalCache, source_path: str, source_hash: i64) -> OptIrModule {
```

**Parameters:**

- `self`: `&IncrementalCache`
- `source_path`: `str`
- `source_hash`: `i64`

**Returns:** `OptIrModule`

*Defined at line 698*

---

## IncrementalCache_store

Store an IrModule in the cache. If an entry with the same path exists,
it is replaced.

```fides
pub fn IncrementalCache_store(self: &mut IncrementalCache, source_path: str, source_hash: i64, module: IrModule) {
```

**Parameters:**

- `self`: `&mut IncrementalCache`
- `source_path`: `str`
- `source_hash`: `i64`
- `module`: `IrModule`

*Defined at line 712*

---

## IncrementalCache_clear

Clear all cached entries.

```fides
pub fn IncrementalCache_clear(self: &mut IncrementalCache) {
```

**Parameters:**

- `self`: `&mut IncrementalCache`

*Defined at line 729*

---

## IncrementalCache_len

Number of cached entries.

```fides
pub fn IncrementalCache_len(self: &IncrementalCache) -> i64 {
```

**Parameters:**

- `self`: `&IncrementalCache`

**Returns:** `i64`

*Defined at line 737*

---

## IncrementalCache_is_empty

Whether the cache is empty.

```fides
pub fn IncrementalCache_is_empty(self: &IncrementalCache) -> bool {
```

**Parameters:**

- `self`: `&IncrementalCache`

**Returns:** `bool`

*Defined at line 742*

---

## global_cache_store

Store an IrModule in the global cache.

```fides
pub fn global_cache_store(source_path: str, source_hash: i64, module: IrModule) {
```

**Parameters:**

- `source_path`: `str`
- `source_hash`: `i64`
- `module`: `IrModule`

*Defined at line 768*

---

## global_cache_get

Look up a cached IrModule by source path + hash.
Returns OptIrModule::Some(module) on hit, OptIrModule::None on miss.

```fides
pub fn global_cache_get(source_path: str, source_hash: i64) -> OptIrModule {
```

**Parameters:**

- `source_path`: `str`
- `source_hash`: `i64`

**Returns:** `OptIrModule`

*Defined at line 775*

---

## global_cache_clear

Clear the global cache.

```fides
pub fn global_cache_clear() {
```

*Defined at line 781*

---

## CTokens

Tokens of a C source: spans, and whether each is an identifier.

```fides
pub struct CTokens {
```

**Fields:**

| Name | Type |
|------|------|
| `is_ident` | `Vec<i64>` |
| `starts` | `Vec<i64>` |
| `ends` | `Vec<i64>` |

*Defined at line 795*

---

## inc_lex_c

```fides
pub fn inc_lex_c(src: str) -> CTokens {
```

**Parameters:**

- `src`: `str`

**Returns:** `CTokens`

*Defined at line 815*

---

## CDefs

Top-level function definitions: `... name ( ... ) { ... }` at depth 0.

```fides
pub struct CDefs {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `name_toks` | `Vec<i64>` |
| `start_toks` | `Vec<i64>` |
| `end_toks` | `Vec<i64>` |

*Defined at line 919*

---

## inc_find_c_defs

```fides
pub fn inc_find_c_defs(src: str, t: &CTokens) -> CDefs {
```

**Parameters:**

- `src`: `str`
- `t`: `&CTokens`

**Returns:** `CDefs`

*Defined at line 926*

---

## inc_canon_hash

```fides
pub fn inc_canon_hash(src: str) -> i64 {
```

**Parameters:**

- `src`: `str`

**Returns:** `i64`

*Defined at line 1028*

---

## inc_unit_hash

Key hash of one `ccompile --inc` unit's object file (inc-roadmap B7/B8).
Like `inc_canon_hash`, except that a function with external linkage (a
definition not marked `static`) keeps its own name and is always live:
other units link against that name, so it is part of what the object
is. With the plain canonical hash, renaming an exported function whose
unit holds no string naming it (the `__p_fns` table is in the owner
unit only) reused the old object, and two modules whose code differed
only in their function names shared one object: either way the link
failed (undefined or duplicate symbol). `static` helpers are still
renamed positionally: nothing outside the unit can see their names.

```fides
pub fn inc_unit_hash(src: str) -> i64 {
```

**Parameters:**

- `src`: `str`

**Returns:** `i64`

*Defined at line 1042*

---

## inc_unit_file_name

`name` (a module name such as `compiler::lexer`) as a file name: every
byte other than a letter, digit or `_` becomes `_` (as fidesc does).

```fides
pub fn inc_unit_file_name(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 1048*

---

## CDefSpans

Byte spans of top-level function definitions: names, start, the `{`, end.

```fides
pub struct CDefSpans {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `starts` | `Vec<i64>` |
| `braces` | `Vec<i64>` |
| `ends` | `Vec<i64>` |

*Defined at line 1191*

---

## inc_c_fn_defs

```fides
pub fn inc_c_fn_defs(src: str) -> CDefSpans {
```

**Parameters:**

- `src`: `str`

**Returns:** `CDefSpans`

*Defined at line 1198*

---

## inc_artifact_path

Store path `<dir>/.fidcache/<key>.exe` of an artifact: keyed on content,
toolchain and flags, so any build (this output or another) reuses it.

```fides
pub fn inc_artifact_path(output: str, canon: i64, tool: str, flags: str) -> str {
```

**Parameters:**

- `output`: `str`
- `canon`: `i64`
- `tool`: `str`
- `flags`: `str`

**Returns:** `str`

*Defined at line 1235*

---

## inc_artifact_get

Copy `path` to `dest` if the artifact exists — local `.fidcache` first,
then the seeded store shipped with the toolchain (B6, inc_seed_get),
then the remote CAS (`FIDES_CAS`, served by tools/cas.fd). A remote hit
is also written back into the local store. The copy is written as bytes:
a new `dest` has no execute bit, so a caller restoring an executable sets
it (BG36: main.fd cmd_ccompile's "Reused cached artifact").

```fides
pub fn inc_artifact_get(path: str, dest: str) -> bool {
```

**Parameters:**

- `path`: `str`
- `dest`: `str`

**Returns:** `bool`

*Defined at line 1250*

---

## inc_artifact_put

Store `source` under `path`. Best-effort: a cache failure never breaks a
build. When `FIDES_CAS` is set the artifact is also mirrored remotely.

```fides
pub fn inc_artifact_put(path: str, source: str) {
```

**Parameters:**

- `path`: `str`
- `source`: `str`

*Defined at line 1278*

---

## inc_set_seed_dir

The seeded store of this toolchain ("" = none).
(Once per build: the hit count starts again.)

```fides
pub fn inc_set_seed_dir(dir: str) {
```

**Parameters:**

- `dir`: `str`

*Defined at line 1304*

---

## inc_seed_hits

How many artifacts this run took from the seeded store.

```fides
pub fn inc_seed_hits() -> i64 {
```

**Returns:** `i64`

*Defined at line 1310*

---

## inc_seed_put

Store the object `source` in the seeded store `dir` under the name of
artifact `path` (`ccompile --inc --seed-store <dir>`). An object already
there has the same key, so the same content: it is kept.

```fides
pub fn inc_seed_put(dir: str, path: str, source: str) {
```

**Parameters:**

- `dir`: `str`
- `path`: `str`
- `source`: `str`

*Defined at line 1354*

---

## inc_cache_evict

Keep the cache in `dir` (a `.fidcache` directory) under its limits.

```fides
pub fn inc_cache_evict(dir: str) {
```

**Parameters:**

- `dir`: `str`

*Defined at line 1472*

---

## fn_arena_on

Per-function arenas are on: FID_FN_ARENA set (not "0") and the runtime
has allocator marks.

```fides
pub fn fn_arena_on() -> bool {
```

**Returns:** `bool`

*Defined at line 1608*

---

## fn_arena_begin

Open a window: its mark, or 0 when none opens (arenas off, a window or
a region already open). Empties the hold block.

```fides
pub fn fn_arena_begin() -> i64 {
```

**Returns:** `i64`

*Defined at line 1621*

---

## fn_arena_end

Close the window `mark` (fn_arena_begin's): everything allocated in it
from the arena goes back at once. The kept values stay in the hold
block for fn_arena_take_*. Does nothing for 0.

```fides
pub fn fn_arena_end(mark: i64) {
```

**Parameters:**

- `mark`: `i64`

*Defined at line 1641*

---

## fn_arena_keep_word

Keep one word past the window's end.

```fides
pub fn fn_arena_keep_word(w: i64) {
```

**Parameters:**

- `w`: `i64`

*Defined at line 1714*

---

## fn_arena_keep_words

Keep a copy of `v`'s elements past the window's end.

```fides
pub fn fn_arena_keep_words(v: Vec<i64>) {
```

**Parameters:**

- `v`: `Vec<i64>`

*Defined at line 1719*

---

## fn_arena_keep_str

Keep a copy of `s`'s bytes past the window's end.

```fides
pub fn fn_arena_keep_str(s: str) {
```

**Parameters:**

- `s`: `str`

*Defined at line 1733*

---

## fn_arena_take_word

The next kept word (0 past the last).

```fides
pub fn fn_arena_take_word() -> i64 {
```

**Returns:** `i64`

*Defined at line 1747*

---

## fn_arena_take_words

The next kept Vec<i64>, made outside the window.

```fides
pub fn fn_arena_take_words() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 1752*

---

## fn_arena_take_str

The next kept string, made outside the window ("" past the last).

```fides
pub fn fn_arena_take_str() -> str {
```

**Returns:** `str`

*Defined at line 1764*

---

## fn_arena_windows

Windows ended so far, bytes their ends gave back, and ends that did
not find the arena back at their mark (0 when every window was sound).

```fides
pub fn fn_arena_windows() -> i64 {
```

**Returns:** `i64`

*Defined at line 1778*

---

## fn_arena_released

```fides
pub fn fn_arena_released() -> i64 {
```

**Returns:** `i64`

*Defined at line 1782*

---

## fn_arena_missed

```fides
pub fn fn_arena_missed() -> i64 {
```

**Returns:** `i64`

*Defined at line 1786*

---

## fn_arena_report

One line for --time: "" when no window was used.

```fides
pub fn fn_arena_report() -> str {
```

**Returns:** `str`

*Defined at line 1791*

---

## fn_arena_close

Give the hold block back (at exit, for --leaks builds of the compiler).

```fides
pub fn fn_arena_close() {
```

*Defined at line 1799*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
