# Sharing generic instances (WP19)

A generic function or method is compiled once per set of type arguments
(monomorphization, `compiler/mono.fd`). Two ways keep the compiler from
building or emitting the same body twice.

## At monomorphization: the key is the lowered type

`mono.fd` names an instance after its type arguments (`pick__i64`,
`Wrap__get__i64`). Since WP19 those arguments are first put into the form
lowering sees (`mono_share_type`, which mirrors `lower.fd`
`Lowerer_resolve_type`): a type alias is replaced by its target, also inside
a reference, a raw pointer, an array and a generic's arguments. So with

```fid
type Meters = i64;
fn count_of<T>(v: Vec<T>) -> i64 { return v.len(); }
```

`count_of(v)` for `v: Vec<Meters>` and for `v: Vec<i64>` both call
`count_of__i64`, built once. Lowering could not tell the two bodies apart:
the IR type, the ownership kind (`Lowerer_rc_kind`) and `type_name_of` all
go through `Lowerer_resolve_type`.

What is not shared here:

- an alias that has an `impl` or a trait impl of its own name (a method call
  on `T` may resolve through that name);
- the instances of a function or method with L8H specializations
  (`instantiate` and `mono_method_inst` choose among them by the spelled
  types);
- a tuple's or a function type's parts (lowering does not resolve aliases
  inside them either);
- distinct types with the same layout (two structs with the same fields):
  their bodies differ in names (field lookup, printing). The IR-level fold
  below merges what is still identical after lowering.

Switches: `FID_MONO_SHARE_OFF=1` keys instances by the spelled types (the
old behaviour); `FID_MONO_STATS=1` prints
`mono: N instance key(s), M built, K shared by lowered type`.

Check: `bash tests/mono_share/check.sh $FID`
(`tests/bootstrap/test_mono_share_alias.fd`).

## After lowering: identical functions folded (`icf`)

`compiler/iropt.fd` `iro_fold_identical` sends every direct call to a
function whose serialized IR equals an earlier function's to that earlier
one (opt-in `FID_IROPT_ON=icf` until a suite run shows the same output with
it on and off: `tests/tooling/midend-optin-check.sh` with `IME=icf`).
