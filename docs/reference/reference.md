# Fides Language Reference

This reference describes the language that the self-hosted compiler `fid`
accepts. The sources are `compiler/*.fd`. The Rust compiler `fidesc`
(`src/*.rs`) is frozen and no longer defines the language. Where the two
differ, see [§14](#14-differences-from-fidesc).

Each rule names the place in the compiler that implements it, as
`file:function`. Examples come from `tests/bootstrap/` or were checked against
the code. "Not implemented" means the parser accepts the construct and no later
pass gives it a meaning.

Contents:

1. [Lexical structure](#1-lexical-structure)
2. [Items](#2-items)
3. [Types](#3-types)
4. [Expressions and operators](#4-expressions-and-operators)
5. [Statements](#5-statements)
6. [Patterns and match](#6-patterns-and-match)
7. [Generics](#7-generics)
8. [Memory and ownership](#8-memory-and-ownership)
9. [Effects, capabilities and sandbox](#9-effects-capabilities-and-sandbox)
10. [Concurrency](#10-concurrency)
11. [Annotations](#11-annotations)
12. [Conditional compilation](#12-conditional-compilation)
13. [The prelude](#13-the-prelude)
14. [Differences from fidesc](#14-differences-from-fidesc)
15. [Docs that disagree with the compiler](#15-docs-that-disagree-with-the-compiler)

---

## 1. Lexical structure

Source: `compiler/lexer.fd` (`Lexer.tokenize`, `keyword_or_ident`,
`read_operator`, `read_number`, `read_string`).

### 1.1 Source text and whitespace

A source file is UTF-8. Space, tab, newline, carriage return, form feed and
vertical tab separate tokens. Non-ASCII text may appear in comments and in
string and character literals only; anywhere else it is a lex error
(`unexpected character`).

### 1.2 Comments

- `//` starts a comment that runs to the end of the line. `///` and `//!` are
  ordinary line comments to the compiler.
- `/*` starts a comment that ends at the first `*/`. Block comments do not nest.

### 1.3 Identifiers

An identifier is an ASCII letter or `_` followed by ASCII letters, digits and
`_`. The single character `_` is an identifier token; the parser gives it a
meaning in patterns (wildcard) and types (inferred type).

### 1.4 Keywords

These words are reserved (`keyword_or_ident_a` … `keyword_or_ident_type`):

```
fn val var mut const given ensure cap type return
if else match select struct enum impl pub use module
with async await comptime defer spawn yield move static try in
as dyn where region sandbox unsafe txn persist for while
loop break continue handle do interrupt extern guard is resume true false
layer model grad backward kernel component system query state on
i8 i16 i32 i64 i128 u8 u16 u32 u64 u128 f32 f64 bool str void
```

- Only the exact word is a keyword: `i8x`, `u16v`, `f64s` are identifiers.
- `q8` and `q4` are other spellings of `i8` in a type (`parse_type`), not
  reserved words: they may also name a variable (`val q4: i64 = 1;`;
  stdlib-agent-bugs 13, `test_fix_type_word_idents.fd`). The type names
  above are reserved: `val f32 = 1;` is an error that says so
  (`test_fix_type_word_name_fail.fd`).
- `move` and `where` are reserved but no grammar rule uses them.
- `kernel` is used only as the annotation `@kernel`.
- The word keywords (all but the type names) may also be used as the name of a
  function, method, parameter, field or local variable (`expect_ident_or_keyword`,
  `token.fd:is_word_keyword`). A keyword that starts a statement (`query`,
  `select`, `region`, `do`, …; `parse_stmt`) still does so there, and one that
  starts an item does so at the top level (`state`, `system`, `on`, …). A
  keyword followed by an assignment operator or `.` at the start of a
  statement is a variable (`handle.fd = 3;`, `state += 1;`), and `handle`
  starts a handler only as `handle {`.

Contextual words have a meaning only in one position: `parallel` (statement
start), `scope` (`spawn scope`, `cancel scope`), `gen` (`comptime gen`), `case`
and `default` (in `select`), `gpu`, `kv_cache`, `budget`, `window` (in
`region`), `include` and `link` (in `extern`), `asm` followed by `!`, `in`, `out`
and `clobber` (in `asm!`), and `self` as a parameter name.

### 1.5 Integer literals

- Decimal: `123`, `1_000_000`. Underscores are allowed between digits.
- Hexadecimal `0xFF`, binary `0b1010`, octal `0o17` (prefix letter in either
  case). These take no underscores: `0xFF_FF` lexes as `0xFF` followed by the
  identifier `_FF`.
- There are no type suffixes: `42i64` is the literal `42` followed by the
  keyword `i64`, a parse error.
- A decimal literal above `i64::MAX` keeps its 64 bits (`lex_parse_int` wraps),
  so `18446744073709551615` is valid for a `u64` (`test_int_width.fd`).

A minus sign is never part of a literal; `-1` is negation applied to `1`.

### 1.6 Floating-point literals

A float literal is digits, `.`, at least one digit, and an optional exponent
`e`/`E` with an optional sign: `3.14`, `1.5e-3`, `1_000.25`. Digits followed
by an exponent and no fraction (`1e5`, `5e3`, `1E-9`) are also a float
(known-bugs 2g.18; only when digits follow the `e`). `1.` and `.5` are not
floats.

### 1.7 Character literals

`'c'` is an integer literal whose value is the Unicode scalar value of the
character (`read_char_lit`): `val c: i64 = 'A';` gives 65. Escapes: `\n` `\t`
`\r` `\\` `\'` `\"` `\0` `\xHH` `\u{H…}` (up to 6 hex digits).

### 1.8 String literals

- `"…"`: escapes `\n` `\t` `\r` `\\` `\"` `\0` `\xHH` `\u{H…}`. Any other escape
  (including `\'`) is a lex error. `\0`, `\x00` and `\u{0}` are a zero byte
  of the string, and the rest of the literal is kept: a string carries its
  length, so `str_len("ab\0cd")` is 5. `p_file_write` and `p_file_append`
  write the string's full length, zero bytes included, on every backend
  (BG81), as `p_file_write_bin` and the byte functions do.
- Raw strings `r"…"` and `r#"…"#` (any number of `#`) take their content
  literally; the closing delimiter is `"` followed by the same number of `#`.
- Format strings `f"…{expr}…"`: text outside braces takes the same escapes as a
  plain string. Each `{…}` (braces may nest) is lexed and parsed again as an
  expression (`parser.fd:parse_fstring_parts`). The result has type `str`.
  There is no escape for a literal brace. Each expression is type-checked; it
  may be text, a number, a bool or a `Vec` — a struct (or a reference to one)
  is an error, since it has no text form
  (`TypeChecker_check_fstring_part`; `test_frontend_fstring.fd`,
  `test_frontend_fstring_struct_fail.fd`). After the expression a `:` at
  brace depth introduces a format specifier — `[fill][<|>|^][#][0][+]
  [width][.precision][x|X|b|o]` — so `f"{x:.2}"` prints `1.50` for `x = 1.5`,
  `f"{n:5}"` pads `7` to `"    7"`, and `f"{x:#x}"` prints `0xff`
  (`lower.fd:Lowerer_lower_fstr_fmt`, core's `fstr_fmt_*`). `:` inside
  brackets or a `::` path stays part of the expression.
- There are no byte-string literals (`b"…"`).

### 1.9 Operators and punctuation

```
+  +=  -  -=  ->  *  *=  /  /=  %  %=
=  ==  =>  !  !=  <  <=  <<  <<=  >  >=  >>  >>=
&  &&  &=  |  ||  |=  |>  ^  ^=  ~  ~>
.  ..  ..=  ...  ,  :  ::  ;  ?  ??  ?.
(  )  {  }  [  ]  #  @
```

When a generic argument list is closed, a `>>`, `>>=` or `>=` token is split so
that its first `>` closes the list (`parser.fd:split_gt`): `Vec<Vec<i64>>`
parses.

### 1.10 Semicolons

The parser consumes a `;` after a statement or item if there is one; it does not
require it. This program is valid:

```fides
fn main() -> i64 {
    val x = 1
    print(x)
    return 0
}
```

---

## 2. Items

Source: `parser.fd:parse_top_level_item`. A file is a sequence of items, each
optionally preceded by `pub`. Items can be in any order. A struct literal
`Name { … }` parses when the parser already knows `Name` as a struct (§4.3),
or, whatever it knows, when `Name` is capitalised and `{` is followed by a
field name (an identifier or a word keyword) and `:`, or by `..`
(`pd_field_init_follows`): `Dev { handle: h }` is a literal even when `Dev`
is declared further down or in a module that is parsed later. An empty
literal `Name {}` is recognised only for a struct the parser already knows.
Structs and enums of `core` and of imported modules are known before
the file is parsed.

### 2.1 Functions

```
function   = ["pub"] ["async" | "comptime" | "interrupt"] "fn" name
             [generic-params] "(" [params] ")" ["->" type] [effects]
             {("given" | "ensure") expr [","]} block
params     = param {"," param} [","]
param      = {annotation} ["..."] name [":" type ["=" expr]]
effects    = "!" name | "!" "{" [name {"," name}] "}"
```

- A missing return type means `void`. `-> !` means the function never returns
  (`Type::Never`). `-> _` asks the type checker to infer the return type from
  the `return` statements (`typechecker.fd:TypeChecker_check_func_body`).
- A function declared to return a value (not `void`, `!` or `_`) must not reach
  the end of its body without returning, except `main`
  (`TypeChecker_check_func_body`, `stmts_always_leave`;
  `test_missing_return_break_fail.fd`).
- A trailing expression statement, or a trailing `if`/`else` or `match`, is the
  function's value (`lower.fd`, function body lowering; `test_if_expr_value.fd`):

  ```fides
  fn max(a: i64, b: i64) -> i64 {
      if a > b { a } else { b }
  }
  ```

- `self` with no type is a parameter of type `Self`; it is also written
  `self: T`, `self: &T` or `self: &mut T`. `&self` is not valid syntax.
- Default values: `fn add(a: i64, b: i64 = 10)`. A call may leave out trailing
  parameters that have defaults (`TypeChecker_register_program` counts the
  required ones); lowering supplies the default.
- Named arguments: `add(b: 2, a: 1)`. Lowering puts them in parameter order:
  positional arguments fill the first parameters, a named one the parameter
  of its name, one left out takes its default. In `fid` a call with named
  arguments is checked (`TypeChecker_check_named_args`): a name that is no
  parameter, a parameter given twice, a positional argument after a named
  one, a parameter without a default left out, and a named argument of the
  wrong type are errors (`test_frontend_named_args.fd` and three `_fail`
  tests). Named arguments to methods are not checked.
- `...name: T` (variadic, last parameter). With a concrete type (`...ns: i64`)
  the extra arguments are packed into a `Vec<i64>`; with a type parameter
  that types nothing else it is a pack, each argument with its own type,
  read only as `xs.len()`, `xs.get(K)` with a literal `K`, `for x in xs` or
  passed on whole (`mono.fd:mono_desugar_variadics`, before type checking;
  [variadic-packs.md](variadic-packs.md), `test_variadic_args.fd`,
  `test_variadic_pack.fd`).
- `given e` is a precondition. A call whose arguments are constants and make
  `e` false is a compile error (`TypeChecker_verify_contracts`,
  `test_contracts_fail.fd`); there is no run-time check. `ensure e` is parsed and
  not checked anywhere: not implemented.
- `async fn` is covered in §10, `comptime fn` in §4.11, `interrupt fn` in §11
  (`@interrupt`).
- Functions cannot be nested. A `fn` inside a block is a closure expression
  (§4.10).

### 2.2 Structs

```
struct = "struct" Name ["<" T {"," T} ">"] "{" {["pub"] field ":" type ["=" expr] [","]} "}"
```

- A field without `pub` is private to the module that declares the struct:
  reading it from another module is an error (`TypeChecker_check_expr_inner`,
  `Expr::FieldAccess`; `test_field_privacy_fail.fd`).
- There are no tuple structs (`struct C(u8, u8)`) and no unit structs
  (`struct E;`).
- A struct literal names every field it sets: `Point { x: 1, y: 2 }`. A field
  left out is not reported. `..base` copies the remaining fields from another
  value of the same struct (`parse_struct_lit_expr`, `test_spread.fd`). There
  is no shorthand `Point { x, y }`.
- A field may declare a default: `port: i64 = 8080`. A literal ending in a
  bare `..` (`Config { port: 80, .. }`) gives every field it leaves out its
  default; a field left out that has none is an error
  (`TypeChecker_check_struct_defaults`). The default expression is written in
  at each such literal during monomorphization (`mn_fill_struct_defaults`),
  so it is evaluated once per literal; it is type-checked against the field.
  Not in `fidesc`; not in a global's or constant's initializer
  (`test_frontend_struct_defaults.fd`, `_fail.fd`).
- Struct values are copied on assignment and when passed by value (§8.1).
- `==` and `!=` on two structs compare field by field (`test_struct_eq.fd`).

### 2.3 Enums

```
enum    = "enum" Name ["<" T {"," T} ">"] "{" {variant [","]} "}"
variant = Name ["(" [ [name ":"] type {"," [name ":"] type} ] ")"]
```

- A variant has no payload or a parenthesised payload list; a payload may be
  named (`V(x: i64)`). There are no struct-like variants (`V { x: i64 }`) and no
  explicit discriminants (`A = 1`).
- A variant is constructed with its enum's name: `Option::Some(5)`,
  `Shape::Empty`. A bare `Some(5)` is a call of an undefined function `Some`.
- `==` on enum values compares the tag and the payload
  (`compiler.fd:synthesize_enum_eq`, for enums without type parameters).
- A recursive enum holds itself through `Box`: `Node(Box<Tree>)`
  (`test_box_owned_fail.fd`).

### 2.4 impl blocks

```
impl = "impl" ["<" T {"," T} ">"] Name ["<" … ">"] "{" {["pub"] function} "}"
```

- Methods are called as `value.method(args)`. A function of the block whose
  first parameter is not `self` is called as `Name::function(args)`.
- A type may have several `impl` blocks.
- `impl<T> Slot<T> { … }`: the type parameters after `impl` are in scope in the
  methods. The argument list after the type name is skipped by the parser
  (`parse_impl_or_trait_impl`).
- Annotations may be written before a method of an `impl` block (CU3:
  `@deprecated(use = "m2") fn m`); in a trait impl `@x fn` is a parse error.
- A method whose name is `drop` is called for some locals at function exit
  (§8.3).

### 2.5 Traits (`cap`)

Traits are declared with `cap`:

```
trait  = "cap" Name [":" Super {"+" Super}] "{" {assoc | method} "}"
assoc  = "type" Name ["<" T {"," T} ">"] [";"]
method = ["pub"] "fn" name ["<" … ">"] "(" params ")" ["->" type] (block | [";"])
impl-trait = "impl" ["<" T … ">"] Trait "for" Type "{" {("type" Name "=" type ";") | ["pub"] function} "}"
```

From `test_generics_full.fd`:

```fides
cap Show {
    fn show(self) -> str;
}

impl Show for Pt {
    pub fn show(self: Pt) -> str { return "pt" + int_to_str(self.x); }
}
```

- A method with a body is a default; an impl that leaves it out gets it
  (`test_trait_default.fd`).
- `cap Sub: A + B` gives `Sub` the methods of `A` and `B`; an impl of `Sub`
  must provide them (`compiler.fd` merges supertrait methods before checking).
- An impl must provide every method without a default and every associated
  type, and the trait must exist (`TypeChecker_verify_trait_impls`;
  `test_trait_verify_fail.fd`). In `fid`, an impl method's parameter count,
  parameter types and return type are compared with the trait's wherever the
  trait writes a concrete type (`TypeChecker_check_impl_sig`;
  `test_trait_sig_fail.fd`).
- A trait method may be generic (`fn m<U: Show>(self, x: U) -> U`). A call
  binds the method's type parameters from its arguments, on a value of a
  concrete type or through a bound `T: Trait`, and is monomorphized per type;
  bounds on them are checked at the call and usable in the body. An impl's
  method must declare the same number of type parameters (it may name them
  differently) and no bound the trait lacks; a default body that an impl
  does not override keeps its type parameters
  (`test_trait_generic_method.fd`, `test_trait_generic_method_generic_impl.fd`,
  `test_trait_generic_default_method.fd`, `test_trait_generic_method_arity_fail.fd`,
  `test_trait_generic_method_bound_fail.fd`,
  `test_trait_generic_method_impl_bound_fail.fd`). A generic method cannot be
  called through `dyn Trait` (it is not object-safe); the trait's other
  methods still can (`test_trait_generic_method_dyn_fail.fd`).
- `impl<T> Show for Slot<T>` implements a trait for a generic struct
  (`test_trait_generic_impl.fd`).
- `dyn Trait` is a trait-object type; a value is made with `v as dyn Trait`
  and its methods, defaults included, are dispatched at run time
  (`test_trait_object.fd`, `test_frontend_dyn_default.fd`). The cast needs
  the type to implement the trait (`test_dyn_not_impl_fail.fd`). In `fid` a
  struct, enum, string, bool or number given where a `dyn T` is wanted, or a
  `dyn T` where one of those is wanted, is a type error: the object exists only
  where the cast made it (`TypeChecker_dyn_incompatible`;
  `test_frontend_dyn_coerce_fail.fd`, `test_frontend_dyn_unwrap_fail.fd`).
  Another `dyn` type and a type parameter stay compatible.

### 2.6 Constants

```
const = "const" NAME ":" type "=" expr [";"]
```

The initialiser is stored and substituted at each use
(`lower.fd:Lowerer_lower_program`, `Lowerer_insert_constant`). Assigning to a
constant is an error (`test_const_assign_fail.fd`).

### 2.7 Global variables

```
global = ("var" | "static") name ":" type ["=" expr] [";"]
```

`static` is another spelling of `var`. There is no top-level `val`: it is a
parse error that suggests `const` (a fixed value) or `var`
(`test_fix_top_level_val_fail.fd`). Without an initialiser the global holds
the zero value of its type (`0`, `0.0`, `""`). An initialiser that is not a
constant runs before `main` (`Lowerer_lower_program`, `global_inits`;
`test_global_init.fd`). A global cannot carry an annotation (the parser accepts
annotations only before `struct` and `fn`). Writing a global from code that runs
on another thread is checked (§10.6).

### 2.8 Type aliases

```
alias = "type" Name "=" type [";"]
```

An alias to a struct can be used in a struct literal (`parse_type_alias`).

### 2.9 Modules and `use`

```
module = "module" seg {"::" seg} [";"]
use    = "use" seg {"::" seg} ["::" "*"] ["as" name] ["{" ("*" | name {"," name}) "}"]
         ["with" "{" [Cap {"," Cap}] "}"] [";"]
```

- `module a::b;` names the file's module. That name is the unit of
  visibility: an item without `pub` used from another module is an error
  (`TypeChecker_check_visibility`). The program file's module is `main`.
- `use a::b;` loads the module from, in order: `<dir of the file>/a/b.fd`,
  `<dir>/a/b/mod.fd`, the standard library directory, and the project root
  (the first ancestor directory holding `stdlib/core.fd` or `Cargo.toml`)
  (`compiler.fd`, import resolution). `core` is always loaded and needs no
  `use`.
- All public items of the module become visible. The `{ a, b }` list and `*`
  are recorded and do not limit what is imported.
- `use m as j;` or `use a::m;` makes `j::f()` / `m::f()` call the function `f`
  of that module, which decides between two modules that define the same name
  (`test_qualified_names.fd`).
- `with { IO, FS }` grants capabilities (§9.2).
- A version pin `use m = "1.0";` is a parse error (`parse_use` returns an error
  after reading the string).
- A `use` statement inside a function body is parsed and does nothing.

### 2.10 extern "C"

```
extern = "extern" ["\"" abi "\""] {"include" string | "link" string}
         ( "{" {"fn" decl} "}" | "fn" decl )
decl   = c_name ["as" fides_name] "(" params ")" ["->" type] [";"]
```

From `test_ffi.fd`:

```fides
extern "C" include "<math.h>" link "m" {
    fn sqrt as pledge_sqrt(x: f64) -> f64;
}
```

- `include` adds a C header, `link` a library. `as` gives the function a
  different Fides name.
- In the program's own module, calling an extern function whose name does not
  start with `p_` needs `unsafe` (`TypeChecker_check_expr_inner`, `Expr::Call`).
- Only named functions can be passed to a C function expecting a callback
  (`TypeChecker_check_c_callbacks`; `test_c_callback_closure_fail.fd`). A
  closure is refused, its captures named, with the fix: the callback's state
  goes through the C function's user data pointer, `&mut state as *u8` in
  the call (the struct's address; parsed `&mut (state as *u8)`), and the
  callback turns it back with `ctx as *State` (`test_c_callback_user_data.fd`,
  `test_c_callback_capture_fail.fd`).

### 2.11 Domain declarations

These declarations are rewritten into ordinary items before type checking
(`compiler.fd:desugar_layers_models`, `desugar_phase8b`, `desugar_query_stmts`):

| Declaration | Becomes |
|---|---|
| `layer L(args) { fields; fn forward(…) }` | a struct, an impl and a constructor. A layer must have `forward`. |
| `model M { name: Layer(args), …; fn forward(…) }` | a struct and an impl. A model must have `forward`. |
| `component C { fields }` | a struct |
| `system S(params) { body }` | a function |
| `state S { A { on e => B, … } … }` | an enum with one variant per state, `S_initial()` and `S_handle(state, event)` |
| `on event(params) { body }` | a function `__on_event` |
| `query (A, B) { body }` (statement) | a loop over entities that have components `A` and `B` |

### 2.12 `#[…]` attributes

`#[name(arg, …)]` at the top level is parsed into a list of attributes that is
not attached to the next item, and `fid` reads none of them. `@` annotations
(§11) are the form the compiler uses. `fid test file.fd` runs the functions
whose names start with `test_` and `bench_`, not `#[test]` functions
(`main.fd:cmd_test_file`).

---

## 3. Types

Source: `parser.fd:parse_type`, `ast.fd:Type`, `typechecker.fd`.

```
type = prim | Name | Name "<" type {"," type} ">" | Name "::" Assoc ["<" … ">"]
     | "&" ["mut"] type | "*" ["mut" | "const"] type
     | "[" type ";" (int | NAME) "]" | "(" [type {"," type}] ")"
     | "fn" "(" [type {"," type}] ")" "->" type | "dyn" Trait | "_" | "!"
prim = i8 | i16 | i32 | i64 | i128 | u8 | u16 | u32 | u64 | u128
     | f32 | f64 | bool | str | void
```

### 3.1 Primitive types

| Type | Values |
|---|---|
| `i8` `i16` `i32` `i64` `i128` | signed integers |
| `u8` `u16` `u32` `u64` `u128` | unsigned integers |
| `f32` `f64` | IEEE floats |
| `bool` | `true`, `false` |
| `str` | a pointer to NUL-terminated bytes (immutable text) |
| `void` | no value |
| `!` | never returns |

There is no `char` (a character literal is an integer), `isize`, `usize` or
slice type.

### 3.2 Compound types

- `[T; N]`: a fixed-size array; `N` is an integer literal or, in a generic
  function, a const parameter name. Indexing is bounds-checked at run time
  (`Panic: Array index out of bounds`).
- `(T, U, …)`: a tuple. `(T)` is `T`; `()` is the empty tuple.
- `fn(T, U) -> R`: a function value (named function or closure). The `->` is
  required.
- `&T`, `&mut T`: references (§8.4). `*mut T`, `*const T`: raw pointers (§8.5).
- `Vec<T>`, `HashMap<K, V>`, `Option<T>`, `Result<T, E>`, `Box<T>`, `Rc<T>`,
  `Arc<T>`, `Future<T>`: library and runtime types (§13).
- `Name<A, B>`: an instance of a generic struct or enum.
- `T::Item`: an associated type. The type checker treats it as compatible with
  every type.
- `_`: a type to infer.
- `Tensor<f64, [2, 3]>` is a shaped tensor type; dimensions are integers or
  names.

### 3.3 Type compatibility

`TypeChecker_types_compatible` decides whether a value of one type may be used
where another is declared. It accepts:

- equal types (after aliases are resolved);
- `!` and `_` on either side; `dyn T` and associated types on either side; a
  type parameter of the current function on either side;
- `Name` against `Name<…>` (a generic used without arguments), and any two
  instances of `Vec`, `HashMap`, `Rc` or `Arc` of which at least one names no
  type arguments (`Vec`, or `HashMap<str, i64>`, the type of `HashMap::new()`
  where no declared type says otherwise). Two instances that name different
  type arguments are not compatible (`Vec<str>` is not a `Vec<i64>`;
  `tc_collection_instances_compatible`. Until Nov 23 any two were, and a
  `Vec<str>` passed as a `Vec<i64>` had its strings read as numbers);
- integer to any integer (whether the value fits is a separate check, §3.4),
  and integer to float;
- tuples element by element;
- raw pointers with the same mutability and compatible pointees, `&T` to
  `*T`, a raw pointer to an integer, and an integer to a raw pointer only inside
  `unsafe` (`test_int_to_ptr_safe_fail.fd`).

It rejects a float where an integer is expected
(`safety/test_float_to_int_fail.fd`), and `str` against an integer
(`test_str_int_fail.fd`); only the runtime layer and the compiler's own modules
may treat `str` as an integer.

### 3.4 Integer literals and narrowing

- A literal takes the integer type the context expects (`i8` … `u64`). Without
  one it takes the smallest of `i8`, `i16`, `i32`, `i64` that holds it
  (`infer_int_type`). An unannotated `val`/`var` initialised from an expression
  of only literals is an `i64`: `var i = 0;` is an `i64`.
- A literal given to an integer type must fit it (`TypeChecker_check_value_fits`):

  ```fides
  var x: u8 = 300;            // error: the literal does not fit in u8.
  ```

  A negative literal fits only a signed type.
- In the program's own module, a value of a wider integer type, or a float,
  given to a narrower integer variable is an error; `as` converts:

  ```fides
  var big: i64 = 5000000000;
  var small: i32 = big;       // error: a i64 may not fit in i32.
  var c: i32 = big as i32;
  ```

  An expression with a literal operand (`x & 0xFF`) is exempt. Imported modules
  are not checked.
- The check applies to `val`/`var` declarations and to `x = e`. A plain
  assignment `x = e` (to a local, a parameter or a global) also checks that
  `e` has the variable's type, as a declaration does
  (`TypeChecker_check_assign_type`): assigning a `bool` to an `i64` variable,
  or a number to a `str` one, is `cannot assign … to variable …` (until Nov
  23 nothing but a literal's fit was checked).

### 3.5 Integer arithmetic at run time

`+`, `-`, `*` and negation panic on overflow at the declared width, unless the
function is `@wrapping`, where they wrap. `/` and `%` by zero always panic.
Unsigned types are unsigned for division, remainder, comparison, `>>` and
printing (`test_int_width.fd`; `safety/test_overflow_add.fd`,
`test_overflow_sub.fd`, `test_overflow_mul.fd`, `test_div_zero.fd`).

---

## 4. Expressions and operators

Source: `parser.fd:parse_expr` down to `parse_primary`; types from
`typechecker.fd:TypeChecker_check_expr_inner`.

### 4.1 Precedence

From lowest to highest. Binary operators of one level associate to the left.
The table follows the parser's call chain, which is the only expression parser
in use (`parse_expr_iterative` is not called).

| Level | Operators | Parser function |
|---|---|---|
| 1 | `\|>` | `parse_pipe_forward` |
| 2 | `??` | `parse_coalesce` |
| 3 | `..` `..=` (one, not chained) | `parse_range` |
| 4 | `\|\|` | `parse_or` |
| 5 | `\|` | `parse_bit_or` |
| 6 | `^` | `parse_bit_xor` |
| 7 | `&` | `parse_bit_and` |
| 8 | `<<` `>>` | `parse_shift` |
| 9 | `&&` | `parse_and` |
| 10 | `==` `!=` | `parse_equality` |
| 11 | `<` `>` `<=` `>=`, `is` | `parse_comparison` |
| 12 | `+` `-` | `parse_addition` |
| 13 | `*` `/` `%` | `parse_multiplication` |
| 14 | prefix `-` `!` `~` `&` `&mut` `*` `comptime` | `parse_unary` |
| 15 | postfix `.f` `.m(…)` `?.f` `[i]` `?` `as T` `~> T` | `parse_postfix` |

Consequences:

- `&&` binds tighter than `|`, `^`, `&`, `<<` and `>>`. `flags & 1 != 0`
  parses as `flags & (1 != 0)`; write `(flags & 1) != 0`. Likewise
  `(x & 255) >> 5 != m` is `(x & 255) >> (5 != m)`, a type error; write
  `((x & 255) >> 5) != m`. This differs from Rust, where `&` and `>>` bind
  tighter than `==`; it is the language's rule (stdlib-agent-bugs 7,
  `test_fix_deref_cast.fd`), so parenthesise a masked or shifted operand.
- `+` binds tighter than `<<`: `1 << 2 + 1` is `8`.
- `as` is a postfix operator, so it binds tighter than any prefix or binary
  operator: `-x as i32` is `-(x as i32)`, `a + b as i64` is `a + (b as i64)`,
  and `*s as f64` is `*(s as f64)` (an error, "cannot dereference
  non-reference type f64", which says to write `(*s) as f64`;
  stdlib-agent-bugs 5/41, `test_fix_deref_cast_line_fail.fd`).
- A call's result can be called on the same line: `pick(1)(5)`,
  `fs.get(1)(5)`, `(fn(x: i64) -> i64 { return x * x; })(7)`. The parser
  reads `e(args)` as `{ val __pcall_k = e; __pcall_k(args) }`
  (`parse_postfix`; `test_fix_call_result_call.fd`). A `(` on the next line
  starts a new statement.
- `await e`, `spawn e` and `yield e` take a prefix-level operand:
  `await a + await b` is `(await a) + (await b)`.
- Assignment is a statement (§5.3), not an operator.

### 4.2 Operator typing

- Arithmetic `+ - * / %` and bitwise `& | ^ << >>` on two integers give the
  wider type; with a float operand they give `f64` (or `f32` when neither
  operand is `f64`).
- `+` on two `str` values concatenates. `+` on a typed string (`SQLString`,
  `ShellString`, `HTMLString`, `JSONString`, `PathString`) is an error.
- `== != < > <= >=` give `bool`. `==` on two `str` values compares text. A `str`
  compared with a number is an error (`safety/test_str_int_compare_fail.fd`);
  to test a runtime `str` for null write `(s as i64) == 0`.
- `&&` and `||` require `bool` operands and short-circuit.
- `!` requires `bool`; `~` requires an integer; `-` requires a number.
- `if` and `while` conditions must be `bool`, `i32` or `i64`. An integer literal
  alone is an `i8` and is rejected (`if 1 { }` is an error).
- On `vec2`/`vec3`/`vec4` values, `+`/`-` work between vectors of one size,
  `*` and `/` with a float, and swizzles (`.xy`, `.rgb`) give a vector of the
  swizzle's length.

### 4.3 Primary expressions

- Literals (§1); `true`, `false`.
- A name: a local, parameter, constant, global or function (a function name is
  a value of `fn` type, `test_expr_values.fd`).
- A call `f(args)`; a named argument is `name: expr`.
- `Name::<T, U>(args)`: a call with explicit type arguments (§7.2).
- `Name::member` and `Name::member(args)`: an enum variant if `Name` is an enum;
  otherwise `m::f(args)` for an imported module `m` (§2.9); otherwise an
  associated function of a struct (`Vec::new()`, `Point::origin()`).
- `Name { field: expr, …, ..base }`: a struct literal, when `Name` is a known
  struct (§2.2).
- `[a, b, c]`: an array literal. The parser fixes its element type to `i64`, so
  an array literal of floats or strings is a type error
  (`array element type mismatch, expected i64, got f64`).
- `(e)` and the tuple `(a, b, …)`.
- `{ stmts; expr }`: a block; its value is its last expression statement.
- `if`, `match`, closures, `try { }`, `await`, `spawn`, `yield`, `comptime`:
  below.
- `asm!("template", in("reg") expr, out("reg") expr, clobber("reg"))`: inline
  assembly. The value is `i64` with an `out` operand, otherwise `void`. The C
  backend emits it as a comment; the native Windows backend encodes it. In the
  program's own module it needs an `unsafe { }` block or an `@unsafe` function
  (§8.5).

### 4.4 Field access, methods, indexing

- `e.f` reads a field; through a reference it reads the referent's field.
  A tuple has no `.0`: `p.0` is a parse error that points to destructuring
  (`val (a, b) = p;`, §5.1; `test_fix_tuple_dot_fail.fd`).
- `e.m(args)` calls a method. `Vec`, `HashMap`, `Rc`, `Arc` and the core
  struct `String` have built-in methods (§13); other types use their `impl`
  blocks. `str` has no methods: use the `str_*` functions (§13.4).
- `e[i]` indexes an array or a `Vec`; the index must be a number.

### 4.5 `?`, `??`, `?.` and `is`

- `e?` requires `Option<T>` or `Result<T, E>` and has type `T`; on `None` or
  `Err` it returns from the enclosing function with that value.
- `e ?? d` requires `Option<T>`; it is the payload of `Some`, or `d`.
- `e?.f` reads field `f` of the struct inside an `Option`, or gives `0` for
  `None` (`test_optional_chain.fd`).
- `e is Name` is a `bool`: an enum-variant test at run time (`x is Some`), or a
  type test (`x is i64`) (`test_is.fd`).

### 4.6 Casts

`e as T` has type `T`. It converts between numeric types, between integers
and raw pointers, between `str` and `i64`, and from a concrete type to
`dyn Trait`. Every other pair is accepted too, except a cast that makes a
reference (`128 as &i64`, `p as &T`), a `str` (`5 as str`) or a struct, an
enum, a collection, a tuple or an array (`7 as Vec<i64>`) out of a word that
is not one: outside `unsafe` each is `Safety error: casting … to … needs an
`unsafe` block` (`TypeChecker_check_cast`, `TypeChecker_check_str_cast`;
until Nov 23 any pair was accepted). Making a raw pointer from an integer
with a cast (`n as *mut i64`) stays allowed in safe code: a raw pointer
cannot be read or written without `unsafe`. `e ~> T` is a quantisation cast
(`QuantCast`).

Numeric casts (decided Nov 17, docs/verification/semantics-2.md §5, §10,
§20.1):

- integer to integer: the low bits are kept, read in the target type
  (`200u8 as i8` is −56; semantics.md §3);
- integer or `f64` to `f32` / `f64`: rounded to the nearest value of the
  target, ties to even; `f32` to `f64` is exact;
- float to integer: truncated toward zero; a value outside the target's
  range gives the nearest end of the range (`1.0e300 as i64` is
  `9223372036854775807`, `-5.5 as u8` is 0, `300.7 as u8` is 255), +∞ the
  maximum, −∞ the minimum, NaN gives 0. A cast never panics. The
  compiler does not do this yet out of range and for NaN
  (`tests/verify/bugs/d21_float_to_int_saturates.fd`, miscompile-log D21).

### 4.7 if and match as expressions

`if c { a } else { b }` has the type of its branches; a branch that diverges
takes the other's type. Without `else` the type is `void`. A `match` used as a
value has the type of its first arm that does not diverge
(`test_if_expr_value.fd`, `test_expr_values.fd`).

### 4.8 Pipe forward

`x |> f(a)` is `f(x, a)`; `x |> f` is `f(x)`; `x |> o.m(a)` is `x.m(a)`
(`Expr::PipeForward`).

### 4.9 try blocks

`try { stmts; expr }` has type `Result<T, i64>`, where `T` is the type of the
last expression (`Expr::TryBlock`; `test_pe_try_block.fd`).

### 4.10 Closures

```
closure = "fn" "(" params ")" ["->" type] (block | "=>" expr)
```

From `test_expr_values.fd`:

```fides
fn make_adder(n: i64) -> fn(i64) -> i64 {
    return fn(x: i64) => x + n;
}
```

- Without `-> type` the return type is inferred from the body and its
  `return` statements.
- A closure captures the variables it names by value when it is created; the
  values are copied into an environment (`lower.fd:Lowerer_make_fn_env`), on
  the heap, or in the frame when the closure is only called there
  (`lw_stack_envs`). To capture by reference, capture a `&` or `&mut`.
- A closure that captures a linear variable is an error
  (`test_linear_closure_fail.fd`); one that captures a reference cannot escape
  (§8.4).

### 4.11 Compile-time evaluation

- `comptime e` and `comptime { stmts }` are evaluated while compiling
  (`compiler/comptime_eval.fd`).
- A `comptime` that cannot be evaluated is a compile error; it does not
  silently run when the program does (`s4_11_fatal_not_runtime.fd`).
  `FID_CT_FALLBACK=1` restores the old warn-and-run-at-runtime behavior
  (docs/verification/kill-switches.md).
- Comptime arithmetic is checked: overflow, a shift amount outside 0..63, a
  left shift that drops bits and division/modulo corner overflow are errors
  with the operands in the message, never silently wrapped values
  (`s4_11_arith_overflow_rejected.fd`, `s4_11_shift_amount_rejected.fd`).
- A comptime value landing in a narrower integer declaration is checked
  while compiling, and the error names the value: `val x: i8 = comptime
  (200 + 100)` is "the comptime value 300 does not fit in i8"
  (`s4_11_comptime_value_fit_rejected.fd`) — the type checker folds what
  comptime computes (consts resolve) and range-checks it as the literal it
  will become.

- A comptime error names the statement it failed on and the chain of
  comptime calls that produced it (`s4_11_error_trace_names_fn.fd`).
- Evaluation has a fuel budget (100 million steps by default, `FID_CT_FUEL`
  to change): a comptime that would run away is an error, never a hang of
  the compiler (`s4_11_fuel_budget_error.fd`).
- A const is comptime-known: `comptime` resolves a const's name to its
  initializer's value (`s4_11_const_computes.fd`).
- `comptime fn` marks a function for compile-time calls; a call with literal
  arguments is evaluated at compile time.
- `comptime T: type` declares a comptime type parameter of a function: a
  type parameter the caller fills by inference or `f::<T>(..)`, as any type
  parameter, whose reflection the body computes per instantiation
  (`s4_11_comptime_type_param.fd`). A generic function's comptime nodes are
  evaluated once monomorphization has filled its parameters.
- A statement `comptime if cond { .. } else { .. }` selects its branch while
  compiling: the branch not taken is not type checked, and its locals are as
  visible as any other statement's (`s4_11_comptime_if_stmt.fd`). Inside a
  generic function the selection waits for the instantiation, and both
  branches are checked as the runtime code they are
  (`s4_11_comptime_if_generic.fd`).
- `inline for v in a..b { .. }` unrolls while compiling: one copy of the
  body per iteration with the loop variable a comptime-known literal, so
  comptime expressions over it compute (`s4_11_inline_for.fd`). The range
  must be known at compile time (`s4_11_inline_for_unknown_bound_fail.fd`);
  at most 4096 iterations unroll, and `break`/`continue` are errors in the
  body (there is no loop to leave).
- A struct field carries `@annotations` as a parameter does (`@secret token:
  str`), and annotation reflection reads them: `comptime
  struct_annotations("S")` and `comptime field_annotations("S", "f")` give
  the names as a comptime Vec<str>, so compile-time code can act on the
  safety annotations a declaration carries (`s4_11_field_annotations.fd`).
  The string-named `field_names_of("S")` and `field_count_of("S")` complete
  the set a comptime fn composes on inside its body — a `comptime T: type`
  function hands the name over with `comptime type_name(T)`
  (`s4_11_secret_field_comptime.fd`).
- `comptime embed("path")` reads a file of the project while compiling, its
  bytes a comptime Vec<i64> — the one file read comptime code may do. The
  path is relative to the project root and never leaves it (`..` and
  absolute paths are errors) (`s4_11_embed.fd`,
  `s4_11_embed_escape_rejected.fd`).
- The build's target as comptime values: `target_os()`, `target_arch()`
  (str), `build_debug()` and `feature_enabled("x")` (bool) give what @cfg
  reads — the target a `--target` named, the host otherwise
  (`s4_11_target_query.fd`). A statement `comptime if` over them gates
  code inside a function body; `@cfg` (§12) is the whole-declaration
  sugar.
- `compile_error("msg")` in comptime code stops the build with the message
  (`s4_11_compile_error.fd`); `comptime_assert(cond, "msg")` checks while
  compiling — false stops the build, true leaves nothing in the program
  (`s4_11_comptime_assert.fd`). Both are comptime-only: calling one outside
  comptime code is an error.
- `comptime gen { … }` (statement) generates code (`test_comptime_gen.fd`):
  the block's value, a string, is parsed and its functions, structs, enums,
  constants, impl blocks and trait impls are added to the program. A
  generated name already declared is an error
  (`test_comptime_gen_clash_fail.fd`, `test_frontend_gen_enum_clash_fail.fd`),
  and so is a generated trait, global, type alias, extern block or `use`.
  `gensym("helper")` in the block gives a name nothing in the program
  declares (`helper__gs1`, …), for helpers that must not collide with the
  program's names (`test_frontend_gensym.fd`); it is also known in a
  `comptime fn` (`test_frontend_gensym_comptime_fn.fd`); `==` compares a
  generated struct or enum by content (`test_frontend_gen_eq.fd`).
- `comptime splice { ... }` / `comptime splice make(args)` (expression): the
  block's value, or the comptime fn call's, is a string that is parsed as
  one expression in the splice's place before type checking
  (docs/reference/comptime-splice.md, `test_frontend_comptime_splice.fd`,
  `test_frontend_comptime_splice_call.fd`).
- Reflection: `comptime type_name(T)`, `is_struct(T)`, `is_enum(T)`,
  `field_count(T)`, `field_names(T)`, `field_types(T)`, `enum_variants(E)`,
  `struct_size(T)`, `field_offset(T.f)`, `enum_discriminant(E.V)`,
  `function_effects(f)`, `function_capabilities(m)` (`parse_comptime_expr`;
  `test_comptime_reflect.fd`).

---

## 5. Statements

Source: `parser.fd:parse_stmt`, `typechecker.fd:TypeChecker_check_stmt_inner`.

### 5.1 Declarations

```
let      = ("val" | "var") name [":" type] "=" expr
         | "var" name ":" type                         // not yet assigned
         | ("val" | "var") "(" name {"," name} ")" "=" expr
persist  = "persist" ("val" | "var") name [":" type] "=" expr
```

- `val` binds a name that cannot be assigned again
  (`test_val_assign_fail.fd`). It does not freeze the value: methods that
  change a `Vec` may still be called on a `val`.
- `var` binds a name that can be assigned. `var x: T;` declares it without a
  value; reading it before an assignment is an error (`Init error`,
  `test_init_guarantee_fail.fd`, `test_init_guarantee_cond_fail.fd`).
- A declaration may shadow an earlier one, also in the same block, with another
  type.
- `val (a, b) = t;` destructures a tuple; the counts must match.
- Annotations may precede a `val`/`var` (`@zeroize`, `@secret`, `@sensitive`);
  annotations before any other statement are a parse error.
- `persist` keeps a variable's value across hot reloads; in a plain run it is a
  `val`/`var` (the C backend loads the initial value and saves nothing).

### 5.2 Expression statements

Any expression followed by an optional `;`.

### 5.3 Assignment

```
assign = place op expr
place  = name | name "[" expr "]" | expr "." field | "*" expr
op     = "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^=" | "<<=" | ">>="
```

- An index assignment needs a variable on the left: `v[i] = x` is valid,
  `v[i][j] = x` and `s.items[i] = x` are parse errors
  (`index assignment target must be an array variable`).
- `p.x.y = v` assigns a nested field.
- `*p = v` writes through a reference, or through a raw pointer inside
  `unsafe`.
- A parameter of scalar type may be assigned (`test_param_mut.fd`). A by-value
  parameter of type `Vec`, `HashMap` or a struct may not be changed — by
  assignment to it or its fields, or by a method that changes it — because the
  caller would not see the change (`TypeChecker_check_param_change`):

  ```fides
  fn f(v: Vec<i64>) { v.push(1); }   // error: 'v' is a by-value parameter of 'f'
  ```

  Take `&mut Vec<i64>` instead. `var v = v;` silences the check and gives a
  copy of its own (proposal 0004, §8.1): changing the copy leaves the
  caller's value as it was (`tests/conformance/s5_3_copy_then_change.fd`).

### 5.4 Control flow

```
if     = "if" expr block ["else" (if | block)]
while  = "while" expr block
for    = "for" name "in" expr block
loop   = "loop" block
guard  = "guard" expr "=>" (block | statement)
```

- `for x in a..b` and `for x in a..=b` iterate a range; `for x in v` iterates a
  `Vec` (elements), an array (elements) or a `HashMap` (keys). The loop
  variable is a single name; `for (i, x) in …` is a parse error.
- Changing the `Vec` a `for` loop walks is an error
  (`test_iter_invalidation_fail.fd`).
- `loop` is a statement, not an expression. `break expr` parses; the value is
  evaluated and discarded.
- `break` and `continue` apply to the innermost loop; there are no labels.
- `guard c => s` is `if !c { s }`.
- `return [expr]`.

### 5.5 defer

`defer { stmts }` (braces are required). The deferred blocks of a function run
when it returns, last registered first (`lower.fd:Lowerer_emit_defers`;
`test_defer.fd`). They run at function exit, not block exit. A deferred block
runs only if its `defer` statement executed, and once however many times the
statement ran (in a loop, also in the copies a loop unrolled at compile time
makes of it): each statement has a flag set when it runs. A `return` that
executes before the `defer` statement (ahead of it in the same block) does
not run it. (Until Nov 23 the blocks were attached by position: one in a
branch that did not execute ran too, and one in a loop ran once per
unrolled copy.)

### 5.6 Blocks with a meaning

| Statement | Meaning |
|---|---|
| `{ stmts }` | a nested scope |
| `unsafe { }` | allows raw-pointer and raw-memory operations (§8.5); skips borrow checks |
| `sandbox { }` | revokes all capabilities (§9.3) |
| `region r [with budget(N)] { }`, `region gpu r …`, `region kv_cache r with budget(N) [window(W)] { }` | §8.6 |
| `txn { }`, `abort;` | a transaction over the function's local variables: at entry each live local is snapshotted (a string, Vec or HashMap shared copy-on-write, a struct copied with its owned fields); `abort;` (a contextual keyword, allowed only inside `txn`) puts every one back and leaves the innermost txn; reaching the end keeps the changes. The same on every backend (lowering, `Lowerer_txn_open`). Assigning a global inside a txn is an error, since it would not be rolled back; data reached through `&mut` parameters or pointers, and IO, are not rolled back (`test_txn_abort.fd`) |
| `handle { } with { … }`, `do E.op(args)`, `resume(cont, v)` | effect handlers (§9.4) |
| `select { }`, `parallel { }`, `spawn scope { }`, `cancel scope;` | §10 |
| `grad(x) { }`, `backward(loss)` | automatic differentiation; `backward` needs an `f64` or tensor |
| `query (A, B) { }` | §2.11 |

---

## 6. Patterns and match

Source: `parser.fd:parse_pattern`, `parse_match_stmt`, `parse_match_expr`.

```
match   = "match" expr "{" {arm} "}"
arm     = pattern ["if" expr] "=>" body [","]
pattern = "_" | int ["..=" int | ".." int] | string | "true" | "false"
        | name
        | Enum "::" Variant ["(" binding {"," binding} ")"]
        | Variant "(" binding {"," binding} ")"
binding = name [":" type-name]
```

- In a `match` statement (one that starts a statement) every arm body is a
  block: `1 => { … }`. In a `match` used as a value, an arm body may also be a
  single expression: `val s = match n { 0 => 10, _ => 20 };`. A statement
  `match x { 1 => f(), _ => g(), }` is a parse error.
- Integer patterns are non-negative literals: `-1 =>` is a parse error
  (`invalid pattern 'Minus'`). `a..=b` and `a..b` match ranges.
- A bare name binds the value and matches anything, unless it is a unit
  variant of the scrutinee's enum: `None =>` on an `Option` is
  `Option::None =>`, as `Some(x)` is, and binds nothing
  (`TypeChecker_bare_pattern_is_unit_variant`,
  `Lowerer_bare_pattern_is_unit_variant`; `s6_bare_unit_variant.fd`,
  `s6_bare_unit_variant_first_arm.fd`. Until Nov 23 the name bound a variable
  and the arm matched every value, so the arms after it could not run).
- `Variant(x, y)` without the enum name works when the variant can be resolved
  from the scrutinee's type or is unique (`TypeChecker_resolve_pattern_enum`).
- Bindings are names (`_` allowed), optionally with a type name
  (`Some(v: i64)`). Constructor patterns nest (`Some(Ok(x))`, LG8: parser.fd
  `pd_desugar_arms` makes inner matches); there are no tuple, struct or `|`
  patterns.
- An arm guard `if expr` is checked after the pattern matches
  (`test_match_guard.fd`).
- A `match` on an enum must cover every variant, and one on `bool` both values,
  unless an arm is `_` or a name (`TypeChecker_check_exhaustive`;
  `test_match_nonexhaustive_fail.fd`). A `match` on integers or strings is not
  checked for exhaustiveness.

---

## 7. Generics

Source: `parser.fd:parse_func`, `typechecker.fd:TypeChecker_check_generic_call`,
`compiler/mono.fd`. Design: `docs/proposals/0002-real-generics.md`.

### 7.1 Declarations

```
generic-params = "<" gp {"," gp} ">"
gp             = T [":" Trait {"+" Trait}] | N ":" int-type | "const" N ":" int-type
```

- Functions, structs, enums, `impl` blocks and trait impls take type
  parameters. `where` clauses are not part of the grammar.
- `T: A + B` bounds a type parameter by one or more traits; every bound is
  checked at each call.
- `N: i64` (any integer type keyword) declares a const parameter; `[T; N]` in
  a parameter takes the array's length (`test_const_generic.fd`).
- Structs and `impl` blocks take const parameters too, written `const N:
  usize` (or `N: i64`): `struct Matrix<T, const N: usize>` takes a number as
  its argument (`Matrix<i64, 3>`), and the size is part of the type, so
  `Matrix<i64, 3>` and `Matrix<i64, 2>` are different types. A function
  generic over `N` (`fn trace<const N: usize>(m: Matrix<i64, N>)`) gets `N`
  from the argument's type, and in `impl<T, const N: usize> Matrix<T, N>`
  every method sees `N` as a constant; each size is its own instantiation
  (`test_const_generic_struct.fd`, `test_const_generic_impl.fd`). A const
  parameter's type must be an integer type
  (`test_diag_parse_struct_const_type_fail.fd`). A size mismatch is an
  error that still names the internal form of the number, e.g. `expects
  'Matrix<i64, __const_2>', got 'Matrix<i64, __const_3>'`
  (`test_const_generic_struct_size_fail.fd`).
- Generic functions are monomorphised: each set of type arguments gets its own
  copy (`mono.fd`).

### 7.2 Calls

- Type arguments are inferred from the arguments, and from the expected type
  when only the result mentions a parameter (`val z: i64 = zero();`).
- A type parameter that nothing gives is an error:
  `cannot infer the type parameter 'T' of 'make'`
  (`safety/test_generic_unbound_fail.fd`).
- An argument that does not implement a bound is an error:
  `i64 does not implement Show (required by T: Show in 'twice')`
  (`safety/test_generic_bound_fail.fd`).
- An integer literal given for a type parameter is an `i64`.
- `f::<T, U>(args)` gives the type arguments; their number must equal the
  function's type parameters (`mono.fd:mono_turbofish_callee`;
  `test_diag_turbofish_count_fail.fd`). Only a plain function name takes a turbofish.
- A reference cannot be a generic argument (§8.4).
- Methods of a generic `impl` are typed per receiver: on a `Slot<f64>` the
  method's `T` is `f64`. On a value written without type arguments (a bare
  `Option`) `T` is `i64`.

---

## 8. Memory and ownership

### 8.1 Values and handles

- Numbers, `bool`, structs, enums, tuples and arrays are values: assignment and
  by-value passing copy them.
- `Vec`, `HashMap` and `str` behave as values too: assigning one gives an
  independent value. The data is shared until one side writes; the first
  write copies it (`core.fd:Vec_share`, `vec_own`; ownership rule of
  proposal 0004, `lower.fd` `LOWER_RC = true`):

  ```fides
  var a: Vec<i64> = Vec::new();
  a.push(1);
  var b = a;
  b.set(0, 99);
  b.push(2);
  print(a.len());   // 1, and a.get(0) is still 1
  ```

  Copying a struct copies the collections it holds the same way: after
  `var t = s; t.v.push(2)`, `s.v` is unchanged (since Nov 15).
- `str` values are immutable, so sharing one is not observable.
- Only `@linear` struct types move (§8.7).

### 8.2 Run-time checks

Indexing a `Vec` or an array out of bounds, `unwrap` of `None` or `Err`,
`first`/`last` of an empty `Vec`, overflow and division by zero panic: the
program prints `Panic: <message>` and a stack trace on stderr and exits with
status 1 (`safety/test_oob_vec_get.fd`, `test_unwrap_none.fd`,
`test_empty_vec_first.fd`).

**The panic rule** (the same on `fid run`, C and native builds):

- A panic ends the program. It is never caught and never confined to the
  task or thread that raised it: a panic in a spawned task ends the whole
  program as one in `main` does (`test_task_panic_fail.fd`). Code that can
  fail and must go on returns `Result` or `Option` instead; `@no-panic` and
  `@crashfree` prove a function cannot panic (§12).
- It prints `Panic: <message>` and the stack trace on stderr, flushes
  stdout (what the program printed before is not lost), and exits with
  status 1.
- Nothing unwinds: no drop or cleanup code runs after a panic. Memory, files
  and sockets are given back by the operating system when the process ends.
- A stack overflow is a panic: `Panic: Stack overflow` (an OS guard page
  catches it; status 1). `fid run` stops at its recursion limit instead
  (1000 nested calls by default, `--max-recursion N`). A memory fault in
  `unsafe` code is reported the same way, as `Panic: Invalid memory access
  (null or dangling pointer)`.
- In a bare-metal build (`fid emit --bare`) a panic calls the program's
  `@panic_handler` function if it has one, else prints on COM1 and halts.

**Tasks are not killed.** Cancelling a task (`task_token_cancel`, a token's
deadline from `task_token_with_timeout`, `task_scope_cancel`) is a request
the task sees at its next checkpoint: a cancellable wait
(`task_sleep_cancellable`, `task_recv_cancellable`,
`task_wait_read_cancellable`/`task_wait_write_cancellable`,
`task_join_cancellable`) returns early and reports that it was cancelled,
and the task returns along its normal path, so its drops and `defer` blocks
run. A task that reaches no checkpoint runs to its end. No backend has a
mid-way kill: it would skip the drops of what the task owns, and nothing
unwinds. `fid run` interleaves tasks as the C and native runtimes do,
switching at yields, sleeps, awaits, joins and channel receives
(docs/reference/interp-tasks.md, `test_task_interleave.fd`).

### 8.3 What is freed automatically

There is no garbage collector and no general destructor mechanism. The compiler
frees:

- a `Vec` or `HashMap` that provably never leaves its local variable: the
  previous one when the variable is written again, the last one at each return;
  a `Vec` that owns strings or `Vec`s frees them too
  (`lower.fd:lw_auto_drop_vecs`);
- strings that provably die where they are used (`lower.fd:lw_auto_drop_strs`);
- for a local declared with an explicit struct type whose struct has a method
  `drop`, that method is called at function exit, last declared first
  (`Lowerer_lower_let_stmt`, `Lowerer_emit_drops`; `test_drop.fd`).

When the compiler cannot prove a value is finished with, it leaves it
allocated. Everything else lives until the process exits.

### 8.4 References

`&e` and `&mut e` make references; `*r` reads through one. Methods and field
access read through references automatically.

**Borrow rules** (`compiler/borrowck.fd`, after type checking; mode
`--borrowck=on` by default, also `warn` and `off`):

1. `&mut x` is an error while any borrow of `x` (or of a related path such as a
   field) is active (`test_borrowck_fail.fd`).
2. `&x` is an error while `&mut x` is active.
3. Changing `x` — assignment, field or index assignment, write through it, or a
   changing method — is an error while a borrow of `x` is active
   (`test_field_borrow_fail.fd`).
4. Reading `x`, or a field path of it, is an error while a mutable borrow of
   it, of a field under it or of a path above it is active
   (`test_lg3_read_during_mut_borrow_fail.fd`,
   `s8_4_read_while_mut_borrowed_rejected.fd`). A mutable borrow of `s.y`
   does not stop a read of `s.x`, and a shared borrow does not stop a read.

A borrow ends at the borrower's last use. `&s.x` and `&s.y` may coexist. Code in
`unsafe` is not checked.

**Escape rules** (`compiler/escape.fd`): references are second-class.

- A reference may be passed to a call, held in a local, and returned when it
  comes from a reference parameter. `Option`, `Result` and tuples may hold
  references in those same places.
- A function may not return a reference to a local or to a by-value parameter
  (`test_escape_return_fail.fd`).
- A reference may not outlive the block of what it points to
  (`test_escape_scope_fail.fd`):

  ```fides
  var a: i64 = 1;
  var r: &i64 = &a;
  {
      var b: i64 = 2;
      r = &b;          // error: 'r' would outlive 'b'
  }
  ```

- A reference may not be stored: not in a struct field, enum variant, array,
  `Vec` (`v.push(&x)`, `test_escape_store_fail.fd`), `Box`, global, or through
  another reference; not passed to a non-reference parameter of a generic or
  storing function; not sent to another task (`spawn f(&x)`). The exception
  is a struct with a lifetime parameter: `struct View<'a> { r: &'a T }` may
  hold a reference in a field, and a value of it is held like a reference
  (rooted where its fields point; a local, a parameter, a result or a field
  of another such struct, never stored; `test_frontend_lifetime_struct.fd`,
  docs/reference/lifetimes-across-calls.md).
- `&&T` and `&mut &T` are errors.
- A closure that captures a reference is rooted like that reference
  (`test_escape_closure_fail.fd`).

**Lifetimes** (`fid` only; parser, `compiler/escape.fd`, `compiler/borrowck.fd`).
Without them, which reference parameters a function's result borrows from is
inferred from its body. A signature can name it instead:

```fides
fn first<'a>(x: &'a i64, y: &i64) -> &'a i64 { return x; }
```

- Lifetimes are declared in the function's `<...>` (`'b: 'a` bounds are
  accepted and not used); `'static` needs no declaration. A lifetime of the
  result must be one a parameter carries. Either mistake is a parse error
  (`test_frontend_lifetime_undeclared_fail.fd`).
- The result borrows from exactly the parameters whose type carries one of the
  result's lifetimes: callers see that, not the body's inference, and a body
  returning a borrow of another parameter is an error
  (`test_frontend_lifetime_body_fail.fd`).
- `val r = first(&a, &b)` makes `r` a borrower of `a` (the first tied
  argument written `&path`), so the borrow rules above hold across the call
  (`test_frontend_lifetime_borrow_fail.fd`; `test_frontend_lifetime.fd`).
- The type stays `&T`: the names are kept as `@lifetime` annotations on the
  parameters and the function. Lifetimes on trait-method signatures are kept
  and an impl's method may tie its result only to the parameters the trait's
  ties it to. A struct may declare lifetime parameters
  (`struct View<'a> { r: &'a T }`) to hold a reference (escape rules above);
  `View<'a>` is the type `View`.

Passing `&c` where `&mut Counter` is expected is a type error
(`test_mut_ref_fail.fd`).

### 8.5 unsafe and raw pointers

`*mut T` and `*const T` are raw pointers. In the program's own module these
need an `unsafe { }` block or an `@unsafe` function
(`TypeChecker_require_unsafe`):

- reading through a raw pointer (`test_deref_safe_fail.fd`) and writing
  through one;
- `p_malloc`, `p_alloc`, `p_free`, `p_realloc`, `p_memcpy`, `p_memset`
  (`test_malloc_safe_fail.fd`);
- making a pointer from an integer (`test_int_to_ptr_safe_fail.fd`);
- calling an `extern "C"` function whose name does not start with `p_`;
- calling an `@unsafe` function, method or associated function
  (``calling the @unsafe function f() needs an `unsafe` block``;
  `test_unsafe_call_user_fail.fd`);
- inline assembly `asm!(...)` (`test_asm_unsafe_fail.fd`).

The rules hold in the program (also when it declares `module x;`) and in the
modules it imports from outside the standard library; the stdlib and the
compiler's own modules wait for M10. Detail is in
docs/reference/unsafe-calls.md. `&T` converts to `*mut T` without
`unsafe`. A double free panics with `Double free`, a free of anything that is
not a live block with `Invalid free` (`test_double_free_fail.fd`,
`test_bad_free_fail.fd`).

### 8.6 Regions

`region r { … }` is a scope. `region r with budget(N) { … }` adds up the
compile-time sizes of the `val`/`var` declarations in the block (an integer's
width in bytes, 8 per `f64` or reference, 16 per `str`, 24 per `Vec`, 48 per
`HashMap`, 8 per struct field; `TypeChecker_type_size`) and
reports an error when the sum exceeds `N` (`TypeChecker_check_stmt_inner`,
`Stmt::RegionBlock`; `test_budget_fail.fd`). Heap allocations in the block
count too: one whose size is known at compile time is added to the sum
(`p_malloc(N)` with a constant `N`, 24 per `Vec::new()`, an f-string of
text, integers and bools: its literal bytes, 20 per integer, 5 per bool, 1);
any other is an error (`… allocates on the heap: '.push()'`): growing
methods (`push`, `insert`, `clone`, `to_str`, …) on a `Vec`, `HashMap` or
string, string `+`, an allocating builtin, a constant-size allocation inside
a loop in the region, and a call to a function that allocates, itself or
through what it calls (`TypeChecker_budget_call`, `TypeChecker_compute_heap_alloc`;
`test_budget_heap_fail.fd`). A method called on a value of a user type that
allocates only through another method call is not seen, and neither is a
region nested in an expression (a closure). The `kv_cache` region's budget
is not held to the heap rule: the cache is bounded by its window. `region kv_cache` requires a
budget (`test_kv_cache_fail.fd`); `region gpu` has the same check
(`test_gpu_region_fail.fd`). A variable declared in a region is not visible
after it (`regions/test_region_escape.fd`). At a region's end the compiler
frees the `Vec`s and `HashMap`s declared in it that provably never leave it
(release C6, `lower.fd:lw_drop_function_vecs`, `Inst::RegionEnd`;
`test_mem_owned_drop.fd`); anything else allocated in a region lives on, and
there is no check that nothing uses a region's memory after its end.

A C build made with `FID_REGION_ARENA=1` gives each region an arena
(proposal 0005; a region that awaits or yields, a `gpu` and a `kv_cache`
region keep none). What is allocated while the region runs comes from the
arena and goes back at its end at once. A value kept by something outside
the region (a variable declared before it, a field or element of one, a
return value, a global, a channel, another task) is copied out of it
first, so programs print the same with or without the arenas. A raw
pointer or an `as i64` handle kept past the region is not followed (§8.5).
Other backends keep regions as scopes.

### 8.7 Linear types

A struct annotated `@linear` is used exactly once (`TypeChecker_consume_var`,
`TypeChecker_pop_scope`):

- using it a second time is an error (`was already consumed (moved)`,
  `test_money_linear_fail.fd`);
- leaving its scope without using it is an error (`is linear but was never
  consumed`, `test_banking_lost_money_fail.fd`);
- using it inside a loop is an error (`test_loop_linear_fail.fd`);
- assigning a new value while the old one is unused is an error;
- a closure may not capture it.

After an `if`, a linear variable counts as used only if both branches used it.

---

## 9. Effects, capabilities and sandbox

### 9.1 Effects

Every function has an inferred set of effects (`TypeChecker_compute_func_effects`,
`TypeChecker_scan_expr_for_effects`), propagated through calls to a fixpoint:

| Effect | Produced by |
|---|---|
| `IO` | `print`, `println`, `print_int`, `print_float`, `print_str`, `p_print_*`, `p_eprint_str`, `p_file_*`, `p_dir_*`, `p_process_*`, `p_clock_*`, `p_time_now`, `p_getenv`, `p_env_get`, `p_read_line`, `p_read_stdin`, `p_os_random_u64`, `p_system`, `p_mkdir`, `p_rename`, `p_remove` |
| `Net` | `p_socket_*`, `p_net_*`, `p_http_*` (except names containing `build` or `parse`), `p_tls_*`, `p_dns_*` |
| `Spawn` | `p_spawn_process`, `p_spawn_thread`, `p_thread_spawn` |
| `Diverge` | `p_panic` |
| `Alloc` | `p_vec_new`, `p_vec_push`, `p_map_new`, `p_map_insert`, `p_string_concat`, other `p_*` allocation builtins, `p_kv_cache_*` |
| `Read`, `Write` | `p_kv_cache_*` |
| `Diff` | `p_tensor_*` operations, `p_quantize_q8`, `p_dequantize_q8`, `p_qmatmul` |
| `Noise` | `p_dp_noise`, `p_dropout`, `p_gaussian_noise` |
| `Comm` | `p_all_reduce`, `p_broadcast`, `p_all_gather`, `p_scatter`, `p_send_tensor`, `p_recv_tensor` |

A function declares effects after its return type: `!IO`, `!{IO, Net}`, or `!{}`
for none. For a top-level function with a declaration, an inferred effect that
is not declared is an error (`TypeChecker_verify_declared_effects`):

```fides
fn bad() !{} {
    print("not pure!");    // error: inferred effect '!IO'
}
```

- Declared names are not validated; an unknown name is accepted.
- Higher-order functions are effect-polymorphic (Nov 4). Inside
  `fn apply(f: fn(i64) -> i64, x: i64) -> i64`, a call through `f` adds no
  effect; each call of `apply` adds the effects of the function passed: a
  named function's inferred effects, a closure's body's, nothing when the
  caller forwards its own function parameter, and every effect for any other
  function value (`TypeChecker_register_ho`, `TypeChecker_passed_fn_effects`;
  `effects/test_effect_ho_instantiation.fd`). So a `!{}` function may call
  `apply(pure_inc, 41)`.
- Effects of `impl` methods are not compared with their declarations; a call
  from a function with a declaration to a method whose declared effects it
  lacks is an error (`TypeChecker_check_method_call_rules`).

### 9.2 Capabilities

`use m with { IO, FS }` grants capabilities. When any import of the program has
a `with` clause, the program runs in strict mode: a call that needs a
capability not granted is an error (`TypeChecker_collect_capabilities`,
`TypeChecker_check_capability`; `capability/test_cap_deny_io.fd`):

| Capability | Calls that need it |
|---|---|
| `IO` | `print`, `println`, `print_int`, `print_float`, `print_str` |
| `FS` | `read_file`, `write_file`, `file_open`, `file_read`, `file_write` |
| `Net` | `http_get`, `http_post`, `socket_connect`, `socket_send`, `socket_recv` |
| `Spawn` | `spawn`, `exec`, `system`, `p_thread_spawn` |
| `Alloc` | `p_malloc`, `p_free`, `p_malloc_persistent`, `p_arena_save`, `p_arena_restore` |

The check looks at each call by name, in every function except those of `core`.
It does not follow calls: a function that calls another function which prints
is not itself checked for `IO`.

### 9.3 sandbox

Inside `sandbox { }` every call in the table above is an error, whatever has
been granted (`Sandbox error: 'print()' requires 'IO' capability`,
`test_sandbox_fail.fd`). So is a call to anything whose effects include
`IO`, `Net` or `Spawn`: an IO builtin (`p_file_read`, `p_getenv`,
`p_socket_*`, …), a function whose inferred effects (transitive, as for
`!{}`) include one, a method of a user type that does, a higher-order
function given such a function, and a call through a function value
(`Sandbox error: 'logit()' has effect '!IO'`, `TypeChecker_check_sandbox_call`;
`test_sandbox_helper_io_fail.fd`, `test_sandbox_file_read_fail.fd`). So is
code whose effects are not known: a C function declared `extern "C"`, a
function loaded at run time (`p_dl_*`), a raw system call, inline assembly,
and any function or method that reaches one of them through its callees
(`TypeChecker_compute_ffi_reach`; a call `x.m()` whose receiver type is not
known counts when any method named `m` reaches C code;
`test_sandbox_ffi_fail.fd`). Pure functions may be called.

### 9.4 Effect handlers

```
handle   = "handle" block "with" "{" handler {"," handler} "}"
handler  = Effect "." op "(" [name {"," name}] ")" "=>" block
do       = "do" Effect "." op "(" [args] ")"
resume   = "resume" "(" expr "," expr ")"
```

`do E.op(args)` inside the `handle` block runs the matching handler with the
arguments bound to its names (as `i64`). In the handler, `cont` is the rest of
the `handle` block after the `do`, and `resume(cont, v)` runs it
(`test_effect_handler.fd`, `test_effect_resume.fd`, `test_effect_noresume.fd`).

---

## 10. Concurrency

### 10.1 spawn

`spawn f(args)` starts `f(args)` on an OS thread (`lower.fd`, `Expr::Spawn`,
`p_thread_spawn`) and gives a `Future<T>`, where `T` is `f`'s return type.
`spawn` of any other expression evaluates it at once.

- Every argument of the spawned call, and every variable the expression names,
  must be `@send` (`TypeChecker_is_send_type`; `test_spawn_send_fail.fd`).
  Numbers, `bool`, `str`, enums, structs marked `@send`, structs whose fields
  are all `@send`, and `Vec`, `HashMap`, `Option`, `Result`, `Box`, `Future`,
  `Mutex`, `Channel` of such types are `@send`. References and raw pointers are
  not.
- No task is left behind (CA6, `docs/reference/structured-concurrency.md`):
  a `spawn f(x)` is accepted only when its handle is read later in the
  function (awaited, joined, passed on, returned or stored:
  `val t = spawn f(x); ... await t`), when it is inside `spawn scope { }`
  (§10.4), or when it is detached by name, `spawn detached f(x)`. A spawn
  whose handle is dropped (`spawn f(x);` as a statement) or never read is a
  "Concurrency error" (`test_spawn_unjoined_fail.fd`,
  `test_spawn_unjoined_handle_fail.fd`).
- `spawn detached f(x)` starts the task as `spawn f(x)` does, with the same
  checks of its arguments, and gives `void`: there is no handle. `detached`
  is a contextual word (`spawn detached(5)` still calls a function named
  `detached`). A detached green task's block is freed when it finishes; the
  end of a `spawn scope` still joins every thread, detached ones included
  (`test_spawn_detached.fd`).
- A struct, enum or tuple that holds a string or a collection is not
  accepted as a spawn argument (its copies would share it, CA5): pass the
  strings and Vecs as arguments of their own.

### 10.2 async and await

```fides
async fn compute_value(x: i64) -> Future<i64> { … }   // the body returns i64
val r = await compute_value(1);
```

- Declare an `async fn`'s result as `Future<T>`; its body returns `T`
  (`TypeChecker_check_func_body`).
- `await e` requires a `Future<T>` and gives `T` (`test_async_fail.fd`).
- An `async fn` runs as a green task on its own stack: it starts at the call
  and runs until it first waits, then the caller continues. Float and struct
  arguments and results pass through an adapter; an `async fn` whose
  parameters or result the task cannot carry runs to its end at the call
  (`lower.fd:lw_is_async_task`, `Lowerer_task_adaptable`). That includes
  a reference parameter, since a reference cannot reach another task
  (§8.4): the function runs on its caller's stack, and where it waits its
  caller waits (`test_lowering_async_ref.fd`).
- An `async fn` may be a method of an impl block or a trait impl, and a
  `cap` may declare `async fn m(..) -> Future<T>;`. The method runs as a
  task as an async function does, through `dyn` too
  (`lower.fd:lw_split_async_methods`; `test_lowering_async_method.fd`).
  An async method of a generic impl runs as a task per instance (its
  `self` and any generic struct argument travel as heap copies:
  `Lowerer_task_gstruct`; `test_lowering_async_generic_method.fd`,
  `test_lowering_async_generic_task.fd`), and a `cap`'s `async fn` with a
  default body gives each impl that uses the default an async method
  (`test_lowering_async_cap_default.fd`). A `dyn` value of a generic trait
  impl is not supported.
  The stdlib module `task` provides `task_go`, `task_yield`, `task_run`,
  `task_sleep`, `task_result`, …
- `yield e` evaluates `e`: not implemented as a suspension.

### 10.3 parallel

```fides
parallel {
    | a = fib(n);
    | b = fib(n + 1);
    | val k: i64 = n * 2;
      println("branch " + int_to_str(k));
}
```

Branches are separated by `|` (`parse_parallel_stmt`; `test_parallel_branches.fd`).
A branch that is one call is spawned on a thread; `x = f(…)` is spawned and `x`
gets the result when the threads are joined; another branch runs as a function
of its own on its own thread when it reads only word-sized locals and assigns
none; any other branch runs in order on the current thread. All threads are
joined before the statement ends.

### 10.4 spawn scope

`spawn scope { … }` runs its body; at the end every spawned thread not yet
joined is joined (`lower.fd`, `Stmt::SpawnScope`; the C runtime's
`p_executor_run_all` joins all outstanding threads, not only those of the
scope). `cancel scope;` cancels the pending tasks of the innermost scope
(`test_spawn_scope.fd`, `test_cancellation.fd`).

### 10.5 Channels and select

The `channel` module provides `channel_create(capacity)`, `channel_send`,
`channel_recv`, `channel_try_recv`, `channel_close`, `channel_is_closed`
(`test_channel.fd`). The builtins `p_chan_create`, `p_chan_send` and
`p_chan_recv` are the runtime layer; the value given to `p_chan_send` must be
`@send` (`test_send_fail.fd`).

```
select = "select" "{" {"case" expr "=>" name "=>" block [","] | "default" "=>" block [","]} "}"
```

`select` takes the first case whose channel has a value, binding it to the
name; otherwise it runs `default` (`test_channels_select.fd`).

### 10.6 Threads and globals

A function that runs on another thread (named in `spawn` or `p_thread_spawn`),
or any function it calls, may not assign a global of the program unless it
calls a function whose name ends in `mutex_lock` or `write_lock`
(`TypeChecker_verify_no_races`; `test_race_global_fail.fd`). Data shared
through pointers is not checked.

### 10.7 Output of threads and tasks

Decided Nov 17 (docs/verification/semantics-2.md §20.3):

- The text of one `print` or `println` call is written whole: another
  thread's output never appears inside it.
- Calls are ordered when one happens before the other: in program order
  inside a thread or task; a thread's or task's calls before it returns
  before those its awaiter makes after `await` returns
  (`test_verify_semantics_ext.fd`, line O2); before and after the joins at
  the end of `spawn scope` and `parallel`; a sender's calls before
  `channel_send` before the receiver's after the `channel_recv` that takes
  that value. Calls that no such order relates may appear in either order;
  a program whose output depends on it has more than one correct output.

---

## 11. Annotations

```
annotation = "@" name {"-" name} ["(" arg {"," arg} ")"]
arg        = name ["=" arg] | int | float | string | "[" arg {"," arg} "]"
```

Annotations are written before a top-level `fn` or `struct` (after or before
`pub`), before a method of an `impl` block (§2.4), before a function
parameter, and before a `val`/`var` statement. They cannot be written on
methods of a trait impl (§2.4), struct fields, enums, globals or other
items, and an annotated function cannot also be `async`, `comptime` or
`interrupt` (the parser expects `fn` after the annotations).

Every annotation name is checked: a name not in the table below is an error
(`Unknown annotation`), and one on the wrong kind of item is an error
(`Misplaced annotation`) (`typechecker.fd:annotation_target_mask`,
`check_one_annotation_fd`).

| Annotation | On | What it does | Where |
|---|---|---|---|
| `@deterministic` | fn | No `IO`, `Net`, `Spawn` or `Diverge` effect and no floating point; methods it calls must be `@deterministic` | `typechecker.fd:TypeChecker_verify_deterministic`, `TypeChecker_check_method_call_rules` |
| `@conserve` | fn | No `IO`/`Net`/`Spawn`/`Diverge` effect, no floats, calls only `@conserve` functions and methods, never a mint (`Money_new`, `Money_from_cents`, `Money_from_dollars`), no `Money { .. }` literal outside `stdlib/money.fd` (whose `Money_add`/`Money_sub` build their result from their inputs; a `Money { .. }` literal does not parse outside the module that declares `Money`) | `typechecker.fd:TypeChecker_verify_conserve` |
| `@gas(N)` | fn | The body's static cost (nodes, with trips of loops that have literal bounds) plus its callees' must not exceed `N` | `typechecker.fd:TypeChecker_verify_gas` |
| `@no-alloc` | fn | No allocating call or method (`push`, `Vec::new`, string `+`, …) | `typechecker.fd:check_safety_body` |
| `@no-recursion` | fn | No direct or indirect recursion | `typechecker.fd:check_safety_body`, `TypeChecker_verify_safety_annotations` |
| `@no-unbounded` | fn | No `while` loop | `typechecker.fd:check_safety_body` |
| `@no-indirect` | fn | No call through a function or `dyn` parameter or dynamic dispatch | `typechecker.fd:check_safety_body` |
| `@no-panic` | fn | No `unwrap`/`expect`/`p_panic`, no division or remainder by a value not proven non-zero, no unchecked indexing or overflow (unless `@wrapping`) | `typechecker.fd:check_safety_body`, `scan_for_panic_risks` |
| `@requires-audit` | fn | The function must also be `@audit` | `typechecker.fd:check_safety_body` |
| `@bounded-stack[(N)]` | fn | Everything reachable is non-recursive and called directly; with `N`, the deepest call chain is at most `N` frames | `typechecker.fd:TypeChecker_verify_safety_annotations` |
| `@firmware` | fn | No OS-dependent runtime call (files, sockets, processes, threads, environment) | `typechecker.fd:check_safety_body`, `is_os_dependent_func` |
| `@crashfree` | fn | `no-panic` + `no-unbounded` | `typechecker.fd:expand_safety_primitives` |
| `@medical`, `@dal-a` | fn | `no-alloc` + `no-recursion` + `no-unbounded` + `no-panic` + `requires-audit` | `typechecker.fd:expand_safety_primitives` |
| `@safety-critical` | fn | `no-alloc` + `no-recursion` + `no-unbounded` + `no-panic` + `no-indirect` | `typechecker.fd:expand_safety_primitives` |
| `@autosar` | fn | `no-alloc` + `no-recursion` + `no-unbounded` + `no-panic` | `typechecker.fd:expand_safety_primitives` |
| `@audit` | fn | Logs entry (with parameters) and exit at run time; satisfies `@requires-audit` | `lower.fd:Lowerer_lower_func` |
| `@constant_time` | fn | No `IO`/`Net`, no early return, no branch, loop, short-circuit, index or division on secret data (every parameter not `@public`), calls only `@constant_time` functions. `fid` checks the source only, not the generated code | `typechecker.fd:TypeChecker_verify_constant_time` |
| `@public` | param | The parameter is not secret for `@constant_time` | `typechecker.fd:ct_secret_dependence` |
| `@realtime` | fn | No `IO`/`Net`/`Spawn` effect, no `while`, no recursion, no allocating, blocking or dynamic-dispatch call | `typechecker.fd:TypeChecker_verify_realtime` |
| `@differentiable` | fn | Restricts effects for automatic differentiation | `typechecker.fd:TypeChecker_verify_differentiable` |
| `@quantized` | fn | Restricts effects; must not reach `dequantize_q8` | `typechecker.fd:TypeChecker_verify_quantized` |
| `@checkpoint` | fn | Requires `@differentiable` and `@deterministic` | `typechecker.fd:TypeChecker_verify_checkpoint` |
| `@dp(epsilon=E)` | fn | Requires the `Noise` effect, a positive epsilon, and callees whose total epsilon is at most `E` | `typechecker.fd:TypeChecker_verify_dp` |
| `@shard` | fn | Requires the `Comm` effect | `typechecker.fd:TypeChecker_verify_shard` |
| `@adapter` | fn | Requires at least one parameter | `typechecker.fd:TypeChecker_verify_adapter` |
| `@tainted` | param | The parameter's value may not reach a sink (output, files, the network, processes, SQL/shell/HTML builders, a `@sink` function), also inside a callee, unless a sanitizer (a `@sanitizer` function, or `sql_escape`, `html_escape`, ...) cleans it first (`test_taint_flow*.fd`); on a function the name check reports it as misplaced | `typechecker.fd:TypeChecker_verify_tainted`, `tf_call` |
| `@sink`, `@sanitizer` | fn | A sink, and a function whose result is clean, for the `@tainted` check | `typechecker.fd:tf_call` |
| `@secret`, `@sensitive` | fn, param, local | The value may not reach an output, log, file, network, global, spawn or call through a function value; a `@secret` function's result is secret | `secrecy.fd:sec_analyze_func` |
| `@declassify` | fn | An audited release: the function may return, or write through reference parameters, secret or sensitive values, and its result is not secret | `secrecy.fd:sec_analyze_func`, `sec_ret` |
| `@zeroize` | local | The variable (and the bytes of a `str` or `Vec` it holds) is overwritten with zeros at function exit | `lower.fd:Lowerer_lower_let_stmt`, `Lowerer_emit_zeroize` |
| `@wrapping` | fn | Integer arithmetic wraps instead of panicking on overflow | `lower.fd:Lowerer_lower_func` |
| `@deprecated[(use = "f")]`, `@deprecated("text")` | fn | Each call of the function (other than its own) is a warning, which names the replacement `f` or gives the text; `fid lint` prints it (`test_frontend_deprecated.fd`) | `typechecker.fd:TypeChecker_warn_deprecated` |
| `@unsafe` | fn | The whole body is an `unsafe` block | `typechecker.fd:TypeChecker_check_func` |
| `@allocator(name)` | fn | Requires an argument; nothing else | `typechecker.fd:TypeChecker_verify_allocator_params` |
| `@interrupt(N)` | fn | An interrupt handler for vector `N` (also `interrupt fn`) | `lower.fd:Lowerer_lower_func` |
| `@kernel`, `@shader` | fn | A GPU entry point; arguments give the workgroup size | `lower.fd:Lowerer_lower_func` |
| `@simd` | fn | Marks the function for wider loop vectorisation | `lower.fd:Lowerer_lower_func` |
| `@linear` | struct | Values are used exactly once (§8.7) | `typechecker.fd:TypeChecker_register_program` |
| `@send` | struct | Values may be sent to another thread (§10.1) | `typechecker.fd:TypeChecker_register_program`, `TypeChecker_is_send_type` |
| `@sync` | struct | Recorded; no check reads it: not implemented | `typechecker.fd:TypeChecker_register_program` |
| `@repr(C)` | struct | C-compatible field layout | `lower.fd:Lowerer_make_struct_layout` |
| `@packed` | struct | Fields with no padding | `lower.fd:Lowerer_make_struct_layout` |
| `@derive(…)` | struct | Generates `S_debug_str` (Debug), `S_clone` (Clone), `S_to_json` (Serialize), `S_default` (Default), `S_from_json` and `S_from_json_error` (Deserialize: each field read from its key; the error names a missing key or a wrong type), `S_equals` (PartialEq, Eq) | `compiler.fd:process_derive` |
| `@cfg(…)` | fn, struct | §12 | `compiler.fd:apply_cfg_filter`, `check_cfg` |
| `@panic_handler` | fn | In a bare-metal (`emit --bare`) or UEFI (`emit --uefi`) image, the function a panic or a failed check calls; elsewhere not read | `main.fd:cmd_build` (`pe_ir_generate_flat`, `pe_ir_generate_uefi`) |
| `@test`, `@bounded`, `@cpu_local` | various | Accepted by the name check; no pass reads them: not implemented. `@cpu_local` is valid only on globals, which cannot carry annotations. | `typechecker.fd:annotation_target_mask` |

A function with any of the safety primitives (`no-alloc`, `no-recursion`,
`no-unbounded`, `no-indirect`, `no-panic`, `requires-audit`, `bounded-stack`,
`firmware`) may call only functions and methods that have every primitive it
has (`check_safety_body`, `TypeChecker_check_method_call_rules`;
`test_crashfree_method_fail.fd`). Built-in methods of `Vec`, `HashMap`, `str`,
`Option` and `Result` are judged by the rules above. Rule-by-rule requirements
and tests: [safety-rules.md](safety-rules.md).

---

## 12. Conditional compilation

`@cfg(…)` on a function or a struct keeps the item only when every argument
holds (`compiler.fd:apply_cfg_filter`, `check_cfg`; `test_cfg.fd`). Arguments:

| Argument | Holds when |
|---|---|
| `linux`, `windows`, `macos`; `platform = "…"`, `os = "…"` with those values | the target's system is that one: the one `--target` names, else the system `fid` runs on (from the `OS` environment variable, `/proc/self/exe` and `/bin/freebsd-version`; `compiler.fd:apply_cfg_filter`) |
| `x86_64`, `aarch64`, `arm64`, `riscv64`, `riscv`; `arch = "…"`, `target_arch = "…"` with those values | the target's architecture is that one (`compiler.fd:cfg_arch_is`; `arm64` is `aarch64`, `riscv` is `riscv64`; `x86_64` when there is no `--target`) |
| `feature = "f"` | ark turned feature `f` on for the project |
| `release` | the build is not under `--debug` |
| `debug` | the build is under `--debug` |
| anything else | always |

The platform is the target's when `--target` names one, the host's
otherwise. `@cfg` is not available on enums, `impl` blocks, globals or
statements — inside a function body, `comptime if` with §4.11's target
queries (`comptime if comptime target_os() == "windows" { .. }`) is the
form.

---

## 13. The prelude

`stdlib/core.fd` is loaded into every program. All its items are public.

### 13.1 Functions

`print(x)` (built in, any value), `println(x)`, `assert(c)`, `assert_msg(c, msg)`,
`assert_eq(a, b)`, `assert_ne(a, b)` and their `_msg` forms, `panic(msg)`,
`max`, `min`, `abs`, `clamp` (on `i64`), `int_to_str`, `float_to_str`,
`byte_to_str`, and the string functions of §13.4.

### 13.2 Option and Result

```fides
pub enum Option<T> { Some(T), None }
pub enum Result<T, E> { Ok(T), Err(E) }
```

- `Option`: `is_some`, `is_none`, `unwrap`, `unwrap_or`, `map`, `and_then`,
  `or_else`, `unwrap_or_else`, `expect`, `context(msg)` (to `Result<T, str>`).
- `Result`: `is_ok`, `is_err`, `unwrap`, `unwrap_or`, `ok`, `err`, `map`,
  `map_err`, `and_then`, `unwrap_or_else`, `expect`, `context(msg)`,
  `with_context(f)`.
- `map` and `and_then` take a function from `T` to `T`.

Traits `Iterator` and `Error` are declared in core.

### 13.3 Vec and HashMap

`Vec<T>`: `Vec::new()`, `push`, `pop`, `len`, `is_empty`, `get`, `set`, `clear`,
`capacity`, `insert`, `remove`, `contains`, `reverse`, `first`, `last`,
`extend`, `truncate`, `sort`, `for_each`, `fold`, `any`, `all`, `find`,
`position`, `clone`, `map`, `filter` (`TypeChecker_check_method_call`). `push`
and `set` check the element type (`test_vec_elem_fail.fd`). A `Vec` written
without an element type holds `i64`. `Vec::filled(n, x)` makes `n` elements,
each `x` (Rust's `vec![x; n]`), for numbers, bools, strings, Vecs and maps; a
`Vec` of structs or enums is built with `push`.

`HashMap<K, V>`: `HashMap::new()`, `insert`, `get`, `contains`, `remove`, `len`,
`is_empty`, `clear`, `get_or`, `keys`, `values`, `clone`. Without type
arguments it is `HashMap<str, i64>`.

`m.keys()` gives the keys in ascending order (decided Nov 17,
docs/verification/semantics-2.md §20.2): integer keys by value, `str` keys
byte by byte as `str_compare`. `m.values()` gives the values in the order of
`m.keys()`. The order does not depend on insertion order, capacity or where
the strings are in memory. (`stdlib/core.fd` `HashMap_keys` sorts `str` keys
as words for now: `tests/verify/bugs/d22_hashmap_str_keys_order.fd`,
miscompile-log D22.)

`Box::new(x)` gives `Box<T>`; `*b` gives the `T`. `Rc::new` and `Arc::new` give
`Rc<T>`/`Arc<T>` with `clone`, `drop`, `get`, `count` (`Rc::new(41)` is an
`Rc<i64>`, as a literal is an `i64`). A number or a bool goes into a
one-word block `[count, value]`; anything else (a string, a `Vec`, a
struct, an enum) goes into a tagged block `[count, tid, payload]` whose
last `drop` frees what the payload owns (the tid is core's `__p_mem_drop`
type id). Nothing drops a handle by itself: `drop` does, once per copy.

### 13.4 Strings

`str` has no methods. Core provides `str_len`, `str_byte_at`, `str_slice`,
`str_concat`, `str_equal`, `str_compare`, `str_find`, `str_rfind`,
`str_contains`, `str_starts_with`, `str_ends_with`, `str_trim`,
`str_trim_start`, `str_trim_end`, `str_split`, `str_split_char`, `str_lines`,
`str_replace`, `str_repeat`, `str_to_upper`, `str_to_lower`, `str_parse_int`,
`str_parse_hex`, `str_parse_float`, `str_bytes`, `str_from_bytes`, and others;
the module `string` adds `str_join` and more. `StringBuilder` builds strings
(`StringBuilder::new()`, `append_str`, `to_str`; `test_method_concat.fd`).

---

## 14. Differences from fidesc

`fid` defines the language. The rules where `fid` and the frozen `fidesc`
differ are listed in
[language-guide.md § 10, "Where `fid` and `fidesc` differ"](language-guide.md#where-fid-and-fidesc-differ-nov-4),
with the rules and tests in [safety-rules.md](safety-rules.md). One more
difference is no longer one: both compilers instantiate a higher-order
function's effects at each call from the function passed (`fidesc`:
`src/typechecker.rs`, `instantiate_fn_arg_effects`; `fid` since Nov 4: §9.1),
and `effects/test_effect_ho_instantiation.fd` is accepted by both.

---

## 15. Docs that disagree with the compiler

- `docs/reference/syntax.md:157`: the suffix literal `42i64` does not lex as
  one literal; there are no literal suffixes (§1.5).
- `docs/reference/syntax.md:164`: `b"bytes"` byte strings do not exist (§1.8).
- `docs/reference/syntax.md:194-198`: `isize`, `usize` and `char` are not
  types (§3.1).
- `docs/reference/syntax.md:217`: `vec![…]` does not parse.
- `docs/reference/syntax.md:237-244` (and `:823`): tuple structs, unit structs
  and struct-like enum variants do not parse (§2.2, §2.3).
- `docs/reference/syntax.md:426` (and `:990`, `:1152`): `where` clauses do not
  parse (§7.1).
- `docs/reference/syntax.md:433` (and `:1077`, `:1291`): `.await` is not a
  postfix operator; `await e` is prefix (§4.1).
- `docs/reference/syntax.md:504-506`: `val Point { x, y } = p` and
  `val Some(v) = o` do not parse; only tuple destructuring does (§5.1).
- `docs/reference/syntax.md:535` and `docs/reference/language-guide.md:390`: no
  move check exists for `String`/`Vec`; only `@linear` types move (§8.1).
- `docs/reference/syntax.md:634`: a statement `match` needs block arms;
  `Red => 0xFF0000,` is a parse error there (§6).
- `docs/reference/syntax.md:655`: struct-like patterns
  `Shape::Circle { radius }` do not parse (§6).
- `docs/reference/syntax.md:669`: exhaustiveness is checked only for enums and
  `bool` (§6).
- `docs/reference/syntax.md:681`: `for (i, x) in …` does not parse (§5.4).
- `docs/reference/syntax.md:715`: `loop` is not an expression (§5.4).
- `docs/reference/syntax.md:741`: `defer close(file);` does not parse; `defer`
  takes a block (§5.5).
- `docs/reference/language-guide.md:250`: `val x = 42;` is an `i64`, not an
  `i32` (§3.4).
- `docs/reference/language-guide.md:282` and `:291`: `&self` is not valid
  parameter syntax (§2.1).
- `docs/reference/language-guide.md:331`: `str` ↔ `i64` is allowed only in the
  runtime layer and the compiler's own modules; in a program it is a type error
  (§3.3).
- `docs/reference/language-guide.md:433`: `push` during a `for` over the same
  `Vec` is rejected by the borrow checker (`test_iter_invalidation_fail.fd`).
- `docs/reference/language-guide.md:561`: `str` has no `to_string` method, and
  a reference to a region-local value cannot be returned (§8.4, §13.4).
- `docs/reference/language-guide.md:699`: `FileIO` is not an inferred effect; the
  function would get `IO` and be rejected (§9.1).
- `docs/reference/language-guide.md:712`: `Read` and `Write` are not
  capabilities the compiler checks; the checked ones are `IO`, `FS`, `Net`,
  `Spawn`, `Alloc` (§9.2).
- `docs/reference/language-guide.md:718`: a `sandbox` denies all capabilities,
  granted or not (§9.3).
- `docs/reference/language-guide.md:720` and `docs/reference/features.md:222`:
  capabilities are checked per call by name and do not propagate through the
  call graph (§9.2).
- `docs/reference/language-guide.md:731`: `@conserve` is money conservation
  (effects, floats, `@conserve` callees, no minting), not "no leaks" (§11).
- `docs/reference/language-guide.md:736`: `@sync` is recorded and not checked
  (§11).
- `docs/reference/features.md:171`: default arguments are implemented (and
  named arguments) (§2.1).
- `docs/reference/features.md:209-210`: `use m::{a}` and `use m::*` import the
  whole module; the list does not select (§2.9).
- `docs/reference/features.md:198` and `:262`: no region use-after-free check
  exists. The type checker writes `freed_regions` and never reads it, and the
  interpreter's region end only pops a stack of names. A region is a scope
  (§8.6).
- `docs/reference/features.md:247`: fixed Nov 4 — `fid` now instantiates
  effects per call site (§9.1).
