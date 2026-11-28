# AI/ML Roadmap — Train in PyTorch, Compile and Serve with Fides

> Goal: a Python team keeps PyTorch for research and hands Fides the part
> Python is bad at — a model that runs as one small, fast, checkable native
> binary on x86-64, ARM64, RISC-V, Windows, WebAssembly and bare metal — and
> then, once the numerics are real, finds that training in Fides works too.
> People do not leave PyTorch for a nicer syntax. They leave a slow or fat
> deployment, or they rewrite one hot spot behind a Python call. This
> roadmap is built on that: sections A to E are the way in, F is what
> Python's ML stack lacks, G to I grow from it.
>
> Created Nov 27 2026, at the user's request ("make the language better,
> more unique and useful for AI and machine learning, good enough to switch
> from Python TensorFlow or PyTorch"). Every "Today" line below was read in
> the tree or run that day. 100 goals, AI1 to AI100.
>
> Not in scope: parity with pandas (AI84 is the common core), a
> TensorFlow-style graph API, training past what one machine's memory holds
> (AI67 stops at a few processes and machines), convolutions of cuDNN's
> class.
>
> Outside the 1.0.0 gate. `release-roadmap.md`'s tracks A to N are the gate;
> no goal below is in it unless the user moves one there. `roadmap.md`'s
> Phase 6 ("AI/ML — complete") describes the fidesc-era builtins; for `fid`
> this file replaces that claim, and `docs/reference/positioning.md` §6 is
> corrected by AI100.
>
> Markers: `[ ]` open, `[~]` partly done, `[x]` done. Sizes are judgments,
> not measurements: S days, M weeks, L months, XL a year or more. Each goal
> says what checks it.
>
> Words used in every entry:
> - **The oracle**: reference values that Python computes (NumPy, PyTorch,
>   onnxruntime, scikit-learn, mpmath), kept in `tests/ml/oracle/` (AI1).
> - **The three backends**: `run`, `ccompile` and `emit`, through
>   `python scripts/verify/expect.py --fid <fid> --backends
>   run,ccompile,emit <test>`; a goal that names the Linux targets adds
>   `--nat`.
> - **Within x**: the largest absolute difference over all elements is at
>   most x, unless the goal says relative.
> - **Done means proved** (goal-workflow.md rule 4): a dated note in the
>   entry with the numbers and the binary that gave them.

## The state it starts from (Nov 27)

| Piece | Where | State |
|---|---|---|
| Tensors | `stdlib/tensor.fd` (160 lines), `compiler/codegen_c.fd`, `compiler/interp.fd`, `stdlib/pe_runtime/tensor.fd` | 27 functions over one builtin type: a flat f64 array with a shape. Three runtimes (C strings the C generator emits, the interpreter's, the native one's). A malloc per operation; no dtype, stride, view, broadcast or slice. `matmul` is an i-j-k loop. |
| Softmax | `codegen_c.fd`, `p_tensor_softmax` | **The backends disagree (run today with `release/fid-night.exe`).** `softmax([[0,1],[2,3]])` is `0.032 0.087 0.237 0.644` through `ccompile` (all elements) and `0.269 0.731 0.269 0.731` through `run` and `emit` (the last axis, as `tensor.fd` documents). The probe is `build/tmp/aiml-probe/softmax.fd`. |
| The example | `examples/neural_network.fd` | Cannot add its biases: `forward_layer` returns `z` unchanged. |
| Autograd | a runtime tape in `compiler/interp.fd` and in the C runtime; `compiler/autograd.fd` is a no-op | 9 primitives (add, sub, mul, matmul, scale, relu, sigmoid, sum, mean); `grad(x) { }`, `backward(loss)`. `@differentiable` is a type-checker mark. `@no_grad` is in neither the type checker nor the reference. |
| Speed | `stdlib/simd.fd`, `stdlib/parallel.fd`, `stdlib/math.fd` | SIMD over f64 lanes (two at a step, four under AVX2). `parallel.fd` spawns threads per call. `math.fd` has 13 functions and 2 constants, f64, and no tanh. No BLAS anywhere. |
| GPU | `compiler/gpu.fd`; `stdlib/vulkan.fd`, `metal.fd`, `webgpu.fd`, `d3d12.fd` | `fid gpu` emits SPIR-V and PTX for integer kernels only. The FFI modules have bindings (Metal and WebGPU down to a dispatch); no `@kernel` runs on a GPU today. |
| Formats | `stdlib/safetensors.fd`, `bpe.fd`, `csv.fd`, `protobuf.fd` | safetensors reads only (F16 and BF16 widen to f32); a GPT-2 style BPE; CSV; the protobuf wire format. No GGUF, ONNX, NumPy file, Parquet or TFLite. |
| Inference | `stdlib/infer.fd` (245 lines), `stdlib/kv_cache.fd` (51 lines) | f64 kernels on `Vec<f64>`: matvec, RMSNorm, RoPE, softmax, argmax, single-head attention, top-k and top-p sampling; a KV cache behind 6 builtins. |
| Serving | `stdlib/http.fd`, `http2.fd`, `grpc.fd`, `sse.fd`, `metrics.fd`, `trace.fd` | The parts of an inference server exist. No model is served. |
| Python | — | Nothing embeds or extends CPython. `emit` writes no shared library. `docs/guides/from-python.md` is a guide for people. |
| Language | `docs/reference/reference.md` | Generics and comptime, `layer` and `model` sugar (a struct, an impl, a constructor), the quantisation cast `e ~> T`, `region … with budget(N)` (a compile-time check), `@dp`, `@checkpoint` and `@shard` (marks with small runtime helpers). No operator overloading, no bf16 or f16, no slices (EV12), no tuples. |

## Costs that come on top of every goal

- **The test cycle.** A build of the compiler is 15 to 30 minutes with the
  machine shared, and the whole-suite run takes hours (AGENTS.md). The
  tensor core and the kernels are standard library code, which the compiler
  reads at run time: no build. A goal that touches `compiler/*.fd` (AI7,
  AI9, AI12, AI17, AI19, AI50, AI60, AI63, AI68, AI70, AI87) costs a build
  for every iteration.
- **Stage 0 pins the language.** `compiler/*.fd` is built by the pinned
  stage 0 (`scripts/stage0.sha256`). New syntax (AI9's operators, AI60's
  shape types) cannot appear in the compiler's own source until stage 0 is
  re-pinned to a fid that has it.
- **Another session edits the tree.** goal-workflow.md rule 2: run
  `git status --porcelain -- <file>` before touching a file.
- **A number needs a quiet machine.** A benchmark taken while another
  session compiles is not a result (AGENTS.md: several fid processes at
  once make a stage 10 to 200 times slower). Every number recorded in an
  entry says with which binary and on how loaded a machine.
- **The interpreter is slow.** A `tests/ml` test is small enough for `run`;
  a benchmark or a model run goes through `ccompile` and `emit`.

## Before any of this

- [ ] AI1. An oracle for every number (S).
      `tests/ml/`: for each operation, its inputs and the output Python
      computes, kept in the tree. `tests/ml/check.sh` runs every
      `tests/ml/test_*.fd` on the three backends against them.
      `tests/ml/oracle/gen.py` regenerates the files (it needs Python with
      NumPy; running the tests never does). The first files are JSON
      (`stdlib/json.fd`); binary `.npy` once AI26 reads it.
      Today: eight ML tests in `tests/bootstrap` with expectations written
      by hand (`test_ml_full.fd`, `test_ml_edge_cases.fd`,
      `test_autodiff*.fd`, `test_driver_gpu.fd`, `test_gpu_region*.fd`),
      and three backends that already disagree on softmax.
      Done when: `bash tests/ml/check.sh` passes with a fid built from the
      tree, each of the 27 functions of `tensor.fd` has a file, the
      softmax difference of the table above is a failing test until AI10
      fixes it, and a deliberately wrong result fails the check.

## A. The tensor core (AI2–AI12)

Proof at the end: every function of `tensor.fd` gives NumPy's bits for
add, sub, mul, div and sqrt on the three backends, in f32 and in f64.

- [ ] AI2. The tensor core written in Fides (M).
      The bodies of `p_tensor_*` leave `compiler/codegen_c.fd` (today C
      strings that every tensor program carries) and become Fides in the
      standard library: one implementation for C, pe_ir and nat_ir, as
      `Vec` is one implementation since Nov 9 (`pe_runtime_alias` sends
      `p_vec_*` to core). The interpreter keeps a thin simulation over its
      own memory, as it does for `p_vec_*`; the arithmetic of
      `pert_tensor_*` and of the interpreter's copy goes.
      Why first: everything after would else be written three times, and
      the three already disagree (softmax).
      Done when: the C of a tensor program no longer contains the 27
      bodies (its size before and after recorded), the oracle results are
      the same on the three backends, and `test_ml_full.fd` with
      `test_ml_edge_cases.fd` pass unchanged.

- [ ] AI3. One type parameter: `Tensor<f32>` and `Tensor<f64>` (M).
      f32 is what the engine runs on; f64 stays for science and for the
      interpreter's checks. Every function of `tensor.fd` and every kernel
      takes the type parameter.
      Today: one builtin type over a flat f64 array in a 32-byte block
      (`stdlib/pe_runtime/tensor.fd`); `safetensors.fd` widens F16 and
      BF16 to f32 on read.
      Done when: the 27 functions and the kernels of `infer.fd` run on
      both types on the three backends; add, sub, mul, div and sqrt are
      NumPy's `float32` and `float64` bits; matmul is within 1e-5 of NumPy
      (the order of summation differs).

- [ ] AI4. Strides and views (M).
      A tensor is data, an offset, a shape and strides. `reshape`,
      `transpose`, `permute`, `squeeze`, `unsqueeze` and slicing return a
      view that shares the data; `contiguous()` copies when needed; matmul
      reads a transposed or strided operand without copying it. A view
      keeps its buffer alive through the owner count (proposal 0004),
      never through a bare pointer.
      Today: every operation allocates a new tensor (a malloc per
      operation in the C bodies); `transpose` copies; the block has no
      strides (`pet_t_stride` is a runtime helper).
      Done when: no view operation allocates data (`fid explain --alloc`
      shows none), a write through a view is seen through the base,
      `scripts/leak-check.sh` frees every block of a program that drops
      views before and after their base, and 15 view shapes match NumPy.

- [ ] AI5. Broadcasting (M).
      NumPy's rules for the elementwise operations and for batched matmul;
      a bias of shape (4) or (1, 4) added to a (B, 4) tensor.
      Today: `add(a, b)` needs equal shapes; `examples/neural_network.fd`
      leaves its biases out because it cannot add them.
      Done when: every case of NumPy's broadcasting guide matches the
      oracle, the example adds its biases and prints a probability sum of
      1, and a pair that cannot broadcast panics naming both shapes.

- [ ] AI6. Indexing and selection (M).
      Reads and writes by n indices, a range per dimension, `gather`,
      `scatter`, `index_select`, `index_put` (with accumulate), boolean
      masks and `where`, `concat`, `stack`, `split`, `pad`, `flip`,
      `expand`. The syntax `t[i, j]`, and ranges inside the brackets,
      waits for slices (evolution-roadmap EV12).
      Today: `get(t, Vec<i64>)` and `set`.
      Done when: 30 oracle files; `gather`, `scatter` and `index_put`
      equal NumPy's (a repeated index in `index_put` accumulates, as
      `np.add.at` does); a mask of the wrong shape is an error.

- [ ] AI7. bf16 and f16 (M).
      Two value types with conversion to and from f32 (round to nearest
      even), arithmetic by widening to f32, printing, literals; storage in
      tensors (AI3) and in safetensors.
      Today: neither type is in the reference; `safetensors.fd` converts
      both to f32 as it reads.
      Done when: all 65,536 bit patterns of each type convert to f32 and
      back unchanged (NaN and infinities included), equal to NumPy's
      `float16` and to `ml_dtypes.bfloat16`.

- [ ] AI8. Integer and quantized tensors (M).
      i8, i16, i32, i64, u8 and bool tensors; `q8` symmetric and
      asymmetric (a scale and a zero point) and packed `q4`, two weights
      to a byte, in GGUF's block layout (AI27 reads the same bytes);
      `e ~> q8` on the quantisation cast the reference already defines.
      Today: `p_quantize` and `p_quantize_q8` on an f64 tensor.
      Done when: quantize then dequantize is within one step of the
      original on random tensors, equal to NumPy's formula; casts out of
      range saturate; the byte layout is in the reference.

- [ ] AI9. Operators on tensors (M, a decision first).
      `a + b`, `a * w`, `a @ w`, `-a`, the comparisons (a bool tensor) and
      `+=`, through a trait implemented in `impl` blocks. The decision:
      which operators, whether `@` joins the language, and how they meet
      the checked arithmetic. Until it is taken, the pipe reads well
      enough: `x |> linear(w, b) |> relu`.
      Today: the reference has no operator overloading; every tensor
      operation is a function call.
      Done when: a program written with operators and the same program
      with calls give the oracle's results on the three backends, the
      reference states precedence and the rules, `fid fmt` handles them,
      and three conformance tests (`//! error:`) reject a misuse.

- [ ] AI10. The operation set of a decoder block and of a CNN (L).
      exp, log, tanh, gelu, silu, sigmoid; softmax and log-softmax per
      axis (today: the last axis on `run` and `emit`, all elements on the
      C path); layer norm, RMS norm, batch norm in inference; variance and
      standard deviation per axis; cumsum; argmax and argmin per axis;
      sum, mean, max and min per axis with `keepdims`; batched matmul;
      where, clip, abs, neg, pow, sqrt, rsqrt; conv1d, conv2d, depthwise
      convolution and pooling (AI22); embedding and one-hot.
      Done when: each has an oracle file, and the backends agree on all
      of them (AI1's check is green).

- [ ] AI11. Special functions, once (S-M).
      tanh, sinh, cosh, asinh, atanh, erf, erfc, erfinv, the gamma family,
      expm1, log1p, exp2, log2, cbrt, hypot, atan2, sigmoid, gelu: in f32
      and in f64, in one module, for the tensors and for scalars.
      Today: `math.fd` has exp, log, pow, sqrt, sin, cos, floor, ceil,
      abs, clamp, min, max and fabs in f64; tanh is nowhere in the
      standard library; erfc and lgamma are in `entropy.fd`.
      Done when: on 100,000 sampled arguments per function the error is
      at most 1 ulp against mpmath at 80 digits (2 ulp for the inverse
      functions), each is monotonic where it should be, and the rules for
      NaN and the infinities are in the reference.

- [ ] AI12. The same bits on every path (M).
      Every reduction (sum, dot, matmul, softmax, a norm) returns the same
      bits whatever the backend, the thread count (AI18) and the vector
      width (AI16), by a fixed order of summation that each path
      implements; nothing is reassociated unless a goal says so (AI19).
      Today: `simd.dot` gives the same bits on every path by summing in
      order (its header); nothing promises it for a tensor operation that
      is threaded or vectorized.
      Done when: a program prints the bit patterns of its results and the
      three backends with 1, 2 and 8 threads print the same lines;
      `tests/ml/determinism-check.sh` runs in CI.

## B. Fast on a CPU (AI13–AI24)

Proof at the end: the scoreboard has Fides on it, next to NumPy and
PyTorch, and the GEMM is within 2 times OpenBLAS.

- [ ] AI13. The scoreboard first (S).
      `tests/ml/bench/`: matmul f32 and f64, attention, conv2d, softmax,
      layer norm and RMS norm at sizes from 64 to 4096, a quantized
      matmul. `bash tests/ml/bench.sh` prints GFLOP/s and GB/s and
      compares with the previous run; `docs/perf/ml-<date>.md` holds the
      numbers next to NumPy (OpenBLAS) and PyTorch on a CPU, on the same
      machine.
      Today: `fid bench <file> -n <count>` times a program;
      `docs/benchmarks.md` has no numeric kernel.
      Done when: the first page exists with Fides' numbers, however poor,
      and the script runs in under two minutes.

- [ ] AI14. BLAS through FFI (M).
      `stdlib/blas.fd`: `extern "C" link "openblas"` (BLIS, MKL and
      Accelerate as alternatives chosen at build time); gemm, gemv, axpy,
      dot and syrk in f32 and f64, the bindings from `fid parse-header` on
      `cblas.h`. When the library is missing, the kernel of AI15 runs.
      Today: no BLAS is mentioned in the standard library or the compiler;
      `extern "C" link` works (`vulkan.fd`, `d3d12.fd`).
      Done when: a 1024³ f32 matmul through OpenBLAS is within 1e-4 of
      NumPy on Windows, Linux and macOS (CI installs the library), and a
      program on a machine without it still runs and says which path it
      took.

- [ ] AI15. A blocked f32 GEMM written in Fides (L).
      Packed panels, a micro-kernel on the f32 lanes of AI16, blocking for
      the L2 cache; single-threaded within 2 times OpenBLAS on the same
      CPU from N = 256 to 4096. The C path and the native paths are
      measured apart.
      Why: this kernel is the proof that the language can express a real
      one, and it is what runs when AI14 has no library.
      Done when: the numbers are in `docs/perf`, and the result is within
      1e-5 of NumPy.

- [ ] AI16. f32 SIMD lanes (M).
      f32x4 and f32x8 in `stdlib/simd.fd`: loads, stores, min, max and a
      fused multiply-add with one rounding (the machine layer has it,
      `nat_ir.fd`; the IR has no instruction for it). AVX2 gives 8 lanes,
      NEON 4, RVV its width, WebAssembly's fixed-width SIMD128 4
      (`compiler/wasm.fd` has no SIMD today).
      Today: f64 lanes, two at a step, four under AVX2.
      Done when: add, sub, mul, div, min and max give the bits of the
      scalar loops on every target, the fused multiply-add gives the
      oracle's bits, and a `tests/nattools` check script runs in CI.

- [ ] AI17. AVX-512 (M).
      Under `--cpu x86-64-v4`: loads and stores of 512 bits, masked tails
      instead of scalar remainder loops; one binary that runs on old and
      new CPUs by choosing at run time.
      Today: `--cpu x86-64-v4` selects AVX2 (`simd.fd`); `nat_ir.fd`
      mentions AVX-512 once.
      Done when: the gain is on the scoreboard on a CPU with AVX-512
      (Intel's SDE emulator if none is at hand), and the bits are those of
      AI12.

- [ ] AI18. A work pool for tensors (M).
      A parallel loop over row blocks on one pool per process; nested
      loops that do not oversubscribe; results that do not depend on the
      thread count (AI12).
      Today: `stdlib/parallel.fd` spawns threads per call for `par_sum`
      and `par_dot`, and runs `par_map`, `par_for_each` and `par_reduce`
      sequentially; a function value cannot reach a spawned task
      (bugs-roadmap BG256, open: a decision of the language).
      Done when: the GEMM of AI15 at N = 2048 uses at least 70% of the
      cores of this machine, and the tests in `tests/par` are unchanged.
      The entry takes the decision of BG256, or passes a function pointer
      and a context word.

- [ ] AI19. `@simd` on f32 loops (M).
      The vectorizer of the machine layer (`@simd`, checked operations,
      TG11) learns f32 loads and stores, reductions in a fixed order and
      strided loads; an explicit `@simd(fast)` asks for reassociation and
      states its accuracy bound.
      Today: TG11's open cases are two reductions; the loops of
      `infer.fd` are not marked.
      Done when: `infer_matvec`, `infer_rmsnorm` and `infer_softmax` marked
      `@simd` show vector instructions in `FID_NAT_DUMP`, the speed-up is
      on the scoreboard, and `@simd` alone leaves the bits unchanged.

- [ ] AI20. Fused kernels (M).
      Bias and activation, RMS norm, layer norm, softmax and gelu, each in
      one pass over memory.
      Done when: each fused kernel reads its input once (bandwidth of at
      least 80% of the machine's stream bandwidth on large tensors) and is
      within 2 ulp of the same operations composed.

- [ ] AI21. Flash attention on a CPU (L).
      Attention in tiles, without the T by T matrix of scores: several
      heads, the causal and the padding mask, f32, for prefill.
      Done when: within 1e-5 of NumPy's attention at head dimensions 64
      and 128 and lengths from 128 to 8192, and the peak memory does not
      grow with the square of the length (`fid explain --alloc`).

- [ ] AI22. Convolution (L).
      conv2d by im2col with GEMM and by a direct tiled loop, NHWC and
      NCHW, stride, padding, dilation and groups; depthwise; conv1d for
      audio; the transposed convolution; max and average pooling.
      Done when: within 1e-5 of PyTorch on a CPU for each combination
      tested, and the images per second of a ResNet-18-shaped block are on
      the scoreboard.

- [ ] AI23. Quantized matmul (L).
      q8 by q8 into i32 (`vpmaddubsw` and `vpmaddwd` on x86, `sdot` on
      AArch64 with the dot-product extension), q4 weights unpacked as they
      are read (AI8).
      Done when: bit for bit the integer reference, and within 2 times
      llama.cpp's Q8_0 and Q4_0 matmul on the same CPU, measured.

- [ ] AI24. Weights in one arena, read once (M).
      `region weights { … }`: every parameter of a model in one arena,
      filled from the file, under `with budget(N)`; a decoder block reads
      the weights in place, with no copy per layer; peak memory and load
      time reported.
      Today: every tensor is its own allocation.
      Done when: the 0.5B model of AI37 loads with a number of allocations
      that does not grow with the number of tensors, and the peak and the
      load time are in `docs/perf`.

## C. Formats, and the way in from Python (AI25–AI36)

Proof at the end: `fides.export(module)` in a notebook writes a directory
that Fides loads, and the Fides program gives PyTorch's outputs.

- [ ] AI25. safetensors, complete (S).
      Writing, reading through a memory map where the OS has one, the
      dtype conversion chosen at load, a header validated as the reference
      implementation validates it.
      Today: 387 lines that read (`st_open`, `st_names`, `st_tensor`,
      `st_f32`, `st_f16`, `st_bf16`, `st_f64`, `st_int`, `st_uint`,
      `st_meta`); BG320 and BG353 fixed a shape overflow and a zero
      dimension.
      Done when: a file Fides writes loads in Python
      (`safetensors.numpy.load_file`) with the same values, a file Python
      writes loads in Fides, and a header with overlapping `data_offsets`
      is refused.

- [ ] AI26. NumPy's `.npy` and `.npz` (S-M).
      Versions 1.0 and 3.0 of the format: the header, `fortran_order`,
      every dtype (f16 and bf16 through AI7). `.npz` through a zip reader
      with stored and deflate entries (zlib inflate is in `compress.fd`).
      The oracle's files become binary.
      Today: nothing reads a NumPy file; `tar.fd` reads archives, and
      there is no zip reader.
      Done when: 30 arrays written by `np.save` and `np.savez` round-trip
      bit for bit, a Fortran-ordered array arrives as a transposed view
      (AI4), and a corrupted header is refused with a message.

- [ ] AI27. GGUF (M).
      A reader for version 3: the header, the metadata values (strings,
      arrays of every type, nested arrays), the tensor infos, the
      alignment; tensor data by type F32, F16, BF16, Q8_0, Q8_1, Q4_0,
      Q4_1, Q5_0, Q5_1 and the K-quants; mapped where the OS has `mmap`,
      read elsewhere.
      Today: nothing reads GGUF.
      Done when: for five public files pinned by sha256 (one per family of
      dtypes, small ones) every tensor's dequantized values equal the `gguf`
      Python package's (f32 and f16 exactly), the metadata is equal, and a
      truncated file is refused.

- [ ] AI28. ONNX, read (M).
      ModelProto, GraphProto, NodeProto, AttributeProto, TensorProto and
      ValueInfoProto on top of `stdlib/protobuf.fd` (343 lines: the wire
      format, no schema compiler); the opset version; external tensor
      data (location, offset, length).
      Done when: ten public models pinned by sha256 (MNIST, a CIFAR CNN, a
      small BERT, a GPT-2 export among them) parse with every node,
      attribute and initializer named, equal to what `onnx.load` reports
      (the oracle, made once by `gen.py`).

- [ ] AI29. ONNX, run (L).
      `onnx_run(model, inputs)` for one listed opset, with the operations
      an exported vision model or transformer uses: MatMul, Gemm, Conv,
      the activations, Softmax, LayerNormalization, Reshape, Transpose,
      Concat, Gather, the Squeeze and Slice family, ReduceMean, Erf, Cast,
      Where, Split, Pad, Expand. An operation outside the list is an error
      that names the node.
      Done when: the ten models of AI28 give onnxruntime's outputs within
      1e-4 on the oracle's inputs.

- [ ] AI30. ONNX to Fides source (L).
      A converter (`fid onnx2fd`) that writes a Fides model with the
      `layer` and `model` sugar and its weights in safetensors, so that
      `fid check` checks the model and the optimizer sees it with the
      program that uses it.
      Done when: for the ten models the generated source passes
      `fid check`, runs with the outputs of AI29, and is at least as fast
      as AI29's reading of the graph.

- [ ] AI31. The PyTorch exporter (L).
      A Python package, `fides-export` on PyPI: `fides.export(module,
      sample_inputs, "model/")` traces the module with `torch.export`,
      writes Fides source (AI30's writer), weights in safetensors (AI25)
      and a `config.json`. This is the user's face of the roadmap: the
      guide "Train in PyTorch, serve in Fides" is written around it.
      Today: nothing moves a model from Python to Fides.
      Done when: `pip install fides-export` works on Windows, Linux and
      macOS; five models (an MLP, a CNN, ResNet-18, a small transformer,
      one GPT-2 block) give PyTorch's outputs within 1e-4 on 20 random
      inputs each; the package's own tests run in CI.

- [ ] AI32. The Hugging Face Hub (M).
      `hf_download(repo, file)`: the `resolve/<revision>/<file>` address,
      HTTP Range for a resumable download, a cache under `HF_HOME` keyed by
      the file's etag, `HF_TOKEN`, retries, and the reading of
      `config.json` and of a safetensors index.
      Today: `stdlib/http_client.fd` sends no Range request; the HTTP/1.1,
      HTTP/2 and HTTP/3 stacks exist.
      Done when: three files pinned by sha256 download from public
      repositories, a download killed at its half resumes, the second call
      makes no request, a 401 and a 404 are named errors; the tests that
      touch the network run only with `HF_TESTS=1`.

- [ ] AI33. Tokenizers (L).
      SentencePiece Unigram and BPE (`tokenizer.model`), the
      `tokenizers.json` format (byte-level BPE, WordPiece, Unigram, with
      normalisers, pre-tokenizers and decoders), special tokens, an
      encoder that streams.
      Today: `stdlib/bpe.fd` (186 lines) loads a GPT-2 style vocabulary
      and a merges list; `unicode.fd` has NFC, NFD, NFKC and NFKD.
      Done when: token ids equal Hugging Face `tokenizers` for ten pinned
      tokenizers (GPT-2, Llama 3, Qwen2, Mistral, BERT, Gemma among them)
      on a 1,000-sentence corpus with emoji, CJK and malformed UTF-8, and
      encode then decode gives the text back.

- [ ] AI34. PyTorch `.pt` without running pickle (M).
      A torch zip (AI26's reader) and a reader of `data.pkl` that executes
      nothing: the opcodes `torch.save` uses for tensors, OrderedDict,
      list, tuple, dict, str, numbers and the `_rebuild_tensor_v2` family;
      anything else is refused by name.
      Done when: five files saved by PyTorch 2.x load with the original
      values (the oracle made by a Python script), a file with another
      callable is refused, and `scripts/fuzz/fuzz_parsers.py` has a
      `torch` target with no finding in 10,000 inputs.

- [ ] AI35. TensorFlow Lite models (L).
      `.tflite` FlatBuffers: tables and vtables; the schema of the fully
      connected, conv2d, depthwise conv2d, average pool, max pool,
      softmax, reshape, add, logistic and tanh operators, in float and in
      int8. This is the way in for a TensorFlow user.
      Done when: the MNIST and the CIFAR-10 TFLite models (pinned) give
      the TensorFlow Lite interpreter's outputs within 1e-4 (float) and
      bit for bit (int8).

- [ ] AI36. Columnar and tabular data (L).
      Parquet v1 pages with PLAIN, dictionary and RLE encodings, snappy
      and zstd (zstd is in `compress.fd`), into columns and tensors; the
      Arrow IPC stream.
      Today: `csv.fd` (RFC 4180), `data.fd` (`data_load_csv` and schema
      helpers), `protobuf.fd`; no Parquet, no Arrow.
      Done when: five Parquet files written by pandas and pyarrow (pinned)
      load with equal values, and the load time and the peak memory of a
      100 MB file are recorded.

## D. The inference engine (AI37–AI49)

Proof at the end: a 0.5B model answers from one binary through an
OpenAI-compatible API.

- [ ] AI37. A decoder block in f32 (L).
      `stdlib/llm.fd`: from a config and weights to `forward(tokens)`,
      the logits: embedding, RMS norm, RoPE, grouped-query attention with
      a causal mask, a gated MLP (SwiGLU), the residual adds, the final
      norm and the output head, tied or not.
      Today: `stdlib/infer.fd` (245 lines) has f64 kernels on `Vec<f64>`:
      matvec, RMS norm, RoPE, softmax, argmax, single-head attention.
      Done when: the logits of a 2-layer toy model and of a real 0.5B
      model (weights through AI25) equal PyTorch's within 1e-2 absolute on
      10 prompts, with the same first token on at least 99 of 100
      positions.

- [ ] AI38. The KV cache as a real one (M).
      Blocks of a fixed size given to sequences as they grow; a read and
      an append by position and layer; a sliding window (the `region
      kv_cache r with budget(N) [window(W)]` block exists); prefill and
      decode as separate paths; eviction.
      Today: `p_kv_cache_new`, `append`, `get`, `len`, `evict` and `clear`
      behind a module of 51 lines.
      Done when: the output with the cache is the output without it (the
      same bits, the same kernel), a windowed attention equals a full
      attention masked to the window, and a context of 32,768 tokens runs
      in a stated memory that stays inside `budget(N)`.

- [ ] AI39. Sampling and structured output (M).
      Temperature, top-k, top-p, min-p, repetition, frequency and presence
      penalties, logit bias, beam search, stop strings and stop tokens, a
      seed. And constraints: a JSON schema or a regular expression
      compiled to a mask of the tokens allowed at each step.
      Today: `infer_top_k`, `infer_sample` and `infer_sample_top_p` in
      f64; `random.fd` (xorshift*).
      Done when: the counts of 1e5 samples pass a chi-square test against
      the exact softmax distribution, the penalties equal Hugging Face's
      processors on fixed logits, and schema-constrained output parses for
      1,000 generations.

- [ ] AI40. Several sequences at once (L).
      Continuous batching: requests join and leave a running batch, a
      batched prefill, one decode step for the batch, each sequence's
      cache positions and mask, a scheduler (first come, first served;
      then by shared prefix).
      Done when: 16 concurrent requests give at least three times the
      tokens per second of one request at a time on this machine (the
      number recorded with the binary), and each sequence's output is
      within 1e-4 of what it gets alone.

- [ ] AI41. Chat templates (S-M).
      The `chat_template` of a model card is Jinja: an interpreter of a
      subset that calls nothing but registered functions and filters, so a
      template is data. Hand-written templates for Llama 3, Qwen2.5,
      Mistral and Gemma are the oracles.
      Today: there is no template engine in the standard library.
      Done when: ten pinned templates render 20 conversations (with tool
      calls where the template has them) byte for byte as Hugging Face's
      `apply_chat_template` does.

- [ ] AI42. Quantized inference, end to end (M).
      A model in Q8_0 or Q4_0 (AI8, AI23, AI27) with f32 activations, and
      its perplexity on wikitext-2-raw (the usual stride of 2048) next to
      the f32 model's.
      Today: `p_quantize`, `p_quantize_q8` and the cast `e ~> T`.
      Done when: for a 0.5B model q8_0 is within 0.5% of the f32
      perplexity and q4_0 within 3%, at 1.5 times the speed of f32 or
      better on this machine.

- [ ] AI43. Speculative decoding (L).
      A small draft model proposes k tokens and the target verifies them
      in one pass; the distribution of the output is the target's own.
      Done when: greedy output is identical to the target's alone for 100
      prompts, and a pair of a 0.1B and a 0.5B model accepts at least 1.5
      tokens per step on this machine.

- [ ] AI44. Embeddings and retrieval (M-L).
      Sentence embeddings (a BERT-class encoder through AI29 or AI83),
      cosine similarity, exact search and an HNSW index kept in a file; a
      `rag` helper from chunks to a prompt.
      Today: nothing of the kind in the standard library.
      Done when: on 1M random vectors of dimension 384 the recall@10 of
      HNSW is at least 0.95 of the exact search at a stated build time,
      and a reloaded index gives the same neighbours.

- [ ] AI45. An OpenAI-compatible server (M).
      `/v1/chat/completions`, `/v1/completions`, `/v1/embeddings` and
      `/v1/models`; streaming by SSE (`stdlib/sse.fd`: the encoding and a
      streaming parser); HTTP/1.1 and HTTP/2 (`http.fd`, `http2.fd`);
      JSON (`json.fd`); the model loaded at start (AI49).
      Today: all the parts except the API and the engine.
      Done when: the `openai` Python client and `curl` complete a chat, a
      streamed chat and an embedding call against a Fides server on this
      machine, and 50 requests (an unknown model, too long a context, a
      bad body among them) give OpenAI's status codes and error shapes.

- [ ] AI46. A gRPC inference service (S-M).
      A unary method and a server-streaming method for tokens, the tensors
      as protobuf messages, on `stdlib/grpc.fd` (1,060 lines: framing,
      status and trailers, a service registry).
      Done when: Python's `grpc` client calls both methods against a
      Fides server on every platform where CI runs one natively.

- [ ] AI47. Production behaviour (M).
      `/metrics` (`metrics.fd`: counters, gauges and histograms in
      Prometheus text); traces (`trace.fd`: a JSON line per span under
      `FIDES_TRACE`); request timeouts; cancellation (`cancel scope;`);
      a limit on concurrent requests with a named refusal; back-pressure
      on streams; a shutdown that finishes the running requests; flat
      memory.
      Done when: a 24-hour run with 16 clients keeps the memory at hour 24
      within 5% of hour 1 with no request failed that the test did not
      cause, the p50 and the p99 latency are recorded, and
      `scripts/leak-check.sh` is clean on the server program.

- [ ] AI48. A hard memory ceiling (M).
      `with budget(N)` on the regions that hold the weights, the cache and
      the activations, enforced at run time for each request: a request
      that would cross it is refused before anything is allocated, with
      what it asked for and what was left.
      Today: budgets are compile-time checks (roadmap.md: "only
      compile-time budgets … exist; no run-time memory limit"); the KV
      region counts what `p_kv_cache_new` allocates.
      Done when: a server started with a budget never allocates more than
      it under adversarial requests (long contexts, many sequences),
      checked with the checked allocator (`--checked-alloc`); a refused
      request gets a named answer and the server keeps serving.

- [ ] AI49. One call to load a model (M).
      `llm_load("qwen2.5-0.5b-instruct")`: the format (GGUF, safetensors
      with a config, ONNX); the architecture from a table (Llama, Llama 3,
      Mistral, Qwen2, Qwen3, Gemma 2, Gemma 3, Phi-3, GPT-2); the
      tokenizer (AI33); the weights in one arena (AI24).
      Done when: ten pinned models load and generate, and a model whose
      architecture or operation is missing is refused after its config and
      before any weight is read, naming what is missing.

## E. The Python bridge and the tools (AI50–AI59)

Proof at the end: `pip install` a wheel, import it, hand it a NumPy array,
get the result without a copy.

- [ ] AI50. Shared libraries (M).
      `fid emit --dll`, `--so` and `--dylib`: the `extern "C"` functions of
      a module exported, with a C header the compiler writes; the runtime
      starts when the library loads.
      Today: `emit` writes executables and objects (`--obj`); the
      reference documents no shared library.
      Done when: a library of three functions loads from Python's
      `ctypes`, from a C program and from another Fides program on
      Windows, Linux and macOS.

- [ ] AI51. A CPython extension (L).
      `fid build --pyext module.fd`: a module against the stable ABI
      (abi3) with a method table; arguments of i64, f64, str, bool and
      arrays (AI52); a Python exception where Fides returns an error or
      panics; the GIL released around a long computation; wheels for
      Windows x64, Linux x86-64 and ARM64, macOS x64 and ARM64, built in
      CI. The prototypes of `Python.h` as `stdlib/sdl2_h.fd` has a C
      library's.
      Today: nothing extends or embeds CPython; `docs/guides/from-python.md`
      is a guide for people.
      Done when: `pip install` of a wheel and `import` call a Fides
      function on the five platforms, and a guide page takes a newcomer
      through it.

- [ ] AI52. NumPy arrays without a copy (M).
      The buffer protocol on a Fides tensor (`__array_interface__`) and
      DLPack in both directions (`__dlpack__`, `from_dlpack`): a NumPy
      array given to Fides is worked on in place, and a tensor returned is
      a NumPy view. The buffer lives while either side holds it (the owner
      count of proposal 0004).
      Done when: `np.shares_memory` is True in both directions, a use
      after the other side let go is caught by the checked allocator in a
      test, and a 4096² matmul through the bridge costs the kernel's time
      and no more (measured, the copies counted).

- [ ] AI53. Calling Python from Fides (M).
      Embedding: `py.import("numpy")`, calls, conversions both ways,
      reference counting per call, the GIL around threads; the link to
      `libpython` (`python3x.dll` on Windows) documented for each platform.
      Done when: a Fides program calls NumPy and matplotlib (a plot written
      to disk) on the three systems in CI, and 1e5 calls grow the memory
      of the process by less than 1 MB.

- [ ] AI54. A Jupyter kernel (M).
      The REPL behind the Jupyter protocol (over WebSockets with
      `stdlib/wss.fd`, or over ZMQ: the first decision of the entry), with
      execution counts, rich display (AI55), an interrupt
      (`cancel scope;`) and errors with their source line.
      Today: `fid repl` runs line by line; `fid notebook <file.fd>`
      renders a Fides file as an HTML page of text and code cells, and
      can run it once.
      Done when: a notebook of ten cells (text, code, a plot, an
      interrupted long cell, a failing cell) runs in JupyterLab on this
      machine, and `jupyter kernelspec list` shows `fides`.

- [ ] AI55. Plots without Python (S-M).
      `stdlib/plot.fd`: line, scatter, bar, histogram, heatmap (an
      attention map, a confusion matrix) and an image; SVG and PNG
      (`zlib_compress` is in `compress.fd`); the loss curve of a training
      run.
      Today: nothing plots; the migration path of positioning.md ends at
      the binary.
      Done when: the six plots render in a browser and in the kernel (AI54)
      from a Fides program, the PNGs decode with Python's PIL, and the loss
      curves of AI86 are in docs.

- [ ] AI56. Tensors printed the way people read them (S).
      As NumPy and PyTorch print: the dtype and the shape, the edges with
      `…` past a threshold, a precision option, scientific notation for
      the large and the small, summaries per axis.
      Today: `p_tensor_to_str` prints with `%.4f` for non-integers
      (AGENTS.md).
      Done when: golden files for 20 printed tensors, and an f-string
      accepts a tensor.

- [ ] AI57. `fid profile` for operations (M).
      The time, the calls and the peak memory of each operation of a model
      run (an operation is a tensor function), and a timeline in the
      Chrome Trace Event format that a viewer opens.
      Today: `fid profile` times functions; `trace.fd` writes a JSON span
      per call of the traced library functions under `FIDES_TRACE`;
      `fid explain --alloc` lists the allocation sites.
      Done when: a profile of the prefill of the 0.5B model names the ten
      operations that take the most time with their share, and the trace
      opens in a trace viewer.

- [ ] AI58. `fid audit --model` (M).
      Loads a model file and, without running it, reports the
      architecture, the operations, the dtypes, every intermediate shape
      (a dry run with a symbolic batch and length), the parameters, the
      weight bytes, the activation bytes for a context length, and whether
      each operation has a kernel on the chosen device.
      Today: `fid audit` reports the unsafe code a program reaches.
      Done when: the report for ten pinned models agrees with the
      safetensors metadata and with torchinfo, and an operation without a
      kernel is named before a server starts.

- [ ] AI59. Errors a newcomer can act on (S-M).
      A shape mismatch names the operation, both shapes and the line, with
      a Fix line as `fid explain-error` gives today; a NaN and Inf detector
      names the first operation that produced one; a refused request says
      what it asked for and what was left.
      Today: a bad matmul returns a null tensor (`return NULL;` in the C
      bodies): the program fails later, somewhere else.
      Done when: 15 tests in `tests/ml` with their expected messages, and
      `scripts/diag-coverage.py` covers every new message.

## F. The trust layer (AI60–AI65)

Properties of a model that the compiler enforces. PyTorch, llama.cpp and
ONNX Runtime have none of them. Lean has the first by construction (its
dependent types give sized arrays) and none of the rest, which are about
the machine a program runs on.
Proof at the end: a decoder whose shapes are checked at compile time, whose
forward pass is pure by type, under a hard memory ceiling, with signed
weights.

- [ ] AI60. Shapes checked at compile time (L).
      `Tensor<f32, [B, T, D]>`: the dimensions are comptime values
      (generics and comptime exist), symbolic ones unify, a matmul of
      `[B, T, D]` by `[D, F]` type-checks and one by `[T, D]` does not,
      broadcasting is computed by the type checker; shapes known only at
      run time stay dynamic.
      Today: `stdlib/shape.fd` (147 lines) asserts at run time and says the
      compile-time checks come later; positioning.md §6 advertises "shape
      mismatch = compile error".
      Not unique to Fides: a dependent type system (Lean's) has sized
      vectors by construction. Its value here is having it in a language
      that needs no runtime and asks for no proofs.
      Done when: 20 conformance tests (10 with `//! error:`) in
      `tests/conformance`; the decoder of AI37 written with shape types
      passes `fid check`; the error names both shapes and the dimension
      that differs.

- [ ] AI61. Differential privacy that means something (M-L).
      `@dp(epsilon=N)` today adds noise and counts epsilon (`p_dp_noise`,
      `p_dp_budget_reset`, `p_dp_budget_check`). Add the mechanism behind
      the number: DP-SGD (a gradient clipped per example, Gaussian noise on
      the sum, an accountant from Rényi differential privacy to a pair
      (ε, δ)), the sampling rate and the number of steps as inputs, epsilon
      reported at each step.
      Done when: the clipped and noised gradient of a DP-SGD step equals
      Opacus's on the same batch within tolerance, the accountant's epsilon
      equals Opacus's RDP accountant within 1e-6, and a step that would
      cross the budget is refused.

- [ ] AI62. Secrets in a model (M).
      `@secret` on weights, on a prompt and on the personal fields of a
      request: none reaches a log, a print, a file or a network sink; a
      tensor can be secret; in the server of AI45 a prompt is secret until
      the writer of the response declassifies it.
      Today: `@secret var` rejects printing a secret (F74,
      `compiler/secrecy.fd`).
      Done when: 20 negative tests (a prompt logged, a weight printed, a
      secret tensor concatenated into a public one, a secret sent over a
      socket) are rejected, and the server passes `fid audit --strict`.

- [ ] AI63. A forward pass that is pure by type (M).
      `fn forward(x: Tensor<f32>) -> Tensor<f32> !Pure` (the name is to be
      chosen): no IO, Net or Spawn effect, proved for the function and for
      everything it calls, so a model can neither read a file, open a
      socket nor fork a task.
      Today: effects are inferred and checked; `@differentiable` asks for
      no IO, Net or Spawn on the function itself; the capability check
      "does not follow calls" and "declared names are not validated"
      (reference §9).
      Done when: the decoder of AI37 is declared `!Pure` and passes; ten
      negative tests (a print, a file read, an HTTP call, a spawn, each
      one call deeper) are rejected; the compiler's own tree passes with
      effect names validated.

- [ ] AI64. The same output bytes everywhere (M).
      The same model, prompt, seed and device give the same output across
      runs, across two builds of the compiler and across Windows, Linux
      and macOS wherever the kernels are deterministic (AI12); a
      `--deterministic` flag turns every other path off.
      Today: `tests/determinism` holds one program (`walks_map.fd`).
      Done when: 10 prompts and 3 models give the same sha256 of the output
      on the three systems in CI, with and without the flag.

- [ ] AI65. Signed, versioned models (S-M).
      A manifest with the sha256 of each file (as
      `tools/release_checksums.fd` does for releases); an Ed25519 signature
      over it, verified at load against a pinned key (`stdlib/ed25519.fd`);
      a format version and a minimum engine version; `fid sign-model`.
      Done when: a signed model loads, a weight file changed by one byte is
      refused, a wrong key is refused, and an unsigned model is refused
      unless `--allow-unsigned` is given.

## G. Autograd and training (AI66–AI77)

Proof at the end: MNIST above 97% with an MLP and above 99% with a CNN,
trained in Fides, and a run that resumes bit for bit.

- [ ] AI66. Gradient rules for the whole operation set (L).
      conv2d and pooling, layer norm, batch norm, embedding, cross-entropy
      from logits, softmax, gelu, silu, tanh, exp, log, pow, sqrt, concat,
      slice, pad, gather, scatter, transpose, reshape, sums and means per
      axis, and the gradient of a broadcast operand summed back to its
      shape.
      Today: nine primitives on a runtime tape (add, sub, mul, matmul,
      scale, relu, sigmoid, sum, mean).
      Done when: each rule matches central finite differences within 1e-5
      in f64 at 100 random points, and the table of the operations and
      their rules is in the reference.

- [ ] AI67. Training on several processes and machines (M-L).
      Data-parallel training: a rank per process or per machine; the
      gradients all-reduced in a ring or a tree over TCP, in buckets and
      overlapped with the backward pass; the model broadcast at the start.
      Today: `p_all_reduce` is an identity in `fid` (roadmap.md: "no newer
      roadmap tracks it"); `p_tp_*` and `@shard` simulate ranks through
      rank 0 (`tests/tp`).
      Done when: four processes on this machine train the CNN of AI86 to
      the accuracy of one process with four times the batch; the gradient
      of the first step equals the one-process gradient of the joined
      batch within 1e-6, and so does the loss curve.

- [ ] AI68. Autograd at compile time (L).
      `grad(x) { … backward(loss); }` is rewritten by the compiler
      (comptime, from source to source) into the forward function and its
      transpose: no tape, no history kept per operation, and the fused
      kernels of AI20 keep their gradient fused. The runtime tape stays in
      the interpreter as the oracle of the transform. This closes
      roadmap.md's Issue 7, and `@differentiable` gets its meaning: a
      backward pass was generated.
      Today: `compiler/autograd.fd` is a no-op of 38 lines; the tape lives
      in the interpreter and in the C runtime.
      Done when: `check compiler/main.fd` is clean; the training step of an
      MLP compiled this way allocates nothing for history (counted) and
      gives the tape's gradients within 1e-12; `test_autodiff*.fd` and the
      gradients of `tests/ml` pass on the three backends.

- [ ] AI69. The gradient of a function, as a value (M-L).
      `grad_of(loss_fn)`: the backward twin of any `@differentiable`
      function, to pass around and to call, for functions closed over
      parameters too.
      Today: `grad(x) { }` is a block written around expressions.
      Done when: it holds for ten functions, with the test of AI68.

- [ ] AI70. `@no_grad` (S).
      A block, or a form on a function, that switches recording off; inside
      it the operations take the inference path (the kernels of AI20, the
      views of AI4) and nothing is kept for history.
      Today: `no_grad` is in none of `compiler/typechecker.fd`,
      `compiler/lower.fd` and the reference; positioning.md §6 lists it.
      Done when: a forward pass under `@no_grad` allocates nothing for
      history (counted), the gradients after the block are those of the
      recorded part alone, and three tests cover it.

- [ ] AI71. Checkpointing, measured (M).
      Activation checkpointing at the level of a block
      (`checkpoint(block, x)`): the backward pass recomputes the
      activations of the block; what it saves and what it costs are
      measured.
      Today: `@checkpoint` is a mark; the tape of the runtime has a mode
      that keeps the inputs (`ag_checkpoint_mode`).
      Done when: the peak memory for activations of a toy transformer of 12
      blocks is below a third with checkpointing, the cost in time is
      recorded, and the gradients are identical.

- [ ] AI72. Optimizers (S-M).
      SGD with momentum and Nesterov, Adam, AdamW, RMSprop, Adagrad and
      Lion; parameter groups; weight decay; clipping by norm and by value.
      Done when: ten steps of each on a fixed problem equal the parameters
      of `torch.optim` within 1e-6.

- [ ] AI73. Learning-rate schedules (S).
      Step, cosine, cosine with warm restarts, linear warmup and decay,
      one-cycle, exponential, reduce on plateau; each with a state that
      AI77 can save.
      Done when: 200 points of each equal `torch.optim.lr_scheduler` within
      1e-12.

- [ ] AI74. Initialisers and seeded streams (S).
      Xavier and Glorot (uniform and normal), Kaiming and He (fan in, fan
      out), orthogonal, truncated normal, constants; a seeded stream per
      parameter, so that a run reproduces whatever the order of the threads
      (`random.fd` has one generator for the process).
      Done when: the mean and the variance of 1e6 samples are within 1% of
      the theory, the statistics equal those of `torch.nn.init`, and a seed
      gives the same weights on the three backends.

- [ ] AI75. Regularisation (S).
      Dropout with train and eval modes (seeded), label smoothing, L1 and
      L2 penalties, early stopping on a validation metric.
      Done when: dropout keeps the stated fraction within 1% and rescales
      exactly, the eval mode is the identity, the loss with label smoothing
      equals the formula, and ten tests cover it.

- [ ] AI76. Mixed precision on a CPU (M).
      Storage in bf16 and f16 (AI7) with accumulation and master weights in
      f32; a dynamic loss scale for f16; an `autocast` block that decides
      per operation which type runs.
      Done when: a CNN and an MLP trained under `autocast` reach the
      accuracy of the f32 run within 0.3 points on MNIST and on CIFAR-10 in
      the same number of steps.

- [ ] AI77. Save, resume, reproduce (M).
      A checkpoint in safetensors with the parameters, the state of the
      optimizer, of the schedule and of the random streams, the step and
      the config, with a version of the format.
      Done when: a run of two epochs interrupted at five points and resumed
      gives the same final parameter bytes as the run that never stopped,
      and a checkpoint of an older version is converted or refused by name.

## H. The neural network library (AI78–AI86)

- [ ] AI78. Modules with state (M).
      A `Module` trait: parameters and buffers with names, `train()` and
      `eval()`, `parameters()`, `state_dict()` and `load_state_dict()` with
      the names PyTorch gives (`layers.0.weight`). The `layer` and `model`
      sugar of the reference gets parameters and a training mode.
      Today: `layer L(args) { fields; fn forward(…) }` and
      `model M { name: Layer(args), …; fn forward(…) }` are a struct, an
      impl and a constructor.
      Done when: an MLP written with `layer` and `model` trains, and its
      state dict has the names PyTorch gives the same architecture.

- [ ] AI79. Layers (M).
      Linear, Embedding, Conv1d and Conv2d (AI22), MaxPool and AvgPool,
      BatchNorm1d and BatchNorm2d (training with running statistics, and
      inference), LayerNorm, RMSNorm, Dropout, Sequential, Flatten.
      Done when: the output and the gradients of each layer equal PyTorch's
      within 1e-5 on random inputs, on the three backends.

- [ ] AI80. Losses (S-M).
      Mean squared error, absolute error, Huber, binary cross-entropy from
      logits, categorical cross-entropy from logits and from probabilities,
      sparse cross-entropy, KL divergence, cosine embedding, triplet
      margin; stable by log-sum-exp.
      Done when: values and gradients equal `torch.nn.functional` within
      1e-6 on 100 random cases each, with logits of ±1e4 among them.

- [ ] AI81. Activations (S).
      ReLU, LeakyReLU, PReLU, GELU (exact and in its tanh form), SiLU,
      Sigmoid, Tanh, Softplus, ELU, Mish, Softmax and LogSoftmax per axis,
      the gated linear units.
      Done when: values and gradients equal PyTorch's within 1e-6.

- [ ] AI82. Attention modules (M).
      Scaled dot-product attention, multi-head and grouped-query attention,
      cross-attention, the causal and the padding mask; the path of AI21
      chosen by itself where it applies.
      Done when: equal to `scaled_dot_product_attention` of
      `torch.nn.functional` within 1e-5 for every mask and dtype tested.

- [ ] AI83. Transformer blocks (M).
      An encoder block and a decoder block, pre-norm and post-norm, RoPE
      (`infer.fd` has a version in f64), ALiBi, learned and sinusoidal
      positions, the cache of AI38.
      Done when: an encoder of 2 layers reaches a stated loss on a toy copy
      task and a decoder of 2 layers on a toy next-token task, in a stated
      number of steps.

- [ ] AI84. Classical machine learning (L).
      `stdlib/learn.fd`: linear and logistic regression, k-means,
      k-nearest neighbours, PCA, a decision tree, a random forest, naive
      Bayes, a Gaussian mixture; and the metrics: accuracy, precision,
      recall, F1, ROC-AUC, a confusion matrix, R² and RMSE.
      Done when: each equals scikit-learn on fixed datasets (bit for bit
      where the algorithm is deterministic, within a stated tolerance
      otherwise), with a cross-validation harness.

- [ ] AI85. The data pipeline (L).
      A `Dataset` trait with map, filter, shuffle, batch, decoding in
      parallel (AI18) and prefetch; readers for MNIST (IDX), CIFAR-10, tar
      archives (`tar.fd`), the columns of a CSV (`csv.fd`, `data.fd`) and
      Parquet (AI36); crops, flips, normalisation, mixup; decoders for PNG
      and for baseline JPEG written in Fides.
      Today: `image.fd` loads PNG, JPEG and BMP through SDL2_image or
      libpng.
      Done when: an epoch of CIFAR-10 decodes at 5,000 images a second or
      better with four threads on this machine; the augmented data of a
      seed is reproducible; on 1,000 files the pixels of PNG equal PIL's
      and those of JPEG are within 1 of PIL's.

- [ ] AI86. Train, validate, and the two bars (S-M).
      The training and the validation loops as library functions; metrics
      logged as lines of JSON; the curves of AI55; and the reference runs:
      an MLP on MNIST above 97% of test accuracy and a small CNN above 99%.
      Done when: both bars are met on this machine in under 5 and under 15
      minutes (recorded with the binary), through `ccompile` and through
      `emit`, and the curves are in docs.

## I. GPU (AI87–AI96)

The CPU kernels are the oracles of the GPU kernels, so this section waits
for A, B and D. Proof at the end: the 0.5B model generates on a GPU, from
kernels written in Fides.

- [ ] AI87. Floats in `fid gpu` (M).
      f32 and f64 arithmetic, comparisons and selects, and loads and stores
      of the arguments of a kernel (a pointer to a buffer with an offset),
      translated to SPIR-V and to PTX.
      Today: "integer kernels only": calls, memory other than scalar slots,
      floats, prints and dynamic values are not translated and give 0, with
      a warning (`compiler/gpu.fd`).
      Done when: `@kernel` functions that compute `y[i] = x[i] * a + b` in
      f32, and a reduction, have no line "not translated", and their SPIR-V
      passes `spirv-val`.

- [ ] AI88. Running a kernel: Vulkan (L).
      `kern.launch([n], args)`: an instance, a device with a compute queue,
      the shader module from the SPIR-V of AI87, a pipeline, buffers, a
      descriptor set, the dispatch, the read back; on the FFI bindings of
      `stdlib/vulkan.fd` (1,387 lines of bindings and structure builders).
      Done when: a vector addition, a reduction and a GEMM give the results
      of the CPU on the driver of this machine, and in CI on a software
      Vulkan (lavapipe on Linux).

- [ ] AI89. Running a kernel: Metal (L).
      MSL for the `@kernel` functions, compiled from source at run time;
      buffers and a dispatch through the Objective-C messages of
      `stdlib/metal.fd` (which has the pipeline, the encoder and the
      dispatch).
      Done when: the three kernels of AI88 give the results of the CPU on a
      macOS runner (`macos-native.yml`).

- [ ] AI90. Running a kernel: CUDA (L).
      The PTX of `fid gpu` loaded with the driver API (`nvcuda.dll`,
      `libcuda.so`); shared memory, a barrier, the shuffles of a warp.
      This machine has an NVIDIA GeForce RTX 5060 Laptop GPU beside an
      AMD Radeon 780M, and `nvcuda.dll`, `vulkan-1.dll` and `d3d12.dll`
      are installed (Nov 28), so the gate runs here. (Until Nov 28 this
      entry said the machine had no NVIDIA GPU: written without checking.)
      Done when: the kernels run on that GPU, and the driver's JIT log (or
      `ptxas -v` where the CUDA toolkit is installed) accepts every kernel
      of `tests/ml/gpu` with the register counts recorded.

- [ ] AI91. Running a kernel: WebGPU (M-L).
      WGSL for the `@kernel` functions, run in Node and in a browser
      through `fid wasm`; the bindings of `stdlib/webgpu.fd` (a compute
      pipeline, `dispatchWorkgroups`) as the other way in.
      Done when: `tests/ml/gpu/webgpu-run.mjs` runs the kernels in CI where
      a WebGPU implementation is installed.

- [ ] AI92. Running a kernel: D3D12 (L).
      HLSL compiled at build time by DXC (the compiler does not sign DXIL);
      a root signature and a compute pipeline (`stdlib/d3d12.fd` has the
      builders); UAV barriers; the dispatch through the COM calls of the
      module.
      Done when: the kernels run on the device of this machine, and the
      numbers stand next to those of Vulkan (AI88).

- [ ] AI93. GPU kernels for the engine (L).
      The elementwise and the fused operations (AI20); reductions; a tiled
      GEMM in shared memory, with cuBLAS through FFI as the fast path where
      it exists; softmax; RMS norm; attention.
      Done when: each equals its CPU kernel within 1e-4; the throughput of
      the GEMM is recorded against cuBLAS; the 0.5B model generates on the
      GPU.

- [ ] AI94. Devices: `cpu` and `gpu` blocks, device regions (L).
      The form was settled with the user on Nov 28. Where code runs is said
      by a block: `gpu [n] { … }`, with an optional other branch
      `cpu { … }` for a machine without a GPU; `gpu fn` is what `@kernel`
      is today. Memory on a device belongs to a region, which frees it at
      its exit, the innermost region first, after waiting for its work:
      `region gpu dev with budget(N) { … }` (the syntax exists today and
      owns nothing). A second device is an instance (`gpu[1]`). `cpu` is
      the default kind: a plain `region r` is a CPU region. Another
      component (an NPU, a DSP) takes the same two forms once a backend can
      emit code for it: no keyword per part.
      Today: a CPU region is an arena, and the end of a region frees the
      Vecs and HashMaps declared in it that provably never leave it; "there
      is no check that nothing uses a region's memory after its end"
      (reference §8.6), and the checker's `freed_regions` list is written
      and never read; a `region gpu` only checks a budget that is a sum of
      sizes known at compile time (AI48 makes it a ceiling at run time);
      `is_gpu` and `is_kv_cache` are two flags of one statement.
      Done when: one source, a float kernel from AI87, runs on this
      machine's GPU from a `gpu { }` block and on its CPU from the `cpu { }`
      branch with the same result; a value that would outlive its device
      region is a compile error (ten negative tests); leaving a region
      waits for its work and frees its memory (the device allocator is back
      to zero); a model moved from CPU to GPU gives the CPU's logits within
      1e-3.

- [ ] AI95. Kernel checks (M).
      Every GPU kernel has a twin on the CPU, and the two are compared on
      every lane that has a GPU (a software one in CI); `--validate` covers
      kernels; an environment variable dumps the SPIR-V, the PTX, the MSL,
      the WGSL and the HLSL.
      Done when: `tests/ml/gpu/check.sh` is green on lavapipe (Linux), on
      the macOS runner, and on the drivers of this machine.

- [ ] AI96. A training step on the GPU (L).
      The gradients of AI66 as GPU kernels; the optimizer on the device,
      with no round trip per step; an MLP trained on the GPU.
      Convolutions of the class of cuDNN are not in this roadmap.
      Done when: MNIST reaches 97% from an MLP trained on a GPU in a stated
      time, with no copy between host and device inside an epoch (counted).

## J. Edge, robots, the plant floor, and the proof (AI97–AI100)

The standard library already speaks to robots and to industrial plants;
these four goals put a model beside that code.

- [ ] AI97. Inference without an operating system (L).
      A keyword-spotting CNN, or an MLP, through `emit --bare` on x86 bare
      metal (QEMU in CI: `bare-metal.yml`) and on Cortex-M
      (`compiler/nat_thumb.fd`), with static allocation only (`--no-std`)
      and int8 weights; and in a browser through `fid wasm` (316 of 344
      suite tests today) with the SIMD128 of WebAssembly (AI16).
      Done when: the outputs of the model equal those of the host within
      the error of the quantization; RAM and flash are stated and under a
      stated budget (256 KB of RAM for a part of the class of a Cortex-M4);
      the time of an iteration is measured on real hardware (outside this
      machine).

- [ ] AI98. A robot policy beside the stack that exists (M-L).
      A trained policy (an MLP or a small transformer) loaded from ONNX
      (AI29) and run at a fixed rate in one process with the publishers of
      ROS2 and DDS (`ros2_msg.fd`, `dds.fd`), the filters (`kalman.fd`,
      `particle_filter.fd`) and the planner (`planner.fd`); the inference
      function under `@realtime`.
      Done when: a loop of 50 Hz on this machine keeps its iteration under
      0.5 ms for 1e6 iterations (the maximum recorded), and a missed
      deadline is detected and reported.

- [ ] AI99. Anomaly detection on the plant floor (M).
      An autoencoder, or an isolation forest (AI84), over the sensor
      streams of the IEC 104, Modbus, DNP3 and MQTT modules (`iec104.fd`,
      `modbus.fd`, `dnp3.fd`, `mqtt.fd`); a bounded time of inference; an
      alarm path annotated for the safety rules
      (`docs/reference/safety-rules.md`).
      Today: the protocol modules exist; no model runs on top of them.
      Done when: on a dataset recorded and pinned in the tree the F1 of the
      detection equals that of a Python reference; the delay from a sample
      to an alarm is recorded; `fid audit --strict` is clean.

- [ ] AI100. The proof, written down (M).
      One page (under `docs/perf`, and on the website): the 0.5B model in
      f32, in q8_0 and in q4_0 — tokens per second, time to the first
      token, the size of the binary, the memory at 1, 16 and 64 concurrent
      requests — next to llama.cpp and ONNX Runtime on the same machine;
      the time and the accuracy of training on MNIST and on CIFAR-10 next
      to PyTorch on a CPU; the kernel table of AI13. The half of
      `docs/guides/from-python.md` for machine learning ("Train in
      PyTorch, serve in Fides": export, serve, a client, in ten minutes).
      positioning.md §6 corrected row by row to what exists.
      Done when: every number comes from a script in the tree that somebody
      else can run, and a person who did not write the guide followed it to
      the end.

## Order

1. AI1, then A in its order: AI2 first, so that one implementation serves
   every backend; then the types, the views, broadcasting and indexing.
2. AI13 before any work on speed, so that each kernel has a number the day
   it lands. AI14 gives the engine a fast matmul at once; AI15 is the proof
   that the language can write one itself.
3. C and D together, in this order: AI25, AI27, AI32, AI33, then AI37 to
   AI39 and AI45 (a GGUF model served over HTTP); then AI28 to AI31 (the
   way in from PyTorch) and the rest of D.
4. E: AI50 to AI52 as soon as a model runs. Nothing moves to Fides while
   Python cannot call it.
5. F whenever its base exists: AI48 and AI63 need only the server of D;
   AI60 and AI61 are their own work. This is the reason to choose
   Fides over llama.cpp or ONNX Runtime.
6. G and H: training. AI66 and AI68 are the long poles; AI86 is the bar.
7. I after the engine is right on a CPU.
8. J last; AI100 whenever a number in it has been measured.

Goals of other roadmaps these depend on: EV12 (slices, AI6), EV4 and EV9
(iterators and struct-of-arrays layouts, AI85), BG256 (function values into
tasks, AI18), TG11's two reductions (AI19), the 24-hour soak that
release-roadmap I6 moved to end-roadmap.md (AI47).

Count on Nov 27 2026: 100 goals, AI1 to AI100, all open.
