# runtime_arm64.fd — AArch64 runtime of the PIR path

Re-audited 2026-11-09 (previous audit 2026-10-22/23).

- **Path:** `compiler/runtime_arm64.fd`
- **LOC:** 800
- **Role:** builds the AArch64 Linux runtime as asm text (`_start`,
  print helpers, allocator over mmap, vec basics, panic stubs, capability
  stubs) and assembles it with `arm64_enc`. Returns `RuntimeParts` and a
  relocation list. Port of `src/runtime_arm64.rs`. The working native
  runtime is `stdlib/linux_runtime/` (Fides code, used by nat_ir).

## Where it is called

| Caller | What |
|--------|------|
| `compiler.fd` `build_executable` (arm64 branch) | `runtime_parts_arm64()`, `runtime_parts_with_caps_arm64(bits)`, `runtime_relocations_arm64()` |

**Unreachable from the CLI** (same branch as arm64_enc.md).

## Findings

All code inspection.

1. **MEDIUM — assembler error returns empty parts — STILL TRUE**
   (:143-146, "defensive"). Rust panics.
2. **Capabilities — FIXED.** `build_executable` calls
   `runtime_parts_with_caps_arm64(caps_to_bits(granted_caps))` when caps
   are given, and `cap_granted_arm64` checks bits per capability (:58-63).
3. **LOW — stale relocation global on the error path** — not re-checked.
4. **INFO — layouts predate proposal 0004** (strings, Vec header); not
   re-checked.

## Fix order

None while unreachable; delete with the PIR path.

## Verification state

- Read: caps helpers, assemble call and error arm.
