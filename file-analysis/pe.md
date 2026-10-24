# pe.fd — PE emitter + second x86-64 codegen

> **Re-verified 2026-10-23:** all findings confirmed — SizeOfStackReserve/HeapCommit fields still pasted (:1391/:1397), FloatLit globals still 8 zero bytes (:2078-2081, `f64_to_bits` still stubbed), `pe_debug.txt` instrumentation still writing truncate-mode files (:1673/1897/2043+).

- **Path:** `compiler/pe.fd` — **7,860 LOC** (Rust `src/pe.rs` = 8,966, ~88%)
- **Role:** `PeCodeGen` — a complete *second* x86-64 codegen that emits machine code directly (own `X86Encoder`, label/reloc fixup, `.rdata` builder, IAT/import machinery), wrapped in a PE32+ image — plus a flat-binary bare-metal mode with ported CR0/CR3/CR4/IDT/GDT/port-I/O stubs. Independent of the codegen_x64 → elf64 → linker path.

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `compiler.fd` | :1735 | `generate_pe(module)` — `build --target windows/x64-windows/x86-64-windows` |
| `main.fd` | :1739 | `generate_flat_binary(ir_mod)` — `build --bare` (no-std flat binary, load @ 0x10000) |

Both live. Note `generate_pe` takes the **raw `IrModule`** — it bypasses the L1/L2/L3 + optimizer pipeline entirely (its own codegen consumes IR directly).

## Verified faithful (code inspection)

- PE image layout byte-correct: MZ stub (`e_lfanew=64`), `PE\0\0`, COFF AMD64/2-sections/opt-240, characteristics `0x22` (EXECUTABLE_IMAGE|LARGE_ADDRESS_AWARE — same as Rust), PE32+ magic `0x20B`, `ImageBase 0x140000000`, SectionAlignment 0x1000, FileAlignment 0x200, all 16 data directories (export empty, import real, IAT real, rest zeroed).
- Section headers: `.text` `0x60000020` and `.rdata` `0xC0000040` — identical constants to Rust (:1755/:1767).
- `resolve_rip_refs` (:1047): `disp = target_rva - (code_rva + 4)` — correct RIP-relative.
- `RdataBuilder` + `patch_rvas`: full ILT/IAT/import-by-name/dll-name machinery with placeholder-then-patch — same two-phase approach as Rust.
- Bare-metal stubs (port I/O, CR0/CR3/CR4, IDT, GDT, GS-base) ported — this backend generates kernel-capable code on both sides.

## Findings

### MEDIUM — stack/heap limits pasted with the wrong byte *(code inspection)*

All four `SizeOf*` fields emit the same LE pattern `00 00 10 00 00…` (= `0x100000`):

| Field | Fides | Rust | Delta |
|---|---|---|---|
| StackReserve | `0x100000` (1MB) | `0x10000000` (**256MB**) | **256× smaller** — deep recursion overflows a guard page far earlier |
| StackCommit | `0x100000` | `0x100000` | ✓ |
| HeapReserve | `0x100000` | `0x100000` | ✓ |
| HeapCommit | `0x100000` (1MB) | `0x1000` (**4KB**) | 256× larger — wasteful, harmless |

The `0x10` sits at byte index 2 for all four — the StackCommit pattern was pasted into StackReserve (should be `00 00 00 10`) and HeapCommit (should be `00 10 00 00`). Classic off-by-one-byte copy bug.

### MEDIUM — `FloatLit` global initializers are zero *(code inspection)*

:2078-2081 — `Expr::FloatLit(f)` pushes eight `0x00` bytes with the comment `// For now, zero-initialize floats (f64_to_bits is a stub)`. Rust emits `(*f as f64).to_le_bytes()` (:2102). Every `var pi: f64 = 3.14` / const float in a PE or `--bare` build silently reads `0.0`. F32 also unaffected-correct — nothing uses the type tag.

### LOW — `pe_debug.txt` instrumentation left in *(code inspection)*

9× `p_file_write("pe_debug.txt", "[pe] …")` across `generate_pe`/`PeCodeGen_generate`/label-resolution (:1673-2120). `p_file_write` truncates → only the last write ("Num labels") survives anyway, and every `build --target windows` drops a stray file in cwd. Rust's `file_write` hits are the *builtin's codegen* — no equivalent instrumentation.

### LOW — `SizeOfHeaders` hardcoded `0x200` *(code inspection)*

Rust computes `header_size = align_up(dos+sig+coff+opt+2×40, FILE_ALIGN)` = 512. Fides's constant matches *today* — but silently wrong the day a third section is added while the pad loop at :1455 uses the real `header_size`. Latent inconsistency.

### Inherited — five `p_vec_*` runtime stubs

`p_vec_insert`/`remove`/`reverse`/`extend`/`sort` → `xor eax,eax; ret` (:6691-6729) — identical stub bodies in Rust (:7538+). Vec ops silently return 0 on the PE path in *both* implementations — a shared missing-runtime area, not a port regression.

## Fix order

1. Fix the StackReserve/HeapCommit byte patterns (one-byte edit each: `0x10`→`0x00` at index 2, `0x10` at index 3 / index 1).
2. Implement `f64_to_bits` (or inline the F64 bits via `p_f64_to_bits` if a builtin exists) and emit real `FloatLit` bytes — silent global corruption otherwise.
3. Strip the 9 `pe_debug.txt` writes.
4. Replace the `0x200` SizeOfHeaders constant with `header_size`.

## Verification state

- No live runs (no `release/fides-c3.exe`; `fidesc.exe` is Rust-side which is correct). All findings structural: byte-level diff of the optional-header emission, stub-body inspection vs identical Rust stubs, RIP math checked against spec.
- Port coverage spot-checked across encoder helpers, PE layout, import machinery, bare-metal stubs.
