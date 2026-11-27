[All modules](index.md)

# compiler::compiler -- Fides Documentation

## Contents

- [ResultStr](#ResultStr) (enum)
- [ResultProgram](#ResultProgram) (enum)
- [ResultIrModule](#ResultIrModule) (enum)
- [ResultVecStr](#ResultVecStr) (enum)
- [ProgramAndWarnings](#ProgramAndWarnings) (struct)
- [ResultProgramAndWarnings](#ResultProgramAndWarnings) (enum)
- [CompileResult](#CompileResult) (struct)
- [ErrCode](#ErrCode) (enum)
- [errcode_to_str](#errcode_to_str) (function)
- [errcode_number](#errcode_number) (function)
- [PathContent](#PathContent) (struct)
- [PathContent_new](#PathContent_new) (function)
- [PathTokens](#PathTokens) (struct)
- [PathTokens_new](#PathTokens_new) (function)
- [PathProgram](#PathProgram) (struct)
- [PathProgram_new](#PathProgram_new) (function)
- [ModCacheEntry](#ModCacheEntry) (struct)
- [modcache_enable](#modcache_enable) (function)
- [modcache_hits](#modcache_hits) (function)
- [ImpScan](#ImpScan) (struct)
- [impcache_summary](#impcache_summary) (function)
- [I64Pair](#I64Pair) (struct)
- [cc_I64Pair_new](#cc_I64Pair_new) (function)
- [StrTriple](#StrTriple) (struct)
- [StrTriple_new](#StrTriple_new) (function)
- [extract_line_col](#extract_line_col) (function)
- [parse_error_position](#parse_error_position) (function)
- [record_module_file](#record_module_file) (function)
- [module_file_of](#module_file_of) (function)
- [module_file_count](#module_file_count) (function)
- [module_file_path_at](#module_file_path_at) (function)
- [enrich_errors](#enrich_errors) (function)
- [source_identifiers](#source_identifiers) (function)
- [did_you_mean_suffix](#did_you_mean_suffix) (function)
- [extract_help](#extract_help) (function)
- [classify_error](#classify_error) (function)
- [errcode_normalize](#errcode_normalize) (function)
- [explain_error_text](#explain_error_text) (function)
- [is_capability](#is_capability) (function)
- [edit_distance](#edit_distance) (function)
- [find_project_root](#find_project_root) (function)
- [ark_project_root](#ark_project_root) (function)
- [ark_push_candidates](#ark_push_candidates) (function)
- [set_ark_dev_visible](#set_ark_dev_visible) (function)
- [ark_dev_visible](#ark_dev_visible) (function)
- [ark_workspace_root_of](#ark_workspace_root_of) (function)
- [cd1_pact_of](#cd1_pact_of) (function)
- [cd1_note_module](#cd1_note_module) (function)
- [Cd1Grants](#Cd1Grants) (struct)
- [cd1_grants_of](#cd1_grants_of) (function)
- [cd1_granted](#cd1_granted) (function)
- [cd1_builtin_cap](#cd1_builtin_cap) (function)
- [Cd1Use](#Cd1Use) (struct)
- [cd1_check](#cd1_check) (function)
- [parse_program_only](#parse_program_only) (function)
- [build_program](#build_program) (function)
- [build_program_with_path](#build_program_with_path) (function)
- [build_program_no_std](#build_program_no_std) (function)
- [build_program_no_std_borrowck](#build_program_no_std_borrowck) (function)
- [set_pe_runtime_merge](#set_pe_runtime_merge) (function)
- [set_native_runtime](#set_native_runtime) (function)
- [pe_runtime_path](#pe_runtime_path) (function)
- [c_platform_module](#c_platform_module) (function)
- [stdlib_dir](#stdlib_dir) (function)
- [fid_seed_store_dir](#fid_seed_store_dir) (function)
- [set_borrowck_mode](#set_borrowck_mode) (function)
- [set_time_on](#set_time_on) (function)
- [time_mark](#time_mark) (function)
- [time_row](#time_row) (function)
- [time_report](#time_report) (function)
- [get_cached_stdlib](#get_cached_stdlib) (function)
- [native_runtime_reset](#native_runtime_reset) (function)
- [serve_reset_build_state](#serve_reset_build_state) (function)
- [Cb2Item](#Cb2Item) (struct)
- [Cb2Mod](#Cb2Mod) (struct)
- [cb2_request](#cb2_request) (function)
- [cb2_request_build](#cb2_request_build) (function)
- [cb2_note](#cb2_note) (function)
- [cb2_commit](#cb2_commit) (function)
- [cb2_config](#cb2_config) (function)
- [cb2_skip_set](#cb2_skip_set) (function)
- [build_program_with_path_borrowck](#build_program_with_path_borrowck) (function)
- [ccompile_source_with_path](#ccompile_source_with_path) (function)
- [compile_to_wasm](#compile_to_wasm) (function)
- [lint_source](#lint_source) (function)
- [repl_expr_type](#repl_expr_type) (function)
- [repl_expr_type_in](#repl_expr_type_in) (function)
- [repl_is_definition](#repl_is_definition) (function)
- [repl_program](#repl_program) (function)
- [repl_print_body](#repl_print_body) (function)
- [repl_add_definition](#repl_add_definition) (function)
- [repl_user_fns](#repl_user_fns) (function)
- [repl_lower](#repl_lower) (function)
- [repl_fn_ir](#repl_fn_ir) (function)
- [repl_fn_effects](#repl_fn_effects) (function)
- [repl_lang_effects](#repl_lang_effects) (function)
- [repl_fn_caps](#repl_fn_caps) (function)
- [repl_fn_opt](#repl_fn_opt) (function)
- [repl_expr_ast](#repl_expr_ast) (function)
- [lint_source_scan](#lint_source_scan) (function)
- [lint_perf](#lint_perf) (function)
- [c_lockcheck_errors](#c_lockcheck_errors) (function)
- [lower_to_ir_module](#lower_to_ir_module) (function)
- [lower_to_ir_module_with_path](#lower_to_ir_module_with_path) (function)
- [interpret_source](#interpret_source) (function)
- [interpret_source_with_options](#interpret_source_with_options) (function)
- [run_crash_report_setup](#run_crash_report_setup) (function)
- [run_set_env](#run_set_env) (function)
- [run_limit_defaults](#run_limit_defaults) (function)
- [run_record_start](#run_record_start) (function)
- [run_record_finish](#run_record_finish) (function)
- [run_replay_start](#run_replay_start) (function)
- [run_replay_program](#run_replay_program) (function)
- [run_replay_source](#run_replay_source) (function)
- [run_replay_finish](#run_replay_finish) (function)
- [interpret_source_with_path](#interpret_source_with_path) (function)
- [build_x86_linux](#build_x86_linux) (function)
- [get_diagnostics](#get_diagnostics) (function)
- [check_text_errors](#check_text_errors) (function)
- [check_text_errors_here](#check_text_errors_here) (function)
- [ServeAnswer](#ServeAnswer) (struct)
- [serve_key_dir](#serve_key_dir) (function)
- [serve_key_path](#serve_key_path) (function)
- [serve_env_port](#serve_env_port) (function)
- [serve_escape](#serve_escape) (function)
- [serve_unescape](#serve_unescape) (function)
- [serve_send_all](#serve_send_all) (function)
- [serve_client_ask](#serve_client_ask) (function)
- [merge_programs](#merge_programs) (function)
- [cfg_ark_features](#cfg_ark_features) (function)
- [apply_cfg_filter](#apply_cfg_filter) (function)
- [set_fuzz_cov](#set_fuzz_cov) (function)
- [fuzz_cov_sites](#fuzz_cov_sites) (function)
- [fuzz_cov_program](#fuzz_cov_program) (function)
- [fzc_has_hook](#fzc_has_hook) (function)
- [synthesize_enum_eq](#synthesize_enum_eq) (function)
- [synthesize_struct_eq](#synthesize_struct_eq) (function)
- [process_derive](#process_derive) (function)
- [cc_record_edition](#cc_record_edition) (function)
- [cc_collect_cold_hints](#cc_collect_cold_hints) (function)
- [default_expr_for_type](#default_expr_for_type) (function)
- [desugar_layers_models](#desugar_layers_models) (function)
- [desugar_phase8b](#desugar_phase8b) (function)
- [fill_default_trait_methods](#fill_default_trait_methods) (function)
- [strip_ungranted_caps](#strip_ungranted_caps) (function)
- [c_str_concat3](#c_str_concat3) (function)
- [c_str_split_n](#c_str_split_n) (function)
- [c_ct_error_lines](#c_ct_error_lines) (function)
- [c_str_lines](#c_str_lines) (function)
- [c_str_substr](#c_str_substr) (function)
- [c_str_parse_i64](#c_str_parse_i64) (function)
- [c_str_trim_quotes](#c_str_trim_quotes) (function)
- [cc_c_str_vec_contains](#cc_c_str_vec_contains) (function)
- [c_str_vec_contains_ptr](#c_str_vec_contains_ptr) (function)
- [c_str_join](#c_str_join) (function)
- [c_str_pad_left](#c_str_pad_left) (function)
- [c_str_find_last](#c_str_find_last) (function)
- [c_vec0_str](#c_vec0_str) (function)
- [c_vec2_str](#c_vec2_str) (function)
- [c_file_read](#c_file_read) (function)
- [c_path_dirname](#c_path_dirname) (function)
- [c_path_join](#c_path_join) (function)
- [c_os_cwd](#c_os_cwd) (function)
- [CUnit](#CUnit) (struct)
- [split_c_units](#split_c_units) (function)
- [incu_on](#incu_on) (function)
- [incu_ready](#incu_ready) (function)
- [incu_count](#incu_count) (function)
- [incu_name](#incu_name) (function)
- [incu_key](#incu_key) (function)
- [incu_is_owner](#incu_is_owner) (function)
- [incu_c_estimate](#incu_c_estimate) (function)
- [incu_reset](#incu_reset) (function)
- [ccompile_inc_prepare](#ccompile_inc_prepare) (function)
- [incu_unit_c](#incu_unit_c) (function)

## ResultStr

```fides
pub enum ResultStr {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 58*

---

## ResultProgram

```fides
pub enum ResultProgram {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 63*

---

## ResultIrModule

```fides
pub enum ResultIrModule {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 68*

---

## ResultVecStr

G8.7: Result type for lint_source — Ok holds a Vec<str> of warnings.

```fides
pub enum ResultVecStr {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 77*

---

## ProgramAndWarnings

G8.7: Program + lint warnings (returned by build_program_with_lints).

```fides
pub struct ProgramAndWarnings {
```

**Fields:**

| Name | Type |
|------|------|
| `program` | `Program` |
| `warnings` | `Vec<str>` |

*Defined at line 83*

---

## ResultProgramAndWarnings

```fides
pub enum ResultProgramAndWarnings {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 88*

---

## CompileResult

```fides
pub struct CompileResult {
```

**Fields:**

| Name | Type |
|------|------|
| `assembly` | `str` |

*Defined at line 93*

---

## ErrCode

```fides
pub enum ErrCode {
```

**Variants:**

- `UndefinedVariable`
- `UndefinedFunction`
- `UndefinedStruct`
- `UndefinedField`
- `TypeMismatch`
- `IfConditionNotBool`
- `ArgCountMismatch`
- `MissingReturn`
- `InitBeforeAssign`
- `NotLinear`
- `CapabilityDenied`
- `SandboxViolation`
- `ConservationViolation`
- `GasExceeded`
- `EffectMismatch`
- `ExpectedToken`
- `UnexpectedChar`
- `BorrowConflict`
- `NoMain`
- `Custom`
- `// CE4: classes classify_error gives beyond the fidesc set`
- `UnterminatedString`
- `UnterminatedFString`
- `UnknownEscape`
- `DuplicateDefinition`
- `TaintViolation`
- `NonExhaustiveMatch`
- `OtherTypeError`
- `DataRace`
- `Deadlock`
- `SafetyViolation`
- `ConstantTimeViolation`
- `RealtimeViolation`
- `// CE4: the other checks' errors`
- `OtherCheck`
- `// CA6: a task never awaited, joined or detached`
- `UnjoinedTask`
- `// TL2: escape.fd's "Reference error"`
- `// "Security error`
- `ReferenceEscape`
- `SecrecyViolation`
- `// TL2: a check of the compiler's own output failed`
- `// the C split's units): E999`
- `InternalError`

*Defined at line 101*

---

## errcode_to_str

```fides
pub fn errcode_to_str(code: ErrCode) -> str {
```

**Parameters:**

- `code`: `ErrCode`

**Returns:** `str`

*Defined at line 148*

---

## errcode_number

TL2: the stable code of an error class, as docs/reference/diagnostics.md
lists them ("E000": uncategorized).

```fides
pub fn errcode_number(c: ErrCode) -> str {
```

**Parameters:**

- `c`: `ErrCode`

**Returns:** `str`

*Defined at line 267*

---

## PathContent

Cache entry: file path -> source content (avoids re-reading from disk)

```fides
pub struct PathContent {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `content` | `str` |

*Defined at line 389*

---

## PathContent_new

```fides
pub fn PathContent_new(path: str, content: str) -> PathContent {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `PathContent`

*Defined at line 394*

---

## PathTokens

Cache entry: file path -> tokens (avoids re-lexing)

```fides
pub struct PathTokens {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `tokens` | `Vec<Token>` |

*Defined at line 401*

---

## PathTokens_new

```fides
pub fn PathTokens_new(path: str, tokens: Vec<Token>) -> PathTokens {
```

**Parameters:**

- `path`: `str`
- `tokens`: `Vec<Token>`

**Returns:** `PathTokens`

*Defined at line 406*

---

## PathProgram

Cache entry: file path -> parsed Program (avoids re-parsing deferred modules)

```fides
pub struct PathProgram {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `prog` | `Program` |

*Defined at line 413*

---

## PathProgram_new

```fides
pub fn PathProgram_new(path: str, prog: Program) -> PathProgram {
```

**Parameters:**

- `path`: `str`
- `prog`: `Program`

**Returns:** `PathProgram`

*Defined at line 418*

---

## ModCacheEntry

```fides
pub struct ModCacheEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `src` | `str` |
| `prog` | `Program` |

*Defined at line 810*

---

## modcache_enable

```fides
pub fn modcache_enable() {
```

*Defined at line 821*

---

## modcache_hits

```fides
pub fn modcache_hits() -> i64 {
```

**Returns:** `i64`

*Defined at line 825*

---

## ImpScan

```fides
pub struct ImpScan {
```

**Fields:**

| Name | Type |
|------|------|
| `text_hash` | `i64` |
| `words` | `i64` |
| `order` | `Vec<i64>` |

*Defined at line 986*

---

## impcache_summary

`[build] import cache: ...` for this build (empty when it was off).

```fides
pub fn impcache_summary() -> str {
```

**Returns:** `str`

*Defined at line 1034*

---

## I64Pair

```fides
pub struct I64Pair {
```

**Fields:**

| Name | Type |
|------|------|
| `first` | `i64` |
| `second` | `i64` |

*Defined at line 1325*

---

## cc_I64Pair_new

```fides
pub fn cc_I64Pair_new(first: i64, second: i64) -> I64Pair {
```

**Parameters:**

- `first`: `i64`
- `second`: `i64`

**Returns:** `I64Pair`

*Defined at line 1330*

---

## StrTriple

```fides
pub struct StrTriple {
```

**Fields:**

| Name | Type |
|------|------|
| `line` | `i64` |
| `col` | `i64` |
| `msg` | `str` |

*Defined at line 1336*

---

## StrTriple_new

```fides
pub fn StrTriple_new(line: i64, col: i64, msg: str) -> StrTriple {
```

**Parameters:**

- `line`: `i64`
- `col`: `i64`
- `msg`: `str`

**Returns:** `StrTriple`

*Defined at line 1342*

---

## extract_line_col

Extract (line, col) from an error string.
Formats recognized:
"Type error (line N): ..."
"Parse error at L:C: ..."
"Error:L:C: ..."

```fides
pub fn extract_line_col(err: str) -> I64Pair {
```

**Parameters:**

- `err`: `str`

**Returns:** `I64Pair`

*Defined at line 1358*

---

## parse_error_position

Parse an error string to extract (line, column, message).
Used by LSP for diagnostics.

```fides
pub fn parse_error_position(full: str) -> StrTriple {
```

**Parameters:**

- `full`: `str`

**Returns:** `StrTriple`

*Defined at line 1423*

---

## record_module_file

```fides
pub fn record_module_file(module: str, path: str) {
```

**Parameters:**

- `module`: `str`
- `path`: `str`

*Defined at line 1506*

---

## module_file_of

The file a module was loaded from, or "" (error path only: a short list).

```fides
pub fn module_file_of(module: str) -> str {
```

**Parameters:**

- `module`: `str`

**Returns:** `str`

*Defined at line 1529*

---

## module_file_count

CB9: how many imported modules' files were recorded (record_module_file),
and the i-th of them: the link-error report searches their `extern "C"`
declarations (main.fd link_error_report).

```fides
pub fn module_file_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 1543*

---

## module_file_path_at

```fides
pub fn module_file_path_at(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 1547*

---

## enrich_errors

```fides
pub fn enrich_errors(errors: Vec<str>, source: str, file: str) -> Vec<str> {
```

**Parameters:**

- `errors`: `Vec<str>`
- `source`: `str`
- `file`: `str`

**Returns:** `Vec<str>`

*Defined at line 1676*

---

## source_identifiers

The identifiers written in `source`, each once, in order of first
appearance: not those in `//` comments or string literals, not numbers.

```fides
pub fn source_identifiers(source: str) -> Vec<str> {
```

**Parameters:**

- `source`: `str`

**Returns:** `Vec<str>`

*Defined at line 1770*

---

## did_you_mean_suffix

For "undefined variable 'x'" or "undefined function 'f'": the line
"\n    Fix: did you mean 'y'?" with the closest identifier of the source
(for a function, also of core's functions), or "" when nothing is
close: an edit distance of at most 1 for names of up to 4 bytes, 2
above. `fid fix` and the editor's quick fix apply it.

```fides
pub fn did_you_mean_suffix(err: str, source: str) -> str {
```

**Parameters:**

- `err`: `str`
- `source`: `str`

**Returns:** `str`

*Defined at line 1846*

---

## extract_help

Extract help text from an error that contains "Fix:" or "Hint:".

```fides
pub fn extract_help(err: str) -> Option<str> {
```

**Parameters:**

- `err`: `str`

**Returns:** `Option<str>`

*Defined at line 2317*

---

## classify_error

Classify an error string into an ErrCode.
Mirrors classify_error() in src/compiler.rs.

```fides
pub fn classify_error(err: str) -> ErrCode {
```

**Parameters:**

- `err`: `str`

**Returns:** `ErrCode`

*Defined at line 2331*

---

## errcode_normalize

"e44", "44" or "E044" -> "E044"; "" when it is no code.

```fides
pub fn errcode_normalize(arg: str) -> str {
```

**Parameters:**

- `arg`: `str`

**Returns:** `str`

*Defined at line 2475*

---

## explain_error_text

The text `fid explain-error <code>` prints: the code's section of
docs/reference/error-codes.md ("## E044 — ..." up to the next "## "),
else its class and where the entry is; "" when `fid` gives no such code.

```fides
pub fn explain_error_text(arg: str) -> str {
```

**Parameters:**

- `arg`: `str`

**Returns:** `str`

*Defined at line 2572*

---

## is_capability

Check if an effect name is a capability (Net, IO, FS, etc.)

```fides
pub fn is_capability(eff: str) -> bool {
```

**Parameters:**

- `eff`: `str`

**Returns:** `bool`

*Defined at line 2612*

---

## edit_distance

```fides
pub fn edit_distance(a: str, b: str) -> i64 {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `i64`

*Defined at line 2638*

---

## find_project_root

Walk up from `start` to find the project root -- the first directory
containing a `stdlib/` or `compiler/` subdirectory.
Falls back to `start` itself if none is found.

```fides
pub fn find_project_root(start: str) -> str {
```

**Parameters:**

- `start`: `str`

**Returns:** `str`

*Defined at line 2693*

---

## ark_project_root

The directory holding the ark.toml of a program in `base_dir` (that
directory or one above it), or "" when there is none.

```fides
pub fn ark_project_root(base_dir: str) -> str {
```

**Parameters:**

- `base_dir`: `str`

**Returns:** `str`

*Defined at line 2719*

---

## ark_push_candidates

Where `use <path>` (`path_str` with `/` for `::`) finds an installed
pact, appended to `candidates`; nothing when `ark_root` is "":
ark_modules/<path>.fd, ark_modules/<path>/mod.fd    (a file pact, any path)
ark_modules/<p>/lib.fd, ark_modules/<p>/src/lib.fd  (`use p`)
ark_modules/<p>/src/<rest>.fd                       (`use p::rest`)

```fides
pub fn ark_push_candidates(candidates: &mut Vec<str>, ark_root: str, path_str: str) {
```

**Parameters:**

- `candidates`: `&mut Vec<str>`
- `ark_root`: `str`
- `path_str`: `str`

*Defined at line 2741*

---

## set_ark_dev_visible

```fides
pub fn set_ark_dev_visible(v: bool) {
```

**Parameters:**

- `v`: `bool`

*Defined at line 2770*

---

## ark_dev_visible

```fides
pub fn ark_dev_visible() -> bool {
```

**Returns:** `bool`

*Defined at line 2774*

---

## ark_workspace_root_of

L5F: the workspace root above the project in `dir`: the nearest
directory above it (at most 8 levels) whose ark.toml has a
`[workspace]` section. "" when `dir`'s own ark.toml is the workspace's,
or there is none.

```fides
pub fn ark_workspace_root_of(dir: str) -> str {
```

**Parameters:**

- `dir`: `str`

**Returns:** `str`

*Defined at line 2782*

---

## cd1_pact_of

The pact `file` (found by `use` in a program of the project at
`ark_root`) belongs to, in the places ark_push_candidates searches;
"" for any other file.

```fides
pub fn cd1_pact_of(file: str, ark_root: str) -> str {
```

**Parameters:**

- `file`: `str`
- `ark_root`: `str`

**Returns:** `str`

*Defined at line 2884*

---

## cd1_note_module

CD1: the import loop's note of module `mod_name`, loaded from `file`.

```fides
pub fn cd1_note_module(mod_name: str, file: str, ark_root: str) {
```

**Parameters:**

- `mod_name`: `str`
- `file`: `str`
- `ark_root`: `str`

*Defined at line 2910*

---

## Cd1Grants

The [capabilities] of an ark.toml: `on` when the section is there;
`caps[i]` the comma-joined (lower-case) grants of pact `names[i]`.

```fides
pub struct Cd1Grants {
```

**Fields:**

| Name | Type |
|------|------|
| `on` | `bool` |
| `names` | `Vec<str>` |
| `caps` | `Vec<str>` |

*Defined at line 2923*

---

## cd1_grants_of

The [capabilities] of ark.toml text `content` (the reading of
compiler/ark.fd ark_cap_grants_of; an array may span lines).

```fides
pub fn cd1_grants_of(content: str) -> Cd1Grants {
```

**Parameters:**

- `content`: `str`

**Returns:** `Cd1Grants`

*Defined at line 2981*

---

## cd1_granted

The capabilities `g` grants pact `name` (none when it is not listed).

```fides
pub fn cd1_granted(g: Cd1Grants, name: str) -> Vec<str> {
```

**Parameters:**

- `g`: `Cd1Grants`
- `name`: `str`

**Returns:** `Vec<str>`

*Defined at line 3032*

---

## cd1_builtin_cap

The capability a runtime builtin needs (ark_cap_rules' names, with the
type checker's split of IO and Net); "" for one that needs none (the
console, the clock, memory, ...).

```fides
pub fn cd1_builtin_cap(c: str) -> str {
```

**Parameters:**

- `c`: `str`

**Returns:** `str`

*Defined at line 3054*

---

## Cd1Use

What a pact uses: each capability once, with where it was first seen.

```fides
pub struct Cd1Use {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `why` | `Vec<str>` |

*Defined at line 3080*

---

## cd1_check

CD1: each pact the checked `program` imports (`mods`/`mod_names`: the
imported modules) against the [capabilities] of the project's ark.toml
(the workspace root's in a member). The errors: one per pact that uses
a capability it is not granted, in the words of ark_cap_check.

```fides
pub fn cd1_check(program: Program, tc: &TypeChecker, mods: Vec<Program>, mod_names: Vec<str>) -> Vec<str> {
```

**Parameters:**

- `program`: `Program`
- `tc`: `&TypeChecker`
- `mods`: `Vec<Program>`
- `mod_names`: `Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 3330*

---

## parse_program_only

The user's program as parsed, without the stdlib, type checking or
lowering: what an editor can still show symbols from while the code
has a type error (fid lsp).

```fides
pub fn parse_program_only(source: str) -> OptProgram {
```

**Parameters:**

- `source`: `str`

**Returns:** `OptProgram`

*Defined at line 3433*

---

## build_program

Build a program from source code (no path -- no import resolution).

```fides
pub fn build_program(source: str) -> ResultProgram {
```

**Parameters:**

- `source`: `str`

**Returns:** `ResultProgram`

*Defined at line 3445*

---

## build_program_with_path

G8.2: Wrapper that uses default borrowck mode "on".

```fides
pub fn build_program_with_path(source: str, source_path: OptStr) -> ResultProgram {
```

**Parameters:**

- `source`: `str`
- `source_path`: `OptStr`

**Returns:** `ResultProgram`

*Defined at line 3450*

---

## build_program_no_std

Build a program in no-stdlib mode (--bare / --no-std).
Skips loading stdlib/core.fd entirely. The user program must define
everything it needs, including any runtime functions.
This is for kernel/UEFI/bare-metal development where the program
must have ZERO hidden runtime dependencies.

```fides
pub fn build_program_no_std(source: str, source_path: OptStr) -> ResultProgram {
```

**Parameters:**

- `source`: `str`
- `source_path`: `OptStr`

**Returns:** `ResultProgram`

*Defined at line 3459*

---

## build_program_no_std_borrowck

No-std build that honors the borrow-checker mode ("off", "warn", "on")
exactly like build_program_with_path_borrowck does for the stdlib path.

```fides
pub fn build_program_no_std_borrowck(source: str, source_path: OptStr, borrowck_mode: str) -> ResultProgram {
```

**Parameters:**

- `source`: `str`
- `source_path`: `OptStr`
- `borrowck_mode`: `str`

**Returns:** `ResultProgram`

*Defined at line 3469*

---

## set_pe_runtime_merge

```fides
pub fn set_pe_runtime_merge(v: bool) {
```

**Parameters:**

- `v`: `bool`

*Defined at line 3809*

---

## set_native_runtime

```fides
pub fn set_native_runtime(dir: str) {
```

**Parameters:**

- `dir`: `str`

*Defined at line 3821*

---

## pe_runtime_path

Locate a PE runtime source file: under stdlib/pe_runtime/ (repo and
installed layouts alike), with a legacy src/pe_runtime/ fallback.

```fides
pub fn pe_runtime_path(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 3828*

---

## c_platform_module

The standard library's directory: ./stdlib (the repository), else that
of an installed toolchain: $FID_HOME/stdlib, <exe dir>/stdlib,
<exe dir>/../stdlib (as the Rust compiler looks).
`use tls` is a different module on each system.

```fides
pub fn c_platform_module(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 3840*

---

## stdlib_dir

```fides
pub fn stdlib_dir() -> str {
```

**Returns:** `str`

*Defined at line 3873*

---

## fid_seed_store_dir

B6 (inc-roadmap): the read-only seeded store of unit objects an
installed toolchain ships, `cache/` beside the compiler (fides.nsi
installs it; inc.fd inc_seed_get reads it). A missing directory is
a store that has nothing.

```fides
pub fn fid_seed_store_dir() -> str {
```

**Returns:** `str`

*Defined at line 3902*

---

## set_borrowck_mode

G8.2: Setter for the borrow-checker mode (called from main.fd).
Fides doesn't support module-qualified assignment, so we use a function.

```fides
pub fn set_borrowck_mode(mode: str) {
```

**Parameters:**

- `mode`: `str`

*Defined at line 3940*

---

## set_time_on

Turn the --time table on (main.fd: globals are set through functions).

```fides
pub fn set_time_on() {
```

*Defined at line 3951*

---

## time_mark

Record `now - since` under `name` when --time is on; returns `now`, so
stages chain as `t = time_mark("stage", t)`.

```fides
pub fn time_mark(name: str, since: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `since`: `i64`

**Returns:** `i64`

*Defined at line 3958*

---

## time_row

A row of `ms` measured elsewhere (a part of the row above it).

```fides
pub fn time_row(name: str, ms: i64) {
```

**Parameters:**

- `name`: `str`
- `ms`: `i64`

*Defined at line 3979*

---

## time_report

Print the table to stderr: one `[time] <stage>: <ms> ms` line per stage.

```fides
pub fn time_report() {
```

*Defined at line 3987*

---

## get_cached_stdlib

Get the cached parsed stdlib. Parses on first call, returns cache on subsequent calls.
Returns (stdlib_program, struct_names, enum_names).

```fides
pub fn get_cached_stdlib() -> OptProgram {
```

**Returns:** `OptProgram`

*Defined at line 4005*

---

## native_runtime_reset

CB4: `fid build` on a Linux or macOS host whose machine-layer build was
rejected goes on through C (main.fd cmd_build): the second front end
merges no native runtime, and @cfg sees the host, as for --via-c.

```fides
pub fn native_runtime_reset() {
```

*Defined at line 4099*

---

## serve_reset_build_state

SP18: `fid serve` runs many builds in one process: what one request's
command set (the runtime merged, @cfg's target, --time, --debug, the
borrow-check mode) is put back as a fresh process has it, so the next
request builds as its one-shot command would.

```fides
pub fn serve_reset_build_state() {
```

*Defined at line 4113*

---

## Cb2Item

One top-level item of a module (see above). kind: 0 a body (function or
method), 1 a declaration, 2 an import line, module line or attribute.

```fides
pub struct Cb2Item {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `hash` | `i64` |
| `names` | `Vec<str>` |
| `words` | `Vec<i64>` |
| `kind` | `i64` |

*Defined at line 4164*

---

## Cb2Mod

One module of the program: its file and text, its items, the words its
items mention (wset: word hash -> nonzero), whether it spawns threads.

```fides
pub struct Cb2Mod {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `text` | `str` |
| `items` | `Vec<Cb2Item>` |
| `wset` | `i64` |
| `spawns` | `bool` |
| `is_user` | `bool` |
| `main_mod` | `bool` |
| `same` | `bool` |

*Defined at line 4174*

---

## cb2_request

A serve check request starts (true) or ends (false). FID_CB2=off keeps
every request a full build, as before CB2.

```fides
pub fn cb2_request(on: bool) {
```

**Parameters:**

- `on`: `bool`

*Defined at line 4236*

---

## cb2_request_build

A serve build request starts (true) or ends (false).

```fides
pub fn cb2_request_build(on: bool) {
```

**Parameters:**

- `on`: `bool`

*Defined at line 4256*

---

## cb2_note

What the last request checked ("3 of 7171 bodies checked", or why all
were), "" when CB2 was off.

```fides
pub fn cb2_note() -> str {
```

**Returns:** `str`

*Defined at line 4263*

---

## cb2_commit

The request passed every check: its modules are the clean state now.

```fides
pub fn cb2_commit() {
```

*Defined at line 4876*

---

## cb2_config

Everything a module's check depends on besides the modules' texts.

```fides
pub fn cb2_config(lib_hash: i64, borrowck_mode: str, base_dir: str) -> i64 {
```

**Parameters:**

- `lib_hash`: `i64`
- `borrowck_mode`: `str`
- `base_dir`: `str`

**Returns:** `i64`

*Defined at line 4932*

---

## cb2_skip_set

CB2: the skip set of a serve check request: `skip0` (core's, or 0) with
every body no edit since the last clean check can reach (see above).
`mod_names`: the imported modules; the program's own file is
`user_text`, lexed as `user_toks`, registered as module `user_module`.

```fides
pub fn cb2_skip_set(skip0: i64, user_text: str, source_path: OptStr, user_toks: Vec<Token>, user_module: str, mod_names: Vec<str>, content_cache: &mut Vec<PathContent>, token_cache: &Vec<PathTokens>, cfg: i64) -> i64 {
```

**Parameters:**

- `skip0`: `i64`
- `user_text`: `str`
- `source_path`: `OptStr`
- `user_toks`: `Vec<Token>`
- `user_module`: `str`
- `mod_names`: `Vec<str>`
- `content_cache`: `&mut Vec<PathContent>`
- `token_cache`: `&Vec<PathTokens>`
- `cfg`: `i64`

**Returns:** `i64`

*Defined at line 4970*

---

## build_program_with_path_borrowck

This is the main entry point for the compiler pipeline:
parse stdlib -> parse user -> resolve imports -> merge -> cfg filter ->
derive -> desugar layers/models -> comptime -> typecheck -> borrowck -> mono
G8.2: borrowck_mode is "off", "warn", or "on" (default).

```fides
pub fn build_program_with_path_borrowck(source: str, source_path: OptStr, borrowck_mode: str) -> ResultProgram {
```

**Parameters:**

- `source`: `str`
- `source_path`: `OptStr`
- `borrowck_mode`: `str`

**Returns:** `ResultProgram`

*Defined at line 5425*

---

## ccompile_source_with_path

Compile source to C code.

```fides
pub fn ccompile_source_with_path(source: str, source_path: str, borrowck_mode: str) -> ResultStr {
```

**Parameters:**

- `source`: `str`
- `source_path`: `str`
- `borrowck_mode`: `str`

**Returns:** `ResultStr`

*Defined at line 6572*

---

## compile_to_wasm

G8.5: Compile source to WASM binary (or WAT text if wat_mode=true).
Pipeline: build_program → lower → generate_wasm/dump_wat

```fides
pub fn compile_to_wasm(source: str, source_path: str, wat_mode: bool) -> ResultBytes {
```

**Parameters:**

- `source`: `str`
- `source_path`: `str`
- `wat_mode`: `bool`

**Returns:** `ResultBytes`

*Defined at line 6641*

---

## lint_source

G8.7: Lint a source file. Returns warnings as a vector of strings.
Returns Err with errors if compilation fails.

```fides
pub fn lint_source(source: str, source_path: str, perf: bool) -> ResultVecStr {
```

**Parameters:**

- `source`: `str`
- `source_path`: `str`
- `perf`: `bool`

**Returns:** `ResultVecStr`

*Defined at line 6670*

---

## repl_expr_type

L7G: `:type <expr>` in `fid repl` — the type the checker gives the
expression `expr_src`, written alone in a function body (no locals), as
`type_to_str` spells it ("i64", "str", "Vec<i64>"); or the errors that
checking it gives. Nothing is run.

```fides
pub fn repl_expr_type(expr_src: str) -> ResultStr {
```

**Parameters:**

- `expr_src`: `str`

**Returns:** `ResultStr`

*Defined at line 6894*

---

## repl_expr_type_in

L7G: repl_expr_type with the REPL session's definitions `defs` (source
text after the expression's function, which stays the program's first),
so that `:type` sees the functions and types defined so far.

```fides
pub fn repl_expr_type_in(defs: str, expr_src: str) -> ResultStr {
```

**Parameters:**

- `defs`: `str`
- `expr_src`: `str`

**Returns:** `ResultStr`

*Defined at line 6901*

---

## repl_is_definition

L7G: whether a REPL line defines something the session keeps (a
function, a type, an import, a constant) rather than being an
expression to run.

```fides
pub fn repl_is_definition(line: str) -> bool {
```

**Parameters:**

- `line`: `str`

**Returns:** `bool`

*Defined at line 7640*

---

## repl_program

L7G: the program of a REPL command: the session's definitions and a
`main` holding `body`.

```fides
pub fn repl_program(defs: str, body: str) -> str {
```

**Parameters:**

- `defs`: `str`
- `body`: `str`

**Returns:** `str`

*Defined at line 7669*

---

## repl_print_body

L7G: the `main` body that prints expression `expr` (what the REPL runs
for an expression line); "" for no expression.

```fides
pub fn repl_print_body(expr: str) -> str {
```

**Parameters:**

- `expr`: `str`

**Returns:** `str`

*Defined at line 7675*

---

## repl_add_definition

L7G: definition `def_src` added to the session's `defs`: the new
definitions when the program still checks, or its errors (the session
keeps the old ones).

```fides
pub fn repl_add_definition(defs: str, def_src: str) -> ResultStr {
```

**Parameters:**

- `defs`: `str`
- `def_src`: `str`

**Returns:** `ResultStr`

*Defined at line 7685*

---

## repl_user_fns

L7G: the names of the functions the session defined (not the stdlib's).

```fides
pub fn repl_user_fns(defs: str) -> Vec<str> {
```

**Parameters:**

- `defs`: `str`

**Returns:** `Vec<str>`

*Defined at line 7700*

---

## repl_lower

L7G: the session lowered to IR, its `main` printing `expr` ("" for an
empty `main`), as the REPL would run it.

```fides
pub fn repl_lower(defs: str, expr: str) -> ResultIrModule {
```

**Parameters:**

- `defs`: `str`
- `expr`: `str`

**Returns:** `ResultIrModule`

*Defined at line 7718*

---

## repl_fn_ir

L7G: `:ir` — function `name` of the lowered session as IR text.

```fides
pub fn repl_fn_ir(module: IrModule, name: str) -> ResultStr {
```

**Parameters:**

- `module`: `IrModule`
- `name`: `str`

**Returns:** `ResultStr`

*Defined at line 7728*

---

## repl_fn_effects

L7G: `:effects` — the effects of function `name` in the lowered session
(its own and those of the functions it calls), "pure" when it has none.

```fides
pub fn repl_fn_effects(module: IrModule, name: str) -> ResultStr {
```

**Parameters:**

- `module`: `IrModule`
- `name`: `str`

**Returns:** `ResultStr`

*Defined at line 7775*

---

## repl_lang_effects

L7G: `:effects <fn>` — the effects the type checker infers for function
`name` of the session `defs` (or of the stdlib) in the language's terms
(reference §9.1: IO, Net, Spawn, Diverge, Alloc, ...), with those of the
functions it calls; "pure" when it has none. Unlike the IR's set
(repl_fn_effects), a stack local is no Alloc/Read/Write effect, so a
pure loop is pure. A method is named `Type.method` or `Type::method`.

```fides
pub fn repl_lang_effects(defs: str, name: str) -> ResultStr {
```

**Parameters:**

- `defs`: `str`
- `name`: `str`

**Returns:** `ResultStr`

*Defined at line 7824*

---

## repl_fn_caps

L7G: `:caps` — the capabilities function `name` needs: what its calls
need (repl_call_cap), followed through the session's own functions
`user` (the stdlib's are not entered: the type checker does not check
capabilities inside core either), and what its effects need (Net, Spawn;
IO when no call named the IO or FS one). "none" when it needs nothing.

```fides
pub fn repl_fn_caps(module: IrModule, name: str, user: Vec<str>) -> ResultStr {
```

**Parameters:**

- `module`: `IrModule`
- `name`: `str`
- `user`: `Vec<str>`

**Returns:** `ResultStr`

*Defined at line 7925*

---

## repl_fn_opt

MI18: `:opt` in `fid repl` (it replaced `:pir`, the PIR path's levels)
— function `name` of the lowered session after the IR optimizer that
`emit` runs before the machine layer (iropt.fd iropt_native, on the
whole session so calls can be inlined), headed by its effects as
`:effects` names them (repl_lang_effects over the session `defs`; the
IR's set when the checker does not know the name).

```fides
pub fn repl_fn_opt(module: IrModule, name: str, defs: str) -> ResultStr {
```

**Parameters:**

- `module`: `IrModule`
- `name`: `str`
- `defs`: `str`

**Returns:** `ResultStr`

*Defined at line 7999*

---

## repl_expr_ast

L7G: `:ast <expr>` in `fid repl` — the parsed expression `expr_src` as
rast_expr prints it (the session's definitions `defs` follow it in the
parsed source), or the parse error. Nothing is checked or run.

```fides
pub fn repl_expr_ast(defs: str, expr_src: str) -> ResultStr {
```

**Parameters:**

- `defs`: `str`
- `expr_src`: `str`

**Returns:** `ResultStr`

*Defined at line 8030*

---

## lint_source_scan

L7E: the unused-variable, naming and import-order warnings of `source`
(lint_source adds them to the checker's).

```fides
pub fn lint_source_scan(source: str) -> Vec<str> {
```

**Parameters:**

- `source`: `str`

**Returns:** `Vec<str>`

*Defined at line 8634*

---

## lint_perf

All `--perf` warnings in one user program.

```fides
pub fn lint_perf(user: Program) -> Vec<str> {
```

**Parameters:**

- `user`: `Program`

**Returns:** `Vec<str>`

*Defined at line 8727*

---

## c_lockcheck_errors

```fides
pub fn c_lockcheck_errors(module: &IrModule) -> Vec<str> {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `Vec<str>`

*Defined at line 9256*

---

## lower_to_ir_module

Lower source to an IR module.

```fides
pub fn lower_to_ir_module(source: str) -> ResultIrModule {
```

**Parameters:**

- `source`: `str`

**Returns:** `ResultIrModule`

*Defined at line 9269*

---

## lower_to_ir_module_with_path

G8.3: Lower source to IR with source path (for import resolution).

```fides
pub fn lower_to_ir_module_with_path(source: str, source_path: str, borrowck_mode: str) -> ResultIrModule {
```

**Parameters:**

- `source`: `str`
- `source_path`: `str`
- `borrowck_mode`: `str`

**Returns:** `ResultIrModule`

*Defined at line 9287*

---

## interpret_source

Interpret a source file by lowering to IR and executing via the
self-hosted interpreter. Returns exit code (0 = success, 1 = error).

```fides
pub fn interpret_source(source: str) -> i64 {
```

**Parameters:**

- `source`: `str`

**Returns:** `i64`

*Defined at line 9376*

---

## interpret_source_with_options

Interpret a source file with execution limits and verbose flag.
Returns exit code (0 = success, 1 = error).

```fides
pub fn interpret_source_with_options(source: str, max_recursion: i64, max_steps: i64, max_memory: i64, verbose: bool) -> i64 {
```

**Parameters:**

- `source`: `str`
- `max_recursion`: `i64`
- `max_steps`: `i64`
- `max_memory`: `i64`
- `verbose`: `bool`

**Returns:** `i64`

*Defined at line 9401*

---

## run_crash_report_setup

CO9: ask the interpreter for a crash report at `path` ("" for the
FID_CRASH_REPORT file) when the program panics, recording build
`version` and source file `program` (interp.fd interp_crash_report_setup).

```fides
pub fn run_crash_report_setup(path: str, version: str, program: str) {
```

**Parameters:**

- `path`: `str`
- `version`: `str`
- `program`: `str`

*Defined at line 9427*

---

## run_set_env

CO2: environment variable `name` = `value` for the next interpreted run
only (interp.fd interp_set_env).

```fides
pub fn run_set_env(name: str, value: str) {
```

**Parameters:**

- `name`: `str`
- `value`: `str`

*Defined at line 9433*

---

## run_limit_defaults

roadmap P12: `fid run --max-mem` / `--cpu-limit` (bytes, ms; 0: none):
the next interpreted run's limits when FID_MEM_LIMIT / FID_CPU_LIMIT is
not set (interp.fd interp_limit_defaults).

```fides
pub fn run_limit_defaults(mem_bytes: i64, cpu_ms: i64) {
```

**Parameters:**

- `mem_bytes`: `i64`
- `cpu_ms`: `i64`

*Defined at line 9440*

---

## run_record_start

CT7: record the effect calls of the next interpreted run into `path`
(interp.fd interp_rr_record_start); false when it cannot be written.

```fides
pub fn run_record_start(path: str, program: str, source: str) -> bool {
```

**Parameters:**

- `path`: `str`
- `program`: `str`
- `source`: `str`

**Returns:** `bool`

*Defined at line 9446*

---

## run_record_finish

CT7: the recorded run ended with exit code `code`.

```fides
pub fn run_record_finish(code: i64) {
```

**Parameters:**

- `code`: `i64`

*Defined at line 9451*

---

## run_replay_start

CT7: answer the next run's effect calls from trace `text` ("" or why
it is not a trace); the recorded program's path and text.

```fides
pub fn run_replay_start(text: str, steps: bool) -> str {
```

**Parameters:**

- `text`: `str`
- `steps`: `bool`

**Returns:** `str`

*Defined at line 9457*

---

## run_replay_program

```fides
pub fn run_replay_program() -> str {
```

**Returns:** `str`

*Defined at line 9461*

---

## run_replay_source

```fides
pub fn run_replay_source() -> str {
```

**Returns:** `str`

*Defined at line 9465*

---

## run_replay_finish

CT7: "" when the replay used every recorded call and ended as the
recorded run did; else what differs.

```fides
pub fn run_replay_finish(code: i64) -> str {
```

**Parameters:**

- `code`: `i64`

**Returns:** `str`

*Defined at line 9471*

---

## interpret_source_with_path

G8.3: Interpret a source file with source path (for import resolution).

```fides
pub fn interpret_source_with_path(source: str, source_path: str, max_recursion: i64, max_steps: i64, max_memory: i64, verbose: bool, borrowck_mode: str) -> i64 {
```

**Parameters:**

- `source`: `str`
- `source_path`: `str`
- `max_recursion`: `i64`
- `max_steps`: `i64`
- `max_memory`: `i64`
- `verbose`: `bool`
- `borrowck_mode`: `str`

**Returns:** `i64`

*Defined at line 9476*

---

## build_x86_linux

Build a native Linux ELF executable using the x86.rs stack-machine backend.
This is the simpler backend (no PIR, no register allocation) — each SSA value
lives on the stack. Emits AT&T assembly -> elf64 assembler -> linker with no
runtime (the x86 backend includes its own runtime stubs).
Mirrors src/compiler.rs::build_x86_linux.

```fides
pub fn build_x86_linux(source: str, source_path: str) -> ResultBytes {
```

**Parameters:**

- `source`: `str`
- `source_path`: `str`

**Returns:** `ResultBytes`

*Defined at line 9510*

---

## get_diagnostics

```fides
pub fn get_diagnostics(source: str, source_path: OptStr) -> Vec<StrTriple> {
```

**Parameters:**

- `source`: `str`
- `source_path`: `OptStr`

**Returns:** `Vec<StrTriple>`

*Defined at line 9547*

---

## check_text_errors

The errors of an editor's text (the LSP's check): from a running
`fid serve --port` when FID_SERVE names its port (SP19: the server's
stdlib and module parses are warm, and they are shared with `fid
check`), else checked in this process. Both run build_program_with_path,
so the diagnostics are the same.

```fides
pub fn check_text_errors(source: str, source_path: OptStr) -> Vec<str> {
```

**Parameters:**

- `source`: `str`
- `source_path`: `OptStr`

**Returns:** `Vec<str>`

*Defined at line 9564*

---

## check_text_errors_here

check_text_errors in this process (also what `fid serve` runs for a
`check-text` request).

```fides
pub fn check_text_errors_here(source: str, source_path: OptStr) -> Vec<str> {
```

**Parameters:**

- `source`: `str`
- `source_path`: `OptStr`

**Returns:** `Vec<str>`

*Defined at line 9593*

---

## ServeAnswer

```fides
pub struct ServeAnswer {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `status` | `i64` |
| `errors` | `Vec<str>` |
| `notes` | `Vec<str>` |

*Defined at line 9614*

---

## serve_key_dir

The directory of the server's key files: <home>/.fid (a user's own).

```fides
pub fn serve_key_dir() -> str {
```

**Returns:** `str`

*Defined at line 9630*

---

## serve_key_path

```fides
pub fn serve_key_path(port: i64) -> str {
```

**Parameters:**

- `port`: `i64`

**Returns:** `str`

*Defined at line 9641*

---

## serve_env_port

FID_SERVE=<port>: the port of a running `fid serve --port`; 0 when it
is unset, and always in the server itself (it never asks itself).

```fides
pub fn serve_env_port() -> i64 {
```

**Returns:** `i64`

*Defined at line 9647*

---

## serve_escape

An error's text as one protocol line: `\` -> `\\`, LF -> `\n`, CR -> `\r`.

```fides
pub fn serve_escape(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 9659*

---

## serve_unescape

```fides
pub fn serve_unescape(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 9663*

---

## serve_send_all

Write all of `data` to a socket (a send may take only part of it).

```fides
pub fn serve_send_all(fd: i64, data: str) -> bool {
```

**Parameters:**

- `fd`: `i64`
- `data`: `str`

**Returns:** `bool`

*Defined at line 9691*

---

## serve_client_ask

One request to the `fid serve --port <port>` of this machine: the key,
`request` as a line, then `body` (check-text's text; "" for none).

```fides
pub fn serve_client_ask(port: i64, request: str, body: str) -> ServeAnswer {
```

**Parameters:**

- `port`: `i64`
- `request`: `str`
- `body`: `str`

**Returns:** `ServeAnswer`

*Defined at line 9725*

---

## merge_programs

```fides
pub fn merge_programs(stdlib: Program, user: Program) -> Program {
```

**Parameters:**

- `stdlib`: `Program`
- `user`: `Program`

**Returns:** `Program`

*Defined at line 10039*

---

## cfg_ark_features

```fides
pub fn cfg_ark_features(base_dir: str) -> str {
```

**Parameters:**

- `base_dir`: `str`

**Returns:** `str`

*Defined at line 10386*

---

## apply_cfg_filter

Conditional compilation (release K10, as src/compiler.rs apply_cfg_filter):
a function or struct whose @cfg names another platform, architecture or
build mode is left out. Lists are rebuilt with plain pushes (the old
Vec-of-struct copy problem that made this a no-op is long fixed).

```fides
pub fn apply_cfg_filter(program: &mut Program) {
```

**Parameters:**

- `program`: `&mut Program`

*Defined at line 10434*

---

## set_fuzz_cov

```fides
pub fn set_fuzz_cov(on: bool, std_too: bool) {
```

**Parameters:**

- `on`: `bool`
- `std_too`: `bool`

*Defined at line 10559*

---

## fuzz_cov_sites

How many blocks the last instrumented build numbered.

```fides
pub fn fuzz_cov_sites() -> i64 {
```

**Returns:** `i64`

*Defined at line 10566*

---

## fuzz_cov_program

Instrument every function and method of `program` (fzc_func_wanted).
2g.83: the lists are refilled in place, not replaced, as parser.fd
pd_cross_module_specializations does: the user program is
build_program_with_path_borrowck's match binding of the parse result,
and a compiler built by a lowering from before the match-binding fix
dropped a replaced list the parse result still held, then dropped it
again with the result ("Double free" in Vec_drop_inplace from
__own_drop_100 there, under `ccompile --fuzz-cov`).

```fides
pub fn fuzz_cov_program(program: &mut Program) {
```

**Parameters:**

- `program`: `&mut Program`

*Defined at line 10688*

---

## fzc_has_hook

Does `program` define the harness's `__fz_hit`?

```fides
pub fn fzc_has_hook(program: Program) -> bool {
```

**Parameters:**

- `program`: `Program`

**Returns:** `bool`

*Defined at line 10732*

---

## synthesize_enum_eq

```fides
pub fn synthesize_enum_eq(program: &mut Program) {
```

**Parameters:**

- `program`: `&mut Program`

*Defined at line 10779*

---

## synthesize_struct_eq

R1.2b: `==` on structs compares fields (proposal 0001). For every
non-generic struct, as src/compiler.rs synthesize_struct_eq:

fn __eq_S(__eq_a: S, __eq_b: S) -> bool {
return __eq_a.f0 == __eq_b.f0 && __eq_a.f1 == __eq_b.f1 && ...;
}

Each field goes back through `==`: nested structs and enums use their own
__eq, strings compare by content. A struct with a field of a generic
struct or enum type is skipped (its `==` would not type-check yet).

```fides
pub fn synthesize_struct_eq(program: &mut Program) {
```

**Parameters:**

- `program`: `&mut Program`

*Defined at line 10862*

---

## process_derive

```fides
pub fn process_derive(program: &mut Program) {
```

**Parameters:**

- `program`: `&mut Program`

*Defined at line 10948*

---

## cc_record_edition

ST3: record the edition `p` states (or the default) for `module`; an
error message for an edition this compiler does not know, else "".

```fides
pub fn cc_record_edition(p: Program, module: str, file: str) -> str {
```

**Parameters:**

- `p`: `Program`
- `module`: `str`
- `file`: `str`

**Returns:** `str`

*Defined at line 11273*

---

## cc_collect_cold_hints

OP15: record each function marked `@cold` (and `Type_method` for a
method) in compiler/ast.fd's set, which the native backends read when
they lay out blocks: a block calling one goes after the hot code.

```fides
pub fn cc_collect_cold_hints(program: Program) {
```

**Parameters:**

- `program`: `Program`

*Defined at line 11291*

---

## default_expr_for_type

Generate a default expression for a given type (used by @derive(Deserialize)).

```fides
pub fn default_expr_for_type(ty: Type) -> Expr {
```

**Parameters:**

- `ty`: `Type`

**Returns:** `Expr`

*Defined at line 11493*

---

## desugar_layers_models

Desugar layer/model declarations into structs + impls + constructor functions.

```fides
pub fn desugar_layers_models(program: &mut Program) {
```

**Parameters:**

- `program`: `&mut Program`

*Defined at line 11546*

---

## desugar_phase8b

Desugar F18 components → structs, F20 event handlers → functions,
F19 state machines → enum + initial/handle functions.

```fides
pub fn desugar_phase8b(program: &mut Program) {
```

**Parameters:**

- `program`: `&mut Program`

*Defined at line 11631*

---

## fill_default_trait_methods

Phase 3B.1: Fill in default trait methods.
For each trait impl, if the trait defines a method with a default body that
the impl doesn't override, add the default body to the impl.

```fides
pub fn fill_default_trait_methods(program: &mut Program) {
```

**Parameters:**

- `program`: `&mut Program`

*Defined at line 11986*

---

## strip_ungranted_caps

I-3A: Transitive Capability DCE -- strip functions requiring capabilities not
in the granted set, then transitively eliminate all functions only reachable
through the stripped functions.

```fides
pub fn strip_ungranted_caps(module: &mut IrModule, granted_caps: Vec<str>) {
```

**Parameters:**

- `module`: `&mut IrModule`
- `granted_caps`: `Vec<str>`

*Defined at line 12103*

---

## c_str_concat3

Concatenate three strings.

```fides
pub fn c_str_concat3(a: str, b: str, c: str) -> str {
```

**Parameters:**

- `a`: `str`
- `b`: `str`
- `c`: `str`

**Returns:** `str`

*Defined at line 12267*

---

## c_str_split_n

Split a string by a delimiter, returning at most n parts.

```fides
pub fn c_str_split_n(s: str, delim: str, n: i64) -> Vec<str> {
```

**Parameters:**

- `s`: `str`
- `delim`: `str`
- `n`: `i64`

**Returns:** `Vec<str>`

*Defined at line 12272*

---

## c_ct_error_lines

Split a string into lines.
LF18: the @constant_time violations the last CCodeGen_generate found
(codegen_c.fd CGen_ct_error_text), one per element; empty when none.

```fides
pub fn c_ct_error_lines() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 12293*

---

## c_str_lines

```fides
pub fn c_str_lines(s: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<str>`

*Defined at line 12310*

---

## c_str_substr

Get a substring from start (inclusive) to end (exclusive).

```fides
pub fn c_str_substr(s: str, start: i64, end: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `start`: `i64`
- `end`: `i64`

**Returns:** `str`

*Defined at line 12332*

---

## c_str_parse_i64

Parse an i64 from a string. Returns 0 on failure.

```fides
pub fn c_str_parse_i64(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 12346*

---

## c_str_trim_quotes

Trim surrounding quotes from a string.

```fides
pub fn c_str_trim_quotes(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 12351*

---

## cc_c_str_vec_contains

```fides
pub fn cc_c_str_vec_contains(vec: Vec<str>, s: str) -> bool {
```

**Parameters:**

- `vec`: `Vec<str>`
- `s`: `str`

**Returns:** `bool`

*Defined at line 14218*

---

## c_str_vec_contains_ptr

Fast contains check using pointer equality (==) instead of str_equal.
Only safe to use when all strings in the vec AND the search string are
interned (i.e., produced by Lexer_intern or from the same str_slice).
This is O(n) but each comparison is a single i64 == instead of a
function call + byte-by-byte comparison.

```fides
pub fn c_str_vec_contains_ptr(vec: Vec<str>, s: str) -> bool {
```

**Parameters:**

- `vec`: `Vec<str>`
- `s`: `str`

**Returns:** `bool`

*Defined at line 14234*

---

## c_str_join

Join a Vec<str> with a separator.

```fides
pub fn c_str_join(vec: Vec<str>, sep: str) -> str {
```

**Parameters:**

- `vec`: `Vec<str>`
- `sep`: `str`

**Returns:** `str`

*Defined at line 14246*

---

## c_str_pad_left

Pad a string on the left to a given width.

```fides
pub fn c_str_pad_left(s: str, width: i64, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 14260*

---

## c_str_find_last

Find the last occurrence of a character in a string.

```fides
pub fn c_str_find_last(s: str, ch: str) -> i64 {
```

**Parameters:**

- `s`: `str`
- `ch`: `str`

**Returns:** `i64`

*Defined at line 14269*

---

## c_vec0_str

```fides
pub fn c_vec0_str() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 14288*

---

## c_vec2_str

```fides
pub fn c_vec2_str(a: str, b: str) -> Vec<str> {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `Vec<str>`

*Defined at line 14292*

---

## c_file_read

Read a file as a string. Returns empty string on failure.

```fides
pub fn c_file_read(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 14300*

---

## c_path_dirname

Get the directory part of a path (everything before the last /).

```fides
pub fn c_path_dirname(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 14318*

---

## c_path_join

Join two path components with a /.

```fides
pub fn c_path_join(a: str, b: str) -> str {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `str`

*Defined at line 14335*

---

## c_os_cwd

Get the current working directory.

```fides
pub fn c_os_cwd() -> str {
```

**Returns:** `str`

*Defined at line 14350*

---

## CUnit

```fides
pub struct CUnit {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `code` | `str` |

*Defined at line 14842*

---

## split_c_units

Split generated C with //@FN marks into one unit per source module. The
prelude's small runtime definitions move to the unit owning `main` (a
prototype stays); a global is defined there and `extern` elsewhere.

```fides
pub fn split_c_units(full: str) -> Vec<CUnit> {
```

**Parameters:**

- `full`: `str`

**Returns:** `Vec<CUnit>`

*Defined at line 15035*

---

## incu_on

Whether `ccompile --inc` generates C per IrUnit (not with
FIDES_INC_IR=off or FIDES_INC_FLAT: then the program's C is cut).

```fides
pub fn incu_on() -> bool {
```

**Returns:** `bool`

*Defined at line 15667*

---

## incu_ready

Whether ccompile_inc_prepare made the units of this build.

```fides
pub fn incu_ready() -> bool {
```

**Returns:** `bool`

*Defined at line 15672*

---

## incu_count

```fides
pub fn incu_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 15676*

---

## incu_name

```fides
pub fn incu_name(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 15680*

---

## incu_key

The object key of unit `i`.

```fides
pub fn incu_key(i: i64) -> i64 {
```

**Parameters:**

- `i`: `i64`

**Returns:** `i64`

*Defined at line 15685*

---

## incu_is_owner

Whether unit `i` owns `main` (its object is this program's alone: B6
seeds a store with the other units' objects only).

```fides
pub fn incu_is_owner(i: i64) -> bool {
```

**Parameters:**

- `i`: `i64`

**Returns:** `bool`

*Defined at line 15691*

---

## incu_c_estimate

An estimate of the size of the program's C, in bytes (cmd_ccompile
compiles a program of more than 16 MB of C at -O0).

```fides
pub fn incu_c_estimate() -> i64 {
```

**Returns:** `i64`

*Defined at line 15697*

---

## incu_reset

Forget an earlier build's units (split_c_units cuts by module again).

```fides
pub fn incu_reset() {
```

*Defined at line 15702*

---

## ccompile_inc_prepare

`ccompile --inc` (B1): lower `source`, cut it into IrUnits and make each
unit's C generator input and object key (`build_key`: the build's
compiler, flags and environment, inc_build_key). Ok: a short C-comment
text that stands for the program's C in cmd_ccompile: the libraries it
links (`// PLINK:` lines, as the C has them) and every unit's key, so a
build whose keys all match the last one is up to date. build_c_units
then generates and compiles only the units it must (incu_unit_c). When
the units cannot be made (a function in two units), Ok holds the whole
program's C and incu_ready() is false: the C text is cut as before.

```fides
pub fn ccompile_inc_prepare(source: str, source_path: str, borrowck_mode: str, build_key: i64) -> ResultStr {
```

**Parameters:**

- `source`: `str`
- `source_path`: `str`
- `borrowck_mode`: `str`
- `build_key`: `i64`

**Returns:** `ResultStr`

*Defined at line 16553*

---

## incu_unit_c

The C of unit `i` (B1): its module through the C generator, then its
part of that text (split_c_units with the units' assignment). "" when
the C generator reported an error.

```fides
pub fn incu_unit_c(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 17089*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
