# elf64.fd — ELF64 assembler + object writer (2,693 LOC)

> **Re-verified 2026-10-23:** all CRITICAL findings confirmed — `offset*256+base` pack + trunc-division decode (:624, ~40 sites incl. the `off+256` pseudo-fix at :857), `.rela.rodata` collected never emitted, `jo`/`movapd` unhandled while codegen_x64 emits `movapd` (:1896/1910). `encode_asm_block` exists (:2477) but runtime_linux never calls it.

## Role

AT&T-assembly → ELF64 relocatable-object pipeline: mnemonic dispatch,
instruction encoders (GP + SSE), symbol/rodata tables, `build_elf` byte
writer, `dump_symbols`. Port of `src/elf64.rs` (2,852 LOC). AGENTS.md
rates this file HIGH-risk.

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `compiler/compiler.fd` | :1644 | `emit_object_file(0, asm)` — the `build` path for x86-64 ELF (asm → peephole → ELF → `link_with_runtime`). The same-named functions in `coff.fd`/`macho.fd` are dead (module-scope resolution picks this one). |

`runtime_linux.fd:2852` still has a TODO to call `encode_asm_block` —
that entry point exists here (:2477) but is not yet consumed.

## Findings

### F1 — PORT REGRESSION (CRITICAL): every negative displacement is off by +1

`try_parse_mem` (`:609-627`) packs `offset * 256 + base` into a single
i64 where Rust returns a real `(off, base)` tuple (:2512). Callers
unpack with `packed / 256` / `packed % 256`. Fides `/` truncates toward
zero — **verified live**: `-2043 / 256 = -7`, `% 256 = -251`.

So `-8(%rbp)` → packed `-2043` → decoded `off = -7`, `base = -251`
(`base & 7` recovers 5 by luck). `encode_modrm_mem` then emits disp8
`0xF9` = **-7** — every stack-relative access lands **one byte above
the intended slot**, overlapping the adjacent slot: `movq -8(%rbp),
%rax` reads -7..0, `movq` to -16(%rbp) writes -15..-8.

Scope: **all** spill loads/stores, locals, stack args, `movapd -N(%rbp)`
— the most common operand shape in generated code. Same defect in the
SIB packer (`/4096` trunc, `-16(%rbx,%rcx,8)` → -15). The `off < 0 →
off + 256` patch at :857 converts to the correct byte pattern only
because it preserves the wrong value (-7 → 249 = -7 disp8).

Fix: pack `offset` and `base` in separate fields/struct, or decode with
floor semantics (`off = (packed - base_guess)/256` won't help — the
pack is lossy at the boundary). Cleanest: return `MemInfo`/`MemInfoSib`
structs — they already exist (:596-606) but are unused.

### F2 — PORT REGRESSION (HIGH): `.rela.rodata` collected but never emitted

`handle_directive` records `.quad <symbol>` into `rodata_relocations`
(:447, :469), and `.rela.rodata` is even pre-seeded in shstrtab (:178) —
but `build_elf` writes **only `.rela.text`** (:2380-2392). Rust emits
the full `.rela.rodata` section (src:2060-2068).

Impact: vtables emit `.quad method` → 8 zero bytes + a reloc that never
reaches the file → **every vtable entry is 0 → every trait-object call
on the ELF path jumps to address 0**. `linker.fd:596-597` explicitly
looks for `.rela.rodata` and never finds it.

### F3 — Unknown/unhandled instructions → `0x90` NOP (inherited, live)

Six sites emit nop on unhandled forms: the dispatch fallthrough (:1142)
plus `encode_mov` (:1443), `encode_alu` (:1566), `encode_cmp` (:1706),
`encode_lea` (:1939), `encode_setcc` (:2066). codegen_x64 emits
**`jo`** (overflow trap after every non-wrapping `addq`/`subq`/`imulq`
— codegen_x64.fd:904/910/918), **`movapd`/`movupd`** (XMM spills
:1896/1910, SIMD :1842/1849), and **`addpd/subpd/mulpd/divpd`** — none
are in the dispatch table. On the ELF path these become 1-byte NOPs:
**integer-overflow checks silently deleted; packed-float ops and
unaligned XMM spills silently dropped** (leaving garbage in xmm0).
Rust has the identical fallthrough (src:867) — inherited, but the
missing mnemonics make it a live hole.

### F4 — `xorq $imm64` stack canary → `xor $0` (inherited, live)

`encode_alu` parses immediates with `str_parse_int` only (:1496) —
no hex path (unlike `encode_mov` :1197). The canary
`xorq $0x5DEADBEEFCAFEBABE, %rax` (run 10) parses to 0 → imm8 form
`83 F? 00` → **canary compare becomes `xor rax, 0`** — never detects
smashing, silently.

### F5 — Custom-section names emitted after shstrtab is frozen (latent)

`build_elf` copies `self.shstrtab` into the file at :2378, then adds
custom-section names at :2437 — names land in the table after its bytes
were already written → custom sections (.data/.bss for kernel work)
get `sh_name` offsets past the emitted table → nameless/garbage-named
sections. No current caller of `add_custom_section` — latent.

### F6 — Additional gaps

- `.align`/`.section`/`.globl`/`.type`/`.size` all ignored (:473) —
  no alignment enforcement anywhere.
- `emit_object_file_for_target` arm64/riscv (:329-345) produces a
  structurally-valid ELF with **empty .text** and the raw asm text in
  `.comment` — silent non-functional object. Dead path today
  (compiler.fd only calls for x86-64).
- `encode_jcc` (:1835) unconditionally makes targets local labels —
  a `jcc non_dot_L` would create a shadowing STB_LOCAL undef symbol.
  All current jcc targets are `.L*` — latent.

## Verification state

- F1 verified live: Fides `-2043 / 256` → `-7` under fidesc interp;
  every negative-disp consumer confirmed by reading the unpack sites.
- F2 verified by absence: `rodata_relocations` has exactly two push
  sites and zero readers in `build_elf`; Rust counterpart emits it.
- F3/F4 verified against codegen_x64's emission vocabulary — `jo`,
  `movapd`, `movupd`, `*pd` arithmetic all absent from the dispatch.
- Can't be exercised end-to-end without running the self-hosted
  compiler; findings are from source + interp semantics.

## Next

`embedded.fd`.
