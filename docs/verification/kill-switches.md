# Kill switches (own-backend roadmap RL19)

Every optimization pass and rule group of the self-hosted compiler can be
turned off for one build, without rebuilding the compiler, so a user can
work around a miscompile on the day it is found and a developer can bisect
it to one pass. The switches are environment variables holding a comma
list of names. They are part of the build-cache key (`compiler/inc.fd`,
`compiler/nat_ir.fd` `nc_start`), so a build with a switch never reuses
code built without it.

    FID_IROPT_OFF=licm,gvn fid emit prog.fd -o prog     # two mid-end passes off
    FID_NAT_OFF=rules fid emit prog.fd --target aarch64-linux -o prog
    FID_IROPT_ON=all FID_IROPT_OFF=sra fid emit prog.fd -o prog

To find the pass behind a wrong result: build the program with each switch
of the tables below off in turn (`scripts/pass-matrix.sh <target> prog.fd`
does it for the native targets) and look for the one that makes the output
right; then shrink the program (`scripts/fuzz/reduce.py`) and log it
([miscompile-log.md](miscompile-log.md)).

`scripts/switch-check.sh` reads the compiler's source for every switch it
tests (`iro_pass_on`, `ime_on`, `ng_on`, `ru_on`, `eg_on`, `ru_opt_in`) and
fails when one is missing from this page, or when this page names one the
compiler no longer reads; CI runs it on every push. `scripts/pass-matrix.sh`
takes its lists from the same place.

Which paths read them: the mid-end (`compiler/iropt.fd`, `iropt_me.fd`,
`rules.fd`, `egraph.fd`) runs on the native paths (`fid emit`: pe_ir on
Windows, nat_ir for the Linux and macOS targets and `--nat`). The C path
(`ccompile`, `dump-c`) and the interpreter (`run`) run no IR optimizer, so
none of these switches changes them.

The `--no-pass=a,b` flag (or `--no-pass a,b`; `compiler/checks.fd`
`iro_cli_off`) adds names to the off lists that `iro_named_off` reads:
the mid-end's, the rules' (`ru_on`) and the e-graph's (`eg_on`). nat_ir's
own `ng_on` reads `FID_NAT_OFF` only.

## Mid-end passes, on by default: `FID_IROPT_OFF`

| Switch | Pass (compiler/iropt.fd unless named) |
|--------|---------------------------------------|
| `FID_IROPT_OFF=closuredevirt` | closure-environment indirect calls resolved to direct calls before inlining (LG5/LG6) |
| `FID_IROPT_OFF=hofspec` | higher-order functions cloned per known closure (LG5) |
| `FID_IROPT_OFF=inline` | inlining of small functions (`IRO_INLINE_MAX`) |
| `FID_IROPT_OFF=fold` | constant folding |
| `FID_IROPT_OFF=dce` | dead-code elimination |
| `FID_IROPT_OFF=tailcall` | self tail calls become loops (also off when `IRO_TAILCALL` is false) |
| `FID_IROPT_OFF=gvn` | global value numbering (also `IRO_GVN`) |
| `FID_IROPT_OFF=licm` | loop-invariant code motion (also `IRO_LICM`) |
| `FID_IROPT_OFF=iv` | induction variables (also `IRO_IV`) |
| `FID_IROPT_OFF=branchfacts` | facts from branch conditions used in the blocks they dominate |
| `FID_IROPT_OFF=sccp` | sparse conditional constant propagation |
| `FID_IROPT_OFF=promote` | locals promoted from memory to values (mem2reg), in the functions `ssa` does not take (ME3, `iro_promote_here`) |
| `FID_IROPT_OFF=devirt` | a dynamic call whose receiver is a `MakeDyn` of a known type made direct (WP16) |
| `FID_IROPT_OFF=gconst` | never-written globals folded to their initializer (WP18) |
| `FID_IROPT_OFF=quietcall` | unused calls that end, never panic and write nothing removed; quiet calls out of any loop block (OP25, iropt.fd iro_quiet_fns) |
| `FID_IROPT_OFF=guardcall` | calls with no effect but a panic moved out of loops by the licmguard guard (OP25, iropt_me.fd ime_guard_moves) |
| `FID_IROPT_OFF=nowritecall` | memory facts kept across calls that write nothing (OP25, LF5/LF6, iropt_me.fd ime_writes) |
| `FID_IROPT_OFF=dse` | dead-store elimination (OP8: on by default since Nov 15; opt-in through FID_IROPT_ON before) |
| `FID_IROPT_OFF=deadf` | dead functions removed after the mid-end when the module has a main (roots: main and `p_*` only; on by default, opt-in through FID_IROPT_ON before; tests/tooling/wp-defaults-check.sh runs the suite both ways) |
| `FID_IROPT_OFF=envtrim` | unused closure-environment parameters removed (WP7; changes calling conventions; on by default since Nov 17, opt-in through FID_IROPT_ON before) |
| `FID_IROPT_OFF=spec` | functions with mixed constant/varying parameters cloned per hot constant (WP14, `iro_call_weights`; each clone checked by `tv_spec_witness` under `--validate`; on by default since Nov 17, opt-in through FID_IROPT_ON before) |
| `FID_IROPT_OFF=dynspec` | a function taking a `dyn` parameter cloned per concrete type its callers pass, so its trait calls on it become direct (inlinable) calls |
| `FID_IROPT_OFF=fielddevirt` | a trait or closure call on a value read from a struct field, every store of which stores one dyn type or one closure target, made direct (OP14) |
| `FID_IROPT_OFF=licmcall` | calls moved out of loops by LICM (whole program only, with the callees' clobber masks); off: calls stay where they are |
| `FID_IROPT_OFF=midreach` | the mid-end works only on the functions main reaches (`iro_mid_reach`, pe_ir_reachable_functions); off: every function |

`FID_IROPT_STATS=1` prints how many functions each pass changed.

## Mid-end passes of `ime_on`, opt-in or on by default: `FID_IROPT_ON`

Read by `ime_on` (compiler/iropt_me.fd). A pass is opt-in
(`FID_IROPT_ON=all` or a comma list) until the suite has run with it on
and off; then it goes into `IME_DEFAULT_ON` and is on unless
`FID_IROPT_OFF` names it. On Nov 21, 15 of the 27 are on by default:
loops, sra, loadhoist, forward, matchfacts, inline2, ssa, ssaexit (since
Nov 16) and unroll, linear, placefwd, vecalias, version, inlineclo,
lendref (since Nov 17). `FID_IROPT_OFF=<name>` still turns one off when
`all` turned it on.

| Switch | Pass |
|--------|------|
| `FID_IROPT_ON=branchfold` | branches on known conditions folded (ME12) |
| `FID_IROPT_ON=ipcp` | interprocedural constant propagation (WP13; miscompile M9 before its fix) |
| `FID_IROPT_ON=simplify` | algebraic identities and reassociation of constant steps (`iro_simplify`) |
| `FID_IROPT_ON=inline2` | inlining with a cost model, bottom-up over the call graph (ME20, ME21; iropt_me.fd). On by default since Nov 16 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=inline2` |
| `FID_IROPT_ON=loops` | loops in canonical form (ME22). On by default since Nov 16 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=loops` |
| `FID_IROPT_ON=ivsr` | induction-variable strength reduction in `@wrapping` code (ME23) |
| `FID_IROPT_ON=unroll` | full unrolling of short one-block loops (ME24; `--size` turns it off). On by default since Nov 17 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=unroll` |
| `FID_IROPT_ON=sra` | scalar replacement of struct locals (ME26). On by default since Nov 16 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=sra` |
| `FID_IROPT_ON=switch` | a chain of `x == k` tests lowered to a balanced tree (ME28) |
| `FID_IROPT_ON=loadhoist` | loads moved out of loops that cannot change them (LF5). On by default since Nov 16 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=loadhoist` |
| `FID_IROPT_ON=forward` | store-to-load forwarding and repeated loads removed (LF6). On by default since Nov 16 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=forward` |
| `FID_IROPT_ON=matchfacts` | in a match arm, the scrutinee's tag is a constant (LF23). On by default since Nov 16 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=matchfacts` |
| `FID_IROPT_ON=ssa` | the SSA round trip (`compiler/ssa.fd`, ME3/ME15): locals become values with phis, dead phis and stores go, phis go back to slots before any backend reads the IR. On by default since Nov 16 (iropt_me.fd `IME_DEFAULT_ON`): `FID_IROPT_OFF=ssa` turns it off |
| `FID_IROPT_ON=ssaexit` | with `ssa`: after a counted loop with constant bounds, its variable is a constant (ME23 exit values). On by default since Nov 16 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=ssaexit` turns it off |
| `FID_IROPT_ON=icf` | functions whose IR is the same (generic instances, specialized clones) all called through the first of them (WP19; whole program only) |
| `FID_IROPT_ON=inlineclo` | inline2's once-called rule for closures alone, when inline2 is off (WP16). On by default since Nov 17 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=inlineclo`; tests/tooling/inlineclo-check.sh |
| `FID_IROPT_ON=lendref` | with dse, loadhoist, forward or matchfacts: what a call lent by reference cannot change kept known across it (whole program only). On by default since Nov 17 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=lendref`; tests/tooling/lendref-check.sh |
| `FID_IROPT_ON=licmguard` | calls with no effect but a panic moved out of loops behind a guard (OP25, `ime_guard_moves`; with `FID_IROPT_OFF=guardcall` not); tests/tooling/licmguard-check.sh |
| `FID_IROPT_ON=linear` | a `@linear` struct parameter (held by this function alone, reference §8.7) checked for aliasing as a local is (OP26). On by default since Nov 17 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=linear` |
| `FID_IROPT_ON=placefwd` | forwarding (LF6) of a second read of the same element (`ime_same_elem`: another GetElemPtr of the same data and index). On by default since Nov 17 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=placefwd` |
| `FID_IROPT_ON=promote` | the old promotion of locals to values in every function, also where `ssa` runs (ME3: retired there, `iro_promote_here`; with `FID_IROPT_OFF=ssa` it runs everywhere anyway) |
| `FID_IROPT_ON=rotate` | loop rotation on the IR (ME23, `ime_rotate`), last on each function |
| `FID_IROPT_ON=soleown` | `vec_own` on a Vec nothing shares removed (LF16, `iro_vec_locals`), before inlining |
| `FID_IROPT_ON=vecalias` | the alias rules of an owned Vec's elements (LF3: elements of two owners never overlap; a call writes a home slot's elements only when given something derived from it); `FID_IROPT_STATS` prints `iropt: vecalias N`; tests/tooling/vecalias-check.sh. On by default since Nov 17 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=vecalias` |
| `FID_IROPT_ON=vecdata` | the data pointer of a `Vec<i64>` a loop only reads moved out of the loop, its proven-in-bounds element reads become loads from it (OP9), then forwarding again (OP26) |
| `FID_IROPT_ON=vecsroa` | a small local Vec of words becomes stack slots (LF14, `iro_vec_locals`), before inlining |
| `FID_IROPT_ON=version` | loops versioned: a copy without the checks a counted loop proves, the original kept for the other case (ME25). On by default since Nov 17 (`IME_DEFAULT_ON`): `FID_IROPT_OFF=version` |

## Backend passes and rule groups: `FID_NAT_OFF`

Read by the machine layer (`compiler/nat_ir.fd` `ng_on`: the Linux and
macOS targets and `--nat` Windows) and by the rule passes it runs
(`rules.fd` `ru_on`, `egraph.fd` `eg_on`).

| Switch | What turns off |
|--------|----------------|
| `FID_NAT_OFF=mir` | the recorded machine IR and its allocator: the direct emission path of before |
| `FID_NAT_OFF=promote` | locals kept in registers (they stay in frame slots) |
| `FID_NAT_OFF=ipra` | interprocedural clobber sets: every call clobbers every caller-saved register |
| `FID_NAT_OFF=fold` | identical-function folding |
| `FID_NAT_OFF=hint` | move-coalescing hints to the allocator |
| `FID_NAT_OFF=coalesce` | copies between virtual registers merged |
| `FID_NAT_OFF=remat` | spilled constants rematerialised instead of reloaded (a spilled rematerialized value takes no slot) |
| `FID_NAT_OFF=rematmov` | a move between two vregs of one copy class not counted as a definition, so `val k = C` is rematerialized (MI8; off: every move counts) |
| `FID_NAT_OFF=uselive` | the allocator's block liveness from use lists (RA2, `ng_use_liveness`; off: the block fixed point, `mi_liveness`; `FID_VERIFY=1` runs both and compares) |
| `FID_NAT_OFF=holes` | live ranges as exact segment lists with holes (RA2, `ng_class_segs`; off: hulls throughout) |
| `FID_NAT_OFF=segscan` | the linear scan over segment lists (RA2, `ng_seg_scan`; off: the scan over hulls) |
| `FID_NAT_OFF=split` | live-range splitting (RA4, `ng_mir_alloc_split`: a spilled value kept in a register between its uses and stored and reloaded at the split places) |
| `FID_NAT_OFF=splitprune` | the second split without the values whose pieces all spilled (RA4) |
| `FID_NAT_OFF=splitfloat` | splitting of float values (RA4/MI14; off: only integer values are split) |
| `FID_NAT_OFF=arghint` | argument-register hints (RA7, `ng_arghint_on`: a value passed as an argument allocated in that argument register; on by default since Nov 17) |
| `FID_NAT_OFF=argfree` | RA3: argument registers (rdi, rsi, rdx, rcx on x86-64) for spilled ranges no call crosses, and the trade of callee-saved registers (`ng_arg_free`; docs/reference/argument-registers.md) |
| `FID_NAT_OFF=argtrade` | RA3: the trade only (callee-saved registers whose ranges all fit argument registers give them up) |
| `FID_NAT_OFF=fcallee` | RA18: floats across calls in callee-saved float registers (d8..d15 on AArch64, fs0..fs11 on RISC-V), saved by the function; off: they spill |
| `FID_NAT_OFF=fregs` | floats in float registers (MI25: xmm, v or f registers on every target, `--nat` Windows included; off: every float in a general register) |
| `FID_NAT_OFF=winfregs` | the float register class on `--nat` Windows only (MI14, `ng_fregs_on`; `FID_NAT_ON=fregs` asks for it whatever this says) |
| `FID_NAT_OFF=coldhint` | blocks that call an `@cold` function, `cold_path()`, panic or a function that never returns laid out after the hot code (OP15, `ng_cold_call_labels`) |
| `FID_NAT_OFF=weights` | loop-weighted spill choice (the range ending last is spilled) |
| `FID_NAT_OFF=vecinline` | `Vec_len`/`get`/`set`/`push` inlined (every one becomes a call) |
| `FID_NAT_OFF=unchecked` | overflow and bounds checks removed where `compiler/checks.fd` proves them (every check stays) |
| `FID_NAT_OFF=panicline` | " at line N" in division panics |
| `FID_NAT_OFF=pairs` | AArch64: callee-saved registers saved and restored two at a time with `stp`/`ldp` (RA13) |
| `FID_NAT_OFF=probe` | stack probing of frames larger than a page (MI29: one page at a time; off: one subtraction) |
| `FID_NAT_OFF=coldpanic` | panic checks branching to cold blocks per function (MI6; off: each check calls its stub inline) |
| `FID_NAT_OFF=xmmcache` | x86-64: remembering which register xmm0/xmm1 mirror |
| `FID_NAT_OFF=peep` | the peephole pass over recorded machine IR (known-bugs 2g.40 was found with it) |
| `FID_NAT_OFF=mirfold` | constant compares and double extensions folded in the peephole (TG18) |
| `FID_NAT_OFF=brfuse` | compare + conditional branch fused into one MO_BRCC (MI23; off: SETCC + BNEZ) |
| `FID_NAT_OFF=brext` | RA19: an EXT in a fused branch's chain followed like a copy |
| `FID_NAT_OFF=addshl` | element addresses as one add-shifted / scaled lea (MI3/MI22; off: li + mul/shl + add) |
| `FID_NAT_OFF=relax` | branch relaxation: short branches where the target is in reach (TG6; off: every branch in its long form) |
| `FID_NAT_OFF=lea` | x86-64: three-operand adds as one LEA (TG7; off: MOV + ADD) |
| `FID_NAT_OFF=memop` | x86-64: an ALU operand read from its slot (TG7) |
| `FID_NAT_OFF=cmpslot` | x86-64: a compare with a spilled operand's slot (TG7) |
| `FID_NAT_OFF=rmwslot` | x86-64: a read-modify-write on a spilled slot (TG7) |
| `FID_NAT_OFF=blsmsk` | x86-64 under `--cpu x86-64-v3\|v4`: `x ^ (x - 1)` as one BLSMSK (TG7, `ng_fuse_bits`) |
| `FID_NAT_OFF=blsi` | x86-64 under `--cpu x86-64-v3\|v4`: `x & -x` as one BLSI (TG7, `ng_fuse_bits`) |
| `FID_NAT_OFF=wbidx` | AArch64: a pointer step next to its load or store in a loop as one pre/post-indexed `ldr`/`str` (TG8, `ng_wb_pair` / `ng_a64_wb_fuse`) |
| `FID_NAT_OFF=jtab` | MI27: no jump tables; every Switch becomes `ir_switch_plan`'s compare tree (docs/reference/jump-tables.md) |
| `FID_NAT_OFF=vzsimd` | K10: in a `@simd` function, checked `+`/`-` on i64 vectorized after a run-time pre-scan (off: such loops stay scalar, as outside `@simd`; docs/reference/simd-annotation.md) |
| `FID_NAT_OFF=vzunroll` | K10: a `@simd` function's vector loops unrolled twice (off: once) |
| `FID_NAT_OFF=vzspill` | TG11: vector spill slots for a vectorized loop with more vector values than the vector registers hold (`ng_vz_pins` / `ng_vz_vslot`; off: such a loop stays scalar) |
| `FID_NAT_OFF=ldfwd` | a spilled value read right after its store is a register move, not a slot load (TG17) |
| `FID_NAT_OFF=ldfwdmov` | the same past register moves that leave the stored register alone (TG17) |
| `FID_NAT_OFF=asm` | `asm!` templates run in a process, each in a thunk of its own (LG13, pe_ir's rule: privileged or unknown instructions give 0; off: every `asm!` gives 0, as before) |
| `FID_NAT_OFF=leaf` | frameless leaf functions: no call, no panic, nothing on the stack (WP8; off: every function has a frame record) |
| `FID_NAT_OFF=tailcall` | sibling calls in tail position as a jump after the epilogue (OP13; off: call, then return) |
| `FID_NAT_OFF=tailframe` | MI28: tail jumps in a function that takes its frame's address when no such address reaches the call (`ng_tail_fn_ok` / `ng_tail_taint`; off: no tail jumps in such a function, the rule before Nov 17) |
| `FID_NAT_OFF=retregs` | one- and two-word struct results (Option, Result of a word) in two registers for functions only called directly (ME27/LF24; off: through the hidden result pointer) |
| `FID_NAT_OFF=wipe` | the frame of a function holding a `@secret`/`@sensitive`/`@zeroize` value zeroed at every return (LF20; off: left as it is) |
| `FID_NAT_OFF=debug` | line tables and debug sections in the image |
| `FID_NAT_OFF=rules` | the rewrite rules of `rules.fd` (`fid test-rules` checks each on values) |
| `FID_NAT_OFF=purecse` | equal calls of pure functions merged (`rules.fd`) |
| `FID_NAT_OFF=readcse` | read-only calls merged (WP10, compiler/rules.fd; on by default since Nov 17, OP25; FID_IROPT_OFF=readcse too) |
| `FID_NAT_OFF=pgolayout` | with `--profile-use`, blocks a profiled run never or rarely entered laid out with the panic paths (OP15/OP30, ng_pgo_cold_labels) |
| `FID_NAT_OFF=pgoweights` | with `--profile-use`, uses and definitions in blocks a profiled run never or rarely entered weigh 1 in the spill choice whatever their loop depth (OP30, ng_spill_weights) |
| `FID_NAT_OFF=egraph` | equality saturation over pure integer values (`egraph.fd`) |
| `FID_NAT_OFF=egplace` | placement of the e-graph's values (`egraph.fd` `eg_place`, ME8: Click's global code motion, hoisted out of loops and sunk toward their uses); on by default since Nov 17; `egplace` in FID_IROPT_OFF (read by `eg_place_on` through `iro_named_off`, not `iro_pass_on`) or `--no-pass egplace` turns it off too |
| `FID_NAT_OFF=forward` | store-to-load forwarding for locals (`egraph.fd`) |
| `FID_NAT_OFF=isel` | the instruction-selection table (`ng_isel_on`, rows by target and operation in cost order); off: the hand-written selection |
| `FID_NAT_OFF=deadmir` | machine operations whose value nothing reads removed before branch fusion (RA19, `ng_dead_mir`) |
| `FID_NAT_OFF=brgap` | a compare fused with a branch up to `NG_BRFUSE_GAP` instructions after it (RA19); off: adjacent only |
| `FID_NAT_OFF=brnot` | `!` of a compared value followed in branch fusion (the branch tests the other way, RA19) |
| `FID_NAT_OFF=brandor` | `&&` / `\|\|` of two compares as two compare-and-branches (RA19, `ng_fuse_andor`) |
| `FID_NAT_OFF=brxblock` | a compare in another block whose value only a branch reads made at the branch (RA19, `ng_fuse_xblock`) |
| `FID_NAT_OFF=extstate` | extensions of a value already extended removed across blocks (`ng_fold_ext_state`) |
| `FID_NAT_OFF=addi` | ADD / SUB of a register and a constant that fits as one ADDI (RA6, `ng_fuse_addi`) |
| `FID_NAT_OFF=shi` | shifts by a constant as one shift-immediate (TG1, `ng_fuse_addi`) |
| `FID_NAT_OFF=madd` | AArch64: a MUL and the ADD / SUB that uses it as one MADD / MSUB (MI3/TG8) |
| `FID_NAT_OFF=andn` | x86-64 BMI1 ANDN of `a & !b` (TG7, `ng_fuse_bits`, `--cpu x86-64-v3\|v4`) |
| `FID_NAT_OFF=blsr` | x86-64 BMI1 BLSR of `a & (a - 1)` (TG7, `ng_fuse_bits`; `blsmsk` and `blsi` are the other two forms) |
| `FID_NAT_OFF=csel` | small if/else of values as conditional selects (`ng_fuse_select`; not in a function with a jump table) |
| `FID_NAT_OFF=tbz` | a branch on one bit as one test-bit branch (TG8/TG7, `ng_fuse_tbz`) |
| `FID_NAT_OFF=opw` | RISC-V: a wrapping i32 add / sub / mul and its sign extension as one ADDW / SUBW / MULW (TG9, `ng_fuse_opw`) |
| `FID_NAT_OFF=ldx` | address arithmetic folded into the access: index * scale and constant offsets as the displacement (`ng_fuse_addr`) |
| `FID_NAT_OFF=ldext` | a load followed by a narrower extension as one narrow load with that extension (`ng_fuse_ldext`) |
| `FID_NAT_OFF=ldop` | x86-64 memory operands: a load whose only reader is an ALU operation folded into it (OP18, `ng_fuse_ldop`) |
| `FID_NAT_OFF=rmw` | x86-64 read-modify-write: `p.f = p.f op x` as one `op [base + disp], x` (`ng_fuse_rmw`) |
| `FID_NAT_OFF=memcmp` | x86-64: a load compared with a register or constant as a compare with memory (`ng_fuse_memcmp`) |
| `FID_NAT_OFF=livecoal` | the allocator's second coalescing round over exact liveness (RA6) |
| `FID_NAT_OFF=frameslots` | spill slots only for spilled vregs (MI8); off: every IR value has its own frame slot, as before |
| `FID_NAT_OFF=retfwd` | the result slots of calls returning in two registers kept in vregs (OP12 / LF24, `ng_rf_promote`) |
| `FID_NAT_OFF=secretregs` | secret values given the highest spill weight, so they stay out of memory (LF20, `ng_secret_weights`) |
| `FID_NAT_OFF=wiperegs` | with `wipe`: clearing the registers a function holding secrets used (LF20, `ng_wipe_setup`) |
| `FID_NAT_OFF=shrinkwrap` | callee-saved registers saved where first needed instead of in the prologue (`ng_shrinkwrap`; not with a jump table) |
| `FID_NAT_OFF=swrestore` | with shrinkwrap: restores before the exits of a part a save dominates (`ng_sw_rest`); off: not tried |
| `FID_NAT_OFF=swcfi` | the CFI rows of shrink-wrapped saves and early restores (DB6, `ng_dbg_sw_to`); off: only the prologue's |
| `FID_NAT_OFF=zerowords` | `p_zero_words(p, n)` with a small constant n as n stores of zero (SP16); off: the call |
| `FID_NAT_OFF=simd` | vector code for core's block copies and the vectorizable builtins (`ng_vblk_call`; not with `--size`) |
| `FID_NAT_OFF=vectorize` | loop vectorization (OP22, `g_vz_on`) |
| `FID_NAT_OFF=dbgvars` | the source variables in the debug info (DB2/DB4) |
| `FID_NAT_OFF=dbgscopes` | lexical scopes in the debug info for a name bound more than once (DB4) |
| `FID_NAT_OFF=dbgbraces` | a scope runs to its block's `}` (DB4); off: only the lines that define or read it |
| `FID_NAT_OFF=dbgranges` | the code ranges where each variable is live (CT6); off: the whole function |
| `FID_NAT_OFF=unwind` | `--nat` Windows: .pdata / .xdata, the exception directory debuggers, profilers and the dispatcher walk (DB6) |
| `FID_NAT_OFF=wxsplit` | Linux: the data on pages of its own (read/execute .text, read/write .data) instead of one RWX segment |
| `FID_NAT_OFF=ldyn` | C functions imported through the dynamic linker (TG15: ELF dynamic section, dyld GOT slots, `--nat` Windows IAT) |
| `FID_NAT_OFF=pie` | a position-independent Linux executable (TG14); wins over `--pie` and `FID_NAT_ON=pie` |
| `FID_NAT_OFF=rvc` | RISC-V 2-byte instructions (TG9/TG19; on by default with every `--cpu` but `rv64g`) |
| `FID_NAT_OFF=relink` | the function cache (SP7): functions put back in the kept layout; off: the layout is not kept |

Note `forward` is two passes: egraph.fd's (`FID_NAT_OFF=forward`) and
iropt_me.fd's LF6 (`FID_IROPT_OFF=forward`); both are on by default.

## pe_ir passes: `FID_PE_OFF`

The pe_ir backend (default `fid emit` on Windows) reads `FID_PE_OFF`
(a comma-separated list, `peir_on` in `compiler/pe_ir_stubs.fd`; part of
its per-function cache key):

| Variable | What it turns off |
|----------|-------------------|
| `FID_PE_OFF=ra` | pe_ir's own register allocator (RA1: `peir_pick_reg_vars`, `peir_global_ra`, coalescing, volatile ranges); every value in its frame slot, as a bare build and an `asm!` function do. Checked by `tests/nattools/ra1_pe_ra_off_check.sh` |
| `FID_PE_OFF=r89` | r8 and r9 as value registers (D4) |
| `FID_PE_OFF=cprop` | leaving out repeated register moves (D13) |
| `FID_PE_OFF=cswap` | a constant left operand moved right (`5 < x` as `x > 5`, D13) |
| `FID_PE_OFF=asm` | `asm!` templates run in a process (LG13; off: every `asm!` gives 0) |
| `FID_PE_OFF=wipe` | clearing the frame and scratch registers of a function holding secrets (LF20) |
| `FID_PE_OFF=tail` | sibling tail calls (OP13) |
| `FID_PE_OFF=layout` | block layout with cold blocks last (OP15) |
| `FID_PE_OFF=dbgvars` | variables, types and frames in the debug information (N9; line tables and functions stay) |

## Opt-in parts of the backend

| Switch | What turns on |
|--------|---------------|
| `FID_NAT_ON=rvc` | RISC-V 2-byte instructions reported on stderr (they are on by default; `FID_NAT_OFF=rvc` wins) |
| `FID_NAT_ON=bparams` | MI1: each function's machine IR into block-parameter form and out again, checked under `FID_VERIFY=1` (`ng_bparams_round`) |
| `FID_NAT_ON=vzdump` | a `[vectorize]` line per counted loop on stderr, taken or why not (as `--stats`) |
| `FID_NAT_ON=pie` | a position-independent Linux executable (TG14; not yet run on the whole suite) |
| `FID_NAT_ON=splitall` | every integer value split and kept (RA4: a stress test of the split places and their parallel moves) |
| `FID_NAT_ON=fregs` | the float register class asked for, also on `--nat` Windows with `FID_NAT_OFF=winfregs` |
| `FID_NAT_ON=greedy` | RA9: the greedy allocator (`ng_greedy_assign`) in place of the linear scan (docs/reference/greedy-allocator.md) |
| `FID_NAT_ON=greedycmp` | RA9: with `--stats`, an `[ra9]` line per function comparing the scan's spills with the greedy assignment's |

Mutations the checkers must reject (`FID_NAT_MUTATE=<name>`; each check
script builds with one and expects `FID_VERIFY=1` to fail):
`FID_NAT_MUTATE=split` leaves out the stores at loop exits of a split
value (the split checker `ng_split_check` must reject it;
tests/nattools/machine_split_check.sh), `FID_NAT_MUTATE=arghint` makes
the allocator ignore implicit uses of argument registers (the
register-allocation checker must reject it; machine_ra7_check.sh),
`FID_NAT_MUTATE=iseldiv` makes the division rules take the other result
register or signedness (TG1; nat_isel_tg1b_check must reject it under
`FID_VERIFY=1`). nat_ir.fd's `ng_mutant` also reads addi, andn, argfree,
blsx, brandor, brgap, brnot, brxblock, cmpslot, fcallee, isel, iselcc,
iselimm, ldext, ldfwdmov, ldop, memcmp, memop, opw, par, rmw, rmwslot,
shi, tbz and wbidx; rules.fd reads `rule<N>` / `rulec<N>` (RL11) and
egraph.fd `egcomm`, `egflip`, `egfold`, `egident`, `egzero`, `egdouble`
and `egsame`.

`FID_NAT_ON=crashfree` is no longer read (Nov 21): no check is dropped on
the annotation's word; the checks of a `@crashfree` function left after
the MIR folds are verified (`ng_crashfree_check`), and an overflow check
left there rejects the build.

## Single rules: `FID_EGRAPH_OFF`, `FID_RULES_OFF`

`FID_NAT_OFF=rules` and `FID_NAT_OFF=egraph` turn a whole rule pass off.
These two turn off one part of it: the pass still runs and only the
equalities that part gives are missing, so the output must not change.
The pass matrix (`scripts/pass-matrix.sh`, RL3) turns each off in turn,
round 4 the e-graph's families and round 5 the rules of rules.fd, and
reads both lists from the source. Both variables are in nat_ir's cache key
(`nc_start`) and in the mid-end memo's key (CS1, compiler/ir_ser.fd
`mec_*`: `cfg_h` of `rules_run` and of `egraph_run`), so a build with one
never reuses code or rewrites made without it.
`tests/tooling/rl3-rules-off-check.sh` checks each switch on
`tests/bootstrap/test_verify_rl3_rule_identities.fd`, which uses every
rule.

`FID_EGRAPH_OFF=<family>,...` (compiler/egraph.fd `eg_rules_off_mask`;
`all`: every family):

| Switch | Rules of the family |
|--------|---------------------|
| `FID_EGRAPH_OFF=comm` | a + b = b + a for add, multiply, and, or, xor, `==`, `!=` |
| `FID_EGRAPH_OFF=flip` | a < b = b > a (and `<=`, `>=`) |
| `FID_EGRAPH_OFF=fold` | a binary operation of two constants |
| `FID_EGRAPH_OFF=ident` | x + 0, x - 0, x or 0, x xor 0, x << 0, x >> 0, x * 1, x / 1, x and -1 = x |
| `FID_EGRAPH_OFF=zero` | x * 0 = 0, x and 0 = 0 |
| `FID_EGRAPH_OFF=double` | x * 2 = x + x |
| `FID_EGRAPH_OFF=shift` | `@wrapping` x * 2^s = x << s |
| `FID_EGRAPH_OFF=reassoc` | `@wrapping` (x + c1) + c2 = x + (c1 + c2) |
| `FID_EGRAPH_OFF=same` | x - x, x xor x = 0; x and x, x or x = x; x == x ... |
| `FID_EGRAPH_OFF=unaryfold` | -c, ~c, !c of a constant |
| `FID_EGRAPH_OFF=invol` | !!b = b, ~~x = x, `@wrapping` -(-x) = x |
| `FID_EGRAPH_OFF=castfold` | a cast of a constant |
| `FID_EGRAPH_OFF=idcast` | a bit cast to its own type is its operand |
| `FID_EGRAPH_OFF=float` | the IEEE-exact float rules (`eg_float_rules`) |

`FID_RULES_OFF=<index>,...` (compiler/rules.fd `ru_rules_off`; `all`:
every rule) names rules by their index in `ru_rules`, from 0, in the order
`fid test-rules` and `FID_RULE_STATS=1` list them. A rule switched off
keeps its place (`FID_RULE_STATS=1` lists it with 0 hits) and never
matches; `fid test-rules`, `fid test-rules --smt` and `FID_NAT_MUTATE`
still see every rule. The indexes on Nov 18 (27 rules):

    0  x + 0 = x     7  0 * x = 0    14  0 | x = x    21  x == x
    1  0 + x = x     8  x / 1 = x    15  x | x = x    22  x != x
    2  x - 0 = x     9  x & 0 = 0    16  x ^ 0 = x    23  x < x
    3  x - x = 0    10  0 & x = 0    17  0 ^ x = x    24  x <= x
    4  x * 1 = x    11  x & -1 = x   18  x ^ x = 0    25  x > x
    5  1 * x = x    12  x & x = x    19  x << 0 = x   26  x >= x
    6  x * 0 = 0    13  x | 0 = x    20  x >> 0 = x

    FID_RULES_OFF=3,18 fid emit prog.fd --nat -o prog.exe   # x - x and x ^ x off

## Names read outside the pass lists

These names are read with `iro_named_off` or a list test of their own, not
through the readers `scripts/switch-check.sh` scans, so the script does
not check them (Nov 21):

| Variable | Name | What it turns off or on |
|----------|------|-------------------------|
| `FID_IROPT_OFF` | `rangejoin` | range facts met at joins (checks.fd; off also turns `rangeset` and `rangeloop` off) |
| `FID_IROPT_OFF` | `rangeset` | several range facts per block (LF8) |
| `FID_IROPT_OFF` | `rangedom` | facts of dominating blocks reaching loads (OP24) |
| `FID_IROPT_OFF` | `rangeloop` | facts over loops' back edges in the final scan (LF8) |
| `FID_IROPT_OFF` | `retrange` | a call's result narrowed to the range its callee returns (LF10) |
| `FID_IROPT_OFF` | `cfgcache` | the cached CFG, dominator and loop trees and use lists (OP3) |
| `FID_IROPT_OFF` | `readcse`, `egplace` | the same as in `FID_NAT_OFF` (rules.fd, egraph.fd) |
| `FID_IROPT_ON` | `paramrange` | parameters start at the ranges every call gives them (LF10, opt-in) |
| `FID_IROPT_ON` | `egskel` | the e-graph's side-effect skeleton (ME7, opt-in) |
| `FID_NAT_OFF` | `declline` | DB2: a function's declaration line is its `fn` line in the debug info (nat_enc.fd); off: the first row's line |
| `FID_NAT_OFF` | `ehframe` | DB6: `.eh_frame` and `.eh_frame_hdr` in Linux images (nat_enc.fd) |

`FID_RULES_ON` names no rule today: nothing calls `ru_opt_in` (readcse,
its last name, is on by default since Nov 17). nat_ir still keeps it in
its cache key.

Other run-time switches:

| Variable | What it does |
|----------|--------------|
| `FID_SWITCH=off` (or `0`) | no `Switch` instructions are formed from compare chains (compiler/ir.fd `ir_switch_on`) |
| `FID_WASM_SWITCH=1` | `fid wasm`: every function keeps the switch loop instead of structured control flow (TG16) |
| `FID_NAT_FAST=1` | the fast allocator of debug builds (RA14), as `--fast-alloc` |
| `FID_NAT_PAR=<n>` | SP5: nat_ir's functions compiled on n threads (1..16; not with the function cache) |
| `FID_PE_CT_OFF=1` | pe_ir's machine-code `@constant_time` check off |
| `FID_C_CT_OFF=1` | the C path's `@constant_time` check off (LF18) |
| `FID_RC_SLOTS_OFF=1` | lower.fd: copies into parameter slots stay counted (LF15, to compare) |
| `FID_MONO_SHARE_OFF=1` | generic instances keyed by their spelled types, not shared (mono.fd) |
| `FID_PE_PRUNE=0` | `emit` keeps every function before pe_ir or nat_ir (PE88) |
| `FID_ELF_PEIR=1` | `emit --elf` takes the old pe_ir ELF path (no stdlib) |

## Checks, not optimizations

| Variable | What it does |
|----------|--------------|
| `FID_VERIFY=1` | the IR verifier after each mid-end stage (`irverify.fd`), the MIR verifier and the register-allocation checker (`nat_ir.fd` `ng_mir_verify`, `ng_mir_check`), and the instruction-selection check (RL6: `nat_sel_check`, each integer MIR operation's selected code run on edge and random values by `nat_enc.fd`'s evaluator) |
| `FID_NAT_DUMP=<function>` | prints that function's machine IR after selection, after each MIR pass and after allocation (`*`: every function; `fid explain --stages` sets it too, DB7) |
| `FID_RULE_STATS=1` | how often each rule fired |
| `FID_VALIDATE=1` (or `--validate`) | translation validation (RL5/VF6, `compiler/tv.fd`): each iropt pass that changes a function, and the SSA round trip, is checked against the function before it; a rejected result is undone and reported on stderr (`tv: ...`) |

## Compile-time shortcuts (the output does not change)

| Variable | Turns off |
|----------|-----------|
| `FIDES_CHECK_CORE=1` | the checked-library stamp: core and the merged runtime are checked again |
| `FID_CT_FULL=1` | comptime substitution's skipping of functions it cannot change |
| `FID_CT_FALLBACK=1` | fatal comptime errors: a `comptime` that cannot be evaluated warns and runs when the program does, as before the hardening (reference.md §4.11, `s4_11_fallback_env_runs.fd`) |
| `FID_CT_FUEL=N` | the comptime fuel budget (100 million steps by default; `<1` means a billion): the total work one evaluation pass may do, so a runaway comptime is an error, never a hang |
| `FID_LOWER_ALL=1` | `emit` lowering only what `compiler/reach.fd` finds reachable |

## Switches that need a rebuilt compiler

Some passes are turned off by a constant in the source, not at run time:
the `LOWER_*` constants of `compiler/lower.fd`, `IRO_TAILCALL`, `IRO_GVN`,
`IRO_LICM`, `IRO_IV` of `iropt.fd`, `PEIR_*` of `pe_ir.fd` and
`PEIR_RANGES` of `checks.fd`. `scripts/verify/variants.py toggle` builds a
compiler per constant (`scripts/verify/build-variant.sh`) and compares.
pe_ir's newer passes have run-time switches (`FID_PE_OFF`, above); its
older ones are these constants.
