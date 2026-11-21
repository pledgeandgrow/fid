# The PIR path is retired (MI18)

Until November 2026 the self-hosted compiler carried a second native
pipeline beside the machine layer: IR -> PIR L1/L2/L3 (`compiler/pir.fd`)
-> the PIR optimizer (`opt.fd`) -> linear-scan allocation (`regalloc.fd`)
-> assembly text (`codegen_x64.fd`, `codegen_arm64.fd`, `codegen_riscv.fd`,
`peephole.fd`) -> an assembler and linker (`elf64.fd`, `arm64_enc.fd`,
`riscv_enc.fd`, `runtime_linux/arm64/riscv.fd`, `linker.fd`). Since Nov 16
no build used it (`build --native` is the machine layer on every target).
On Nov 17 its commands were re-targeted and its entry points removed.

## What the commands do now

| Command | Before | Now |
|---|---|---|
| `fid pir`, `fid dump-pir` | PIR L1/L2/L3 | exit 1 and name the replacements below |
| `fid opt <f> [--fn <name>]` | PIR L2 after opt.fd | the IR after the IR optimizer `emit` runs (iropt.fd `iropt_native`); `FID_IROPT_OFF`, `FID_IROPT_ON`, `FID_IROPT_STATS` work as for `emit` |
| `fid codegen <f> [--target x86-64\|arm64\|riscv\|<triple>] [--fn <name>]` | assembly text of codegen_*.fd | the machine layer's code as instructions by function (as `fid disasm`); the short names are their Linux targets |
| `fid codegen <f> --elf\|--coff\|--macho <o>` | the assembler path's object | the machine layer's object file (as `fid emit --obj`): ELF for the Linux target, COFF for x86-64 Windows, Mach-O for the macOS target of the architecture |
| `fid codegen ... --peephole` | ran peephole.fd | accepted, does nothing |
| REPL `:pir <fn> [1\|2\|3]` | PIR of one function | says it was removed; `:opt <fn>` prints the function after the IR optimizer, headed by its effects |
| `fid emit --boot` | pe_ir's bootable image (PEIR_BARE_BOOT); the old pe.fd kernel only with PEIR_BARE_BOOT off | unchanged; with PEIR_BARE_BOOT off it is now an error instead of the PIR kernel |

Related views: `fid dump-ir` (IR as lowered), `fid dump-mir` (machine IR
after allocation), `fid disasm` (machine code), `fid explain <f> --stages
<fn>` (one function after each stage).

## What was removed from the compiler's own modules

- `compiler/main.fd`: `cmd_pir` and the PIR bodies of `cmd_opt` and
  `cmd_codegen`; the `--boot` branch through `generate_boot_binary`; the
  imports of `pir`, `opt`, `pe`, `codegen_x64`, `codegen_arm64`,
  `codegen_riscv`, `peephole`, `elf64`, `coff`, `macho`.
- `compiler/compiler.fd`: `build_executable` and `caps_to_bits`;
  `repl_fn_pir` (replaced by `repl_fn_opt`); the imports of `pe`,
  `runtime_linux`, `codegen_x64`, `peephole`, `pir`, `opt`, `codegen`,
  `codegen_arm64`, `arm64_enc`, `runtime_arm64`, `codegen_riscv`,
  `riscv_enc`, `runtime_riscv`. `elf64`, `linker` and `x86` stay:
  `fid build-x86` (`build_x86_linux`) uses them.

After this, `main.fd` reached `pir.fd` only through `elf64.fd`'s
`use compiler::pir;` (elf64 used nothing from it). That line is gone
(Nov 21: elf64.fd imports only core, vec, string and image_writer), so these
files are reached by nothing the compiler imports and can be deleted: `pir.fd`, `opt.fd`, `regalloc.fd`, `codegen_x64.fd`,
`codegen_arm64.fd`, `codegen_riscv.fd`, `peephole.fd`, `pe.fd`,
`codegen.fd`, `arm64_enc.fd`, `riscv_enc.fd`, `runtime_arm64.fd`,
`runtime_riscv.fd`, `runtime_linux.fd`, with `tests/selfhost/test_pir.fd`
and `test_opt.fd` (they import pir/opt). `coff.fd` and `macho.fd` are no
longer reached from main.fd either; `tests/nattools/image_writer_obj_unit.fd`
still tests them.

## Checks

```
FID=release/fid.exe bash tests/tooling/mi18-pir-retired-check.sh
FID=release/fid.exe bash tests/tooling/repl-inspect-check.sh
FID=release/fid.exe bash tests/tooling/repl-exit-check.sh
STRICT=1 FID=release/fid.exe bash tests/tooling/mi18-pir-retired-check.sh   # once elf64.fd drops pir
```
