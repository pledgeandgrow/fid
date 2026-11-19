# Infra runs, Nov 16 2026 (batch 7: BM10, RT11, VF4, VF5, VF14)

Runs on a busy shared Windows 11 machine (other sessions' compilers
running), `fid` = an agent's one-stage build of the Nov 16 tree (fidh1).
Linux runs are in WSL2 (Kali, gcc 15.2, 16 cores, 15 GB).

| Goal | Check | Result |
|---|---|---|
| VF14 | `scripts/sanitize-wsl.sh` (new): the Windows `fid` writes each test's C and its `fid run` output; gcc in WSL builds it with ASan+UBSan and runs it | first run of the sanitizers on the C path. Unpatched: every program that turns a number or tensor into a string hits finding S1 (test_vec_builtins at its first `int_to_str`). With S1 patched in the C (`PATCH=`), a 45-program sample: 37 clean, 3 findings (S2, S3), 5 C-path output differences that are not sanitizer findings, 3 that `dump-c` cannot lower |
| VF14 | `SAN=thread scripts/sanitize-wsl.sh` (the 22 threaded programs of sanitize.sh's TSan set) | 21 clean, no data race reported; test_async_suspend prints differently through C also without TSan |
| VF14 | S2 and S3 patched in the C as well | test_autodiff_grad and test_lora clean, same output as `fid run`; test_free_escape then shows S4 |
| VF5 | `tests/bootstrap/test_verify_opt_ipcp_gconst.fd`, `test_verify_opt_promote_facts.fd`, `test_verify_opt_spec_devirt_icf.fd` (new) | each prints its expected output on `fid run`, `ccompile`, `emit` (pe_ir), `emit --nat`, and `emit` with `FID_IROPT_ON=ipcp,spec,icf` |
| VF5 | `FID_IROPT_STATS=1` on them | promote 48-49, hofspec 1-3, sccp 2 changed functions by default; ipcp 8, spec 49 with `FID_IROPT_ON=ipcp,spec`; **gconst 0 everywhere**, also on a 7-line program whose only global is read once (finding O1); branchfacts 0 (every case in the test is one where a fact must not be used, so only a wrong pass changes anything) |
| VF5/VF7 | five mutants added to `scripts/verify/variants.py` (ipcp-first-constant-wins, gconst-stored-folded, promote-join-first-store, facts-join-assumed, facts-float-negated); each edit matches its source exactly once | not built here (no compiler builds in this batch); the weekly `mutants` job builds them. gconst-stored-folded will survive while O1 stands |
| VF4 | `toggles-peir` job in verify-weekly.yml (new): Windows, one matrix job per `PEIR_*` variant, the whole suite and 20 generated programs | wired, not run (it builds a compiler per variant) |
| BM10 | `scripts/bench-history-check.sh` (new) | 15 checks pass: history.sh order and untouched page, weekly history file, `bench/page-merge.sh` order and idempotence, the weekly job and pages.yml wiring |
| BM10 | `.github/workflows/pages.yml` (new) | not run: needs GitHub Pages turned on (Settings > Pages > Source: GitHub Actions) |
| RT11 | `scripts/idle-bench-check.sh` (new) | server builds through C, selftest, 200 and 1000 idle connections held and counted, every ping answered |
| RT11 | `bench/idle/run.sh` on Windows; `bench/idle/idle.py` against the C build in WSL | docs/perf/rt11-idle-connections.md |
| all | `scripts/ci-wiring-check.sh` | every gate wired (BM10, RT11, VF4 lines added) |

## Sanitizer findings (VF14)

The C fixes of S1-S3 as checked here are `scripts/verify/sanitize-patches.sed`
(`PATCH=scripts/verify/sanitize-patches.sed bash scripts/sanitize-wsl.sh`
looks past them until codegen_c.fd has them).

S1. **Header probes read in front of a large or sanitizer allocation.**
`p_str_hdr_c`, `P_STR_DUP`/`P_STR_DROP` and `__p_vbuf_free` read the words
at `s - 16` and `s - 24` to tell a counted string or a tagged Vec buffer
from a plain one (known-bugs 21, 25). A block of the size classes has room
there (the block before it), but `p_malloc`'s large path (over 1 MB, and
every block under ASan: `P_SAN_ALLOC`) is `calloc(total)` with one header
word, so the probe reads 8-16 bytes before the allocation. Under ASan:
heap-buffer-overflow in the first `int_to_str`; without it, a read of
another allocation's memory, or of an unmapped page for a fresh mmap. Fix
(compiler/codegen_c.fd, checked by patching the C): give the large path
two more words in front, `calloc(1, total + 16)` and `big += 2` before the
header is written, `free(b - 3)` in `p_free`'s `P_CLASS_LARGE` branch, and
the same in `p_shared_alloc` (`+ 24`, `b += 2`).

S2. **`p_tensor_to_str` probes a libc buffer.** `p_tensor_to_str__raw`
builds its text with `malloc`/`realloc`, and `p_str_own_c` reads 16 bytes
before it (heap-buffer-overflow in test_autodiff_grad, test_lora); the
buffer is also never freed. Fix: `char* r = p_tensor_to_str__raw(t); char*
o = p_str_from_bytes_c(r, strlen(r)); free(r); return o;`.

S3. **A global `str` with a literal initializer is a plain C literal.**
`var kept_str: str = "";` becomes `static void* _g_kept_str = "";`, without
the `[P_STR_STATIC][len]` header every other literal has; the first
assignment's `P_STR_DROP` reads 16 bytes before the literal
(global-buffer-overflow in test_free_escape), and would decrement or free
whatever is there if it looked like a count. Fix (codegen_c.fd
`gen_global_init_expr`, `Expr::StringLit`): emit the headered static
(`static const struct { int64_t m; int64_t l; char s[N]; } _g_x_h = {
P_STR_STATIC_C, len, "..." };` and initialize the global with
`(void*)_g_x_h.s`).

S4. **A string captured by a returned closure is freed with its scope.**
test_free_escape `by_closure`: `val s: str = "1234567" + int_to_str(8);
return fn(x: i64) { str_len(s) + x - 1 };` — `by_closure` drops `s` when it
returns (`Str_drop`), and the closure's call reads it
(heap-use-after-free in `str_len`). Without ASan the freed block still holds
the bytes, so the test passes. The capture must take an owner (proposal
0004: keeping a value takes an owner; compiler/lower.fd closure env
build).

C-path differences seen (not sanitizer findings, output differs from
`fid run` also without sanitizers, under gcc on Linux): test_c_callback,
test_metrics_serve, test_references, test_midend_egraph_unary,
test_async_suspend. Not followed up here.

## Optimizer finding (VF5)

O1. **gconst folds nothing.** `iro_const_globals` (compiler/iropt.fd,
WP18) reports 0 folded loads on every program tried, including

```fd
var base: i64 = 100;
fn rb(d: i64) -> i64 { return base + d; }
fn main() -> i64 { println(rb(5)); return 0; }
```

whose IR is `GlobalAddr` then `Load` in `rb` and nothing else. Some use of
the global's address outside these functions (the module's own
initialisation, or the merged runtime) must mark it as escaping in phase 0.
The pass is on by default, so this is lost optimisation, not a
miscompilation.
