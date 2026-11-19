# Operators on structs and units of measure (CL10)

Complaint CL10 ("metres added to seconds"): units of measure checked at
compile time and free at run time. This page describes what `fid` accepts.
The code is in `compiler/typechecker.fd` (section "units of measure") and
`compiler/lower.fd` (`Lowerer_binop_plan`, `Lowerer_lower_dim_operator`).
The dimension arithmetic is in `compiler/ast.fd` (`dim_*`).

## Operators on any struct

`a + b`, `a - b`, `a * b`, `a / b` and `a % b`, where `a` is a struct, call
`a.add(b)`, `a.sub(b)`, `a.mul(b)`, `a.div(b)` and `a.rem(b)`. These are
Rust's names. The method takes `self` and one operand, which need not be
the same struct (`Money * 3`). The result has the method's return type.

```fid
struct P { x: i64, y: i64 }
impl P {
    fn add(self: P, o: P) -> P { return P { x: self.x + o.x, y: self.y + o.y }; }
}
val c: P = a + b;          // a.add(b)
```

- A missing method is an error that names it: ``no operator `-` on struct
  'P': it has no method `sub` ``.
- A method whose parameter does not accept the operand is an error:
  ``Meters.add` takes Meters, got Seconds``.
- **A struct on the right only.** When `a` is not a struct and `b` is,
  `a op b` calls `b.radd(a)`, `b.rsub(a)`, `b.rmul(a)`, `b.rdiv(a)` or
  `b.rrem(a)`, so `4 * money` is `money.rmul(4)`. The right operand is
  evaluated first, because it is the receiver.
- **Compound assignment.** `x += e` on a struct variable is `x = x + e`,
  and the same holds for `-=`, `*=`, `/=` and `%=`. It follows the same
  rules and gives the same errors. A field works too: `body.pos += step`
  is `body.pos = body.pos + step` when `body.pos` is a variable or a field
  path. Before this, a compound assignment on a struct did arithmetic on
  the struct's address.
- `vec2`/`vec3`/`vec4` keep their own operators: `+` and `-` take another
  vec of the same size, and `*` and `/` take a scalar.

## Units: dimensions computed by the compiler

A struct marked `@dimension(...)` is a unit. Its exponents are given over
the SI bases `m`, `kg`, `s`, `A`, `K`, `mol` and `cd`, each between -8 and
7. A bare base means exponent 1. A unit holds exactly one `f64`.

```fid
@dimension(m = 1)                 struct Len    { value: f64 }
@dimension(s = 1)                 struct Time   { value: f64 }
@dimension(m = 1, s = -1)         struct Speed  { value: f64 }
@dimension(m = 1, kg = 1, s = -2) struct Force  { value: f64 }
@dimension(m = 2, kg = 1, s = -2) struct Energy { value: f64 }

val e: Energy = f * d;      // Force * Len: m^2 kg s^-2
val v: Speed  = far / t;    // Len / Time
val r: f64    = ten / two;  // Len / Len cancels: a plain f64
val k: Len    = 5.0 * d;    // an f64 scales a unit from either side
val hz: Freq  = 1.0 / half; // f64 / unit: the inverse unit
```

The rules:

| Expression | Result |
|---|---|
| `unit * unit`, `unit / unit` | the unit whose dimension is the sum (difference) of the operands' dimensions; an `f64` when they cancel |
| `unit * f64`, `unit / f64`, `f64 * unit` | the same unit |
| `f64 / unit` | the unit of the inverse dimension |
| `unit + unit`, `unit - unit`, `unit % unit` | the same unit, only when both are the same unit |
| `-unit` | the same unit, its value negated |
| `unit < unit` (`<=`, `>`, `>=`) | `bool`, comparing the values; both must be the same unit |
| `unit == unit` | `bool`, by content as on any struct (CL9) |

No function has to be written for each pair of units. At run time a unit
is its `f64`: `f * d` lowers to `Energy { value: f.value * d.value }`.

Errors:

- Units of different dimensions do not add: ``units of different
  dimensions (m and s) do not add``.
- No unit declares the result's dimension. The error names the dimension
  and the annotation to write, for example `@dimension(m = 1, kg = 1, s = -1)`.
- More than one unit declares it (energy and torque are both
  m^2 kg s^-2). The result is then ambiguous. Give the left unit a `mul`
  (or `div`) method that says which unit it makes.
- An integer combined with a unit (`2 * d`). Write `2.0 * d`.
- A unit compared with another unit or a number: ``a unit compares only
  with a value of its own unit (Len)``.
- A malformed annotation (a name that is not an SI base, a base given
  twice, an exponent out of range), or a unit that does not hold exactly
  one `f64`.

A method still wins when it takes the operand. `Area.mul(self, k: f64)`
serves `a * 2.0`, and `a * len` still uses the dimension rule. A unit's
own `add` that takes something else keeps its error.

## Tests

- `tests/bootstrap/test_frontend_struct_ops.fd`, `_fail`: operator methods.
- `tests/bootstrap/test_frontend_units_ops.fd`, `_fail`: `stdlib/units.fd`'s
  impls.
- `tests/bootstrap/test_frontend_units_dims.fd`: the dimension rule,
  scaling from either side, the inverse unit, a fitting method, `rmul`.
- `tests/bootstrap/test_frontend_units_dims_{add,ambiguous,annot,field,int}_fail.fd`
  and `test_frontend_units_dims_fail.fd`: the errors above.
- `tests/bootstrap/test_frontend_struct_compound.fd`, `_fail`,
  `test_frontend_units_compound_fail.fd`: compound assignment on struct
  variables and fields.
- `tests/bootstrap/test_frontend_units_compare.fd`, `_fail`: comparisons
  and negation of units.

## Not done

- `stdlib/units.fd` does not mark its units with `@dimension` yet. Its
  derived-unit functions (`travel`, `force`, ...) still do that work.
- Ordering on structs that are not units (`p < q`) is not an operator.
  It still compares addresses: write a `cmp` method (CL9).
- Generic structs: an operator on `Pair<T>` is not looked up.
