[All modules](index.md)

# process -- Fides Documentation

## Contents

- [process_exit](#process_exit) (function)
- [process_system](#process_system) (function)
- [process_run](#process_run) (function)
- [process_ok](#process_ok) (function)
- [process_stdout](#process_stdout) (function)
- [process_stderr](#process_stderr) (function)
- [process_code](#process_code) (function)
- [process_close](#process_close) (function)
- [process_pid](#process_pid) (function)
- [process_capture](#process_capture) (function)
- [Command](#Command) (struct)
- [process_is_windows](#process_is_windows) (function)
- [process_quote](#process_quote) (function)
- [Command](#Command) (impl)
- [process_spawn](#process_spawn) (function)
- [process_spawn_to](#process_spawn_to) (function)
- [process_spawn_files](#process_spawn_files) (function)
- [process_wait](#process_wait) (function)
- [process_kill](#process_kill) (function)
- [process_child_pid](#process_child_pid) (function)
- [process_release](#process_release) (function)
- [ChildResult](#ChildResult) (struct)
- [process_run_argv](#process_run_argv) (function)
- [PROC_PIPE_STDIN:](#PROC_PIPE_STDIN:) (const)
- [PROC_PIPE_STDOUT:](#PROC_PIPE_STDOUT:) (const)
- [PROC_PIPE_STDERR:](#PROC_PIPE_STDERR:) (const)
- [PROC_STDERR_TO_STDOUT:](#PROC_STDERR_TO_STDOUT:) (const)
- [PROC_STDOUT:](#PROC_STDOUT:) (const)
- [PROC_STDERR:](#PROC_STDERR:) (const)
- [process_spawn_io](#process_spawn_io) (function)
- [process_write](#process_write) (function)
- [process_close_stdin](#process_close_stdin) (function)
- [process_read](#process_read) (function)
- [process_read_all](#process_read_all) (function)
- [Child](#Child) (struct)
- [Child](#Child) (impl)

## process_exit

Exit the process with the given exit code.

```fides
pub fn process_exit(code: i64) -> void {
```

**Parameters:**

- `code`: `i64`

**Returns:** `void`

*Defined at line 18*

---

## process_system

Run a shell command and return the exit status.
Returns -1 on failure.

```fides
pub fn process_system(cmd: str) -> i64 {
```

**Parameters:**

- `cmd`: `str`

**Returns:** `i64`

*Defined at line 24*

---

## process_run

Run `cmd` through the platform shell (sh -c / cmd /C) capturing its
stdout, stderr and exit code. Returns an opaque result handle, or 0 if
the process could not be started.

```fides
pub fn process_run(cmd: str) -> i64 {
```

**Parameters:**

- `cmd`: `str`

**Returns:** `i64`

*Defined at line 36*

---

## process_ok

True when `h` is a live process result.

```fides
pub fn process_ok(h: i64) -> bool {
```

**Parameters:**

- `h`: `i64`

**Returns:** `bool`

*Defined at line 41*

---

## process_stdout

The captured standard output of a finished process ("" on bad handle).

```fides
pub fn process_stdout(h: i64) -> str {
```

**Parameters:**

- `h`: `i64`

**Returns:** `str`

*Defined at line 46*

---

## process_stderr

The captured standard error of a finished process ("" on bad handle).

```fides
pub fn process_stderr(h: i64) -> str {
```

**Parameters:**

- `h`: `i64`

**Returns:** `str`

*Defined at line 51*

---

## process_code

The process exit code (-1 on a bad handle).

```fides
pub fn process_code(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 56*

---

## process_close

Release a process result handle.

```fides
pub fn process_close(h: i64) -> void {
```

**Parameters:**

- `h`: `i64`

**Returns:** `void`

*Defined at line 61*

---

## process_pid

The current process id.

```fides
pub fn process_pid() -> i64 {
```

**Returns:** `i64`

*Defined at line 66*

---

## process_capture

Convenience: run `cmd`, return (stdout, code) after releasing the
handle. Useful when stderr is not needed.

```fides
pub fn process_capture(cmd: str) -> Vec<str> {
```

**Parameters:**

- `cmd`: `str`

**Returns:** `Vec<str>`

*Defined at line 72*

---

## Command

A child process to run. Build with Command::new and the methods below.

```fides
pub struct Command {
```

**Fields:**

| Name | Type |
|------|------|
| `program` | `str` |
| `args` | `Vec<str>` |
| `env_keys` | `Vec<str>` |
| `env_vals` | `Vec<str>` |
| `env_unset` | `Vec<str>` |
| `dir` | `str` |
| `stdin_data` | `str` |
| `has_stdin` | `bool` |
| `merge_stderr` | `bool` |
| `stages` | `Vec<str>,  // programs piped after this one, rendered` |
| `io` | `i64,  // spawn(): the PROC_PIPE_* streams asked for` |

*Defined at line 112*

---

## process_is_windows

True on Windows (cmd.exe quoting); sh quoting otherwise.

```fides
pub fn process_is_windows() -> bool {
```

**Returns:** `bool`

*Defined at line 129*

---

## process_quote

`s` quoted as one word for the platform shell (unchanged when it
needs no quotes). sh: single quotes; cmd: double quotes, a quote
inside doubled, trailing backslashes doubled before the closing quote.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(process_quote("plain"), "plain");
if process_is_windows() {
    assert_eq(process_quote("a b"), "\"a b\"");
} else {
    assert_eq(process_quote("a b"), "'a b'");
}
assert(process_pid() > 0);
```

```fides
pub fn process_quote(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 183*

---

## Command

```fides
impl Command {
```

**Methods:**

### new

A command that runs `program` (looked up on PATH like the shell does).

```fides
pub fn new(program: str) -> Command {
```

### arg

Add one argument (passed as one word, whatever it contains).

```fides
pub fn arg(self: &mut Command, a: str) -> void {
```

### args_all

Add several arguments.

```fides
pub fn args_all(self: &mut Command, list: Vec<str>) -> void {
```

### env

Set environment variable `key` to `value` for the child.

```fides
pub fn env(self: &mut Command, key: str, value: str) -> void {
```

### env_remove

Remove environment variable `key` for the child.

```fides
pub fn env_remove(self: &mut Command, key: str) -> void {
```

### cwd

Run the child in directory `path`. If it cannot change there, the
program does not run and the exit code is non-zero.

```fides
pub fn cwd(self: &mut Command, path: str) -> void {
```

### stdin

Give the child `data` as its standard input (otherwise it gets the
parent's).

```fides
pub fn stdin(self: &mut Command, data: str) -> void {
```

### stderr_to_stdout

Send the child's standard error to its standard output.

```fides
pub fn stderr_to_stdout(self: &mut Command) -> void {
```

### pipe

Pipe this command's standard output into `next` (its program and
arguments; its environment and directory are not used). Piping
again adds a further stage.

```fides
pub fn pipe(self: &mut Command, next: &Command) -> void {
```

### render_stage

```fides
fn render_stage(self: &Command) -> str {
```

### command_line

The shell command line this runs, with `stdin_path` as the file
its input is read from ("" for none).

```fides
pub fn command_line(self: &Command, stdin_path: str) -> str {
```

### output

Run the command to completion; returns a process result handle
(process_stdout / process_stderr / process_code / process_close),
or 0 when it could not start.

```fides
pub fn output(self: &Command) -> i64 {
```

### status

Run the command to completion and return its exit code (-1 when it
could not start). Output is captured and dropped.

```fides
pub fn status(self: &Command) -> i64 {
```

### pipe_stdin

spawn(): give the child a pipe as its standard input
(Child::write, Child::close_stdin).

```fides
pub fn pipe_stdin(self: &mut Command) -> void {
```

### pipe_stdout

spawn(): give the child a pipe as its standard output
(Child::read_stdout, Child::read_all_stdout).

```fides
pub fn pipe_stdout(self: &mut Command) -> void {
```

### pipe_stderr

spawn(): give the child a pipe as its standard error
(Child::read_stderr, Child::read_all_stderr).

```fides
pub fn pipe_stderr(self: &mut Command) -> void {
```

### spawn

Start the program directly, without a shell, and return at once
(roadmap L4H): each argument reaches it as one word whatever it
holds (no cmd.exe or sh quoting, no `%NAME%` expansion), the
environment changes and the directory apply, and the streams asked
for with pipe_stdin / pipe_stdout / pipe_stderr are pipes to this
process; the others are ours (stderr_to_stdout: stderr goes where
stdout goes). Data given with stdin(data) is written to the child
at once, then its input is closed unless pipe_stdin was asked for.
None when the program could not start (on POSIX one that is not
found starts and exits with 127) or the command has pipe() stages
(a pipeline runs through the shell: output()).

Example:
var c: Command = Command::new("sort");
c.stdin("pear\napple\n");
c.pipe_stdout();
match c.spawn() {
Option::Some(ch) => {
var child: Child = ch;
val text: str = child.read_all_stdout();   // "apple\npear\n"
val code: i64 = child.wait();              // 0
child.release();
}
Option::None => { }
}

```fides
pub fn spawn(self: &Command) -> Option<Child> {
```

*Defined at line 234*

---

## process_spawn

Start `argv` (program, then its arguments; the program is looked up on
PATH) with `env` added to the environment, in directory `dir` ("" for
ours), its stdout and stderr going where ours go. Returns at once: a
handle for process_wait / process_kill / process_release, 0 when it
could not start.

```fides
pub fn process_spawn(argv: Vec<str>, env: Vec<str>, dir: str) -> i64 {
```

**Parameters:**

- `argv`: `Vec<str>`
- `env`: `Vec<str>`
- `dir`: `str`

**Returns:** `i64`

*Defined at line 553*

---

## process_spawn_to

process_spawn with the child's stdout and stderr both written to the
file `out_path` (created or emptied; "" for ours).

```fides
pub fn process_spawn_to(argv: Vec<str>, env: Vec<str>, dir: str, out_path: str) -> i64 {
```

**Parameters:**

- `argv`: `Vec<str>`
- `env`: `Vec<str>`
- `dir`: `str`
- `out_path`: `str`

**Returns:** `i64`

*Defined at line 559*

---

## process_spawn_files

process_spawn with the child's stdout written to the file `out_path`
and its stderr to `err_path` (each created or emptied; err_path "":
with stdout; out_path "": ours).

```fides
pub fn process_spawn_files(argv: Vec<str>, env: Vec<str>, dir: str, out_path: str, err_path: str) -> i64 {
```

**Parameters:**

- `argv`: `Vec<str>`
- `env`: `Vec<str>`
- `dir`: `str`
- `out_path`: `str`
- `err_path`: `str`

**Returns:** `i64`

*Defined at line 566*

---

## process_wait

Wait for child `h` at most `timeout_ms` milliseconds (negative: as long
as it takes; 0: only look). Some(exit code) once it has ended (a child
killed by signal n: 128 + n; one process_kill stopped on Windows: 137),
None while it is still running (or for handle 0). Asking again after it
ended gives the same code.

```fides
pub fn process_wait(h: i64, timeout_ms: i64) -> Option<i64> {
```

**Parameters:**

- `h`: `i64`
- `timeout_ms`: `i64`

**Returns:** `Option<i64>`

*Defined at line 619*

---

## process_kill

Stop child `h` (SIGKILL; TerminateProcess on Windows). True when it was
running and was told to stop; process_wait then gives its code.

```fides
pub fn process_kill(h: i64) -> bool {
```

**Parameters:**

- `h`: `i64`

**Returns:** `bool`

*Defined at line 644*

---

## process_child_pid

The operating system's process id of child `h` (-1 for a bad handle).

```fides
pub fn process_child_pid(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 657*

---

## process_release

Forget child `h` (wait for it first: one still running keeps running).

```fides
pub fn process_release(h: i64) -> void {
```

**Parameters:**

- `h`: `i64`

**Returns:** `void`

*Defined at line 670*

---

## ChildResult

What process_run_argv found: the exit code (-1 when the program did not
start or did not end), whether it was stopped at the time limit, its
stdout (output) and its stderr (errors).

```fides
pub struct ChildResult {
```

**Fields:**

| Name | Type |
|------|------|
| `started` | `bool` |
| `timed_out` | `bool` |
| `code` | `i64` |
| `output` | `str` |
| `errors` | `str` |

*Defined at line 685*

---

## process_run_argv

Run `argv` (no shell) with `env` added, in `dir`, for at most
`timeout_ms` milliseconds (negative: no limit); one still running then
is killed. Its stdout and stderr are captured through two files in the
temp directory.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var argv: Vec<str> = Vec::new();
if process_is_windows() {
    argv.push("cmd");
    argv.push("/c");
    argv.push("echo hi& echo oops 1>&2& exit 3");
} else {
    argv.push("sh");
    argv.push("-c");
    argv.push("echo hi; echo oops >&2; exit 3");
}
val r: ChildResult = process_run_argv(argv, Vec::new(), "", 30000);
assert(r.started);
assert(!r.timed_out);
assert_eq(r.code, 3);
assert_eq(str_trim(r.output), "hi");
assert_eq(str_trim(r.errors), "oops");
```

```fides
pub fn process_run_argv(argv: Vec<str>, env: Vec<str>, dir: str, timeout_ms: i64) -> ChildResult {
```

**Parameters:**

- `argv`: `Vec<str>`
- `env`: `Vec<str>`
- `dir`: `str`
- `timeout_ms`: `i64`

**Returns:** `ChildResult`

*Defined at line 729*

---

## PROC_PIPE_STDIN:

process_spawn_io: the child's standard input is a pipe (process_write).

```fides
pub const PROC_PIPE_STDIN: i64 = 1;
```

*Defined at line 788*

---

## PROC_PIPE_STDOUT:

The child's standard output is a pipe (process_read(h, PROC_STDOUT)).

```fides
pub const PROC_PIPE_STDOUT: i64 = 2;
```

*Defined at line 790*

---

## PROC_PIPE_STDERR:

The child's standard error is a pipe (process_read(h, PROC_STDERR)).

```fides
pub const PROC_PIPE_STDERR: i64 = 4;
```

*Defined at line 792*

---

## PROC_STDERR_TO_STDOUT:

The child's standard error goes where its standard output goes.

```fides
pub const PROC_STDERR_TO_STDOUT: i64 = 8;
```

*Defined at line 794*

---

## PROC_STDOUT:

process_read: which stream.

```fides
pub const PROC_STDOUT: i64 = 1;
```

*Defined at line 796*

---

## PROC_STDERR:

```fides
pub const PROC_STDERR: i64 = 2;
```

*Defined at line 797*

---

## process_spawn_io

Start `argv` (no shell; the program looked up on PATH) with `env`
added and in `dir` as process_spawn does, with pipes to the streams
`io` names (PROC_PIPE_STDIN | PROC_PIPE_STDOUT | PROC_PIPE_STDERR, and
PROC_STDERR_TO_STDOUT); the others are ours. Returns at once: a handle
(0 when it could not start) for process_write / process_read /
process_close_stdin and process_wait / process_kill / process_release
(which closes the pipes still open).

```fides
pub fn process_spawn_io(argv: Vec<str>, env: Vec<str>, dir: str, io: i64) -> i64 {
```

**Parameters:**

- `argv`: `Vec<str>`
- `env`: `Vec<str>`
- `dir`: `str`
- `io`: `i64`

**Returns:** `i64`

*Defined at line 809*

---

## process_write

Write all of `data` to child `h`'s standard input pipe: false when it
has none, it was closed, or the child has closed its end.

```fides
pub fn process_write(h: i64, data: str) -> bool {
```

**Parameters:**

- `h`: `i64`
- `data`: `str`

**Returns:** `bool`

*Defined at line 845*

---

## process_close_stdin

Close child `h`'s standard input pipe: it reads the end of its input.

```fides
pub fn process_close_stdin(h: i64) -> void {
```

**Parameters:**

- `h`: `i64`

**Returns:** `void`

*Defined at line 862*

---

## process_read

What child `h` wrote next to its stdout (which PROC_STDOUT) or stderr
(PROC_STDERR) pipe: at most 64 KB, waiting for at least one byte. None
once the pipe is empty and closed (the child ended or closed it) or
when there is no such pipe.

```fides
pub fn process_read(h: i64, which: i64) -> Option<str> {
```

**Parameters:**

- `h`: `i64`
- `which`: `i64`

**Returns:** `Option<str>`

*Defined at line 876*

---

## process_read_all

Everything child `h` writes to the stream until it closes it.

```fides
pub fn process_read_all(h: i64, which: i64) -> str {
```

**Parameters:**

- `h`: `i64`
- `which`: `i64`

**Returns:** `str`

*Defined at line 923*

---

## Child

A running child process with pipes (Command::spawn).

```fides
pub struct Child {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64,  // 0 once released` |
| `stdin_open` | `bool` |

*Defined at line 940*

---

## Child

```fides
impl Child {
```

**Methods:**

### id

The operating system's process id (-1 once released).

```fides
pub fn id(self: &Child) -> i64 {
```

### write

Write `data` to the child's standard input (pipe_stdin): false when
it has no pipe or the child has stopped reading.

```fides
pub fn write(self: &mut Child, data: str) -> bool {
```

### close_stdin

Close the child's standard input: it reads the end of its input.

```fides
pub fn close_stdin(self: &mut Child) -> void {
```

### read_stdout

What the child wrote next to its standard output (pipe_stdout),
waiting for it; None at the end.

```fides
pub fn read_stdout(self: &mut Child) -> Option<str> {
```

### read_stderr

What the child wrote next to its standard error (pipe_stderr).

```fides
pub fn read_stderr(self: &mut Child) -> Option<str> {
```

### read_all_stdout

All the child's standard output, to its end.

```fides
pub fn read_all_stdout(self: &mut Child) -> str {
```

### read_all_stderr

All the child's standard error, to its end.

```fides
pub fn read_all_stderr(self: &mut Child) -> str {
```

### wait

Wait for the child to end (its input is closed first) and give its
exit code (killed by signal n: 128 + n; Windows kill: 137), -1 for
a released child.

```fides
pub fn wait(self: &mut Child) -> i64 {
```

### wait_timeout

Wait at most `timeout_ms` milliseconds: Some(exit code) once it has
ended, None while it is still running.

```fides
pub fn wait_timeout(self: &mut Child, timeout_ms: i64) -> Option<i64> {
```

### kill

Stop the child; true when it was running and was told to stop.

```fides
pub fn kill(self: &mut Child) -> bool {
```

### release

Forget the child and close its pipes (wait for it first: one still
running keeps running).

```fides
pub fn release(self: &mut Child) -> void {
```

*Defined at line 952*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
