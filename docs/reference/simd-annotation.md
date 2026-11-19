# `@simd` on every path (K10)

`@simd` on a function asks for its loops to be vectorized. It never
changes what the function computes: every path prints the same output, and
an overflow panics where it would without the annotation.

| Path | What `@simd` does |
|------|-------------------|
| `fid run` (interpreter) | nothing: the loops run as written |
| C (`ccompile`, `dump-c`) | the definition gets `P_SIMD_FN`: under GCC `__attribute__((optimize("tree-vectorize","vect-cost-model=unlimited")))`, the relaxed cost model fidesc's `@simd` asks for; nothing under clang, `zig cc`, tcc or MSVC (compiler/codegen_c.fd) |
| machine layer (`fid emit` for Linux x86-64, AArch64, RISC-V with V, `--nat` Windows, macOS) | the loop vectorizer (docs/reference/loop-vectorization.md) takes more loops and unrolls them, below |
| pe_ir (the default Windows `emit`) | nothing: pe_ir does not vectorize |

## The machine layer

The code is in compiler/nat_ir.fd, section "OP22: automatic loop
vectorization": `NgVz.simd` (set in `ng_vz_try` from
`IrFunction.is_simd`), `ng_vz_simd_kind`, `ng_vz_scan`, `ng_vz_guard` and
`ng_vz_body`.

Outside `@simd` the vectorizer leaves a loop with a checked i64 `+` or `-`
alone: lanes added two or four at a time would wrap where the scalar loop
panics. In a `@simd` function it also takes

- a checked i64 `+` or `-` whose operands are element reads `v.get(i)` /
  `v[i]` or invariants (`c[i] = a[i] + b[i]`, `v[i] = v[i] - k`);
- a checked `s = s + v.get(i)` reduction into an i64 local.

and proves before the vector loop, at run time, that none of them can
overflow (`ng_vz_guard`):

- each invariant operand is in [-2^31, 2^31);
- for the reduction, `s` starts in [-2^61, 2^61) and fewer than 2^30
  iterations are left;
- a vector pre-scan over each Vec whose elements feed one of them (two
  lanes at a time: `x + 2^31` or-ed into one vector, whose high 32 bits are
  tested once at the end) finds every element in [-2^31, 2^31).

Then the sum or difference of two operands stays within 2^32, and any sum
of the reduction's elements, in any order, within 2^62: the wrapping
vector operations give exactly what the checked ones would, and none of
those would have panicked. When a check fails the vector loop is skipped
and the loop runs as written, so it panics on the same element with the
same elements written before it (tests/nattools/simd_ovf_*.fd). The
pre-scan reads what the loop reads: the loop reads each element at index
`i` only, before it writes it.

The vector loop of a `@simd` function is unrolled twice: two vectors a
trip (the second at displacement w), then one vector a trip for one that
is left, then the scalar tail.

Left as before, also under `@simd`: an `f64` sum (adding in lanes changes
the rounding; the C path does not reassociate either, and the outputs must
stay the same), an i64 `*` (no 64-bit vector multiply before AVX-512), a
checked `+` of computed lanes (`s = s + (a[i] ^ b[i])`), loops with `if`
in the body, `Vec_push`, the counter in the arithmetic. So the four loops
of tests/bootstrap/test_simd_annotation.fd (`*`, `push`, `s + i`, `if`)
stay scalar natively; test_simd_native.fd has the ones that are taken.

## Switches and dump

- `FID_NAT_OFF=vzsimd`: `@simd` changes nothing on the machine layer.
- `FID_NAT_OFF=vzunroll`: the `@simd` relaxations stay, the unrolling goes.
- `FID_NAT_OFF=vectorize`: no loop is vectorized at all.
- `fid emit --stats` (or `FID_NAT_ON=vzdump`): the `[vectorize]` line of a
  `@simd` loop ends with what `@simd` added:

  ```
  [vectorize] sum_i: bb1 vectorized: 16-byte vectors (2 lanes), 1 reads, 0 writes, 0 splats, reduction +, @simd: 1 checked (pre-scan of 1 Vecs, 0 invariants), unrolled x2
  [vectorize] add_into: bb1 vectorized: 16-byte vectors (2 lanes), 2 reads, 1 writes, 0 splats, @simd: 1 checked (pre-scan of 2 Vecs, 0 invariants), unrolled x2
  [vectorize] sum_plain: bb1 not vectorized: a reduction other than ^ | & or a wrapping +
  ```

## Tests

- tests/bootstrap/test_simd_annotation.fd: four `@simd` functions, the same
  output on every path.
- tests/bootstrap/test_simd_native.fd: the loops `@simd` lets in (a checked
  sum, elementwise `+` and `-`, an invariant operand, a Vec written in
  place, a `for` loop), the same loop without `@simd`, an `f64` sum under
  `@simd`; lengths 0..9, 33 and 1001; elements and an invariant past 2^31
  (the pre-scan skips the vector loop). The expected output is `fid run`'s
  (the same through ccompile).
- tests/nattools/simd_native_check.sh `<fid> [runs]`: run and ccompile of
  both tests; FID_VERIFY=1 builds for SSE2, AVX2, NEON, RVV and `--nat`;
  the `[vectorize]` lines (taken, checked, unrolled; the plain loop and the
  f64 sum not); 4 `vld` in sum_i's MIR (2 with `FID_NAT_OFF=vzunroll`);
  the images' output, also with `FID_NAT_OFF=vzsimd` and `vzunroll`; the
  three overflow programs panic natively as under `fid run`; the speed of
  tests/nattools/simd_bench.fd (`@simd`, `vzsimd` off, `vectorize` off: the
  same output, `@simd` not slower) and of test_simd_annotation.fd.

## Numbers

Not measured yet: written before the compiler with it was built. Run
tests/nattools/simd_native_check.sh and put its bench table here.

## Left

- AVX2 for a loop with a reduction (lane extraction reads lanes 0 and 1
  only), `f64` reductions under a flag that allows reassociation, an i64
  `*`, checked operations on computed lanes (a range analysis through
  `& | ^` and nested `+`), a runtime-dispatched AVX2 path without `--cpu`.
- pe_ir (the default Windows `emit`) and the interpreter ignore `@simd`.
