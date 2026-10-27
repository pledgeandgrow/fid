# pe.fd — PE format writer + runtime stubs (was: emitter + 2nd codegen)

> **CONSOLIDATED 2026-10-23 (v3, ~6,850 LOC):** the embedded second x86-64
> codegen is **deleted** (~1,000+ LOC: `PeCodeGen_gen_all_functions`,
> `gen_function`, all `gen_inst_*`/`gen_*_binop`/`gen_call*`, `PeStackFrame`,
> `collect_strings`, `pe_debug.txt` instrumentation). `generate_pe` now takes
> `(module: IrModule, l3: L3Module)` — user code is emitted by the shared
> `codegen_x64` → assembled by `ElfEmitter` → PE writer resolves symbols/
> relocs and lays out sections. **Windows builds now go through the full
> L1/L2/L3 + optimizer pipeline** (previously bypassed).
>
> What remains in the file: `X86Encoder` (for runtime stubs + boot code),
> all ~90 Win64 runtime stub bodies, `register_runtime_labels`, IAT/import
> machinery (`RdataBuilder_build_imports` — now dynamic, externs collected
> from the shared assembler's undefined syms), PE headers/sections, entry
> point, bare-metal + multiboot paths.
>
> New glue: `pe_emit_sysv_prologue` (SysV→Win64 arg shuffle wrappers in
> front of the raw stub bodies), `pe_emit_import_tramps` (call-thunks into
> the IAT), `pe_apply_text_relocs`/`pe_apply_rodata_relocs`,
> `pe_sym_text_off`, `PeLinked`, `pe_build`, `PendingUserRef` for
> user-fn references (`main`, IDT handlers).
>
> **STATUS: typechecks clean; end-to-end output still UNVERIFIED** — the
> `--native` pipeline was blocked by a pre-existing `rpo_dfs` stack
> overflow (iterative fix applied in opt.fd same day). ABI caveat: shared
> codegen is SysV — wrappers convert at p_* boundaries; user functions
> calling each other stay SysV internally, which is fine.
>
> Findings below describe the **deleted** codegen where noted; PE-format
> findings (stack/heap fields, FloatLit globals) apply to the writer that
> survived — verify against the new code.

- **Path:** `compiler/pe.fd` — **6,852 LOC** (was 7,860)
- **Role:** PE32+ format writer + runtime stub library + bare-metal emitters.

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
