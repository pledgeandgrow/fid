# Bugs found while writing the 200 new stdlib modules (Nov 17)

Found by the agents that wrote the modules; each has a repro. Numbered in the order found; several are the same bug seen from different sides (noted). The repro files named here were in a scratchpad folder that may be cleared; the inline repros are enough to reproduce.

## Status (Nov 17)

Fixed, each with a regression test `tests/bootstrap/test_fix_*.fd`. All 38 pass on a one-stage build from a snapshot of that day, in `fid run` and through C; `fnval_c_ptr` runs through C only.

| Bugs | Fix | Test(s) |
|---|---|---|
| 1 | a free function named like a method (`pt_twice` vs `pt::twice`) is an error, in user programs (the compiler's own Parser_new / math's vec3_sub duplicates keep it off for imported modules) | method_name_clash_fail |
| 2, 14, 18, 22, 29, 46 | calls through a fn value: struct/enum results through a result pointer (CallPtr ABI `(result, env, args)`), struct arguments, Result/Vec results | fnval_struct_ret, fnval_struct_arg, fnval_result, fnval_vec_ret |
| 3 | f64 captures keep their type, and are stored as floats in the env (C converted 3.0 to the integer 3) | fnval_capture_f64 |
| 4, 43 | `Vec::filled` with an f64 | vec_filled_f64 |
| 5, 41 | by design (`*s as f64` is `*(s as f64)`, reference §4.4); the error now has its line | deref_cast, deref_cast_line_fail |
| 6 | inf/NaN print and convert in the interpreter and C | float_inf_nan |
| 7 | by design: the precedence table of reference §4.1 | (doc) |
| 8, 44 | `v.extend(r)` with `r: &Vec` (escape check, and the argument is now loaded like the receiver); `HashMap::contains_key` | extend_ref |
| 9 | a top-level `val` is a clear error (use `const` or `var`) | top_level_val_fail |
| 10 | fn values returning Vec<str>/Vec<Struct> | fnval_vec_ret |
| 13, 26 | `q4`/`q8` and words that start like a type are identifiers; `f32`, `i64`, `str` stay reserved, with an error that says so | type_word_idents, type_word_name_fail |
| 15, 33, 39, 48 | escape check: a fn value's non-reference result is not a reference | escape_fn_value |
| 16 | `p.0` is an error that says to use a struct; a call on a call result (`fs.get(1)(5)`, `h.f(x)` through a local) works | tuple_dot_fail, call_result_call |
| 17 | generic struct with a `Vec<V>` field | generic_vec_field |
| 19 | generic fn taking `& mut T` | generic_ref_mut |
| 23, 36 (keywords) | keywords (`send`, `handle`, ...) as struct field names | struct_lit_keyword_field |
| 25 | globals of struct type get their full size | global_struct |
| 27 | a closure calling a captured fn value; a closure made inside a closure got the same name as its maker and ran it | fnval_capture_call |
| 28 | a closure keeps its own copy of a captured struct; names a match arm or `for` loop binds inside a closure are not captures | fnval_capture_struct |
| 30 | an unannotated local holding a fn value from a struct field | fnval_field |
| 31, 38, 47 | `Vec<fn(..)>` elements called | fnval_vec_of_fn |
| 32 | a user function named `take` | fn_named_take |
| 34 | struct assignment through `&mut` takes an owner of the new value (the old one is left, a leak: dropping it crashed a compiler built with it, in its own comptime pass) | deref_assign_struct |
| 35 | concatenation of strings holding NUL bytes in the interpreter | str_concat_nul |
| 37 | closures read and write globals in place | fnval_capture_global |
| 40 | `HashMap<str, f64>.get` passed to an f64 parameter | map_f64_get |
| 42, 53 | borrow errors (and expression errors) carry their line | borrow_line_fail, borrow_line2_fail, deref_line_fail, expr_error_line_fail |
| 45, 50 | calling a C function address (`addr as fn(..)`), C builds | fnval_c_ptr |
| 20, 51, 52 (part) | the interpreter reuses freed blocks and frees a frame's allocas at return | interp_mem_reuse |

Open:
- 11 and 24: the import-order struct-literal parse error. Not rechecked against its repro; the keyword-field fix may cover 24.
- 12: interpreter speed of core Vec ops.
- 20, 51, 52 in full: RSA-2048 and P-256 are still too slow or too big for `fid run`. The agents' tests run them through C.
- 21: a tool issue, not the compiler.
- 36: type aliases nested inside `Vec<..>` or struct fields.
- 49: no repro.
- 54: redeclaring a name with another type.
- 55: the inconsistent part of 42.
- 56: not reproduced alone.

Found while checking the fixes (fixed):
- C runtime: the OS-mapped arena chunks (RT7) put the first block on a page boundary. String checks read `s - 16`, so any compiled program printing an int crashed. Blocks now start 16 bytes into the mapping (codegen_c.fd `__p_arena_new_chunk`). A compiler built in one stage from a stage0 that has RT7 but not this fix still crashes in `ccompile`.
- C backend: a function named `v` + digits (`fn v4`) clashed with the generator's value names. It is now renamed `p_v4_user` (`sanitize_c_name`).
- C backend: an `if` or `match` whose value is a struct (`val m: Wav = if c { f() } else { g() }`) read the struct's first word as its address and crashed (stdlib wav). The value is now the result slot's address (lower.fd `Lowerer_lower_if_expr`, `Lowerer_lower_match_expr`; test_fix_if_struct_value).
- C backend: a fn value returning nothing, called through an unannotated local of a nested field of a `&mut` struct (`val adv = s.game.advance`), aborted with exit 127 (stdlib netcode). The call's type had lost its `-> void`. Void functions are now candidates at a word-returning call site (codegen_c.fd CallPtr; test_fix_fnval_void_field).

Native backends: calls through fn values on nat_ir/pe_ir still use the old env-first order, and iropt's LG5 devirtualization needs the same reordering.


From robotics math (a3eeca37):
1. A free function named like an impl method is silently merged with it:
   struct pt { x: i64 } impl pt { pub fn twice(self: pt) -> i64 { return self.x * 2; } }
   fn pt_twice(p: pt) -> i64 { return p.x * 3; }   // pt_twice(pt{x:5}) prints 10, not 15
2. Calling a fn value whose return type is a struct crashes ("Invalid memory access"; args shift by one):
   struct P { a: i64, b: i64 } fn first(x: i64, y: i64) -> P { ... } fn call(f: fn(i64, i64) -> P) -> P { return f(1, 2); }
3. A returned closure capturing an f64 parameter reads its bits as garbage:
   fn mk(k: f64) -> fn(f64) -> f64 { return fn(x: f64) -> f64 { return x * k; }; }  mk(3.0)(2.0) -> 9227875636482146304
4. Vec::filled(3, 0.5).get(2) returns 0 for f64 in the interpreter.
5. (*s as f64) parses as *(s as f64); the error has no line number.

From IoT protocols (abdb9be6):
6. float_to_str(1.0 / 0.0) never returns in the interpreter; println(0.0 / 0.0) prints "-nan(ind)".
7. `&` and `>>` bind more loosely than `==`/`!=` (`(x & 255) >> 5 != m` misparses) — check against reference.md's precedence table.
8. v.extend(r) with r: &Vec<i64> is rejected ("a reference cannot be passed to method 'extend'").
9. A top-level `val NAME: str = ...` does not parse (const works).

From OS storage (a43c1fd5; repros in scratchpad/os):
10. Calling a fn value that returns Vec<str> or Vec<Struct> gives garbage elements (f(1).get(0) prints 5000, not "abc"); Vec<i64>, str, i64 work.
11. A struct literal of another module's type inside a module fails to parse depending on import order ("expected { but got 'Colon'" when the program imports `use M; use vfs;` but not `use vfs; use M;`). Repro bug3.fd / bug3b.fd.
12. (speed) Core Vec ops run interpreted in `fid run`: Vec::filled(512, 0) ~17 ms, a CoW copy of 512 elements ~25 ms.

From bare metal (a94945e4):
13. The identifiers `q4` and `q8` lex as the type token I8 ("expected identifier but got 'I8'"): `val q4: i64 = 1;`. q1-q3, q5, q7, q15, i4 work.

From game audio and AI (a553c08a):
14. A struct passed by value through a function-typed parameter crashes ("Invalid memory access"); a struct returned through one reads as zeros; the C build of the same program exits 127 with no output (so bug 2 is not interpreter-only). Repro:
    struct S { v: i64, w: i64 } fn bump(s: S) -> S { return S { v: s.v + 1, w: s.w }; }
    fn run(s: S, f: fn(S) -> S) -> i64 { val n: S = f(s); return n.v; }  run(S{v:1,w:2}, bump)
15. Escape check false positive: fn runr(s: &S, f: fn(&S) -> i64) -> i64 { return f(s); } then int_to_str(runr(&s, g)) is rejected ("a reference cannot be passed to parameter 'n' of 'int_to_str'").
16. Tuple field access `p.0` does not parse; calling a struct field holding a fn value as h.f(x) does not work (copy to a local first). Maybe by design.

LICENSE NOTE (resolved Nov 17): stdlib/mp3.fd's Huffman tables and synthesis window are now credited to ISO/IEC 11172-3 Annex B (Tables 3-B.7 and 3-B.3), with a prefix-code completeness check and a filterbank round trip in the test; no Symphonia (MPL-2.0) provenance remains.
From security tools and web data (ad887b14):
17. A generic struct with a Vec<V> field reads back garbage: struct Slot<V> { vals: Vec<V> }; var a: Slot<i64> = Slot { vals: Vec::new() }; a.vals.push(5); a.vals.get(0) prints ~5928.
18. (same family as 2/14) fn(Vec<i64>) -> R through a function value panics "Integer overflow on addition ... in Vec_len".
19. A generic function with a cap bound taking `& mut T` fails at run time: fn use_store<T: Store>(s: & mut T) -> "Unknown function 'use_store__T'".

From crypto foundations (a24a7993; repros scratchpad/cyber/mem_*.fd):
20. The interpreter leaks ~1 KB on every Vec.set / Vec.push (never freed, and slow): 45,000 writes -> 149 MB; long numeric work reaches an 8-17 GB allocation and stops with OOM (argon2, ecdsa, x509, rsa tests pass through C but OOM in `fid run`). bigint_modpow with a 768-bit exponent reached 7 GB. Interpreter loop speed ~17,000 iterations/s.
21. Stopping a background shell does not stop the fid.exe it started (orphaned processes).

From IoT devices (aaf5b970):
22. (same family as 2/10/14/18) A fn value returning Result breaks in the interpreter: args shift, Vec_len panics "Integer overflow on addition". fn cntr(ms: Vec<i64>) -> Result<i64, str>; val g = cntr; g(d).
23. `send` is a keyword, so it cannot be a struct field name, and the parse error doesn't say so.
24. Unconfirmed: a struct literal with function-value fields gave "expected { but got Colon" until the fields were reordered (likely the same as bug 11).

From OS kernel and network (a2b2bc69; repro scratchpad/kdev/bug_global_struct.fd):
25. A global of struct type gets one storage slot: writing the next global overwrites its second field.
    struct U { lo: i64, hi: i64 } var g_u: U = U { lo: 1, hi: 100 }; var g_n: i64 = 0; g_n = 7; g_u.hi -> 7 (expected 100).
26. `f32` cannot be a variable name (parsed as the type keyword), like q4/q8 in bug 13.

From web networking (aa910752):
27. A closure calling a captured function value fails: "Unknown function 'nx'" in the interpreter; ccompile emits a call to the undeclared name nx(v1).
    fn w(next: fn(str)->str) -> fn(str)->str { val nx = next; return fn(s: str) -> str { return nx(s); }; }
28. A closure capturing a struct holding a Vec reads the Vec dangling ("Vec index out of bounds" / "Invalid memory access").
29. (fn-value family) A function taking a struct by value called through a fn value gets garbage, even with str/i64 results (q.names.get(0) prints "5008").
30. An unannotated local holding a fn value from a struct field loses its string results: val f = c.h; f("q") prints nothing (typed local works).
31. Vec of bare fn values: v.get(i) then f(a,b) panics "CallPtr: function pointer is not a string or pointer".
32. A user function named `take` corrupts the struct passed to it ("Invalid free" when the caller's struct is dropped); renaming fixes it (name clash with a core function?).
33. The escape check treats a fn value's str/bool result as a reference when a reference argument was passed (same family as 15).
34. `*early = r;` (struct assignment through &mut) then copying it in the caller -> "Double free".
35. String concatenation truncates at the first NUL byte in the interpreter: str_from_bytes([65,0,66]) + "y" has length 2.
36. `handle` cannot be a field name (keyword, undocumented); type aliases do not nest inside Vec or struct fields (Vec<L> with L = fn(H) -> H rejected).

From robot devices (ac788845; repros scratchpad/probe):
37. Closures capture globals by value at creation: val f = fn(x) { return g; }; g = 2; f(0) gives 1 (a named fn gives 2); a closure doing h = h + 1 twice leaves h at 1.
38. (same as 31) Vec<fn(i64)->i64>: v.get(0) then f(1) panics "CallPtr: function pointer is not a string or pointer"; fs.get(1)(5) does not parse.
39. (same as 33) Escape check: an i64 result of a fn value called with a &mut argument is treated as a reference.

LICENSE NOTE: stdlib/apriltag.fd embeds the 587 tag36h11 codes from AprilTag's tag36h11.c (BSD licence; the notice is kept in the module header).

From energy models (a817e9b0):
40. HashMap<str, f64>.get passed straight to an f64 parameter arrives as the raw word: float_to_str(h.get("Q")) prints ""; comparing with an f64 panics "Arithmetic error (int, float)"; a typed local works.
41. (same as 5) `*seed as f64` parses as *(seed as f64); E079 has no line number.
42. The borrow error for f(&mut x, g(&mut x)) reports "line 0".

From game engine (af9aa39e):
43. (refines 4) Vec::filled(n, x) with any non-zero f64 stores a denormal near 0 in the interpreter (0.0 works).
44. HashMap has no contains_key (it is `contains`): a Rust-habit trap, maybe add an alias.
45. Fides cannot call a raw C function pointer (d3d12's COM vtable calls need a 6-line C shim, fides_com_call).

From mobile (a1c89abf; repros scratchpad/mine):
46. (same as 2/14) struct returned through a fn value: bug_structret.fd prints 0 instead of 7; s3.fd crashes.
47. (same as 31) bug_vecfn.fd.
48. (same as 33) bug_escape.fd.
49. "Double free" in Vec_drop assigning out.ids = e.ids inside a fn-value seam (out: &mut NdkInput, e copied from a global Vec<NdkInput>), no minimal repro yet (bug_dfree.fd does not trigger it); maybe related to 25 (global structs) or 34.
50. Fides cannot call a multi-argument C function pointer (jni needs entry/call fn values; class_addMethod refuses a fn variable) — same as 45.

From crypto protocols (ae0910f7):
51. P-256 is unusable in the interpreter: ec_public_key takes minutes, ecdsa_sign dies with "p_malloc: OOM allocating 4294967304 bytes" (same as 20). C builds are correct and fast.
52. Interpreter OOM after a few RSA-1024 operations ("OOM allocating 2147483656 bytes"), same as 20.
53. (same as 42) borrow error "line 0" for pump(& mut c, & mut s, ssh_rekey(& mut c), Vec::new()).

From the completion agents (ext4/mongodb/mysql, exi/iso15118/opcua):
54. Redeclaring a name in the same scope with a different type is accepted, and later uses keep the first type; the error has no line:
    struct S { f: i64 } fn main() { var x: Vec<i64> = Vec::new(); var x: S = S { f: 2 }; x.f = 3; }   // "cannot assign field 'f' on type Vec<i64>"
55. (same as 42/53) `f(&mut b, g(&b))` is rejected at "line 0" in some functions and accepted in others of the same shape.
56. `fid ccompile` sometimes reports "C compilation failed" and succeeds on a plain rerun (likely memory pressure from parallel builds; not reproduced alone).

From the stdlib repair pass (queue/stack/set generics attempt; repros build/t_gen*.fd):
57. Generic functions and impl-block associated functions corrupt heap-typed
    generic parameters: a `Vec<T>` or `T` (str) parameter moved into a
    struct field or pushed into a `Vec<T>` inside a generic fn arrives as a
    pointer/empty word (`Stack::from_vec` stored an empty Vec; `bs.data.get`
    printed 4112/4136 instead of the str). `impl <T> S { fn f(v: T) }`
    methods called on a *concrete* `S<str>` work; `self.data.pop()` inside
    a generic method on `S<str>` returns the raw word. Net effect:
    `Queue<T>`/`Stack<T>`/`Vec<T>` wrappers are effectively i64-only until
    generic param lowering/mono ownership for heap types is fixed.
58. Field assignment through `&mut self` on a generic struct is rejected:
    `self.head = self.head + 1` inside `impl <T> Q` gives "cannot assign
    field 'head' on type &mut Q<T>" (the two-stack queue design avoids
    scalar fields).
59. Return-position generic constructors cannot bind T: `Box2::new()` with
    no T source produces `Box2<T>` (unresolved) rather than defaulting to
    the annotation; `Type::<T>::name()` and `Type::name::<T>()` are not
    supported turbofish positions (free-fn `name::<T>(args)` is the only
    one).

From the stdlib capability pass (parallel/arena/collections; repros build/t_par*.fd):
60. RESOLVED (Nov 20): spawn args are whole deep copies — `Lowerer_spawn_call`
    (compiler/lower.fd) inserts `p_send_copy` (stdlib/core.fd: str, Vec with
    sized or recursive elements, HashMap with keys and values), and a
    generated wrapper frees the copies when the task body ends. Pinned by
    tests/bootstrap/test_spawn_vec_copy.fd, test_spawn_mutate_both.fd and
    test_spawn_deep_copy.fd (parent and task each mutate their own value;
    `Vec<i64>` verified on interpreter and C; `Vec<str>`/HashMap need a fid
    built by itself — a stage-1 binary's 1 MB task stacks overflow in the
    recursive drop, a messaged panic, not corruption). The original report:
    a typed `Vec<i64>` passed through `spawn` arrived *shared*; workers
    copy-on-wrote invisibly to the caller.
61. impl-method module attribution bug: with `use collections` (impl
    HashSet) and `use set` (HashSet_* free functions) together, the
    collections' impl-HashSet method bodies are checked under module
    'set' — private fields and private helpers then fail with "private to
    module 'collections'". Workaround: keep the field and the hs_* helper
    fns `pub`.
62. `var v = Vec::new(); return v as i64;` escapes a STACK header (LF13):
    the word is usable for reads but Vec_drop/p_free on it panics
    "Double free". For raw handles use heap blocks: arena's region
    allocator uses an intrusive malloc-linked list ({next,size} node
    before each payload), no Vec at all.

From the media/method-dispatch pass (fid-new2 rebuild):
63. Method resolution for `v.name(...)` on `Vec<T>` is gated by THREE
    hard-coded lists in the typechecker, all of which must name the
    method: `TypeChecker_vec_lib_method_ok` (~5993), the safety/builtin
    table (~9076), the generic-Vec dispatch fallthrough (~21622) — plus
    matching tables for the Named-Vec and auto-deref paths (~21899,
    ~22095) and the readonly table in lower.fd (~9438). Missing any one
    makes `v.m()` unreachable while `vec_m(v)` still works. All vec.fd
    impl methods (find_opt, position_opt, take, skip, zip, enumerate,
    flat_map, chunk, window, dedup, sort_by, contains_val) were wired
    through all sites; fid-new2 is the first compiler with the full set.
63b. RESOLVED: a method `self: & mut Vec<i64>` (dedup, sort_by) resolved
    but silently did nothing: the call site passed the loaded Vec word
    while the impl forwards `self` to a `& mut` free function expecting
    a slot address. `Lowerer_lower_method_call` now checks
    `Lowerer_mut_self_impl` and lowers place receivers (Ident,
    FieldAccess, Deref — side-effect free) as `Expr::Ref(recv, true)`.
    Verified interpreter + C backend (dedup/sort_by mutate in place,
    field receivers too). `get_vec().dedup()` on a temporary still
    passes the word (harmless: mutating a discarded temp).
64. `extern "C" ... link "SDL2"` in a stdlib module makes EVERY C build
    that imports the module link `-lSDL2`, even though the compiler
    synthesizes missing externs as returning-0 stubs. image.fd's loader/
    texture externs now use `link ""`: `use image` no longer drags SDL2
    into zig cc links.
65. Global `var FB = image_create(320, 240, 4)` eagerly pushes ~300K
    elements at import time — interpreter startup hangs for minutes.
    Module-level resources must be lazily allocated (game.fd's
    game_ensure_fb()).
