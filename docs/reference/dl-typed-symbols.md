# Typed symbols from run-time library loading

Roadmap L6C. A C function found at run time can be bound with a function
type and called through it:

```fid
val libm: i64 = p_dl_load("libm.so.6");          // ucrtbase.dll on Windows
val sq: fn(f64) -> f64 = p_dl_find(libm, "sqrt");
println(int_to_str((sq(2.25) * 100.0) as i64));  // 150
```

- **Checking** (compiler/typechecker.fd `TypeChecker_check_dl_sym`, for
  `p_dl_find` and the simulated `p_dl_sym`): the lookup takes the declared
  function type; its parameters and result must be C types (integers,
  floats, bool, str, raw pointers, void). Every call through the binding is
  checked against that type (`TypeChecker_check_fn_value_args`). Without a
  declared function type the lookup is the `i64` address, as before.
- **Calling** (compiler/lower.fd `Lowerer_dl_fn_let_type`): such a `let` is
  lowered as `addr as fn(..)`, the function value whose env holds the marker
  `__fid_c_fnptr` and the address. A call through it, or through a
  parameter it is passed to, calls the address as a plain C function, with
  no env and each argument in its own C type (compiler/codegen_c.fd,
  `CallPtr`).

## Backends

- C (`fid ccompile`): works (tests/bootstrap/test_dlsym_typed_call.fd).
- Interpreter (`fid run`): cannot call machine code, and stops with a
  message saying so. It also has no `p_dl_load`/`p_dl_find`.
- Native (`fid emit`, pe_ir and nat_ir): binding works, a call does not yet:
  their `CallPtr` passes the env first and does not know the marker (the
  marker's address is 0 there).

## Limits

Only a `let`/`val` bound straight from the lookup is made a C function
pointer. Assigning a lookup to an existing variable, returning it, or
storing it in a field is not; write `p_dl_find(lib, name) as fn(..)` there.
