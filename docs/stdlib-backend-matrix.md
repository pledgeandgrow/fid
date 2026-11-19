# The standard library on every backend

Complaint CD2 asks for a standard library whose modules are "each tested on
every backend". `tests/stdlib_backends.sh` is the check: for each stdlib test
program (`tests/bootstrap/test_lib_*.fd`, the newer module tests and the
competitive SL track's tests: HTTP, JSON, TLS, regex, time, CSV, compression,
encodings), it runs

| column   | what                                                            |
|----------|-----------------------------------------------------------------|
| `run`    | `fid run` (the interpreter)                                     |
| `c`      | `fid ccompile` (CC, default `zig cc`), then the executable      |
| `native` | `fid emit` (pe_ir on Windows; nat_ir with `TARGET=<triple>`)    |
| `expected` | the output against the test's `// Expected output:` block     |

and compares standard output (CR stripped) with the interpreter's (or, with
`run` skipped, with the first backend's: `ref`). `NOLIB` is a C library this
machine lacks (SDL2, sqlite3), not counted. Exit status 1 when anything
differs or fails to build.

```bash
FID=release/fid.exe bash tests/stdlib_backends.sh                  # everything
FID=release/fid.exe LIST=sl bash tests/stdlib_backends.sh          # SL tests only
FID=release/fid.exe bash tests/stdlib_backends.sh tests/bootstrap/test_zstd_compress.fd
TARGET=x86_64-linux  SKIP=run,c FID=release/fid.exe bash tests/stdlib_backends.sh
TARGET=aarch64-linux SKIP=run,c FID=release/fid.exe bash tests/stdlib_backends.sh   # qemu in WSL
```

One fid process at a time; several at once slow each other down (and on a
loaded machine a build can fail for lack of memory: rerun the program alone
before calling it a failure).

## Run of Nov 17 (C and native Windows)

With the previous day's compiler (no interpreter column: that compiler's
`run` lacks today's core.fd builtins), 116 programs plus the two new
compression tests (which agree). Agreed on C and native, and with their
expected output where they have one, on the first pass: 102. The other 14:

| program | backend | cause | where it is fixed |
|---|---|---|---|
| `test_regex_flags` | expected | `\b`/`\B` searches stopped at the first position where the assertion failed (`concat cat` found no `\bcat`) | `stdlib/regex.fd` `rv_run` (this run) |
| `test_lib_sys` | c, native | `stdlib/sys.fd` called POSIX `sysconf`, `getuid`, ... on Windows | `sys.fd` per-system `sx_*` (@cfg) (this run); native also needs `GlobalMemoryStatusEx` in pe_ir's kernel32 imports |
| `test_lib_unicode` | c | the C generator's value names (`v1`, `v2`) hid the test's functions `v1`, `v2` | `codegen_c.fd` `sanitize_c_name` (already in today's compiler) |
| `test_lib_profiler` | c | the C runtime had no `p_prof_*` | `codegen_c.fd` (already in today's compiler) |
| `test_lib_ota`, `test_hmac_sha512`, `test_crypto_tls_vectors`, `test_runtime_tls_record`, `test_stdlib_tls13_schedule`, `test_stdlib_http3` | native | that compiler's `@secret` check did not know `@declassify` | today's compiler |
| `test_stdlib_http_binary` | native | a body with NUL bytes lost its bytes (known-bugs 2g.41) | `pe_ir_stubs.fd` (another group) |
| `test_http2_frames` | expected | the script read prose ("Expected output: every ...") as an expected block | `tests/stdlib_backends.sh` (this run) |

`test_lib_os` and `test_lib_http_client` failed for lack of memory (other
builds ran at the same time) and pass alone; so did `test_decimal` in one
of two runs.

Linux targets (`TARGET=x86_64-linux`, `aarch64-linux`) could not be checked
with that compiler: its nat_ir build of any stdlib program needs
`nat_tls_set`, and its e-graph pass overflowed on the XXH64 constants of
`zstd.fd` (both fixed in today's sources). Run them with the release
compiler.

## Depth added for the SL track

- `stdlib/zstd.fd`: a compressor (`zstd_compress`, `zstd_compress_level`,
  `zstd_compress_str`): hash-chain matching with repeat offsets, raw, RLE
  and Huffman literals (direct or FSE-compressed weights, one or four
  streams), predefined sequence tables, RLE and raw blocks, the checksum.
  `test_zstd_compress.fd`; its frames decode with libzstd (Python 3.14's
  `compression.zstd`).
- `stdlib/brotli.fd`: a compressor (`brotli_compress`,
  `brotli_compress_level`, `brotli_compress_str`): LZ77 with the last
  distance, simple and complex prefix codes (repeat codes 16 and 17),
  uncompressed meta-blocks when nothing compresses. `test_brotli_compress.fd`;
  its streams decode with the reference decoder (Python's `brotli`).
