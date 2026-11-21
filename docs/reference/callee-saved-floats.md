# Callee-saved float registers (RA18)

The machine layer (`compiler/nat_ir.fd`) keeps floats in float registers on
every target by default (MI25, OP17, MI14). Its float pool is caller-saved
on every target, so before RA18 a float that lived across a call always went
to a stack slot: a store before the call and a load after it.

RA18 gives such a value one of the callee-saved float registers of the
platform ABI:

| Target | Registers | Pool order |
|--------|-----------|------------|
| AArch64 (Linux, macOS) | d8..d15 (the low 64 bits of v8..v15) | d8 first |
| RISC-V 64 (lp64d) | fs0, fs1, fs2..fs11 (f8, f9, f18..f27) | fs0 first |
| x86-64 (System V, `--nat` Windows) | none | — |

System V keeps no xmm register across calls. Windows keeps xmm6..xmm15, but
those already serve as the float pool and the vector class there, so RA18
leaves x86-64 alone.

## Allocation

`ng_float_assign` runs one linear scan over the float ranges in order of
start:

- A range that a call falls inside takes a free register from
  `ng_float_callee_pool`. When none is free, it may take one from an active
  range that weighs less (spill weights) and holds a callee-saved register.
  Otherwise it spills.
- A range that no call falls inside takes a caller-saved register first,
  then a callee-saved one.

The function saves every callee-saved float register it uses in its
register save area, after the integer ones (`ng_mir_alloc`, the `save`
list). On AArch64, two neighbouring float saves are paired: `stp d8, d9,
[sp, #off]` is built by `ng_a64_fpair`, and `ng_pair_ok` never pairs a
general register with a float one. The other saves go one at a time
through `ng_fsave_slot`: `str d` / `ldr d`, `fsd` / `fld`, and through the
address register when the offset is out of reach.

The saves go through the same `ng_save_subset` / `ng_restore_subset` as the
integer registers, so these work unchanged:

- RA12 shrink wrapping: a deferred save, or an early restore.
- OP13 tail calls: `ng_epilogue_tail` restores the registers.
- The register-class check of RA10.

Because these registers are not in the float pool's clobber mask, the
allocation checker (`ng_mir_check`, `FID_VERIFY=1`) lets one hold a value
across a call. It also requires every callee-saved float register a value
holds to be in the function's save area.

The `.debug_frame` rows do not describe the float saves.
`ng_dwarf_reg` gives a float register no number, so a debugger unwinding
through such a frame shows the callee's d8 as the caller's.

## Tasks

`nat_ctx_swap` (`ng_prim_ctx_swap`, the task switch) saves only the
integer callee-saved registers. Suppose a task is switched out while one of
its frames has changed d8. The thread it gives control back to would then
see that d8, and so would the task itself when it resumes, possibly on
another worker.

So a program that can start a task keeps every float off these registers.
`ng_fcs_tasks` decides this from the IR module before the code cache's salt
is computed: the module can start a task when it has a call to `p_co_go`
or `p_co_async`, or a call from outside the scheduler to the runtime
functions that make a task. The result is stored in `g_ng_fcs_block` and
also goes into the salt. Once `nat_ctx_swap` saves d8..d15 / fs0..fs11 too,
`g_ng_fcs_block` can go.

## Switches

| Setting | Effect |
|---------|--------|
| `FID_NAT_OFF=fcallee` | No callee-saved float register is used: a float across a call spills, as before. |
| `FID_NAT_MUTATE=fcallee` | The registers are used but not saved (TG2): `tests/bootstrap/test_machine_fcallee.fd` must then print something else. Never set in a real build. |
| `--stats` | Prints `[fcallee] <fn>: N float ranges across calls in callee-saved float registers`. |

## Tests

- `tests/bootstrap/test_machine_fcallee.fd`: floats across calls in loops,
  recursion, twelve floats over one call, f32, an early return, a tail
  call, and floats passed to calls. Its expected output comes from
  `fid run`, and the C backend agrees.
- `tests/bootstrap/test_machine_fcallee_tasks.fd`: two tasks that yield
  while floats are live.
- `tests/nattools/machine_fcallee_check.sh` checks:
  - the `[fcallee]` lines;
  - the output with the switch on and off;
  - that the mutation is caught;
  - with objdump, the d8 / fs0 saves;
  - the tasks gate;
  - that x86-64 is unchanged.
- `tests/nattools/ra18_float_bench.sh` runs the measurement:
  - the float loop (`tests/nattools/bench_float.fd`) on `--nat` and
    x86_64-linux, with floats in float registers against the xmm cache
    alone (`FID_NAT_OFF=fregs`);
  - on aarch64-linux / riscv64-linux, `fcallee` against
    `FID_NAT_OFF=fcallee`.

## Measurements

To be filled in from `bash tests/nattools/ra18_float_bench.sh release/fid.exe 5`
on a quiet machine (the numbers of the Nov 9 first step, xmm cache only:
the float loop 1834 -> 1364 ms).

Nov 21 (roadmap RA18 check, fid built from a self-hosted stage 0, not a
quiet machine): float ranges a call crosses take callee-saved float
registers on AArch64 (31 ranges) and RISC-V (40); the fcallee tests pass
and the mutation is caught. ra18_float_bench: AArch64 0.89 of the time with
spilling, RISC-V 1.07 (slower, under qemu).
