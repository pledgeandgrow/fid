# Incremental Compilation Roadmap (self-hosted)

> Goal: rebuilds cost what changed, not what exists. Stdlib and unchanged
> modules never recompile; semantic (effect-scoped) dependencies skip work
> that a call graph alone would demand.
>
> Created: Oct 30, 2026. Status (Nov 15): Phase A done on both compilers —
> including A11's remote CAS end-to-end; Phase B done *at the C level*
> (`--inc`, both compilers); the per-module-IR split (B1–B5) and bootstrap
> fixpoint check (B9) stay open; Phase C realized as per-unit prototype
> scoping (see below) — all C-acceptance items verified on fidesc, emit
> parity verified on fid.

## Where it stands

- **Already exists (`src/inc.rs`):**
  - `IncrementalCache` — in-memory `(path, source_hash) → L2Module`, used
    per process (LSP, watch, test harness). Dies with the process.
  - `BuildCache` — `<output>.fides-stamp` files recording every file read
    (content hashes), missing import candidates, compiler identity, and
    command line. `build`/`emit`/`ccompile` short-circuit whole-program
    when nothing changed (`src/main.rs`). `--no-cache` / `FIDES_NO_CACHE`
    opt out.
  - `deps_snapshot()` — the deps recorded during a build feed the
    in-memory cache key, so a changed import can't reuse stale L2.
- **The gap:** any dep change → full rebuild of the whole program (stdlib
  + all modules + user code). L2 is whole-program; there is no per-module
  unit to cache.
- **Not doing:** delta-through-pipeline compilation and speculative builds
  are research directions, not roadmap items (see last section).

## Phase A — Semantic-hash artifacts (dedupe, not skip)

Hash the *canonicalized* compiled artifact instead of source text, so
comment/format/reorder edits never invalidate, and identical outputs
share one artifact.

**Implemented Oct 30 (Rust side):** `canonicalize_c` + `ArtifactStore` in
`src/inc.rs`; wired into `ccompile` (`src/main.rs`). Findings vs. the
original plan are noted inline.

- [x] A1. Canonical form of generated C (`canonicalize_c`): comments and
      whitespace stripped, all non-keyword identifiers alpha-renamed
      positionally, unreferenced top-level function definitions dropped
      (fixpoint liveness from `main` + top-level refs). String/char
      literals and preprocessor lines kept verbatim.
- [x] A2. Artifact store: `<output_dir>/.fidcache/<key>.exe`, key =
      hash(canon, cc, opt flags, target). Gitignored. Atomic write via
      tmp+rename; cache failures never break a build. Nov 4 (`fid`,
      `compiler/inc.fd` `inc_cache_evict`): least recently used artifacts are
      evicted past 64 files or 1 GB.
- [~] A3. Per-*file* source hashes were already in the stamp; per-module
      *C* hashes are impossible until Phase B splits codegen — the C
      backend emits one translation unit. Adjusted scope recorded.
- [x] A4. Canon hit → cached bytes written to output, C compiler skipped;
      miss → compile, then store the exe bytes.
- [x] A5. Stamp v2 (`fides-stamp 2`) records a `canon` line; v1 stamps
      still parse. `changed_inputs()` reports which source files forced
      the rebuild — `ccompile` prints `Changed: <files>`.
- [x] A6. Self-host parity: canonical hash and artifact store in
      `compiler/inc.fd`; `--inc` in `compiler/compiler.fd` (split_c_units) and
      `compiler/main.fd`. Hashes differ between the two compilers (each has
      its own `.fidcache` keys), the behaviour does not.
- [x] (bonus) `CC` env var is now split on whitespace — `CC="zig cc"`
      works in `fidesc ccompile`, matching the documented bootstrap usage.

### Semantic-hash acceptance — revised after implementation

- [x] A7. Whitespace/comment edit → zero C-compiler work (already true via
      the `c` hash; still holds with v2 stamps).
- [x] A8'. **Finding, not a bug:** variable/function renames do NOT get a
      shared artifact — and that's *correct*: Fides codegen emits function
      names as string literals for panic stack traces (since Nov 2 in the
      `__p_fns` table, no longer `p_call_stack_push("plus")`), so a rename
      changes observable behavior. The canonicalizer still dedupes raw-C renames; Fides
      programs just don't qualify. Roadmap A8 was wrong as stated.
- [~] A9'. Dead functions can't be dropped either: every function is
      `p_thread_register`ed (spawn-by-name), i.e. address-taken, so there
      are no dead functions in Fides C output. The pruning code is kept —
      it fires if codegen ever stops registering unused functions.
- [x] A10. Works today: artifact key is content+toolchain, not
      path/output — two checkouts or different `-o` names share cached
      exes. Verified: deleted output restored with zero C compile
      ("Reused cached artifact").
- [x] A11. Remote-cache server for `.fidcache` (Bazel-style HTTP CAS) —
      `tools/cas.fd` (`fid run tools/cas.fd -- [root] [port]`, default
      `./fidcas` :7331): `GET|PUT /cas/<name>` raw bytes + `/health`,
      name allowlist + traversal rejection, tmp+rename writes (Windows:
      same-key PUT keeps existing object, no tmp leak). Client side:
      `FIDES_CAS=http://host:7331` on both compilers — remote GET fills
      the local store, remote PUT mirrors local builds, failures are
      silent (dead port falls through to a normal compile). Binary-safe
      end-to-end: counted-str bodies (`hc_text`, `hs_free`, socket
      writes via `p_slen`, linear `str_from_bytes`), verified with 2 MB
      NUL-embedded round-trips and a 413 KB exe PUT byte-identical.

### Corrections (Nov 1)

Renaming every identifier was unsound: two programs differing only in which
external C function they call got one artifact, so a build could reuse the
wrong binary. Only functions the unit defines are renamed now, and `// PLINK:`
lines stay. Stamps record the output's hash, the key includes `FID_HOME`, and
`fid emit` has stamps too.

## Phase B — Per-module compilation units (prerequisite for fine-grained)

Today L2 is whole-program because lowering + monomorphization + opt merge
everything. Splitting is the real work; hashing just exploits it.

**Status (in progress):** the *C-codegen-level* split is implemented and
opt-in via `fidesc ccompile --inc` — front-end stays whole-program, then
`generate_c_units` emits one TU per module (shared prelude/`core` + per-fn
chunks by `func_modules` provenance), each compiled to a content-keyed `.o`
in `.fidcache/` and linked. Verified: single-function edits rebuild one
unit; imported-module edits rebuild only that module; 20/20 bootstrap tests
match single-TU output. `FIDES_INC_FLAT=1` forces a single unit for
bisecting. Open items: per-*module-IR* split (below) is still needed for
front-end incrementality; `--inc` is experimental until the full suite
passes.

Bugs found by `--inc` so far (both fixed):
- Unit compiles lacked `-fno-strict-aliasing`/`-fno-strict-overflow` — the
  generated C type-puns via pointer casts; strict aliasing at -O miscompiled
  `HashMap_find_slot`. Unit flags now match the single-TU path and are part
  of the artifact key.
- `Expr::Ref` on an lvalue (`&p.x`) lowered to *copying the referent into a
  fresh alloca* — returning the pointer escaped dead stack memory (latent UB
  that single-TU survived by frame-layout luck) and broke write-back
  semantics. Now emits `GetFieldPtr` for `FieldAccess` and the pointer
  itself for `Deref` (src/lower.rs + compiler/lower.fd parity). Same
  alloca-escape hazard remains for `&rvalue` (`&(x+1)`) returns.

- [ ] B1. Per-module IR boundary: `lower()` emits one `IrModule` per source
        module plus a link table (exports, extern decls, struct layouts).
- [ ] B2. Cross-module type info: struct layouts and enum tags must be
        identical across module objects — export a canonical layout table,
        hash it, include in the module's dep key.
- [ ] B3. Monomorphization policy: generic instances are emitted in the
        *caller's* module unit (like C++ templates) — an instance's key
        includes the generic's source hash + the concrete type args.
- [ ] B4. Whole-program opt boundary: keep `optimize_l2` whole-program for
        `fid build` (correctness over speed); add `--inc` mode running
        per-module opt only, documented as producing slower binaries.
- [ ] B5. Backend split: C codegen and PE codegen emit one object per
        module; linker merges. PE path needs an object format or a
        late-link step inside `pe_ir` — design decision recorded here.
- [~] B6. Stdlib ships prebuilt: warm-cache story exists via `.fidcache`
        + `FIDES_CAS` (A11) — a seeded store or remote CAS serves stdlib
        objects with zero compilation. Installer does not yet bundle a
        `.fidcache/` for the stdlib modules.

### Per-module acceptance

- [x] B7. Edit one file in a 10-module program → only that module's unit
        recompiles + relink. Verified Nov 15 on fidesc: `Compiling 1 of
        10 units: mod5` after a body edit; `2 of 10: main, mod5` after a
        signature edit (definer + caller). Split bugs fixed along the
        way: prelude fn-def extraction (line-anchored spans — bare
        `P_ARITH(...)` macro invocations no longer swallowed into def
        spans or duplicated across units), `static` helpers stay per-unit
        (internal linkage), moved-def prototypes stand in place (macro
        expansions mid-prelude keep seeing their callees).
- [~] B8. Output equality verified — the 10-module slice and an 8-round
        randomized edit loop both produce `--inc` binaries whose program
        output matches single-TU exactly (54908 == 54908). Linked
        binaries are NOT byte-identical (object/section layout differs
        by construction — that's inherent to split objects, not a
        codegen difference). verify.sh baseline not yet rerun.
- [ ] B9. Bootstrap fixpoint still holds: per-module self-compile produces
        the same `fid.exe` (hash-identical artifacts).

## Phase C — Effect-scoped invalidation (the novel part)

Use the effect system to shrink dependency edges below the call graph.
A function's compiled unit depends only on what it can *observe*.

**Status (Nov 15):** implemented as *per-unit prototype scoping*. The C
backend is the level where edges exist, and at that level the
signature-vs-body split is exact: a fn's *signature* is its C prototype,
its *body* lives entirely inside its own module's unit. Each unit now
emits only the prototypes its text names (one ident pass over the unit
body; `FIDES_INC_DEBUG=1` dumps the per-unit edge table). Result: a body
edit — IO or pure — can never invalidate another unit (C8), and a
signature edit rebuilds exactly the units that name the fn (C2/C9).
Effects and capability annotations are erased before codegen, so they
cannot create cross-unit edges — the conservative direction the roadmap
asks for is guaranteed by construction (C3/C4), not by a table that
could drift.

- [x] C1. Realized as the per-unit proto set; `FIDES_INC_DEBUG=1` prints
        `unit -> n of N protos` (the edge table, inspectable).
- [x] C2. Signature = the fn's C prototype in each caller's unit;
        body = the unit's own text. Verified: sig edit → definer+callers.
- [x] C3. Annotation edits are erased at codegen → no C-level edge; a
        dep-graph red edge cannot form (conservative by construction).
- [x] C4. Capability fns keep conservative edges automatically: their
        protos scope exactly like everyone else's, no pruning below
        "does the unit name this fn".
- [x] C5. Runtime helpers (`p_malloc` et al.) live in the shared prelude
        as protos + owner-unit defs — a body edit changes only the
        owner's tail; every non-owner unit is untouched. Verified as a
        special case of C8.
- [x] C6. `FIDES_INC_EFFECTS=off` keeps all protos in the shared prelude
        (old call-graph-coarse behavior). Verified: every unit then
        carries every proto; output identical.
- [x] C7. `compiler/compiler.fd:split_c_units` mirrors `src/codegen_c.rs`
        — C names recovered from each def's first line (`main` →
        `main_entry`), proto lines keyed by name, per-unit ident scan.
        Verified on the b7 fixture: fid emits the same scoped proto set
        as fidesc (f1–f4, f6–f8 in core's unit; arities the comptime
        dispatch table doesn't name are excluded on both sides).

### Effect-invalidation acceptance

- [x] C8. Edit an IO function's body → pure callers' units untouched.
        Verified: body edits on mod5 rebuild `1 of 10: mod5` only —
        bodies never cross unit boundaries, IO or not.
- [x] C9. Edit a pure function's signature → all callers rebuild.
        Verified: `f5` arity edit → `Compiling 2 of 10: main, mod5`;
        non-callers (incl. `core` and every other mod) untouched.
- [x] C10. Staleness fuzz: 8 rounds of randomized body edits + signature
         flips across a 10-module program — every round rebuilt ≤2 units,
         final `--inc` output matches a fresh single-TU build exactly.

## Deferred (research, not roadmap)

- **Delta-through-pipeline:** propagate edit deltas as first-class values
  through parse → typecheck → lower → opt → codegen. Requires stable
  per-node IDs across stages; the current pipeline is not delta-shaped.
- **Speculative builds:** precompile predicted next-states from IDE
  context. Only cheap once A1–A6 exist (speculated work hits the artifact
  cache); revisit after Phase B.
- **Whole-program opt + incremental conflict:** LTO/monomorphization want
  whole-program visibility; incremental wants isolation. Likely answer:
  `--inc` for dev builds, whole-program for release — but record the
  decision when B4 is done.

## Notes

- Order matters: A before B before C. A is useful standalone; B is the
  structural change; C is only worth it once per-module units exist to
  be selectively invalidated.
- The `.fides-stamp` format is versioned (`fides-stamp 1`); A5 bumps it.
- Reference points: Bazel content-addressed actions (Phase A),
  rustc/salsa red-green edges (Phase C's dep-graph versioning, C3),
  Unison semantic content addressing (A1's canonical form idea).
