# Locals on the stack (LF13)

A value that never leaves its function does not need the heap. The lowering
(`compiler/lower.fd`, after ownership has placed every drop) moves four kinds
of local onto the stack. Each pass looks at the IR of one function: a value
qualifies only if every use of it, and of the slot it is kept in, is one the
pass knows to be safe. Anything else (a store elsewhere, a return, a copy, a
call it does not know) leaves the value on the heap, so a miss costs speed,
never correctness.

| What | Pass | Creation becomes | Drop becomes | Switch |
|------|------|------------------|--------------|--------|
| Vec header (40 bytes) | `lw_rc_stack_vecs_fn` | `Vec_init_typed`, `Vec_init_filled` | `Vec_drop_inplace` | `LOWER_STACK_VECS` |
| `Vec::filled(n, x)` buffer too | `lw_rc_vbuf_fits`, `lw_rc_stack_vbuf_rewrite` | `Vec_init_filled_at` | `Vec_drop_stackbuf` | `LOWER_STACK_VBUFS` |
| `a + b` string | `lw_rc_stack_strs_fn` | `Str_concat_into` | unchanged | `LOWER_STACK_STRS` |
| Box block | `lw_rc_stack_boxes_fn` | stack area | `__p_mem_drop` | `LOWER_STACK_BOXES` |

Each pass also needs core's entry point (the functions in the "Creation
becomes" column, `Vec_drop_stackbuf` for buffers): with one missing, the pass
does nothing.

## Vec headers

A local `Vec` whose value reaches only core's `Vec_*` functions (argument 1),
the `p_vec_*` builtins (argument 0), or parameters of the program's own
functions that do the same (OP27, `lw_rc_header_safe_params`), keeps its
header in a 5-word stack area. The buffer stays on the heap and grows as
usual. `Vec_drop_inplace` frees the buffer and clears the header, so a loop
that makes the Vec again starts from an empty header.

## Small `Vec::filled` buffers

`Vec::filled(n, x)` keeps its buffer on the stack as well when:

- `n` is a constant above 0;
- the elements are plain values (element type 0, at most a word each: `i64`,
  `f64`, `bool`, `u8`, ...), and `n` of them fit in 512 bytes
  (`LOWER_STACK_VBUF_MAX`);
- every value loaded from its slot goes only to calls that use the buffer in
  place: `Vec_get`, `Vec_set`, `Vec_len`, `Vec_is_empty`, `Vec_contains`,
  `Vec_first`, `Vec_last`, `Vec_capacity`, `Vec_reverse`, `Vec_to_str`,
  `Vec_clone` (a clone is a new heap Vec), `p_vec_get`, `p_vec_len`,
  `p_vec_is_empty`, or its drop.

A push, insert, extend, share, a call of the program's own function, a return
or a store anywhere keeps the buffer on the heap. Such a buffer never grows,
is never shared and is never freed: `Vec_drop_stackbuf` wipes it when it is
sensitive and clears the header.

## Strings

`var s = a + b` (a `p_str_concat` stored in one local) is built in a 128-byte
stack area by `Str_concat_into`, with a literal's header
(`[P_STR_STATIC][len]`). The drops ownership placed stay as they are: a drop
leaves a literal-headed string alone. The result is a counted heap string, as
before, when it is longer than 111 bytes or when `a` or `b` lies in the same
area (`s = "<" + s` in a loop), so a value is never built over itself; the
same drops free it then.

The local may only be read: printed, an argument of `str_len`, `str_equal`,
`str_compare`, `str_find`, `str_contains`, `str_starts_with`,
`str_ends_with`, `str_byte_at`, `str_parse_int` (and their `p_*` forms), an
operand of another `+`, or dropped. A copy (`Str_dup`), a store, a return, a
call of the program's own function, `s = s + x` (which appends in place) or a
second `+` stored in the same local keeps it on the heap. At most 4 strings
per function go on the stack.

## Checking it

```bash
$FID dump-c tests/bootstrap/test_stack_str.fd  | grep -c "Str_concat_into((void"
$FID dump-c tests/bootstrap/test_stack_vbuf.fd | grep -c "Vec_init_filled_at((void"
$FID run tests/selfhost/test_stack_lower.fd      # the passes on hand-built IR
./scripts/leak-check.sh $FID tests/leaks/stack_vbuf_str.fd
```

The output of a program must not change: `test_stack_str.fd` and
`test_stack_vbuf.fd` print the same on every backend whether a value is on the
stack or not.
