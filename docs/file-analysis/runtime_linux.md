# runtime_linux.fd

> **Corrected 2026-10-23 (v2):** the earlier re-verify note was stale — `encode_asm_block` **is** called at :2858 (A1 fix): bytes appended at `asm_base`, defined `.text` symbols merged, reloc indices resolved to names. `p_string_*`/`p_map_*` builtins are now in the runtime symbol table. `cap_granted` now does per-bit checks (`FS/IO`=1, `Net`=2, `Spawn`=4) and `runtime_parts_with_caps` is called from `build_executable`. Remaining from the original findings: linker call relocs still land rel32=0 for *unknown* symbols (linker.md).

- **Path:** `compiler/runtime_linux.fd`
- **LOC:** ~4,370 (Rust `src/runtime_linux.rs`: ~4,383 — near-identical size)
- **Role:** Hand-encoded x86-64 machine-code runtime for the Linux ELF backend. Emits raw bytes (`pb1`–`pb9` helpers) for `_start`, syscall wrappers, memory allocators, vec/map/string builtins, plus `.rodata` (format strings, panic messages). Also builds a ~1,450-line **assembly text** block meant for `elf64::encode_asm_block`.

## Where called

| Caller | Line | Mechanism |
|---|---|---|
| `compiler/compiler.fd` | 43 | `use runtime_linux` |
| `compiler/compiler.fd` | 1693 | `runtime_parts()` — x86-64 `build` path (ELF target) |
| `compiler/compiler.fd` | 1694 | `runtime_relocations()` — reads `g_runtime_relocations` global set inside `runtime_parts_impl` |

Sole consumer: the x86-64 ELF path (`codegen_x64` asm → `elf64::emit_object_file` → `runtime_parts` → `linker::link_with_runtime`). The ARM64/RISC-V targets use sibling runtimes (`runtime_arm64.fd`, `runtime_riscv.fd`).

## Findings

### HIGH — 39 runtime functions are silently missing: the entire `runtime_asm` block is built and then dropped

`runtime_parts_impl` spends :1400–~2850 concatenating a `runtime_asm` string containing **39 complete assembly functions** — then at :2852:

```
// TODO: call elf64::encode_asm_block(runtime_asm) when elf64.fd is ported
var asm_bytes: Vec = Vec::new();
var asm_syms: Vec<RuntimeSymbol> = Vec::new();
var asm_relocs: Vec<RuntimeReloc> = Vec::new();
```

The three Vecs stay **empty for the rest of the function** — `runtime_asm` is never encoded, appended to `text`, or converted to symbols. The reloc-merge loop at :4357 iterates an empty Vec. `elf64.fd` **is** ported — `encode_asm_block` exists at `elf64.fd:2477`. The TODO is stale; the fix is literally the Rust code at `src/runtime_linux.rs:2876` (`text.extend(asm_bytes)`, push `asm_syms`, merge `asm_relocs`).

The dropped functions (label set verified identical to Rust's raw string — the text was copied verbatim):

- **All 18 `p_string_*`**: `len, is_empty, from, contains, starts_with, ends_with, find, char_at, substring, to_uppercase, to_lowercase, trim, split, replace, repeat, push_str, parse_int` — plus `p_strlen` and `p_strcmp`
- **All 10 `p_map_*`**: `new, insert, get, get_or, contains, remove, len, is_empty, clear, clone` + `p_print_map`
- **8 `p_vec_*` ops**: `insert, remove, reverse, extend, sort, clear, clone, contains, truncate`

None of these appear in `add_sym` either — they are not stubbed, they are **absent from the symbol table entirely**.

**Consequence chain** (verified through `linker.fd`): a user `call p_string_len` emits a reloc the linker can't resolve. `linker.fd:803` hits the `// Unknown symbol -- skip` arm, leaving `call rel32 = 0` in place — which calls **the very next instruction**, i.e. falls through and returns whatever was in `%rax`. Not a crash: silent garbage results. `str.len()` on an ELF build returns arbitrary values.

Impact: any ELF x86-64 `build` output that touches `str.len()`, `str.contains`, `map.get`, `vec.sort`, `strcmp`, etc. is silently wrong. This is the largest single functional hole found in any module so far.

### MEDIUM — The runtime-capability story (F-1B) is unwired end-to-end

Three separate drops, each confirmed:

1. **`runtime_parts_with_caps` is dead.** Defined at :103 (correctly sets `has_caps=true` + passes `caps_set`), but `compiler.fd:1693` unconditionally calls `runtime_parts()` → `has_caps=false`. Rust gates it at `src/compiler.rs:1788-1792` (`if let Some(caps) = granted_caps { runtime_parts_with_caps(&cap_set) }`).
2. **`build_executable` has no `granted_caps` parameter** (:1660) — Rust's takes `Option<&[String]>` (`src/compiler.rs:1704`). The capability set never enters the build pipeline at all.
3. **`strip_ungranted_caps` is dead** — defined `compiler.fd:2549`, header comment at `:10` claims "fully functional," but it has zero callers.

Net effect: the `if has_caps && !cap_granted(caps_set, "Net") { trap_stub }` gates sprinkled through the stub section (:3252+) **always evaluate false** — socket/http externs get benign `xor rax; ret` stubs instead of `ud2` traps. Runtime cap enforcement silently absent: an ungranted-Net program returns 0 from `p_socket_listen` rather than trapping.

### Verified faithful (hand-encoded portion — code inspection, byte-level)

- **`_start`**: stack align → call entry → `movl $60` (sys_exit) → `syscall` → `ud2` — correct.
- **Syscall numbers**: exit=60, write=1, mmap=9 — verified against Rust and the Linux table.
- **Hand-emitted builtins** (~115 `add_sym` symbols): `p_malloc`/`p_free` (mmap/munmap wrappers), `p_str_concat`, `p_print_int/float/bool`, `p_argc`/`p_argv`/`p_getenv`/`p_exit`, vec basics (`new/push/len/get/first/last/pop/is_empty`), `p_db_mem_*` (real impls with malloc/free calls), `p_simd_dot` (genuinely hand-encoded SSE2 — `movsd`/`mulsd`/`addsd` loop with correct REX.WXB SIB encoding), bare-metal I/O (`p_inb/p_outb/p_cli/p_sti/p_hlt`, `p_cr0/3/4` read/write, `p_idt_set_entry`, `p_invlpg` — real encodings, harmless on Linux).
- **Reloc machinery**: `g_runtime_relocations` set at :4368, read by :1694 (called after `runtime_parts` — ordering correct); RIP-rel LEA → `rtype 2` (R_X86_64_PC32), calls → `rtype 4`; the duplicate `pbool_true_lea` push at :4333/4336 (rtype 1 then re-pushed as 2 — comment admits the fix-up) is benign because rtype-1 entries are ignored downstream.

### Inherited / cosmetic

- `p_vec_new` allocates a fixed 4,096 bytes (`cap=509`, "no growth — enough for tests") — same comment + limitation in Rust.
- Stubs for `p_json_*`, `p_tensor_*`, most `p_simd_*`, `p_socket_*`, `p_http_*` — `xor rax; ret` bodies, largely matching Rust's own stub coverage.
- The asm-text-vs-bytes split exists in Rust too (same comment block, same layout) — the difference is purely that Rust executes the encode.

## Suggested fix order

1. Wire `elf64::encode_asm_block(runtime_asm)` at :2852 and merge `asm_bytes`/`asm_syms`/`asm_relocs` into `text`/`symbols`/`relocations` (copy the Rust :2876–2895 block). This single change restores 39 functions — strings, maps, `strcmp`, vec ops — on the ELF path.
2. Thread `granted_caps` through `build_executable` → call `runtime_parts_with_caps` when `Some`, and call `strip_ungranted_caps` on the module (Rust does both around `src/compiler.rs:1743-1790`). Restores F-1B.
3. Optional: when the linker hits an unknown symbol, emit a diagnostic instead of silently leaving `rel32=0` — a `call` to a missing runtime fn should be a link error, not a fall-through. (Hardens against the next dropped block.)

## Verification

Inspection-only — `release/fides-c3.exe` not present in this environment; `target/release/fidesc.exe` is the Rust reference and exercises the Rust side only. All findings derive from direct source comparison between `compiler/runtime_linux.fd` and `src/runtime_linux.rs` plus tracing the missing-symbol path through `compiler/linker.fd:776-806`. Confidence is high: the dropped block, the empty Vecs, the dead `with_caps` entry point, and the missing `granted_caps` parameter are all directly visible in source.
