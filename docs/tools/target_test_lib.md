[All modules](index.md)

# target_test_lib -- Fides Documentation

## Contents

- [tt_env](#tt_env) (function)
- [tt_env_or](#tt_env_or) (function)
- [tt_env_set](#tt_env_set) (function)
- [tt_args](#tt_args) (function)
- [tt_which](#tt_which) (function)
- [tt_uname](#tt_uname) (function)
- [tt_host](#tt_host) (function)
- [tt_null](#tt_null) (function)
- [tt_spawn](#tt_spawn) (function)
- [tt_status](#tt_status) (function)
- [tt_finish](#tt_finish) (function)
- [tt_run](#tt_run) (function)
- [tt_pack](#tt_pack) (function)
- [tt_pool](#tt_pool) (function)
- [tt_read](#tt_read) (function)
- [tt_chomp](#tt_chomp) (function)
- [tt_cat](#tt_cat) (function)
- [tt_no_cr](#tt_no_cr) (function)
- [tt_lines](#tt_lines) (function)
- [tt_drop_cr](#tt_drop_cr) (function)
- [tt_trim_blanks](#tt_trim_blanks) (function)
- [tt_has_line_prefix](#tt_has_line_prefix) (function)
- [tt_glob](#tt_glob) (function)
- [tt_sort](#tt_sort) (function)
- [tt_base](#tt_base) (function)
- [tt_head3_has](#tt_head3_has) (function)
- [tt_case_match](#tt_case_match) (function)
- [tt_block_of](#tt_block_of) (function)
- [tt_block_exit_of](#tt_block_exit_of) (function)
- [tt_same_lines](#tt_same_lines) (function)
- [tt_panic_key](#tt_panic_key) (function)
- [tt_msys_path](#tt_msys_path) (function)
- [tt_pwd](#tt_pwd) (function)
- [tt_append_line](#tt_append_line) (function)
- [tt_mkdir_p](#tt_mkdir_p) (function)

## tt_env

`${NAME:-}`: the variable, or "" when it is unset.

```fides
pub fn tt_env(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 39*

---

## tt_env_or

`${NAME:-def}`: the variable, or `def` when it is unset or empty.

```fides
pub fn tt_env_or(name: str, def: str) -> str {
```

**Parameters:**

- `name`: `str`
- `def`: `str`

**Returns:** `str`

*Defined at line 48*

---

## tt_env_set

`[ -n "${NAME:-}" ]`: is the variable set to something?

```fides
pub fn tt_env_set(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 55*

---

## tt_args

The arguments after the program (`fid run tools/x.fd a b` passes "run"
and the path first: those are left out).

```fides
pub fn tt_args() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 61*

---

## tt_which

Is `name` a program on PATH (`command -v name`)? A name with a slash is
looked for as it is.

```fides
pub fn tt_which(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 76*

---

## tt_uname

The output of `uname <flag>` without its newline ("" when there is no
uname).

```fides
pub fn tt_uname(flag: str) -> str {
```

**Parameters:**

- `flag`: `str`

**Returns:** `str`

*Defined at line 101*

---

## tt_host

What the scripts' `case "$(uname -s)"` makes of this host: windows,
macos, freebsd or linux (anything else).

```fides
pub fn tt_host() -> str {
```

**Returns:** `str`

*Defined at line 112*

---

## tt_null

The null device as a file to write to.

```fides
pub fn tt_null() -> str {
```

**Returns:** `str`

*Defined at line 150*

---

## tt_spawn

Start `argv` with `env` added: stdout to `out` ("" ours), stderr to
`err` ("" with stdout), stdin a pipe that gets `input` and is then
closed. 0 when it could not start.

```fides
pub fn tt_spawn(argv: Vec<str>, env: Vec<str>, out: str, err: str, input: str) -> i64 {
```

**Parameters:**

- `argv`: `Vec<str>`
- `env`: `Vec<str>`
- `out`: `str`
- `err`: `str`
- `input`: `str`

**Returns:** `i64`

*Defined at line 158*

---

## tt_status

A child's exit code as bash's $? shows it (see the header).

```fides
pub fn tt_status(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 187*

---

## tt_finish

Wait for `h` at most `secs` seconds (0 or less: no limit); kill it at
the limit. The status as `timeout` gives it (124 at the limit).

```fides
pub fn tt_finish(h: i64, secs: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`
- `secs`: `i64`

**Returns:** `i64`

*Defined at line 203*

---

## tt_run

`timeout secs argv < /dev/null > out 2> err` (err "": 2>&1; out "":
ours): the status ($?).

```fides
pub fn tt_run(argv: Vec<str>, env: Vec<str>, out: str, err: str, secs: i64) -> i64 {
```

**Parameters:**

- `argv`: `Vec<str>`
- `env`: `Vec<str>`
- `out`: `str`
- `err`: `str`
- `secs`: `i64`

**Returns:** `i64`

*Defined at line 226*

---

## tt_pack

argv as one item of a tt_pool list.

```fides
pub fn tt_pack(argv: Vec<str>) -> str {
```

**Parameters:**

- `argv`: `Vec<str>`

**Returns:** `str`

*Defined at line 237*

---

## tt_pool

Run every command of `cmds` (tt_pack'ed), with its out/err files and
time limit, `jobs` at a time; their statuses in the same order.

```fides
pub fn tt_pool(cmds: Vec<str>, env: Vec<str>, outs: Vec<str>, errs: Vec<str>, secs: Vec<i64>, jobs: i64) -> Vec<i64> {
```

**Parameters:**

- `cmds`: `Vec<str>`
- `env`: `Vec<str>`
- `outs`: `Vec<str>`
- `errs`: `Vec<str>`
- `secs`: `Vec<i64>`
- `jobs`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 243*

---

## tt_read

The text of `path`, "" when there is none.

```fides
pub fn tt_read(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 334*

---

## tt_chomp

`s` without its trailing newlines (what `$(...)` keeps).

```fides
pub fn tt_chomp(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 343*

---

## tt_cat

`$(cat path 2>/dev/null)`.

```fides
pub fn tt_cat(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 350*

---

## tt_no_cr

`s` with every "\r" removed (`tr -d '\r'`).

```fides
pub fn tt_no_cr(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 355*

---

## tt_lines

The lines of `s` as grep and awk see them: split at "\n", no empty
last line for a final "\n".

```fides
pub fn tt_lines(s: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<str>`

*Defined at line 361*

---

## tt_drop_cr

`s` without one trailing "\r".

```fides
pub fn tt_drop_cr(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 376*

---

## tt_trim_blanks

Leading and trailing blanks (spaces and tabs) removed.

```fides
pub fn tt_trim_blanks(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 382*

---

## tt_has_line_prefix

Does any line of the file start with `prefix` (`grep -q '^prefix'`)?

```fides
pub fn tt_has_line_prefix(path: str, prefix: str) -> bool {
```

**Parameters:**

- `path`: `str`
- `prefix`: `str`

**Returns:** `bool`

*Defined at line 391*

---

## tt_glob

The file names in `dir` that start with `prefix` and end with `suffix`,
in byte order (a bash glob in the C locale), each as dir/name.

```fides
pub fn tt_glob(dir: str, prefix: str, suffix: str) -> Vec<str> {
```

**Parameters:**

- `dir`: `str`
- `prefix`: `str`
- `suffix`: `str`

**Returns:** `Vec<str>`

*Defined at line 403*

---

## tt_sort

Byte order (insertion sort: the lists are a few thousand names).

```fides
pub fn tt_sort(v: &mut Vec<str>) {
```

**Parameters:**

- `v`: `&mut Vec<str>`

*Defined at line 439*

---

## tt_base

`basename "$f" .fd`: the last path part, without ".fd" unless that is
all of it.

```fides
pub fn tt_base(f: str) -> str {
```

**Parameters:**

- `f`: `str`

**Returns:** `str`

*Defined at line 455*

---

## tt_head3_has

The first 3 lines of a file hold `word` (`head -3 f | grep -q word`).

```fides
pub fn tt_head3_has(path: str, word: str) -> bool {
```

**Parameters:**

- `path`: `str`
- `word`: `str`

**Returns:** `bool`

*Defined at line 466*

---

## tt_case_match

The bash `case` pattern `pat` (*, ?, [...]) matches all of `s`.

```fides
pub fn tt_case_match(s: str, pat: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `pat`: `str`

**Returns:** `bool`

*Defined at line 477*

---

## tt_block_of

The lines after `// Expected output:` (awk in cross-test.sh's block_of
and bare-qemu-test.sh's expected_of): each `// ` stripped (with `trim`
also the blanks around the rest), up to the first line that is not a
comment; `// Expected exit:` lines left out. Each line ends in "\n".

```fides
pub fn tt_block_of(path: str, trim: bool) -> str {
```

**Parameters:**

- `path`: `str`
- `trim`: `bool`

**Returns:** `str`

*Defined at line 539*

---

## tt_block_exit_of

The test's `// Expected exit: N` (the first one; "0" when none), as
the digits are written.

```fides
pub fn tt_block_exit_of(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 567*

---

## tt_same_lines

Do the lines of `actual` match the lines of `expected` (tt_block_of's
text): as many, each equal, or for an expected line ending in "..."
starting with the rest. With `trim` each actual line loses its blanks
first.

```fides
pub fn tt_same_lines(actual: str, expected: str, trim: bool) -> bool {
```

**Parameters:**

- `actual`: `str`
- `expected`: `str`
- `trim`: `bool`

**Returns:** `bool`

*Defined at line 588*

---

## tt_panic_key

A panic's message without its details: the first line starting with
"Panic:", "\r"s removed, cut before its next ':', then without a final
" at line N" (and with `hex`, " at 0x<addr>").

```fides
pub fn tt_panic_key(path: str, hex: bool) -> str {
```

**Parameters:**

- `path`: `str`
- `hex`: `bool`

**Returns:** `str`

*Defined at line 631*

---

## tt_msys_path

"/c/x/y" for "C:\x\y" or "C:/x/y" (what Git Bash's pwd prints).

```fides
pub fn tt_msys_path(p: str) -> str {
```

**Parameters:**

- `p`: `str`

**Returns:** `str`

*Defined at line 652*

---

## tt_pwd

The current directory as the script's `$(pwd)` gives it.

```fides
pub fn tt_pwd() -> str {
```

**Returns:** `str`

*Defined at line 661*

---

## tt_append_line

Append `line` and "\n" to `path`.

```fides
pub fn tt_append_line(path: str, line: str) {
```

**Parameters:**

- `path`: `str`
- `line`: `str`

*Defined at line 668*

---

## tt_mkdir_p

`mkdir -p path` (fs_mkdir_all, but a Windows drive "C:" is not a
directory to make).

```fides
pub fn tt_mkdir_p(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 674*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
