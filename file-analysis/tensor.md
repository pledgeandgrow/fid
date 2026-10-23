# tensor.fd

- **Path:** `compiler/tensor.fd`
- **LOC:** 755 (Rust `src/tensor.rs`: 609)
- **Role:** Pure-Fides n-dimensional tensor library — `Tensor` (shape/strides/data), elementwise ops, matmul/dot, reductions, activations (relu/sigmoid/softmax), **Q8 quantization** (ML10: `quantize_q8`/`dequantize_q8`/`find_quant_params`), `to_json`, and `KvCache` with sliding-window eviction (ML9). Phase 5C.1 AI/ML foundation.

## Where called

**WIRED since 2026-10-23** — the suggested fix order item #1 was done.

| Evidence | Detail |
|---|---|
| `compiler/interp.fd` | `p_tensor_*`/`p_quantize_*`/`p_kv_*` builtins now call the real `Tensor_*`/`KvCache_*` functions — the `:2701` block header reads "wired to compiler::tensor", e.g. `p_tensor_zeros` → `Interpreter_alloc_tensor(Tensor_zeros(shape))` (:2703-2707). ~35 call sites implemented |
| `compiler/main.fd` | `use compiler::tensor` — no longer vestigial (interp pulls the module transitively) |
| `runtime_linux.fd` | 9 `p_tensor_*` xor/ret stubs — **still stubbed**: compiled (ELF) binaries still can't reach real tensors. Only `fides-c run` (interp) works |

Rust uses `crate::tensor::Tensor`/`KvCache` directly inside `src/interp.rs` (~20 call sites) — the Fides interp now does the equivalent via `Interpreter_alloc_tensor` wrapping `Value::Ptr` handles.

## Findings

### MEDIUM → RESOLVED on interp path (2026-10-23) — the port now executes under `run`

The ML9+ML10 port itself was verified faithful and is unchanged — what changed
is that `fides-c run` can now reach it through the wired `p_tensor_*` builtins.
Remaining gap: the **native/ELF path** still has xor/ret `p_tensor_*` stubs in
`runtime_linux.fd`, and `codegen_c`'s C runtime has its own tensor story — a
program using tensors works under `run` but not yet under `build`/`emit` (ELF).

Verified per-function parity:

- **`matmul`/`dot`**: identical O(m·n·k) triple loop through `get`/`set`, same 2D/inner-dim guards (Fides error strings are shorter — no dimension echo).
- **`KvCache`**: all six methods (`new`/`append`/`get`/`len`/`clear`/`evict_oldest`) ported — the sliding-window eviction (ML9) logic present.
- **Quantization**: `quantize_q8`/`dequantize_q8`/`find_quant_params` + `QuantParams` — the ML10 round-trip (min/max → scale/zero-point → clamp [−128,127] → back).
- **Activations**: relu/sigmoid/softmax (max-subtracted stable softmax via `tensor_exp`), sum/mean/max/min reductions, transpose, reshape.
- **`to_json`/`to_string`** formatting ported.

Because nothing called it, tensor builtins on all three live paths used to return
garbage-free-but-empty defaults. **Status now:** interp path → real values;
ELF path → still xor/ret `0` (`runtime_linux` stubs). `Vec<f64>` "tensor"
programs work under `run` but still silently zero under native ELF builds.

### LOW — `I64Vec` shim struct

`pub struct I64Vec` (:575) exists to box `Vec<i64>` where nested generics were needed — boilerplate artifact, harmless.

## Suggested fix order — updated

1. ~~Wire `interp.fd`'s `p_tensor_*`/`p_quantize_*`/`p_kv_*` builtins to `Tensor_*`/`KvCache_*`~~ — **DONE (2026-10-23)**.
2. `runtime_linux.fd`'s `p_tensor_*` stubs still need real impls for compiled binaries (bigger lift — the same missing-runtime story as `p_string_*`/`p_map_*` — which are *also* dropped, see runtime_linux.md).
3. Functional check: run a tensor/matmul/kv-cache program under `fides-c run` to confirm the `Value::Ptr`-based `Interpreter_alloc_tensor` path returns correct values (wiring is code-reviewed, not runtime-verified here).

## Verification

Inspection-only — no self-hosted binary; and moot anyway since the module has no reachable entry point. Parity verified by function-signature and algorithm diff against `src/tensor.rs`; deadness verified by exhaustive grep for `Tensor_`/`tensor` across `compiler/` and `stdlib/` (only the import line and this file itself match).

## Post-verification addendum (runtime check)

Runtime probing after A3 wiring surfaced a defect the static pass missed — one
that made the entire module unusable regardless of callers:

### HIGH (fixed) — `Tensor` struct name collides with the builtin type

`Tensor` is a **language-builtin type name**: `lower.fd:1195` special-cases it
alongside `Vec`/`HashMap`/`Allocator` (resolves to `IrType::Opaque("Tensor")`),
and `typechecker.fd:5081` matches `Type::Named("Tensor")`. So every `self:
&Tensor` / `: Tensor` annotation in this module bound to the *builtin opaque
type*, not the local struct. Result: direct field access on a value worked
(`t.shape.len() == 2`), but any `&Tensor` method arg saw all Vec fields as
empty (`Tensor_ndim(&t) == 0`, `Tensor_sum(&t) == 0`, matmul "requires 2D
tensors"). The struct could never pass through its own API — a port-level
defect invisible while the module was dead.

Minimal reproduction: identical-shape struct `PHolder3{a,b,c: Vec}` through `&`
works; `Tensor` through `&` reads zeros, under both local and cross-module
imports. Confirmed not a general struct/Vec/borrow bug.

**Fix applied:** struct renamed `Tensor` → `TensorData` (all 42 sites in
tensor.fd + 9 type refs in interp.fd). Verified live under `fidesc run`:
filled(2x2,1.5)+set(0,0,3) → ndim=2, size=4, get=3, sum=7.5, matmul(0,0)=11.25,
KvCache_len=1 — all correct.

### Caveat on the interp handle path

`p_tensor_*` extern args arrive as `Value::Ptr` handles into `interp.tensors`
(side table of `TensorData`). The side-table ops are typechecked and the callee
side is runtime-verified; the full `interpret_source` path through the Fides
interpreter is verified structurally but the meta-interpreting run is too slow
to have completed in-session.
