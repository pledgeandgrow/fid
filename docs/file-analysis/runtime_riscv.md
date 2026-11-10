# runtime_riscv.fd — RISC-V runtime of the PIR path

Re-audited 2026-11-09 (previous audit 2026-10-22/23).

- **Path:** `compiler/runtime_riscv.fd`
- **LOC:** 827
- **Role:** the RV64 twin of runtime_arm64: runtime asm text assembled by
  `riscv_enc`, returned as `RuntimeParts` + relocations. Port of
  `src/runtime_riscv.rs`. The working native runtime is
  `stdlib/linux_runtime/`.

## Where it is called

| Caller | What |
|--------|------|
| `compiler.fd` `build_executable` (riscv branch) | `runtime_parts_riscv()`, `runtime_parts_with_caps_riscv(bits)`, `runtime_relocations_riscv()` |

**Unreachable from the CLI** (same branch as riscv_enc.md).

## Findings

All code inspection.

1. **MEDIUM — assembler error returns empty parts — STILL TRUE**
   (:137-140).
2. **Capabilities — FIXED**: wired from `build_executable`, per-bit
   `cap_granted_riscv` (:56-61).
3. **Inherited — no `p_string_*` / `p_map_*`** — not re-checked.
4. **LOW — stale relocation global on error; `_start` saves before
   `addi sp`** — not re-checked.

## Fix order

None while unreachable; delete with the PIR path.

## Verification state

- Read: caps helpers, assemble call and error arm.
