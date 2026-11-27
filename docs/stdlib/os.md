[All modules](index.md)

# os -- Fides Documentation

## Contents

- [os_getenv](#os_getenv) (function)
- [os_getenv_or](#os_getenv_or) (function)
- [os_hasenv](#os_hasenv) (function)
- [os_argc](#os_argc) (function)
- [os_argv](#os_argv) (function)
- [os_args](#os_args) (function)
- [os_exit](#os_exit) (function)
- [os_exit_success](#os_exit_success) (function)
- [os_exit_failure](#os_exit_failure) (function)
- [os_system](#os_system) (function)
- [os_name](#os_name) (function)
- [os_name](#os_name) (function)
- [os_name](#os_name) (function)
- [os_name](#os_name) (function)
- [os_name](#os_name) (function)
- [os_home_dir](#os_home_dir) (function)
- [os_home_dir](#os_home_dir) (function)
- [os_cwd](#os_cwd) (function)
- [os_user](#os_user) (function)
- [os_user](#os_user) (function)
- [os_hostname](#os_hostname) (function)
- [os_shell](#os_shell) (function)
- [os_shell](#os_shell) (function)
- [os_term](#os_term) (function)
- [os_path](#os_path) (function)
- [os_tmp_dir](#os_tmp_dir) (function)
- [os_tmp_dir](#os_tmp_dir) (function)
- [os_path_sep](#os_path_sep) (function)
- [os_path_sep](#os_path_sep) (function)
- [os_path_list_sep](#os_path_list_sep) (function)
- [os_path_list_sep](#os_path_list_sep) (function)
- [os_home_dir](#os_home_dir) (function)
- [os_user](#os_user) (function)
- [os_shell](#os_shell) (function)
- [os_tmp_dir](#os_tmp_dir) (function)
- [os_path_sep](#os_path_sep) (function)
- [os_path_list_sep](#os_path_list_sep) (function)
- [env_get](#env_get) (function)
- [env_exists](#env_exists) (function)
- [args_len](#args_len) (function)
- [args_get](#args_get) (function)
- [args_program_name](#args_program_name) (function)

## os_getenv

Get an environment variable value. Returns "" if not set.

```fides
pub fn os_getenv(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 35*

---

## os_getenv_or

Get an environment variable, or a default value if not set.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(os_getenv_or("FIDDOC_NO_SUCH_VAR", "dflt"), "dflt");
assert(!os_hasenv("FIDDOC_NO_SUCH_VAR"));
assert_eq(os_getenv("FIDDOC_NO_SUCH_VAR"), "");
assert(os_argc() >= 1);
assert(str_len(os_name()) > 0);
```

```fides
pub fn os_getenv_or(name: str, default: str) -> str {
```

**Parameters:**

- `name`: `str`
- `default`: `str`

**Returns:** `str`

*Defined at line 59*

---

## os_hasenv

Check if an environment variable is set.

```fides
pub fn os_hasenv(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 75*

---

## os_argc

Get the number of command-line arguments.

```fides
pub fn os_argc() -> i64 {
```

**Returns:** `i64`

*Defined at line 86*

---

## os_argv

Get a command-line argument by index ("" when out of range).

```fides
pub fn os_argv(idx: i64) -> str {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `str`

*Defined at line 91*

---

## os_args

Get all command-line arguments as a Vec of strings.

```fides
pub fn os_args() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 103*

---

## os_exit

Exit the process with a status code.

```fides
pub fn os_exit(code: i64) -> void {
```

**Parameters:**

- `code`: `i64`

**Returns:** `void`

*Defined at line 117*

---

## os_exit_success

Exit with success (code 0).

```fides
pub fn os_exit_success() -> void {
```

**Returns:** `void`

*Defined at line 127*

---

## os_exit_failure

Exit with failure (code 1).

```fides
pub fn os_exit_failure() -> void {
```

**Returns:** `void`

*Defined at line 137*

---

## os_system

Execute a system command. Returns the exit status.

```fides
pub fn os_system(cmd: str) -> i64 {
```

**Parameters:**

- `cmd`: `str`

**Returns:** `i64`

*Defined at line 147*

---

## os_name

```fides
pub fn os_name() -> str {
```

**Returns:** `str`

*Defined at line 162*

---

## os_name

```fides
pub fn os_name() -> str {
```

**Returns:** `str`

*Defined at line 167*

---

## os_name

```fides
pub fn os_name() -> str {
```

**Returns:** `str`

*Defined at line 172*

---

## os_name

```fides
pub fn os_name() -> str {
```

**Returns:** `str`

*Defined at line 177*

---

## os_name

```fides
pub fn os_name() -> str {
```

**Returns:** `str`

*Defined at line 182*

---

## os_home_dir

```fides
pub fn os_home_dir() -> str {
```

**Returns:** `str`

*Defined at line 188*

---

## os_home_dir

```fides
pub fn os_home_dir() -> str {
```

**Returns:** `str`

*Defined at line 223*

---

## os_cwd

Get the current working directory (from PWD env var).
No getcwd builtin exists yet; PWD is set by shells on POSIX hosts.

```fides
pub fn os_cwd() -> str {
```

**Returns:** `str`

*Defined at line 240*

---

## os_user

```fides
pub fn os_user() -> str {
```

**Returns:** `str`

*Defined at line 257*

---

## os_user

```fides
pub fn os_user() -> str {
```

**Returns:** `str`

*Defined at line 273*

---

## os_hostname

Get the hostname (env-var best-effort; no gethostname builtin yet).

```fides
pub fn os_hostname() -> str {
```

**Returns:** `str`

*Defined at line 300*

---

## os_shell

```fides
pub fn os_shell() -> str {
```

**Returns:** `str`

*Defined at line 327*

---

## os_shell

```fides
pub fn os_shell() -> str {
```

**Returns:** `str`

*Defined at line 343*

---

## os_term

Get the terminal (from TERM env var).

```fides
pub fn os_term() -> str {
```

**Returns:** `str`

*Defined at line 359*

---

## os_path

Get the PATH environment variable as a Vec of paths.

```fides
pub fn os_path() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 375*

---

## os_tmp_dir

```fides
pub fn os_tmp_dir() -> str {
```

**Returns:** `str`

*Defined at line 394*

---

## os_tmp_dir

```fides
pub fn os_tmp_dir() -> str {
```

**Returns:** `str`

*Defined at line 431*

---

## os_path_sep

```fides
pub fn os_path_sep() -> str {
```

**Returns:** `str`

*Defined at line 450*

---

## os_path_sep

```fides
pub fn os_path_sep() -> str {
```

**Returns:** `str`

*Defined at line 455*

---

## os_path_list_sep

```fides
pub fn os_path_list_sep() -> str {
```

**Returns:** `str`

*Defined at line 461*

---

## os_path_list_sep

```fides
pub fn os_path_list_sep() -> str {
```

**Returns:** `str`

*Defined at line 466*

---

## os_home_dir

```fides
pub fn os_home_dir() -> str {
```

**Returns:** `str`

*Defined at line 474*

---

## os_user

```fides
pub fn os_user() -> str {
```

**Returns:** `str`

*Defined at line 490*

---

## os_shell

```fides
pub fn os_shell() -> str {
```

**Returns:** `str`

*Defined at line 506*

---

## os_tmp_dir

```fides
pub fn os_tmp_dir() -> str {
```

**Returns:** `str`

*Defined at line 511*

---

## os_path_sep

```fides
pub fn os_path_sep() -> str {
```

**Returns:** `str`

*Defined at line 527*

---

## os_path_list_sep

```fides
pub fn os_path_list_sep() -> str {
```

**Returns:** `str`

*Defined at line 532*

---

## env_get

Alias for env_get — get an environment variable value.

```fides
pub fn env_get(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 540*

---

## env_exists

Alias for env_exists — check if an environment variable is set.

```fides
pub fn env_exists(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 545*

---

## args_len

Alias for args_len — number of command-line arguments.

```fides
pub fn args_len() -> i64 {
```

**Returns:** `i64`

*Defined at line 550*

---

## args_get

Alias for args_get — get a command-line argument by index.

```fides
pub fn args_get(idx: i64) -> str {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `str`

*Defined at line 555*

---

## args_program_name

Alias for args_program_name — get the program name (argv[0]).

```fides
pub fn args_program_name() -> str {
```

**Returns:** `str`

*Defined at line 560*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
