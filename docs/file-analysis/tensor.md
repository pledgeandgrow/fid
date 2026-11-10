# tensor.fd

- **Path:** `compiler/tensor.fd` — module `compiler::tensor`
- **LOC:** 757 (2026-11-09). Rust `src/tensor.rs`: 609.
- **Role:** the interpreter's tensor library: `TensorData` (shape, strides,
  `Vec<f64>` data), creation, indexing, elementwise ops, matmul/dot,
  reductions, relu/sigmoid/softmax, Q8 quantization helpers, JSON/string
  output, and `KvCache` (ML9). Pure Fides, no builtins of its own.
- **Status:** LIVE on `fid run` only (through interp.fd's `p_tensor_*`,
  `p_kv_cache_*`, `p_find_quant_params`, autograd). The C and Windows
  backends have their own tensor runtimes and do not use this file.
- **Re-audited:** 2026-11-09.

## Where it is called

| Caller | Site | What |
|---|---|---|
| `compiler/interp.fd` | :27 import; `tensors: Vec<TensorData>`, `kv_caches: Vec<KvCache>` (:819-822) | `call_tensor` (:7039) and the `p_tensor_*` block (~:3950-4300) call `Tensor_*` / `KvCache_*`; `Interpreter_alloc_tensor` (:5414) wraps a `TensorData` as `Value::Ptr` |
| `compiler/main.fd` | :30 | `use compiler::tensor`: nothing in main.fd uses it (interp pulls it in anyway) |

Reached from: `fid run` (and `fid veil`, which runs the interpreter).

Other tensor implementations (not this file): the C runtime in
`compiler/codegen_c.fd` (112 `p_tensor_` references), `stdlib/pe_runtime/
tensor.fd` (Windows `emit`), `stdlib/tensor.fd` (the user-facing wrapper).
`stdlib/linux_runtime/` has no `p_tensor_*`. The old
`compiler/runtime_linux.fd` still registers `p_tensor_*` symbols (:3110+).

Unused public functions (no caller outside this file):
`Tensor_quantize_q8`, `Tensor_dequantize_q8`, `Tensor_to_string`
(interp.fd has its own `interp_quantize_q8`, `interp_dequantize_q8`,
`interp_tensor_to_string`, :7340-7372). `Tensor_sigmoid`, `Tensor_softmax`,
`Tensor_to_json` are called only from interp.fd handlers that cannot be
reached (:4135-4144, :4288; `call_tensor` answers those names first,
:3096), so in practice also unused.

## Findings

### Verified live (2026-11-09, `s18.exe run`)

1. **FIXED (was MEDIUM) — module unreachable.** Wired through interp.fd
   since 2026-10-23. `tests/bootstrap/test_tensor.fd`, `test_kv_cache.fd`,
   `test_quantization.fd`, `test_tensor_shape_vec.fd` run to exit 0 with
   plausible output (matmul `[[58, 64], [139, 154]]`, transpose, JSON).
2. **FIXED (was HIGH) — `Tensor` struct name clashed with the builtin type.**
   Renamed `TensorData`; still so.

### From code inspection

3. **NEW, MEDIUM — `tensor_exp` is a 12-term Taylor series** (:735-747):
   inaccurate for |x| > ~5, returns `1e8` for x > 20 and 0 for x < -20.
   It feeds `Tensor_sigmoid` and `Tensor_softmax`. Not visible today
   because interp.fd uses `math_exp` versions instead (:7219-7262, comment
   says why); anyone calling these two directly gets wrong values.
4. **NEW, LOW — `Tensor_quantize_q8` truncates** (`p_float_to_int`, :439);
   Rust rounds (`src/tensor.rs` `(v / scale).round()`). Unused (interp
   rounds in its own copy).
5. **NEW, LOW — no per-dimension bounds check.** `Tensor_get/set` (:121-135)
   flatten the index with the strides and only check the flat offset, so
   `[0, 5]` on a 2x3 tensor reads element `[1, 2]`; a fully out-of-range
   index returns 0.0 / ignores the write silently.
6. **NEW, LOW — empty tensors.** `Tensor_max`, `Tensor_min`,
   `Tensor_find_quant_params` read `data.get(0)` without a length check
   (:339, :350, :468).
7. **NEW, INFO — `KvCache_get` builds a `Vec<i64>` of f64 bit patterns and
   passes it as the `Vec<f64>` data of a tensor** (:666-681). It works
   because a Vec stores 8-byte words either way, and it type-checks under
   the lenient checking of `compiler::*` modules.
8. **STILL TRUE, LOW — `I64Vec` wrapper** (:577) exists only to avoid a
   nested-generic parse problem; harmless.
9. **CHANGED — "runtime_linux p_tensor stubs".** The native Linux backend
   is now `nat_ir` + `stdlib/linux_runtime/`, which has no `p_tensor_*`
   at all (grep). A tensor program built with `emit --target *-linux` has
   unresolved builtins (not run here).

## Fix order

1. Replace `tensor_exp` with `math_exp`, round in `Tensor_quantize_q8`,
   and delete interp.fd's duplicates and dead handlers so there is one
   implementation.
2. Bounds-check each index against `shape`; length-check the reductions.
3. Remove the unused `use compiler::tensor` from main.fd.

## Verification state

- 2026-11-09: whole file read; callers by grep; four repo tensor tests run
  under `fid run`. Output not compared against the C backend.
