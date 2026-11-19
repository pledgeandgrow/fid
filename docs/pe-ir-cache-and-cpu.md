# pe_ir: the per-function cache and `--cpu`

The default Windows backend (`compiler/pe_ir.fd`) has two switches added
on Nov 16–17, 2026.

## The per-function cache (inc-roadmap B5)

`fid emit` (and `fid build`) for Windows saves each function's machine
code before its labels and `.rdata` references are resolved. The
references are stored by name: a function, an arithmetic panic stub, a
string's text, a global, an import slot, or the allocator's state. A
function's entry is keyed by its IR (`iw_irfunc`) and by a salt. The salt
covers the compiler's own hash, the struct layouts, globals, externs,
every labelled name, the C overrides, the named and cold functions,
`FID_PE_OFF`, `FID_PE_CT_OFF`, `--cpu` and `--checked-alloc`.

On the next build, a function whose key matches has its bytes copied to
its new place, and its references are registered again. The image is
laid out and linked from scratch on every build, so it comes out
byte-identical to a build without the cache.

These are never cached: `@interrupt` and `@constant_time` functions,
`asm!`, dyn values, functions that call something nobody defines, and
debug builds (`-g`, `FID_PE_DEBUG`).

| Setting | Effect |
|---|---|
| (nothing) | on: the file `<output>.fides-fncache.pe` sits beside the output, like the build stamp `<output>.fides-stamp` |
| `FID_FNCACHE=<path>` | the file `<path>.pe` |
| `FID_FNCACHE=0` (or `off`, `no`, `none`, `false`), `--no-fncache` | off |
| `FIDES_NO_CACHE=1`, `--no-cache` | off, unless `FID_FNCACHE` names a path |
| `FID_PE_FNCACHE_STATS=1` | prints `[pe] fncache: N hits, M misses` |

Code: `pe_fc_base_path`, `pe_fc_output`, `PeirGen_fc_start`,
`PeirGen_fc_capture`, `PeirGen_fc_replay`, `PeirGen_fc_load`,
`PeirGen_fc_finish` and `PeirGen_gen_cached`. The driver records the
output with `pe_set_output_path`. Without that call, the `-o` of an
`emit`/`build` command line is used.

Checks:
- `tests/nattools/pe_fncache_check.sh`: for every suite program it builds
  without the cache, cold, warm and with the default cache, and every
  image must be the same; it also checks an edit pair and the off
  switches.
- `scripts/pe-win-check.sh` (B5).

## `--cpu` (competitive OP20)

`fid emit --cpu <name>` takes the names that `nat_cpu_check` accepts for
x86-64. An unknown name is an error. With `x86-64-v3` or `x86-64-v4`
(`peir_bmi`), the backend uses these instructions:

- `SHLX`/`SARX`/`SHRX` for a 64-bit shift by a variable count
  (`PeirGen_shift_bmi`). The operands come straight from their registers
  and `CL` is not needed.
- `BLSR` for a 64-bit `x & (x - 1)` (`peir_blsr_src`, `PeirGen_gen_blsr`).
  The `x - 1` is still generated, along with its overflow check.

Without `--cpu`, and with `baseline` or `native`, the image is the same
as before. The encoders `PeirEnc_shiftx`, `PeirEnc_blsr` and
`PeirEnc_andn` (`compiler/pe_ir_enc.fd`) use the same encodings as the
machine layer's `x64_shiftx`, `x64_blsr` and `x64_andn`.

ANDN is not selected on pe_ir yet. pe_ir frees a value's register after
its last use, so at the And, the operand of `!b` may already be gone. The
fusion needs the Not to be skipped and `b` kept live.

Check: `tests/nattools/pe_cpu_check.sh` runs
`tests/bootstrap/test_pe_cpu_bits.fd`.
