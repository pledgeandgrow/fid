# Expressions written at compile time: `comptime splice` and `gensym`

Roadmap LG9 (competitive-roadmap, Track LG). `comptime gen { }` adds
declarations; `comptime splice { }` writes an expression in place. Together
with `gensym` they cover what hygienic macros are used for, without a second
language: the code that writes code is ordinary comptime Fid.

## `comptime splice { block }`

An expression. The block runs inside the compiler (the comptime evaluator,
`compiler/comptime_eval.fd`) and returns the expression's source text as a
string; that text, parsed as exactly one expression, takes the splice's
place before type checking. Everything after that (type checking, borrow
checking, lowering) sees only the generated expression.

```fid
comptime fn sum_upto(n: i64) -> str {
    var s: str = "0";
    var i: i64 = 1;
    while i <= n {
        s = s + " + " + int_to_str(i);
        i = i + 1;
    }
    return s;
}

fn main() -> i64 {
    println(comptime splice { return sum_upto(4); });   // 0 + 1 + 2 + 3 + 4
    val n = 3;
    println(comptime splice { return "n * " + int_to_str(2); });   // 6
    comptime splice { return "println(5)"; };          // as a statement
    return 0;
}
```

- Where: the statements and expressions of a function or a method (impl
  blocks, trait impls) and a constant's initializer. A splice inside a
  closure or a trait's default method is not expanded; the type checker
  reports it (`this comptime splice { } was not expanded`).
- The text may hold another splice; it is expanded too, to a depth of 16.
- Errors, each naming the splice's line: the block cannot run at compile time
  (I/O, an unknown function, ...); the block returns something other than a
  string (`test_frontend_comptime_splice_value_fail.fd`); the text is not one
  expression (`test_frontend_comptime_splice_fail.fd`). Unlike
  `comptime <expr>`, a splice never falls back to running when the program
  does: there is nothing to run.
- The generated tokens count lines from the splice's line, so a type error
  inside the generated expression points at the splice.

### The call form: `comptime splice make(args)`

What a macro call in an expression does: `comptime splice` followed by a
call of a `comptime fn` that returns source text is the splice
`comptime splice { return make(args); }`. The arguments are evaluated at
compile time too, and the result is type-checked where it stands (a str
spliced where an i64 is wanted is the ordinary type error,
`test_frontend_comptime_splice_type_fail.fd`).

```fid
comptime fn square_of(e: str) -> str {
    val t = gensym("sq");
    return "{ val " + t + " = " + e + "; " + t + " * " + t + " }";
}

fn main() -> i64 {
    val x = 4;
    println(comptime splice square_of("x + 1"));       // 25
    println(comptime splice square_of("3") + 1);       // 10: the splice is the call only
    return 0;
}
```

`comptime splice(x)` (the word followed by `(`) stays a `comptime` call of
a function named `splice`. Tests: `test_frontend_comptime_splice_call.fd`,
`test_frontend_comptime_splice_call_fail.fd`.

Implementation: `parser.fd` `parse_comptime_expr` makes the call
`__comptime_splice(ComptimeBlock(block), line)` and sets
`parser_saw_splice()`; `comptime_eval.fd` `process_gen` then runs
`ct_expand_splices` (only when a splice was parsed, and only on bodies that
hold comptime nodes), `ct_expand_splice` evaluates the block and parses the
text with `parser.fd` `parse_expression_source`.

## Hygiene: `gensym(prefix)`

`gensym("t")` gives a name no declaration of the program uses and no earlier
`gensym` gave (`t__gs1`, `t__gs2`, ...; `comptime_eval.fd` `ct_gensym`).
Generated code names what it introduces with it, so it cannot capture or
shadow the caller's names:

```fid
comptime fn offset_by_100(e: str) -> str {
    val t = gensym("t");
    return "{ val " + t + " = 100; " + t + " + " + e + " }";
}

fn main() -> i64 {
    val t = 7;
    println(comptime splice { return offset_by_100("t"); });   // 107, not 200
    return 0;
}
```

A name the generated text spells out (`e` above, the caller's `t`) means
that name where the splice stands: that is the point of writing it.

`gensym` is compile-time code's: it is known in `comptime gen` and
`comptime splice` blocks, in `comptime { }` and `comptime e`, and in the body
of a `comptime fn` (the type checker treats that body as compile-time code,
`typechecker.fd`; `test_frontend_gensym_comptime_fn.fd`). An ordinary
function calling it is an error (`test_frontend_gensym_runtime_fail.fd`). A
`comptime fn` the program calls when it runs (arguments not known before)
gets a fresh string from each call, `<prefix>__gsr<n>` (`lower.fd`
`Lowerer_lower_gensym_rt`): nothing is declared at run time, so there is
nothing for it to avoid. A function of the program's own named `gensym` wins
over the builtin everywhere.

## `==` on generated types

A struct or enum that `comptime gen` creates compares by content with `==`,
as a declared one does. `compiler.fd` synthesizes `__eq_<T>` for declared
types (`synthesize_enum_eq` / `synthesize_struct_eq`) before `process_gen`
runs; `process_gen` makes the same helpers for the types it adds
(`comptime_eval.fd` `ct_gen_synth_eq`: field by field for a struct, Vec
fields through `Vec_eq_words` / `Vec_eq_strs`; tag and payload for an enum;
generic types skipped; a `__eq_<T>` the program writes wins). Test:
`test_frontend_gen_eq.fd`.

## Tests

| Test | Checks |
|------|--------|
| `test_frontend_comptime_splice.fd` | splices in calls, statements, an impl method, a constant; nested splice; gensym hygiene; run-time gensym |
| `test_frontend_comptime_splice_call.fd` | `comptime splice make(args)`: compile-time arguments, gensym hygiene, in a method; `comptime splice(x)` still a call |
| `test_frontend_comptime_splice_call_fail.fd` | a call form whose text is not one expression |
| `test_frontend_comptime_splice_type_fail.fd` | a spliced expression is type-checked in place |
| `test_frontend_comptime_splice_fail.fd` | text that is not one expression |
| `test_frontend_comptime_splice_value_fail.fd` | a block that returns no string |
| `test_frontend_gensym_comptime_fn.fd` | gensym in a `comptime fn` used by gen blocks |
| `test_frontend_gensym_runtime_fail.fd` | gensym outside compile-time code |
| `test_frontend_gen_eq.fd` | `==` / `!=` on a generated struct and enum |
| `test_frontend_gensym.fd` | (earlier) gensym in a gen block, generated enums/consts/impls |
