# Lab 12 — A safe wrapper around C

**Unit:** [week 12, systems programming II: C, machine code and the back ends](../../week12-ffi-backends.md)
(exercise 1). **Time:** 2 hours. **Runs in the playground:** no

The playground runs programs in WebAssembly, where there is no C library
to call. Write the lab in the playground if you like, but build and run it
with `fid ccompile` (a C compiler is needed: `CC="zig cc"` works
everywhere). `fid run` cannot call `atoi`: the interpreter knows only a
few C functions.

## Start

- [Open the starter in the playground](https://fides-lang.org/playground#JTJGJTJGJTIwTGFiJTIwMTIlMjBzdGFydGVyJTNBJTIwYSUyMHNhZmUlMjB3cmFwcGVyJTIwYXJvdW5kJTIwQyUyMGZ1bmN0aW9ucy4lMEElMkYlMkYhJTIwbGFiJTNBJTIwMTIlMEElMkYlMkYhJTIwY2hlY2slM0ElMjBydW5zJTBBJTJGJTJGISUyMGVudiUzQSUyMEZJRF9MQUJfUE9SVCUzRDgwODAlMEElMkYlMkYhJTIwZW52JTNBJTIwRklEX0xBQl9CQUQlM0RlaWdodHklMEElMkYlMkYlMEElMkYlMkYlMjBUYXNrcyUyMChzZWUlMjBSRUFETUUubWQpJTNBJTBBJTJGJTJGJTIwJTIwJTIwMS4lMjBEZWNsYXJlJTIwJTYwZ2V0ZW52JTYwJTIwYW5kJTIwJTYwYXRvaSU2MCUyMGZyb20lMjAlM0NzdGRsaWIuaCUzRSUyMGluJTIwYW4lMjBleHRlcm4lMjAlMjJDJTIyJTIwYmxvY2suJTBBJTJGJTJGJTIwJTIwJTIwMi4lMjBXcml0ZSUyMGElMjBzYWZlJTIwJTYwZW52X2ludChuYW1lJTNBJTIwc3RyJTJDJTIwZGVmYXVsdCUzQSUyMGk2NCklMjAtJTNFJTIwaTY0JTYwJTNBJTIwZXZlcnklMEElMkYlMkYlMjAlMjAlMjAlMjAlMjAlMjAlNjB1bnNhZmUlNjAlMjBibG9jayUyMHNtYWxsJTIwYW5kJTIwd2l0aCUyMGElMjBTQUZFVFklMjBjb21tZW50JTJDJTIwYSUyMG51bGwlMjBwb2ludGVyJTBBJTJGJTJGJTIwJTIwJTIwJTIwJTIwJTIwZnJvbSUyMGdldGVudiUyMGdpdmluZyUyMCU2MGRlZmF1bHQlNjAuJTBBJTJGJTJGJTIwJTIwJTIwMy4lMjBhdG9pKCUyMmVpZ2h0eSUyMiklMjBpcyUyMDAlMkMlMjB0aGUlMjBzYW1lJTIwYXMlMjBhdG9pKCUyMjAlMjIpLiUyME1ha2UlMjBlbnZfaW50JTIwcmV0dXJuJTBBJTJGJTJGJTIwJTIwJTIwJTIwJTIwJTIwJTYwZGVmYXVsdCU2MCUyMGZvciUyMGElMjB2YWx1ZSUyMHRoYXQlMjBpcyUyMG5vdCUyMGFsbCUyMGRpZ2l0cyUyQyUyMHVzaW5nJTIwRmlkJ3MlMjBvd24lMEElMkYlMkYlMjAlMjAlMjAlMjAlMjAlMjBzdHJpbmclMjBmdW5jdGlvbnMlMjBvbiUyMHRoZSUyMEMlMjBzdHJpbmcuJTBBJTBBJTJGJTJGJTIwVE9ETyUyMDElM0ElMjBleHRlcm4lMjAlMjJDJTIyJTIwaW5jbHVkZSUyMCUyMiUzQ3N0ZGxpYi5oJTNFJTIyJTIwJTdCJTIwLi4uJTIwJTdEJTBBJTBBJTJGJTJGJTIwVE9ETyUyMDIlMkMlMjAzJTBBZm4lMjBlbnZfaW50KG5hbWUlM0ElMjBzdHIlMkMlMjBkZWZhdWx0JTNBJTIwaTY0KSUyMC0lM0UlMjBpNjQlMjAlN0IlMEElMjAlMjAlMjAlMjByZXR1cm4lMjBkZWZhdWx0JTNCJTBBJTdEJTBBJTBBZm4lMjBtYWluKCklMjAtJTNFJTIwaTY0JTIwJTdCJTBBJTIwJTIwJTIwJTIwcHJpbnRsbiglMjJwb3J0JTIwJTIyJTIwJTJCJTIwaW50X3RvX3N0cihlbnZfaW50KCUyMkZJRF9MQUJfUE9SVCUyMiUyQyUyMDgwKSkpJTNCJTBBJTIwJTIwJTIwJTIwcHJpbnRsbiglMjJ1bnNldCUyMCUyMiUyMCUyQiUyMGludF90b19zdHIoZW52X2ludCglMjJGSURfTEFCX1VOU0VUJTIyJTJDJTIwODApKSklM0IlMEElMjAlMjAlMjAlMjBwcmludGxuKCUyMmJhZCUyMCUyMiUyMCUyQiUyMGludF90b19zdHIoZW52X2ludCglMjJGSURfTEFCX0JBRCUyMiUyQyUyMDgwKSkpJTNCJTBBJTIwJTIwJTIwJTIwcmV0dXJuJTIwMCUzQiUwQSU3RCUwQQ==) to read it, and
- copy [`starter.fd`](starter.fd). It compiles and prints the default
  three times.

Embed:

```html
<iframe src="https://fides-lang.org/playground/embed.html#JTJGJTJGJTIwTGFiJTIwMTIlMjBzdGFydGVyJTNBJTIwYSUyMHNhZmUlMjB3cmFwcGVyJTIwYXJvdW5kJTIwQyUyMGZ1bmN0aW9ucy4lMEElMkYlMkYhJTIwbGFiJTNBJTIwMTIlMEElMkYlMkYhJTIwY2hlY2slM0ElMjBydW5zJTBBJTJGJTJGISUyMGVudiUzQSUyMEZJRF9MQUJfUE9SVCUzRDgwODAlMEElMkYlMkYhJTIwZW52JTNBJTIwRklEX0xBQl9CQUQlM0RlaWdodHklMEElMkYlMkYlMEElMkYlMkYlMjBUYXNrcyUyMChzZWUlMjBSRUFETUUubWQpJTNBJTBBJTJGJTJGJTIwJTIwJTIwMS4lMjBEZWNsYXJlJTIwJTYwZ2V0ZW52JTYwJTIwYW5kJTIwJTYwYXRvaSU2MCUyMGZyb20lMjAlM0NzdGRsaWIuaCUzRSUyMGluJTIwYW4lMjBleHRlcm4lMjAlMjJDJTIyJTIwYmxvY2suJTBBJTJGJTJGJTIwJTIwJTIwMi4lMjBXcml0ZSUyMGElMjBzYWZlJTIwJTYwZW52X2ludChuYW1lJTNBJTIwc3RyJTJDJTIwZGVmYXVsdCUzQSUyMGk2NCklMjAtJTNFJTIwaTY0JTYwJTNBJTIwZXZlcnklMEElMkYlMkYlMjAlMjAlMjAlMjAlMjAlMjAlNjB1bnNhZmUlNjAlMjBibG9jayUyMHNtYWxsJTIwYW5kJTIwd2l0aCUyMGElMjBTQUZFVFklMjBjb21tZW50JTJDJTIwYSUyMG51bGwlMjBwb2ludGVyJTBBJTJGJTJGJTIwJTIwJTIwJTIwJTIwJTIwZnJvbSUyMGdldGVudiUyMGdpdmluZyUyMCU2MGRlZmF1bHQlNjAuJTBBJTJGJTJGJTIwJTIwJTIwMy4lMjBhdG9pKCUyMmVpZ2h0eSUyMiklMjBpcyUyMDAlMkMlMjB0aGUlMjBzYW1lJTIwYXMlMjBhdG9pKCUyMjAlMjIpLiUyME1ha2UlMjBlbnZfaW50JTIwcmV0dXJuJTBBJTJGJTJGJTIwJTIwJTIwJTIwJTIwJTIwJTYwZGVmYXVsdCU2MCUyMGZvciUyMGElMjB2YWx1ZSUyMHRoYXQlMjBpcyUyMG5vdCUyMGFsbCUyMGRpZ2l0cyUyQyUyMHVzaW5nJTIwRmlkJ3MlMjBvd24lMEElMkYlMkYlMjAlMjAlMjAlMjAlMjAlMjBzdHJpbmclMjBmdW5jdGlvbnMlMjBvbiUyMHRoZSUyMEMlMjBzdHJpbmcuJTBBJTBBJTJGJTJGJTIwVE9ETyUyMDElM0ElMjBleHRlcm4lMjAlMjJDJTIyJTIwaW5jbHVkZSUyMCUyMiUzQ3N0ZGxpYi5oJTNFJTIyJTIwJTdCJTIwLi4uJTIwJTdEJTBBJTBBJTJGJTJGJTIwVE9ETyUyMDIlMkMlMjAzJTBBZm4lMjBlbnZfaW50KG5hbWUlM0ElMjBzdHIlMkMlMjBkZWZhdWx0JTNBJTIwaTY0KSUyMC0lM0UlMjBpNjQlMjAlN0IlMEElMjAlMjAlMjAlMjByZXR1cm4lMjBkZWZhdWx0JTNCJTBBJTdEJTBBJTBBZm4lMjBtYWluKCklMjAtJTNFJTIwaTY0JTIwJTdCJTBBJTIwJTIwJTIwJTIwcHJpbnRsbiglMjJwb3J0JTIwJTIyJTIwJTJCJTIwaW50X3RvX3N0cihlbnZfaW50KCUyMkZJRF9MQUJfUE9SVCUyMiUyQyUyMDgwKSkpJTNCJTBBJTIwJTIwJTIwJTIwcHJpbnRsbiglMjJ1bnNldCUyMCUyMiUyMCUyQiUyMGludF90b19zdHIoZW52X2ludCglMjJGSURfTEFCX1VOU0VUJTIyJTJDJTIwODApKSklM0IlMEElMjAlMjAlMjAlMjBwcmludGxuKCUyMmJhZCUyMCUyMiUyMCUyQiUyMGludF90b19zdHIoZW52X2ludCglMjJGSURfTEFCX0JBRCUyMiUyQyUyMDgwKSkpJTNCJTBBJTIwJTIwJTIwJTIwcmV0dXJuJTIwMCUzQiUwQSU3RCUwQQ=="
        width="100%" height="420" style="border:0" title="Lab 12 starter"></iframe>
```

## Tasks

1. **Declare.** Declare `getenv` and `atoi` from `<stdlib.h>` in an
   `extern "C"` block (`fn getenv as c_getenv(name: str) -> str;`). Call
   `c_atoi("42")` outside `unsafe` and read the error (the answer is
   [`unwrapped_fail.fd`](unwrapped_fail.fd)).
2. **Wrap.** Write `env_int(name: str, default: i64) -> i64`. Every
   `unsafe` block is as small as it can be and has a `// SAFETY:` comment.
   A null pointer from `getenv` (a missing variable) gives `default`:
   test it with `(raw as i64) == 0` (a comparison of a `str` with `0`
   compares text).
3. **Validate.** `atoi("eighty")` is `0`, like `atoi("0")`. Make `env_int`
   return `default` for a value that is not all digits, using Fid's own
   string functions on the C string.

## Check your work

```text
$ CC="zig cc" fid ccompile starter.fd -o lab12
$ FID_LAB_PORT=8080 FID_LAB_BAD=eighty ./lab12
```

prints, when you are done:

```text
port 8080
unset 80
bad 80
```

## Questions to hand in

- `getenv` returns a pointer into the environment. Who owns that memory?
  What would go wrong if `env_int` kept it, or freed it?
- What does your wrapper promise its callers that `getenv` and `atoi` do
  not? Write it as the comment a reviewer would want above `env_int`.

## For instructors

[`solution.fd`](solution.fd) is a model answer and
[`expected.txt`](expected.txt) what it prints with the two variables set;
`python3 tests/course/check_labs.py --fid <fid> lab12` builds it with
`fid ccompile` (`//! run: ccompile`, `CC` or `zig cc`), sets the
variables (`//! env:`) and compares, and checks that
[`unwrapped_fail.fd`](unwrapped_fail.fd) is rejected.
