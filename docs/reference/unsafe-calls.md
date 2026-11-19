# Who needs `unsafe` (release M2, M10)

`fid` holds raw memory to one rule: code that can write or read memory
nobody vouched for stands in an `unsafe { }` block (with a `// SAFETY:`
comment saying why the access is sound) or in an `@unsafe` function, whose
caller makes that promise. The check is `TypeChecker_require_unsafe` in
`compiler/typechecker.fd`.

## What needs it

- reading or writing through a raw pointer (`*mut T`, `*const T`);
- `p_malloc`, `p_alloc`, `p_free`, `p_realloc`, `p_memcpy`, `p_memset`;
- making a pointer from an integer;
- calling an `extern "C"` function (one whose name does not start with `p_`
  and that no trusted header declares);
- **calling an `@unsafe` function** (release M10): a free function, a method
  (`x.m()`) or an associated function (`S::f()`), also by a module-qualified
  name (`volatile::ptr_read(a)`). The message is
  `calling the @unsafe function NAME() needs an `unsafe` block`.

An `@unsafe` function's body is one `unsafe` block, so it may call other
`@unsafe` functions directly. A safe function that calls one inside an
`unsafe` block is a safe wrapper: it is the one that keeps the contract.

## Which code is checked

- the program being compiled, with or without a `module x;` declaration
  (release M2: before, a program file that declared its module was not
  checked; `g_tc_program_module`, the last module
  `TypeChecker_register_program` sees);
- the modules it imports from outside the standard library directory
  (`tc_note_module_file`, called by `compiler.fd record_module_file`).

Not checked yet: the standard library (its `unsafe` blocks carry `// SAFETY:`
comments, listed by `fid audit <file> --stdlib --strict`), the compiler's
own modules (`compiler::*`), and a program that is itself a file of the
standard library directory (`tc_note_program_file`, when the driver calls
it).

## How @unsafe functions are found

`TypeChecker_register_program` records the functions each program marks
`@unsafe` (`tc_note_unsafe_fns`): a free function under its name, a method
or associated function as `Struct_method`. A later program that defines the
same name without `@unsafe` replaces the record, so the program's own safe
function of the same name as a library's `@unsafe` one is not affected.

Known limits:

- a call through a function value (an `@unsafe` function passed by name and
  called as a parameter) is not checked;
- methods of generic structs are not checked (only `Type::Named`
  receivers);
- the stdlib's own calls are exempt with the rest of the stdlib.

## Tests

- `tests/bootstrap/test_frontend_unsafe_module_decl_fail.fd`,
  `test_frontend_unsafe_module_decl.fd` (M2, `module x;`);
- `test_frontend_unsafe_import_fail.fd` (M2, imported user module);
- `test_unsafe_call_user_fail.fd`, `test_unsafe_call_method_fail.fd`,
  `test_unsafe_call_static_fail.fd`, `test_unsafe_call_qualified_fail.fd`,
  `test_unsafe_raw_apis_fail.fd` (M10: each call form outside `unsafe`);
- `test_unsafe_call_ok.fd`, `test_unsafe_raw_apis.fd` (the same calls inside
  `unsafe` and from `@unsafe` functions).
