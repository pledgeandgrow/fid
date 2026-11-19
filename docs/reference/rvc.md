# RVC: RISC-V's 2-byte instructions (TG9)

The machine layer's RISC-V 64 backend (`compiler/nat_ir.fd`, encoders in
`compiler/nat_enc.fd`) writes 4-byte instruction words, and every fixup
(`NATFIX_RV_JAL`, `NATFIX_RV_B12`, `NATFIX_RV_AUIPC_I`, `NATFIX_SELF32`)
assumes that. RV64GC, the base every Linux RISC-V system has, also has the C
extension: 2-byte forms of the most common instructions. TG9 uses them
through one pass after layout, so nothing before it changes.

## Turning it on

Since TG19 RVC is on by default: every `fid emit --target riscv64-linux`
executable is compressed. rv64gc, the default `--cpu`, has the C
extension, and so do the other names `--cpu` takes on RISC-V except
`rv64g`.

| Switch | Effect |
|---|---|
| (none) | compress, quietly (`ng_rvc_wanted`: `NG_RVC_DEFAULT`) |
| `FID_NAT_ON=rvc` | compress, and report on stderr |
| `--size` | compress, and report (TG19: smaller code over faster code) |
| `--stats` | report |
| `FID_NAT_OFF=rvc` | never; wins over all of the above |
| `--cpu rv64g` | never (no C extension) |

`NG_RVC_DEFAULT = false` in compiler/nat_ir.fd makes it opt-in again
(FID_NAT_ON=rvc or `--size`), TG9's first rule.

Not for an object file (`fid emit --obj`: its relocations would need linker
relaxation), Windows or macOS (RISC-V is Linux only here). A build that
is asked to report prints one line on stderr:

```
[rvc] 5123 of 9311 instructions in 2 bytes: code 37248 -> 27002 bytes
```

or, when it leaves the code as it was, why (`[rvc] not compressed: ...`;
nothing has changed then). A default build says nothing either way; its
ELF's e_flags tell: 5 compressed, 4 not.

## Code size mode (TG19)

`fid emit --size` asks for smaller code over faster code (nat_ir
`g_nat_size`, set by compiler/main.fd): loop heads are not padded to 16
bytes on x86-64 (`ng_code_align`), Vec calls are not inlined
(`ng_gen_call`, the `vecinline` rule), the vectorizers stay off, the
mid-end's unrolling is off (compiler/checks.fd `iro_cli_off`), and on
RISC-V the code is compressed. The other pieces are on in every build:
shared cold panic blocks per function (MI6) and short branch encodings
(TG6). `bash tests/nattools/tg19_size_check.sh release/fid.exe` builds
programs for riscv64-linux without RVC, by default, with
`FID_NAT_ON=rvc`, with `--size` and with `--cpu rv64g`, checks the
e_flags, the reports, that the default image is smaller than the
uncompressed one and the `--size` image no larger than the default one,
runs them under qemu-riscv64, and prints the sizes.

## The pass

`ng_rvc_pass` (nat_ir.fd) runs right after the code's end label is placed,
before any data is laid out and before `NatEnc_resolve`.
`nat_rvc_compress` (nat_enc.fd) then:

1. Sorts the code's 4-byte units: a fixup's word, and the second word of an
   AUIPC pair, is kept at 4 bytes; a `NATFIX_SELF32` word (a jump-table
   entry) is data; the asm! thunks (`nat_asm_emit`) are copied as they are;
   each function's frame prologue (`addi sp, sp, -16; sd ra, 8(sp);
   sd s0, 0(sp); addi s0, sp, 16`) is kept, because `nat_has_frame` and the
   CIE rules of `.debug_frame` read it; a branch or JAL without a fixup has
   a constant offset that will be moved; calls through a register
   (`jalr ra`) keep 4 bytes, as do `ebreak` and `ecall`. A word that is not
   a 32-bit instruction (zeros) is left alone. Any other fixup in the code
   (an absolute word) or an AUIPC without a fixup makes the pass refuse.
2. Lays the code out again: each plain instruction with a 2-byte form
   (`nat_rvc_of`) takes 2 bytes. Data words and the thunks start on a
   4-byte boundary (a `c.nop` before them where needed: never executed,
   the code before them ends in a jump).
3. Checks that every branch still reaches: a `B<cc>` by fixup within
   +-4 KB, a JAL within +-1 MB, and the same for the constant offsets.
   Distances only shrink, except by 2 bytes where a data word is
   re-aligned, so a branch at the very edge of its reach can fail: then the
   pass refuses and the code stays as it was (never a wrong jump).
4. Rewrites: the new bytes, every placed label, every fixup's position,
   each constant offset re-encoded.

`ng_rvc_pass` then moves what nat_ir recorded by offset: the line rows
(`g.lrows`), the frame rows (`g.cfi_rows`), the variables' code ranges
(`g_ng_dbg_vars`: scope lines and the ranges after `@` in each location,
relative to the function's start) and the functions' code sizes
(`g_ng_dbg_size`, which the frame table in the data holds).

The ELF says `EF_RISCV_RVC` (e_flags 5 instead of 4), and the CIE of
`.debug_frame` (and so `.eh_frame`) has code alignment 2 instead of 4
(`nat_code_align`; the prologue's four rules advance by 2 units a word).

## The forms

`nat_rvc_of(w)` gives the 2-byte form of a 32-bit word, or -1. Where two
forms exist it takes the one assemblers take (`addi sp, sp, -16` is
`c.addi` 1141, not `c.addi16sp`; `addiw rd, zero, imm` is `c.li`):

| 2-byte form | from |
|---|---|
| c.nop, c.li, c.mv, c.addi, c.addi16sp, c.addi4spn | `addi` |
| c.addiw, c.li | `addiw` |
| c.slli, c.srli, c.srai, c.andi | shifts, `andi` |
| c.lui | `lui` with a 6-bit signed upper immediate, rd not zero or sp |
| c.mv, c.add | `add` (either operand order) |
| c.sub, c.xor, c.or, c.and, c.subw, c.addw | on x8..x15 (xor/or/and/addw either order) |
| c.ld, c.lw, c.fld, c.sd, c.sw, c.fsd | x8..x15 base, scaled offsets |
| c.ldsp, c.lwsp, c.fldsp, c.sdsp, c.swsp, c.fsdsp | sp base |
| c.jr | `jalr zero, 0(rs)` (a return, an indirect jump) |

Never made: `c.j`, `c.beqz`, `c.bnez` (branches keep 4 bytes: a 2-byte
branch would need relaxation over the new layout), `c.jal` (RV32 only),
`c.jalr` (a return address stays a call's 4-byte end), `c.ebreak`.

`nat_rvc_expand(h)` is the other way, for every RV64C form: the 32-bit
instruction a 2-byte one stands for (for `fid disasm` and the tests).

## Checks

* `fid run tests/nattools/rvc_unit.fd` prints `rvc ok`
  (`nat_rvc_unit_check`): known encodings; every compressible shape over
  many registers and immediates through `nat_rvc_of`, `nat_rvc_expand` and
  back, the operations simulated (nat_enc's RISC-V simulator); a laid-out
  function compressed and resolved, every branch, the AUIPC pair and the
  jump-table entry checked; the refusals.
* `python3 tests/nattools/enc-oracle-tg9.py --fid release/fid.exe`: 36,206
  forms against clang's assembler with `.option rvc` (the same length and
  bytes for each), then `scripts/enc-oracle-tg.py`.
* `bash tests/nattools/rvc_check.sh release/fid.exe`: programs built plain,
  with `FID_NAT_ON=rvc`, with `--size` and with `FID_NAT_OFF=rvc`, run
  under qemu-riscv64, the e_flags and the CIE's code alignment, and the
  sizes (tests/bootstrap/test_machine_rvc.fd among them).
* The suite: `NATIVE=1 FID_NAT_ON=rvc FID=release/fid.exe JOBS=4 bash
  scripts/cross-test.sh riscv64-linux`.
