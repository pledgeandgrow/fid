# Loop vectorization (OP22)

`fid emit` on the machine layer (compiler/nat_ir.fd: Linux x86-64,
AArch64 and RISC-V, `--nat` Windows, macOS) turns counted loops over plain
`Vec<i64>` / `Vec<f64>` elements into a vector loop followed by the
original loop, which runs the iterations left over. The code is the section
"OP22: automatic loop vectorization" of compiler/nat_ir.fd (`ng_vz_*`); it
uses the vector operations of TG11 (docs/reference/vector-mir.md). `fid run`,
the C path (`ccompile`, `dump-c`) and pe_ir (the default Windows `emit`) do
not vectorize.

## Which loops

```fid
fn saxpy(y: & mut Vec<f64>, x: Vec<f64>, k: f64) {
    for i in 0..x.len() {
        y.set(i, k * x.get(i) + y.get(i));
    }
}

fn xor_all(a: Vec<i64>) -> i64 {
    var s: i64 = 0;
    var i: i64 = 0;
    while i < a.len() {
        s = s ^ a.get(i);
        i = i + 1;
    }
    return s;
}
```

A loop is taken when, in the IR the machine layer gets:

- its header loads a counter `i` (an `i64` local), compares `i < n` (signed)
  with an invariant `n` and branches into the body or out;
- its body is one to four blocks in a chain, the last jumping back to the
  header (a `for` loop's latch block included), and holds only:
  - loads of the counter, used as the index of element reads and writes and
    in `i = i + 1`;
  - element reads `v.get(i)` / `v[i]` of `Vec<i64>` or `Vec<f64>` values that
    do not change in the loop;
  - lane arithmetic on them and on invariants: `f64` `+ - * /`; `i64`
    `& | ^`, and `+ -` only where they wrap (a `@wrapping` function, or
    checks.fd proved the operation cannot overflow), so that no overflow
    panic is lost;
  - element writes `v.set(i, x)` / `v[i] = x`, after every read;
  - at most one reduction `s = s op lane` into an `i64` local, `op` one of
    `^ | &` or a `+` that wraps. An `f64` sum is never taken: adding in lanes
    changes the order, and so the rounding;
  - the increment `i = i + 1`.

Invariants are constants, parameters and values from before the loop,
loads of locals and of parameter pointers (`& mut Vec` read in the loop),
and the length of an invariant Vec. The loop's only stores are the counter,
the accumulator and Vec elements, so none of these change in it. Every
access is at index `i`, so two iterations never touch the same element,
even when two of the Vecs are the same Vec (`v.set(i, v.get(i) * k)`).

Loops left as they are include: a checked `+` or `-` on `i64` (except in
a `@simd` function, which takes them after a run-time pre-scan, and unrolls
its vector loops twice: docs/reference/simd-annotation.md (K10)), an `f64`
accumulator, a read after a write, the counter in the arithmetic
(`a[i] + i`), an index other than `i` (`a[i + 1]`), a call other than the
element accesses and `Vec_len`, a loop that needs more than 16 vector
values at once (more than four go to frame slots, below).

## The code

Where the loop's preheader jumps to its header, the vector loop comes first:

1. checks, at run time: `i >= 0`, `n >= lanes`; for each Vec, 8-byte words
   (code word 0) and a length of at least `n`; for each Vec written, not
   shared (`cap >= 0`) and owning no elements (the conditions of Vec_set's
   fast path). When one fails, the vector loop is skipped and the loop runs
   as before. When they hold, no iteration in `[i, n)` can fail a bounds
   check, so running `lanes` of them at once changes nothing but the speed;
2. invariants used as lanes are splat into vector registers, the
   accumulator gets the identity (0; all ones for `&`);
3. the vector loop, while `i < n - (lanes - 1)`: per element read a vector
   load, per operation a vector operation, per element write a vector store,
   per reduction step one operation into the accumulator; `i += lanes`;
4. after it, the accumulator's lanes are folded into `s` and `i` and `s` go
   back to their locals; the original loop then does the rest (the scalar
   tail).

Vectors are 16 bytes (2 lanes: SSE2, NEON, RVV at VL = 2 x e64; RISC-V
needs `--cpu rv64gcv` or `rva23u64`), 32 bytes (4 lanes) under AVX2
(`--cpu x86-64-v3|v4`) unless the loop has a reduction (lane extraction
reads lanes 0 and 1 only). The four registers of the vector class are
assigned by the vectorizer itself: splats and the accumulator for the whole
loop, lane values from their definition to their last use. A loop needing
more (TG11) is assigned again over up to 16 vector homes: the accumulator
and the first splats keep registers 2 and 3, the other values live in 16-
or 32-byte frame slots past the locals, and each operation goes through
registers 0 and 1 (docs/reference/vector-mir.md, "Vector values in frame
slots"; `FID_NAT_OFF=vzspill`: such a loop stays scalar). Its dump line
ends in `spilled to N vector slots`.

## Switches and dump

- `FID_NAT_OFF=vectorize`: no loop is vectorized (`FID_NAT_OFF=simd` does
  not affect it; it is for the simd module's intrinsics).
- `fid emit <file> --stats` or `FID_NAT_ON=vzdump`: a line on stderr per
  counted loop, taken or not:

  ```
  [vectorize] saxpy: bb1 vectorized: 16-byte vectors (2 lanes), 2 reads, 1 writes, 1 splats
  [vectorize] xor_all: bb1 vectorized: 16-byte vectors (2 lanes), 1 reads, 0 writes, 0 splats, reduction ^
  [vectorize] fsum: bb1 not vectorized: a store into memory other than an i64 local (an f64 sum, a field)
  ```
- `FID_NAT_DUMP=<fn>`: the function's machine IR, where the vector loop's
  `vld`, `vbin`, `vst`, `vsplat`, `vlane` and `vend` show.

## Tests

- tests/bootstrap/test_vectorize.fd: loops taken and loops left, lengths
  0..9, 33 and 1001, a shared buffer written (the run-time check skips the
  vector loop), a Vec written in place. The expected output is
  `fid run`'s.
- tests/nattools/vectorize_check.sh `<fid>`: `fid run` and `ccompile`;
  FID_VERIFY=1 builds for SSE2, AVX2, NEON, RVV and `--nat`; the
  `[vectorize]` lines of the loops that must and must not be taken; vector
  operations in their MIR; 32-byte vectors under AVX2; the images run with
  the expected output (qemu-riscv64 at VLEN 128 and 256); the same with
  `FID_NAT_OFF=vectorize` and without V on RISC-V.
- tests/nattools/vectorize_bench.sh `<fid>`: saxpy, an elementwise add and
  an xor reduction over 2^20 elements, 100 times, vectorized and not.

## Numbers

Nov 21 (roadmap OP22 check, fid built from a self-hosted stage 0):
vectorize_bench.sh gives 534 ms vectorized against 1,058 ms scalar on
`--nat`. test_vectorize prints its 39 lines, and every image (SSE, AVX2,
NEON, RVV at VLEN 128 and 256, and the off builds) prints the expected
output. Reductions (xor/or/and/sum accumulators, float sums) are never
vectorized ("the accumulator read in the loop header"), and
vectorize_check.sh fails 22 checks: its functions are now inlined into
`one()`, and it passes MSYS paths to wsl.exe.

## Left

- A vector register class the allocator assigns (the vectorizer picks its
  own registers and frame slots; loops over 16 vector values stay scalar),
  and spilled loops keep every lane value in memory (a smarter split would
  keep more in registers 0 and 1).
- Reductions under AVX2 (lanes 2 and 3 cannot be read yet), `f64`
  reductions (only with a flag allowing reassociation), integer `*`
  (no 64-bit vector multiply before AVX-512), `f32` lanes (Vec<f32> stores
  8-byte words), strides and offsets other than `i` (`a[i + 1]`), loops
  whose counter steps by more than 1, and loops with conditions in the body.
- pe_ir (the default Windows `emit`) and the C path do not vectorize.
