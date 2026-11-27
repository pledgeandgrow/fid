[All modules](index.md)

# args -- Fides Documentation

## Contents

- [args_len](#args_len) (function)
- [args_get](#args_get) (function)
- [args_program_name](#args_program_name) (function)
- [args_all](#args_all) (function)
- [args_flag_in](#args_flag_in) (function)
- [args_option_in](#args_option_in) (function)
- [args_has_option_in](#args_has_option_in) (function)
- [args_int_in](#args_int_in) (function)
- [args_positionals_in](#args_positionals_in) (function)
- [args_flags_present_in](#args_flags_present_in) (function)
- [args_opts_present_in](#args_opts_present_in) (function)
- [args_positionals_exact_in](#args_positionals_exact_in) (function)
- [args_flag](#args_flag) (function)
- [args_option](#args_option) (function)
- [args_has_option](#args_has_option) (function)
- [args_int](#args_int) (function)
- [args_positionals](#args_positionals) (function)
- [args_positional](#args_positional) (function)
- [args_positionals_len](#args_positionals_len) (function)
- [args_flags_present](#args_flags_present) (function)
- [args_opts_present](#args_opts_present) (function)
- [args_positionals_exact](#args_positionals_exact) (function)
- [args_usage](#args_usage) (function)
- [args_require](#args_require) (function)
- [os_argc](#os_argc) (function)
- [os_argv](#os_argv) (function)

## args_len

Number of command-line arguments (including program name).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(args_len() >= 1);
```

```fides
pub fn args_len() -> i64 {
```

**Returns:** `i64`

*Defined at line 24*

---

## args_get

Get a command-line argument by index.
Returns empty string if index is out of bounds.
Index 0 is the program name, index 1 is the first argument, etc.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(args_get(-1), "");
assert_eq(args_get(args_len()), "");
```

```fides
pub fn args_get(index: i64) -> str {
```

**Parameters:**

- `index`: `i64`

**Returns:** `str`

*Defined at line 35*

---

## args_program_name

Get the program name (argument 0).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(args_program_name(), args_get(0));
```

```fides
pub fn args_program_name() -> str {
```

**Returns:** `str`

*Defined at line 50*

---

## args_all

All argv entries as a Vec of strings (index 0 = program name).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(args_all().len(), args_len());
```

```fides
pub fn args_all() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 58*

---

## args_flag_in

True when the flag `short`/`long` is present in `av` before "--".


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var av: Vec<str> = Vec::new();
av.push("prog"); av.push("-v"); av.push("--"); av.push("--quiet");
assert(args_flag_in(av, "-v", "--verbose"));
assert(!args_flag_in(av, "-q", "--quiet"));
```

```fides
pub fn args_flag_in(av: Vec<str>, short: str, long: str) -> bool {
```

**Parameters:**

- `av`: `Vec<str>`
- `short`: `str`
- `long`: `str`

**Returns:** `bool`

*Defined at line 108*

---

## args_option_in

Value of an option in `av`: "--opt=v" inline form, or the argument
after "short"/"long". Returns `def` when absent or valueless.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var av: Vec<str> = Vec::new();
av.push("prog"); av.push("--out=a.bin"); av.push("-j"); av.push("4");
assert_eq(args_option_in(av, "-o", "--out", "a.out"), "a.bin");
assert_eq(args_option_in(av, "-j", "--jobs", "1"), "4");
assert_eq(args_option_in(av, "-m", "--mode", "fast"), "fast");
```

```fides
pub fn args_option_in(av: Vec<str>, short: str, long: str, def: str) -> str {
```

**Parameters:**

- `av`: `Vec<str>`
- `short`: `str`
- `long`: `str`
- `def`: `str`

**Returns:** `str`

*Defined at line 130*

---

## args_has_option_in

True when the option is present with any value.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var av: Vec<str> = Vec::new();
av.push("prog"); av.push("--level=3");
assert(args_has_option_in(av, "-l", "--level"));
```

```fides
pub fn args_has_option_in(av: Vec<str>, short: str, long: str) -> bool {
```

**Parameters:**

- `av`: `Vec<str>`
- `short`: `str`
- `long`: `str`

**Returns:** `bool`

*Defined at line 156*

---

## args_int_in

Option value parsed as an integer; `def` when absent or not a number.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var av: Vec<str> = Vec::new();
av.push("prog"); av.push("-n"); av.push("12"); av.push("--x=abc");
assert_eq(args_int_in(av, "-n", "--count", 0), 12);
assert_eq(args_int_in(av, "-x", "--x", 7), 7);
```

```fides
pub fn args_int_in(av: Vec<str>, short: str, long: str, def: i64) -> i64 {
```

**Parameters:**

- `av`: `Vec<str>`
- `short`: `str`
- `long`: `str`
- `def`: `i64`

**Returns:** `i64`

*Defined at line 179*

---

## args_positionals_in

Positional args of `av`: entries not starting with '-' (or bare "-"),
plus everything after "--". NOTE: a value given to an option in the
separate form ("-o file") is not distinguishable here — use
args_positionals_exact_in when you have an option spec.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var av: Vec<str> = Vec::new();
av.push("prog"); av.push("a.fd"); av.push("-v"); av.push("--"); av.push("-b.fd");
var p = args_positionals_in(av);
assert_eq(p.len(), 2);
assert_eq(p.get(1), "-b.fd");
```

```fides
pub fn args_positionals_in(av: Vec<str>) -> Vec<str> {
```

**Parameters:**

- `av`: `Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 213*

---

## args_flags_present_in

Every present flag from `flags_spec` (comma-separated names) in `av`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var av: Vec<str> = Vec::new();
av.push("prog"); av.push("-v"); av.push("--fast");
var f = args_flags_present_in(av, "-v,--fast,--slow");
assert_eq(f.len(), 2);
```

```fides
pub fn args_flags_present_in(av: Vec<str>, flags_spec: str) -> Vec<str> {
```

**Parameters:**

- `av`: `Vec<str>`
- `flags_spec`: `str`

**Returns:** `Vec<str>`

*Defined at line 265*

---

## args_opts_present_in

Every option occurrence from `opts_spec` in `av` as "name=value"
strings (name is the actual spelling used on the command line).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var av: Vec<str> = Vec::new();
av.push("prog"); av.push("--out=x"); av.push("-j"); av.push("2");
var o = args_opts_present_in(av, "--out,-j");
assert_eq(o.len(), 2);
assert_eq(o.get(0), "--out=x");
assert_eq(o.get(1), "-j=2");
```

```fides
pub fn args_opts_present_in(av: Vec<str>, opts_spec: str) -> Vec<str> {
```

**Parameters:**

- `av`: `Vec<str>`
- `opts_spec`: `str`

**Returns:** `Vec<str>`

*Defined at line 290*

---

## args_positionals_exact_in

Exact positional args given specs: option values are consumed with
their flag, unknown "-" args are skipped (treated as flags), and
everything after "--" is positional.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var av: Vec<str> = Vec::new();
av.push("prog"); av.push("-o"); av.push("out.exe"); av.push("main.fd"); av.push("-v");
var p = args_positionals_exact_in(av, "-v", "-o");
assert_eq(p.len(), 1);
assert_eq(p.get(0), "main.fd");
```

```fides
pub fn args_positionals_exact_in(av: Vec<str>, flags_spec: str, opts_spec: str) -> Vec<str> {
```

**Parameters:**

- `av`: `Vec<str>`
- `flags_spec`: `str`
- `opts_spec`: `str`

**Returns:** `Vec<str>`

*Defined at line 319*

---

## args_flag

True when the flag `short`/`long` is present before "--".


*Example -- run as a test by `scripts/doctest.py`:*

```fides
if args_flag("-v", "--verbose") { println("verbose"); }
```

```fides
pub fn args_flag(short: str, long: str) -> bool {
```

**Parameters:**

- `short`: `str`
- `long`: `str`

**Returns:** `bool`

*Defined at line 349*

---

## args_option

Option value from argv; `def` when absent or valueless.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var out = args_option("-o", "--output", "a.out");
assert(str_len(out) > 0);
```

```fides
pub fn args_option(short: str, long: str, def: str) -> str {
```

**Parameters:**

- `short`: `str`
- `long`: `str`
- `def`: `str`

**Returns:** `str`

*Defined at line 358*

---

## args_has_option

True when the option is present with any value.

```fides
pub fn args_has_option(short: str, long: str) -> bool {
```

**Parameters:**

- `short`: `str`
- `long`: `str`

**Returns:** `bool`

*Defined at line 363*

---

## args_int

Option value as integer; `def` when absent or not a number.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var jobs = args_int("-j", "--jobs", 1);
assert(jobs >= 1);
```

```fides
pub fn args_int(short: str, long: str, def: i64) -> i64 {
```

**Parameters:**

- `short`: `str`
- `long`: `str`
- `def`: `i64`

**Returns:** `i64`

*Defined at line 372*

---

## args_positionals

Positional arguments (see args_positionals_in for the caveat).

```fides
pub fn args_positionals() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 377*

---

## args_positional

The nth positional argument (0-based), or "" when absent.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(args_positional(1000), "");
```

```fides
pub fn args_positional(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 385*

---

## args_positionals_len

Number of positional arguments.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(args_positionals_len(), args_positionals().len());
```

```fides
pub fn args_positionals_len() -> i64 {
```

**Returns:** `i64`

*Defined at line 398*

---

## args_flags_present

Present flags from spec against real argv.

```fides
pub fn args_flags_present(flags_spec: str) -> Vec<str> {
```

**Parameters:**

- `flags_spec`: `str`

**Returns:** `Vec<str>`

*Defined at line 403*

---

## args_opts_present

Present options from spec against real argv, as "name=value" strings.

```fides
pub fn args_opts_present(opts_spec: str) -> Vec<str> {
```

**Parameters:**

- `opts_spec`: `str`

**Returns:** `Vec<str>`

*Defined at line 408*

---

## args_positionals_exact

Exact positional args against real argv (specs consume option values).

```fides
pub fn args_positionals_exact(flags_spec: str, opts_spec: str) -> Vec<str> {
```

**Parameters:**

- `flags_spec`: `str`
- `opts_spec`: `str`

**Returns:** `Vec<str>`

*Defined at line 413*

---

## args_usage

Print usage: program name + spec text.

```fides
pub fn args_usage(spec: str) -> void {
```

**Parameters:**

- `spec`: `str`

**Returns:** `void`

*Defined at line 420*

---

## args_require

When `cond` is false, print usage + message to stderr and exit with
`code`. For early argument validation.

```fides
pub fn args_require(cond: bool, spec: str, msg: str, code: i64) -> void {
```

**Parameters:**

- `cond`: `bool`
- `spec`: `str`
- `msg`: `str`
- `code`: `i64`

**Returns:** `void`

*Defined at line 426*

---

## os_argc

Alias for os_argc — number of command-line arguments.

```fides
pub fn os_argc() -> i64 {
```

**Returns:** `i64`

*Defined at line 438*

---

## os_argv

Alias for os_argv — get a command-line argument by index.

```fides
pub fn os_argv(idx: i64) -> str {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `str`

*Defined at line 443*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
