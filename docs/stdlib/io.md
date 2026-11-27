[All modules](index.md)

# io -- Fides Documentation

## Contents

- [STDIN:](#STDIN:) (const)
- [STDOUT:](#STDOUT:) (const)
- [STDERR:](#STDERR:) (const)
- [io_print](#io_print) (function)
- [io_println](#io_println) (function)
- [io_print_int](#io_print_int) (function)
- [io_println_int](#io_println_int) (function)
- [io_eprint](#io_eprint) (function)
- [io_eprintln](#io_eprintln) (function)
- [io_putc](#io_putc) (function)
- [io_newline](#io_newline) (function)
- [io_getc](#io_getc) (function)
- [io_readln](#io_readln) (function)
- [io_read_all](#io_read_all) (function)
- [io_read_lines](#io_read_lines) (function)
- [io_flush](#io_flush) (function)
- [io_flush_err](#io_flush_err) (function)
- [io_print_args](#io_print_args) (function)
- [io_print_separator](#io_print_separator) (function)
- [io_print_header](#io_print_header) (function)
- [IO_SRC_BYTES:](#IO_SRC_BYTES:) (const)
- [IO_SRC_FILE:](#IO_SRC_FILE:) (const)
- [IO_SRC_STDIN:](#IO_SRC_STDIN:) (const)
- [IO_SINK_STDOUT:](#IO_SINK_STDOUT:) (const)
- [IO_SINK_STDERR:](#IO_SINK_STDERR:) (const)
- [IO_SINK_FILE:](#IO_SINK_FILE:) (const)
- [IO_SINK_BYTES:](#IO_SINK_BYTES:) (const)
- [IO_BUF_SIZE:](#IO_BUF_SIZE:) (const)
- [Reader](#Reader) (struct)
- [Reader](#Reader) (impl)
- [BufReader](#BufReader) (struct)
- [BufReader](#BufReader) (impl)
- [Writer](#Writer) (struct)
- [Writer](#Writer) (impl)
- [BufWriter](#BufWriter) (struct)
- [BufWriter](#BufWriter) (impl)

## STDIN:

```fides
pub const STDIN: i64 = 0;
```

*Defined at line 31*

---

## STDOUT:

```fides
pub const STDOUT: i64 = 1;
```

*Defined at line 32*

---

## STDERR:

```fides
pub const STDERR: i64 = 2;
```

*Defined at line 33*

---

## io_print

Print a string without a newline.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
io_print("total: ");
io_println_int(42);
io_print_separator(45, 10);
io_eprintln("diagnostics go to stderr");
io_flush();
```

```fides
pub fn io_print(s: str) -> void {
```

**Parameters:**

- `s`: `str`

**Returns:** `void`

*Defined at line 44*

---

## io_println

Print a string followed by a newline.

```fides
pub fn io_println(s: str) -> void {
```

**Parameters:**

- `s`: `str`

**Returns:** `void`

*Defined at line 49*

---

## io_print_int

Print an integer.

```fides
pub fn io_print_int(n: i64) -> void {
```

**Parameters:**

- `n`: `i64`

**Returns:** `void`

*Defined at line 54*

---

## io_println_int

Print an integer followed by a newline.

```fides
pub fn io_println_int(n: i64) -> void {
```

**Parameters:**

- `n`: `i64`

**Returns:** `void`

*Defined at line 59*

---

## io_eprint

Print to stderr.

```fides
pub fn io_eprint(s: str) -> void {
```

**Parameters:**

- `s`: `str`

**Returns:** `void`

*Defined at line 64*

---

## io_eprintln

Print to stderr followed by a newline.

```fides
pub fn io_eprintln(s: str) -> void {
```

**Parameters:**

- `s`: `str`

**Returns:** `void`

*Defined at line 69*

---

## io_putc

Print a character.

```fides
pub fn io_putc(ch: i64) -> void {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `void`

*Defined at line 75*

---

## io_newline

Print a newline.

```fides
pub fn io_newline() -> void {
```

**Returns:** `void`

*Defined at line 84*

---

## io_getc

Read a single character from stdin. Returns -1 on EOF.

```fides
pub fn io_getc() -> i64 {
```

**Returns:** `i64`

*Defined at line 95*

---

## io_readln

Read a line from stdin (without the trailing newline).
Returns "" on EOF.

```fides
pub fn io_readln() -> str {
```

**Returns:** `str`

*Defined at line 105*

---

## io_read_all

Read all of stdin into a string.

```fides
pub fn io_read_all() -> str {
```

**Returns:** `str`

*Defined at line 117*

---

## io_read_lines

Read all of stdin into a Vec of lines.

```fides
pub fn io_read_lines() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 128*

---

## io_flush

Flush stdout.

```fides
pub fn io_flush() -> void {
```

**Returns:** `void`

*Defined at line 151*

---

## io_flush_err

Flush stderr: it is not buffered.

```fides
pub fn io_flush_err() -> void {
```

**Returns:** `void`

*Defined at line 156*

---

## io_print_args

Print multiple values separated by spaces, followed by a newline.

```fides
pub fn io_print_args(args: Vec<i64>) -> void {
```

**Parameters:**

- `args`: `Vec<i64>`

**Returns:** `void`

*Defined at line 162*

---

## io_print_separator

Print a separator line.

```fides
pub fn io_print_separator(ch: i64, width: i64) -> void {
```

**Parameters:**

- `ch`: `i64`
- `width`: `i64`

**Returns:** `void`

*Defined at line 177*

---

## io_print_header

Print a header with separator lines above and below.

```fides
pub fn io_print_header(title: str, width: i64) -> void {
```

**Parameters:**

- `title`: `str`
- `width`: `i64`

**Returns:** `void`

*Defined at line 187*

---

## IO_SRC_BYTES:

Reader sources.

```fides
pub const IO_SRC_BYTES: i64 = 0;
```

*Defined at line 240*

---

## IO_SRC_FILE:

```fides
pub const IO_SRC_FILE: i64 = 1;
```

*Defined at line 241*

---

## IO_SRC_STDIN:

```fides
pub const IO_SRC_STDIN: i64 = 2;
```

*Defined at line 242*

---

## IO_SINK_STDOUT:

Writer sinks.

```fides
pub const IO_SINK_STDOUT: i64 = 0;
```

*Defined at line 244*

---

## IO_SINK_STDERR:

```fides
pub const IO_SINK_STDERR: i64 = 1;
```

*Defined at line 245*

---

## IO_SINK_FILE:

```fides
pub const IO_SINK_FILE: i64 = 2;
```

*Defined at line 246*

---

## IO_SINK_BYTES:

```fides
pub const IO_SINK_BYTES: i64 = 3;
```

*Defined at line 247*

---

## IO_BUF_SIZE:

BufReader's chunk and BufWriter's pending bytes by default.

```fides
pub const IO_BUF_SIZE: i64 = 8192;
```

*Defined at line 249*

---

## Reader

A byte source.

```fides
pub struct Reader {
```

**Fields:**

| Name | Type |
|------|------|
| `src` | `i64` |
| `path` | `str` |
| `data` | `Vec<i64>,  // bytes and file: everything; stdin: bytes pulled, not yet read` |
| `pos` | `i64,  // next byte of data` |
| `eof` | `bool,  // stdin: the stream has ended` |

*Defined at line 252*

---

## Reader

```fides
impl Reader {
```

**Methods:**

### from_bytes

A Reader over bytes in memory.

```fides
pub fn from_bytes(bytes: Vec<i64>) -> Reader {
```

### from_str

A Reader over the bytes of a string.

```fides
pub fn from_str(s: str) -> Reader {
```

### open

A Reader over a file; None if it does not exist or is a directory.

```fides
pub fn open(path: str) -> Option<Reader> {
```

### from_stdin

A Reader over standard input.

```fides
pub fn from_stdin() -> Reader {
```

### read

Up to n bytes; [] at the end. On standard input it waits for n
bytes or the end of the stream.

```fides
pub fn read(self: &mut Reader, n: i64) -> Vec<i64> {
```

### read_byte

One byte, or -1 at the end.

```fides
pub fn read_byte(self: &mut Reader) -> i64 {
```

### read_to_end

Everything left.

```fides
pub fn read_to_end(self: &mut Reader) -> Vec<i64> {
```

### read_to_string

Everything left, as text.

```fides
pub fn read_to_string(self: &mut Reader) -> str {
```

### read_line

The next line without its newline (and a '\r' before it); None at
the end. On standard input it reads a byte at a time.

```fides
pub fn read_line(self: &mut Reader) -> Option<str> {
```

### at_end

True when nothing is left (on standard input: once a read found
its end).

```fides
pub fn at_end(self: &Reader) -> bool {
```

*Defined at line 297*

---

## BufReader

A buffered byte source: a Reader that pulls standard input in chunks.

```fides
pub struct BufReader {
```

**Fields:**

| Name | Type |
|------|------|
| `src` | `i64` |
| `path` | `str` |
| `data` | `Vec<i64>` |
| `pos` | `i64` |
| `eof` | `bool` |
| `cap` | `i64,  // stdin: bytes pulled at a time` |

*Defined at line 426*

---

## BufReader

```fides
impl BufReader {
```

**Methods:**

### new

Buffer a Reader, IO_BUF_SIZE bytes at a time.

```fides
pub fn new(r: Reader) -> BufReader {
```

### with_capacity

Buffer a Reader, `cap` bytes (at least 1) at a time.

```fides
pub fn with_capacity(r: Reader, cap: i64) -> BufReader {
```

### fill

Pull the next chunk of standard input; false when none came.

```fides
fn fill(self: &mut BufReader) -> bool {
```

### read

Up to n bytes; [] at the end.

```fides
pub fn read(self: &mut BufReader, n: i64) -> Vec<i64> {
```

### read_byte

One byte, or -1 at the end.

```fides
pub fn read_byte(self: &mut BufReader) -> i64 {
```

### read_until

The bytes up to and including the next `delim` (or to the end);
[] at the end.

```fides
pub fn read_until(self: &mut BufReader, delim: i64) -> Vec<i64> {
```

### read_line

The next line without its newline (and a '\r' before it); None at
the end.

```fides
pub fn read_line(self: &mut BufReader) -> Option<str> {
```

### lines

Every line left.

```fides
pub fn lines(self: &mut BufReader) -> Vec<str> {
```

### read_to_end

Everything left.

```fides
pub fn read_to_end(self: &mut BufReader) -> Vec<i64> {
```

### read_to_string

Everything left, as text.

```fides
pub fn read_to_string(self: &mut BufReader) -> str {
```

### at_end

True when nothing is left (on standard input: once a read found
its end).

```fides
pub fn at_end(self: &BufReader) -> bool {
```

*Defined at line 435*

---

## Writer

A byte sink. Writes go out at once (a BufWriter batches them).

```fides
pub struct Writer {
```

**Fields:**

| Name | Type |
|------|------|
| `sink` | `i64` |
| `path` | `str` |
| `mem` | `Vec<i64>,  // IO_SINK_BYTES: everything written` |
| `failed` | `bool,  // a file write failed` |
| `written` | `i64,  // bytes accepted so far` |

*Defined at line 622*

---

## Writer

```fides
impl Writer {
```

**Methods:**

### to_stdout

A Writer to standard output.

```fides
pub fn to_stdout() -> Writer {
```

### to_stderr

A Writer to standard error.

```fides
pub fn to_stderr() -> Writer {
```

### memory

A Writer that keeps what it is given (see bytes()).

```fides
pub fn memory() -> Writer {
```

### create

A Writer to a new (or truncated) file; None if it cannot be created.

```fides
pub fn create(path: str) -> Option<Writer> {
```

### append

A Writer to the end of a file, created if missing.

```fides
pub fn append(path: str) -> Option<Writer> {
```

### write

Write bytes; the number written (all of them, or 0 once a write
failed).

```fides
pub fn write(self: &mut Writer, bytes: Vec<i64>) -> i64 {
```

### write_all

Write all the bytes; false if the sink failed.

```fides
pub fn write_all(self: &mut Writer, bytes: Vec<i64>) -> bool {
```

### write_str

Write the bytes of a string; false if the sink failed.

```fides
pub fn write_str(self: &mut Writer, s: str) -> bool {
```

### write_line

Write a string and a newline.

```fides
pub fn write_line(self: &mut Writer, s: str) -> bool {
```

### flush

Push out what the runtime holds (standard output's buffer); false
once a write failed.

```fides
pub fn flush(self: &mut Writer) -> bool {
```

### bytes

What a memory Writer was given.

```fides
pub fn bytes(self: &Writer) -> Vec<i64> {
```

### written

The bytes accepted so far.

```fides
pub fn written(self: &Writer) -> i64 {
```

*Defined at line 630*

---

## BufWriter

A buffered byte sink: writes are kept until `cap` bytes are pending,
then handed to the sink in one write; flush() hands over the rest.
Call flush() (or into_inner()) before dropping it: what is pending is
lost otherwise.

```fides
pub struct BufWriter {
```

**Fields:**

| Name | Type |
|------|------|
| `sink` | `i64` |
| `path` | `str` |
| `mem` | `Vec<i64>` |
| `failed` | `bool` |
| `written` | `i64` |
| `buf` | `Vec<i64>,  // pending bytes` |
| `cap` | `i64` |
| `writes` | `i64,  // writes handed to the sink so far` |

*Defined at line 739*

---

## BufWriter

```fides
impl BufWriter {
```

**Methods:**

### new

Buffer a Writer, IO_BUF_SIZE bytes.

```fides
pub fn new(w: Writer) -> BufWriter {
```

### with_capacity

Buffer a Writer, up to `cap` (at least 1) pending bytes.

```fides
pub fn with_capacity(w: Writer, cap: i64) -> BufWriter {
```

### drain

Hand the pending bytes to the sink.

```fides
fn drain(self: &mut BufWriter) -> bool {
```

### write

Write bytes; the number accepted (all of them, or 0 once the sink
failed).

```fides
pub fn write(self: &mut BufWriter, bytes: Vec<i64>) -> i64 {
```

### write_all

Write all the bytes; false if the sink failed.

```fides
pub fn write_all(self: &mut BufWriter, bytes: Vec<i64>) -> bool {
```

### write_str

Write the bytes of a string.

```fides
pub fn write_str(self: &mut BufWriter, s: str) -> bool {
```

### write_line

Write a string and a newline.

```fides
pub fn write_line(self: &mut BufWriter, s: str) -> bool {
```

### flush

Hand everything pending to the sink and flush it; false if the sink
failed.

```fides
pub fn flush(self: &mut BufWriter) -> bool {
```

### pending

The bytes waiting for the next write to the sink.

```fides
pub fn pending(self: &BufWriter) -> i64 {
```

### bytes

What a memory sink was given (pending bytes not included).

```fides
pub fn bytes(self: &BufWriter) -> Vec<i64> {
```

### into_inner

Flush and give the Writer back.

```fides
pub fn into_inner(self: &mut BufWriter) -> Writer {
```

*Defined at line 750*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
