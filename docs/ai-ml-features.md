# P for AI/ML — Feature Opportunities

> Mapping P's unique capabilities to real problems in AI/ML systems
> programming, based on 2024-2026 research and industry trends.

## The Problem Space (2024-2026)

### 1. GPU Memory Safety is a Crisis

A 2026 study (Model2Kernel, arxiv 2603.24595) found **353 memory-safety
bugs** in CUDA kernels from popular LLM inference systems. Out-of-bounds
accesses corrupt model weights, crash inference services, and enable
adversarial attacks. Current CUDA/OpenCL/HIP are all C++ dialects with
zero memory safety.

> "Memory-safety bugs in CUDA kernels can lead to severe consequences.
> Out-of-bounds accesses and data races often trigger 'illegal memory
> access' errors on the GPU, potentially crashing the entire inference
> service." — Model2Kernel, 2026

**P's advantage:** P already has compile-time region-based memory safety
on CPU. Extending regions to GPU memory would eliminate this entire class
of bugs — at compile time, with zero runtime cost.

### 2. KV Cache Management is Manual and Error-Prone

vLLM's PagedAttention (SOSP 2023, 2-4x throughput improvement) is
essentially **manual region-based memory management for GPU memory**.
Developers manually page KV cache blocks in and out, track which
blocks belong to which request, and free them when conversations end.
This is exactly what P's regions do — but P does it with compile-time
safety, while vLLM does it with manual C++ code that can leak or
double-free.

**P's advantage:** Region-scoped KV cache. Allocate KV cache in a region,
free it automatically when the conversation ends. Compile-time guarantee
that no references escape. No manual paging code, no leaks.

### 3. Mojo is the Only Competitor — and It's AI-Only

Mojo (Modular, 2024-2026) is the only language seriously targeting AI/ML
systems programming. It has:
- `TileTensor` — memory layout as a compile-time property
- GPU kernel programming with safety
- MLIR-based compilation
- Python interop

But Mojo is **AI-niche**. It's not a general systems language. It depends
on MLIR (LLVM infrastructure). It has no capability system, no effect
system, no hot reload, no custom toolchain.

**P's advantage:** P is a general systems language that can also do AI/ML.
P doesn't need MLIR. P has capabilities + effects + regions + comptime +
hot reload — none of which Mojo has.

### 4. Model Serving Needs Hot Reload

Current model serving (vLLM, TGI, SGLang) requires restarting the server
to update model code. This means downtime, lost KV cache, dropped
connections. The industry workaround is blue-green deployments — running
two servers and switching, wasting GPU memory.

**P's advantage:** P already has hot reload with `persist`. You can
reload model code while keeping KV cache and connection state alive.
This is impossible in Python/C++/Rust without massive infrastructure.

### 5. Edge AI Needs Memory Guarantees

Running models on embedded devices (microcontrollers, IoT) requires
proving the model fits in SRAM. Currently this is done by running the
model and hoping it doesn't OOM, or by manual calculation.

**P's advantage:** `region r with budget(N)` already proves memory
budgets at compile time. Extending this to GPU shared memory and edge
device SRAM gives a compile-time guarantee that a kernel or model fits.

---

## Proposed Features for AI/ML

### ML1 — Region-Scoped GPU Memory

**Status:** Not started
**Effort:** ~500 lines (gpu.rs + pir.rs + codegen)
**Impact:** Critical — eliminates GPU memory leaks and use-after-free

```
// GPU memory tied to a region — freed when region exits
region gpu r {
    let kv_cache = gpu_alloc::<f32>(batch * seq_len * dim);
    let activations = gpu_alloc::<f32>(batch * hidden_dim);

    // ... run attention ...

    // No manual free — region exit frees all GPU memory
    // Compile-time guarantee: no references to kv_cache escape
}
```

**Why it's novel:** No GPU programming language has region-based memory
management. CUDA, OpenCL, HIP, Triton, Mojo — all use manual allocation
or garbage collection. P's regions would give compile-time safety with
zero runtime cost.

**Why it matters:** The Model2Kernel study found 353 memory bugs in
production CUDA kernels. P's regions would eliminate these at compile
time. vLLM's PagedAttention is manual region management — P makes it
automatic and safe.

### ML2 — comptime Tensor Shape Verification

**Status:** Not started
**Effort:** ~300 lines (comptime.rs + typechecker.rs)
**Impact:** High — eliminates the #1 runtime error in ML

```
comptime pf verify_shapes(input: TensorShape, weight: TensorShape) -> bool {
    input.cols == weight.rows
}

pf linear(input: Tensor<f32, [batch, in_dim]>, weight: Tensor<f32, [in_dim, out_dim]>)
    -> Tensor<f32, [batch, out_dim]>
    where comptime verify_shapes(input.shape(), weight.shape())
{
    // Shape mismatch is a COMPILE ERROR, not a runtime crash
    matmul(input, weight)
}
```

**Why it's novel:** Mojo has dependent types for shapes, but they're
rigid. P's comptime can run arbitrary shape verification logic at compile
time — any function, any condition. More flexible than Mojo's type system.

**Why it matters:** Shape mismatches are the most common runtime error in
PyTorch/TensorFlow. They waste GPU time (the error happens after kernel
launch) and are hard to debug. P catches them at compile time.

### ML3 — Effect-Traced Autodiff

**Status:** Not started
**Effort:** ~800 lines (typechecker.rs + new autodiff.rs)
**Impact:** High — compile-time verification of differentiability

```
// Mark operations as differentiable
effect Diff;  // new effect: "this operation supports autodiff"

pf matmul(a: Tensor, b: Tensor) -> Tensor !{Diff, Alloc} {
    // Differentiable matmul
}

pf relu(x: Tensor) -> Tensor !{Diff} {
    // Differentiable activation
}

pf load_model(path: String) -> Model !IO {
    // NOT differentiable — has IO effect
}

// The compiler verifies: is this model differentiable?
pf forward(model: Model, input: Tensor) -> Tensor !{Diff, Alloc} {
    let h1 = matmul(input, model.w1);  // !{Diff, Alloc} — OK
    let a1 = relu(h1);                 // !{Diff} — OK
    let h2 = matmul(a1, model.w2);     // !{Diff, Alloc} — OK
    h2
    // Compiler proves: forward has !{Diff} effect → differentiable
}

// This would be a COMPILE ERROR:
pf bad_forward(model: Model, input: Tensor) -> Tensor !{Diff} {
    let m = load_model("weights.bin");  // ERROR: load_model has !IO, not !{Diff}
    matmul(input, m.w1)
}
```

**Why it's novel:** No language uses effects for differentiability
tracking. PyTorch/JAX trace at runtime. Mojo doesn't have effects. P's
effect system can prove differentiability at compile time.

**Why it matters:** In production ML, you need to know if a model is
differentiable (for training) or not (for inference). Currently this is
discovered at runtime. P makes it a compile-time property.

### ML4 — Hot-Swappable Model Weights via persist

**Status:** Partially implemented (persist works for scalars)
**Effort:** ~400 lines (extend persist to tensors + GPU memory)
**Impact:** Critical — zero-downtime model updates

```
pf serve_model() {
    // Model weights persist across hot reloads
    persist pv weights: Tensor<f32, [4096, 4096]> = load_weights("v1.bin");
    persist pv kv_cache: KVCache = KVCache::new();

    // Inference loop
    loop {
        let request = accept_request();  // !IO
        let output = forward(weights, request, kv_cache);
        send_response(output);  // !IO
    }
}

// Hot reload: change forward() implementation
// weights and kv_cache survive the reload
// No downtime, no lost state, no re-loading weights from disk
```

**Why it's novel:** No ML framework has language-level state persistence
across hot reloads. vLLM/TGI/SGLang all require full restart. P's
`persist` + hot reload makes this a language feature.

**Why it matters:** Model serving downtime is expensive. A 7B model
takes 30+ seconds to load weights. With P, you can update the inference
code while keeping weights in memory — zero downtime.

### ML5 — sandbox { } for Untrusted Model Plugins

**Status:** ✅ Implemented (sandbox { } works today)
**Effort:** 0 (already done)
**Impact:** High — safe model marketplaces, federated learning

```
// Load a custom activation function from an untrusted source
pf run_custom_activation(plugin: extern fn(Tensor) -> Tensor, input: Tensor) -> Tensor {
    sandbox {
        // Plugin CANNOT do IO, Net, or Spawn
        // It can only compute on the input tensor
        plugin(input)
    }
    // If plugin tries to call print() or socket(), it's a COMPILE ERROR
}
```

**Why it's novel:** No ML framework can sandbox custom model code at
compile time. PyTorch lets custom ops do anything. ONNX Runtime has
runtime sandboxing (slow). P does it at compile time — zero runtime cost.

**Why it matters:** Model marketplaces (HuggingFace, etc.) distribute
custom model code that runs with full privileges. A malicious model
could exfiltrate data, make network calls, or spawn processes. P's
`sandbox { }` proves at compile time that a plugin can only compute.

### ML6 — Compile-Time SRAM Budget for Edge AI

**Status:** ✅ Implemented (region with budget works today)
**Effort:** ~200 lines (extend to GPU shared memory)
**Impact:** High — embedded AI, microcontroller deployment

```
// Prove at compile time that a kernel fits in GPU shared memory
region shared with budget(48 * 1024) {  // 48KB SRAM on this GPU
    let tile_a = shared_alloc::<f32>(16 * 16);   // 1KB
    let tile_b = shared_alloc::<f32>(16 * 16);   // 1KB
    let tile_c = shared_alloc::<f32>(16 * 16);   // 1KB
    // 3KB used, 45KB remaining — COMPILE-TIME GUARANTEE
    // If you add too many tiles, it's a compile error, not a runtime crash
}
```

**Why it's novel:** No language can prove GPU shared memory budgets at
compile time. CUDA developers manually calculate SRAM usage and hope
they got it right. P's `region with budget(N)` makes it a compile-time
guarantee.

**Why it matters:** Edge AI (running models on microcontrollers, IoT
devices) has strict memory limits. Exceeding SRAM causes a runtime crash
on the device. P catches this at compile time, before deployment.

### ML7 — Capability-Aware Inference Stripping

**Status:** ✅ Implemented (--caps flag works today)
**Effort:** 0 (already done)
**Impact:** Medium — smaller inference binaries, proven attack surface

```
// Build an inference-only binary — no training code
pledgec build model.p --caps "IO" -o inference_server

// The compiler strips:
// - All gradient computation functions (require Alloc + Diff, not granted)
// - All training loop functions (require IO + Spawn for data loading)
// - All checkpoint save functions (require IO for file writes)
// Only inference code remains — smaller binary, proven attack surface
```

**Why it's novel:** No ML framework strips training code from inference
binaries. PyTorch ships the entire autodiff engine in every deployment.
P's `--caps` flag removes it at link time.

**Why it matters:** Production inference servers ship with full training
capability — a security risk. An attacker who compromises the server can
modify gradients or extract training data. P's capability-aware linking
strips this code at compile time.

### ML8 — comptime Kernel Specialization

**Status:** Partially implemented (comptime works, GPU codegen exists)
**Effort:** ~300 lines (connect comptime to GPU codegen)
**Impact:** High — zero-cost kernel specialization

```
// Specialize a GEMM kernel for exact shapes at compile time
comptime pf gemm_kernel(M: usize, N: usize, K: usize) -> String {
    // Generate a kernel string specialized for these exact dimensions
    f"""
    pf gemm_{M}_{N}_{K}(a: Tensor<f32, [{M}, {K}]>,
                        b: Tensor<f32, [{K}, {N}]>)
        -> Tensor<f32, [{M}, {N}]> !{Alloc} {{
        // Unrolled loops for exact dimensions — no loop overhead
        // Register allocation optimized for these shapes
        // ...
    }}
    """
}

// The compiler generates a specialized kernel at compile time
comptime gen gemm_kernel(128, 128, 64);
// Now gemm_128_128_64 exists — fully specialized, zero overhead
```

**Why it's novel:** Mojo has comptime specialization, but P's `comptime
gen` can parse and inject arbitrary P source code. More flexible than
Mojo's metaprogramming. torch.compile does this at runtime; P does it at
compile time.

**Why it matters:** GPU kernel performance depends heavily on exact
shapes. Generic kernels are 2-5x slower than specialized ones.
torch.compile specializes at runtime (first-call overhead). P specializes
at compile time — zero runtime overhead.

---

## Priority for AI/ML Features

```
ML5: sandbox { } for plugins     ✅ DONE — immediate value for model marketplaces
ML7: --caps inference stripping  ✅ DONE — immediate value for production deployment
ML6: SRAM budget for edge AI     ✅ DONE — immediate value for embedded deployment

ML1: Region-scoped GPU memory    ← HIGHEST PRIORITY — solves the #1 GPU bug class
ML4: Hot-swap model weights      ← HIGH — zero-downtime serving killer feature
ML2: comptime shape verification ← HIGH — eliminates #1 ML runtime error
ML8: comptime kernel specialization ← MEDIUM — performance, not safety
ML3: Effect-traced autodiff      ← MEDIUM — nice to have, not critical
```

## The Story

> P is the only language where you can:
> - Allocate GPU memory in a region and prove it's freed at compile time
> - Verify tensor shapes at compile time, not runtime
> - Prove a model is differentiable via effect tracking
> - Hot-reload model code while keeping weights and KV cache alive
> - Sandbox untrusted model plugins at compile time
> - Prove a kernel fits in SRAM at compile time
> - Strip all training code from an inference binary
> - Specialize GPU kernels for exact shapes at compile time
>
> No other language — not Python, not C++, not Rust, not Zig, not Mojo —
> can do ALL of these. P's unique combination of regions + capabilities +
> effects + comptime + hot reload + custom toolchain makes this possible.
