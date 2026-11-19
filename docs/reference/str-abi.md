# The `str` ABI: one pointer, not (pointer, length)

Decision record for competitive-roadmap RT2 (Nov 16 2026). RT3 recorded
its own decision the same way.

## What RT2 asked for

`str` carries its length: `str_len` is O(1), zero bytes are allowed, and
HTTP bodies can be binary. The last open question was whether a `str`
should become a two-word value, `{ptr, len}`, on every backend.

## What a `str` is today

A `str` is one machine word: the address of its first byte. The words
in front of that address say what it is:

| Kind | `s - 16` | `s - 8` | Made by |
|------|----------|---------|---------|
| heap string | `P_STR_RC + count` | length | `p_str_alloc` (core), `p_str_alloc_c` (C), the native concat stub, `p_str_adopt` for runtime results |
| literal | `P_STR_STATIC` | length | the C string table, pe_ir and nat_ir `.rdata` |
| one-byte string (RT3) | `P_STR_STATIC` | 1 | the C runtime's 256 static strings, `pert_char_to_str` |
| plain C string | anything | anything | a C function's result, before `p_str_adopt` |

`p_str_hdr_len` checks the header, `p_str_length` falls back to a NUL scan
for a plain C string. Every byte after the address, up to the length, is
part of the string, zero bytes included. A NUL follows the last byte,
so the address can go straight to a C function that wants `char*`.

So the three things RT2 asked for already hold with one word:

- `str_len` is O(1): it reads `s - 8` (test_str_length).
- Zero bytes are allowed: the string functions go by the stored length
  (test_str_zero_bytes); a `\0` in a literal is a byte of it
  (test_frontend_str_nul).
- Binary data reaches files and sockets: HTTP bodies are binary (release
  I8), and `p_file_write_bin(path, data, n)` writes `n` bytes, zero bytes
  included, on the interpreter, C, pe_ir and the machine layer
  (test_file_write_bin). `p_file_write`/`p_file_append` stay text
  builtins that stop at the first zero byte, as `fputs` does
  (test_binary_io).

## Decision: keep the one-word `str`

The two-word ABI is not going to be done. Reasons:

1. **It buys nothing RT2 still needs.** Length, zero bytes and binary
   I/O work now. What a fat pointer adds is a substring without a copy,
   and `StrView` (stdlib/string.fd: `{base, start, len}`) gives that
   where a program asks for it.
2. **It changes every backend at once.** The interpreter, the C
   generator, pe_ir, nat_ir (three architectures), the WASM path and both
   native runtimes pass a `str` in one register and store it in one word.
   Two words means new calling conventions for every function that takes
   or returns a `str`, new layouts for every struct with a `str` field,
   `Vec<str>` elements of two words (element code 16 and up), and
   HashMap keys of two words. No part of that can be tested alone: the
   compiler is written in Fides and must reach its fixpoint after the
   change.
3. **C interop gets worse.** A `str` goes to `extern "C"` functions as
   `char*` with nothing done to it, because a NUL follows the bytes. A
   fat `str` would need a conversion at every FFI call, or a second
   string type for C.
4. **Code relies on one word.** `(s as i64) == 0` tests a runtime result
   for null (AGENTS.md known bug 18); `p_file_read`, `p_getenv` and
   `p_argv` return a raw `i64`; the ownership rules (proposal 0004) drop a
   string by its one address. All of that would need rewriting.
5. **Cost.** Every `str` argument, return and field would take twice the
   registers and memory, for programs that never slice.

## What would reopen it

A measured case where copying substrings is the bottleneck and `StrView`
cannot be used (for example a parser API that must return `str`). The
smaller step to try first would be a slice that shares its base string's
buffer under the same one-word ABI (a header kind that points at its
owner), not a fat pointer.

## Validation

```bash
$FID run       tests/bootstrap/test_file_write_bin.fd
$FID ccompile  tests/bootstrap/test_file_write_bin.fd -o fwb.exe && ./fwb.exe
$FID emit      tests/bootstrap/test_file_write_bin.fd -o fwb_pe.exe && ./fwb_pe.exe
$FID emit      tests/bootstrap/test_file_write_bin.fd --nat -o fwb_nat.exe && ./fwb_nat.exe
$FID emit      tests/bootstrap/test_file_write_bin.fd --target x86_64-linux -o fwb   # run in WSL
```

Each prints the lines in the test's `// Expected output:` comment.
