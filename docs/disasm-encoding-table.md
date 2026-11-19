# One table of encoding forms (TG5)

`fid disasm` (compiler/disasm.fd) and the machine layer's encoders
(compiler/nat_enc.fd) describe the same instructions. Since TG5 the
description lives in one place: the table in compiler/disasm.fd, section
"The encoding forms: one table". The decoders read only the table. The
encoders' base words are the `EFW_*` constants the table rows are built
from, and tests/nattools/enc_table_unit.fd checks every encoder against its
row. A form cannot change on one side and not on the other without that
test failing.

## A row

| Target | What the row fixes | Operands |
|---|---|---|
| AArch64, RISC-V 64 | `base`, `mask`: word `w` is the form when `(w & mask) == base` | the template's `{fields}`, in the bits `mask` leaves free |
| x86-64 | mandatory prefix (none, any, 66, F2, F3), opcode map (one byte, 0F, 0F 38, 0F 3A), opcode, ModRM.reg digit (or /r, none, +r), REX.W / VEX.W rule, VEX or not | the template's `{fields}`: ModRM operands, VEX.vvvv, immediates |

The template is the assembler text with each operand written as a field:

```
ef_w(a, EFW_A64_ADD, 0xFF200000, "add {xd}, {xn}, {xm}{shift}");
ef_w(r, EFW_RV_LD, 0x0000707F, "ld {rd}, {immi}({rs1})");
ef_x(0xF2, 1, 0x58, -1, -1, "addsd {x}, {xrm}");
ef_vx(0x66, 2, 0xF7, -1, -1, "shlx {r64}, {rm64}, {gv}");
```

`ef_field_code` lists the fields. Each field has two halves in the same
file: `ef_put` (encode: put an operand into the word) and `ef_field_text` /
`x64_field_text` (decode: the operand as text). The fields cover registers
(`xd`, `xns` for sp, `wn`, `dd`, `qd`, `vd`, `rd`, `fs1`, `rvs2`, `r`, `rm`,
`x`, `xrm`, `v`, `vv`, ...), immediates (`imm12`, `imm16`, `immi`, `immst`,
`shamt6`, `ib`, `sb`, `sz`, `iq`, ...), shifts and conditions (`shift`,
`sh12`, `hw`, `cond`, `ncond`, `rsc`), branch targets (`b26`, `b19`, `b14`,
`boff`, `joff`: absolute addresses in the text, word or byte offsets for
`ef_word`) and a few with their own rules (`lsl6`: UBFM with
imms + 1 == immr; `fsame`: rs1 == rs2; `rmd`: the rounding mode, shown when
it is not dyn).

## Matching order

Aliases (`ef_alias`, priority 0: `mov`, `cmp`, `neg`, `cset`, `li`, `mv`,
`ret`, `beqz`, ...) are matched before the general forms they are cases of;
within a priority, table order. An x86-64 row whose prefix rule is the
exact mandatory prefix wins over a row that takes any prefix.

## API

| Function | Use |
|---|---|
| `ef_init()` | builds the table once (every other function calls it) |
| `ef_id(arch, template)` | the row of a template, -1 when none |
| `ef_word(id, a, b, c, d)` | AArch64 / RISC-V: the word of a row with its operands in template order |
| `ef_x64_rr(id, reg, rm, v, w, l256)` | x86-64: the bytes of a row between registers (prefix, REX or VEX, opcode, ModRM); immediates follow |
| `ef_decode_word(arch, w, pc)` | the text of a word, "" when no row has it |
| `ef_check()` | the table's own problems: unknown fields, a field on fixed bits, two fields on the same bits, base bits outside the mask, a row no word reaches |
| `EFW_*` | the base words (AArch64 and RISC-V) the encoders use |

## Adding a form

1. Add the `EFW_*` constant (AArch64 / RISC-V) and the row; an alias of an
   existing form with `ef_alias`.
2. Write the encoder in compiler/nat_enc.fd with the constant.
3. Add a line for it to tests/nattools/enc_table_unit.fd: the encoder's
   word against `ef_word` of its row, and the text it decodes to.
4. Run the checks below.

## Checks

```bash
$FID run tests/nattools/enc_table_unit.fd      # table ok / a64 ok / rv64 ok / x64 ok
FID=$FID bash scripts/disasm-check.sh          # objdump + assembler round trip
python3 scripts/enc-oracle.py --fid $FID       # the encoders against clang
```

scripts/disasm-check.sh emits each test program for x86_64-linux,
aarch64-linux and riscv64-linux and lists it with `fid disasm`, then

- x86-64: compares the instruction start addresses with `objdump -d -z`
  (llvm-objdump, or GNU objdump: `OBJDUMP=...`) on a copy of the image in
  which the padding `fid disasm` names is nops
  (tests/nattools/disasm_pad.py); a linear disassembler otherwise runs
  from the zeros after a jump into the next instruction;
- AArch64 / RISC-V: no word left undecoded that objdump decodes;
- every target: tests/nattools/disasm_roundtrip.py writes each listed
  instruction as assembly, assembles it with `zig cc -c` and requires its
  own bytes back. That checks the operands, not only the lengths.

## What the first runs found (Nov 17)

- x86-64 alignment padding: zeros after a `jmp` / `ret` / `ud2` up to a
  16-byte loop head (MI30) or a 4-byte function start inside one symbol
  were decoded as `add [rax], al` and the decoder ran into the next
  instruction (in test_driver_disasm, 69 start addresses that
  `objdump -d -z` did not have; 217 without -z). They are now one
  `(N zero bytes of padding)` line. The trailing padding of a function is
  found at an instruction boundary, so an instruction ending in zero bytes
  is not cut.
- `uxtb x, w` / `uxth x, w` (64-bit UBFM #0, #7 / #15) is not valid
  assembly: the 32-bit alias only. Now `ubfx x, x, #0, #8` / `#16`.
- `sh2add` / `sh3add` rows: found by enc_table_unit.fd against
  `rv_shadd` while building the table.
- Not decoded before: AArch64 `tbz` / `tbnz` and the register-offset
  loads and stores, x86-64 `test al/eax, imm` (A8/A9), `bt` (0F BA /4),
  C5 VEX, the 16-bit immediate of `66 81` / `66 C7`, packed SSE and AVX
  forms, RISC-V `czero.*`, `andn`, vector forms.

On the corpus (test_driver_disasm, test_float, test_lib_atomic,
test_machine_fast, test_machine_simd for three targets, test_float with
`--cpu x86-64-v3` and `--cpu rva23u64`, and the `--nat` Windows PE), with
the Nov 16 compiler's images: every x86-64 start address agrees with GNU
objdump, no word or byte is left undecoded, and every listed instruction
assembles back to its own bytes.
