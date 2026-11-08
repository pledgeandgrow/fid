# Miscompile and robustness log (VF19)

Every bug where the compiler, the runtime or the stdlib does something other
than what the reference says, found by the verification tools: what it is,
its cause where known, how it escaped the suite, and the test that now
catches it. A bug stays here when it is fixed; its status changes.

Repros are in `tests/verify/bugs/` (each file carries its expected result as
`//>` / `//!` lines). Run them all with

    python scripts/verify/expect.py --fid release/fid.exe tests/verify/bugs

and move a repro into `tests/bootstrap/` when its bug is fixed (VF20).

Compiler for the Nov 7 entries: the self-hosted `fid` built from the tree of
Nov 7 ~20:30 (the parent session's `fid-opA`/`fid-opB2`), on Windows x64.

## Wrong code (miscompiles)

| ID | Backends | What | Cause | How it escaped | Caught by | Status |
|----|----------|------|-------|----------------|-----------|--------|
| M1 | run; emit (compare, call argument, println) | `u8 as i8`, `u16 as i16`, `u32 as i32`: the value keeps the zero extension of the unsigned type (`200u8 as i8` prints 200, compares ≥ 0, is passed as 200) | `compiler/lower.fd` `Lowerer_lower_cast` lowers a same-size int cast to `BitCast`; the interpreter and `pe_ir` treat `BitCast` as the identity, the C backend emits a real C cast | no test casts between signedness at one width; VF1's generator had no casts | `tests/verify/bugs/m1_same_width_cast.fd`; `gen_vs_c.py --avoid none` | open |
| M3 | all | `println` of a u64 ≥ 2^63 prints it signed; `print` is right | `println` in `stdlib/core.fd` | no test prints a u64 above i64::MAX with `println` | `m3_println_u64.fd`, `tests/conformance/s3_5_unsigned_println.fd` | open |
| M4 | ccompile | `for k in 0..v.len() + 1 { v[k] }` reads past the end (prints 45, exit 0) instead of panicking; emit panics | the counted-loop check removal on the C path (`compiler/checks.fd`, shared with codegen_c) accepts a bound it cannot prove | the suite has loops to `v.len()` only; nothing tests a bound past the end | `m4_c_range_check_dropped_fail.fd` (VF5 test of `PEIR_RANGES`) | open |
| M5 | run (wrong values), ccompile (C does not compile) | `var arr: [i64; 8] = [1, …, 8];` — the interpreter reads 4663, 0, 0, …; the C generator writes `int64_t[8] v = 0;` | interpreter array-literal initialisation; C local array declaration | arrays in the suite are not initialised from a literal in a local `var` of array type | `m5_fixed_array_literal.fd` | open |
| M7 | emit | a narrowing cast to a signed type is not sign-extended: `59134u16 as i8` is 254 (prints 254, compares as 254) | the native backend's `IntTrunc` to a signed type zero-extends | as M1 | `m7_emit_narrow_signed_cast.fd`; gen_vs_c seed 137 (reduced) | open |
| M6 | emit | a fixed-array index past the end is not checked: `a[3]` of `[i64; 3]` reads 0, exit 0 (§3.2, §8.2) | — | array tests index in range only | `m6_emit_array_index_unchecked.fd`, `tests/conformance/s8_2_array_oob_panics.fd` | open |
| E1 | emit | `fn main() -> i64` ending without `return` after a loop exits with a leftover register value (144); run and ccompile exit 0 | no return value is set for main's fall-through in the native backend | every test `main` ends in `return 0` or returns `void` | `e1_emit_main_without_return.fd` | open |
| V1 | run, emit | spurious `Integer overflow on addition/subtraction` panics, and missed panics, in random programs that C and ccompile run correctly (seeds 100, 103, 107, 113, 120, 136, 137, 140, 154 of `gen_vs_c.py --seed 100`; 12 of 60 programs) | seed 137 reduces to M7 (emit); the others are being reduced (`scripts/fuzz/reduce.py`), likely M1/M7 through casts | — | `gen_vs_c.py` | open, unreduced |

## Wrong rejections (the compiler refuses a valid program)

| ID | What | Caught by | Status |
|----|------|-----------|--------|
| M2 | `if c { if d { 1 } else { 2 } } else { 3 }` used as a value is typed `void` (§4.7: "has the type of its branches"); parentheses around the inner `if` work | `m2_nested_if_value.fd`, `tests/conformance/s4_7_nested_if_value.fd` | open |
| N1 | a program function with the name of an imported module's public function (`handle_request`, defined by `stdlib/http.fd`) replaces it inside that module, which is then rejected | `n1_program_fn_replaces_module_fn.fd` | open |
| N2 | two imported modules defining one function (`file_append` in file.fd and fs.fd): an unqualified call is reported as "undefined function" instead of ambiguous | `n2_two_modules_same_fn.fd` | open |
| K1 | a function named with a word keyword (§1.4 allows it) cannot be called inside an expression when the keyword starts a statement or an item: `(s + handle(2))` and `println(layer(5))` are parse errors | `tests/conformance/s1_4_word_keyword_call.fd`, `s1_4_word_keyword_as_name.fd` | open |
| S1 | `stdlib/os.fd` `os_args` (`p_str_take(p_argv(i))`) does not type-check with the Nov 7 compiler, so every program importing `os` (also through `http_client`, `fs`, `dns`) is rejected | `tests/verify/chaos.fd` before its workaround | open (tree in flux) |

## The reference and the compiler disagree

| ID | What | Status |
|----|------|--------|
| D1 | reference §8.1 says `var b = a; b.push(2)` changes `a` (Vec is a handle; "Copy-on-write ... switched off"); with proposal 0004 (accepted Nov 7) `a.len()` is 1 on every backend. One of them must change | open (`tests/conformance/s8_1_vec_is_handle.fd` follows the reference) |

## Crashes of the compiler

| ID | What | Caught by | Status |
|----|------|-----------|--------|
| F1 | the parser recurses once per nesting level with no limit: about 3000 nested `(`, `[`, `{`, `-` or `!` kill `fid` with a stack overflow (0xC00000FD) | `scripts/fuzz/fuzz_frontend.py` (VF3), `f1_parser_deep_nesting.fd` | open |

## Stdlib parsers on untrusted input (VF15)

| ID | What | Status |
|----|------|--------|
| P1 | `url_parse` of a port longer than an i64 panics (integer overflow in `String_parse_int`) | open, `p1_url_port_overflow.fd` |
| P2 | `regex_match` with `a{3999…9}` panics (integer overflow in `rx_parse_brace`) | open, `p2_regex_brace_overflow.fd` |
| P3 | `xml_parse("<")` allocates until the process runs out of memory; `xml_parse("<a><b><<")` runs for more than 20 s (likely the same loop on a stray `<`) | open, `p3_xml_lt_oom.fd` |

## Runtime under failure (VF16, VF17)

| ID | What | Status |
|----|------|--------|
| L1 | a request-handler loop leaks about 3.9 KB per request on the C path (`tests/verify/soak_service.fd`: 2.4 GB after 390 000 requests, out of memory after ~1 M in 15 s). `fid run --leaks` over 2000 requests: `String_slice`, string concatenation, `json_string`, `int_to_str`, `json_unescape`, and the Vecs made inside `http_parse_head` and `json_get_*` | open (proposal 0004 work in progress) |
| C1 | the HTTP client transport (`stdlib/http_client.fd` `hc_read_all`) has no timeout: a server that accepts and never answers hangs the client forever | open |
| C2 | with `fid emit`, `read_file` of a missing file (or a directory) panics with "Invalid memory access" instead of giving `None` | open, `c2_emit_read_file_missing.fd` |
| C3 | with `fid emit`, unbounded recursion reports "Invalid memory access (null or dangling pointer)" instead of "Stack overflow" (the exit is clean) | open, `c3_emit_stack_overflow_message.fd` |
| C4 | on Linux (C path), `file_append` returns 0 (success) when the write fails with EFBIG (`ulimit -f`, as on a full disk): 200 chunks "written", the file holds 1 MiB | open |
| C5 | out of memory on Linux (`ulimit -v`): `p_malloc: OOM allocating N bytes`, then SIGABRT and a crash banner (exit 134), not a panic and exit 1 | open |

## Reproducibility (VF12)

| ID | What | Status |
|----|------|--------|
| R1 | `fid ccompile` executables differ between two builds of the same source: the PE timestamp (offset 0x80) and the debug directory (PDB GUID and path). `SOURCE_DATE_EPOCH=0` fixes the timestamp with `zig cc`; the PDB path follows the output path (build without debug info, or `/pdbaltpath`). `dump-c` and `emit` output are byte-identical | open |

## Earlier miscompiles (from AGENTS.md and docs/reference/known-bugs.md)

Fixed before this log; listed so the log is the one place to look. None of
these records names the test that now catches the bug, which is what VF20
asks for: adding one per entry is open.

| Was | What | Cause | Status |
|-----|------|-------|--------|
| AGENTS 4 | `Vec` handle passed as `&mut Vec` read as a header: garbage lengths, crashes | `cast_arg` without the `*(void**)` deref when the pointee equals the parameter type | fixed |
| AGENTS 9 / known-bugs 1.6 | struct fields after an enum field read at the wrong offset | PIR L2 `GetFieldPtr` used `index * 8` | fixed |
| AGENTS 10 / 1.7 | native ELF tests printed wrong values | one wrong REX prefix byte (`0x49` for `0x4C`) in `p_json_at` | fixed |
| AGENTS 12 | zero-valued struct fields printed garbage in optimized native builds | CSE's `replace_refs_in_inst` ignored `L2Inst::Copy` after SROA | fixed |
| AGENTS 13 | the first stack slot overlapped the stack canary | PIR frames started at offset 0 | fixed |
| AGENTS 21 | natively built `fid` freed `s - 16` of a plain string | the string-header magic alone was trusted | fixed |
| known-bugs 1.11 | enum `==` compared pointers | no tag/payload comparison | fixed (`synthesize_enum_eq`) |
| known-bugs 1.18 | `print` of any value called `p_print_int` | `Inst::Print` ignored the type | fixed |
| known-bugs 1.19 | statements silently dropped in long functions | — | fixed |
| known-bugs 1.30 | float constants lost their bit pattern in C | — | fixed |
