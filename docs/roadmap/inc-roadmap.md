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
> parity verified on fid. Nov 15 (fid): unit objects keyed so exported
> names count (a rename reused a stale object), `a::b` modules build,
> literals scoped per unit, generic method instances placed with their
> generic; B4 decided, B2/B3/B5 partly, `scripts/inc-check.sh` for B8/B9
> (not run yet). B1 not started.

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
- [x] A3. Per-*file* source hashes were already in the stamp; per-module
      *C* hashes came with Phase B's C-level split: under `--inc` each
      module's unit is hashed on its own and the hash keys its object in
      `.fidcache/` (fid: `compiler/main.fd` `build_c_units` →
      `compiler/inc.fd` `inc_unit_hash`, Nov 15; fidesc: `canon_hash` per
      unit in `src/main.rs`). Without `--inc` there is still one TU and one
      hash.
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
- [x] A9'. Dead functions can't be dropped either: every function is
      `p_thread_register`ed (spawn-by-name), i.e. address-taken, so there
      are no dead functions in Fides C output. The pruning code is kept —
      it fires if codegen ever stops registering unused functions.
      Checked Nov 15: still so in fid (`compiler/codegen_c.fd`
      `emit_main_wrapper` registers every function, and the `__p_fns`
      panic-trace table takes every function's address too). Left (not in
      the tooling files): register only functions a `ConstFnRef`/spawn
      names, and list only live functions in `__p_fns`, in codegen_c.fd.
      Done Nov 16 (codegen_c.fd): C main registers only the functions the
      program names as a string constant or a function reference (every
      lookup by name gets its name from one: spawn, async, p_co_go,
      p_executor_spawn), and `__p_fns` lists only what `cg_compute_live`
      reaches from main, those names, C callbacks and the runtime's own
      callees through calls, references, strings and dynamic calls. A dead
      function's address is no longer taken. Decision: a name built at run
      time is not seen; `set_c_register_all(true)` keeps the old tables.
      test_cback_live_fns.fd.
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

Found Nov 15 (fixed in fid; fidesc is frozen and keeps them):
- A unit's object was keyed by the *canonical* hash, which alpha-renames
  every function the unit defines. Only the owner unit holds strings that
  name its functions (`__p_fns`), so renaming an exported function of
  another module reused its old object (link: undefined symbol), and two
  modules whose code differed only in function names shared one object
  (link: duplicate and undefined symbols). fid keys objects by
  `inc_unit_hash` (`compiler/inc.fd`): non-`static` definitions keep their
  names and are always live.
- Unit files were named after the module (`compiler::lexer.c`): `:` is not
  allowed in a Windows file name, so `--inc` failed on any program with
  `a::b` modules (the compiler itself). Files are now `<index>_<sanitized>`.
- String literals (`static const struct {..} _strN_h` + `#define _strN`,
  since D16) stayed in the shared prelude, so every literal edit rebuilt
  every unit; only the old `static const char _strN[]` form was scoped.
  `split_c_units` (`c_str_decl_key`) now gives each unit only the literals
  it names. A literal added in one module still renumbers the later
  modules' literals (`_strN` is a whole-program number), so those rebuild.
- fid's `Compiling n of m units` did not name the units (fidesc did); it
  does now, and `FIDES_INC_DEBUG=1` prints the per-unit proto table on fid
  too (C1).
Checks: `scripts/inc-check.sh` (fixture: cold build with two same-body
modules, a one-unit body edit, an exported-fn rename, a new literal,
the debug table; `SUITE=1` for B8, `FIXPOINT=1` for B9). Not run yet
(written Nov 15, no build in that session).

- [~] B1. Per-module IR boundary: `lower()` emits one `IrModule` per source
        module plus a link table (exports, extern decls, struct layouts).
        Checked Nov 15: not started. Lowering, mono and the IR are
        whole-program; module provenance exists only as a name → module
        table (`compiler/compiler.fd` `record_func_modules`) used to split
        the generated C.
        Nov 16, lowering side: decided to cut the lowered program rather
        than lower each module alone (ownership helpers, layouts and generic
        instances are whole-program, and mono's instances already have a
        home module, B3). `ir_units_split` (lower.fd) makes one `IrUnit` per
        source module: its functions with the shared tables, exports,
        imports (calls and function references another unit defines), a
        content key over its ir_ser bytes; the `IrLinkTable` adds the owner
        of each function and the layout-table hash (B2). `ir_units_check`
        and `ir_units_link` (the identity, function for function in the
        original order). Behind `FID_IR_UNITS=1` on every command that
        lowers through `lower_to_ir_module_with_path` (`=<dir>`: each unit
        as a `.fir` file plus `link.txt`); `test_lowering_ir_units.fd`.
        Left: nothing consumes the units yet: `--inc` still splits the
        generated C text (`split_c_units`); next is codegen per unit keyed
        by `IrUnit.key`, and a per-unit layout table (B2).
- [~] B2. Cross-module type info: struct layouts and enum tags must be
        identical across module objects — export a canonical layout table,
        hash it, include in the module's dep key.
        Done at the C level, by construction: every unit carries the same
        shared prelude (struct typedefs, enum layouts, extern decls) and
        that text is part of each unit's object key, so objects can never
        disagree on a layout. Scoped per unit (split_c_units: struct
        blocks and FFI declarations a unit names, transitively;
        FIDES_INC_LAYOUTS=off). The __eq_<S>/__eq_<Enum> helpers go to
        their type's module unit (record_func_modules; FIDES_INC_EQ=main:
        old placement). Tests: tests/tooling/inc-layouts-check.sh,
        inc-layouts-ffi-check.sh. At the IR level (Nov 17, lower.fd
        `lw_unit_layout_key`): each `IrUnit` has a `layout_key`, the hash of
        the layouts its functions use (value, parameter and return types,
        Alloca/GetFieldPtr/GetElemPtr/Load/Store types, closed over the
        fields), in struct_layouts order; `link.txt` prints `layouts <unit>
        <hash>` per unit, and the whole table's hash stays the cross-unit
        check. Left: #include/PLINK lines are shared (the first extern of a
        new header rebuilds every unit), and nothing keys an object on
        `layout_key` yet (the C per IrUnit hashes its own C text).
- [~] B3. Monomorphization policy: generic instances are emitted in the
        *caller's* module unit (like C++ templates) — an instance's key
        includes the generic's source hash + the concrete type args.
        Done (Nov 15, `func_module_of` in `compiler/compiler.fd`): every
        instance goes to the unit of the module that defines the generic:
        `f__T` as before, and now also method instances `S__m__T` (mono.fd
        `mangle_method`; before, they fell to `main`'s unit, the one that
        is edited most). Each instance is defined once, so no weak/COMDAT
        linkage is needed (tcc and cl do not share one), and the unit's
        content key holds the instance's code, so it changes exactly when
        the generic's body or the type arguments do.
        Policy (Nov 17, c_copy_instances_to_callers, docs/inc-instances.md):
        an instance only one other unit calls moves there. One that two or
        more units call, and that its generic's unit does not, has its
        external definition in the calling unit first by name and a
        `static` copy in each other calling unit; its instance callees are
        placed the same way. An address-taken instance (a function value,
        any use but a call or its __p_fns row) and one the generic's unit
        calls stay put. FIDES_INC_INSTANCES=move|home compare. Cost: a
        frame in a static copy is unnamed in panic traces. Test:
        tests/tooling/inc-instance-copies-check.sh.
- [x] B4. Whole-program opt boundary: keep `optimize_l2` whole-program for
        `fid build` (correctness over speed); add `--inc` mode running
        per-module opt only, documented as producing slower binaries.
        Realized (checked Nov 15): `fid build`/`emit` run the IR optimizer
        whole-program and have no `--inc`; the C path runs no IR optimizer
        (`compiler/compiler.fd` `ccompile_source_with_path`), so with and
        without `--inc` the C is the same and the only optimizer that sees
        less is the C compiler: one TU optimizes (inlines) across modules,
        `--inc` units only within a module, so `--inc` binaries can be
        slower. Decision for the deferred conflict below: `--inc` for dev
        builds, whole-program for release.
- [~] B5. Backend split: C codegen and PE codegen emit one object per
        module; linker merges. PE path needs an object format or a
        late-link step inside `pe_ir` — design decision recorded here.
        C half done: `split_c_units` + `build_c_units`, one `.o` per
        module, then one link. Decision for native: a late-link step inside
        the backend at function granularity rather than an object format —
        nat_ir already has it (CS6 `FID_FNCACHE`: a function's bytes before
        fixups plus symbolic fixups, replayed and relinked at the new
        offset; covers Linux targets and `--nat` Windows). Left: pe_ir (the
        default Windows backend; `compiler/pe_ir*.fd`, not the tooling
        files) has no such step.
        Nov 16, pe_ir half written (not run yet): with FID_FNCACHE=<path>
        `fid emit` keeps in `<path>.pe` each function's code before its
        labels and .rdata references are resolved, the references by name
        (function, panic stub, string text, global, import slot, allocator
        state), keyed by the function's IR (`iw_irfunc`) and a salt (the
        compiler's hash, struct layouts, globals, externs, every labelled
        name, C overrides, cold functions, FID_PE_OFF). A hit copies the
        bytes to the function's new place and registers its references
        again; the image is linked afresh, and comes out byte-identical to
        a build without the cache (pe_ir.fd `PeirGen_gen_cached`,
        `PeirGen_fc_*`). Not cached: @interrupt, @constant_time, asm!, dyn
        values, calls to undefined functions, debug builds.
        `FID_PE_FNCACHE_STATS=1` prints hits and misses. Check:
        scripts/pe-win-check.sh. Left: on by default; a run on the suite.
- [~] B6. Stdlib ships prebuilt: warm-cache story exists via `.fidcache`
        + `FIDES_CAS` (A11) — a seeded store or remote CAS serves stdlib
        objects with zero compilation. Installer does not yet bundle a
        `.fidcache/` for the stdlib modules.
        Checked Nov 15: a bundled store would rarely hit. A `core` unit's
        key includes the whole program's shared prelude (every struct and
        extern of the program) and the instances of core's generics the
        program uses, so it is the same only across programs with the same
        types. Left: scope the prelude per unit (B2), then bundle
        (`installer/`, not the tooling files).
        Nov 16 (ci-release): still waits on B2. Besides, an installed fid has
        no store to read a bundle from: `.fidcache` is found beside the
        output (inc.fd `inc_dir_of`), then `FIDES_CAS`. The plan once B2
        scopes the prelude: a read-only seeded store `<fid dir>/cache/`
        looked up after the local one in `inc_artifact_get`, filled by
        installer/build-installer.sh with `--inc` builds of the examples
        and installed by fides.nsi.
        Nov 17: written. The store (`<fid dir>/cache/`, inc.fd
        `inc_seed_get`/`inc_seed_put`, after the local `.fidcache` and
        before `FIDES_CAS`; FIDES_SEED_STORE=<dir>|off) and `ccompile --inc
        --seed-store <dir>` to fill it; the unit key no longer holds the
        input/output names, `--inc`, `--seed-store`, CC or FID_HOME, and a
        unit other than main's carries only the globals and C functions it
        uses (docs/inc-seed-store.md, tests/tooling/inc-seed-store-check.sh).
        installer/build-installer.sh fills build/installer/cache from hello
        and the examples (once per C compiler on PATH: an object's name
        holds the C compiler) and fides.nsi installs it as `$INSTDIR\cache`
        (section "Prebuilt standard library objects"). Left: run the check
        and an installer build.

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
        Nov 15: `SUITE=1 scripts/inc-check.sh` builds every top-level
        `tests/bootstrap/test_*.fd` (not expected-fail) single-TU and
        `--inc` with fid and compares program output and exit code (tests
        whose output depends on time or threads may differ both ways).
        Nov 17: two split bugs that broke every program with a
        module-level global under `--inc` are fixed: main's unit lost the
        globals' initializers (`var x: i64 = 7` read 0), and the `static`
        `__init_globals` was copied into every unit's prelude ahead of the
        globals it names (docs/inc-seed-store.md § B8,
        tests/tooling/inc-globals-check.sh).
        Left: run it (not run: no build in that session); record the
        SUITE=1 counts here once it has run.
- [~] B9. Bootstrap fixpoint still holds: per-module self-compile produces
        the same `fid.exe` (hash-identical artifacts).
        Linked executables cannot be byte-identical (B8), so the check is
        on what can be: `FIXPOINT=1 scripts/inc-check.sh` builds the
        compiler with `ccompile --inc`, requires that compiler's `dump-c
        compiler/main.fd` to equal `$FID`'s, and requires a second `--inc`
        self-build to reuse every unit's object ("All N units up to
        date"). Before Nov 15 `--inc` could not build the compiler on
        Windows at all (`compiler::x` unit file names). Left: run it.

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
        `unit -> n of N protos` (the edge table, inspectable). On fid the
        print was missing until Nov 15 (`split_c_units` now prints
        `[inc] unit <name>: n of N protos`, as fidesc).
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
  decision when B4 is done. Recorded Nov 15 (B4): that is the decision.

## Notes

- Order matters: A before B before C. A is useful standalone; B is the
  structural change; C is only worth it once per-module units exist to
  be selectively invalidated.
- The `.fides-stamp` format is versioned (`fides-stamp 1`); A5 bumps it.
- Reference points: Bazel content-addressed actions (Phase A),
  rustc/salsa red-green edges (Phase C's dep-graph versioning, C3),
  Unison semantic content addressing (A1's canonical form idea).
