# runtime.fd — Fides-authored runtime layer (NEW, 531 LOC)

> **Created 2026-10-23** as the consolidation target for the three
> hand-written runtimes (runtime_linux 4,370 LOC / runtime_arm64 794 /
> runtime_riscv 825 — each builtin encoded 3× in raw bytes/text-asm).
> Implements builtin *logic* once in Fides; per-arch keeps only a thin
> asm stub layer.

- **Path:** `compiler/runtime.fd` — **module `compiler::runtime`**
- **Status:** scaffold — typechecks clean under `fidesc run`; NOT yet
  wired into any build path.

## Architecture

```
user code → core.fd (Vec/String/Map — already Fides over p_malloc)
         → p_* builtins = THIS FILE (Fides logic, unsafe ptr ops)
         → rt_sys_* ABI boundary → per-arch asm stubs (~150 LOC each)
```

`rt_sys_*` extern block declares the full per-arch contract: write/read/
open/close/mmap/exit/argc/argv/getenv/clock_ns, plus deferred ctx-switch
and atomic ops (documented in file header with per-arch syscall numbers
and register conventions for x86-64/aarch64/riscv64 Linux + Win64 notes).

## What it implements

- `p_malloc`/`p_free`/`p_alloc` — first-fit free-list allocator over
  `rt_sys_mmap` (24B block header [size|free|next], 16B align, 64KB
  grow chunks). **No coalescing yet** — documented limitation.
- `p_memset`/`p_memcpy` — byte loops.
- String layer: `p_str_length` (strlen), `p_str_concat`, `p_str_equal`,
  `p_str_compare`, `p_to_str` (itoa), `p_char_to_str`, `p_bool_to_str`.
- I/O: `p_write_str`, `p_print_str`/`p_eprint_str`/`p_print`/`p_eprint`,
  `p_print_int`/`p_print_bool`/`p_print_newline`, `p_panic` (msg + exit 1),
  `p_exit`.
- Files (Linux flag numbers): `p_file_write`/`p_file_append`/`p_file_exists`/
  `p_file_read` (doubling buffer read loop).
- Process: `p_argc`/`p_argv`/`p_getenv`, `p_clock_now`/`_ms`/`_us`.

## Integration contract (not yet wired)

- Native pipeline should prepend this file to the program (same mechanism
  as core.fd) and link the per-arch stub object; `p_*` definitions then
  shadow the builtin extern decls.
- C backend does NOT use this file — codegen_c emits its own C runtime.
- `--no-std`/bare-metal skips it (bare mode keeps pe.fd's stub set).

## Open work (tracked in-file)

~140 builtins remain beyond the scaffold: sb/join/string-ops, float
formatting (needs dtoa), maps/channels (need rt_futex or spin-lock),
green threads (needs rt_ctx_save/restore + scheduler over this file),
sockets/net/http/db/dl/tensor/grad.

## Verification

- Typecheck: clean via `use compiler::runtime` probe (fidesc run, exit 0).
- Allocator/string/itoa logic exercised indirectly through that probe
  (p_malloc/p_free/p_to_str/p_str_length run on interp builtins — the
  Fides bodies themselves verified by reading, not yet linked natively).
