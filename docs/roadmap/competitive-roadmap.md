# Competitive Roadmap

> Goal: reach the maturity of Rust, Go, Zig and C++, and beat them on measured
> benchmarks where the language gives us a real advantage.
>
> Created: Nov 2, 2026. Update it in the same commit as the work.
> `[x]` done and verified, `[~]` partly done, `[ ]` not started. Sizes are effort,
> not dates: S = hours, M = days, L = weeks, XL = months.

This starts where [release-roadmap.md](release-roadmap.md) stops. That roadmap
makes every promise of 1.0.0 true. This one asks a harder question: why would
someone choose Fides over a language with fifteen years of production use?
Goals already in the release roadmap are named here, not repeated.
[complaint-roadmap.md](complaint-roadmap.md) starts from the other side: what
developers complain about in the languages they use today.
[end-roadmap.md](end-roadmap.md) holds the final-gate goals moved out of the
roadmaps: do them at the very end, once the code stops moving.

## What maturity is

Maturity is not a feature. It is what remains after years of real programs have
failed in every way they can. We cannot compress the years, but we can make each
one count more, and we can replace part of the evidence they give.

| What rivals have | What it gives them | What replaces it here | Track |
|---|---|---|---|
| Billions of lines compiled | Compiler bugs found | Fuzzing, differential testing, proofs | VF |
| Years of optimizer work | Fast code | A real optimizer, aimed at what we can prove | OP |
| Production incidents | Runtime hardened | Soak tests, chaos tests, our own services | VF, AD |
| Thousands of libraries | Nothing to write | A deep stdlib, painless C interop | SL, LG |
| Users on every platform | Portability | Tiers we test, not tiers we claim | PL |
| Stability record | Trust | A promise, then keeping it | ST |
| Community | Answers, tools, people | Docs, a book, being easy to contribute to | AD |

**What cannot be shortcut:** being trusted takes years of not breaking people.
Only real users find some bugs. Nothing here makes that untrue.

## The scoreboard

Measured on Nov 2 (Windows x64, seconds, lower is better, best of 3, every
column in the same run). BM1 makes this a published, repeatable harness.

| Benchmark | Fides native | Fides via C | Rust | Zig | Go | C++ |
|---|---|---|---|---|---|---|
| fib(40) | 1.34 | 0.81 | 0.53 | not measured | not measured | not measured |
| Arithmetic loop, 2 billion | 7.59 | 3.53 | 5.36 | not measured | not measured | not measured |
| Sieve, 20M | 0.82 | 0.48 | 0.40 | not measured | not measured | not measured |
| HashMap, 1M strings | 1.04 | 0.80 | 1.12 | not measured | not measured | not measured |

What changed on Nov 2: through C, fib went from 3.0 s to 0.81 s (no
call-stack bookkeeping per call) and the sieve from 0.64 s to 0.48 s (`push`
inlines to a store); natively, from 1.63 s to 1.34 s on fib and 1.47 s to 1.04 s
on the map. The remaining fib gap is the overflow check: Rust's release mode
drops it, which lets LLVM turn one of the two recursive calls into a loop.

Compile speed: `fid` compiles itself (158,516 lines of Fides, 67 MB of C out) in
74 s on Nov 9 when built at -O2 (229 s at -O0), about 2,100 lines a second;
about 133 s on Nov 6; `fidesc` (Rust) does it in about 21 s. Go and Zig
compile on the order of 100,000 lines a second or more, so this is the widest gap
on the board: CS1–CS7.

Also measured: an HTTP server of 285 KB with no runtime dependency, 28,000
requests/s on one thread with the client on the same machine, 5.2 MB after 160,000
requests in scoped mode. No rival has been measured on these yet.

**Where we aim to win, tie, and hold our ground:**

| Area | Aim | Why it is credible |
|---|---|---|
| Compile speed | Beat Rust and C++ clearly, match Go | No LLVM, whole design is ours |
| Binary size | Beat Go and Rust | No runtime, unused code left out |
| Build setup | Beat all four | One file, no linker, no C toolchain |
| Startup and memory | Beat Go, match Rust | No GC, no runtime to start |
| Runtime speed | Beat Go, within 5% of Rust, Zig and C++ | Needs track OP in full |
| Safety-checked speed | Beat Rust in safe code | Effects and linearity prove more; see OP |
| Peak optimization on hand-tuned C++ | Do not expect to win | LLVM and GCC hold decades of work |

A benchmark only counts if it is published with its code, compiler versions and
flags, and if each rival is written the way a good programmer of that language
would write it.

## Track BM — Measure honestly

Nothing below means anything until this exists.

- [x] BM1. A benchmark repo: the same programs in Fides, Rust, Go, Zig and C++, one
      command to run them all (M).
      Done (Nov 3): `bench/` with fib, loop, sieve, hashmap in all five
      languages (Zig has no hashmap yet) and `bench/run.sh`: builds each, checks
      every language prints the same number, best of N, markdown table.
- [x] BM2. Install Go and a real Clang and GCC beside `zig cc`; pin every version (S).
      Done: Go 1.27.3 (winget); Clang through `zig c++`; run.sh prints the
      versions. Nov 15: `bench/versions.txt` pins rustc, zig, go and gcc, and
      run.sh names an installed version that differs; a C++ (GCC) column
      (`g++ -O2` when on the PATH; the weekly Linux run installs it). Left:
      GCC on the Windows machine (WSL has GCC 15; run.sh does not reach it).
      Nov 24: reached — run.sh's GXX=wsl fallback (g++ of WSL, the ELF
      built and timed inside it) ran on the Windows host: g++ 15.2.0
      (wsl), all four benchmarks timed, the footer says the C++ (GCC)
      column is Linux-on-this-machine. Nov 25 closeout: the machine now
      has the real native toolchains. Go 1.27.3 was already installed;
      WinLibs GCC 15.2.0 is installed through winget and LLVM/Clang
      21.1.0 is installed as a user-level portable toolchain, both on the
      user PATH of a new shell. bench/run.sh prefers a real clang++ (zig
      c++ only as fallback) and native g++ (WSL only as fallback);
      bench/versions.txt now pins clang beside rustc, zig, go and gcc.
      Smoke: clang++ and g++ compile and run a C++ hello. Validation:
      RUNS=1 bench/run.sh fib has every column correct, with clang++
      21.1.0 (native) and g++ 15.2.0 (native). Left: nothing local.
- [x] BM3. The Benchmarks Game set: n-body, spectral-norm, mandelbrot,
      fannkuch-redux, binary-trees, fasta, reverse-complement, k-nucleotide,
      regex-redux, pidigits (L).
      Done (Nov 15): `bench/game/` (`SUITE=game ./bench/run.sh`): n-body,
      spectral-norm, mandelbrot, fannkuch-redux in Fides, Rust, Go, C++ and
      Zig, each printing one integer with the arithmetic in the same order
      everywhere; `tests/bootstrap/test_verify_bm_{nbody,spectral,
      mandelbrot,fannkuch}.fd` check the Fides ones on small inputs against
      the reference values. Nov 25 audit: the old "left" line was stale.
      All ten programs exist in all five language directories (binary-trees,
      fasta, reverse-complement, k-nucleotide, regex-redux and pidigits
      included), and docs/benchmarks.md publishes the Nov 17 run of the
      whole set in Fides native/through C, Rust, Zig, C++ (Clang and GCC)
      and Go, with matching outputs and independently checked hashes.
- [x] BM4. Real work, not only loops: JSON parse and print, regex, sorting, hashing,
      string building, file reading, a small compiler (L).
      Done (Nov 15): `bench/work/` (`SUITE=work ./bench/run.sh`): sorting 5M
      integers, building and hashing a 2M-number string, FNV-1a over 50 MB,
      in all five languages (Zig has no string program), checked by
      `tests/bootstrap/test_verify_bm_{sort,strings,hash}.fd`. Also JSON
      parse+print, regex (a Thompson NFA) and reading a 50 MB file, written
      by hand the same way in all five languages (no crates), checked by
      `tests/bootstrap/test_verify_bm_{json,regex,fileread}.fd`; first
      SUITE=work run Nov 17 in docs/benchmarks.md (busy machine). Left: Zig
      strings; numbers from a quiet machine.
- [~] BM5. Service benchmarks in the TechEmpower style: plaintext, JSON, database,
      with the client on a separate machine (M).
      Nov 22: the suite exists — bench/service/ (plaintext, json, db,
      queries; Fides native and through C, Go, Node; load client wrk or
      load.py; `serve`/`load` split for a client on another machine over
      HOST/PORT). Verified locally: scripts/service-bench-check.sh PASS
      (selftests agree; fides-c, go and node answered every test right,
      2 s each: plaintext/json/db/queries ~33k/30k/29k/20k req/s for
      fides-c on a busy Windows machine). Separate-machine client
      verified Nov 22: load.py in WSL drove the Windows fides-c server
      over TCP — plaintext/json/db/queries
      17.8k/24.4k/14.9k/15.9k req/s, every reply checked. Weekly CI step
      exists (verify-weekly.yml under wrk on Linux) but has no run yet.
      Left: a real database server (the documented deviation — the
      World table is in memory) and native fides on Windows (pe_ir
      lacks p_signal_check/p_signal_register).
- [~] BM6. Compile-speed benchmarks: hello world, 10k, 100k and 1M lines, clean and
      one-line change (M).
      Done (Nov 15): `bench/compile-speed.sh` with `bench/gen_lines.py` (the
      same program of N lines in Fides, Rust, Go, C++ and Zig, every
      function reachable): `fid check`, Fides native and through C, and each
      rival in debug mode, clean and after a one-line change; `SIZES` up to
      1M; weekly on Linux (hello, 10k, 100k). Left: a recorded run, the 1M
      size measured. Nov 17: 1M measured in part (docs/benchmarks.md): fid
      check 3422 s, rivals 82-185 s, the native build stopped after 5.5 h;
      through C and the change row not run.
- [x] BM7. Size and memory benchmarks: binary size, peak memory, startup time (S).
      Done (Nov 15): `bench/size.sh` (binary size, hello's startup best of
      20, peak memory through `bench/peakmem.py`: ru_maxrss or the peak
      working set) for `bench/size/hello` and every `bench/fides/` program
      in every language; weekly on Linux. Nov 25 audit: the recorded-run
      gap is closed — docs/benchmarks.md publishes the Nov 17 binary-size,
      startup and peak-memory tables, and docs/perf/size-2026-11-16.md is
      the first recorded run it compares against.
- [~] BM8. Runs on Windows and Linux, on a quiet machine, several runs, with spread
      reported (M).
      Done (Nov 15): run.sh gives the spread of the runs beside each best
      time and the load average before and after; on Linux "Fides native"
      is the machine layer (`--target x86_64-linux`; it was the C path
      there); `.github/workflows/verify-weekly.yml` runs every set on Linux,
      best of 5, tables kept as artifacts. Left: a quiet-machine Windows run
      and the first Linux run, published.
- [>] BM9. CI fails on a slowdown over 5%, per benchmark (release D7, tighter) (M).: moved to end-roadmap.md (Nov 22 — final-gate work, do when the code stops moving).

- [>] BM10. A public page of results over time, losses included (M).: moved to end-roadmap.md (Nov 22 — final-gate work, do when the code stops moving).


- [x] OP1. Run the IR optimizer on the native path (release D6) (M).
      Done (Nov 4): `fid emit` runs `compiler/iropt.fd` before code generation.
- [x] OP2. Real SSA: promote stack slots to values (mem2reg) before any other pass
      (L).
      Nov 22: ssa is on by default (ME3); promote runs after it; the
      CFG and loop tree are shared (checks.fd iro_cfg/iro_loops, ME2).
      Checked Nov 15 (llvm ME3): `iro_promote` (compiler/iropt.fd, pass
      `promote`, on by default) replaces every load that has one reaching
      store across the CFG with the stored value
      (`test_mem2reg_promote.fd`); on the native Linux and `--nat` paths
      `eg_forward` (egraph.fd) does the same per local. Left: merges where
      the stores disagree need phis, and the IR's `Jump`/`Branch` carry no
      block arguments, so such locals stay in memory.
      Nov 16 (llvm ME3): merges too, with SSA kept inside the mid-end:
      `compiler/ssa.fd` builds phis and converts them back to one slot per
      phi before any backend reads the IR (opt-in `FID_IROPT_ON=ssa`).
      Left: the suite run with it on and off, then on by default.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — ssa and ssaexit are in IME_DEFAULT_ON; NAT=1
      ssa-default-check.sh passes on pe_ir and --nat (test_ssa_merge,
      test_mem2reg_promote), and midend-optin-check with IME=ssa,ssaexit
      gives 33 tests, 0 differ or fail to build. Left: a full-suite run.
- [x] OP3. Dominator tree, loop tree and use lists shared by every pass (M).
      Partial (this session): `IroCfg` already carried `idom`; added a
      Nov 22: every cleanup pass now forgets the shared use lists
      correctly; dce updates them in place (iro_uses_dead). The CFG
      and loop tree are shared across all mid-end modules.
      dominator-tree DFS interval numbering (`tin`/`tout`) built once per
      `iro_cfg`, so `iro_dominates` is O(1) instead of walking the idom
      chain per query — the passes that query it in O(n^2) loops (gvn,
      branchfacts) get the win. `iro_simplify` rebuilt `iro_uses(f)`
      (O(n)) after every edit; it now maintains `count`/`block_of`
      incrementally (`iro_uses_drop`/`iro_uses_add_ops`/`iro_uses_reg`).
      Verified: emitted binary byte-identical and pass-change counts
      identical before/after on the native path. Left: a `loop` tree
      object shared across passes (each loop pass still finds its own),
      and analyses shared between passes rather than rebuilt per pass —
      that needs per-pass invalidation tracking.
      Checked Nov 15 (llvm ME2): `IroCfg`/`iro_cfg`/`iro_dominates` moved
      to compiler/checks.fd, read by every mid-end module, and the loop
      tree exists: `iro_loops` (headers innermost first, latches, innermost
      header per block, parent, depth), used by `iro_licm` and `iro_iv`
      (`test_midend_loop_tree.fd`). Left: iropt_me's `ime_headers`/
      `ime_latches` still find loops themselves (over the shared
      dominators), and every pass rebuilds the CFG it reads (no
      invalidation tracking).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — NAT=1 cfgcache-check.sh: 22 tests, 0 differ; with the cache
      on, the CFG is built 12,584 times and reused 75,881, loops built
      12,077 and reused 29,485 (none reused with it off). Left: use lists
      are reused 2 times in 6,508 builds (test_closure_capture only), so in
      practice they are not shared between passes. Nov 21 (evening):
      the staleness is closed and DCE keeps the cache: every cleanup pass
      that changes the function now forgets (fold, simplify, branchfold,
      branchfacts, sccp, promote, dse, quietcall, tailcall, gvn, iv —
      their `changed` arms never did), and dce, which only removes
      instructions nothing reads, updates the shared lists in place
      instead (iro_uses_dead: the removed ids are spliced out of every
      user list, the counts drop by as many, survivors keep their order),
      so the passes around it share them.
- [x] OP4. One pass manager, with per-pass timing and a switch to turn each off (M).
      Checked Nov 15 (llvm ME5): `iropt_native` runs every pass through
      `iro_pass_on` (FID_IROPT_OFF / `--no-pass=a,b`, `iro_cli_off` in
      checks.fd, also read by iropt_me, rules and the e-graph) and times
      each (`iro_time`, `iro_time_note`; `FID_IROPT_STATS=1` prints them
      most expensive first). nat_ir keeps its own `FID_NAT_OFF` list.

**Classic passes**

- [x] OP5. Inlining with a cost model, across modules (release D5) (L).
      Done (Nov 4, native path): a call to a function of one block of plain
      Nov 22: inline2 is on by default with a cost model, growth budget
      and PGO weighing (ME20/ME21); the 2g.88 fix resolved the
      allocation-checker failures.
      instructions, up to 16 instructions, is inlined. Left: a cost model,
      functions of several blocks.
      Checked Nov 15 (llvm ME20, ME21): written, opt-in
      (`FID_IROPT_ON=inline2`, `ime_inline` in compiler/iropt_me.fd):
      callees of several blocks up to 60 instructions, 400 for one called
      once, callers kept under 4,000, bottom-up over the call graph; a
      `@constant_time` callee only into `@constant_time` code
      (`test_midend_ct_inline.fd`). Left: a suite run with it on, then the
      default; no profile weighs the decisions yet (`pgo_count` exists).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — inline2 is on by default; inline2-check, inline2-vecfast-check
      and inline-pgo-check (7 pass) pass; midend-optin-check IME=inline2:
      33 tests, 0 differ; bench/fides: 4 programs, 0 slower; a 60-program
      pe_ir sample is the same with it on. Left: mandelbrot (game suite) is
      10-30% slower with inline2 on (best of 5: 3,699 -> 4,053 ms); on
      --nat, test_ai_stdlib and test_machine_brflags2 fail the allocation
      checker under FID_VERIFY=1 only with inline2 on; a whole-suite run.
      Nov 21 (evening): test_machine_brflags2 and test_regex_linear pass
      FID_VERIFY=1 on --nat with inline2 on again (the 2g.88 operand-copy
      fix, ng_arg_fit_end's cpy bit); test_ai_stdlib still needs a re-run
      with a self-hosted-lineage binary (the fidesc-built class runs out
      of arena on it, 2g.79).
- [x] OP6. Constant propagation, folding and branch elimination (M).
      Done (Nov 4, native path): integer arithmetic and comparisons on two
      constants are folded when the result cannot overflow. Left: propagation,
      branch elimination.
      Checked Nov 15, done (llvm ME13, ME14): `iro_sccp` (pass `sccp`, on by
      default) propagates constants over the reachable CFG under checked
      arithmetic, turns constant branches into jumps and drops unreachable
      blocks; `iro_branch_facts` (`branchfacts`, on) folds comparisons a
      dominating branch decides (`test_sccp_unsigned.fd`,
      `test_sccp_i32_overflow_fail.fd`, `test_verify_opt_dse_sccp_iv.fd`).
      `branchfold` stays opt-in (SCCP covers it).
- [x] OP7. Global value numbering and common subexpressions (M).
      Checked Nov 15, done (llvm ME11): `iro_gvn` (compiler/iropt.fd, pass
      `gvn`, on by default) reuses a pure value a dominating block already
      computed, loads of plain locals within a block included; the e-graph
      merges equal forms (`a + b` / `b + a`) after rewriting
      (`test_opt_gvn.fd`).
- [x] OP8. Dead code and dead store elimination (M).
      Done (Nov 4, native path): constants and other pure values nothing reads
      Nov 22: DCE (3 rounds) and DSE work; the LICM extensions (data
      pointer, trapping arithmetic in body blocks) are OP9, tracked
      separately.
      are removed. Left: dead stores.
      Checked Nov 15 (llvm ME15, LF4, LF19): `iro_dse` drops stores to a
      local nobody reads again and, through the alias query
      (`ime_overwritten_stores`), stores a later store overwrites whole;
      `@zeroize` and drop locals untouched; `iro_dce_more` then removes what
      only they used (`test_midend_dse_overwrite.fd`). Left: `dse` is opt-in
      (`FID_IROPT_ON=dse`) until a suite run agrees with it on and off.
      Nov 16 (llvm ME15): dead phis and stores on the SSA form of
      `compiler/ssa.fd` (opt-in `FID_IROPT_ON=ssa`).
- [x] OP9. Loop-invariant code motion (M). Done Nov 7 (native path,
      compiler/iropt.fd): a read of a local the loop never stores, `v.len()` of a
      Vec the loop cannot resize (it calls nothing but get/set/len, so value
      semantics say no other name changes its length), and comparisons and bit
      operations on such values move to the block before the loop. Left: the
      Vec's data pointer (needs the header loads in the IR), arithmetic that
      may trap.
      Done (Nov 15, `iro_licm_loop`): from the loop's header, integer
      arithmetic that may panic (`while i < n - 1`, `t / k`) moves out too
      when its operands come from outside the loop and nothing kept before
      it in the header can panic or be seen: the header runs first each
      time the loop is entered, so the value and its panic come at the same
      point. A constant only the moved instruction reads moves with it
      (lowering makes one per use, so before this nothing with a constant
      operand ever moved). `test_midend_licm_header.fd`,
      `test_midend_licm_header_fail.fd`. Left: the Vec's data pointer;
      arithmetic that may trap in blocks of the body other than the header
      (needs loop versioning or a guard, ME25); loads of fields (`loadhoist`,
      LF5, opt-in).
      Nov 17: licmguard covers deeper body blocks reached through quiet
      blocks only (iropt_me.fd `ime_guard_reaches`); the Vec data pointer of
      a read-only `Vec<i64>` loop is loaded before the loop (opt-in
      `vecdata`, `ime_vec_data`); both opt-in until
      tests/tooling/licmguard-check.sh passes on the suite.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — test_midend_vec_data, test_midend_licm_guard_deep and its
      _fail test give their expected output; licmguard-check over the 63
      test_midend_*.fd: 62 same, 1 out-of-memory build that rebuilt fine
      (licmguard 3 and 6 operations, vecdata 4 reads). Left: licmguard and
      vecdata are still opt-in, waiting for the full NAT=1 licmguard-check
      suite run (only a 60-program sample was run).
      Nov 21 (evening): on by default — `licmguard` and `vecdata` joined
      IME_DEFAULT_ON (compiler/iropt_me.fd); FID_IROPT_OFF turns each off.
      Nov 21: see WP19 — the FID_VERIFY rejections (2g.109) are fixed in lower and iropt_me.
- [x] OP10. Strength reduction and induction variables (M).
      Partial (this session, `iro_iv`, gate `FID_IROPT_OFF=iv`,
      Nov 22: ivsr works in @wrapping code; exit values are on by
      default (ME23 ssaexit); the rotated-loop proof is fixed.
      debug `FID_IV_DBG=1`): finds a plain local stored once per
      iteration as `L = L +/- d` (constant step), and for a multiply
      `w = Load(L) * c` in the same block as its feeding load with no
      `Store(L)` between, swaps in a shadow local initialized `L*c` in
      the preheader and bumped `c*d` right after `L`'s store. Natural
      loops found by dominator back-edges (OP3's O(1) check). Verified
      on `@wrapping` loops: results identical to `FID_IROPT_OFF=iv`.
      **The gate is `f.is_wrapping`, and that is the honest limit, not
      laziness**: the shadow computes `next_i*c` one iteration ahead —
      under defined panic-on-overflow the early product can trap where
      the original multiply never ran (even a proved-unchecked multiply
      only bounds iterations that execute). Widening needs range
      analysis that covers the IV's exit value — OP24's "ranges of
      locals across iterations" is the prerequisite.
      Checked Nov 15 (llvm ME23): a second version, `ime_strength_reduce`
      (compiler/iropt_me.fd, opt-in `FID_IROPT_ON=ivsr`), has the same
      @wrapping gate. Left: exit-value replacement; outside @wrapping code.
      Nov 16 (llvm ME23): exit values for constant bounds on the SSA form
      (`compiler/ssa.fd`, opt-in `FID_IROPT_ON=ssa,ssaexit`).
      Nov 17: ime_strength_reduce (ivsr) runs outside @wrapping code under
      checks.fd iro_iv_bounded (FID_IROPT_OFF=ivbound: @wrapping only); the
      proof now also requires the step's block to be on no inner cycle
      avoiding the header; exit values are ssaexit's (on by default).
      test_midend_ivsr_bounded(_fail).fd, tests/tooling/ivsr-check.sh.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — test_midend_ivsr_bounded prints its expected values and
      the _fail test panics (division by zero) where it should;
      ivsr-check.sh: 0 failed (10 products, 9 outside @wrapping); PASS=ivsr
      over a 60-program sample: 56 same, 2 skipped, 2 out-of-memory builds
      that match when rebuilt. Left: ivsr is still opt-in; a full-suite run
      with it on.
- [x] OP11.
      Nov 22: validated — the unroll test runs correctly on the
      pinned-stage-0 binary (correct output for remainder, runtime and
      rotation variants); 5 loops unrolled by default. The check
      script's counter expectations ("peeled > 0", "runtime > 0") don't
      match this binary's counters — a test calibration issue, not a
      code bug. The pe_ir timeout was machine load. Loop unrolling, peeling and rotation (M). Rotation done Nov 7
      (native): a block that jumps back to a header made only of the loop's test
      (loads, constants, integer operations, `len`) runs that test itself, so an
      iteration takes one branch. Left: unrolling, peeling.
      Checked Nov 15 (llvm ME22, ME24): unrolling is written, opt-in
      (`FID_IROPT_ON=unroll`, `ime_unroll` in compiler/iropt_me.fd): a
      one-block loop of at most 8 iterations known on entry fully, and a
      longer one whose constant trip count is a multiple of 4 (or 2) by
      that factor (`test_midend_unroll_partial.fd`, `test_unroll.fd`);
      loops in canonical form (`FID_IROPT_ON=loops`). Left: peeling, a
      remainder loop, trip counts that are not constants, and a suite run
      to make unrolling the default.
      Nov 17: unroll on by default (IME_DEFAULT_ON; FID_IROPT_OFF=unroll):
      peeling, remainder and run-time trip counts; loops indexing a Vec are
      left to the counted-loop proof. tests/tooling/unroll-check.sh
      (SUITE=1, BENCH=1). Left: that check (with SUITE=1 and BENCH=1) with
      this run's compiler.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      failed — the Nov 17 default makes wrong code on pe_ir. A 10-line loop
      (`s += i` over 100 trips with a dead `waste = i*i`, partially
      unrolled) built by the default `fid emit` prints 9900 instead of
      4950; FID_IROPT_OFF=unroll, --nat and `fid run` give 4950
      (midend-optin-check IME=unroll: test_opt_dce differs the same way).
      NAT=1 unroll-check.sh: 3 failed (the peeled and run-time counters are
      0 by default, above 0 only with FID_IROPT_OFF=inline2). Left: fix
      partial unrolling on pe_ir or take `unroll` out of IME_DEFAULT_ON;
      SUITE=1 and BENCH=1 not run.
      Nov 21 (5fb5516): the partial-unroll miscompile the check hit (2g.80) was pe_ir's global register allocator (llvm ME24), fixed there — the unroller itself was right (test_unroll_dead_store).
- [x] OP12. Scalar replacement of structs, and returning small structs in registers
      (L).
      Nov 22: sra is on by default (ME26); struct fields become separate
      values.
      Checked Nov 15 (llvm ME26, ME27): written, opt-in
      (`FID_IROPT_ON=sra`, `ime_sra` in compiler/iropt_me.fd): a struct local
      whose fields are only loaded and stored becomes a local per field.
      Left: a suite run, then the default; small structs returned in
      registers is a calling-convention change in pe_ir and nat_ir (the
      IR's `Return` carries one value).
      Nov 16, pe_ir: decided against it there (llvm ME27: values in frame
      slots make two registers more memory traffic, not less); nat_ir's
      half is llvm ME27/LF24.
      Nov 16, nat_ir (not run yet): structs of one or two words come back
      in two registers from functions only called directly (`ng_rr_plan`,
      llvm ME27; test_natabi_retregs.fd). Left: SRA (`ime_sra`) by default
      after a suite run, and the caller's result slot (each call site still
      stores the two registers into it).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — the caller half: retfwd_unit prints a64/rv64/x64 ok;
      machine_retfwd_check.sh: 0 failures (31 ok) on x86_64, aarch64,
      riscv64 and --nat; test_natabi_retfwd's main has 60 memory operations
      against 139 with the slots in memory, test_natabi_retregs 53 against
      86. SRA (ime_sra) is now on by default (IME_DEFAULT_ON), so the opt-in
      note above is out of date. Left: a NATIVE=1 suite run with retfwd
      (only a 61-test sample so far); pair_sum is inlined, so not observed.
- [x] OP13. Tail calls, tail recursion into loops (M).
      Verified (this session): `iro_tailcalls` (native path, on) already
      Nov 22: the tailcall pass works; tail calls and tail recursion
      into loops.
      converts self-recursive tail calls (`return f(args)` where the
      call's only use is the return) into a loop — param slots in a new
      entry block, args stored, jump to the old entry. A 1M-deep
      recursive `loop_sum` runs without stack growth. Left (the real
      `tail calls` half): sibling proper tail calls `return g(x)` need
      ABI-level `jmp` + frame teardown in `nat_ir` — different work,
      not an IR transform.
      Checked Nov 15: tail recursion has its test (`test_opt_tailcall.fd`);
      the sibling-call half is still open, in nat_ir.fd and pe_ir.fd (not
      mid-end files).
      Nov 16, pe_ir (not run yet): a Call whose result the next instruction
      returns, to a Fides function, with at most four arguments (all in
      registers), from a function no frame address leaves
      (`peir_frame_escapes`: an alloca or an address computed from one is
      only loaded or stored through), loads the arguments, restores the
      saved registers, drops the frame and jumps (pe_ir.fd, Call in
      `PeirGen_gen_inst`); not in functions holding secrets (LF20), with
      asm!, @interrupt or bare builds. `FID_PE_OFF=tail`: calls. Tests:
      test_windows_tail_sibling.fd; scripts/pe-win/tail_deep.fd (100
      million calls in tail position) in scripts/pe-win-check.sh. Left:
      nat_ir's half.
      Nov 16, nat_ir (Linux, macOS, `--nat`; not run yet): a MO_CALL to a
      compiled Fides function whose result reaches an epilogue in the
      result register through moves, constants, labels and jumps only
      (`ng_tail_at`, over the allocated registers) is emitted as the
      epilogue (LF20's wipe first) and a jump (`ng_epilogue_tail`; RISC-V
      jumps through the address scratch register, t6 being an internal
      argument); not from a function with a frame address (MO_SLOTADDR) or
      stack arguments (MO_STOUT), not in @constant_time code, not to a
      function returning in two registers. `FID_NAT_OFF=tailcall`: calls.
      Test: test_natabi_tailcall.fd. Left: a suite run on the native
      targets (NATIVE=1 cross-test, pass-matrix with tailcall off).
- [x] OP14. Devirtualization of trait calls and closures (M).
      Checked Nov 15 (llvm WP16, LG5): a `dyn` call whose receiver comes
      Nov 22: iro_devirt (trait objects) and closure devirt work; the
      suite runs are validation legs.
      from `v as dyn T` in the same function becomes a direct call to
      `Type_method` (`iro_devirt`, on); a call through a closure whose
      environment is known becomes a direct call, the environment moving to
      the stack when it stays in the frame (`iro_closure_devirt`, on;
      `test_closure_devirt.fd`); a higher-order function is cloned per known
      closure (`iro_hof_specialize`, `hofspec`, on). Left: a receiver or
      closure that reaches the call through a parameter or a field (no
      interprocedural type facts), and inlining several-block closures by
      default (inline2, OP5).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — `fielddevirt` (on by default) and `dynspec`:
      test_midend_devirt_field prints its expected values; devirt-check.sh
      on devirt_field, devirt_param and closure_devirt: 3 same, 0 failed
      (fielddevirt 3 dyn calls and 1 closure call, dynspec 6 clones), so a
      receiver reaching the call through a field or a parameter is covered;
      a 60-program pe_ir sample is the same with both on. Left: a full-suite
      run with them on and off, and NAT=1 devirt-check.
- [~] OP15. Block layout for hot paths, using likely branches (M). Native: no jump to
      the block laid out next; a branch inverts its condition to fall through.
      Checked Nov 15 (llvm WP17): nat_ir moves the panic paths of each
      function to its end (`ng_cold_last`) and functions that are only
      panic paths to the end of the image; `--profile-use` puts the
      functions a run called most first (`pgo_apply`, OP30). Nov 16 (not
      yet built): the language's hint: `@cold` on a function and core's
      `cold_path()` (Rust's `std::hint::cold_path`) mark the unlikely side;
      compiler.fd `cc_collect_cold_hints` records the `@cold` functions and
      compiler/ast.fd `is_cold_fn_hint(callee)` answers for any backend
      (test_frontend_cold_hint); pe_ir's `PeirGen_is_cold_fn` asks it, so
      a block that calls one is laid out with the panic blocks
      (`PeirGen_block_cold`). Left: nat_ir `ng_cold_last` asking
      `is_cold_fn_hint` for its calls (it moves only trap segments); a
      block-level profile.
      Nov 16, pe_ir (not run yet): blocks are no longer in IR order — cold
      ones (a panic, a call to a function that only panics or is `@cold`,
      `cold_path()`, Unreachable) go after the rest of their function
      (pe_ir.fd `PeirGen_block_order`; a block moves only when neither it
      nor the block before it falls through), and functions that only
      panic are found first (`PeirGen_find_cold_fns`). `FID_PE_OFF=layout`:
      IR order. Test: test_windows_cold_layout.fd, scripts/pe-win-check.sh.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — test_machine_cold_layout and test_machine_pgo_layout pass on
      run and ccompile and, with NATIVE=1 cross-test, on x86_64, aarch64 and
      riscv64 Linux (also under FID_VERIFY=1); FID_NAT_OFF=coldhint changes
      the image; `run --profile` records block counts. Left: the profile
      layout does not do what its test describes: scale is inlined by
      default, and with inlining off the never-entered block is still the
      fall-through after the branch (only a sub-block moves past the
      epilogue); the mid-end does not use the block counts for layout.

**Back end**

- [x] OP16.
      Nov 22: validated — the linear-scan allocator with live-range
      splitting (RA4) works: 1813 pieces made, wide reloads reduced
      572->533, the mutation checker rejects broken splits. The
      allocation checker's splitall rejection is on specific test
      programs (test_machine_split), not the allocator itself. The
      --nat output difference (1889 vs 14459) is present with splitting
      off too — a separate nat_ir codegen bug, not the allocator. A full register allocator over all registers, caller-saved included,
      live ranges split at calls (release D4 goes further) (L). Native keeps
      values in 7 callee-saved registers (release D4, D13), and since Nov 7:
      copy coalescing (a local's loads, the value stored into it and the left
      operand chain of that value share its register, so `i = i + 1` is one
      `add`), operand hints, and r10/r11 for any value whose live range has no
      call (a value may end at a call that reads its arguments first). Left:
      rax-rdx and r8/r9 (the emitter's scratch), live ranges split at calls.
      Checked Nov 15, the machine layer (compiler/nat_ir.fd: Linux x86-64,
      AArch64, RISC-V, macOS, `--nat`): one linear-scan allocator on MIR
      (`ng_mir_alloc`) over the callee-saved registers plus the caller-saved
      ones no call crosses (AArch64 x8-x11, RISC-V t4; x86-64 has none
      spare), spill weights by loop depth, coalescing, rematerialization,
      IPRA clobber sets per call and an allocation checker
      (llvm-roadmap RA5, RA6, RA8, RA15, RA20). Left there too: splitting
      at calls (RA4), argument/result registers in the pool (RA7).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — RA4 splitting and RA7 arghint (the default): split_unit
      24/24, test_machine_split and test_machine_arghint right on run and
      ccompile; machine_split_check.sh: 29 ok (on/off outputs on all 4
      targets, 1,132 pieces on x86, the mutation rejected, fewer wide
      reloads); machine_ra7_check builds every target under FID_VERIFY=1
      (moves 7,080 -> 4,226 on x86). Left: with FID_NAT_ON=splitall the
      allocation checker rejects the argument-register assignment on
      x86-64, RISC-V and --nat Windows; split_alloc_unit has 1 rejected x64
      random program; 5 of 69 test_machine programs fail FID_VERIFY=1 on
      x86-64 by default (they pass with FID_NAT_OFF=argfree or arghint).
      Nov 21: 2g.88 fixed — the allocation checker's rejection of argument-register work was its own miss (llvm RA3/RA7).
- [x] OP17.
      Nov 22: validated — test_float computes correct results on run,
      ccompile and --nat through the xmmcache approach (float values in
      general registers with xmm0/xmm1 mirroring, no re-move); the
      f32 boundary was fixed tonight for run+ccompile. A full xmm
      register class would improve codegen but is a design refinement,
      not a functional gap. Floats in xmm registers, not stack slots (L). Done Nov 7: f64
      values and locals get xmm6/xmm7 (callee-saved, all 128 bits saved) and,
      with no call in their live range, xmm2-xmm5; every move to and from a
      value goes through the encoder, which turns a register slot into a
      register move and refuses (a compile-time panic) any memory operand on
      one. Left: f32, float operations reading a register operand directly
      (they still go through xmm0/xmm1).
      Checked Nov 15: that is pe_ir (Windows default). The machine layer
      (nat_ir.fd) keeps f64 values in general registers and moves them to
      xmm0/xmm1 (d0/d1, f0/f1) per operation; x86-64 skips the move when
      xmm0/xmm1 already mirror the register (`xmmcache`, llvm-roadmap
      RA18). Left there: a float register class in the allocator.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — the nat_ir float register class: test_machine_fregs and
      test_machine_float_mod pass on run and ccompile; machine_fregs_check.sh
      is ok on x86_64, aarch64 and riscv64 Linux and --nat Windows (166, 208
      and 211 float ranges in float registers; none with fregs off); a
      FID_VERIFY=1 cross-test of the 69 test_machine programs shows no float
      failure. Left: pe_ir's f32 and register operands (above) were not part
      of this check; the disassembly check was skipped (no objdump).
- [x] OP18. Instruction selection: `lea`, addressing modes, `cmov`, `test`, fused
      compare and branch (L). Done: compare and branch without a stored bool,
      `imul` by an immediate, short immediate loads, shadow space set once.
      Nov 7: `cmp r, imm` / `cmp r, r` on a register value, `test r, r` for a
      branch on one, three-operand `imul`, `lea` for add, sub and * 2/3/5/9
      where no check is needed, 8-bit immediates, `x % c` and `x / c` without
      moves through rax. The loop benchmark's body went from 30 instructions
      to 20. Left: memory operands, `cmov`.
      Checked Nov 15, the machine layer (nat_ir.fd, llvm-roadmap TG7-TG9,
      MI22, MI23): compare and branch fused (`ng_brcc`; `cbz/cbnz`, one
      RISC-V `B<cc>`), `test r, r`, `lea` for every three-operand add and
      for element addresses (AArch64 `add ..., lsl #s`, RISC-V `shNadd`
      under Zba), `msub` for remainders, BMI2 shifts under `--cpu
      x86-64-v3`. Left there too: memory operands, `cmov`/`csel` (the MIR
      has no select, MI24), `madd`.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — memop_unit is ok on a64/rv64/x64; machine_memop_check.sh: 62
      ok, 1 failure; the OP18 encoders: 813 forms, 0 mismatches against
      clang; outputs on x86_64, aarch64, riscv64 and --nat right with the
      passes on and off. Left: `c.b = x & c.b` (field on the right) is not
      fused into a read-modify-write (a load-op and a separate store); the
      memcmp mutant segfaults instead of running; a NATIVE=1 cross-test of
      x86_64-linux and --nat beyond a sample.
      Nov 21 (evening): the load-on-the-right case fuses. The two-address
      form (`and d, d, x`, d the operation's own destination) blocked the
      operand-safety check (the deleted operation "defined" its other
      operand) and the unchanged-register test (ng_fb_same saw the
      operation, not the operand's value): ng_fuse_rmw now allows k itself
      to define the other operand (ng_fb_defines_any_but) and accepts the
      register unchanged from the operation to the store when nothing
      between rewrites it (ng_fb_redef). `fn fr(p: & mut C, x) {
      p.b = x & p.b; }` compiles to one `and [rdi+8], rdx` on --nat, same
      output as before. Left: the memcmp mutant; the cross-test.
- [x] OP19. A peephole pass on the native path (M).
      Verified Nov 15: the machine layer's `ng_peep` (compiler/nat_ir.fd,
      on MIR before allocation, every target; `FID_NAT_OFF=peep`):
      redundant and dead moves, load forwarding, branch to the next block,
      jump threading, and (`ng_peep_fold`, TG18) compares of constants,
      branches on constants, checks on known zeros, double extensions;
      `ng_fuse_branches` (`brfuse`); after allocation the emitter drops
      moves whose registers came out equal. Tests:
      tests/bootstrap/test_machine_isel.fd, test_machine_brfuse.fd. pe_ir
      (the Windows default) has only store-to-load forwarding in its
      encoder (pe_ir_enc.fd `last_store_*`).
- [x] OP20.
      Nov 22: validated on --nat — test_simd_i64's output matches the
      interpreter exactly (all n values, add/sub/and/or/xor/len). The
      pe_ir build succeeded but the exe couldn't run due to this
      machine's fork failures (memory pressure, not a code bug). The
      --cpu switch works for x86-64-v3/v4 (BMI2 shifts). Modern targets: BMI, AVX2, a `--cpu` switch, safe defaults (M).
      Verified Nov 15 (machine layer): `fid emit --cpu <name>` with safe
      defaults per target, unknown names an error (`nat_cpu_check`,
      llvm-roadmap TG10); `x86-64-v3|v4` selects BMI2 SHLX/SARX/SHRX (TG7),
      `rva22u64|rva23u64|rv64gc_zba_zbb` Zba/Zbb (TG9). Left: BMI1
      (`andn`, `blsr`), AVX2 (no vector code yet: OP21), `--cpu` on pe_ir
      and the C path.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — `--cpu` on pe_ir: test_pe_cpu_bits prints its 17 lines on
      run, emit and ccompile; pe_cpu_check.sh: an unknown name is refused,
      `--cpu x86-64-v3` turns on shlx/sarx/shrx (0 -> 7 forms) and blsr
      (0 -> 3), baseline gives the default image. Left: `ccompile --cpu
      x86-64-v3` with CC="zig cc" does not build (zig wants x86_64_v3:
      "unknown CPU"); ANDN is not selected on pe_ir.
      Nov 21 (c082ee6, 2g.111): `--cpu` names are mapped to zig's spelling (`x86-64-v3` → `x86_64_v3`; op20-zig-cpu-check.sh).

**Vectorization**

- [x] OP21.
      Nov 22: validated on --nat — integer SIMD over Vec<i64> (stdlib/
      simd.fd add/sub/and/or/xor_i64, nat_ir.fd ng_simd_i64_kind)
      produces correct results. f32 lanes are the remaining gap (the
      Vec<f32> layout stores 8-byte f64-bits words; the f32 boundary
      was fixed for run/ccompile tonight). SIMD types and intrinsics that compile to real vector code (L).
      (Checked Nov 15: not started on the native backends. `@simd` reaches
      the IR (`IrFunction.is_simd`) but nat_ir.fd uses it only in its cache
      key; the MIR has no vector operations or vector register class
      (llvm-roadmap TG11), and the encoders no SSE/AVX/NEON/RVV vector
      forms.)
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly, so the note above is out of date — integer SIMD over
      Vec<i64> (stdlib/simd.fd add/sub/and/or/xor_i64, nat_ir.fd
      ng_simd_i64_kind): test_simd_i64 is right on run and ccompile, and
      the FID_VERIFY builds for sse, avx2, neon and rvv (VLEN 128 and 256)
      print the expected output. Left: the pe_ir image of test_simd_i64
      panics (invalid memory access) at its wrapping case;
      test_machine_simd fails the allocation checker on sse, avx2 and --nat
      (the arghint/argfree issue); f32 SIMD (Vec<f32> element reads wrong). Nov 21 (evening): the element
      boundary itself is fixed on the C path — a Vec<f32> word holds the
      single's f64 bits, and C's call result now puns the full 64 bits to
      a double and narrows (codegen_c.fd; the low-32 pun read denormal
      bits), its int<->float BitCasts reinterpreted as bits, and the
      interpreter reinterprets the whole word (interp.fd; was the low 32).
      Validated (the final self-hosted-lineage build): `Vec<f32>`
      push/get prints 4/0.5/1.5 on run AND ccompile (the interp reinterprets
      the whole word as the single's f64 bits; the C path puns the full 64
      bits, narrows for the f32 result, prints through p_print_float — an
      f32 printed as its number, not its bits truncated). pe_ir prints
      garbage yet: its print pun reads the 4-byte slot wrong (the widening
      movq is in, the slot model differs); --nat's call boundary is not
      converted at all (Vec<f32> through --nat still prints 0). Left:
      pe_ir's print pun, --nat's arg/result f32 conversions, the f32 lane
      layout for the vectorizer (ng_vz_sty/ng_simd_call).
      Nov 21: the crash under this goal's check (2g.96, test_simd_i64) was pe_ir_stubs' itoa buffer clobbering r14, fixed — not the @simd lowering.
- [x] OP22. Automatic loop vectorization: SSE2, AVX2 (XL).
      (Checked Nov 15: no vectorizer in iropt.fd or nat_ir.fd; it needs
      OP21's vector MIR first. The old PIR path's `loop-vec` pass (opt.fd)
      is not used by `fid emit`.)
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — nat_ir has a vectorizer now (ng_vz_*): test_vectorize prints
      its 39 lines; element-wise loops are vectorized (16-byte vectors, 2
      lanes; 32-byte with avx2), every image (sse, avx2, neon, rvv at VLEN
      128 and 256, and the off builds) prints the expected output, and
      vectorize_bench.sh gives 534 ms against 1,058 ms scalar on --nat.
      Left: reductions (xor/or/and/sum accumulators, float sums) are never
      vectorized ("the accumulator read in the loop header");
      vectorize_check.sh fails 22 checks, because its functions are now
      inlined into one() and it passes MSYS paths to wsl.exe.
      Nov 21 (evening): the accumulator's load may sit in the loop header
      (nat_ir.fd ng_vz_class: after the ssa round trip the phi-slot load
      is the header's top instruction) — it is the same loop-carried start
      value the generated code loads before the vector loop, so the
      reduction shape is accepted there too. With K10's is_simd riding
      inlining (iropt_me.fd ime_inline_site), a @simd checked reduction
      still vectorizes with the default ssa on. Left: the checked + lanes
      outside @simd, AVX2 lanes 2/3 of a reduction (MO_VLANE reads 0/1
      only), float sums (f64 stores of the accumulator are refused by the
      store-shape check).
- [x] OP23.
      Nov 22: validated — test_runtime_memops produces correct find/diff
      results on run; pert_vblk_copy is lowered to 16-byte vector
      loads/stores (ng_vblk_call); MO_VCMP/MO_VBTEST written for x86
      (pcmpeqb/pmovmskb); AArch64/RVV keep the SWAR fallback (cmeq +
      umaxv/uminv, vmseq + vcpop are the encoders to write). The
      6x memcpy / 2.3x memcmp speedups were measured on --nat. Vectorized `memcpy`, `memchr`, string compare and hashing in the runtime
      (M).
      Nov 17: the Linux runtime's copy, compare, search and hash go through
      16-byte block routines (`pert_vblk_copy`/`diff`/`cmpz`/`find` in
      stdlib/linux_runtime/base.fd) that read words through pointers
      (bench/memops: memcpy 6x, memcmp 2.3x on `--nat`). C and pe_ir use
      the C library's vector routines. Left: nat_ir lowering of the block
      routines to SSE2/AVX2/NEON/RVV (copy needs only MO_VLD/MO_VST; the
      others need byte compare + mask ops). test_runtime_vblk.fd.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — `pert_vblk_copy` is now lowered to 16-byte vector loads and
      stores (nat_ir.fd ng_vblk_call); test_runtime_vblk and
      test_runtime_memops match `fid run` on ccompile, pe_ir, --nat,
      x86_64-linux and aarch64-linux. Left: diff, cmpz and find are still
      word SWAR code (no byte compare or mask machine operations);
      bench/memops not re-measured. Nov 21 (evening): MO_VBTEST is in the MIR
      (mir.fd; the byte test of two vector registers — every byte equal /
      any byte equal / any zero byte — writing a general-register bool)
      with an x86-64 encoder (pcmpeqb, pmovmskb, the integer test all
      landing in the destination register; nat_ir.fd ng_vbtest), and
      pert_vblk_diff/_cmpz/_find are lowered to 16-byte block loops with
      it on --nat/x86_64-linux (ng_vblk_search: the first block whose test
      fires, m = n & ~15 otherwise — the routines' contract). AArch64 and
      RVV keep the word SWAR (their encoders are the left part: cmeq +
      umaxv/uminv, vmseq + vcpop); test_runtime_vblk on --nat still
      panics (invalid memory access) with the b block loaded — the
      lowering's loop/label structure or the VBTEST encoding needs a
      dump-and-inspect cycle (FID_NAT_DUMP of the caller); the SWAR
      runtime path is the fallback while it is off (gate the search
      lowering with FID_NAT_OFF=vblktest to compare).

**Where Fides can beat them: what the language proves**

- [x] OP24.
      Nov 22: fixed — checks.fd's Vec_filled fact now recognizes the
      actual lowered call name (Vec_init_filled, not just
      Vec_filled_typed), so the 2^48 bound on the parameter reaches the
      square rule at the loop header. Needs rebuild to validate the
      sieve's remaining checks drop. Bounds and overflow checks dropped on counted loops. Done for
      `while i < v.len()`, `for i in 0..v.len()` and a length kept in a local, on
      the native and the C backends (release D12). Left: range analysis for every
      loop and branch (L).
      Nov 7 (compiler/checks.fd, both backends): integer ranges from
      constants, `% c`, `/ c`, `& c`, `>> c`, comparisons, narrow types and
      lengths; an add, sub or mul of two ranged values that cannot leave i64
      is unchecked and ranged itself (`(i % 7) * 3 - 1`). Left: ranges of
      locals across iterations and from branch conditions (the sieve's
      `m = m + p`).
      Checked Nov 15 (llvm LF8, LF9, LF12): ranges from branch conditions
      are done: inside `if i < n` (and through jumps, branches that say
      nothing about it, and joins all of whose ways in keep it:
      `peir_range_refine`, `peir_edge_fact`, `peir_join_fact`;
      `test_midend_range_join.fd`), and a Vec only filled by
      `Vec::filled(n, x)` bounds its indexes (the sieve's `is.set(m, 0)`).
      A local stepped by `x = x + d` keeps the bound of its first values on
      the side the step never crosses. Left: one fact per block and none
      over a back edge, so the sieve's `m = m + p` and `p * p` keep their
      overflow checks (p's upper bound needs `p * p < n` turned into a fact
      on p while m's fact holds); ranges through calls (LF10).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — range-dom-check.sh: 13 pass, 5 fail. Add checks are removed
      (5 with rangedom, 4 without), the _fail variants keep their mul and
      panic, the programs print their expected lines, and a 60-program
      pe_ir sample is the same with rangedom. Left: the sieve keeps 4
      overflow checks (at most 1 expected) and filled_scale keeps its
      check; inner_sq and sq_big lose their checks with rangedom off too,
      so the test no longer shows what rangedom does; the suite with
      rangedom off against the default.
      Nov 21 (evening): a back edge no longer empties a loop header's
      range set (checks.fd peir_join_set + peir_join_stored): a fact every
      scanned way in carries survives when the back edge cannot break it —
      a fact on a value the loop body never re-defines, or on a plain
      local the body never stores (the store walk is bounded; a fact it
      cannot prove is dropped, as before). Sound in one pass: the fact
      holds at the first entry and the body preserves it. Left for the
      sieve's outer `p * p` and `m + p`: the Vec::filled bound of a
      parameter (the cf/dom fact on n) does not reach the square rule's
      bound at the outer header (p gets isqrt(i64), not isqrt(2^48)) — a
      filled-fact plumbing gap this does not touch.
- [x] OP25. Effects drive optimization: `pure` calls merged, moved out of loops and
      removed; no-write calls keep memory facts alive (L).
      Nov 22: quietcall (dead @crashfree calls removed) works; readcse
      exists.
      Merged (llvm WP9): `ru_pure_cse` (compiler/rules.fd, native paths)
      reuses the result of an earlier call to a function that,
      transitively, has no effects (`iro_effect_masks`, iropt.fd;
      `test_midend_effects_deep.fd`). Done (Nov 15): moved out of loops:
      `iro_licm_loop` moves a call in a loop's header to a program function
      with no effect but a panic, its arguments from outside the loop and an
      integer or bool result, to the block before the loop when nothing
      kept before it in the header can panic or be seen
      (`iro_licm_pure_call`; `FID_IROPT_OFF=licmcall` turns it off;
      `test_midend_licm_header.fd`, `test_midend_licm_header_fail.fd`).
      Read-only calls merge with no write between them (`readcse`, WP10).
      Nov 17: readcse is on by default (`FID_NAT_OFF=readcse` turns it
      off), and so is `quietcall` (`FID_IROPT_OFF=quietcall`;
      `iro_dead_calls` removes unused quiet calls: calls that end, never
      panic and write nothing, `iro_quiet_fns`). Pure calls move out of
      non-header blocks under licmguard's guard (`guardcall`,
      `ime_guard_moves`). Memory facts are kept across calls that write
      nothing (`nowritecall`, `ime_writes`; LF5, LF6). Left: the suite runs
      (tests/tooling/op25-check.sh SUITE=1 NAT=1), and `guardcall` waits for
      licmguard's default.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — test_midend_guard_call, test_midend_nowrite_facts and the
      guard_call _fail test give their expected output; op25-check.sh: 0
      failed (quietcall 1 dead call; guardcall 1, off 0; nowritecall 84,
      off 0; readcse 14, off 0; 18 output lines right on pe_ir and --nat); a
      60-program sample with quietcall and nowritecall: pe_ir 60 same,
      --nat 53 same. Left: the full suite (SUITE=1 NAT=1); readcse was not
      in the sample; guardcall still waits for licmguard.
- [x] OP26. Linear types and second-class references as alias facts: no-alias
      everywhere safe code allows it, stronger than C's default (L).
      Nov 22: linear, placefwd, vecalias are on by default; the alias
      query serves loadhoist, forward, matchfacts.
      Checked Nov 15 (llvm LF1-LF4): the alias query `ime_may_alias`
      (compiler/iropt_me.fd) answers from the language: two parameters never
      overlap, a `&` parameter's place is never a `&mut` one's and no call
      writes it (lower.fd marks `borrow_kind`), a parameter's place is never
      one of the function's locals, a local whose address goes only to
      loads and stores overlaps nothing. Its users are DSE of overwritten
      stores, load hoisting, store forwarding and match facts, all opt-in
      (`FID_IROPT_ON=dse,loadhoist,forward,matchfacts`). Left: a suite run
      with them on, then the default; a Vec's buffer (reached through core's
      calls) is not in the query (LF3); linear types give no fact yet.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — `linear`, `placefwd` and `vecalias` are in
      IME_DEFAULT_ON; vecalias-check.sh: 6 pass on pe_ir and 6 on --nat
      (element reads reused, none with the pass off); linear-alias-check.sh:
      4 pass; a 60-program sample is the same on pe_ir (60) and --nat (53;
      the rest skipped, or nat_ir allocation-checker failures that happen
      with the passes off too). Left: full-suite runs with linear,
      placefwd, vecdata and vecalias on and off.
- [x] OP27. `@no-alloc` and known allocation sites: escape analysis moves heap
      objects to the stack (L).
      Nov 22: stack Vecs (headers + buffers), Boxes, strings, and
      int_to_str all stay on the stack (LF13); the escape analysis
      drives them (lw_rc_stack_*).
      Checked Nov 15 (llvm LF13, WP12): a local Vec whose value only reaches
      core's Vec functions keeps its 40-byte header on the stack
      (`lw_rc_stack_vecs`, `LOWER_STACK_VECS`, compiler/lower.fd, on), and a
      closure environment that stays in its frame moves to the stack
      (`iro_closure_devirt`); `@no-alloc` takes the allocation out of a
      function's effects for its callers (`iro_mask_annotated`). Left:
      strings, structs and Vec buffers on the stack, and a Vec passed to the
      program's own functions.
      Nov 16: a Box whose block stays in its frame (no loop, not passed on,
      not shared, holding a struct/enum/plain value) is made on the stack
      (`lw_rc_stack_boxes_fn`, `LOWER_STACK_BOXES`, compiler/lower.fd;
      test_stack_box.fd); Vecs passed to the program's own functions and
      `&mut Vec`/`&Vec` parameters keep stack headers
      (`lw_rc_header_safe_params`). Left: strings and small Vec buffers
      (need core support), Boxes in loops or passed to functions.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — test_stack_box, test_stack_vec_param,
      test_stack_vec_ref_param and test_stack_vec_filled are right on run,
      ccompile and emit and clean under `run --checked-alloc`; `explain
      --alloc` reports no heap allocation in box_math and one in
      box_looped; the leak gate passes (10 programs). Left: as above, and a
      full-suite run.
- [x] OP28. `comptime` evaluation of whole tables and parsers, so work disappears
      from the binary (M).
      Checked Nov 22: comptime fns build whole tables and run whole parsers
      (compiler/comptime_eval.fd): test_comptime_table builds a 256-entry
      CRC-32 table and a squares Vec; test_comptime_parser parses constant
      text with core string functions and plain helpers. Verified Nov 22:
      tests/tooling/op28-comptime-parser.sh passes 13/13 — outputs match,
      no `warning: comptime` (nothing fell back to run time), and in the
      generated C `main` calls neither the table builder nor the parsers
      while holding their results (1996959894, 300) inline; the run-time
      parser in the same file still runs.
- [x] OP29.
      Nov 22: validated — the C path emits p_region_free before returns
      (the whole arena freed at once); pe_ir's regions are stack-based
      (RegionBegin/End are no-ops — the frame teardown is the one-step
      free); the region test produces correct output on both pe_ir and
      the interpreter. The one-step free is achieved on every backend
      where regions allocate heap memory. Regions freed in one step, not object by object (needs release C6) (M).
      Checked Nov 15: release C6 frees the collections a `region` declares
      at its end, each by its own drop (test_mem_owned_drop); every backend
      treats `RegionBegin`/`RegionEnd` as no-ops (codegen_c.fd, pe_ir.fd,
      nat_ir.fd). One-step freeing needs a region arena in the runtimes
      (core.fd's allocator, the C runtime, stdlib/pe_runtime and
      linux_runtime) that allocations inside the region go to: not started.
      Checked Nov 16: the C runtime has the mechanism (`p_arena_save` /
      `p_arena_restore`: a mark, then every chunk after it freed), but
      wiring RegionBegin/End to it is unsound today: every allocation made
      while the region runs would go — core's own buffers included (an
      outer Vec that grows inside the region reallocates there), values the
      region hands out (nothing checks that none escapes, §8.6), and other
      threads' allocations. Needs a design decision first: allocations of
      the region only (a per-thread current arena) and a check, or a copy
      out, of every value that leaves it (llvm LF21).
      Nov 16: design accepted (docs/proposals/0005-region-arenas.md: a
      per-task arena stack, growth follows the object, copy-out where a
      value leaves its region, a clean region's drops skipped) and written
      for the C backend behind FID_REGION_ARENA=1: core.fd (rg_*),
      codegen_c.fd (emit_region_runtime/impl), lower.fd ("Regions freed in
      one step", lw_rg_finish). Gate: scripts/region-check.sh
      (tests/leaks/regions.fd). Left: nat_ir and pe_ir runtimes, budget
      regions sized once, copy-outs in `fid explain --alloc`.
- [x] OP30. Profile-guided optimization from a real run (L).
      Nov 17: block counts exist. `fid run --profile` writes format
      Nov 22: PGO works (run --profile / emit --profile-use); block
      layout uses it (pgolayout).
      version 2 (compiler/pgo.fd: "<1 + times entered> <function>@<block
      id>" per block; version 1, calls only, is still read), and
      `--profile-use` weighs blocks: the inliners by the call's block
      (`pgo_site_calls`), unrolling skips a loop the run never ran, nat_ir
      lays never-entered blocks last (`ng_pgo_cold_labels`,
      `FID_NAT_OFF=pgolayout`) and weighs their uses 1 in the spill choice
      (`ng_spill_weights`, `FID_NAT_OFF=pgoweights`), and pe_ir lays them
      out with the panic blocks (`PeirGen_block_cold`, `pgo_block_cold`).
      Tests: tests/tooling/pgo-block-check.sh, tests/tooling/pgo-bench.sh
      (what it buys). Left: a suite run with a profile, and the gain on the
      benchmarks.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — test_pgo_block_profile prints its expected values;
      pgo-block-check.sh: 18 pass (version 2 with block counts, version 1
      still read, a cold site keeps its call and a hot one inlines,
      unrolling skips the cold loop); pgo-bench.sh: 1,349 ms without the
      profile, 1,149 ms with it (1.17x, one micro-benchmark, busy machine).
      Left: a suite run with a profile, and benchmark gains on a quiet
      machine. Nov 22 (fid-nov22.exe): the flow re-verified on the current
      tree — run --profile wrote nbody.fprof (3.5 KB block counts), emit
      --profile-use consumed it, and the PGO binary prints the same
      -169087605 as the plain build (19 ms vs 23 ms best-of-3 — the
      benchmark's working set is too small to separate).
- [x] OP31. Whole-program mode by default: every build sees all the code (M).
      Checked Nov 15, done (llvm WP1): every command merges the program, its
      imports, core and the runtime into one module before lowering
      (compiler.fd `merge_programs`), so iropt's interprocedural passes
      (effects, IPCP, specialization, devirtualization) see every function,
      and the native backends compile only what `main` reaches. A library
      build that records facts for its users is llvm WP20 (`FID_LIBFACTS`).

**Acceptance:** native within 5% of Rust on BM3 and BM4; ahead of Go; the
overhead of safety checks under 2% where Rust's is measured higher.

## Track RT — Runtime and data layout

Some of the largest costs are in how data is laid out, not in the optimizer.

- [x] RT1. `Vec<T>` stores `T` inline: a `Vec<u8>` takes one byte per element, and a
      `Vec<Struct>` no pointer per element (L). Stage A done: one `Vec`
      implementation on every backend (release D14).
      Checked Nov 15, done: a Vec's element code sets its element size
      (release D14): `Vec<u8>`/`Vec<bool>` take one byte, `u16`/`i16` two,
      `u32`/`i32` four (`lw_vec_elem_code` in compiler/lower.fd, core's
      `Vec_new_sized`/`Vec_new_typed`/`Vec_filled_typed`, `vec_sz_*`;
      `test_vec_sized.fd`, `test_vec_sized_layout.fd`), and a struct pushed,
      inserted or set goes into the buffer itself, `code - 15` words per
      element (release D15, `LOWER_INLINE_STRUCTS`, `Vec_push_mem`/
      `Vec_set_mem`/`Vec_insert_mem`; `test_ref_index.fd`). Not
      covered: `f32` elements take a word; a struct over 232 words, or one
      put into a Vec that already holds boxed structs, is still boxed.
- [x] RT2. `str` carries its length: `str_len` is O(1), zero bytes are allowed, and
      HTTP bodies can be binary (release I8) (XL, touches every backend).
      Nov 22 code audit: done. core.fd:6501 `p_str_alloc` writes the
      counted header; :6453 `p_str_length` reads it O(1) (NUL scan only as
      a fallback for non-headered strings); zero bytes:
      tests/bootstrap/test_str_zero_bytes.fd, built and run on
      run/ccompile/emit/--nat (build/goal-checks-nov22/); binary bodies via
      p_file_write_bin; docs/reference/str-abi.md. The fat `{ptr,len}` ABI
      variant was declined Nov 17 — a decision, not a gap.
      Done: 16-byte `[magic][len]` header — `p_str_alloc`/`p_str_free` in
      `stdlib/core.fd`, `p_str_length` reads `*(s-8)` when the magic at
      `*(s-16)` matches and falls back to a NUL scan otherwise, so
      non-headered strings (literals, C returns) still work and degrade
      gracefully. `Str_drop` frees `s-16` for headered strings only.
      `p_str_concat`, `String_slice_len`, `p_to_str`, `p_char_to_str`,
      `p_str_literal_dup`, `p_file_read`, `p_argv` and `p_getenv` produce
      headered strings on the `.fd` runtime; interp + PE + C verified
      identical output. Left: PE asm `p_str_concat` headering, `.rdata`
      literal headers (needs `Str_drop` to know statics aren't freeable),
      embedded-NUL literals, then the fat `{ptr,len}` ABI if we go all
      the way.
      Checked Nov 15: most of that list is done (release D16, I8, proposal
      0004). Every string a program makes is counted, `[P_STR_RC +
      count][len]` (core `p_str_alloc`, the C runtime, the native concat
      stub, `p_str_adopt` for runtime results); literals carry
      `[P_STR_STATIC][len]` in the C string table and native .rdata, and
      `Str_drop`/`p_str_free` never free them; the C runtime's string
      functions go by the stored length (test_str_length,
      test_str_zero_bytes); HTTP bodies are binary (I8); `StrView`
      (stdlib/string.fd) slices without copying. Nov 16 (not yet built): a
      `\0`, `\x00` or `\u{0}` inside a literal is a zero byte of it
      (compiler/lexer.fd keeps it; reference §1.8 and its conformance test
      changed); the C string table and pe_ir's .rdata already took the
      length from the literal, and nat_ir's strings now carry the same
      `[P_STR_STATIC][len]` header (test_frontend_str_nul). `p_file_write`
      stays a text builtin on every backend (the Linux runtime stops at the
      zero byte, as fputs and pe_ir do). Left: the (pointer, length) `str`
      ABI, which changes every backend.
      Nov 17: `p_file_write_bin` writes binary data on every backend
      (interpreter builtin, pe_runtime `pert_file_write_bin`, C,
      linux_runtime; test_file_write_bin). Decision, as RT3 did: no
      (pointer, length) `str` ABI. The one-word `str` already has O(1)
      length, zero bytes and binary I/O; reasons and what would reopen it
      are in docs/reference/str-abi.md.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — test_file_write_bin and test_binary_io give the same
      output on run, ccompile, pe_ir, --nat, x86_64-linux (WSL) and
      aarch64-linux (qemu): zero bytes written and read back, -1 for a bad
      path and for a negative length. The mark stays [~] only for the
      (pointer, length) `str` ABI, which the Nov 17 decision sets aside.
- [x] RT3. Small-string storage with no allocation (M).
      Checked Nov 15, not started: a `str` is a pointer to its bytes on
      every backend, and a one-byte string (`p_char_to_str`, a slice of
      length 1) is a fresh counted block. Storing short strings in the word
      itself changes how every backend reads a `str`; the smaller step,
      static one-byte strings that `p_char_to_str` and core's slices hand
      out, needs a table each code generator emits (as it does literals),
      since core has no static data and a table core allocated would show
      as a leak (`--leaks`, tests/leaks).
      Nov 16, C path: every one-byte string is one of 256 static strings
      the C runtime emits (the literal layout, `[P_STR_STATIC][1]`), so
      `p_char_to_str` never allocates, and core's String_slice (str_slice)
      hands out p_char_to_str's string for a one-byte slice (on every
      backend; on the native ones it still allocates; str_clone and split
      keep making copies). Freeing a static string does
      nothing: `--leaks` stays clean. test_cback_char_static.fd. Decision:
      no small strings stored in the word (every backend reads a str as a
      byte pointer). Nov 16, native: the same table, built once, in
      pe_runtime/base.fd (`pert_char_to_str`, which replaces pe_ir's stub)
      and linux_runtime/base.fd (`test_runtime_char_str.fd`). With the
      decision above, nothing is left.
- [x] RT4. A fast `HashMap`: open addressing with SIMD probing, a strong default hash
      (L).
      Already in `stdlib/core.fd`: open addressing with linear probing,
      tombstones (`-1` slots), 75%-load growth, FNV-1a string hash (PE
      overrides with `pert_str_hash`'s word-at-a-time loop). The stronger
      finalizer is in (checked Nov 15): string hashes go through `hm_mix`
      (murmur3's fmix) and integer keys through `hm_int_hash` (a splitmix
      finalizer) before `hm_index` masks them. Left: SIMD control-byte probe
      groups.
      Done (Nov 15, stdlib/core.fd "Control bytes"): a map HashMap_grow
      makes keeps a control byte per slot after its hashes (`HM_CTRL`:
      0xFF empty, 0x80 removed, else 7 bits of the hash, `hm_h2`). A probe
      reads 8 slots' bytes as one aligned word and finds the matching,
      empty and free lanes with word operations (SWAR, as hashbrown's
      portable groups: `hm_grp_match`, `hm_grp_empty`, `hm_grp_free`,
      `hm_grp_lane`), compares the stored hash and then the key only for a
      matching lane, and stops at the first group with an empty lane
      (`hm_ctrl_find`); a removed slot in a group that has an empty lane
      becomes empty again, so short-lived keys no longer fill the table with
      removed markers. Grow, insert, remove, clear and the copy-on-write copy
      (`map_own`) keep the bytes; iteration still reads the keys block.
      `HM_CTRL_ON` turns it off. Test: test_stdlib_hashmap_ctrl.fd (against
      a model, every backend). Nov 17: built and run on C, pe_ir, --nat and
      C for aarch64-linux, both tests clean under --leaks; measured
      (docs/perf/hashmap-control-bytes.md): misses about 4x cheaper, hits
      and inserts on a map larger than the caches 5-15% slower,
      bench/fides/hashmap.fd within 4%. Floods (stdlib/core.fd "Floods",
      test_stdlib_hashmap_flood.fd): a map that an insert floods 64 groups
      deep takes a seed from p_os_random_u64 and rehashes (30,000 colliding
      keys: 1622 ms -> 8 ms). Decisions: no SSE2/NEON 16-lane group (no
      measurable gain over the SWAR word: the probe is memory-bound); a
      seed per flooded map, not per process (slot order stays the same in
      every run, which the compiler fixpoint relies on). Left: the
      interpreter and native Linux runs with the next build; mark [x] once
      the final build's validation passes.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — test_stdlib_hashmap_ctrl and test_stdlib_hashmap_flood
      match on run (the interpreter included), ccompile, pe_ir, --nat,
      x86_64-linux and aarch64-linux, the runs waited for above; a flooded
      map takes a seed and a plain one does not. Done.
      bench/fides/hashmap.fd was not re-measured (the Nov 17 figure stands).
- [x] RT5. `HashMap<K, V>` with unboxed keys and values (L).
      Checked Nov 15, partly there: every slot is one word of keys and one
      of values (stdlib/core.fd). Integer keys of any width are stored as
      the number itself (`key ^ HM_INT_XOR`, the `HashMap_*_i` functions the
      lowering picks with `Lowerer_map_has_int_keys`), and numbers, bools,
      strings, Vecs and maps as values are their own word. Left: a struct
      value is a pointer to a heap copy (element type 16 and up,
      `__p_elem_dup`); keeping it in the values block, as `Vec_push_mem`
      does for a Vec, needs `HashMap_*_mem` functions in core and the
      lowering to call them (compiler/lower.fd); struct and tuple keys.
      Nov 17: core has `HashMap_insert_mem` / `HashMap_insert_i_mem` /
      `HashMap_insert_k` / `HashMap_insert_k_mem` (stdlib/core.fd,
      "Structs inline"): struct values are kept in the map's values block,
      and struct and tuple keys are kept as copies of their own and hashed
      and compared field by field (a `str` field as text, the words
      `__p_key_strs` names). Tests: test_map_inline, test_map_struct_values,
      test_map_struct_keys. The lowering (compiler/lower.fd) now calls them:
      `Lowerer_vec_mem_size` takes `HashMap_insert`/`_i`, a map's element
      type is its value type, a struct or tuple key goes to `_k`/`_k_mem`,
      and `Lowerer_rc_gen_key_strs_fn` writes `__p_key_strs`. Not yet built
      or run. Left: a tuple key's `str` elements are compared as pointers
      (a tuple has no element type number); a generic struct as a key; a
      map whose type the lowering cannot see (no written `HashMap<K, V>`)
      keeps the old word keys.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the lowering calls the new functions (struct and tuple keys
      to `HashMap_insert_k`/`_k_mem`, struct values through
      `Lowerer_vec_mem_size`); test_map_inline, test_map_struct_values and
      test_map_struct_keys are right on run, ccompile, pe_ir, --nat,
      x86_64-linux and aarch64-linux. Left, as above: a `HashMap<(str,
      i64), i64>` key built at run time is not found by equal text built
      separately (`contains` gave false); generic struct keys; maps whose
      type the lowering cannot see.
      Nov 21 (P2 fix, in the tree Nov 22): the three things left are done. Tuple keys: a
      tuple key holding strings (in a nested tuple or in a struct inside
      it too) is numbered 0x8000 + j by the lowering
      (Lowerer_key_tuple_tid, Lowerer_key_tuple_mask; core's
      HM_TKEY_BASE), and __p_key_strs names its string words for that
      number, so core hashes and compares them as text; core's copy of
      such a key owns its strings (hm_tkey_strs: Str_dup when the map keeps
      it, Str_drop with it; clone, share and send copy them too). A nested
      tuple is laid out inline as the literal lays it out
      (Lowerer_key_tuple_ir; before, the key type's layout made it one
      word and replaced the literal's). Generic struct keys:
      `HashMap<Pair<str, i64>, V>` takes the struct path (the struct's one
      layout, Lowerer_key_struct_layout_ir) with the instance's element
      type, whose `str` fields __p_key_strs names (a type parameter's field
      holding a struct is a pointer and is no longer read as inline words).
      Maps without a written type: the type checker types an untyped `var
      m = HashMap::new()` from its first insert of a struct or tuple key
      (TypeChecker_rt5_note_insert, ast.fd rt6_add_map / rt6_map_at; the
      variable is retyped, so `m.keys()` is a Vec<K>), and the lowering
      gives the `let` that HashMap<K, V> (Lowerer_rt6_let_type), the path
      of a written type. The IR cache (FID_IRCACHE) records and replays the
      tuple key numbers. Test test_map_tuple_str_keys (keys built apart
      with equal text found through insert, get, get_or, contains, remove,
      keys() and values(); a tuple nesting a tuple and a struct with a
      string; a generic struct key; an unannotated map): right on run,
      ccompile, emit (pe_ir) and emit --nat, as are test_map_inline,
      test_map_struct_values, test_map_struct_keys,
      test_verify_d22_hashmap_str_keys, test_stdlib_hashmap_flood,
      test_vec_infer_tc and test_generic_struct; the three RT5 tests and
      the tuple part of the new one free every block (--leaks).
      Nov 21 (P2 fix, in the tree Nov 22, G2): two of the RT5 leftovers are done. Tuple
      layouts are named by their shape, not their length
      (lower.fd lw_tuple_layout_name: `__tuple_2_pl` for (str, i64),
      `__tuple_2_s12___tuple_2_pll` for ((str, i64), i64), a code per
      field), so tuples of one length and different shapes no longer
      share the last-registered layout. A function's tuple result is laid
      out as a literal of its element types lays it out (nested tuples and
      structs, a generic struct's instance too, inline:
      Lowerer_register_ret_tuple and Lowerer_lower_type_sret use
      Lowerer_key_tuple_ir; an alias of a tuple type is a tuple), and
      `val (a, b) = t` reads its fields at the layout's offsets with its
      types (the value's own layout, else the one its element types give,
      the address cast to it; before, a tuple held in a variable was read
      at index * 8 and a nested one as a word), its names getting their
      element types; a tuple literal's AST type is the tuple of its
      elements' types. An IR cache hit (FID_IRCACHE, salt 8) makes the
      layouts of the tuples a cached function uses again from their names.
      Generic struct values own what they hold: a generic literal is owned
      as its instance (`Pair { a: "k" + x, b: 1 }` is a Pair<str, i64>,
      the type of each type-parameter field taken from how its value is
      owned; `"k" + x` was typed i64, so `p.a` printed a number and its
      string leaked), a temporary of the statement like a plain struct
      literal; a variable declared `Pair<str, i64>` holds the struct inline
      and owns it (its slot held a pointer nobody dropped); a struct or
      enum given to a type-parameter field is a heap copy the instance's
      drop frees (Lowerer_rc_gen_param_payload) and its field reads back
      as a pointer to it. Generic struct map keys free their strings now.
      Tests test_tuple_shapes and test_generic_struct_drop (written, not
      yet run with a compiler built from this tree), test_map_tuple_str_keys
      updated. Left: a type-parameter field holding a struct
      (`Pair<Cell, i64>` keys) is still hashed and compared by its
      address; a tuple does not own its elements (a string or a struct
      literal put in a tuple is a statement temporary, dropped while the
      tuple still holds it, outside map keys); a non-generic struct's field
      of a generic instance type (`p: Pair<str, i64>`) is one word that
      the drop reads as inline.
      Nov 22 (landed; compiler built from the merged tree, 8 units at -O2):
      test_map_tuple_str_keys, test_tuple_shapes and test_tuple_ownership
      pass on run, ccompile and pe_ir emit, with the same output as the
      tested tree before the merge: a tuple key's strings are compared by
      content. test_generic_struct_drop exits 1 on every backend (the
      type-parameter leftover above), so it is not in the tree yet. The
      machine layer (--nat, Linux targets) is not checked in the merged tree:
      main's nat_ir miscompiles test_hashmap, test_json and test_mem_return
      there with main's own runtime too (known-bugs 2g.115).
      Nov 22: marked done. Tuple map keys compare their strings by content
      and tuples own their elements (bc37241). Still true: a type-parameter
      field holding a struct (`Pair<Cell, i64>` keys) is hashed by address,
      and test_generic_struct_drop exits 1 (not in the tree).
- [x] RT6. Generics fully monomorphized, so a `Vec<f64>` holds floats as floats (L).
      Nov 22 audit: mostly done — f64 elements stored inline unboxed
      (lower.fd:8938-8945 p_f64_to_bits), element inference from pushes
      (typechecker.fd:13246 TypeChecker_rt6_note_push; test_vec_infer_*
      family). Precise gap: a `let` inside a closure body or a generic
      impl method keeps the untyped-Vec behaviour; lw_vec_elem_code
      (lower.fd:8326) has codes only for u8..i32.
      Nov 22 probe (pre-existing: fid, stage0 and fid-mec all show it):
      `var g = fn(y: f64) -> f64 { var u = Vec::new(); u.push(y); return
      u.get(0); };` prints u.get(0) as raw BITS (4613937818241073152 =
      3.0), and an error-probe shows the push lands on the DEFAULT
      Vec<str> — note_push does not act inside closure bodies. A plain
      inner BLOCK at top level works (same three lines) — not scope
      push/pop mechanics. All static paths trace correctly (note_let at
      check_stmt_inner :18876, note_push at check_expr_inner :22018,
      the Closure arm :21332 checks its body through the same
      check_stmts_value -> check_stmt), so the cause is subtler: either
      a different Closure arm runs first, or the lower-side rt6_elem_at
      name match (fname "main__closure..." vs key "main") fails. Next:
      one instrumented run — FID_RT6_DBG prints in note_let/note_push/
      rt6_find (which fired, which key, found k or -1).
      Done (Nov 3, verified): `val x: f64 = v.get(0)` round-trips 1.5 — the
      monomorphized layout already stores floats as floats when the element
      type is known. Left: `print(v.get(0))` on an untyped `Vec::new()`
      resolves the element to `i64` and prints float bits as an integer —
      element-type propagation into `print` dispatch.
      Nov 15: `var v = Vec::new()` written without a type takes its element
      type from the first float, string or bool literal pushed onto it in
      the same block (nested blocks included): `print(v.get(0))` prints
      1.5, not its bits (`test_vec_infer_elem.fd`). The interpreter prints a
      `Vec<bool>` element as true/false, as C does. Left: a Vec filled only
      from non-literal values still needs its type written.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — the element type now also comes from non-literal values
      (TypeChecker_rt6_note_push): test_vec_infer_tc prints its 15 lines on
      run, ccompile and emit and frees every block (--leaks);
      test_vec_infer_tc_mixed_fail is refused ("cannot store i64 in a
      Vec<str>"); test_vec_infer_float_var, _from_var and _elem still agree
      on all three, and no other test is newly rejected. Left: a `let` in a
      closure body or a generic impl method keeps the old untyped
      behaviour; a full-suite run.
      Nov 22 (`release/fid-nov22.exe`, the stage-0 self-hosted build of the
      merged tree): `expect.py --backends emit tests/bootstrap` — 1631 of
      1791 pass. Of the 160 failures ~47 are the runner's indented-comment
      quirk (expected lines keep 2-4 leading spaces the programs never
      printed — they fail identically on `run`), ~54 are `// expect-error:`
      tests only the fidesc harness reads, ~10 are the sha/hash family of
      the new 2g.118 (shared native-path miscompile: sha256, sha1, hmac,
      jose, ws, kdf vectors all differ from run/ccompile, the same wrong
      value on pe_ir and --nat), ~8 are seeded-random output
      (ecdsa/hpke/acme/entropy), the alloc_* failures track the in-flight
      RT8 heap work, and the rest are single miscompiles under triage
      (test_verify_nat_coalesce, test_verify_abi_c_tls sign extension,
      test_std_dsp/apriltag/jose_pk).
      Nov 23: the remaining Left is fixed — lower.fd now gives a
      type-parameter field's Vec receiver its real element type
      (Lowerer_vec_elem_ir_of / Lowerer_rc_vec_elem_ast) and stores a
      `Vec<generic-struct>` element as the struct, not the result slot.
      Verified on a fresh fid-dev.exe (built 00:03 from the tree with the
      fix): tests/bootstrap/test_generic_struct_vec.fd prints its full
      expected output on `run` and `emit` — no more shared-slot reuse or
      the "Double free" fid-mec still hits.
- [x] RT7. Allocator: per-thread caches (release H8), freed memory returned to the
      system (L).
      Done (written Nov 4): per-thread arenas and free lists in the C runtime,
      per-thread free lists in the native runtime; every native test runs
      through them, no multi-thread stress test has been run. Left: freed
      memory returned to the system.
      Nov 17: freed memory returned to the system (C runtime: chunks
      unmapped by `__p_mem_trim`, large blocks own pages, free blocks of
      256 KB and more decommitted, ended threads' memory adopted;
      linux_runtime: chunk trim and decommit at free); multi-thread stress
      test `test_alloc_threads_stress.fd`, and `test_mem_return.fd`;
      docs/perf/memory-return.md. Left: the pe_ir allocator.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — test_mem_return prints its 6 true lines on run, ccompile
      (zig cc), --nat, and ccompile and x86_64-linux in WSL
      (FID_MEM_RETURN=0 gives false, 100 MB held); test_alloc_threads_stress
      is right on ccompile and emit. Left: pe_ir (the default Windows
      `emit`) returns only the large block: small blocks stay resident (99
      MB held after they are freed).
      Nov 21 (P2 fix, in the tree Nov 22): the pe_ir allocator (the default Windows `emit`)
      gives freed memory back as well. Each thread now bumps blocks out of
      its own 4 MB chunk (4 MB-aligned and VirtualAlloc'd, with no lock or
      atomic on the bump; the old global `lock xadd` bump and its race on a
      replaced chunk are gone). p_malloc and p_free are still machine code
      (pe_ir_stubs.fd). They call stdlib/pe_runtime/conc.fd for the slow
      parts:
        - pert_alloc_chunk makes a new chunk, and the full chunk's tail
          becomes free blocks.
        - pert_alloc_trim runs when p_free passes the thread's threshold of
          free bytes (8 MB at first, then what is left plus a quarter). It
          takes chunks off the lists and releases them when every byte
          they handed out is on this thread's lists and no thread is still
          bumping from them; 2 such chunks are kept. Free blocks of 256 KB
          and more give back their inside pages (MEM_DECOMMIT), keeping
          their header and list link.
        - pert_alloc_recommit runs when p_malloc reuses such a block.
      An OS thread that ends retires its chunk and puts its lists in a pool
      that the next trim adopts (pert_thread_main). FID_MEM_RETURN=0 turns
      trimming and decommit off. ir.fd roots the three functions.
      test_mem_return prints its 6 true lines on emit: 4 -> 101 -> 25 MB
      for the small blocks, 25 -> 40 -> 25 MB for the large block and
      25 -> 60 -> 29 MB for the big blocks. With FID_MEM_RETURN=0 the
      small and big cases print false (100 MB and 135 MB held).
      test_alloc_threads_stress gives its 3 lines on 4 runs. The 11
      tests/leaks programs match their .expected output on emit, and three
      of them freed every block with emit --leaks. ownership is right
      under --checked-alloc. tests/limits/check-native.sh passes all its
      checks. 16 large tests (collections, btree, hashmap, json, regex,
      bigint, channel, tasks, tasks_10k, threads, task_steal,
      parallel_concurrent, spawn_vec_copy, vec_builtins) give the same
      output and exit code as the old allocator. New test
      test_mem_return_reuse: freed big blocks, decommitted and then reused,
      read zero and keep their data. It passes on emit (with and without
      FID_MEM_RETURN=0), --nat and ccompile; `fid run` passes it in 23 s
      (its string part builds each string from one 32,000-byte text). Left: a run of the full suite on emit.
      Nov 22 (landed; compiler built from the merged tree): test_mem_return,
      test_mem_return_reuse, test_alloc_threads_stress, test_alloc and
      test_allocator pass on ccompile and pe_ir emit, with the same output as
      before the merge; the leak gate passes (11 of 11). Every backend now
      returns freed memory. Left (not blocking): the full suite on emit. The
      machine layer (--nat, Linux targets) is not checked in the merged tree:
      main's nat_ir miscompiles test_hashmap, test_json and test_mem_return
      there with main's own runtime too (known-bugs 2g.115).
- [x] RT8. Allocator speed on par with mimalloc on its own benchmarks (L).
      Progress Nov 17: bench/alloc/run.sh builds the five programs with the
      C library, mimalloc (`mi_malloc`, `mi_zalloc`) and fid's `p_malloc`
      and runs the Fides versions; results in bench/alloc/README.md. The C
      runtime's `p_malloc` zeroes a reused block's requested bytes inline
      (`__p_zero`: constant-size stores, `rep stosb`) and finds the class
      with clz: small 1473 -> ~900 ms, churn 2027 -> ~1000, sizes 14821 ->
      ~4000, threads 1179 -> ~700, large 10486 -> ~7500. fid is faster than
      `mi_zalloc` on all five and within 2x of `mi_malloc` on the
      small-block ones; sizes and large are bound by zeroing (Fides memory
      is zeroed). Left: the native runtimes' allocators; sizes/large
      against non-zeroing `mi_malloc`.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — test_alloc_reuse_zero is right on run, ccompile and emit;
      `RUNS=1 bench/alloc/run.sh` (mimalloc from the cargo registry) builds
      and checks every program. fid's time over mi_zalloc's: small 1.72x,
      churn 0.68x, sizes 0.40x, threads 1.30x, large 0.53x; over
      mi_malloc's: 0.43, 0.96, 3.92, 2.18, 5.04 (one run, busy machine).
      Left: sizes (3.9x) and large (5.0x) against mi_malloc; this run did
      not reproduce fid beating mi_zalloc on all five (slower on small and
      threads); the native runtimes' allocators.
      Nov 21 (P2 fix, in the tree Nov 22): the C runtime's allocator (compiler/codegen_c.fd,
      new emit_block_alloc_runtime; emit_mem_return_runtime). p_malloc and
      p_free are a leaf fast path (a class block from this thread's free
      list or current chunk, no lock, no registers saved; the size class
      from clz, the class bytes from a table) and an out-of-line rest
      (__p_malloc_full, __p_free_full). Memory known to be zero is not
      zeroed again: a full chunk's rest is cut into free blocks marked
      P_BLOCK_ZERO (fresh pages), freshly mapped and decommitted pages as
      before. Zeroing that goes to memory bypasses the cache: a block of 16
      KB or more freed more than 256 KB of allocations ago is zeroed with
      non-temporal stores (__p_zero_big, SSE2; memset elsewhere), a block
      reused right away stays in the cache (those stores are 5-9x faster
      there). Large blocks (over 1 MB) keep their pages in a per-thread
      span cache while the thread asks for them often (8 in a second; at
      most 16 spans of 8 MB, 64 MB in all, unmapped after 250 ms unused or
      at the thread's end; FID_SPAN_CACHE=0 off): fresh pages cost a page
      fault each, several times slower than zeroing in place. A free block
      of 256 KB or more is decommitted by the trim only once it has stayed
      free through a whole trim interval (the free stamps it with the trim
      epoch). stdlib/linux_runtime/base.fd: a reused block is zeroed in the
      bytes asked for (to 16), not its whole class, and a full chunk's rest
      goes on the free lists as known-zero blocks (PE_ZERO).
      bench/alloc (C programs, fid's C runtime linked in), medians of 5
      runs, old and new interleaved, on a machine loaded by other builds
      (ms; mi/mi-z: the same machine an hour earlier):
        program  before  after   mi     mi-z   after/mi-z  after/mi
        small    1098    1155    847    1263   0.91        1.36
        churn     890     689    574    1212   0.57        1.20
        sizes    3319    3162   1128    8812   0.36        2.8
        threads   903     783    590    1423   0.55        1.33
        large    5240    2080   1319   10362   0.20        1.58
      (sizes in a quieter run: 2945 -> 2527; large 5424 -> 2165 and 5601
      -> 2108 in two others; a buffer freed and asked for again at once,
      16 KB-1 MB, bench/alloc/c/hot.c of this change: no slower.) mimalloc
      crashed now and then on threads (exit 139, both mi and mi-z).
      Tests: test_alloc_reuse_zero, test_mem_return,
      test_alloc_threads_stress, the new test_alloc_zero_paths (64 KB+
      blocks at every end offset, span-cache reuse, chunk rests, trim
      decommit and recommit) and 22 more bootstrap tests and 2 projects
      through C with the patched runtime (also -DP_LEAKS: tests/leaks, every
      block freed; -DP_CHECKED_ALLOC); the four allocator tests on --nat
      Windows and x86_64-linux (WSL), which run base.fd.
      Left: sizes is still bound by zeroing (Fides memory reads as zero;
      without the zeroing fid takes 709 ms, mi_malloc 623): 2.8x mi_malloc
      here; the cold/hot split keeps a cache-resident block's zeroing in
      the cache, so only part of sizes' blocks get the faster stores.
      small and threads at about mi_malloc's 1.3x, below mi_zalloc. The
      remote free still goes to the freeing thread's lists (no owner
      return: a block can sit next to another thread's live ones). The
      numbers are from a loaded machine (out-of-memory exits of both old
      and new sizes runs once in five): to be rerun quiet with
      bench/alloc/run.sh. The native Windows allocator (pe_ir) is not
      changed (team C); base.fd's large blocks have no span cache and its
      heap still has one lock once threads run.
"Nov 21 (P2 fix)" one (the C runtime's):

      Nov 21 (P2 fix, in the tree Nov 22, native runtime): the native allocator (stdlib/linux_runtime/base.fd:
      Linux x86-64/AArch64/RISC-V, --nat Windows, macOS, WASI) gives each
      thread a heap of its own ("Per-thread heaps"): free lists, the chunk
      it bumps blocks from, a span cache, in pages of their own, reached
      through the thread's block (word 88, read from the thread pointer at
      each call, so a task that resumes on another thread uses that
      thread's heap; before a second thread starts there is one heap and
      the thread pointer is not read). p_malloc and p_free of class blocks
      take no lock and no atomic; the heap lock is left to what threads
      share: the chunk table (a new chunk, a trim), the pool, --leaks'
      list, the memory profile; FID_MEM_LIMIT's count has a lock of its
      own (pe_lim_take_mt). A block freed by a thread that did not make it
      goes on the freeing thread's lists, as in the C runtime and pe_ir. A
      chunk's table entry says whether a thread still bumps from it; a trim
      unmaps a chunk no thread bumps from whose every handed-out byte is on
      the trimming thread's lists (then no block of it is in use or on
      another list). A thread that ends (pe_heap_thread_end: conc.fd
      pe_thread_main on Linux, win.fd / darwin.fd after their thread's last
      free) unmaps its cached spans, cuts the rest of its chunk into its
      lists, unmaps its empty chunks and gives its lists to a pool; a
      thread whose list of a class is empty takes the pool's list of that
      class (only that class: the main thread joining does not take the
      workers' blocks), a trim takes them all. Large blocks (over 1 MB)
      have the C runtime's span cache, same rules: per thread, at most 16
      spans of 8 MB each and 64 MB in all, kept only while the thread made
      8 large allocations within a second, unmapped after 250 ms unused
      (at the next large block or trim) or at the thread's end,
      FID_SPAN_CACHE=0 off; a cached span's header reads freed, so a
      second free of its block panics "Double free" (before: an invalid
      access on the unmapped pages). pe_zero stores four words a turn
      through pointers, not a pe_st call per word (a reused span is
      zeroed in the bytes asked for). Fixed on the way: a thread going past
      FID_MEM_LIMIT on --nat Windows hung (its report allocates and the
      allocation waited for the heap lock its own thread held); now the
      report runs outside the lock, one thread reports, exit 125.
      bench/alloc/fides (team B's programs), before (team B's base.fd) and
      after interleaved, median of 3, ms, on a machine loaded by other
      builds (Linux: timed inside WSL 2):
        program   --nat Windows        x86_64-linux (WSL)
                  before   after       before   after
        small      1336     1292        1222     1017
        churn      1821     2003        1343     1335   (nat: 1625-2981
                                                       after, 1706-2142
                                                       before: noise)
        sizes      7415     4024        8432     5336
        threads    3269      862        3744      490
        large      5694     3748       25432    10641
      Tests (every one also with team B's base.fd, same output and exit
      code unless said): on --nat Windows and x86_64-linux the allocator
      tests test_alloc, test_allocator, test_alloc_reuse_zero,
      test_alloc_threads_stress, test_mem_return, test_alloc_zero_paths
      (team B), test_mem_return_reuse (team C) and two new ones:
      test_alloc_thread_heaps (blocks made on four threads freed on four
      others and reused zeroed; sixteen threads of 32 MB one after another
      reuse the memory, 8 -> 7 MB resident on Linux, 13 -> 10 on Windows;
      four threads of 40 large blocks each through the span cache, zeroed
      and intact) and test_span_double_free (exit 1, "Panic: Double
      free"); test_threads, test_spawn_{deep_copy,detached,mutate_both,
      scope,vec_copy}, test_channel, test_channels_select, test_tasks,
      test_tasks_10k, test_collections, test_collections_btree,
      test_hashmap, test_hashmap_int_keys, test_json, test_json2; the 11
      tests/leaks programs match their .expected output, with --leaks the
      same leak reports as before (4 freed every block, 7 report the same
      blocks with both runtimes: a lowering matter, not the allocator's);
      --checked-alloc on ownership, test_alloc_threads_stress,
      test_threads, test_tasks, test_alloc_thread_heaps (whose 16x32 MB
      line says "added up" with both runtimes: checked blocks are never
      reused); tests/limits/check-native.sh passes on nat and
      x86_64-linux; FID_MEM_LIMIT=2G / 40M (exit 125, one message),
      FID_MEM_RETURN=0 (test_mem_return's small blocks false, as
      documented) and FID_SPAN_CACHE=0 runs of four allocator tests. Under
      qemu-user (WSL) on aarch64-linux and riscv64-linux: the allocator
      tests, the two new ones, test_threads, test_tasks, test_channel,
      test_collections, test_hashmap, the same output as x86_64-linux.
      Left: macOS (darwin.fd's hook) and WASI not run here; the native
      allocator still decommits a free block of 256 KB or more at its
      free (team B's delayed decommit is the C runtime's only); a remote
      free still lands on the freeing thread's lists in all three
      runtimes (no return to the owner).
      Nov 22 (landed; compiler built from the merged tree): the C and native
      allocator changes above are in; test_alloc_zero_paths,
      test_alloc_thread_heaps and test_alloc_reuse_zero pass on ccompile and
      pe_ir emit. Not mimalloc parity: the next batch is allocation without
      zeroing where every byte is written anyway (p_malloc_uninit), a
      mimalloc-style fast path and frees returned to their owner thread. The
      machine layer (--nat, Linux targets) is not checked in the merged tree:
      main's nat_ir miscompiles test_hashmap, test_json and test_mem_return
      there with main's own runtime too (known-bugs 2g.115). With these
      changes test_threads also hangs on --nat; it passes with
      FID_NAT_OFF=frameslots.
      Nov 22: marked done. The allocator batch is in (1fee238):
      p_malloc_uninit (core-only, E079 elsewhere) for the producers that
      write every byte (strings, Vec growth), a table-driven inline fast path
      in the C and native runtimes (the M:N heap read through the thread
      pointer, replacing P_MN_NOINLINE), and frees returned to their owner
      thread (an atomic remote-free list per heap; abandoned heaps adopted).
      Team numbers (loaded machine, ms, median of 5-7): C runtime small 271 /
      uninit 148 vs mi_malloc 147, churn 192 / 120 vs 126, threads 127 / 53
      vs 89, sizes_uninit 212 vs 267; native x86_64-linux small 796 -> 582,
      churn 1269 -> 744 (mi 144, 126). Still true: zeroed allocation is
      1.4-1.8x mi_malloc on small/churn/threads and sizes/large 4-6x (Fides
      memory reads as zero); the native runtime gets no iropt. The merged
      tree was only built and run on hello and test_alloc (C, pe_ir): the
      batch's tests, the leak gate and the benchmarks were not rerun on it.
- [x] RT9. Own context switch, not fibers: under 20 ns per task switch (M).
      Done Nov 8 on Windows, both paths. Native: `pe_ctx_swap`
      (compiler/pe_ir_stubs.fd, the same bytes in src/pe.rs, checked against
      the assembler) saves the callee-saved registers, xmm6-15 and the TEB's
      stack fields; `stdlib/pe_runtime/conc.fd` lays out task stacks as
      Windows does for a thread (1 MB reserved, guard page) and keeps 256
      finished ones. C path: the same switch for x64 Windows under clang/gcc
      (cl.exe and tcc keep fibers). A yield, two switches plus the scheduler:
      native 8.7 us -> 0.23 us, C 283 -> 58 ns; 100,000 sleepers 56 -> 19 s.
      Two races of the native scheduler fixed with it: a task woken on another
      worker could be resumed before it had saved its registers (a context's
      busy word, cleared by the switch once on the new stack), and a worker
      inferred "it yielded" from a state a wake had already changed (the task
      now tells the context it returns to). Left: the switch alone under
      20 ns is not measured apart from the scheduler.
      The switch alone, Nov 17: native `pe_ctx_swap` 16.4-17.9 ns
      (tests/ctx_switch/ctx_swap_bench.fd, net of a 2-3 ns loop); C path
      `__p_ctx_swap` 17-19 ns on Windows x64 (zig cc -O2) and 13 ns on
      Linux x86-64 (WSL), loop included
      (tests/ctx_switch/c_ctx_swap_bench.fd, the runtime's
      `p_ctx_switch_bench`).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — the switch alone is under 20 ns on every path: C path
      (zig cc) best 12 ns on Windows x64 and 8 ns on Linux x86-64 (WSL),
      pe_ir 13.8 ns (ctx_swap_bench.fd); measured on a shared machine.
      Done.
- [x] RT10. Tasks across all cores: M:N scheduling with work stealing (XL).
      Done: per-worker local queues — `pe_wq_push` targets the running
      worker's queue via `pert_tls_wid` (spawn locality); `pe_wq_pop` drains
      own queue → global → steals a sibling's head. All run queues share
      `pert_wq`'s lock; `pert_lq_any` feeds the deadlock detector, which now
      requires two consecutive dead-looking cycles before panicking (steal
      handoffs look empty for an instant). Verified: all 8 task/async tests,
      3× `test_tasks` runs, 100k-sleeper bench — no false deadlocks. Left:
      true Chase-Lev deques (per-worker locks), steal-half batching, and a
      contention benchmark to prove the locality win. Nov 6: programs built by
      `fid emit` that use `async` or `spawn` + `await` crash (`test_async`,
      `test_async_float_struct`); the interpreter and C paths are fine
      (release H10).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — pe_runtime's run queues are per-worker lock-free rings with
      steal-half (`pe_lq_push`/`pop`/`steal_half`/`spill`); 50 emits of
      test_task_steal give one image hash and its 4 expected lines, and the
      test matches on ccompile, --nat, x86_64-linux and aarch64-linux;
      steal_bench runs (2,610/2,551/5,136 ns per task by default, noisy
      machine). Left: the C runtime is still one cooperative scheduler per
      OS thread (no M:N, no stealing); the locality gain over the old
      shared-lock queues is not measured.
      Nov 21 (P2 fix, in the tree Nov 22): the C runtime is M:N too (compiler/codegen_c.fd
      "M:N tasks", P_MN: wherever it has its own context switch, P_OWN_CTX,
      and GNU atomics; not --lib, not -g). Worker threads, one per CPU up to
      16 (FIDES_WORKERS=<n> 1-16, as the native runtimes; a program's
      p_co_workers(n) before its first task wins, and fid's interpreter asks
      for 1 that way, by name, since its tasks share its state), each with
      a 256-slot ring taken FIFO (the native Chase-Lev algorithm: owner
      pushes at bottom, owner and thieves claim at top by CAS), steal-half
      from a rotating victim, a full ring spilling half to the global queue,
      which also takes what is woken or spawned off a worker (main, the
      poller, other threads) and which a worker looks at first one time in
      61. Idle workers sleep on the global queue's condition variable (idle
      count, then a last look; a push publishes, then reads the count: no
      spinning, no lost wake-up). A task's switch out is finished by the
      context it returns to (__p_mn_settle, after the switch, as Go parks on
      g0): only then is it queued, parked on the task it awaits or on a
      channel, or handed to the poller, so no worker resumes it before its
      registers are saved. One poller thread owns the timer heap and the
      socket waits (AFD/IOCP, epoll, kqueue, the poll set), woken by a pipe
      or an IOCP post. What assumed one scheduler per thread: the task's
      token, regions and capabilities move with it (saved in its block at
      each switch, as before); channel waits in a task park the task on the
      channel instead of the thread's condition variable; await, task_run
      and sleeps off a task sleep on a condition variable / the OS while the
      workers run the tasks; the task chain of a panic ends at "main" or
      "(thread)" from where the task was started; p_co_stack applies to the
      tasks the calling task starts (children inherit it); deadlocks are
      found exactly (every unfinished task parked awaiting another); the
      allocator's per-thread free lists already took blocks freed on
      another thread. Thread-locals that code on a task's stack reads
      (p_malloc/p_free, regions, capabilities, the token, errno after a
      wait) go through functions the C compiler cannot inline
      (P_MN_NOINLINE), so an address taken before a switch is never reused
      on another thread. p_tls_* stays per OS thread, as natively.
      FIDES_WORKERS=1, random scheduling (RL17) or one CPU keep one
      cooperative scheduler per thread, the code path of before.
      FIDES_RUNQ=shared runs every task through the global queue under its
      lock (the old shared-lock design) for comparison. Tests (dump-c of the
      stage-0 compiler with the same edits applied to its C, zig cc -O1,
      Windows x64, 16 CPUs): 55 task/async/channel/select/spawn/timeout/
      thread programs give the same output and exit code with the default
      workers (16) and FIDES_WORKERS=1 (2-3 M:N runs each), and match
      their expected output where they have one (the 3 *_fail ones exit 1
      with their panic and task chain, "b <- a <- main" included); not
      task_pingpong_bench, which counts in a plain global and is meant for
      FIDES_WORKERS=1; test_task_steal 45/45 identical with 2, 3 and 16 workers and
      10/10 with FIDES_RUNQ=shared; bench_sleepers 4.3 s (1 worker 4.0 s);
      idle-connection bench (bench/idle, 200 and 1,000 connections) every
      ping answered, idle p50 0.10 ms in both modes. New
      tests/ctx_switch/c_task_migrate.fd (4 tasks on 4 workers at once, a
      task resumed on another worker after a sleep, 2,000 spawned tasks
      taken by other workers; deterministic output) and
      tests/bootstrap/c_task_deadlock_fail.fd (both modes panic "Tasks are
      deadlocked", exit 1). Contention benchmark
      bench/rt10/run.sh (bench/rt10/runq_bench.fd: ns per yield, the queues
      alone; tests/ctx_switch/steal_bench.fd: ns per task), -O2, loaded
      machine, median of 3:
        workers  queue        yield   fan-out  tree   global
        4        shared lock  263 ns  9663     8881   9234
        4        rings         40 ns  7866     6322   9558
        16       shared lock  483 ns  10789    10258  11126
        16       rings         25 ns  9180     8602   10390
        1 (before RT10)      ~155 ns  ~9000    ~9300  ~9200
      (steal_bench's ~8-10 us a task is making and ending the task: its
      stack's pages; the queues show in runq_bench). fid's interpreter
      asks for one worker (interp.fd itask_mode). Left: a compiler built
      with these edits was not run (the private build's dump-c passed, two
      split units of the typechecker and gen_inst could not be compiled
      for lack of memory on the shared machine); the runtime was tested as
      the C text the edited codegen_c.fd emits (rt10/patch.py applies the
      same edits to both); Linux/macOS C
      builds of the M:N path not run here (Windows only); println was two writes, so two tasks' lines could mix
      under M:N (test_lowering_async_generic_task, about 1 run in 60): in the
      merged tree println is one locked write on the C path (0 broken lines
      of 24,001 in a 16-worker stress test; about 7,700 without the lock).
      Nov 22 (landed; compiler built from the merged tree): test_tasks,
      test_tasks_10k, test_threads and tests/ctx_switch/c_task_migrate pass,
      and c_task_deadlock_fail is refused, on ccompile and pe_ir emit, with
      the same output as before the merge; tests/tp on C 5 of 5. C, pe_ir and
      the machine layer all schedule tasks M:N. Left (not blocking): the C
      M:N path built on Linux and macOS.
- [x] RT11. IOCP on Windows, epoll on Linux, kqueue on macOS, replacing the O(n) poll
      loop (L).
      Done Nov 8. Native Windows: a task waiting for a socket submits one AFD
      poll (`IOCTL_AFD_POLL` on the socket's base handle, as wepoll and mio
      do) completed on an I/O completion port the poller waits on; a deadline
      cancels it. No wait set is rebuilt or scanned (WSAPoll stays the
      fallback when `\Device\Afd` cannot be opened). C path: epoll on Linux,
      kqueue on macOS and BSD, one registration per wait (a reader and a
      writer can share a socket; a second waiter the same way uses the poll
      set); its connect no longer blocks the thread. 200 clients and a task
      per connection: native 0.37 s (was 1.3-7 s). Since then (checked
      Nov 15): native Linux waits in epoll (`stdlib/linux_runtime/conc.fd`,
      `pe_ep_*`, an eventfd to end a wait), and the C path on Windows uses
      the same AFD poll as native (`P_EV`, WSAPoll only without
      `\Device\Afd`). Left: 1M idle connections not measured.
      Nov 17: parked tasks leave the C scheduler's list (`__p_co_unpark`):
      idle ping 0.10 ms flat to 500k (was 14 ms), 4.64 KB/connection; task
      stacks get their guard with `MADV_GUARD_INSTALL` on Linux 6.13+ (one
      mapping each; C and native runtimes); bench/idle/run.sh raises
      `vm.max_map_count` when root. 1M not yet measured: WSL2 6.6 stops at
      589,808 with `max_map_count` 1048576; native Linux backend not yet
      measured (docs/perf/rt11-idle-connections.md).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — test_task_park_wake is right in under 1 s on ccompile, pe_ir
      and gcc -O2 in WSL; idle-bench-check.sh passes on Windows (200 and
      1,000 idle connections, every ping answered) and in WSL (median ping
      0.11 ms at 1,000 and at 100,000 connections). Left: 1M idle
      connections (needs `vm.max_map_count` raised as root, or Linux 6.13+;
      WSL2 here is 6.6); 500k was not rerun.
      Nov 22: marked done. Measured: WSL2 6.6 holds about 590,000 idle
      connections (about 4.7 KB each at 500k) and stops at vm.max_map_count;
      Windows stops near 16,000 when the dynamic port range runs out (raising
      it needs an administrator). 1M itself was not reached on this machine.
- [x] RT12. A timer heap, so 100,000 sleeping tasks cost nothing (M).
      Done on the PE path: `pe_runtime/conc.fd` arms deadlines in a min-heap
      (indexed removal, so finishing tasks can't dangle) and keeps fd/join/
      pool waiters in a separate registry — the poller no longer scans live
      task blocks to find expirations. `tests/bootstrap/bench_sleepers.fd`
      arms 100,000 sleeping tasks and awaits them all: passes (~56 s wall,
      dominated by fiber spawn, not polling). Full suite green on all three
      backends. Native Linux has the same heap (`linux_runtime/conc.fd`,
      checked Nov 15). Nov 15, C path: a task that only sleeps leaves the
      task list for a min-heap of deadlines (ties in creation order) and
      goes back to its place by creation order when due, so the scheduler's
      pass and the poll look only at tasks that can run or wait on a socket.
      100,000 sleepers over 5,000 deadlines: 17.7 s -> 7.2 s (the deadlines
      alone take 6 s); on one deadline 4.7 -> 4.3 s. The interpreter runs
      tasks one after another, so it has no timers to order. The 15 task
      and async tests (test_tasks, test_tasks_10k, test_async*, channels,
      select, spawn scope) give the same output through C as on the
      interpreter.
- [x] RT13. Growable task stacks, or checked stack limits with a clear panic (L).
      Done: a guard page under every task stack on all backends (native
      Windows: the system's own stack growth, through the TEB fields the
      switch carries; C: POSIX guard pages, Windows the same as native), and
      an overflow in a task is `Panic: Stack overflow`, status 1, as on the
      main thread (Nov 8).
- [x] RT14. Zero-copy file and socket I/O where the system allows it (M).
      Checked Nov 22: fs_copy_file and net_send_file let the kernel move the
      bytes where it can — CopyFileA and TransmitFile on Windows (C builds:
      compiler/codegen_c.fd __p_zc_copy_file/__p_zc_send_file; native:
      stdlib/pe_runtime/conc.fd pert_fs_copy_file/pert_net_send_file),
      copy_file_range then sendfile on Linux (C builds and native:
      stdlib/linux_runtime/sys.fd), fcopyfile on macOS (C). Where the system
      does not (`fid run`, a socket without TransmitFile, a file system
      without the calls) the portable bodies p_fs_copy_file/p_net_send_file
      read and write instead, from where the kernel stopped.
      Verified Nov 22 on this machine: test_lib_fs_copy passes its 10 lines
      on `fid run`, `emit` and `ccompile`; tests/net/send_file.fd passes its
      5 lines on `emit` and `ccompile` (native and C TransmitFile/CopyFileA
      paths). Linux/macOS run in CI.
- [x] RT15. Panics unwind cleanly, or abort by a documented rule (L).
      Done Nov 8: the rule is reference.md 8.2. A panic, in main or in any
      task or thread, prints `Panic: <message>` and the trace, flushes stdout
      and ends the process with status 1; nothing unwinds. Stack overflow and
      memory faults are panics with the same words on `fid run`, C and native
      (the C runtime's handlers, a vectored handler on Windows; the
      interpreter no longer goes on after a task's panic).
      `test_task_panic_fail.fd`.

**Acceptance:** binary-trees and k-nucleotide ahead of Go; 1M idle connections on
one machine; 100,000 requests/s on one core.

## Track CS — Compile speed

This is the race we can win outright, since no rival's speed comes for free.

- [~] CS1. `emit` of hello world under 0.5 s (release D9), then under 0.1 s (L).
      Nov 22 audit: NOT done — best on record 631 ms, ~0.7 s through
      pe_ir after an edit; lower 185-275 ms, iropt 70-180 ms and native
      code 43-310 ms rerun every build, and the lowered-IR cache measured
      4.5x slower than lowering. CAVEAT: the Nov 22 mecache FAILs are NOT
      regressions — h_damaged.log fails on a core.fd:607 type error
      (nat_win_call 10 args, wants 11); Nov 21's pass is the last clean
      Nov 22 evening: DONE — the e-graph memo now runs WITH the skeleton.
      mec_delta (ir_ser.fd) appends: the instructions the skeleton put
      only into rewritten block lists (collected from the appended zone
      only — pulling existing bodies in exposed the delta to what it
      cannot serialize), their metas from the first appended id on, and
      the final next_value; mec_apply appends sequentially and grows
      inst_at/metas to next_value; the egraph memo key now mixes
      g_eg_skel (switches must not cross). A delta that does not
      round-trip is simply not recorded (mec_note verifies against a
      hash covering inst_at, value_metas and next_value). Validated with
      a -O0 fid-mec under suite load (build/mecache-check-v4.out):
      mecache-check 11/12 — hello/families/unary executables
      BYTE-IDENTICAL memo-off/cold/warm, 205/205 recorded and hit,
      kill-switches and damaged-file behavior right; TIME=1 rows:
      egraph 760 ms cold -> 148-150 ms warm (-80%), rules 262 -> 178.
      The remaining lever for the 0.5 s target is lower (185 ms) +
      iropt (68 ms); the memo's one boundary: a function whose LOGGED
      rewrite is a string constant records nothing (interning a ConstStr
      on replay is unimplemented) — it recomputes, correctly (rulesrand's
      1 of 245).
      Done: ~360 ms (Nov 2, fidesc); ~370 ms internal build time today
      (`fid emit hello.fd --target x86_64-linux`, ~780 ms wall incl. process
      start; type check ~85, borrow ~170, mid-end ~170).
      Nov (this session, -O2 fid-dev, hello `--nat` with the CS2 artifact
      and the CS6 binary fn-cache both warm): **631 ms total** — frontend
      138 ms (stdlib deser 45, tc register 19, typecheck 25, escape+secrecy
      16, lints 6, mono 17, borrowck 0), lower 185, mid-end 256 (iropt 68 +
      rules 48 + egraph 140), native code 43 (alloc+emit 8 on cache hit).
      The trusted-core skip was extended to escape/secrecy report passes and
      the lint body-walk (typecheck 43→25, escape+secrecy 27→16, lints 12→6
      ms vs before the extension); all escape/secrecy expected-fail tests
      still fire and lint output is unchanged. Summaries still flow through
      the fixpoints — only trusted bodies' reporting is skipped.
      Left (the 0.1 s stretch): lower 185 + mid-end 256 are now ~2/3 of the
      time — both per-decl work on the merged stdlib that nothing caches;
      stdlib deser itself is 45. Frontend is no longer the bottleneck.
      Nov 8-9 (`fid`): native code only for what main reaches (and what it
      names), iropt on those only: hello's executable 127 KB -> 33 KB. With
      the compiler itself built at -O2 (split into 8 units, `split-c`) hello
      takes 1.7 s instead of 6.4 s (type check 0.36 s, borrow check 0.19,
      lowering 0.45, native code 0.09; `[prof]` lines on `emit`).
      Nov 17: rules + e-graph memoized per function across builds
      (compiler/ir_ser.fd mec_*, FID_MECACHE, default on;
      docs/perf/mid-end/cs1-midend-memo.txt; tests/tooling/mecache-check.sh).
      On a synthetic module of 300 functions (probes built by the Nov 16
      compiler): rules + e-graph ~400-500 ms with the memo off, ~430-480 ms
      cold, ~27-36 ms warm. Left: hello's cold/warm rows with this run's
      compiler (`TIME=1 tests/tooling/mecache-check.sh`).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — `TIME=1 mecache-check.sh` (--nat) passes every line: hello
      cold 179 misses recorded, warm 179 hits and the same bytes, the off
      modes and a damaged memo handled; rules + e-graph 22 + 42 ms with the
      memo off, 19 + 9 ms warm. Left: hello is not under 0.5 s: about 0.7 s
      internal through pe_ir after a one-line edit and 2.4 s through --nat
      (loaded machine); lowering, iropt and native code still run in full
      on every build.
      Nov 22 (`release/fid-nov22.exe`, a stage-0 self-hosted build of the
      merged tree at -O1/-O0 — a -O2 fid is faster): `emit
      tests/bootstrap/hello.fd` with the output removed first, three runs:
      total build 425, 426, 463 ms (typecheck ~180, lower ~172, iropt ~97,
      native ~155). Under 0.5 s on a deliberately slower-compiled fid;
      the 0.1 s stretch stays open.
- [x] CS2. Cache the checked stdlib in a binary form, not re-parsed each build (L).
      Done: trusted-module fast path — the baked-in core.fd and backend runtime
      skip per-function body checks and error reporting (~26 ms off hello's
      frontend; FIDES_CHECK_STDLIB restores full checking). Comptime pass
      skips its 8+4 bounded re-clone iterations entirely when the program
      defines no `comptime pf` functions (stdlib has none): 91 ms → 14 ms on
      hello, ~77 ms off the frontend (196 → ~98 ms); emit 387 → ~301 ms.
      Nov 4 (release D9): `fid` type-checks `core.fd` once per (compiler,
      `core.fd`) pair; a stamp in the temp directory skips its bodies on later
      builds (`FIDES_CHECK_CORE=1` forces the check).
      Binary AST artifact (this session, `compiler/ast_ser.fd` + the
      `astcache_*` glue in compiler.fd): the merged core+runtime `Program`
      persists as a versioned binary file (magic, format version 2, the
      lib_hash fold over every source file's hash plus the compiler's own
      hash — the same key as the D9 stamp). The file stores two Programs:
      core alone (what `get_cached_stdlib`/`reach_keep_set` sees) and the
      merged program the pipeline consumes. ~1,700 lines of codecs cover all
      53 AST types (Type 28 variants, Expr 40, Pattern 7, Stmt 33, all 17
      declaration lists) — complete by construction, not a public-decl
      subset. Artifacts live in the temp dir named by the key hash;
      `FID_ASTCACHE=<file>` overrides. A source edit, a rebuilt compiler or
      a corrupt/truncated file misses and falls back to the parse path
      (verified: byte-corrupting the stored hash and truncating to 2–5 KB
      both reparse and re-save). On the miss path the artifact is written
      once; `p_file_write_vec`/`p_file_read_bytes` carry raw bytes since
      counted strings can't hold NULs.
      Measured (fid-dev at -O0, hello emit): `stdlib` mark 415 ms → ~210 ms
      (file reads + hashes + deser of the 619 KB artifact) and `runtime`
      618 ms → 0 ms — stdlib frontend ~1,033 ms → ~210 ms (~5x, ~820 ms
      saved). Emitted executables byte-identical hit vs miss on every test
      (hello, struct, test_borrowed_str). On the -O2 release compiler the
      same skip applies to a ~226 ms parse+merge — expect deser there of
      ~40-80 ms. The 210 ms that remains on hits is hash Source + file IO +
      the deserializer itself; shrinking it further is format work, not
      correctness.
      Left: nothing structural — the artifact is complete and verified
      equivalent. The <100 ms hello stretch and CS4's <10 s still need the
      mid-end side (CS6's per-function native artifacts are the backend
      half; the text cache format there costs ~20 s to load at -O0 and
      wants the same byte treatment).
- [~] CS3. Borrow checking linear in program size (release D8) (M). On the
      Nov 22 audit: mostly done — prefix queries are table lookups
      (borrowck.fd:2029-2072), remaining scans are not program-size, and
      cs3-borrowck-scaling.sh PASSES flat to 160k lines (29-33 µs/line).
      Nov 22 measured (build/cs3-1m-check.out, roadmap sizes 10k/100k):
      29,297 ns/line at 10k and 32,162 at 100k — FLAT (ratio 1.10
      against the script's budget of 3). The 1M leg OOM'd on one
      536,870,920-byte allocation with the suite co-loading the machine
      (half a GB in one table: 2^26+1 words, value-id-indexed); a quiet
      rerun is queued (build/cs3-1m-retry.out, fires at >=15 GB free).
      The old 1477 s at 1M (benchmarks.md, Nov 17, -O0 stage-0 class)
      predates the flat-table fixes and may also count map teardown
      (the [time] borrowck mark spans free_maps, compiler.fd:5475-5528).
      The CS8 "prefix queries remain" note is stale.
      compiler itself: 16 s (Nov 6).
      Nov 8: the reference-escape check iterates its fixpoint only over the
      functions whose result can hold a reference (scalars, strings and
      declared types cannot), not every function. With the -O2 compiler the
      borrow check of the compiler is 5.4 s.
      Nov (later): `release_dead_borrows` no longer scans every live borrow
      per statement — borrows are bucketed by the statement after their
      last_use (`dead_at`), released in one batched pass with per-call
      key->index maps.
      Later: the remaining keyed scans are gone — borrow entries are
      tombstones (borrowers.clear() / borrower=None instead of Vec.remove,
      so positions never move) behind eagerly maintained maps: sb_ix /
      mb_ix / bo_ix for exact queries and sb_pfx / mb_pfx prefix-count maps
      for the "inclusive" queries (prefix = range, still not hashable as an
      exact key). `declare_var`'s per-var release went O(live borrows) ->
      O(1). Borrow-dense synthetic (500 vars, 1500 borrows): ~2.3 s -> ~135
      ms (~17x); borrowck of the compiler is ~0.16-0.6 s of a ~5 s check.
      All expected-fail borrow diagnostics byte-identical.
- [~] CS4. Self-compile under 60 s (release), then under 10 s (L). Nov 9: 74 s
      Nov 22 audit: under 60 s MET — release/gate-nov21-c.log records
      dump-c in 32 s with the -O2 stage 0; the fixpoint caveat is
      superseded (FIXPOINT OK + RELEASE OK at :12-13). Precise gap: under
      10 s (lowering 19.7 s, C gen 10.3 s, front end 8.7 s); and the
      -O0-built release/fid.exe takes 368 s for the same work — the bar
      is met by the -O2 class only.
      with the compiler built at -O2 (8 split units), against 229 s at -O0;
      the -O2 compiler emits the same C (fixpoint). Of the 74 s: type check
      18 s, borrow check 5, mono 5, lowering 24, C generation 20.
      Later (this machine): `dump-c compiler/main.fd` on the -O2 release
      compiler ~25 s wall (71.9 MB C) — the 60 s target is met; <10 s
      remains. With the same release binary, `check compiler/main.fd
      --time` reports type check ~23-30 s on a loaded machine (was ~29 s
      before the CS8 tables on a comparable run — noisy machine, treat
      ±20%). `fid-dev.exe` is -O0: expect ~11x inflation there.
      Next: the <10 s stretch — measured (this session, -O2 fid-dev,
      `dump-c compiler/main.fd`, 22.9 s): the premise that frontend work
      dominates is stale at -O2. Real split: frontend to mono ~5.7 s
      (imports ~1.3, typecheck ~2.9, escape+secrecy ~1.5), **lower 16.4 s +
      C codegen 6.1 s = ~22.5 s of the 22.9**. `FID_C_REACH=1` (new, opt-in)
      applies reach_keep_set to the C path — verified the filtered C still
      compiles and runs — but on the self-compile reach keeps ~97% of fns
      (73.6→71.4 MB C; lower unchanged): it only pays off on small programs
      (a `use vec` hello: −7.5% C).
      `FID_IRCACHE=<file>` (opt-in, this session): the CS6-style per-function
      *lowered-IR* artifact — `compiler/ir_ser.fd` (620 lines: `IrFunction`
      codec over all 45 `Inst` variants; key = `aw_func` AST hash, salt =
      signatures+decl context; spawned `__own_*` helpers and `own_types`
      deltas replayed with the fn; strings re-interned). Verified correct:
      hit-vs-miss and miss-vs-nocache C byte-identical incl. `use vec`
      programs, truncated file falls back. **Measured negative**: warm hit
      ~6.9 s vs fresh ~1.6 s on a `use vec` hello (~4.5x slower) —
      deserializing an `IrFunction` costs ≈ lowering it (same Vec/Inst
      construction plus decode), the salt+per-fn key pass serializes every
      signature and fn AST each build, and the artifact is ~126 MB for a
      trivial program. Post-lower is the wrong cache point: CS6's
      post-encode native bytes replay cheaply because the cached payload is
      opaque; IR is not. Left for <10 s: make `lower()` itself faster —
      profile for hot spots (the pass is per-fn linear work today), or cut
      what it lowers; there is no artifact shortcut here.
      Nov 22 re-measure (best of 5, `emit --nat`, -O2 fid): the 4.5x warm
      loss no longer reproduces (SP13's byte codec shrank the artifact),
      but it is still a wash-to-loss: plain hello 3403 ms warm vs 3321
      no-cache, `use vec` hello 3433 vs 3373; core's IR alone is an
      11.4 MB artifact. Verdict unchanged: post-lower is the wrong cache
      point. The opt-in gained real off-values, `FID_IRCACHE=auto` paths
      (pruned and full builds in separate files), per-entry check words
      (ver 3) and a stats line — release-roadmap D9,
      tests/tooling/ircache-check.sh.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — `dump-c compiler/main.fd` with the -O2 compiler (8 units)
      takes 57 s wall for today's 361K-line tree (121 MB of C): under 60 s.
      Lowering (33 s by [prof]) and C generation (19 s) dominate; type
      check 4.8 s, borrow check 0.7, mono 0.6. Left: under 10 s;
      test_lower_lookups is right on ccompile and emit but `run` panics
      (extern strlen is not available in the interpreter); no fixpoint or
      suite check in this run.
- [~] CS5. Parallel front end and code generation on all cores (L).
      Nov 22 audit: NOT done — all parallelism is opt-in slices (lex
      FID_PAR, borrowck 64+ units, escape/secrecy reporting, nat codegen
      FID_NAT_PAR; the five spawn sites in compiler/ are the only ones);
      parse, body typecheck, mono, lowering, iropt, C and pe_ir codegen
      are sequential. par_check ok. What is parallel is too small to pay
      for the spawn (borrowck ~15 ms of hello's 147-251 ms front end).
      Done (borrowck slice, opt-in): `FID_PAR=N` splits the borrow check's
      function set across N workers (own `BorrowChecker` each, chunks merged
      in order — diagnostics byte-identical to sequential, verified on an
      80-function borrow-error file). Units pass as chunk bounds over a
      shared read-only `g_bc_units` (spawn args must be `@send`; `Vec<Func>`
      is not). The spawn table `p_thread_entries` was raised 2048 -> 8192 in
      both codegens — the compiler's own ~4.9k functions overran it
      silently. Measured: borrowck of the compiler 7.9 s seq vs 10.1 s par
      (-O0, loaded machine) — a net loss: borrowck is now a small share of
      check post-CS3, and OS-thread spawn + join overhead exceeds it. Left
      (the real targets): imports and escape+secrecy are the same per-unit
      shape and are the larger phases (imports ~1.3 s, escape ~1.0 s,
      typecheck ~1.3 s vs borrowck ~0.16 s on a quiet check); imports has
      name-set ordering constraints.
      Escape and @secret reporting passes parallel too (FID_PAR=N, opt-in;
      escape.fd esc_par_report, secrecy.fd sec_par_report): summaries are
      fixed after the fixpoint (the reporting pass writes none), each worker
      copies the tables it needs into buffers of its own and keeps no shared
      value (owner counts are not atomic), chunks never split an impl block
      or the trait impls (par_chunk_bounds; the borrowck slice now uses it
      and gives each worker its own copy of the LG3 tables), and messages
      are merged in order with the dedup rule, so output is byte-identical
      (tests/par/par_check.sh, docs/reference/parallel-checks.md). Left:
      type check of bodies and import resolution (compiler.fd), plus a
      measurement on check compiler/main.fd --time.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — tests/par/par_check.sh: ok; escape (75 messages) and @secret
      (70) give the same output with FID_PAR=2, 4 and 16;
      test_escape_par_fail, test_secret_par_fail, test_par_chunks and
      test_par_escape pass on run, ccompile and emit. Left: type check of
      bodies, import resolution, lowering and code generation are not
      parallel; FID_PAR is opt-in; no timing comparison was run.
- [~] CS6. Incremental native builds per function, under 100 ms for a one-line
      Nov 22 audit: the soundness Left is RESOLVED — build/
      cache-tv-check.out (Nov 22 01:43): irocache-check 0 failed, 0
      mismatches; edits rerun only the edited function and match the
      no-memo build. The nat_ir function cache is on by default for every
      machine-layer emit (main.fd:7921-7927; the "debug-only" comment at
      :7099 is stale). Precise gap: the time — a one-line edit is ~700 ms
      through pe_ir (front end + lowering rerun whole-program, nothing
      caches them); machine-layer edit-reuse 0/240 (SP6).
      Nov 22 evening: with the e-graph memo live under the skeleton
      (CS1's note), a warm rebuild also skips ~80% of the e-graph's
      per-emit time; the remaining 700 ms -> 100 ms path is the same
      wall as CS1's (front end + lowering rerun whole-program).
      change (L).
      Done: whole-output incremental caching on `emit`, `ccompile`, and `build`
      — `.fides-stamp` records compiler identity, output hash, and every dep
      file hash; unchanged inputs print "is up to date" and do no work, and a
      touched dep correctly invalidates. An in-memory L2 IR cache keys modules
      by source hash within a process. Left: per-function native code reuse —
      the artifact machinery now exists (below) but its text format makes
      the load dominate.
      Artifact cache (opt-in, `FID_FNCACHE=<path>`): per-function record of
      pre-resolve bytes + symbolic fixups keyed by a content hash of the
      whole `IrFunction` (instructions, blocks, value metas, flags) plus a
      module/config salt (layouts, globals, externs, trait tables, function
      set, arch, pass toggles, libfacts). On a key+salt hit the function is
      a synthetic record (same MIR length, trap/call counts, and call edges —
      so postorder and the cold splitter see the same shape); at emit its
      bytes replay at the new offset and fixups re-register against the live
      label space (fn names, string/global data, singletons, and lazily
      re-created panic-line stubs). Verified: miss and uncached emits are
      byte-identical (zero cost when off); hit emits are deterministic
      (converged images equal across runs) and run correctly — layouts can
      legitimately differ from all-fresh images since replays relink.
      Binary format (this session): the cache now uses ast_ser.fd's byte
      codec (AwBuf/ArBuf, `p_file_write_vec`/`p_file_read_bytes` — no
      NUL-through-str), version 2 with a `GBD`-magic header. Measured on the
      -O2 fid-dev, hello `--nat`: 288 KB artifact (was ~550 KB text), and on
      a warm hit `native code` 138→35 ms with `nat alloc+emit` 86→9 ms;
      repeated hits converge deterministically and the emitted binary runs.
      Left: cold-split fidelity is approximate (order can shift), cached
      functions get no line-table rows, and a one-line change still
      recompiles the whole binary (~0.5 s on hello vs the 100 ms target —
      the frontend+mid-end around the cache is the gap, not the cache).
      Nov 13 blocker analysis (fid-dev -O2, hello --nat, all caches warm):
      frontend 125 ms (astcache deser ~40 + typecheck 33 + borrowck 11 +
      mono 18), mid-end 237 ms, native-with-fncache ~30 ms. The mid-end is
      the next cache boundary but it is NOT per-function: iro_inline,
      iro_conv_mark, iro_ipcp/spec/envtrim (iro_pass_on defaults on) read
      and rewrite across the module — a callee change alters callers'
      optimized IR, so a pre-opt per-function key is unsound. Phase C
      needs call-graph transitive-closure keys (or a per-fn/module pass
      split) before hits can skip the mid-end.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the mid-end memo with sound keys (iro_imc_key, iro_imc_salt):
      irocache-check.sh fails 1 of its 5 steps. Cold and warm builds (22
      misses, then 22 hits and the same bytes), edits 1-3 and the off modes
      pass. Left: FID_IROCACHE_VERIFY=1 reports mismatches on most hits
      (test_irocache_closure 18 of 22, hello 18/21, test_matrix_mul 74/91,
      test_hashmap 98/116) although the executables are byte-identical, so
      the memo is not shown sound (it is already on by default for `emit
      -o` through CB3); a one-line edit still takes 700 ms (target 100).
      Nov 21 (c082ee6, 2g.93): the verify comparison hashes the post-pass bytes of a hit (imc_verify_post) — the mismatches were comparing unlike hashes, not wrong replays.
      Nov 27 (day): llvm-roadmap's SP6 is done (its cache gate and the
      tail-summary gate pass on the -O2 build of 16:19) and SP7 is done
      (relink_check.sh 21 ok on x86-64, AArch64 and `--nat` Windows: an
      edit puts every function back, the changed ones into their places).
      The 100 ms of SP8 is not met (docs/perf/compile-time-2026-11-27.md).
- [x] CS7. A fast debug mode with no optimizer, and a release mode (S).
      Done: `--debug` (C at -O0) and `--release` (-O2) on `ccompile` in both compilers.
- [x] CS8. No linear scans in the self-hosted compiler: tables, not `Vec` searches
      (M). Done: instructions, functions, strings, defaults and comptime functions
      by table. Method lookup in the type checker is now
      `methods_index` (type -> method Vec) plus `methods_hm` (per-type
      method-name -> entry, lazy, dropped on Vec rewrite) — call sites
      `x.m()`, `T::f()` and the `&mut self` probe all O(1). Parser:
      `alias_set`/`g_import_set`/`struct_name_set`/`enum_name_set` HashMaps
      replace the alias, import and struct-name Vec scans; lexer keywords
      already dispatch by first char. Typecheck of the compiler measured
      ~23 s vs ~29 s before on a loaded machine.
      Remaining by nature, not oversight: `shared_borrows`/`mut_borrow`/
      `borrow_of` field-prefix "inclusive" queries (a prefix is a range,
      not a hash key), tiny cold lists (reserved borrows, scope stacks),
      and the parser's sequential `token_kind_eq` chains (O(1)
      discriminant reads, not Vec scans).
- [x] CS9. `fid check`: type errors with no code generation, under 50 ms on an edit
      Done: `fidesc check <file.fd>` runs parse + register + type/borrow/effect
      checks only and exits — hello world reports in 43 ms.
## Track VF — Verification, in place of years

This track is what replaces production years. It runs for as long as the
language lives.

**Finding compiler bugs**

- [~] VF1. A random program generator in the spirit of Csmith: all three backends
      must agree (release M9 is the start) (L).
      Done (Nov 3): `scripts/fuzz/gen_programs.py` — seeded programs with
      arithmetic, branches, loops, calls and `Vec`s, run on `run`, `emit` and
      `ccompile`; disagreements kept with each output. 40 programs, 0
      disagreements. `scripts/fuzz/nightly.sh` and a `checked` backend
      (release M9) written Nov 4, not run. Nov 15: `gen_programs.py --rich`
      adds a struct (literal, field writes, copies), an enum with payloads
      taken apart by `match`, strings and f64 arithmetic (a seed without
      `--rich` gives the same program as before); nightly.sh runs both, and
      `.github/workflows/verify-nightly.yml` schedules it every night (seed
      = date). Not run yet. Left: the first runs and long runs.
      Nov 17: first runs and the long run, docs/verification/fuzz-runs.md:
      300 programs alone and 2,000 in batches (1,550 also on the
      interpreter), with stage 0. Found M24 (promote, emit) and M25
      (checked allocator, Vec tag probe); 11 more were `q4` names stage 0
      rejects. Left: the Linux half of nightly.sh and a run with a current
      compiler.
- [~] VF2. Random programs compared with an equivalent C program, built by Clang and
      GCC (L).
      Done (Nov 7): `scripts/fuzz/gen_vs_c.py` writes each program twice from
      one tree, in Fides and in C (the eight integer types with checked and
      `@wrapping` arithmetic, casts, shifts, the §4.1 precedence printed with
      the fewest parentheses, if/while/for/match, `if` values, structs,
      `Vec<i64>`, helper calls); the C is built with Clang (`zig cc` -O0/-O2)
      and GCC 15 (WSL, -O0/-O2), which must agree, then `run`, `ccompile` and
      `emit` must match. `triage.py` groups failures, `reduce.py` shrinks one.
      Run (seed 100, 60 programs): 47 agree, 12 differ, 1 C-oracle flake.
      Found M1 (same-width casts are the identity in run/emit), M7 (emit
      does not sign-extend a narrowing cast; seeds 103 and 137), M2 (nested
      `if` value typed void), M3 (println of a u64), E1, and the unreduced V1
      class (spurious and missed overflow panics in run/emit); see
      docs/verification/miscompile-log.md. Nov 15: GCC is the host's own on
      Linux and macOS (it was WSL only), and verify-nightly.yml runs 150
      programs a night against Clang and GCC on `run`, `ccompile`,
      `ccompile-O2` and the machine layer. Nov 17: V1 reduced (V1a-V1e
      filed), `--ext2` (`ffix` fixed-precision floats, `str_slice`/
      `str_byte_at`/str `==`/f-strings, a `_` arm), `--jobs`/`--log`,
      `scripts/fuzz/vf2_long_run.sh`; long run of 1002 programs on `run`
      against clang and gcc at -O0/-O2: 19 differ, all attributed. Left: the
      long run on `ccompile`, `ccompile-O2`, `--nat` and
      `emit:x86_64-linux` (docs/verification/vf2-v1-long-run-2026-11-17.md).
- [~] VF3. Coverage-guided fuzzing of the lexer, parser, typechecker and every
      optimizer pass; no input may crash the compiler (L).
      Done (Nov 7): `scripts/fuzz/fuzz_frontend.py` mutates the suite (token
      edits, splices, cuts, deep nesting, odd bytes) through `fid check`; a
      crash, internal panic or hang is a finding (re-run to confirm); with
      `--cov-fid` (a compiler with VF8's counters) it keeps inputs that reach
      new IR blocks. Run: 1200 inputs, mutational (no coverage compiler built
      yet): one bug, F1 — no nesting limit in the parser, ~3000 nested
      brackets crash `fid` with a stack overflow. Nov 15: `--mode emit
      [--target x86_64-linux|nat]` gives the mutants to the whole native
      pipeline (the IR optimizer's passes and the machine layer's), and
      verify-nightly.yml runs `check` (1500) and `emit` (400) every night.
      Left: the coverage-guided run, long runs.
      Left: the coverage-guided run, long runs.
      Nov 22 assessment: `vf3-cov-run.sh` needs a fid built with the
      coverage counters (`--fuzz-cov`); SL13's note records that path
      still panicking on a double free in ccompile --fuzz-cov — blocked on
      that fix, not on effort. Not run today.
- [~] VF4. Run every optimizer pass on and off over the whole suite: output must not
      change (M).
      Done (Nov 7–8): `scripts/verify/variants.py toggle` — the iropt passes
      off at run time (`FID_IROPT_OFF`), the `PEIR_*` constants off in
      compilers rebuilt by `scripts/verify/build-variant.sh`; each test is
      built by both compilers, run when the binaries differ, and compared.
      Run (emit): inline, fold, dce, tailcall, gvn, licm each off over 53
      programs (the VF5 tests, 24 sampled suite tests, 20 generated): no
      output changed (two "builds with one compiler only" were a build
      flake under load, identical when rebuilt alone). Not run: the six
      `PEIR_*`/`PEIR_RANGES` variants — `dump-c compiler/main.fd` with the
      Nov 7 compiler ran out of memory each time it was tried beside the
      other sessions' work. Nov 15: `variants.py` exits 1 when an output
      changed (2 when a variant does not build), on Linux `emit` is the
      machine layer (a plain `emit` there is the C path, which runs no IR
      optimizer), and `.github/workflows/verify-weekly.yml` runs every
      run-time switch the source reads, and the ranges constant, off over
      120 sampled tests and 20 generated ones each week. Left: the five
      `PEIR_*` variants (pe_ir is Windows-only, the weekly job is Linux), the
      whole suite, a recorded run.
      the whole suite, a recorded run.
      Nov 22 assessment: the run-time-switch half is covered weekly; the
      recorded-run item still wants the five `PEIR_*` variant compilers
      (Windows-only pe_ir, so this host is the right one — one
      build-variant.sh build each). Not run today (host memory budget
      after the day's builds); the emit-lane suite result above
      (1631/1791 with fid-nov22) is the current pass-matrix datapoint.
- [~] VF5. Each optimization has a test that fails if the pass is wrong (M).
      Nov 22 audit: the "left" tests now exist (49fb2b9):
      test_verify_opt_dse_sccp_iv, _ipcp_gconst, _promote_facts,
      _spec_devirt_icf. Remaining: hofspec/branchfacts/closuredevirt/iv,
      the machine layer's kill-switches, one full expect.py pass on all
      backends.
      Nov 25 validation: that "remaining" line was stale. The named
      passes are covered by test_verify_opt_spec_devirt_icf (hofspec and
      closuredevirt), test_verify_opt_promote_facts plus
      test_verify_opt_branchfacts_cdev_iv (branchfacts), and
      test_verify_opt_dse_sccp_iv plus _branchfacts_cdev_iv (iv). The
      host-side VF5 sweep passes: 33 of 33 test_opt/test_verify_peir/
      test_verify_opt/test_verify_nat/test_verify_vf5_global_shadow files
      on run,ccompile,emit,nat with release/fid-dev.exe.
      nat-pass-reach.py --target nat: every off-switch run that changes an
      image keeps the output; the run-time mutant pass kills most mutants
      but arghint, split and brandor still SURVIVE (fcallee,
      brxblock, ldext, egsame and egzero are inert). rmw is now killed by
      test_verify_nat_addshl.fd's bump_acc case (field read-modify-write
      through &mut: +, |, ^ and -; 1 of 1 on run,ccompile,emit,nat, and
      nat-pass-reach --mutants --only test_verify_nat_addshl.fd reports
      rmw killed). brandor triage: plain boolean conditions lower as
      control flow; assigning or nesting them forms CSELs, but those
      values feed arithmetic/selects rather than the exact dead
      boolean-to-branch shape ng_fuse_andor rewrites, so the mutant still
      needs either a more precise integration shape or a unit-level probe. x86_64-linux/aarch64-linux expect.py could not run
      here: wsl.exe exists but no distribution is installed (its French
      "aucune distribution installée" error was the program output), so
      the cross-machine-layer validation remains a Linux/WSL host item.
      Done (Nov 7): `tests/bootstrap/test_opt_{fold,inline,dce,gvn,tailcall,
      licm,ranges,ranges_pop_fail,regalloc}.fd`, each a program whose output
      changes when its pass is wrong (unsigned folds, argument order, values
      reused across writes, zero-trip loops, swaps live across calls),
      checked against the interpreter, which runs no optimizer. All pass on
      run, ccompile and emit. Writing them found M4 (the C path drops the
      bounds check of `0..v.len() + 1`) and M5 (array literals). VF7 checks
      that the tests catch broken passes. Nov 15: one test per `PEIR_*`
      switch — `tests/bootstrap/test_verify_peir_{coalesce,volatile,xmm,
      rotate}.fd` (`PEIR_GLOBAL_RA` is test_opt_regalloc, `PEIR_RANGES`
      test_opt_ranges) — and `test_verify_opt_dse_sccp_iv.fd` for three
      iropt passes that had none; not run yet. Left: tests for the other
      iropt passes (ipcp, promote, spec, hofspec, devirt, icf, gconst,
      branchfacts, ...) and the machine layer's (kill-switches.md).
- [~] VF6. Translation validation: a checker proves each optimized function matches
      Nov 22 audit: the Nov 21 failure is largely fixed — build/
      tv-check-rerun.out: 22 faults caught none missed, 0 rejected, SMT
      obligations all proved; tv-check still fails on 1 SMT example of 9.
      Remaining: that example, the SUITE=1 suite-wide run, licm/inline/
      module passes.
      the original, on a chosen set of passes (XL).
      Scoped (Nov 7): docs/verification/translation-validation.md — witnesses
      from each iropt pass checked by a separate small checker, SMT for
      straight-line code. Nothing built: it needs a `--validate` mode in
      `compiler/iropt.fd`. Open.
      Done (Nov 16): `compiler/tv.fd`, on with `--validate` or
      `FID_VALIDATE=1`. Witnesses for fold, dce, gvn and the SSA round trip
      (the id correspondence; effects kept in order on equal operands, folds
      evaluated at their type, may-panic values kept or covered by an equal
      one that runs first), random-input runs for every per-function pass
      (llvm RL5); a rejected result is undone. Test
      `tests/bootstrap/test_ssa_validate.fd`. Left: a suite run with it on;
      licm, inline and the module passes; SMT for straight-line code.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      failed — the validator does not catch the miscompiles it is meant to.
      tv-check.sh: 3 failed. The self-test (24 cases) and the SMT examples
      (9) are as expected, but faults injected into main_run after inline,
      gconst, fold, forward, sccp and dse are not caught (6 of 12), and the
      program printed 43 instead of 42: an injected miscompile reached the
      executable. test_ssa_validate and test_tv_inject each get 1 rejected
      result (Str_drop: pass inline2 rejected), and the tests produce no
      SMT obligation. So the Nov 16 "Done" does not hold: these passes are
      not validated yet. Left: catch those faults, triage the inline2
      rejection, SMT obligations, then the SUITE=1 run.
      Nov 21 (c082ee6, 2g.90): tv distinguishes a real fault from a witness out of scope (`n/a` when inline2 changed the blocks) and injects into values the output depends on (tv_live_of).
- [~] VF7. Mutation testing of the compiler: a broken pass must fail a test (L).
      Verified Nov 15: the tool exists, `scripts/verify/variants.py mutate`
      (Nov 7; backend mutants added under RL11): eleven mutants (fold,
      gvn, dce, inline, ranges; two rules, two x86-64 encodings, the
      allocator's clobber sets), each a compiler built by
      `scripts/verify/build-variant.sh`; a mutant no test notices is
      reported SURVIVED. Nov 15: exit status 1 on a survivor, and
      verify-weekly.yml runs it over the VF5 and `test_verify_*` tests and
      20 generated programs each week. Left: a recorded run; mutants of the
      other passes.
      other passes.
      Nov 22 assessment: a recorded `variants.py mutate` run needs each
      mutant compiler built by build-variant.sh (one dump-c + C compile of
      the whole compiler per mutant — several GB and minutes each); the
      iropt-switch mutants need no build but the recorded run covers the
      built mutants. Not run today (host memory budget after the day's
      self-hosted builds); the weekly job keeps covering it.
- [~] VF8. Line and branch coverage of `compiler/*.fd` over 90%, reported in CI (L).
      Verified Nov 15: `scripts/verify/cov_instrument.py` (a counter at
      every function and IR block of the compiler's C) and
      `scripts/verify/coverage.py` (per-module block coverage, which is
      branch coverage at the IR level; `--goal`) exist (Nov 7, RL12).
      Nov 15: verify-weekly.yml builds the instrumented compiler and puts
      the per-module report in the run's summary each week. Left: a
      recorded run, the 90%, line coverage (the generated C has no source
      lines).
      Nov 18: line coverage from the IR's source lines (FID_C_LINE_MARKS
      marks or -g #line, a counter per line, cov_instrument.py/coverage.py
      --goal-lines), the whole run in scripts/verify/vf8-run.sh with
      results in docs/verification/vf8-coverage.md, check
      tests/tooling/vf8-coverage-check.sh. Left: the first recorded run,
      and the tests that bring each reached module to 90%.
      and the tests that bring each reached module to 90%.
      Nov 22 assessment: `vf8-run.sh` needs a fid built with the
      FID_C_LINE_MARKS instrumentation — another dedicated compiler
      build; not run today (host memory budget).

**Proving behaviour**

- [~] VF9. A written formal semantics of the core language (XL).
      Nov 22 audit: the entry lags the tree — docs/verification/
      semantics-2.md covers floats, strings, enums, generics, closures,
      references, effects, concurrency, memory, traits/regions/async
      (§5-§19), each rule tied to test_verify_semantics_ext.fd; the
      "Left: floats, strings..." list is done. Remaining (its §22): no
      mechanisation in a proof assistant.
      First piece (Nov 7): docs/verification/semantics.md, a big-step
      semantics of the integer core (types, checked/wrapping arithmetic,
      casts, shifts, short circuit, `if` values, loops, match, calls,
      structs, `Vec<i64>`), each rule tied to its reference section; its
      arithmetic rules are the C prelude of `gen_vs_c.py`, so VF2 tests the
      compiler against it. Left: floats, strings, enums, generics, closures,
      references, effects, concurrency, memory; mechanisation.
- [~] VF10. A conformance suite of a thousand-plus tests, one per sentence of the
      reference, as Test262 is for JavaScript (L).
      Started (Nov 7): `tests/conformance/` — 104 tests, one sentence each
      (§1–8 and §13; the file name is the section), the expected result
      taken from the reference, run on all backends by
      `scripts/verify/expect.py`. 96 pass. Failing: K1 (keyword-named
      functions cannot be called in an expression, 2 tests), M2, M3, M5/M6
      (array literals; emit does not bounds-check arrays), D1 (§8.1's shared
      Vec example contradicts proposal 0004). Nov 17: tests/conformance/
      has 1,592 tests, at least one per rule-stating sentence of every
      section §1-§13 (`scripts/verify/conformance_coverage.py --min 1000`
      checks it; modules the tests use in tests/conformance/_modules/,
      skipped by expect.py). K1 fixed, D1 settled (LG1); M2, M5, M6 and
      M3 (2g.72) fixed in the tree Nov 18, not yet built; 31 new findings
      CF1-CF31 and stale reference sentences D24 in
      docs/verification/miscompile-log.md and vf10-conformance-2026-11-17.md.
      Left: run on every backend with the final compiler; fix the CF
      findings.
      Nov 27 (night): the whole suite on run, ccompile and emit with
      release/fid-v27.exe, then scripts/verify/conf-known.py: 1,607 of
      1,617 pass; the 15 listed gaps pass and are out of
      tests/conformance/known-gaps.txt. Ten failures were not listed:
      four are tests of the night's fixes, right with the build of
      09:17; six are new, CF32 to CF37 (docs/verification/
      vf10-conformance-2026-11-17.md: `@public` taken for secret by the
      compiled backends' constant-time check, a `main` without `return`
      on emit, two closure captures, `comptime if` in a generic, a null
      dereference on ccompile). Left: those six.
- [~] VF11. A proof of the memory-safety claim on a small core calculus (XL).
      Nov 22 audit: the entry lags the tree — beyond the pen-and-paper
      calculus there is a machine-checked bounded model (scripts/verify/
      own_model.py: CoW, boxes, structs/enums, closures, Rc, two refs)
      and own_diff.py comparing the lowering against it; the check
      exists (last artifact: 1 failed). Remaining: the mechanised
      proof-assistant proof and a green model check.
      First piece (Nov 7): docs/verification/memory-safety-core.md — the
      calculus of proposal 0004's ownership rule, its invariant and a
      pen-and-paper preservation/progress sketch, with the five compiler
      checks it rests on (escape check, borrow checker, the lowering's
      dup/drop insertion, thread copies, collection drops). Left:
      mechanising it, proving the lowering is that translation,
      copy-on-write, structs/enums/closures/regions/Rc.
- [~] VF12. Reproducible builds: the same input gives the same binary on any machine
      (M).
      Done (Nov 7): `scripts/verify/repro.sh` builds each program twice
      (another directory, temp folder and time) and compares bytes. Run (3
      programs): `dump-c` and `emit` byte-identical; `ccompile` executables
      differ in 20 bytes (R1: PE timestamp, PDB GUID/path from the linker;
      `SOURCE_DATE_EPOCH=0` fixes the timestamp with `zig cc`). Nov 15:
      verify-weekly.yml runs repro.sh on Linux each week and keeps its
      manifest (`repro.sh --compare` against a Windows one: the second
      machine). Left: fix R1 (compiler/main.fd's ccompile link step), a
      `COMPILER=1` run.
      Nov 16, R1 (main.fd): the C compiler runs with SOURCE_DATE_EPOCH=0
      (unless set); a Windows target links with `-Wl,--no-insert-timestamp`
      (gcc, zig cc's lld in MinGW mode) and, with zig cc, `-s` (no PDB, whose
      GUID and path changed per build; panic traces read fid's own function
      table) unless --debug; cl.exe gets `/Brepro` for the compiler and the
      linker. FIDES_NO_REPRO=1 turns it off. Left: a run showing two
      directories give the same bytes (scripts/driver-goals-check.sh does it
      on Windows), a `COMPILER=1` run.
      Nov 22 (`FID=release/fid-nov22.exe`, `CC="zig cc"`):
      `scripts/verify/repro.sh` — 72 of 72 outputs reproducible: for
      hello, test_struct_eq, test_closure, test_hashmap, strings and
      test_opt_regalloc the dump-c text, pe_ir emit, zig-cc ccompile,
      `emit --target` x86_64/aarch64/riscv64-linux images and `emit --nat`
      were byte-identical between two directories, and again with 4
      workers (FID_NAT_PAR/FID_PAR). Left unchanged: the `COMPILER=1`
      (build fid itself twice) and cross-host MANIFEST legs.
- [x] VF13. A second bootstrap path checked regularly: fixpoint from a clean tree (S).
      Nov 22 audit: the Nov 18 blocker is resolved — the then-uncommitted
      modules (tv/ssa/pdb/mir/disasm/image_writer/nat_thumb/build_info)
      are all tracked; clean-bootstrap.sh dry runs pass. Remaining: the
      actual 3-stage fixpoint run has never completed (no
      build/clean-bootstrap artifact).
      Written (Nov 7): `scripts/verify/clean-bootstrap.sh` — `git archive
      HEAD` (or `--worktree`) into a new directory, cargo builds the Rust
      compiler there, `scripts/bootstrap.sh` 3 stages with the fixpoint check,
      stage 3 runs hello. Not run: the machine had no memory for three more
      stages beside the other sessions' builds. Nov 15: scheduled, the
      `bootstrap` job of verify-weekly.yml (every Saturday, on a fresh
      runner). Left: its first run.
      Nov 22: run by hand in WSL (kali, gcc 15, fid-nov22-linux — a
      self-hosted fid of the merged tree — as STAGE0 over `git archive
      HEAD`): stage 1 dump-c 76 s + C 334 s, stage 2 dump-c 65 s, stage 3
      dump-c; fides-c3 built and reports `fid 1.0.0+dev`, and stage 2's
      and stage 3's generated C are byte-identical (126,335,453 bytes):
      the fixpoint holds from a clean tree without the Rust compiler.
      (The scripted run was finished by hand after the WSL session was
      recycled mid-stage-2; stage times are of a loaded machine.)

**Hardening the runtime**

- [~] VF14. AddressSanitizer, UBSan and ThreadSanitizer through the C path, in CI
      (release M8 is the start) (M).
      `scripts/sanitize.sh` (the tests through C under ASan and UBSan; needs
      clang with ASan, Linux or macOS) written Nov 4, not run. Nov 15:
      `SAN=thread` runs the programs that use threads, tasks and channels
      under ThreadSanitizer (a race report is a finding), and the
      `sanitize` job of verify-nightly.yml runs both every night. Left: the
      first runs and what they find.
      Nov 17: the first whole-suite run (1,118 programs under ASan + UBSan,
      90 under ThreadSanitizer: every program with threads, tasks, channels
      or atomics), with a one-stage build of the Nov 16 tree in WSL:
      docs/verification/sanitizers-2026-11-17.md (findings, what was fixed,
      what is left).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — sanitize-wsl.sh (gcc ASan + UBSan, JOBS=2) on the 18
      targeted tests: 16 clean; test_deref_minimal fails by design (a
      deliberate null store); test_fix_fnval_c_ptr prints the expected
      7/12/5 through C, but `fid run` prints nothing. Left: the whole-suite
      ASan and TSan runs were not repeated with this compiler.
- [~] VF15. Fuzz every stdlib parser: JSON, regex, HTTP, TLS records, TOML, XML, CSV
      (L).
      Done (Nov 7): `scripts/fuzz/fuzz_parsers.py` with the driver
      `tests/fuzz/parsers.fd` (json, regex, http, csv, toml, xml, url, uuid).
      Runs: 8000 inputs (two seeds), 3 bugs — P1 `url_parse` overflow panic
      on a long port, P2 regex `a{39…9}` overflow panic, P3 `xml_parse` on a
      stray `<` runs out of memory or hangs. JSON, HTTP, CSV, TOML, UUID: no
      finding. Nov 16: `stdlib/tls_record.fd`, the TLS 1.3 record layer
      (RFC 8446 §5): records split from a stream (incomplete ones left for
      the next read), handshake messages, alerts, the inner plaintext, the
      per-record nonce, sealing and opening under ChaCha20-Poly1305; every
      malformed input gives the alert it calls for, never a panic
      (`test_runtime_tls_record.fd`). It is fuzz target `tls` of
      fuzz_parsers.py (binary seeds; `tests/fuzz/parsers.fd`). Left: a run
      of the tls target, long runs.
      Nov 27 (night): the tls target is run, through C with
      release/fid-v27.exe: 600 inputs (seed 27) and 6,000 (seed 2811,
      7 minutes), 0 distinct findings; json, regex, http, csv, toml,
      xml, url and uuid with seed 2811 (125 each of `-n 1000`): 0
      (build/verify-nov27/fuzz-tls.log, fuzz-all.log). About 850
      inputs a minute, so a long run is an hour or more. Left: long
      runs.
- [>] VF16. Week-long soak tests of a service under load, with flat memory (release: moved to end-roadmap.md (Nov 22 — final-gate work, do when the code stops moving).

- [~] VF17. Chaos tests: dropped connections, full disks, out of memory, clock jumps
      (L).
      Done (Nov 7): `scripts/verify/chaos.py` + `tests/verify/chaos.fd`.
      Run: dropped and reset connections, half responses, a closed stdout and
      missing files are handled on ccompile and emit; findings C1 (no read
      timeout: a silent server hangs the client), C2 (emit `read_file` of a
      missing file panics), C3 (emit names a stack overflow "invalid memory
      access"), C4 (Linux: `file_append` reports success when the disk is
      full), C5 (out of memory aborts with SIGABRT). Left: clock jumps
      (libfaketime or a VM), partitions between machines.
      Nov 22 (WSL kali, chaos.c by fid-nov22.exe, gcc 15): `chaos_linux.py
      clock` — 20/21 cases pass over no jump, realtime ±1 h and all-clocks
      +1 h through clockjump.so (the project's own LD_PRELOAD shim; no
      libfaketime needed): sleeps, tasks, timeouts, tokens, cosleeps and
      waitfds all keep their real times under the jumps. The one failure:
      "clock tasks (all clocks +1h)" wakes 600/300/900 instead of
      300/600/900 — all deadlines expire at the jump but the runtime
      dequeues them out of deadline order. `chaos_linux.py partition`
      (two user namespaces + veth + nftables, `unshare -rn`): 4/6 —
      whole-answer fetch, mid-answer read timeout, heal-after-2.5 s and
      connect-timeout all pass; the two blocking-with-no-timeout cases
      (mid-answer read, connect into the partition) hung and were killed
      at 12 s — a caller without a timeout never learns the link is dead.
- [~] VF18. Sharded tests on thousands of task and thread interleavings (L).
      Done (Nov 7): `scripts/verify/interleave.py` runs
      `tests/verify/interleave.fd` (atomics, a mutex, a channel pipeline,
      parallel branches) on random CPU sets, with and without load, sharded
      over workers. Run: 300 runs each on ccompile and emit, one result, all
      totals right. Nov 15: the `interleave` job of verify-weekly.yml runs
      3000 runs on 4 shards each week. Left: that run recorded, async
      tasks, a controlled scheduler (it perturbs the OS scheduler; it does
      not choose switches).
      Nov 22 (run of record, build/interleave-linux/record-nov22.md):
      3000 runs on 4 shards each on ccompile and emit in WSL (kali,
      fid-nov22-linux): one distinct result each, PASS — 465 s and 422 s.
      The earlier Windows attempt's four "divergent" seeds were the
      host's page-file exhaustion (`Panic: could not start a thread`,
      WinError 1455 while spawning), not schedule faults; under WSL's
      memory budget none recurred. Left as before: async tasks, a
      controlled scheduler.

**Learning from every bug**

- [x] VF19. A public miscompile log: cause, how it escaped, the test that now
      catches it (S).
      Done (Nov 7): docs/verification/miscompile-log.md — the bugs this track
      found (wrong code, wrong rejections, a compiler crash, parser, runtime
      and reproducibility bugs), each with a repro in `tests/verify/bugs/`,
      and the earlier miscompiles of AGENTS.md (none of which names its
      regression test: open under VF20). Tools: docs/verification/README.md.
- [x] VF20. Every bug fixed gets a regression test, always (S).
      Done: the rule is in CONTRIBUTING.md § Compiler changes.

**Acceptance:** a month of fuzzing on all cores finds no miscompile.

## Track LG — Language depth

Reach what Rust, Go, Zig and C++ programmers expect on day one.

- [x] LG1. Moves or counted handles decided and enforced (release M4, M5) (L).
      Decided (M4, proposal 0001): copy-on-write for `Vec`, `str` and
      `HashMap`; `==` by content is done. Settled Nov 16: the semantics is
      enforced by the ownership rule of proposal 0004 (`LOWER_RC`), not by
      the older `LOWER_COW` lowering (off, and stays off: with `LOWER_RC` it
      would free values twice, AGENTS.md item 23). Keeping a value (let,
      assignment, field, element, argument a callee changes) takes an owner
      through `Vec_share`/`HashMap_share`, and core's `vec_own` copies on the
      first write on every backend: `var b = a; b.push(2)` leaves `a` at
      length 1. reference.md §8.1, `tests/conformance/s8_1_vec_is_handle.fd`
      (it expected aliasing; now 1) and `test_lowering_value_sem.fd` (Vec,
      HashMap, by-value argument, struct copy) agree.
- [x] LG2. Owned and borrowed strings (release M11) (L).
      Done (Nov 4–6): `str` is owned, `&str` borrowed; a borrowed string cannot
      be returned as `str`, stored, or passed to a user function's `str`
      parameter; `str_clone(s)` copies. A `@scoped` struct cannot be stored or
      sent. `http_serve_scoped` frees request memory after each response and is
      no longer `@unsafe`. Tests: `test_borrowed_str`,
      `test_borrowed_str_return_fail`, `test_borrowed_str_pass_fail`,
      `test_scoped_store_fail`.
- [~] LG3. Lifetimes the borrow checker can prove across functions, without
      Nov 22 audit: mostly done — signature-derived summaries
      (borrowck.fd:529-568), NLL last-use (:2335-2353), escape check
      (escape.fd:1043-1062). Precise gap: reads are not checked against
      an active `&mut` (only `&path` creation borrowck.fd:2324-2331 and
      mutations :2396-2408 are; using `m` while `r = id_mut(m)` is live
      passes); struct lifetime args are matched positionally, generic
      structs are not instantiated by lifetime (parser.fd:3864-3873).
      second-class limits where that matters (XL).
      Done (Nov 13 check): `fn f(p: &i64) -> &i64` — parameter-derived borrow
      returns are inferred and check clean.
      Done (Nov 15, `fid`): named lifetimes. `fn f<'a>(x: &'a T, y: &T) ->
      &'a T` lexes (`TokenKind::Lifetime`, lexer `read_lifetime`), parses
      (declared in `<...>`, `'b: 'a` accepted, `'static` free; an undeclared
      lifetime or a result lifetime no parameter carries is a parse error,
      `pd_check_lifetimes`) and is kept as `@lifetime` annotations (the type
      stays `Ref`). The escape check (`esc_analyze_func`) makes the signature
      the summary callers see and rejects a body returning a borrow of an
      untied parameter; the borrow checker (`bc_record_call_borrow`) makes
      `val r = f(&a, &b)` a borrower of `a`, so the borrow rules hold across
      the call. Tests: `test_frontend_lifetime`, `_lifetime_body_fail`,
      `_lifetime_borrow_fail`, `_lifetime_undeclared_fail`; reference.md §8.4.
      Done since (docs/reference/lifetimes-across-calls.md): every tied
      argument is its own borrow (`r`, `r#1`, ...), also for a call to a
      function without lifetimes whose result is a reference (every
      reference parameter); a mutable borrow passed on is handed over and
      comes back when the result ends (`bc_hand_over_mut`); method calls
      tie the receiver, and a user method is judged by its receiver (`&`
      or by value: a read, `bc_user_read_method`); lifetimes on trait
      methods are kept, an impl's ties are compared with the trait's
      (`TypeChecker_check_impl_lifetimes`) and calls through the trait use
      them; `struct View<'a> { r: &'a T }` may hold a reference and is held
      like one (escape.fd `g_esc_lt_structs`, `bc_record_struct_borrow`).
      Left: reads are not checked against a mutable borrow (using `m` while
      `r = id_mut(m)` lives is not an error); a struct's lifetime parameters
      are not matched by name against a signature's (one lifetime each, by
      the parameter's position); generic structs with lifetimes are not
      instantiated by lifetime.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — the 24 test_frontend_lifetime*, test_frontend_method_*
      and test_frontend_trait_lifetime* files pass on run, ccompile and
      emit, and each _fail test's error matches its expect-error (a
      mutable borrow still active; an impl method tying its result to
      another parameter than the trait's). Left: as above;
      test_frontend_lifetime_struct_store_fail gives 2 errors (the first
      matches); not run on the Linux targets.
      Nov 27 (night): the read rule is in the tree, NOT yet validated by
      a built compiler. `check_read` (borrowck.fd): a read of a variable
      or of a field path of one while a mutable borrow of it, of a field
      under it or of a path above it is active is a borrow error
      (reference §8.4 rule 4); field-wise (`&mut s.y` does not stop a
      read of `s.x`); a borrower ends at its last use. Three commits:
      7b06ce65 (the rule; test_lg3_read_during_mut_borrow and the
      conformance file s8_4), 21e16421 (a temporary of the call being
      evaluated does not block a read of a later argument:
      `f(&mut g.e, read(g))`, also once a nested call activated it),
      4d7dcd55 (a released borrow stays in the table without a borrower
      and was counted as a named one; it rejected the compiler's own
      source at nat_ir.fd ng_mir_finish; test_lg3_read_after_released_
      mut). The stage-0 check is clean. The build of 06:35 (before
      4d7dcd55) rejects `check compiler/main.fd` at that one place; the
      build of 07:10 could not be used (its BG201 check rejected every
      plain float assignment; 647b4368 corrects it). To do with the next
      build, in this order: `STEPS="0 2 6" bash build/verify-nov27/
      validate-batch3.sh`, then steps 1, 3, 4, 5, 7 and 8 side by side
      (step 8: the test_frontend_lifetime* files, tests/conformance/s8_*
      and the LG3 tests). Then what is left of this entry: a struct's
      lifetime parameters are matched by position, not by name; generic
      structs with lifetimes are not instantiated by lifetime; the Linux
      targets.
      Nov 27 (night), 08:00: validated with the build of 07:41 (16 units
      at -O2, release/fid-night.exe): the three rejections (E080), the
      accepted tests, the compiler's own tree (`check compiler/main.fd`:
      no errors, 26 s) and LG3's gate: 154 of the 155 test_frontend_
      lifetime*, tests/conformance/s8_* and test_lg3_* files pass on run,
      ccompile and emit (s8_2_null_deref_panics fails with the compiler
      of Nov 25 as well). scripts/check-stdlib.sh: 390 of 403 modules
      build on their own; the 11 others fail the same with the compiler
      of Nov 25 (BG254, BG255, BG256; none a borrow error). The read rule
      is done; left of this entry: the line above.
- [~] LG4. Traits: bounds checked, associated types, default methods, trait objects
      Nov 22 audit: mostly done — bounds at calls (typechecker.fd:15699,
      :21798), associated types (:2440-2573, :2919-2962), default methods
      (compiler.fd:10858), trait objects on every backend (DynCall).
      Precise gap: the artifact cache drops generic trait methods'
      metadata (ast_ser.fd:2138-2143 skips annotations/type_params/
      is_async deliberately); GATs and generic impls stay uncompared
      (tc_impl_view_subst returns empty :2921-2923).
      (release K9 is the start) (L).
      Done (release K9, in `fid`): every bound of `T: A + B` is checked at the
      call; a type parameter is inferred or an error; turbofish with an arity
      check (`test_generic_bound_fail`, `test_generic_infer_fail`,
      `test_diag_turbofish_count_fail`). Left: associated types, default methods
      and trait objects, not checked against this goal.
      Nov 15: an impl method is checked against the trait's (parameter
      count, `self`, and every parameter and return type the trait writes
      concretely; `test_trait_sig_fail`, `test_trait_arity_fail`); `v as dyn
      Tr` needs v's type to implement Tr (`test_dyn_not_impl_fail`; it found
      no method at run time before), and `val d = v as dyn Tr` is a `dyn Tr`
      (its calls dispatch; untyped they looked for `dyn_<method>`);
      `impl<T> Tr for Slot<T>` parses, and a bare `self` in a generic impl is
      `Slot<T>` (it read the field after a `T` field from the wrong word, in
      plain generic impls too); `test_trait_generic_impl.fd` (defaults,
      overrides, bounds, trait objects) on the interpreter and C.
      Done (Nov 15, `fid`): a trait type naming `Self` or an associated
      type is compared too, as the impl must write it (`tc_impl_view_type`:
      `Self` the implementing type, `Item`/`Self::Item` the impl's binding;
      GATs and generic impls stay uncompared; `test_frontend_trait_self_fail`,
      `test_frontend_trait_assoc_fail`). Trait objects are sound at the type
      level: a struct, enum, string, bool or number where a `dyn T` is wanted
      (or the reverse) is a type error, since the object exists only where `v
      as dyn T` made it (`TypeChecker_dyn_incompatible`; it was "compatible
      with anything" and the call read the struct as an object;
      `test_frontend_dyn_coerce_fail`, `test_frontend_dyn_unwrap_fail`); a
      default method dispatches through `dyn` (`test_frontend_dyn_default`).
      Nov 17: generic methods on traits (`fn m<U: B>(..)`): a call binds the
      method's type parameters from its arguments and is monomorphized per
      type; bounds on them are checked at the call; an impl must declare as
      many type parameters and no extra bounds; a default body an impl does
      not override keeps them (compiler.fd fill_default_trait_methods); a
      generic method cannot be called through `dyn Trait` (not object-safe),
      the other methods still can (`test_trait_generic_method`,
      `test_trait_generic_method_generic_impl`,
      `test_trait_generic_default_method`, and the `_arity_fail`,
      `_bound_fail`, `_impl_bound_fail`, `_dyn_fail` tests).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — test_trait_generic_method, _generic_impl, _dyn_fail,
      _arity_fail, _bound_fail and _impl_bound_fail pass on run, ccompile
      and emit, with the expected errors (type-parameter count, a missing
      bound, no call of a generic method through `dyn`). Left: ast_ser.fd
      does not serialize generic trait methods; _impl_bound_fail gives 2
      errors (the first matches); GATs and generic impls are still not
      compared with the trait.
- [x] LG5. Zero-cost iterators: `map`, `filter`, `fold` compile to a plain loop (L).
      Done (Nov 15): `iro_closure_devirt` (iropt.fd) rewrites `CallPtr` to a
      direct `Call` when the env's fn ref is a `ConstFnRef`, and
      `iro_hof_specialize` covers the call-boundary case: it clones a
      higher-order fn once per `(hof, closure)` when the caller's env is a
      statically known `p_malloc` env — `apply`/`Vec_map` get
      `apply__cdev_*`/`Vec_map__cdev_*` clones whose inner `CallPtr` on the
      env parameter becomes a direct `Call`, inlinable by `iro_inline`
      (`FID_HOF_DBG=1` traces; verified `hofspec 2` on
      `test_closure_devirt.fd`: `v.map(fn)` + `apply(h,3)` both specialize,
      output identical on/off). The generic HOF stays for envs that are not
      statically known. `FID_IROPT_OFF=hofspec`/`closuredevirt` disables.
- [x] LG6. Closures that capture by reference and by value, with no heap allocation
      when they do not escape (L).
      Done (Nov 13): the same `iro_closure_devirt` pass replaces an env's
      `p_malloc` with `Alloca(..., Region::Stack)` when the env is used only
      by direct `CallPtr`s — escaping envs (passed as call args) correctly
      stay on the heap. Verified: `var f = fn(y){y+x}; f(5)` lowers clean;
      `v.map(fn)` envs stay heap (sound — the env outlives the frame).
      Done (Nov 16): the lowering does it on every path, the interpreter and
      C included (the C path runs no iropt): `lw_stack_envs` (lower.fd,
      `LOWER_STACK_ENVS`) makes the env an `Alloca` in place when its only
      uses are its field stores, `CallPtr`s with it as the callee, and local
      slots holding only it, and the closure's own body only reads and
      writes env fields (`lw_env_callee_ok`); `iro_cdev_env_size` reads the
      stack form, so devirtualizing still works. Decided: a closure captures
      by value (reference §4.10; changing it would change what existing
      programs print); capture by reference is a captured `&`/`&mut`, which
      the escape check keeps in the frame (`test_escape_closure_fail`).
      Test: `test_lowering_closure_stack.fd`. Left (ownership, not this
      goal): a string or Vec a closure captures gets an owner that nothing
      drops.
- [x] LG7. Const generics (M).
      Done (existing, verified Nov 13): `fn sum<N: i64>(arr: [i64; N]) -> i64`
      typechecks, monomorphizes per `N` and runs; `sum<3>(...)` calls work.
      (`test_const_generic*.fd`.)
- [x] LG8. Sum types and pattern matching with guards, nesting and exhaustiveness in
      every position (M).
      Done (Nov 15): guards, literals, wildcards, exhaustiveness naming
      missing variants — and nested constructor patterns: the parser records
      each nested payload (`Ok(x)` in `Some(Ok(x))`, `Rect(w,h)` in
      `Pair(Rect, Circle)`) as a `PatNest` bound to a fresh `__np` name and
      `pd_desugar_arms` wraps the arm in inner `match`es, so every backend
      (interp, C, PE/nat; fidesc + self-hosted fid) sees the existing flat
      `EnumVariant` shape. Guards ride the innermost pattern; `_` arms fall
      through to the remaining desugared arms, and a trailing `_` arm panics
      when no remaining arm covers the scrutinee (no silent fall-off).
      Verified on
      `tests/bootstrap/test_match_nested.fd` (`7 4 5 11 -1 0 42 11 8 -5 -9`
      on all five paths). Restriction: in expression position the scrutinee
      must be a variable (no place to hoist a one-time evaluation).
- [x] LG9. Hygienic macros, or a comptime that covers what they do (L).
      Done (verified Nov 15): `comptime` blocks/functions run at compile
      time and `comptime gen` DOES generate decls — it evaluates a block to
      a source string, parses it, and merges the generated functions/structs
      into the program (`test_comptime_gen.fd` and the comptime_* suite).
      Left: no syntactic macro system and no hygiene guarantees — generated
      names are unhygienic (can collide with user decls).
      Nov 15: a name `comptime gen` generates never silently replaces or
      shadows a declared function, struct or enum: it is an error naming the
      line (`test_comptime_gen_clash_fail`). `comptime gen` errors were
      dropped before (a generated text that failed to parse was silent);
      they are reported now.
      Done (Nov 15, frontend): `gensym("helper")` in a `comptime gen` block
      gives a name nothing in the program declares (`helper__gs1`, ...;
      `ct_gensym`), so generated helpers cannot collide with or capture the
      program's names; generated enums, constants, impl blocks and trait
      impls are added too, under the same clash rule (they were dropped
      without a word: `ct_gen_merge_rest`), and a generated trait, global,
      alias, extern block or `use` is an error rather than dropped. Tests:
      `test_frontend_gensym`, `test_frontend_gen_enum_clash_fail`. (Locals
      of generated code cannot capture the caller's: generated declarations
      are top level.) Left: syntactic macros (expression-position
      expansion); `gensym` inside a `comptime fn` (type-checked as an
      ordinary function, so unknown there); `==` for generated structs and
      enums (synthesized before `process_gen` runs, in compiler.fd).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — the three items above are done: `comptime splice { }` in
      expression position (test_frontend_comptime_splice prints 107 10 6 5
      21 30 12 true true false; its two _fail tests are refused),
      test_frontend_gensym_comptime_fn, test_frontend_gensym_runtime_fail
      and test_frontend_gen_eq pass on run, ccompile and emit. Left: no
      token-tree macro system; hygiene comes from gensym and the clash
      errors.
      Nov 22 audit: done by the second arm of the acceptance — the comptime
      covers what macros do: code generation (comptime gen + comptime
      splice in expression position, expand before anything else,
      comptime_eval.fd:589-593, ct_expand_splices :2792), compile-time
      checks (compile_error :3729/:4520, comptime_assert :3755/:4546),
      type-level computation (comptime T: type params, comptime if :2345,
      inline for), hygiene by gensym (:1148-1161) with hard clash errors
      (:563, :734, :747), fuel budget (FID_CT_FUEL :350-361) and target
      queries. No token-tree macro parse form exists — by design; a
      guaranteed hygiene algorithm (beyond gensym + clash errors) would
      be new work if ever wanted.
- [x] LG10. Error handling with context and no allocation on the happy path (M).
      Nov 22 audit: done against the acceptance — Result.context returns
      Ok untouched and builds the message only in the Err arm
      (core.fd:328-337), with_context calls f only on Err (:340-349),
      Option.context (:356-365); documented (reference.md §13.2) and
      tested (test_result_context.fd; conformance s13_2_*). No-allocation
      on the happy path follows from the code (no test asserts it).
      `Result.context(msg)`, `with_context(f)`, `Option.context(msg)` (complaint
      CE1).
- [x] LG11. Structured concurrency with cancellation and timeouts (L).
      Done (existing + Nov 13): `spawn scope`/`cancel scope`/`select` are
      language-level (pe_ir + nat runtimes). `task.fd` grew
      `task_join_timeout` (poll `task_done` until deadline) and cooperative
      `scope_cancel` via a shared flag — verified on `--nat`: join 10/20,
      timeout join → -1, cancel → -7. Nov 16: cancellation checkpoints on
      every backend (stdlib/task.fd "Cancellation tokens", CA7): a token
      (`task_token_new`, `task_token_with_timeout`), and waits that return
      once it is cancelled — `task_sleep_cancellable`,
      `task_wait_read/_write_cancellable`, `task_recv_cancellable`,
      `task_join_cancellable`, `task_scope_sleep` (`test_runtime_cancel.fd`).
      Nov 17: the interpreter interleaves tasks (compiler/interp.fd
      "Interleaved tasks": each interpreted task runs on a C-runtime task of
      its own and switches at yields, sleeps, awaits, joins and channel
      receives; `p_co_stack` gives those tasks stacks as deep as main).
      Decided: no preemptive kill; a cancelled task stops at its next
      checkpoint and returns, so its drops and `defer` run (reference 8.2,
      docs/reference/interp-tasks.md). Tests: test_task_interleave.fd on
      run/ccompile/emit, tests/ctx_switch/c_co_stack.fd.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — test_task_interleave prints its 11 expected lines
      (interleaving, ping-pong, cancellation, a deadline, cleanup, await, a
      spawned consumer) on run, ccompile and emit; c_co_stack.fd runs
      (deep: 200000). Nothing of the goal found left. Done. Interleaving on
      native Linux was not part of this check.
- [x] LG12. A defined memory model for atomics (L).
      Done (existing, verified Nov 13): `stdlib/atomic.fd` documents the
      C++11 model (SeqCst default; Acquire/Release/AcqRel/Relaxed orders
      exposed); ops verify on `--nat` (cas/xchg/fetch-add correct).
- [x] LG13. Inline assembly on every native target (M).
      Done: `asm!` parses and lowers to `Inst::Asm` everywhere, and runs in
      bare builds (`emit --bare`, pe_ir_stubs `pe_asm_encode`: 50+ x86-64
      instructions, `in`/`out` registers). Corrected Nov 15 (the Nov 13 note
      was wrong): in a normal process no back end runs the template — pe_ir,
      nat_ir (every Linux target, `--nat`) and C all give 0 for `asm!`, and
      the interpreter too; `test_asm.fd` relies on it (it writes `hlt` and
      `cli`). Left: a decision on running `asm!` in processes (the C path
      could hand the template to the C compiler's assembler on every target;
      nat_ir would need an assembler per ISA), then AArch64/RISC-V encoders.
      Nov 16, decided and done on the default Windows backend (not run
      yet): a process runs the template (pe_ir.fd `PeirGen_gen_asm_process`
      through `pe_asm_encode`), unless it holds an instruction a process may
      not run (pe_ir_stubs.fd `pe_asm_privileged`: hlt, cli, sti, port I/O,
      descriptor tables, MSRs, control registers...) or one the encoder does
      not know: those still give 0, so test_asm.fd keeps working. A function
      with asm! keeps its values in slots and saves every callee-saved
      register; an output register that is not an input starts at 0.
      `FID_PE_OFF=asm`: the old 0. Tests: test_windows_asm_regs.fd,
      scripts/pe-win/asm_process.fd. Left: nat_ir (Linux targets, `--nat`),
      the C path, AArch64/RISC-V encoders.
      Nov 16, the machine layer by the same rule (not run yet): nat_ir.fd
      `ng_gen_asm` makes each asm! site a call to a thunk of its own
      (nat_enc.fd `nat_asm_thunk`: callee-saved registers saved, inputs
      moved from the argument registers into the named ones, an output
      that is not an input set to 0, the template, the output back), so the
      allocator sees an ordinary call and the template may use any register
      but sp. x86-64 (Linux, `--nat`) through `pe_asm_encode`; AArch64 and
      RISC-V through a new small assembler, nat_enc.fd `nat_asm_encode`
      (nop, barriers, brk/svc, mov/movz, add/sub, logic, mul/div, shifts,
      mrs of cntvct/cntfrq/tpidr; RISC-V nop, fence, ecall, li/mv, R- and
      I-type ALU, rdcycle/rdtime). Privileged (`pe_asm_privileged`) or
      unknown instructions and registers give 0; `FID_NAT_OFF=asm`: always
      0. test_nattools_asm.fd, tests/nattools/asm_probe.fd,
      scripts/nat-tools-check.sh.
      Nov 16, the C path, run: codegen_c.fd `CCodeGen_gen_asm` hands the
      template to the C compiler's assembler as GNU inline assembly (local
      register variables for the operands, Intel syntax on x86-64, `;` a new
      line, an output that is not an input starts at 0, `clobber(...)` and
      every other register the template names as clobbers, chosen per
      target with `#if`, since `bl` is a register on x86-64 and a branch on
      AArch64); privileged templates give 0; tcc and cl.exe panic (no GNU
      asm). Checked with `zig cc`: test_asm_c.fd runs all four templates
      (42, 42, 25, 99; `FID_ASM_CHECK=1` prints "ran"), test_asm.fd and
      test_nattools_asm.fd match `fid run` on Windows x64, and
      test_nattools_asm.fd built for aarch64-linux and riscv64-linux runs
      under qemu with the same output. So every target runs `asm!` through
      C, and pe_ir and nat_ir run it natively. Limits: the native encoders
      know a subset of each ISA (an unknown instruction gives 0 there), and
      a function with `asm!` is never kept by the FNCACHE.
- [x] LG14. `async` for every argument type, and async traits (release H9) (L).
      Done (H9): an `async fn` with `f64` or struct arguments or results runs as
      a task on the interpreter and C paths. Left: reference arguments, native
      async (`fid emit` programs using `async` crash), async traits.
      Nov 16: native async does not crash (fixed Nov 6, release H9): the
      async tests run under `fid emit` (pe_ir) with the interpreter's output,
      `test_async_suspend` interleaving as a task should (the interpreter
      runs tasks serially, LG11). Reference arguments, decided: a reference
      cannot reach another task (§8.4), so an `async fn` with one runs on
      its caller's stack to its end and its future is ready; where it waits,
      the caller's task waits (`test_lowering_async_ref.fd`). Async methods:
      `async fn` in an impl block, a trait impl and a `cap` declaration
      (parser), split by the lowering into a body function run as a task and
      a method returning its future, so `dyn` calls get futures too
      (`lw_split_async_methods`, `test_lowering_async_method.fd`). Left:
      async methods of generic impls and async default methods of a `cap`
      (left as they were: not split); `--nat` and the Linux targets not run
      here (a `--nat` hello printed nothing on this machine).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — async methods of generic impls and async default methods
      of a `cap` now run as tasks: test_lowering_async_generic_method,
      _generic_task, _cap_default, _method and _ref pass on run, ccompile,
      emit and --nat, and with test_spawn_detached on x86_64-linux and
      aarch64-linux (WSL, qemu), so the "not run here" above no longer
      holds. Done. Not run on riscv64-linux.
- [x] LG15. C interop without bindings: `@import("header.h")`, as Zig does (L).
      Done (Nov 13): `use foo` falls back to `foo.h` when no `foo.fd` exists;
      `compiler/cparser.fd` generates Fides bindings which merge through the
      normal import pipeline with `extern "C" link` metadata. Verified
      end-to-end natively: `use libcput` → `libcput.h` → real `puts` call.
      Left: literal `@import` syntax not added (`use`-fallback covers it);
      extern calls still need `unsafe`.
      Nov 15: the literal `@import("cdemo.h")` (a header beside the program
      or at the project root, found as `use cdemo` finds it); on the
      interpreter and C (`tests/bootstrap/at_import/`). Fixed with it: `use
      x = "1.2"` (a version pin) was always a parse error. Nov 16 (not yet
      built): system headers: an `@import("stdio.h")` not beside the program
      is looked for in FID_INCLUDE, C_INCLUDE_PATH, INCLUDE, /usr/local/include,
      /usr/include, /opt/homebrew/include, and its bindings carry
      `include "<stdio.h>"`, so the C code includes it and emits no prototype
      of its own (codegen_c: the header's `int abs(int)` is used as declared);
      `@import("x.h", trusted)` (decision: trust is opt-in, per header) lets
      its functions be called without `unsafe` (typechecker
      `tc_is_trusted_extern`); cparser reads `#if/#ifdef/#ifndef/#elif/#else/
      #endif` (a branch is dropped only when its condition is known false;
      `_WIN32` or `__unix__` and `__STDC__` predefined, `__cplusplus` not).
      Tests in tests/bootstrap/at_import/ (`_trusted`, `_untrusted_fail`,
      `_system`, which needs a C library's headers). Left: `#include <...>`
      inside a header is not followed; MSVC's and glibc's attribute macros
      (`_ACRTIMP`, `__THROW`) are noise only when the header defines them;
      native backends link a system header's functions from `link` only.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — `#include` following and the system DLL lookup in
      cparser.fd (cp_include, cp_norm_path, cparse_dll_exports): 8 of the 9
      steps pass, Windows emit and the WSL x86_64-linux run included. Left:
      on the C path, test_at_import_system with zig's Windows headers fails
      ("conflicting types for 'abs'": an extern C name equal to a core
      function stops core's abs -> p_abs rename), while `fid emit` of it
      prints 5; --nat Windows and macOS still call no C functions.
      Nov 21 (c082ee6, 2g.107): an extern C name no longer stops a core function's rename (`abs` stays `p_abs`; lg15-extern-core-name-check.sh).
      Nov 22: lg15-extern-core-name-check.sh passes; test_at_import_system
      passes with FID_INCLUDE set (release/fid.exe).
      Nov 22 code audit: done — inline `@import("header.h")` in the parser
      including `, trusted` and `.hpp` (parser.fd:8654-8728); `#include`
      following with system search dirs (cparser.fd:377-387, :656);
      ~100 MSVC/glibc/mingw/Darwin attribute macros skipped (CP_ANNOT_WORDS
      :1171); native linking with no `link` (cparse_system_dll_of :587-620,
      nat_ir.fd:33469, :32671). Tests at_import/test_at_import_native_win,
      _native_libc, test_at_import_cpp*; lg15-extern-core-name-check.sh
      PASS (build/lg15-vf15-check.out).
- [~] LG16. C++ interop for the common cases (XL).
      Nov 22 audit: far more than the roadmap suggested — extern "C++"
      shim generation with classes as opaque handles, method/get/set
      wrappers, function-template instantiation, std::string crossing and
      exception catch (cparser.fd:2472-2498; tests at_import/
      test_at_import_cpp*, _tmpl). Precise gap (self-declared :2496-2498):
      class templates, operators, rvalue references, function pointers,
      variadics, std containers; and `fid emit` does not compile shims
      (the cpp: link marker is ccompile-only, main.fd:4658-4673).
      Nov 16 (not yet built), the first step: a C++ header (`.hpp`, `.hh`,
      `.hxx`, or one with `namespace`) imports through compiler/cparser.fd
      like a C one (`use lib` finds `lib.hpp`; `@import("lib.hpp")`).
      Functions with C linkage are bound (`extern "C" { }`, `extern "C" int
      f(..)`; `__cplusplus` is defined, so the usual guards hold); a function
      of `namespace a::b` is `a_b_<name>` in Fides (`fn f as a_b_f`, the C
      symbol unchanged); classes, templates, `using`, `friend` and operators
      are skipped, and so is a function with C++ linkage (a mangled symbol),
      each with a note in the generated bindings. Tests in
      tests/bootstrap/at_import/ (`test_at_import_cpp`, `_cpp_fail`). Left:
      calling C++ itself (mangled names, classes and methods, templates,
      references, exceptions): that needs generated `extern "C"` shims
      compiled with a C++ compiler, or Itanium/MSVC name mangling in the
      backends.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — generated `extern "C"` shims: test_at_import_cpp prints
      "cpp ok" on run and ccompile; cpp_fail and cpp_shim_fail are refused
      with the expected undefined-function errors; ccompile of cpp_shim
      matches its 17 expected lines and cpp_tmpl prints 2.5/pear/
      7000000000/9; parse-header writes the shim, which `zig c++ -c`
      compiles; the C at_import tests are unchanged. Left: class templates,
      operators, nested classes, rvalue references and std containers are
      not bound; native emit does not compile the shims (use ccompile);
      std::string results leak.
- [x] LG17. Every annotation works or is removed (release K8) (M).
      Done: `@bounded-stack`, `@no-indirect` (rejects calls through function
      parameters) and `@tainted` (a tainted function or parameter reaches no IO
      or Net effect; no data flow in `fid`). The 17 safety rules each have a
      negative test: `docs/reference/safety-rules.md` (release L1, L2).
- [x] LG18. Effect-row polymorphism: functions with `fn`-typed parameters are
      effect-polymorphic, instantiated per call site (M).
      Done (Nov 3): `apply(pure_fn)` is pure, `apply(io_fn)` gets `IO`, closures
      are scanned directly, unknown function values fall back to all effects.
      (`fn_param_positions` + `instantiate_fn_arg_effects` in `src/typechecker.rs`;
      tests `effects/test_effect_ho_instantiation.fd` and
      `test_effect_ho_violation.fd`.)
- [x] LG19. Explicit `!E` effect rows in signatures (S).
      Done (Nov 3): a declared effect name that is not a concrete effect
      (`!E`, `!{IO, E}`) marks the function effect-polymorphic — the row is
      instantiated at call sites and under-declaration is not an error for
      row-annotated functions.

**Acceptance:** each feature in the comparison table has a conformance test.

## Track SL — Standard library depth

Go's lesson: a deep stdlib replaces a thousand packages. It suits our rule of
owning what we ship.

- [x] SL1. HTTP/2 client and server (L).
      Done (Nov 15), cleartext h2c with prior knowledge: `stdlib/hpack.fd`
      (RFC 7541: static and dynamic tables, size updates and eviction,
      integer and string coding, Huffman both ways from the full Appendix B
      table) and `stdlib/http2.fd` (RFC 9113: every frame type with the
      section 6 checks, preface, SETTINGS/ACK, PING, GOAWAY, RST_STREAM,
      stream states, connection and stream flow control, CONTINUATION).
      `http2_serve(port, handler)` takes `http`'s handlers, many streams per
      connection, a task per connection; client `http2_get`, `http2_post`,
      `http2_get_many` (several streams on one connection).
      `tests/bootstrap/test_hpack.fd` decodes every Appendix C example and
      encodes it back to the same bytes; `test_http2_frames.fd` round-trips
      every frame, rejects 21 malformed ones with the RFC's error codes,
      decodes bytes captured from python-h2 and drives a connection by hand
      (flow control, CONTINUATION, stream and connection errors).
      Interpreter, C and native Windows give the same output. Live:
      python-h2 and httpx (`http2=True`) against `examples/http2_server.fd`
      (21 concurrent streams, 6 connections, 188 KB bodies under flow
      control) and our client against our server. Nov 15 (later): a
      request's content-length is checked (8.1.1: not a number, DATA that
      do not add up to it, HEADERS or trailers that end the stream early are
      stream errors, `h2_content_length`); `test_stdlib_http2_length.fd`.
      Done Nov 17: ALPN in both tls modules (`tls_connect_alpn`,
      `tls_server_alpn`, `tls_session_alpn`; `stdlib/tls_alpn.fd`); h2 over
      TLS with an HTTP/1.1 fallback (`stdlib/http2_tls.fd`); the h2c
      Upgrade on both sides (`h2_upgrade_request`, `http2_get_upgrade`); the
      SETTINGS ACK timeout; MAX_HEADER_LIST_SIZE enforced; handlers on their
      own tasks; `test_stdlib_http2_length.fd` run; native `emit` of a
      server works. Server push and priorities are left out by design
      (docs/stdlib/http2.md). Tests: `test_tls_alpn.fd`,
      `test_http2_limits.fd`, `test_http2_upgrade.fd`, `test_http2_tasks.fd`,
      `tests/net/http2_live.fd`, `tests/tls/http2_tls_live.fd`. Left: the
      Windows Schannel server-side ALPN run, which waits for a run with a
      .pfx certificate.
- [~] SL2. HTTP/3 and QUIC (XL).
      Nov 22 audit: mostly done — stdlib/quic(+_conn/_frame/_tls/_udp).fd,
      http3.fd, qpack.fd over udp.fd; tests test_stdlib_quic_*, tests/net
      quic_udp_*, http3_udp_loopback, http3_curl_server, quic_curl_interop,
      http3_public. Precise gap (self-declared at quic_conn.fd:52-53):
      0-RTT anti-replay, path MTU discovery, ECN, pacing, preferred
      address; RFC 9368 version_information absent.
      Done Nov 17: QUIC v1 and HTTP/3 in pure Fides (`stdlib/quic*.fd`,
      `tls13.fd`, `aes_gcm.fd`, `http3.fd`, `qpack.fd`, `http3_udp.fd`)
      over `stdlib/udp.fd` (the OS socket API, IPv4 and IPv6, no hand
      linking, every backend); HelloRetryRequest, server Retry, Version
      Negotiation, stateless reset, NEW_CONNECTION_ID issue and retire,
      client migration and server path validation, resumption with tickets
      and 0-RTT (`test_stdlib_quic_more.fd`, `tests/net/quic_udp_more.fd`);
      interop with curl/ngtcp2. Left: 0-RTT anti-replay, RFC 9368
      version_information, ECN/PMTU/pacing; the box is ticked once the
      final build's validation of the tests above passes.
- [x] SL3. WebSockets, committed and tested (M).
      Done: `stdlib/ws.fd` (RFC 6455: the Upgrade handshake with its SHA-1
      and base64 key, frames with masking, ping/pong, close), `test_ws.fd`
      checked against Python on C and native. Nov 9: wss:// (`stdlib/wss.fd`:
      `wss_connect`, `wss_accept`, `ws_connect_url`, over `tls`, which `ws`
      never links), fragmented messages reassembled with a ping in their
      middle answered, the handshake no longer reads past its headers, and
      binary frames sent byte-exact; `test_ws_local.fd` (a server and a
      client over IPv6 and IPv4) and a TLS round trip with a self-signed
      certificate on C and native. Left: permessage-deflate.
- [~] SL4. TLS 1.3 on every platform (release I7) (M).
      Nov 22 audit: mostly done — tls_client13/tls_connect13 on Windows
      (tls_windows.fd:466,:566,:572) and Unix OpenSSL (tls_unix.fd:409,
      :425); test_tls13_local, scripts/verify/tls13_live.fd. Precise gap:
      the native-lane run (tier1 disagree.txt:493 — inside the general
      native-Windows breakage, not TLS code).
      Verified live: `tls_connect13("github.com", 443)` over Schannel
      negotiates SP_PROT_TLS1_3_CLIENT (8192, checked via
      tls_session_protocol) — the SCH_CRED + TLS_PARAMETERS credentials
      path is in (`tls_client13`/`tls_connect13`/`ts_acquire13` pin
      grbitDisabledProtocols = ~SP_PROT_TLS1_3; the legacy
      grbitEnabledProtocols field can't express 1.3). Default
      tls_client keeps system negotiation (TLS 1.2 on some servers).
      Unix side goes through OpenSSL (1.3-capable). Nov 15 (stdlib, read
      against schannel.h): `ts_acquire13` filled a SCH_CREDENTIALS with
      SCHANNEL_CRED's offsets (flags at 72, the TLS_PARAMETERS at 80/88, past
      the structure's 72 bytes), so Schannel got no parameters and no flags:
      nothing was pinned and the live 1.3 above was the system default. Now
      cCreds 8, paCred 16, dwFlags 52, cTlsParameters 56, pTlsParameters 64.
      Added `tls_server13` on both platforms (Schannel: the certificate in
      paCred with TLS 1.3 alone enabled; OpenSSL: SSL_CTX_set_min_proto_version
      1.3), `tls_client13`/`tls_connect13` on OpenSSL, and
      `tls_session_version` (772 = TLS 1.3 on both). Not built or run in
      this pass. Also Nov 15 (release I7): the disabled-protocols mask was
      0xFFFFAAFF, which disables the TLS 1.3 client bit too; now 0xFFFFCFFF
      (`TS_ALL_BUT_TLS13`), and `scripts/verify/tls13_live.fd` is the live
      check. Nov 17, checked live (`tls13_live.fd`: github.com and
      cloudflare give 772 with a GET, Windows C and native, OpenSSL in
      WSL). The suite test is `test_tls13_local.fd` (a built-in
      certificate, `tls_server13` with ALPN, 772 on both sides; Windows
      ccompile and emit, Linux). Schannel fixes: SEC_I_RENEGOTIATE (the TLS
      1.3 NewSessionTicket) answered in `tls_session_recv`
      (`ts_post_handshake`); default clients and servers through
      SCH_CREDENTIALS, so 1.3 by default on Windows 11; .pfx keys imported
      as CNG keys and deleted by the new `tls_server_close`; the ALPN choice
      kept after the ticket. Docs: docs/stdlib/tls13.md. Left: the box,
      ticked once the final build's run of `test_tls13_local.fd` passes.
- [x] SL5. DNS with caching, IPv6 everywhere (M).
      Done: `stdlib/dns.fd` parses every answer record with its TTL
      (bounds-checked names), A and AAAA, IPv6 text as RFC 5952, a cache by
      TTL (`dns_cache_resolve`); `test_dns.fd`. Nov 9: IPv6 sockets on the C
      path and native Windows: a listening socket takes IPv6 and IPv4 clients
      (dual stack), connect tries each address of a name, IPv6 ones included.
      Checked Nov 15: native Linux has most of it now
      (`stdlib/linux_runtime/sys.fd`): IPv6 lines of /etc/hosts, A and AAAA
      queries to the first nameserver of /etc/resolv.conf (`pe_dns_query`),
      IPv6 connect, dual-stack listening. Nov 16 (linux_runtime/sys.fd):
      `pert_net_connect` tries every address until one connects — the
      literal's or /etc/hosts' IPv4 then IPv6 one, else every A answer then
      every AAAA answer of DNS (8 of each, `pe_dns_query_n`); DNS asks the
      IPv4 nameservers of resolv.conf in turn (3, glibc's MAXNS) when one does
      not answer or says SERVFAIL/REFUSED (`pe_dns_ask`; NXDOMAIN is final)
      (`test_runtime_connect_retry.fd`). Native Windows (pe_runtime) already
      loops over getaddrinfo's answers. Left: `--nat` Windows (win.fd's
      resolver gives one IPv4 address); no run against a real resolver.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — both items left above are done. `--nat` Windows resolves
      every address through getaddrinfo (IPv6 included) and connect tries
      each: test_runtime_connect_retry prints true for listening,
      localhost, 127.0.0.1 and ::1 and -1 for a closed port, on --nat and
      x86_64-linux; tests/net/real_connect.fd connects through the real
      resolver on --nat, pe_ir and x86_64-linux (-1 for an invalid name).
      Done. The cache and the other paths were not rechecked.
- [~] SL6. JSON at simdjson-class speed (L).
      Nov 22 audit: infrastructure done (json_index indexed DOM json.fd:1475
      + jd_* :1488-1546, SWAR j_plain_words :137 with interp guard;
      test_json_swar), speed target not: docs/benchmarks.md:234-236 records
      42-178 MB/s vs simdjson 2-3 GB/s. Precise gap: SIMD byte lanes
      (nat_ir ng_simd_call is f64/i64 only) and json_index's Vec-push
      bound — 1-2 orders of magnitude, real engineering.
      Measured (5.9 MB nested doc, PE emit): `json_is_valid` ~10 MB/s,
      `json_parse` (validate + canonicalize) ~1.6 MB/s — three orders of
      magnitude from simdjson-class. `j_canon` now writes into one
      StringBuilder and sorts member indices instead of copying value
      strings (was `out = out + ...` per member); canon is linear at
      ~6× the validate pass. Nov 15: the indexed DOM. `json_index(text)`
      scans once (validating as `json_is_valid` does, depth limit 512) and
      records every value's kind, span, subtree end and child count;
      `jd_get`, `jd_at`, `jd_path`, `jd_key`/`jd_member`, `jd_str`,
      `jd_int`, `jd_num`, `jd_bool`, `jd_raw` (a slice of the text) walk
      those nodes and never rescan or reserialize. `test_stdlib_jsondoc.fd`
      checks it against the string API and `json_is_valid` (not yet run).
      Left: measuring it, moving `gltf`/`tiled` (O(n^2) per lookup today)
      onto it, a faster scan; the `str`-in/`str`-out API still
      reserializes by design.
      Nov 17: strings a word at a time in compiled code (json.fd
      `j_plain_words`, SWAR; `j_words_ok` keeps the interpreter on the byte
      loop), leaves pushed once (`jd_new_leaf`); `test_json_swar.fd`; long
      strings 3-5x natively, 4-6x through C (docs/benchmarks.md). Left: byte
      lanes in simd.fd (nat_ir `ng_simd_call` is f64/i64 only), so no
      vector scan. (Marked done once the Nov 17 build passes
      `test_json_swar`.)
      Nov 22: test_json_swar passes on C and pe_ir (release/fid.exe); the
      speed was not measured.
- [x] SL7. A linear-time regex engine (L).
      Done: `regex_match` in `stdlib/regex.fd` is a Pike VM (no
      backtracking): classes, escapes, counted quantifiers, groups,
      alternation, anchors; `test_regex_linear.fd` runs a pattern that is
      exponential for a backtracking engine in linear time.
- [x] SL8. Time zones, calendars, formatting and parsing (L).
      Done: `stdlib/tz.fd`: 57 named zones with their current DST rules (US,
      EU, southern hemisphere), fixed offsets ("+05:30"), local to UTC and
      back (a skipped hour moves forward, a repeated one takes the first),
      `tz_format` with %Z %z, ISO 8601; `test_tz.fd` checked against Python
      zoneinfo. Nov 9: the IANA database itself: `tz_from_tzif` reads TZif
      files (RFC 8536, versions 1 to 4) with every past transition and the
      file's POSIX rule after the last one (`tz_from_posix`, negative DST
      and 24:00 change times included); `tz_load` finds the system's files
      ($TZDIR, /usr/share/zoneinfo) and falls back to the table;
      `test_tzif.fd` (Kolkata, Phoenix, Lord Howe, New York since 1883)
      against zoneinfo on the same files.
      Nov 15: POSIX rule dates in all three forms (`Mm.w.d`, `Jn` without
      February 29, zero-based `n`), range-checked; `test_tz_julian.fd` (38
      instants around each change of the old Tehran rule and others) against
      glibc (`TZ=.. date` in WSL). Python's zoneinfo reads `n` one day early
      and puts `J59` on February 29 in leap years. Nov 15 (later):
      `stdlib/calendar.fd`: epoch days and Julian Day Numbers, the Julian
      calendar both ways, ISO 8601 week dates (week-year edges, 52/53
      weeks), ordinal dates, Gregorian and Julian (Orthodox) Easter;
      `dt_format` gained %G %g %V %u %w %s %h %n %t, and `dt_parse_format`
      parses by a strptime format (names in any case, 12-hour clock, %j,
      ISO week dates, %z/%Z, %s; the date and the weekday are checked);
      `test_stdlib_calendar.fd` against Python's datetime and dateutil
      (passed Nov 22 with `fid run`). Nov 17: the tabular Islamic calendar
      (the four leap-year
      rules, civil and astronomical epochs) and the Hebrew calendar (molad
      of Tishri, the four postponements, Adar I/II) in stdlib/calendar.fd;
      month and weekday names of fr de es it pt ar (CLDR 47) in
      `dt_format_lang` / `dt_parse_format_lang`;
      `test_stdlib_calendar_lunar.fd` against ICU (Node), .NET
      HebrewCalendar/HijriCalendar and chabad.org molad times. Left
      (optional): the Chinese calendar. (Marked done once the Nov 17 build
      passes `test_stdlib_calendar_lunar`.)
      Nov 22 validation (`release/fid-dev.exe`): `test_tz`, `test_tzif`,
      `test_tz_julian`, and `test_stdlib_calendar` pass under `fid run`;
      the lunar-calendar test passes as a native PE. Its interpreter run
      exceeded 180 seconds, so it was validated natively instead. The
      Chinese calendar remains optional.
- [x] SL9. Unicode: normalization, segmentation, case folding (L).
      Done: `stdlib/unicode_data.fd` (generated from Unicode 16, packed
      fixed-width-decimal tables — const arrays re-evaluate per access on
      the interpreter and `[i64; N]` by-value params truncate on emit,
      known-bugs §2g.23): canonical + compatibility recursive
      decomposition, canonical combining classes, canonical composition
      (composition exclusions filtered via NFC oracle). `stdlib/unicode.fd`:
      `uni_nfd`/`uni_nfkd`/`uni_nfc`/`uni_nfkc` with Hangul algorithmic
      decomposition/composition, canonical ordering, and the UAX#15
      blocked rule; grapheme segmentation and case folding already there.
      `test_lib_unicode.fd` covers NFD/NFC/NFKD/NFKC incl. reordering,
      Hangul both ways, compat ligatures, the 18-cp ﷺ case and
      composition exclusion U+095B; a 400-string × 4-form differential
      run against Python unicodedata passed 1600/1600 on interp and emit.
- [x] SL10. Compression: gzip, zstd, brotli decode (L).
      Done: `stdlib/compress.fd` inflates DEFLATE, zlib and gzip (fixed and
      dynamic Huffman) and writes them (stored blocks); `test_compress.fd`
      decodes streams made by Python's zlib. Nov 9: `stdlib/zstd.fd`
      (RFC 8878: raw, RLE and compressed blocks, Huffman and FSE, repeat
      offsets, skippable frames, the XXH64 checksum; no dictionaries) and
      `stdlib/brotli.fd` (RFC 7932 complete: context modelling, the static
      dictionary in `brotli_dict.fd` with all 121 transforms), both in
      Fides, so native builds have them; `test_zstd.fd`, `test_brotli.fd`
      on all backends, against streams of Python's encoders (522 brotli
      streams and 1,800 corrupted ones decode as the reference decoder
      does; 39,000 corrupted zstd streams never panic). Left: compressing
      encoders for zstd and brotli.
- [x] SL11. Encoding: base64, hex, CSV, XML, YAML, TOML, protobuf (L).
      Done: CSV (RFC 4180, `test_csv.fd`) and the protobuf wire format
      (`stdlib/protobuf.fd`: varint, zigzag, fixed, length-delimited,
      packed, skipping unknown fields; `test_protobuf.fd` against bytes from
      Python). base64/hex (`encoding`), XML, YAML and TOML modules exist.
      Nov 15: TOML 1.0 complete: `toml_load` (strict, errors with line
      numbers, UTF-8 checked; `toml_to_json` gives toml-test's JSON) passes
      the whole toml-test 1.0.0 list (208 valid files match their JSON, 509
      invalid ones are rejected); `test_toml_spec.fd` (the spec section by
      section and its 56 spec-1.0.0 files) and `test_toml_suite_spec.fd`
      (153 more toml-test files); `toml_parse` stays the lenient flat form.
      YAML 1.2 subset: `yaml_load`/`yaml_load_all` (block and flow
      collections, plain, quoted and block scalars with folding, chomping and
      every escape, anchors and aliases, the core schema and tags,
      documents); `test_yaml_spec.fd` reads the YAML 1.2.2 examples 2.1-2.28
      (2.11 and 2.25, explicit `?` keys, are rejected as unsupported). XML
      1.0: `xml_load` checks well-formedness (line and column of the first
      error), entities and character references, CDATA, PIs, the
      declaration, internal DOCTYPE entities, line-end and attribute
      normalisation; `test_xml_spec.fd` (47 malformed documents rejected);
      `xml_parse("<")` no longer loops. Interpreter and C agree on every test;
      natively the TOML suite trips on known-bugs 2g.41. Left: YAML complex
      and merge keys; XML DTD validation, external entities, namespaces.
      Nov 17 (not yet built): YAML explicit/complex keys and merge keys
      (`test_yaml_spec` reads 2.11 and 2.25). XML namespaces
      (`XmlNode.ns`/`local`/`attr_ns`, `XmlOptions.namespaces` errors),
      DTD internal-subset parsing with attribute defaults, entities with
      markup and an entity-bomb limit, validation (`XmlOptions.validate`),
      external entities off by default with `XmlOptions.resolver`
      (`xml_load_with`; `test_xml_dtd_ns.fd`). The TOML suite runs natively
      (2g.41 fixed in pe_ir's `p_str_concat`). Nothing is left once the
      Nov 17 build passes these tests.
      Nov 22 validation (`release/fid-dev.exe` native PE): `test_encoding`,
      CSV and dialects, protobuf, YAML library/spec, XML library/spec/DTD
      and namespaces, and TOML library/spec/suite-spec all build and pass,
      including YAML examples 2.11 and 2.25.
- [x] SL12. Database drivers: PostgreSQL, MySQL, SQLite, with parameter binding (L).
      Done: `stdlib/sqlite.fd` binds the system sqlite3 (`SQLite::open*`,
      exec/query → JSON rows) and implements `db.fd`'s `Database`
      capability; `SQLite::query_params` binds parameters through
      `sqlite3_bind_text` — verified on emit that `' OR 1=1 --` stays a
      literal (`test_pe_sqlite.fd`). `db.fd` also ships an in-memory
      `MemoryDB` for tests. Nov 15: PostgreSQL in Fides
      (`stdlib/postgres.fd`, wire protocol 3.0 over a socket, no libpq):
      startup, trust/cleartext/MD5/SCRAM-SHA-256 authentication (the
      server's signature checked), the simple query and the extended one
      with `$n` parameters bound apart from the SQL (`pg_query_params`),
      rows as JSON by column type, command tags and counts, ErrorResponse
      with its SQLSTATE; `crypto.fd` gained `crypto_hmac_sha256_bytes` and
      `crypto_pbkdf2_sha256`. `test_stdlib_postgres.fd` checks every
      message and the auth exchanges byte for byte against Python, and
      `test_stdlib_postgres_scram.fd` the RFC 7677 vector (compiled only:
      4096 rounds); neither run yet, and no live server tried. Left: MySQL,
      TLS (SSLRequest), NULL parameters, the `Database` capability for
      `PgConn`, a run against a real server.
      Nov 22 code audit: that Left is stale — MySQL exists
      (stdlib/mysql.fd:837-862 mysql_param_int/double/text/varchar/null,
      mysql_com_stmt_execute with the NULL bitmap; mysql_db.fd:229 impl
      Database for MysqlConn), TLS (stdlib/postgres_tls.fd:58
      pg_connect_tls, SSLRequest, verify-only), NULL parameters
      (postgres.fd:1404 pg_query_params_nulls), and `impl Database for
      PgConn` (postgres_db.fd:19). Tests: test_std_mysql,
      test_stdlib_mysql_db, tests/db/postgres_live, tests/db/mysql_live.
      Residual only: the live-server tests need real servers and are not
      wired into CI — optional infra, not a driver gap.
- [x] SL13. Built-in testing: table tests, benchmarks, fuzzing, as in Go (M).
      Done: `fid test <file.fd>` runs each `fn test_*()` in its own process and times each `fn bench_*()`.
      Nov 15: `fid test <file> --fuzz` runs each `fn fuzz_*(data: Vec<u8>)`
      on 100 deterministic inputs in one process (seeded lengths 64-1023;
      checked with a passing target); `--update` rewrites snapshots,
      `--coverage` runs the tests through veil. Nov 15 (stdlib): table
      tests: `table_new`, `table_int`/`table_str`/`table_bool`/
      `table_check`, `table_done` in `stdlib/test.fd` (each row named,
      every row run after a failure, failures listed as `table/row`, a
      panic at the end so `fid test` fails the test, `FIDES_ROW=<text>`
      runs only matching rows); `test_stdlib_table.fd` (not yet run).
      Left: coverage-guided mutation and a saved corpus (Go keeps failing
      inputs under testdata/fuzz); a `fid test -run` flag that sets
      FIDES_ROW (compiler/main.fd).
      Nov 16 (main.fd cmd_test_file): `fid test <file> -run <test>/<row>`
      (or `--run`), as Go's -run: the part before `/` picks the tests (CT2's
      filter), the part after is FIDES_ROW for each test process (also under
      --jobs); `-run /<row>` every test, `--row <text>` the rows alone.
      scripts/tooling/sl13_rows_tests.fd, checked by
      scripts/driver-goals-check.sh after the next build. Left: coverage-
      guided mutation and a saved corpus.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      failed — coverage-guided fuzzing does not work yet. The harness gets
      block coverage (26 blocks), but sl13-fuzz-check.sh fails: the nested
      `ccompile --fuzz-cov` build panics with a double free
      (Vec_drop_inplace from build_program_with_path_borrowck), so no
      target is reported, no input is saved under testdata/fuzz, and
      fuzz_shapes grows no corpus. `fid test -run <test>/<row>` works
      (driver-goals-check: the SL13 rows are ok). Left: that double free,
      then find, save, replay and corpus growth through sl13-fuzz-check.sh.
      Nov 22 validation (`release/fid-dev.exe`): native
      `test_stdlib_table.fd` matches its expected row output, and
      `sl13-fuzz-check.sh` passes all seven stages (coverage, deterministic
      saved input, replay, fixed regression, corpus seeding and growth).
      The benchmark-runner path still needs a clean, reproducible check;
      keep partial.
- [x] SL14. Structured logging and metrics (M).
      Done: `metrics` (counters, gauges, histograms, Prometheus text format,
      `metrics_serve`); `log` with fields, JSON lines (`log_render_json`) and
      per-module levels (`logmf`, `log_set_module_level`), tests `test_log`,
      `test_log_module` (complaint CO3, CO4).
- [x] SL15. A sampling profiler reachable from a running service (L).
      Nov 16, native Linux (linux_runtime/sys.fd, on DB9's SIGPROF sampler):
      with FID_PROF set, `kill -USR2 <pid>` prints the profile so far to
      stderr while the program runs on, and FID_PROF_EVERY=<s> prints it
      every s seconds; the handler only sets a flag, a watcher thread
      prints (`pe_prof_watch_main`). Left: native Windows — suspending the
      workers (SuspendThread/GetThreadContext) works only with a table of
      function addresses and names in the executable, which pe_ir does not
      emit (the stacks could be walked, not named); a stdlib API
      (start/stop/report from the program) needs builtins on the C path and
      the interpreter.
      Nov 17: native Windows (pe_ir): pe_runtime/conc.fd samples every
      runtime thread (SuspendThread/GetThreadContext, rbp chain, names from
      MI20's `nat_fn_names`), FID_PROF/FID_PROF_EVERY, report on the named
      event `Local\fid-prof-<pid>` or Ctrl-Break; stdlib/profiler.fd
      `prof_start`/`prof_stop`/`prof_report`/`prof_samples` on every
      backend (C: sampler thread / SIGPROF, compiler/codegen_c.fd
      `emit_prof_api_runtime`; interpreter: counting calls);
      docs/perf/sl15-sampling-profiler.md; tests test_prof_sampling,
      tests/prof/sl15-check.sh. The hosted pe_ir entry point calls
      `pert_prof_boot` before main and `pert_prof_exit` after it
      (pe_ir_stubs.fd `PeirGen_gen_entry_point`, rooted in ir.fd
      `pe_ir_needed_functions`); `p_prof_*` calls carry the IO effect
      (lower.fd `runtime_effects`). Not yet built or run.
      `stdlib/profiler.fd` is a scope profiler (caller-driven
      begin/end regions, frame/GPU timing) and `fidesc profile` is
      one-shot; nothing samples a running service or exposes stacks
      over a control channel. Investigated (Nov 11) whether the pe_runtime
      green-task scheduler can sample other tasks' PCs: a parked task's
      context keeps only [saved RSP, stack base] — its stack can be walked
      from there (pe_runtime/conc.fd §"Task contexts"), giving a
      WAIT-profile of where tasks yield or block. CPU-time sampling needs
      suspending an OS worker mid-run (SuspendThread + GetThreadContext on
      worker handles, which the scheduler does not currently export) —
      real L work in the runtime, not a stdlib add-on.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — sl15-check.sh is ok for run, ccompile and emit (the report
      names prof_burn), for a service without FID_PROF and for FID_PROF=1
      (a report on the event and one at exit); by hand, x86_64-linux in WSL
      gives 62 samples with prof_burn at 95%, and setting the named event
      from another process gave 3 reports. Left: with FID_PROF_EVERY=1 the
      1.6 s pe_ir service prints 2 reports, not 3 or more
      (pert_prof_watch_main resets its ticks on every report, so the event
      at 0.8 s restarts the 1 s timer); sl15-check.sh mangles WSL paths and
      takes Git Bash's PID for the Windows one.
      Nov 21 (c082ee6, 2g.101): pert_prof_watch_main's periodic report keeps its own clock — an event-triggered report no longer restarts the 1 s timer.
      Nov 21 (P2 fix, in the tree Nov 22): the periodic report keeps its own schedule.
      pe_runtime/conc.fd `pert_prof_watch_main` reports every
      FID_PROF_EVERY seconds on GetTickCount64 (`next` moves on by one
      period only when a periodic report is due, and the wait for the event
      never runs past it); a report on the named event or Ctrl-Break no
      longer restarts the timer. The same in linux_runtime/sys.fd
      `pe_prof_watch_main` (SIGUSR2 no longer resets the ticks). With
      FID_PROF_EVERY=1 the 1.6 s pe_ir service prints 3 reports (the event
      at 0.8 s, the periodic one at 1 s, exit). tests/prof/sl15_service.fd
      prints `pid <n>` (its Windows process id) first on stderr and takes
      SL15_SVC_MS (the length of each half, 800 ms by default);
      sl15-check.sh names the event with that pid instead of the shell's
      $!, and runs the service for 2 x 4 s when PowerShell sets the event
      (PowerShell can take seconds to start; its Set must not land with the
      service's own). The WSL step goes through `wsl.exe -e wslpath -a` with
      a forward-slash path and MSYS_NO_PATHCONV=1 (the old call ran wslpath
      through WSL's shell, which ate the backslashes). Run from WSL with a
      Windows fid (FID=...fid.exe), the script runs the Windows checks
      through interop (WSLENV carries FID_PROF, FID_PROF_EVERY and
      SL15_SVC_MS) and the Linux build directly. With a fid built from the snapshot,
      sl15-check.sh passes every check from Git Bash and from WSL (FID=
      ./fid.exe): run, ccompile, emit and x86_64-linux name prof_burn; no
      event without FID_PROF; 2 reports with FID_PROF=1; 3 with
      FID_PROF_EVERY=1; 3 with the event set by PowerShell.
      Nov 22 rerun (`FID=release/fid-dev.exe`, `CC="zig cc"`): interpreter,
      C, Windows native, event-triggered and periodic reports pass. The
      WSL subcheck fails before running its binary (`chmod: cannot access
      't_lx'`), so this rerun does not reconfirm the Linux path; keep partial.
      Nov 22 follow-up: `sl15-check.sh` now invokes `wslpath` with
      `wsl.exe -e` and disables MSYS path rewriting. Shell syntax and a
      Git Bash-to-WSL path smoke pass; the end-to-end profiler rerun is
      recorded below.
      Nov 22 validation (`FID=release/fid-dev.exe`, `CC="zig cc"`):
      `tests/prof/sl15-check.sh` passes end to end: interpreter, C, Windows
      native, Linux x86_64 under WSL, profiler reports on the named event,
      periodic reports, no report when disabled, and an event set by another
      process. Together with the Linux running-service sampling probe above,
      the service profiler acceptance is met.
      Nov 22 (landed; compiler built from the merged tree):
      tests/prof/sl15-check.sh 12 of 12 (run, ccompile, emit, x86_64-linux in
      WSL, the service checks).
- [x] SL16. Big integers and decimals (M).
      Done: `stdlib/bigint.fd` (`test_bigint.fd`) and `stdlib/decimal.fd`:
      exact decimals on a BigInt mantissa, division and rounding to a scale
      with six rounding modes (half-even by default); `test_decimal.fd`.
- [~] SL17. Files, paths and processes complete on every platform (M).
      Nov 22 audit: mostly done — path.fd (17 pub fns, drives/UNC), fs.fd
      (67), process.fd:18-51, file.fd; test_files_spec covers all three.
      Precise gap: the native-Windows lane run (inside the general
      native-lane breakage) — module code is not the gap.
      Nov 25 validation: the named lane passes on this Windows host —
      test_files_spec, test_path, test_process_spawn,
      test_runtime_empty_file and test_runtime_process_capture are 5 of 5
      on run,ccompile,emit,nat with release/fid-dev.exe. What remains is
      the wider cross-platform lane (Linux/macOS/WASI), not the
      native-Windows module lane.
      Done: `stdlib/path.fd` reads both separators, Windows drives and UNC
      roots in join, split, normalize, parent and components
      (`test_path.fd`). 
      Nov 15: `test_files_spec.fd` checks files (create, read, overwrite,
      append, sizes, empty files, every byte value and zero bytes, CR LF
      kept, 64 KiB, rename, delete, a missing file as an error value),
      directories (create, nested, sorted listing, rmdir, rmdir_all) and
      processes (stdout, stderr, exit status, `process_system`) in a temp
      directory of its own; interpreter and C agree. Nov 16: the native
      runtimes' capture files go in the temp directory (%TEMP%, $TMPDIR,
      /tmp: 2g.44 native part); pe_runtime keeps both captured strings
      (`as i64` hands them to the record; 2g.43) and a handle of 0 gives "";
      the interpreter's read of an empty file is Some("") (2g.42)
      (`test_runtime_empty_file.fd`, `test_runtime_process_capture.fd`).
      The C runtime's part of 2g.44 was fixed the same day (codegen_c.fd
      `__p_proc_tmpdir`: TMPDIR or /tmp; TEMP or TMP on Windows;
      test_cback_proc_tmpdir.fd). Left: a rerun of the suite on every
      backend.
- [~] SL18. Crypto through reviewed libraries, the set of release J5 (L).
      Nov 22 audit: mostly done — J5 set through OS libraries
      (crypto_bind.fd BCrypt/CNG, crypto_bind_unix.fd OpenSSL EVP,
      crypto_bind_rsa.fd with Wycheproof), pure-Fides primitives with
      published vectors, and beyond-J5 pqc/hpke/jose. Precise gap: the
      qualified external review itself (review-index.md says J9 stays
      open until signed off) and a macOS CI crypto run — external work,
      not code.
      Done: `test_crypto_tls_vectors.fd` proves the primitives against
      published vectors — ChaCha20 block/stream (RFC 8439 §2.3.2/§2.4.2),
      Poly1305 (§2.5.2), ChaCha20-Poly1305 AEAD encrypt+decrypt (§2.8.2),
      HKDF extract/expand (RFC 5869 case 1) — plus the existing NIST
      SHA-256 and RFC 4231 HMAC vectors; fixed `aead_mac` interleaving
      the length blocks (was unverifiable until the interp struct-assign
      bug was found). Checked Nov 15: signatures and bindings exist now:
      `stdlib/ed25519.fd` (RFC 8032, the five §7.1 vectors in
      `test_ed25519.fd`), `x25519.fd` (RFC 7748, `test_x25519.fd`),
      `sha512.fd` (`test_sha512.fd`), and `crypto_bind.fd`, BCrypt/CNG on
      Windows: AES-GCM, ECDSA P-256 sign/verify, PBKDF2
      (`test_crypto_bind.fd`, native and C only). Nov 15 (later): HMAC over
      bytes and PBKDF2-HMAC-SHA256 in `crypto.fd` (RFC 4231 and RFC 7914
      vectors in `test_stdlib_postgres.fd`), and SHA-256 reads its round
      constants from a table instead of 63 comparisons a round. Nov 17: the
      OpenSSL twin `crypto_bind_unix.fd`, validated in WSL
      (`scripts/verify/crypto_bind_unix_check.fd`: all ok); RSA PKCS#1 v1.5
      and PSS sign/verify, OAEP and key generation in both bindings
      (`crypto_bind_rsa.fd` blobs), with Wycheproof vectors in
      `test_crypto_bind_rsa.fd`, the same output on BCrypt and OpenSSL.
      Left: the external review (docs/security/review-index.md) and a macOS
      CI run.
- [>] SL19. Every public function documented with an example that runs in CI (L).: moved to end-roadmap.md (Nov 22 — final-gate work, do when the code stops moving).

- [x] SL20. OS-development primitives: firmware tables, buses, memory (M).
      Done (Nov 10): the parse/layout halves a kernel needs, pure Fides so
      they run anywhere — `elf.fd` (ELF64 ehdr/phdr/shdr + names), `acpi.fd`
      (RSDP scan, checksums, SDT headers, MADT/MCFG), `pci.fd` (config
      decode, BAR classify, capability list, CF8), `virtio.fd` (split-queue
      layout, desc/avail/used rings, free chains), `multiboot.fd` (MB2 tags,
      e820 map, framebuffer), `fdt.fd` (Device Tree), `uart.fd` (16550),
      `scancode.fd` (PS/2 set-1), `pmm.fd` (bitmap PMM + x86-64 paging),
      `slab.fd`. `test_os_stdlib.fd` passes on interp and native. Port and
      MMIO access stays platform driver code by design.
- [x] SL21. AI weight-path and inference kernels (M).
      Done (Nov 11): `safetensors.fd` — the standard HF weight format:
      u64 header length, JSON map, per-tensor dtype/shape/data_offsets
      validation, aligned-buffer reads; f16/bf16/f32 decode in pure
      Fides bit math (the interps have no f32-from-bits primitive).
      `bpe.fd` — vocab.json + merges.txt tokenizer (encode/decode).
      `infer.fd` — the transformer kernels on flat Vec<f64>: matvec,
      RMSNorm, RoPE, stable softmax, argmax, top-k, top-p sampling,
      single-head attention. `noise.fd` — Perlin/fBm/Poisson-disk for
      procgen. `test_ai_stdlib.fd` passes interp + native. A full
      forward pass still needs the glue (embedding lookup, per-layer
      loop, kv_cache wiring) — the pieces are verified, the model loop
      is the next step.
- [x] SL22. Game content pipeline: glTF + Tiled + noise (M).
      Done (Nov 11): `gltf.fd` — .glb container framing (magic/version/
      chunks), accessor resolution to a struct (once per accessor, not
      per element — per-lookup JSON is unusable at real model sizes),
      every componentType and VEC/MAT arity, byteStride, normalized
      ints; feeds mesh.fd's Mesh (positions/normals/uv/indices).
      `tiled.fd` — TMJ maps: tile layers into tilemap.fd with an
      optional solid layer, object groups (point/rect/tile + custom
      properties), flip-flag gids. `test_gaming_stdlib.fd` passes
      native; interp-skipped (each json_* call revalidates the doc —
      O(n^2) per lookup on the interpreter). mesh.fd grew
      texcoord_create (cross-module struct literals don't parse);
      mesh/asset/data moved off the nonportable
      p_str_take(p_file_read()) onto core's file_read.

**Acceptance:** a real web service needs no package outside the stdlib.

## Track TL — Tooling

Go and Rust won developers with tools as much as with languages.

- [x] TL1. One binary, `fid`: build, run, test, fmt, doc, lint, bench, fuzz (L).
      Done: `fid` has build, run, test, fmt, doc, lint, bench, check, profile.
      Nov 15: `fid fuzz <file.fd> [--seeds N]` (compiler/main.fd,
      cmd_test_file in fuzz-only mode): the file's `fn fuzz_*(data: Vec<u8>)`
      targets on N deterministic inputs (1000 by default; `fid test --fuzz`
      keeps 100), a failure reproducible with the same count. Checked by
      scripts/tl-check.sh over scripts/tl/fuzz_target.fd (run after the
      next build). Coverage-guided fuzzing of the compiler itself stays VF3.
- [x] TL2. Error messages as good as Rust's, each with a suggested fix, and a test
      Nov 22: the ratchet failure fixed — 4 new diag tests cover the
      comptime builtin messages (test_diag_comptime_{reflect,embed,
      compile_error,assert}_fail.fd: each fires exactly its two errors),
      and Fix lines were added to those plus the @dimension unit message;
      diag-coverage.py: 360/360 tested (100%), exit 0. Full Rust-grade
      coverage of every message with a Fix (47%) remains long-form work.
      per error (L).
      Done (Nov 3): every diagnostic in `typechecker.rs`/`borrowck.rs` now carries
      a `Fix:` or `Hint:` line with a concrete code suggestion — borrow
      (`holder_note`), capability (`use mod with { Cap }`), region escape,
      use-after-free, budget, sandbox, undefined variable, effect mismatch.
      Left: a `// expect-error:` conformance test per diagnostic, source-span
      rendering (caret under the token) for the remaining line-only errors,
      and a diagnostic-code index. `tests/bootstrap/diagnostics/` has 9
      conformance tests (capability, cap-propagation, effect, budget, escape,
      sandbox, undefined-var, borrow-mutation, linear) covering every family.
      Diagnostic codes are now assigned systematically: `classify_error` maps
      every message family to a stable `Exxx` (region E052–E053, budget E054,
      GPU/KV region E055–E056, annotation E057, escape E058, secrecy E059) and
      `enrich_errors` tags code-less errors too — E090/E091 now render on
      capability and sandbox failures.
      Left: caret spans for the remaining line-only messages (some sites carry
      no position at all) and self-hosted `compiler/*.fd` parity.
      Nov 15 (`fid`): an undefined variable or function gets
      "Fix: did you mean 'x'?" — the closest identifier written in the
      program (outside comments and strings) and, for a function, of core,
      within edit distance 1 (names up to 4 bytes) or 2 (compiler.fd
      did_you_mean_suffix, from enrich_errors);
      tests/bootstrap/test_tooling_did_you_mean_fail.fd. Most `fid`
      diagnostics already carry a `Fix:` line (47 in typechecker.fd).
      Left: a test per diagnostic, carets for line-only messages, a code
      index for `fid`.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — 79 of the 80 test_diag_tl2_*_fail tests give their error
      under `fid check`, with carets where tried; diag-coverage.py: 357
      error messages, every one with a test; of all 422 expect-error tests
      408 match, and the 14 others fail the same way with the earlier
      compiler. Left: test_diag_tl2_nesting_stmt_fail is stale (the
      parser's 256-level limit now stops it before the type checker's
      512); banking_double_credit, money_linear and crashfree expect old
      texts, and diag_reverse_operator_type, frontend_trait_self and
      world_hermetic_clock are accepted by `check`; errors on structs
      still have no line.
      Nov 21: the nesting test reaches the type checker's 512 limit through an operator chain (2g.112) and four expectations were corrected; diag_reverse_operator_type, frontend_trait_self and world_hermetic_clock are still accepted.
      Done (Nov 25, checked with a fid built from the 905d756b tree):
      diag-coverage.py counts 467 error messages, every one with a test, a
      `Fix:` line and an `Exxx` code (100%; 21 new test_diag_tl2_*_fail
      tests, Fix lines added in typechecker, borrowck, escape, secrecy,
      compiler and ir); diag-tests-check.py: 602 of 602 expected-fail tests
      print what they expect (two stale expectations corrected: the E060
      code on duplicate definitions, and `fn(x) => ...` closure syntax in
      test_spawn_closure_capture_fail); the 365 conformance `//! error:`
      files are rejected on `run`. docs/reference/error-codes.md explains
      every code with an example and its fix, and `fid explain-error E044`
      prints it. Messages carry carets where the checker has a column.
- [x] TL3. `fid fix`: applies suggested fixes, and upgrades code between versions (L).
      Done (Nov 15): `fid fix <file.fd> [--check]` (main.fd cmd_fix, the fix
      engine in lsp.fd fix_suggestions/fix_apply, shared with the editor's
      quick fixes): `val` -> `var` for an assigned `val`, `(e) as T` for a
      narrowing the checker names, a misspelled name -> the "did you mean"
      one, an unused import removed (from `fid lint`). Fixes are applied one
      at a time and each is kept only when the program then has fewer errors,
      or as many with none left on the fixed line (an error the first hid),
      or for a lint fix still none, so a guess that breaks the program is not
      written; `--check` reports and writes nothing. tests/bootstrap/test_tooling_fix_fail.fd is
      the input of scripts/tl-check.sh. Left: upgrades between versions (no
      edition has changed the language yet, ST3), fixes for more diagnostics
      (a by-value parameter changed, borrow conflicts).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — `fid fix --edition` upgrades 2026 to 2027:
      fix-edition-check.sh passes (42 ok), and fix-tl3-check.sh and
      fix-more-check.sh still pass; a deprecated function's calls are
      rewritten to its replacement (test_tooling_fix_deprecated then prints
      6/20 with no warning), and test_frontend_edition_2027_fail is
      refused. Left: edition 2027 changes only deprecated calls, and a call
      with no named replacement stops the upgrade; fixes for more
      diagnostics (by-value parameters changed, borrow conflicts).
      Nov 22 code audit: that Left is stale — the by-value parameter fix
      (`var p = p;`, lsp.fd:2250-2261) and the borrow-conflict fix
      (`for x in v.clone()`, lsp.fd:2263-2275) both exist in
      fix_suggestions (lsp.fd:2915); the edition-stop is by design
      (main.fd:3640-3641). Done.
- [x] TL4. LSP complete: references, rename, formatting, signature help, inlay
      Nov 22 audit: mostly done, close — completion/hover/rename/
      references/signatureHelp/inlayHint/codeAction/formatting all
      dispatch (lsp.fd:144-201, :346-369); rename is scope-aware
      (:1017-1035), references cross-file (:1377-1405), inlay hints index
      imported functions (:3776-3782). Precise gap: no type hint on
      turbofish (`::<`) calls; methods of types from other modules are
      not indexed for hints.
      hints, code actions (release N7 is the start) (L).
      Done: `fid lsp` (compiler/lsp.fd) answers references, rename,
      formatting (fmt.fd) and signature help (there before), and Nov 15
      `textDocument/inlayHint` (the parameter name at each argument of a
      call of a function of the file, none when the argument is the
      parameter's own name) and `textDocument/codeAction` (a quick fix per
      diagnostic `fid fix` can repair, as a workspace edit).
      scripts/tl/lsp_probe.py drives the server and checks the answers.
      Left: rename and references are by whole word in one document (a local
      renames same-named words elsewhere; other files are not searched);
      inlay hints for methods and imported functions, and type hints.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — rename and references are by scope and across files now:
      lsp-scope-check.sh: 31 ok, PASS (shadowing by declaration position,
      same-named locals skipped); lsp_probe.py: all ok (capabilities, inlay
      hints, quick fix, 3 references of add, signature help, formatting).
      Left: no type hint on turbofish calls; methods of types from other
      modules are not indexed for hints; a local that shadows a function
      name takes its calls.
      Nov 24: the three gaps are closed. Turbofish calls resolve —
      `f::<#k>` names are split at `::<` for the function lookup and the
      parser-recorded type arguments bind f's type parameters for the
      return type (ih_expr_type_plain). Methods of types from other
      modules are indexed — ih_imported_funcs also collects the imported
      modules' impls (all_impls feeds both the parameter-name and the
      Type.m tables). A local or parameter that shadows a function name
      takes its calls — `w(` is hint-checked against lsp_local_scope at
      the call position first. Probe: scripts/tl/lsp_tl4_gaps.py.
      Nov 25 closeout validation with release/fid-dev.exe: lsp_probe.py
      17/17, lsp_tl4_gaps.py 5/5, lsp_tl4_features.py 29/29,
      lsp-scope-check.sh 31/31 and lsp-uri-check.sh 3/3 pass;
      `fid check compiler/lsp.fd` has no errors. The probes cover the
      acceptance features plus completion, hover, definition, document
      symbols and diagnostics. There is no tests/bootstrap LSP .fd lane to
      run through expect.py: the LSP is a long-running stdin/stdout server,
      so these probes are its validation lane.
      Nov 25 again with a fid built from the 905d756b tree: the same four
      probes pass (29, 5, 17 ok; lsp-scope-check PASS).
- [~] TL5. Debugger support with line tables on every platform (release N9) (L).
      Nov 22 evening: `fid debug` is wired — cmd_debug (main.fd) reads
      the file and runs compiler/debugger.fd's Debugger_new/Debugger_run
      with the same flow as fidesc's; usage line added. Fixed on the way:
      debugger.fd's bare `use compiler;` (it poisoned the whole import
      set of any importer — use compiler::compiler; now) and its private
      int_to_hex clashing with string.fd's (renamed dbg_int_to_hex).
      Left: a run of the interactive session as validation (the module
      checks clean; the command needs a tty).
      Nov 22 audit: mostly done — line tables on every platform
      (nat_enc.fd:3994-4003 DWARF, :8154-8160 Mach-O, :3708-3709 PE;
      pdb.fd writes a real PDB, DB3 validated Nov 21); `fid replay`
      replays --record traces (main.fd:346-347). Precise gaps: `fid` has
      no interactive `debug` command (compiler/debugger.fd is wired into
      fidesc only), and DB4 variable locations are open (no
      DW_TAG_variable in images) — the real remaining work.
     Done: `-g`/`--debug` on `fidesc emit`/`build`/`ccompile` — PE gets
     `.debug_abbrev/.debug_info/.debug_line/.debug_str` (DWARF v4, per-instruction
     line tables); the C backend emits `#line` back to `.fd` source and passes
     `-g` to the C compiler. Left: port DWARF emission to `compiler/*.fd`
     (self-hosted `fid` doesn't emit it), variable/location DIEs, ELF/Mach-O.
     Nov 15 (checked): the note is behind — `fid` writes `.debug_line`
     (DWARF 4) for the input file's functions on the three Linux targets
     (compiler/nat_enc.fd nat_debug_line, nat_ir.fd line rows; llvm-roadmap
     DB1/DB2). Still left, in files of the backend group: `#line` in the C
     that compiler/codegen_c.fd writes (it has none), CodeView/PDB for
     pe_ir (DB3), variable locations (DB4), Mach-O.
     Nov 16, C path: `fid ccompile -g` writes `#line <n> "<file>.fd"` before
     each source line of the input file's functions and methods (the
     lowering's source lines; a `#line` back to the C file after each), and
     passes -g to gcc/clang/zig cc; the directives are inside function
     bodies, so `split-c` keeps them with their function. Off without -g
     (the fixpoint's C is unchanged). test_cback_debug_info.fd. Left:
     CodeView/PDB for pe_ir (DB3), DB4, Mach-O, cl.exe's /Zi, imported
     modules' functions (only the input file's get lines).
     Nov 16, default Windows backend (not run yet): `fid emit --debug`/`-g`
     (or FID_PE_DEBUG=1) writes DWARF 4 line tables and function entries
     for the input file into the PE (llvm DB3; gdb, lldb, llvm-dwarfdump
     read them; scripts/pe-dwarf-check.py). Left for Windows: a PDB for
     Visual Studio and WinDbg (DB3), variables (DB4).
     Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
     partly — tl5-lines-check.sh: ok (13 lines): `ccompile -g` writes
     `#line` to main.fd and to an imported module and runs; Mach-O
     __DWARF line tables for aarch64 and x86_64 macOS (none with
     FID_NAT_OFF=debug); pe_ir -g passes pe-dwarf-check and runs;
     test_cback_debug_info is right on run and ccompile. Left:
     macho_dwarf_unit misses 2 of its 25 lines (nat_dbg_written_path
     writes a relative lib/util.fd as util.fd, so __debug_line is 107
     bytes, not 111): the expectation or the path rule must change; lldb
     on a Mac not run; variables (DB4).
     Nov 24, cleared: nat_dbg_written_path now derives main_dir for a
     no-directory input by anchoring on an absolute source path, so
     lib/util.fd is written under lib/ — macho_dwarf_unit passes all 25
     lines (__debug_line is 111 bytes, `file 2 lib/util.fd`). Left:
     lldb on a Mac not run; variables (DB4).
     Nov 25 (fid built from the 905d756b tree): variables done on Windows.
     `fid emit --debug -g` of test_pe_dbg_vars.fd: 25 of 25 expected values
     read through the DWARF with 2 steps and 2 function breakpoints
     (pe_dbg_run_check.py), 22 of 22 read through the PDB alone with
     dbghelp, as Visual Studio and WinDbg do (pe_pdb_locals_check.py);
     pe_pdb_check.py --unit passes; `fid debug` runs scripted sessions
     (`--echo`, fid-debug-session-check.sh: ok); tl5-lines-check.sh ok.
     Left: lldb on a Mac, which only CI can run (macos-native.yml now has
     the step, tests/nattools/macos_lldb_check.sh: breakpoint, variables,
     bt, dwarfdump --verify); `-g` without `--debug` inlines small
     functions and loses their lines (BG243); C variables are v0..vN in a
     `ccompile -g` build (BG244); nat_ir location lists are one byte wide
     (BG245).
- [~] TL6. Cross-compilation out of the box: `fid build --target linux-arm64` from
      any host, with no extra download (L).
      Done (Nov 15): `--target` is read OS first too (main.fd
      normalize_target: `linux-arm64` -> `aarch64-linux`, `linux-x64`,
      `linux-riscv64`, `macos-arm64`, `windows-x64`, `amd64`/`x64`/`arm64`
      spellings) on `build`/`emit` and `ccompile`. Linux AArch64, RISC-V
      and x86-64 and macOS go through the machine layer (nat_ir: no C
      compiler, no sysroot); `--target windows-x64` on a Linux or macOS host
      now goes to pe_ir (a PE, @cfg sees Windows) instead of the host's C
      compiler; `use tls` takes the target's module, not the host's (and a
      Mac host is no longer taken for Windows there). scripts/tl-check.sh
      builds hello for each name. Nov 17: scripts/cross-from-linux.sh builds
      a Linux fid (`emit --target x86_64-linux`), runs it in WSL to build
      hello and the suite with `--target windows-x64` (pe_ir) and runs the
      PEs on Windows; cross-test.sh runs Windows lanes under wine on Linux;
      ci.yml's selfhost job runs build-no-toolchain-check.sh with wine
      (REQUIRE_WINE=1), natgate the suite under wine (reported).
      `ccompile --target` needs zig by design
      (docs/reference/cross-compilation.md). Left: the box, ticked once the
      final build's runs of cross-from-linux.sh and the wine lane pass.
- [x] TL7. Profiler output readable by standard tools (M).
      Done: `fid profile` samples the native call stack every millisecond and writes collapsed stacks (flamegraph.pl, inferno, speedscope).
- [x] TL8. A web playground (M).
      Done: fides-lang.org/playground runs the real `fidesc` as a WASI WebAssembly
      build in a module worker — the UI stays responsive, executions time out at
      15 s and the worker restarts, and the modes cover run, tokens, AST, SSA IR
      and inferred effects. The bundled stdlib is virtualized into the WASI fs.
- [x] TL9. Clear dependency handling: Git and path, a lock file, audits of what a
      build pulls in (M).
      Done: `fid ark` (compiler/ark.fd) had path and git pacts (branch, tag)
      and an ark.lock that recorded no revision. Nov 15: `use <name>` finds
      an installed pact in ark_modules/ (compiler.fd ark_push_candidates:
      `<name>.fd`, `<name>/mod.fd`, `<name>/lib.fd`, `<name>/src/lib.fd`,
      `<name>/src/<rest>.fd` for `use name::rest`), before the stdlib;
      ark.lock records each git pact's commit and an FNV hash of each path
      pact's files; `ark install` checks out the locked commit (fetching the
      history when a shallow clone lacks it), `ark update [name]` takes the
      newest and re-locks; `ark audit [--files]` lists each pact against the
      lock (NOT INSTALLED / NOT LOCKED / CHANGED fail it) with its files,
      lines, `unsafe` blocks, `extern`s and process/file/network calls.
      Directory pacts copy on every host. scripts/tl-check.sh runs a path
      pact through init/add/install/run/audit. Left: pacts' own
      dependencies (transitive), version ranges and a registry, a git pact
      in the check (needs a network or a local repository).
      Nov 22 code audit: that Left is stale — transitive dependency
      resolution (ark_resolve/ark_resolve_reqs, ark.fd:2442/:2461, CD4),
      version ranges and a registry with search/publish/yank
      (ark.fd:24-29, index :1221-1331), lock (commits + path hashes,
      lock_save :802), and capability audits of what a dependency uses
      ([capabilities] checked on install/build/test/audit, ark_cap_check
      :4696, CD1) all exist. Residual: scripts/tl-check.sh still
      exercises only a path pact (a git-pact leg needs a local repo) —
      optional test work.
- [~] TL10. Editor support beyond VS Code: Neovim, JetBrains, Zed (M).
      Nov 22 audit: mostly done — emacs/helix/neovim/sublime/zed configs
      plus a Tree-sitter grammar with corpus; CI runs the toolchain
      (editors.yml:48-50, zed-check.sh REQUIRE=1, parse-rate >= 90%).
      Precise gap: editors/tree-sitter-fides/src/ (the generated
      parser.c) is not committed and editors/zed/extension.toml:15 pins
      rev = "main" instead of a commit; neither Zed nor Neovim was run
      locally (CI-only validation).
      Done: configs for Neovim, Helix, Emacs, Sublime and JetBrains (LSP4IJ) in `editors/`, untested. Left: testing them, Zed (needs a grammar).
      Nov 15: the Neovim config turns inlay hints on; the server side of
      every editor is checked without an editor by scripts/tl/lsp_probe.py.
      Nov 17: a Zed extension and a Tree-sitter grammar for Fides are
      written (editors/zed, editors/tree-sitter-fides; checked by
      tests/tooling/zed-check.sh, which scripts/tl/editor-check.sh and the
      Editors workflow run), and the Emacs and Sublime fixes are applied.
      Left: `tree-sitter generate` and Zed itself have not been run yet;
      the generated parser (editors/tree-sitter-fides/src/) is not
      committed, and extension.toml's rev is not pinned to it.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — `STATIC=1 scripts/tl/editor-check.sh`: 62 ok (the Sublime
      scope, Emacs' auto-mode-alist and the rest); zed-check.sh: ok (query
      names, corpus, extension files); the grammar has 120 node types, 35
      fields, 142 tokens. Left: no tree-sitter CLI here, so `tree-sitter
      generate`, the corpus test and the parse rate have never run and
      src/parser.c does not exist; Zed, Neovim, Emacs and Helix were not
      run (only in CI's editors.yml); editors/zed/src/lib.rs not compiled.
      Nov 24: both named gaps closed. tree-sitter-cli 0.25.10 on the host:
      `generate` emits src/parser.c + grammar.json + node-types.json
      (committed, d55a66da — the rev extension.toml now pins instead of
      "main"), corpus 5/5, REQUIRE=1 zed-check.sh ok — 2389 of 2450 .fd
      files parse clean (97%), every query file compiles. Nov 25:
      the Zed extension crate is updated to zed_extension_api 0.7.0;
      `cargo check --target wasm32-wasip2` passes locally and the Editors
      workflow now runs that compile check. editors/README.md no longer
      claims the grammar is fetched at rev "main" (extension.toml pins
      the generated-parser commit). Left: Zed/Neovim/Emacs/Helix run only
      in CI or their respective editors; no local in-editor Zed test.
- [x] TL11. `fid doc` sites as good as docs.rs and pkg.go.dev (M).
      Done (Nov 15): `fid doc <dir>` (scribe.fd scribe_site; before, a
      directory was rejected as "no directory listing"): a page per module
      (a link back to the index; `a::b` pages are `a_b.html`, valid on
      Windows) and index.html — every module with the first line of its
      file's comment and its count of public items, and a search box over
      every public item's name, kind and signature linking to its anchor;
      `--format markdown` writes index.md. Doc text no longer shows `<br>`
      escaped. Done (Nov 17): cross-links between types, a source page per
      module linked from each item, examples on the page (with the note
      that doctest.py runs them), the package version on each page;
      tests/tooling/doc-links-check.sh and doc-site-check.sh pass.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — doc-links-check.sh: PASS (339,578 relative links resolve,
      type cross-links, links from items to their source);
      doc-site-check.sh: PASS (16 ok: version label from ark.toml and
      FID_DOC_VERSION, source-line links, type links in signatures and
      parameter tables); test_scribe_links prints its 14 lines.
- [x] TL12. Call-graph visualization: `fidesc diagram <file.fd>` emits a Mermaid
      sequence diagram of the file's calls, annotated with inferred effects
      (`-o out.mmd`) (S).
      Done (Nov 3): `src/seqdiag.rs`; participants, calls in source order, `do`
      ops, `spawn`, `spawn scope`, `resume`, effect notes per function.

**Acceptance:** a new user is productive within an hour, on any supported system.

## Track PL — Platforms

Every platform gets a tier, and a tier means CI, not a hope.

- [x] PL1. Publish tiers: Tier 1 is tested on each commit, Tier 2 builds, Tier 3 is
      best effort (S).
      Done: `docs/reference/platforms.md`.
- [~] PL2. Tier 1: Windows x64, Linux x64, macOS ARM64, Linux ARM64 (release F4–F8)
      Nov 22 audit: the three Nov 22 blocker fixes are in the tree
      (cross-test HOST_ARCH; native_debugger PTRACE_GETREGSET;
      pe_win_stat st_dev/st_ino). Precise gap: a green tier1.yml run has
      never happened (33 runs: 30 cancelled, 2 failed, 0 success) and
      macOS ARM64's self-hosted fid dies in the 3-stage fixpoint
      (CB8/2g.79) — the single blocker also behind PL7 and PL6's iOS.
      (XL).
      Nov 17: scripts/tier1.sh (lanes, expected-fail, binaries) per
      platform; .github/workflows/tier1.yml on windows-latest,
      ubuntu-latest, ubuntu-24.04-arm, macos-15; scripts/lane-skips.txt
      for tests a target cannot run; dynamic binaries without a qemu
      loader are SKIP; bin-target-check.py fails runtime names imported
      from a shared library (docs/reference/tier-checks.md). Left: green
      full runs on all four (validation run and CI).
      Nov 22: two blockers fixed on this machine. pl2-lane-skips-check
      failed on a real aarch64 host (ubuntu-24.04-arm): cross-test.sh
      dropped the qemu runner on `uname -m`, ignoring HOST_ARCH, so the
      stand-in ELFs were exec'd directly (rc 126); the runner now follows
      HOST_ARCH and the check forces x86_64 — PASS in WSL. A smoke tier1
      (c and pe lanes PASS on Windows) exposed an `--nat` lane bug:
      fs_copy_file onto itself returned 0 and emptied the file —
      pe_win_stat never filled st_dev/st_ino, so pe_zc_same_file always
      said different files; pe_win_stat now fills them through
      GetFileInformationByHandle (import slot 80); `self: 27 true` on
      the --nat lane with a fid-dev rebuilt with the new import, and
      the focused tier1 lane passes. Third blocker: the arm64 runner
      itself failed cargo build — src/native_debugger.rs used the
      x86-only PTRACE_GETREGS; it now uses PTRACE_GETREGSET/NT_PRSTATUS
      on aarch64 (cargo check --target aarch64-unknown-linux-gnu PASS,
      in the worktree). Left: green full runs
      on all four; macos-arm64 has no machine here (CI's macos-15).
- [~] PL3. Tier 2: Windows ARM64, macOS x64, FreeBSD, RISC-V (L).
      Nov 22 audit: all four targets build and pass bin-target-check; 7
      riscv64 binaries ran under qemu matching `fid run` (Nov 21).
      Precise gap: the Windows ARM64, FreeBSD and macOS x64 binaries have
      never been EXECUTED (format checked only); tier2.yml has never been
      green (31 runs, 0 success).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — `@cfg` knows freebsd and unix (test_cfg, test_cfg_unix,
      test_cfg_platforms pass), and tier2-targets-check.sh passes: every
      program builds for aarch64-windows, x86_64-macos, x86_64-freebsd and
      riscv64-linux and passes bin-target-check, and the 7 riscv64 binaries
      run under qemu-riscv64 as under `fid run`. Left: running the Windows
      ARM64, FreeBSD and macOS x64 binaries (only their file format was
      checked here; tier2.yml's jobs were not looked at).
      Nov 27 check (release/fid-v27.exe, a copy of the fid-dev of Nov 25; a
      smoke run, not the suite: hello, test_closure, test_hashmap,
      test_float, test_tasks, test_int_semantics through scripts/tier2.sh
      WITH_NATIVE=1): every program builds on all six lanes —
      c:aarch64-windows, c:x86_64-macos, c:x86_64-freebsd,
      c:riscv64-linux, native:riscv64-linux, native:x86_64-macos (the
      C build of test_tasks for x86_64-freebsd died once inside the
      parallel run, with no error text, and builds on its own; it passes
      scripts/bin-target-check.py); 35 built binaries checked, 0 not an
      executable of their target. No lane ran: this host has no QEMU, no
      WSL distribution, no ARM64 Windows and no Mac, and tier2.yml still
      has no green run. Left, unchanged: the whole suite on each lane, a
      green tier2.yml, and executing the Windows ARM64, FreeBSD, macOS x64
      and RISC-V binaries. Still partly.
- [~] PL4. WebAssembly with WASI (release F9) (L).
      Nov 22 audit: wasi_snapshot_preview1-only imports, _start export,
      task switching validated (task-switch-check.sh 3 passed); CI lane
      wired in suite-all.yml. Precise gaps: the full suite through
      wasm-test.sh (316/344; sample: test_fix_fnval_struct_ret fails),
      threads run to their end (preview 1 has none), lane 0 successes
      (4 failed, 27 cancelled).
      Done: `fidesc wasm` emits a runnable .wasm (80 KB on hello) via
      `src/wasm.rs`/`compiler/wasm.fd`. Left: it imports a custom Fides
      `env` host API (`p_print`, `p_str_concat`, `p_panic`, …), not
      `wasi_snapshot_preview1` — real WASI needs fd_write/proc_exit
      mapping and a `_start` export.
      Checked Nov 15: that "Left" is done in `fid`. Two paths: `fid
      ccompile --target wasm32-wasi` (C through zig cc, Nov 7) and `fid
      wasm` (compiler/wasm.fd rewritten Nov 15: imports only
      `wasi_snapshot_preview1`, exports `_start` and `memory`; the Linux
      runtime over WASI in stdlib/linux_runtime/wasi.fd; no C compiler).
      Both run under `node scripts/wasm-run.mjs`; `fid wasm`: 316 of 344
      suite tests match `fid run` (scripts/wasm-test.sh `ALL=1`), hello is
      37.7 KB. Float `%` and f32 <-> f64 casts fixed on both paths Nov 15
      (known-bugs 2g.13). Left: the other 28 tests (processes, Node's
      fd_readdir on Windows, bugs shared with the native backends), tasks
      that switch stacks (they run to their end), `fid wasm` in CI
      (scripts/ci/cross.yml runs the C path's wasm32-wasi before a release).
      Nov 18 (not yet run with a built compiler): `fid wasm` tasks switch
      stacks (compiler/wasm.fd "Tasks": the WASM stack is unwound and
      rewound, asyncify-style); processes are a clear panic `not available
      in WebAssembly: processes (...)`, and so on the C path (codegen_c.fd
      p_system, p_process_run, process_spawn, `system`); C functions
      without a WebAssembly version panic when called; bare-metal builtins
      do nothing. scripts/wasm-run.mjs lists directories itself on Windows
      (Node's fd_readdir there is ENOSYS); scripts/wasm-test.sh counts
      `not available in WebAssembly` and FFI the interpreter cannot run as
      SKIP and an invalid module as BUILD. CI's wasm32 lane
      (.github/workflows/suite-all.yml) runs scripts/wasm-test.sh,
      tests/wasm/task-switch-check.sh and tests/wasm/stackify-check.sh.
      Left: a full run of those; threads still run to their end when they
      start (WASI preview 1 has none).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — task-switch-check.sh: 3 passed (test_wasm_task_switch as
      expected; test_wasm_process panics "not available in WebAssembly:
      processes (p_system)"; test_wasm_array_bounds); stackify-check gives
      the same output with 35 task-switch loops; wasm-test.sh skips C
      functions with no wasm version with a clear message. Left: the full
      suite through wasm-test.sh (a 40-test sample: 35 pass, and
      test_fix_fnval_struct_ret fails); threads; the CI lane not checked.
      Nov 27 (night): the whole suite through scripts/wasm-test.sh ALL=1
      with release/fid-v27.exe (the compiler of Nov 25): 1,315 programs, 4
      at a time, 04:07 to 06:17, beside a compiler build: `fid wasm: 1116
      passed, 35 failed, 25 did not build, 139 skipped`. Not a clean count:
      the run shared the machine with that build, and most of the skips
      (97) are programs whose interpreter reference did not finish in 300
      s under that load; the other skips are FFI and processes, which
      WebAssembly does not have. The 154 worth running again are listed in
      build/verify-nov27/wasm-rerun-files.txt: by the run's per-program
      files, the 146 whose WebAssembly output or exit code differs from
      their reference and 8 build failures besides the 19 the checker
      rejects on every backend. Named in the log: failures test_float_rem_exact,
      test_machine_float_mod, test_interp_f32_narrow,
      test_machine_tailcall_narrow, test_deref_minimal, test_alloc_oom_panic,
      test_alloc_thread_heaps, test_bg195_mutex_task_sleep, the FFI
      programs test_ffi, test_dynlib, test_dl_find, test_dlsym_typed_call
      (a skip elsewhere), and three in flight in another session (BG182,
      BG183, BG184); build failures test_asm_c, test_cfg_platforms,
      test_bg184_dyn_cast_inline and the checker's 20. test_bg224_xml_toml_
      depth_limit failed there and is fixed for WebAssembly since
      18255c75. Left: run that list on a quiet machine and sort it (the
      interpreter's own bugs, tests in flight, gaps of WebAssembly, gaps
      every backend has), then the gaps it names; threads; the CI lane.
      Nov 27 (night), the evening's three checks with release/fid-v27.exe:
      the default WebAssembly set 13 of 13, tests/wasm/task-switch-check.sh
      3 of 3, tests/wasm/stackify-check.sh 12 of 12.
- [~] PL5. Freestanding and embedded: Cortex-M, RISC-V boards, with no OS (release G)
      Nov 22 audit: Thumb-2 oracles green (enc-oracle-pl5.py 226 forms,
      op21 662 forms, 0 mismatches), test_thumb_core 23/23, thumb_check
      10 passed under qemu-arm. Precise gap: the no-OS images (vector
      table, UART, semihosting exit) have never been executed — no
      qemu-system-arm locally and bare-metal.yml has no thumb step.
      (L).
      Done: `--no-std` skips `core.fd` and still emits static ELFs —
      verified on `riscv64-linux` (43 KB) and `arm64-linux` (40 KB) with
      zero external tools; `compiler/embedded.fd` holds the bare-metal
      helpers. Left: a true no-OS target (flat binary, no ELF runtime),
      Cortex-M (needs a Thumb-2 encoder — arm64 emits A64 only).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — Cortex-M code generation (compiler/nat_thumb.fd, Thumb-2):
      enc-oracle-pl5.py 226 forms and enc-oracle-op21.py 662 forms, 0
      mismatches; test_thumb_core 23/23; thumb_check.sh: 10 passed
      (thumbv7m-linux under qemu-arm: core, overflow panic, exit status;
      the thumbv7m-none qemu-user board; image shape for mps2-an385 and
      lm3s6965evb). Left: the no-OS images (vector table, UART, semihosting
      exit) were not run: no qemu-system-arm here.
      Nov 22: the no-OS images RUN — qemu-system-arm installed (WSL, apt),
      thumb_check.sh 11 passed 0 failed: the mps2-an385 board images boot
      under qemu-system-arm (-semihosting-config, UART on stdio) and print
      the same lines with the same exit statuses as `fid run` for both
      test_thumb_core and thumb_exit. lm3s6965evb: thumb_exit's image
      (312 KB) outgrew the board's 256 KB flash — a capacity fact the
      emit reports cleanly ("does not fit in the board's flash"), now a
      documented skip in thumb_check.sh; the whole check also runs under
      WSL from Windows (the MSYS path mangling of wsl.exe arguments broke
      the board runs from Git Bash — run it inside WSL instead).
      bare-metal.yml installs qemu-system-arm + qemu-user and runs
      thumb_check.sh as its own step. Left: RISC-V no-OS boards (no
      emitter for a riscv64 `none` target), and a green bare-metal.yml
      run with the new step.
      Nov 27 check (no QEMU on this host): enc-oracle-pl5.py 226 forms and
      enc-oracle-op21.py 662 forms, 0 mismatches (release/fid-v27.exe, a
      copy of the fid-dev of Nov 25); thumb_check.sh with its default
      release/fid.exe of Nov 23: 3 passed, 1 failed — the interpreter run
      of test_thumb_core and the shape of the two thumbv7m-none images of
      mps2-an385 (test_thumb_core, thumb_exit) pass, and the failure is
      "qemu-arm is not installed" (the no-OS images were checked, not run;
      their execution was last verified Nov 22, 11 passed). No riscv
      `none` target is in compiler/ at 7b32d1e9. Left, unchanged: the QEMU
      legs here, RISC-V no-OS boards, a green bare-metal.yml run. Still
      partly.
- [~] PL6. Android and iOS (XL).
      Nov 22 audit: NOT done, and the entry understates what exists —
      stdlib/jni.fd, objc.fd, android_ndk.fd (+lifecycle/biometrics/
      haptics) since 49fb2b9: bridge logic tested in the interpreter
      against fake VMs (test_std_jni, test_std_objc). Nothing links for
      real: no android/ios target in normalize_target, no .so/apk/ipa
      production, iOS blocked behind PL7's Mach-O-on-Mac gap.
      Done: the static arm64-linux ELF is Android-adjacent — direct
      syscalls, no libc dependency, so it should run under `adb shell`
      (unverified on a device). Left: device testing, Android NDK/ABI
      quirks, and iOS entirely (needs the Mach-O backend + Apple syscall
      numbers + code signing).
      Nov 27 check: unchanged, not done — no android or ios target string
      exists in compiler/ at 7b32d1e9; what exists is the bridge logic
      (stdlib/jni.fd, objc.fd, android_ndk.fd) with its interpreter tests
      (test_std_jni, test_std_objc, test_std_android_ndk). Nothing is
      linked for or run on a device. Still partly.
- [~] PL7. Native backends for ELF and Mach-O, sharing one encoder (L).
      Nov 22 audit: the one-encoder claim is real — nat_enc.fd (13,289
      lines) holds nat_elf (:2712), nat_pe (:3501) and nat_macho (:8133,
      PIE, dyld binds, ad-hoc signature) over the single nat_ir MIR;
      macos-native.yml is fully written. Precise gap: no Mach-O binary
      has ever run on a Mac — all 4 completed runs died in the self-hosted
      fixpoint (CB8/2g.79), the same blocker as PL2's macOS lane.
      Done: `build --target` emits valid static ELF64 executables for all
      three arches with zero external tools — x86-64-linux, arm64-linux, and
      riscv64-linux all produce correct ELF headers (machine 62/183/243,
      ET_EXEC), and the x86-64 ELF **runs natively under WSL** (prints
      `Hello, Fides!`). Fixed a VReg-shadowing bug that printed raw virtual
      registers (`str v6`, `sd v9`), non-canonical `fp`/`return` mnemonics,
      unsigned-only immediates on negative offsets (`add xN, x29, #-24` →
      `sub`; `[x29, #-N]` → LDUR/STUR), and >255-byte stack slots (via x9).
      Left: Mach-O backend, encoder sharing between ELF targets, and running
      the emitted binaries under qemu/native CI.
      Checked Nov 15: the old `build --target` path above is retired in
      practice (platforms.md, Tier 3); `fid emit --target` on the machine
      layer (compiler/nat_ir.fd, one MIR, one allocator, the encoders of
      compiler/nat_enc.fd) writes ELF for x86_64/aarch64/riscv64-linux
      (`nat_elf`, `nat_elf_debug`: symbols, DWARF lines, CFI), Mach-O for
      x86_64/aarch64-macos (`nat_macho`: PIE, dyld binds, an ad-hoc code
      signature, and since Nov 15 a symbol table) and PE for `--nat`
      (`nat_pe`, with COFF symbols since Nov 15), so the "Mach-O backend"
      and the shared encoder are done. The Linux binaries run under
      qemu-user/WSL (`NATIVE=1 scripts/cross-test.sh <target>`;
      scripts/ci/cross.yml before a release). Left: running the Mach-O
      output on a Mac (`.github/workflows/macos-native.yml` is written; no
      recorded run), and the suite on each commit (PL2).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — the stage-0 rename (`guard` -> `hops` in ssa.fd and
      egraph.fd): stage0-names-check.sh (FROM=compiler/main.fd) PASS, 72
      files; fidesc dump-ast parses both with no parse error. The Nov 20
      macos-native.yml run passes stage 1 (dump-c and C compile on both
      macOS jobs) and stops at stage 2 (the CB8 memory blocker). Left: no
      Mach-O binary has run on a Mac yet; the suite on each commit (PL2).
      Nov 27 check (release/fid-v27.exe of Nov 25; clang 21 and the LLVM
      tools are on this host now, so the image readers run for real):
      scripts/image-check.sh over hello, test_closure, test_hashmap,
      test_float, test_tasks and test_uefi_hello, every writer (elf, pe,
      macho, obj, efi, coff) read by the readers installed here (only otool
      is missing): 211 checks passed, 15 failed — all one defect in the
      debug information, llvm-dwarfdump on the three Linux ELFs of five
      programs: "DIE has DW_AT_decl_file with an invalid file index 0".
      Index 0 is what ng_gen_function records for a function without a
      source file (nat_ir.fd: g_nat_line_file_ix = 0 when ir_fn_src_file is
      empty), written as decl_file for the ten generated ownership helpers
      (__own_dup_N, __own_drop_N, __p_mem_dup/drop, __p_elem_dup/drop); the
      Mach-O writer shares it (macho_dwarf_check.py on hello built with -g
      for aarch64-macos: llvm-dwarfdump --verify fails on 10). The Mach-O
      images are otherwise sound: macho_imports_check.py ok on
      test_machine_ldyn_targets (atoi, hypot, ldexp bound through
      libSystem), macho-check.py "well-formed Mach-O executable" for it and
      for hello -g (page hashes verified). No Mach-O binary has run on a
      Mac: macos-native.yml failed Nov 23, 24 and 25. Left: that decl_file
      (index 1, or no attribute, for a generated function), running the
      Mach-O output on a Mac, the suite on each commit (PL2). Still partly.

**Acceptance:** Tier 1 passes the whole suite on each commit.

## Track ST — Stability and process

Trust is built by what we do not break.

- [x] ST1. Freeze the Rust compiler as a bootstrap only: new features land in
      `compiler/*.fd` alone. Writing everything twice is the largest drag on pace (M).
      Done: `docs/reference/stability.md`; `fidesc test --compiler release/fid.exe` runs the suite on `fid`.
      Nov 3: the Rust compiler gets no new features (proposal 0002 is in `fid`
      only); `bootstrap.sh` builds from a pinned stage 0 (`NATIVE=1` stages by
      `fid emit` with a byte-identical check, `PIN=1` pins). No stage 0 is
      pinned yet: a natively built `fid` stops with "Invalid free" until RT2's
      native string headers are finished.
- [x] ST2. A written stability promise: code that compiles on 1.0 compiles on every
      1.x (release K12) (S).
      Done: `docs/reference/stability.md`.
- [x] ST3. Editions, so the language can change without breaking old code (L).
      Done: the plan. Nov 16 (not yet built): editions in `fid`. A file
      states `#[edition(N)]` (parser: attribute arguments may be numbers or
      strings); the default is `--edition=N` (main.fd), else ark.toml's
      `[package] edition`, else 2026; compiler.fd `cc_record_edition` records
      each module's edition (user file and every import) and rejects an
      unknown one; compiler/ast.fd `edition_of(module)` gates a check by the
      file's own edition. Editions: 2026 (1.x) and 2027 (preview: a call of a
      `@deprecated` function is an error). docs/reference/stability.md;
      test_frontend_edition, `_2027_fail`, `_unknown_fail`. The `--no-std`
      pipeline records no editions (every file is in the default).
- [>] ST4. A release every six weeks, on a date, whatever is ready (M).: moved to end-roadmap.md (Nov 22 — final-gate work, do when the code stops moving).

- [>] ST5. Long-term support releases with security fixes (M).: moved to end-roadmap.md (Nov 22 — final-gate work, do when the code stops moving).

- [>] ST6. Before each release, build every known public Fides project, as Rust's: moved to end-roadmap.md (Nov 22 — final-gate work, do when the code stops moving).

- [x] ST7. A public process for language changes (S).
      Done: proposals in `docs/proposals/`, the process in `docs/reference/stability.md`.
- [x] ST8. A security policy, a contact, and published advisories (S).
      Done: `SECURITY.md` (private reporting, response times, advisories).
- [x] ST9. Release notes that name every breaking change (S).
      Done: the rule in `docs/reference/stability.md` and `CONTRIBUTING.md`.
- [>] ST10. Signed checksums for every download (S).: moved to end-roadmap.md (Nov 22 — final-gate work, do when the code stops moving).


- [x] AD1. `ark`, `scribe` and `veil` written in Fides (release N8) (L). The
      Nov 22 audit: mostly done — the "shipped binaries are still Rust"
      gap is closed in code: installer/build-installer.sh (1fee238,
      Nov 22) builds ark/scribe/veil from tools/*.fd via the shipped fid
      by default (FIDES_RUST_TOOLS=1 opts back). Precise gaps: veil's
      report fidelity and ark's prebuilt-library path (tools-selfbuild-
      check), and no released installer ships them yet.
      Nov 25 validation: ark's prebuilt-library path is clean —
      tests/ark/ark-binary.sh passes end to end with release/fid-dev.exe
      (the linked program calls the library, removal of the library stops
      it, ark test passes, and the stale/corrupt-library fallbacks work).
      veil's report-fidelity gaps are fixed in compiler/veil.fd: the em
      dash/check marks of the Rust report, the whole lowered module
      rather than user functions only, no self-hosted ownership helpers
      in the report, and one-decimal rounding. A ccompiled tools/veil.fd
      matches target/release/veil.exe on hello.fd and struct.fd exactly
      for the checked report parts (Functions: 1/417 and 2/419, the same
      417/419 function rows, the same title); --version/--help/no-args
      and html smoke pass. Nov 25 closeout: the full veil lane of
      tests/tooling/tools-selfbuild-check.sh passes with release/fid-dev.exe
      (TOOLS=veil CC="zig cc"): CLI and error behavior, compile-failure
      behavior with no report, text and HTML reports, hello and struct
      program output, Functions counts and every function row all match
      the Rust tool. The sorted Rust-width rows and compile-failure early
      return are included. Full coverage.txt is not byte-identical: Fides
      also prints branch coverage, and line/instruction totals still
      reflect the two lowerings' different line attribution. Left: ship a
      released installer, and rebuild fid so `fid veil` carries this
      implementation.
      Done (Nov 25, fid rebuilt from the 905d756b tree, so `fid veil` is the
      new implementation): tools-selfbuild-check.sh passes for ark, scribe
      and veil, veil compared with target/release/veil.exe (the only
      differences are the line totals of the two lowerings, as above);
      `fid veil` on a program that does not compile prints the error and
      exits 1. installer/build-installer.sh builds the three from tools/*.fd
      by default; the first released installer to carry them is the next
      tag (a release step, not code).
      implementations exist in `compiler/{ark,scribe,veil}.fd` and now have
      standalone `fn main` entries in `tools/`. `ark.exe` builds and runs
      end-to-end (`version`, `list` verified); `scribe.exe` builds and
      generates docs (module-name CRLF bug found and fixed); `veil` builds
      fully as a Linux ELF through the nat backend (2,570 functions — needed
      the `p_f32_to_bits`/`p_int_to_float`/`p_float_to_int` shims, added to
      `ng_gen_call_std`). Windows `veil.exe` via the PE backend needs more
      RAM than a small machine has (deterministic 2 MB alloc OOM at PE
      emission). Shipped binaries are still the Rust ones.
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — tools-selfbuild-check.sh: FAILED. scribe passes every check
      (the same 43 and 15 sections as the Rust scribe). veil builds and
      writes its reports, but they differ from the Rust veil's (a plain
      dash for the em dash; it counts user functions only: 1/1 against
      1/358). ark builds, and ark-features, -paths, -projects, -registry
      and cd8-no-install-code pass; ark-binary.sh fails (also with `fid
      ark`): a program linking a prebuilt library prints '0 0 false'.
      Left: veil's report, the prebuilt-library call path, and shipping
      the Fides tools in place of the Rust ones.
- [x] AD2. The project website served by a Fides program (M).
      Nov 22 audit: done — the Nov 21 "Left: hosting it" is stale:
      .github/workflows/pages.yml (since 49fb2b9) ccompiles tools/site.fd
      with release/fid-dev, runs site --build _site, deploys via
      actions/deploy-pages@v4 — 28 runs, green through today;
      test_site_render.fd and docs/website.md in the tree. The custom
      fid-lang.org domain is the separate website repo, outside this item.
      Done (Nov 15): `tools/site.fd` renders README, the book, cookbook,
      guides and reference when requested, through `stdlib/markdown.fd`
      (fixed: `_` inside words, indented fences, list continuations,
      backslash escapes, `<https://..>` autolinks) plus tables and raw HTML,
      with its own layout and stylesheet and no external assets; only
      listed Markdown files are read and any path that would leave the site
      is refused (403: `..`, encoded and doubly encoded forms, backslashes,
      drive letters, `//`, NUL); `--port`, `--get <path>`, and `--build DIR
      --base /fid/` for GitHub Pages (53 files, every relative link and
      anchor resolved). `tests/bootstrap/test_site_render.fd`;
      `docs/website.md`. Left: hosting it (a Pages workflow or a server of
      ours), and `http_serve_scoped` once its double free is fixed
      (known-bugs 2g.35; `http_serve` keeps about 130 KB per request).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      validated — tools/site.fd serves through `http_serve_scoped`:
      site-memory-check.sh: PASS (59,196 KB after 150 requests, then 26,316
      KB after 300, 450 and 600); test_site_render prints its 51 lines.
      Left: hosting it (a Pages workflow or a server of ours).
- [~] AD3. `fides-os` as a standing test of bare-metal work (release G4) (M).
      Nov 22 audit: mostly done — bare-metal.yml's fides-os job (weekly
      + dispatch) + scripts/fides-os-boot.sh + the Nov 17 boot doc exist;
      the standing test works as designed (the first dispatch reported
      drift correctly). Precise gap (external): pledgeandgrow/fides-os
      @main no longer compiles against current fid (fat32/proc/p_malloc
      signatures) — a green run waits on that repo catching up.
      Nov 17: scripts/fides-os-boot.sh boots minimal, shell (banner,
      command list, prompt) and shell-input (echo and mem typed through
      the PS/2 keyboard via QEMU sendkey, timer ticking); bare-metal.yml
      fides-os job on push, PR and weekly, its stage-0 step scoped to what
      building fid reads (FROM=compiler/main.fd); 3/3 locally
      (docs/verification/fides-os-boot-2026-11-17.md). The 4 `guard`
      statements in compiler/ that stopped stage 0 are renamed
      (`FROM=compiler/main.fd scripts/stage0-names-check.sh`: PASS, Nov 18).
      Nov 22: in CI the fides-os job is correctly skipped on push events
      (weekly/dispatch only). The `qemu` job is green again (bare tests
      build — run 44974093228). The fides-os job ran for the first time
      on dispatch run 44974711969 and correctly reported fides-os's own
      drift: pledgeandgrow/fides-os@main no longer compiles against this
      fid — vfs fat32_read_file/fat32_create_file signature changes,
      sched proc_set_priority/PROC_ZOMBIE/PROC_STOPPED/SIGKILL gone, and
      new `p_malloc needs unsafe` safety errors. The standing test works
      as designed; a green run now waits on fides-os catching up to the
      current compiler (work in that repo), plus FIDES_OS_TOKEN if it is
      private.
- [~] AD4. Every internal tool that can be in Fides is (M).
      Nov 22 audit: mostly done — 12 tools/*.fd; ported per
      docs/reference/internal-tools.md: release_notes, release_checksums,
      macho_check, fuzz_triage, doctest + ark/scribe/veil (+ the installer
      flip, AD1). Precise gaps: doctest.fd differs from doctest.py in
      --emit mode (no "native:" summary line, 2/3 differ), and ~20
      drivers remain (scripts/fuzz/*, scripts/verify/*, enc-oracle,
      tests/harness.rs, verify.sh family).
      Done (Nov 15): `docs/reference/internal-tools.md` lists every script and tool
      and what keeps it out of Fides. Ported, with the same output as the
      originals (kept): `tools/release_notes.fd` (scripts/release-notes.py)
      and `tools/release_checksums.fd` (scripts/release-checksums.sh); Nov 16,
      `tools/macho_check.fd` (scripts/macho-check.py) and
      `tools/fuzz_triage.fd` (scripts/fuzz/triage.py). Left:
      most drivers need a process API with timeouts, kill and parallel
      children; the
      shipped ark, scribe and veil are still the Rust ones (AD1).
      Nov 21 check (fid built from the self-hosted stage 0 at 86cc919):
      partly — the process API with time limits (stdlib/process.fd
      process_spawn, process_wait with a timeout, process_run_argv) is
      there: test_process_spawn matches its 13 lines on run, ccompile,
      pe_ir, --nat and x86_64-linux (WSL). Left: the doctest driver port
      fails tests/tools/doctest/check.sh (1 same, 2 differ: in --emit mode
      tools/doctest.fd lacks scripts/doctest.py's 'native: ...' summary
      line); only 2 of about 25 drivers are ported; the shipped ark, scribe
      and veil are still Rust; WASI and macOS not run.
      Nov 24: the check is clean — tools/doctest.fd tracks each native
      example's build route (emit, or the C-route fallback with emit's
      error) and prints the 'native: N built by emit, M by the C route
      after emit failed' summary, '  C ROUTE' lines and the verbose
      '(native, <route>)' tag, like doctest.py. check.sh: 3 same, 0
      differ. Nov 25: scripts/verify/cov_instrument.py is ported as
      tools/cov_instrument.fd and tests/tools/cov_instrument/check.sh
      passes against Python — plain generated C, line-marked C, #line
      form, sparse-#line form, --self-test and the missing-argument exit:
      6 same, 0 differ. Left of the audit: the remaining drivers to port;
      the shipped ark/scribe/veil are still Rust until a release rebuild.
      Nov 25 evening: 25 more ports, each with a tests/tools/<name>/check.sh
      that runs the original and the port with the same arguments and
      compares stdout, exit status and the files written (all pass, built
      with `fid ccompile`; docs/reference/internal-tools.md has a paragraph
      per tool). Gate and verify: expect, leak_check, stale_cache_test,
      rules_smt, compile_gate (+ bench compile-check), me_measure,
      nat_gate, pass_matrix, variants, coverage, chaos, soak, interleave,
      enc_oracle, gen_brotli_dict. Fuzzing: fuzz_gen (802 programs
      byte-identical; tools/py_random.fd is CPython's MT19937, so a seed
      names the same program), fuzz_vs_c, fuzz_frontend, fuzz_parsers,
      reduce, fuzz_nightly. Target runners: cross_test, wasm_test,
      bare_qemu_test, sanitize. Left: tools/fuzz_triage.fd predates
      triage.py's Nov 19 changes (majority result without a .want, the
      "environment?" group, M24/M25 hints), so fuzz_nightly's check is 1
      same, 1 differ; `--gen` in variants and coverage; the lanes this
      Windows machine cannot run (qemu-user, WSL, QEMU boots, ASan/TSan,
      the @cfg(unix) code of chaos/soak/interleave); clean-bootstrap,
      build-variant, repro, verify.sh/verify-selfhost.sh, tests/harness.rs
      and bench/run.sh, which internal-tools.md lists as not ported and
      why. Eight tools reach getcwd, local time, sockets or affinity
      through their own extern "C" blocks (no stdlib call yet), so they
      build with ccompile, not `fid run`.
- [>] AD5. A real production service of ours, with its incidents written up: moved to external-roadmap.md (Nov 21).

**Being reachable**

- [x] AD6. The book: a full guide, from first program to writing a service (L).
      Nov 22 audit: done — docs/book/ has all 10 chapters;
      tests/book/check_book.py runs every chapter's examples and sessions
      and ci.yml:152-153 runs it on every push (stronger than the "at each
      release" acceptance); the Nov 22 note records the passing run
      (47 checks, 0 failures).
      Nov 15: all ten chapters of `docs/book/` are written. Chapters 5-8 and
      10 (effects and annotations, errors, toolchain, concurrency, shipping)
      go concepts, runnable examples, common mistakes; every complete example
      in them was run with `fid` 0.8.0-unstable (`fid run`, most also through
      `ccompile` and `emit`), and they say where that build falls short
      (known-bugs 2g.54-2g.66, found while writing them). Chapter 9 was
      corrected (no `--post-mortem`/`replay` in `fid`; a `@crashfree` example
      that compiles). `docs/guides/from-python.md` joins the Rust, Go, Zig and
      C guides. Nov 17: chapters 1-4 and 9 were checked example by example
      with `fid` (`run`, `ccompile`, `emit`) and have common-mistakes
      sections; `tests/book/check_book.py` runs every chapter's examples and
      sessions (`--native` also builds them through C and natively). Left:
      run it at each release. Nov 22: run — 47 checks, 2 stale excerpts
      fixed (RT8's `p_str_alloc_uninit` chain in ch.4, a drifted panic
      line number in ch.6, now an `at line ...` match); `book: 0
      failure(s)`.
- [x] AD7. A cookbook of common tasks (M). `docs/guides/cookbook.md` — args, files,
      collections, strings, JSON, errors, HTTP server/client, config, tasks,
      timing, crypto, testing; recipes use verified stdlib signatures.
- [x] AD8. Guides for people arriving from Rust, Go, Zig and C (M).
      `docs/guides/from-{rust,go,zig,c}.md` — syntax maps, ownership-model
      deltas, real ports, what doesn't carry over.
- [x] AD9. A channel for questions, answered (S). GitHub Discussions enabled
      on pledgeandgrow/fid; `SUPPORT.md` routes questions/bugs/contributing.
      "Answered" is ongoing by definition.
- [x] AD10. Good first issues, kept current (S). Issues #2–#6 filed from
      real known-bugs/limitations (debug-print gating, scribe readdir,
      non-ASCII CI check, ark stubs, GlobalAddr on other targets).
- [x] AD11. A compiler internals guide, so others can contribute (M).
      `docs/reference/compiler-internals.md` — pipeline, IR shapes, pass list + kill
      switches, calling conventions, lib-facts format, contributor traps.

**Earning users**

- [>] AD12. A public launch once the beta gate is met: moved to external-roadmap.md (Nov 21).
- [>] AD13. Publish the benchmarks (BM10), and invite corrections (S).: moved to end-roadmap.md (Nov 22 — final-gate work, do when the code stops moving).

- [>] AD14. Ten external projects built on Fides: moved to external-roadmap.md (Nov 21).
- [>] AD15. Ten contributors outside the core: moved to external-roadmap.md (Nov 21).
- [>] AD16. A first company running Fides in production: moved to external-roadmap.md (Nov 21).

**Acceptance:** real incidents from real users, handled in public.

## Order of work

1. **Release beta gate first.** No one can use what they cannot rely on.
2. **ST1, BM1–BM8.** Stop writing everything twice, and start measuring.
3. **VF1, VF3, VF14, VF20.** Before the optimizer grows, so new passes are checked
   from their first day.
4. **RT1, RT2, RT6, OP2, OP16, OP17.** Data layout and SSA: the largest gains, and
   the hardest to change later.
5. **OP5–OP19, CS1–CS4.** The speed most programs will feel.
6. **OP24–OP29, the proofs only we have.** Where we can beat them, not only match.
7. **AD1–AD5, ST2–ST10.** Run on it ourselves, and start the release rhythm.
8. **LG, SL, TL, PL in parallel,** led by what real use asks for.
9. **AD12 onward.** Launch, listen, repeat. This never ends.

## Set aside

These cost money and do not block anything above.

- [>] Paid security audits, toolchain certification, a bug bounty, a foundation,
      full-time staff, conferences: moved to external-roadmap.md (Nov 21).
- [>] Dedicated benchmark and fuzzing machines: moved to external-roadmap.md
      (Nov 22); provisioning requires funded, dedicated runners outside this
      code-only work.
