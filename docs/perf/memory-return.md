# Freed memory back to the system (RT7)

The allocators of Fid programs used to keep every page they ever took:
arena chunks were never released, so a program that held 100 MB once stayed
at 100 MB resident after freeing it. Now freed memory goes back to the
operating system on the C path and on the machine layer's runtime.

## C runtime (`compiler/codegen_c.fd`)

- **OS layer** (`emit_os_mem_runtime`): `__p_os_map` / `__p_os_unmap` /
  `__p_os_decommit` / `__p_os_recommit` over VirtualAlloc/VirtualFree on
  Windows and mmap/munmap/madvise on POSIX (Linux: `MADV_DONTNEED`; other
  POSIX: a `MAP_FIXED` remap). WASI and `-DP_NO_OSMEM`: calloc/free, no
  decommit.
- **Arena chunks** (4 MB) are pages of their own (`__p_os_map`), freed with
  `__p_arena_free_chunk`.
- **Large blocks** (over 1 MB) are pages of their own (class
  `P_CLASS_MAPPED`, layout `[span][0][0][header]` payload), unmapped by
  `p_free`. AddressSanitizer builds keep calloc (`P_CLASS_LARGE`).
- **Trim** (`emit_mem_return_runtime`, `__p_mem_trim_keep`): `p_free` counts
  the bytes on the thread's free lists (`__p_fl_bytes`); past
  `__p_trim_next` (8 MB, then what is left plus a quarter, at least 8 MB) the
  thread
  - unmaps its chunks whose every handed-out byte is on its free lists (but
    the current chunk and `P_KEEP_FREE_CHUNKS` = 2 empty ones), after taking
    their blocks off the lists;
  - decommits the inside pages of every other free block of 256 KB or more
    (`P_BLOCK_DECOMMITTED` in its freed header); `p_malloc` recommits such a
    block before reuse and clears only the bytes around its pages (they read
    zero).
  A block freed by another thread is on that thread's lists, so its chunk is
  not empty for its owner: a chunk is never unmapped while a block of it is
  in use.
- **Ended threads** (`__p_mem_thread_exit`, from `p_thread_wrapper`): give
  back their empty chunks (the current one too) and hand the rest, free
  lists and chunks, to a locked pool that the next thread to trim adopts
  (`__p_mem_adopt`). Before this the memory of an ended thread was lost.
- `p_arena_save` (stdlib `arena`) counts chunk positions: a thread that has
  taken a mark trims no more.
- `FID_MEM_RETURN=0` turns trimming off (large blocks are unmapped either
  way), to compare.

## Machine-layer runtime (`stdlib/linux_runtime/`)

One heap with one lock: `base.fd` keeps a table of the chunks `pe_bump`
maps; `pe_trim` (same thresholds) unmaps empty chunks. A freed block of
256 KB or more gets its inside 64 KB-aligned pages replaced at once by an
`mmap(MAP_FIXED)` (`pe_decommit`; 64 KB is a multiple of every page size of
the targets, so the call can neither fail on alignment nor round over the
next block). The OS layers: Linux natively, `win.fd` decommits and commits
again, `darwin.fd` maps over, `wasi.fd` answers ENOSYS (the block keeps its
pages). `FID_MEM_RETURN=0` turns the chunk trim off.

pe_ir's allocator (default Windows `emit`) is machine code in
`compiler/pe_ir_stubs.fd` and still keeps its chunks; its large blocks are
HeapAlloc'd and go back with HeapFree.

## `p_mem_resident()`

The process's resident bytes: the working set on Windows
(K32GetProcessMemoryInfo, found at run time), `/proc/self/statm` on Linux
(C and native), `task_info` on macOS (C). `-1` where it is not known: the
interpreter, pe_ir executables, the native Windows/macOS/WASI layers.

## Measured (Nov 17, splicing the new allocator into stage-0 C)

`tests/bootstrap/test_mem_return.fd`, Windows (zig cc -O2):

| case | before | held | after free |
|------|-------:|-----:|-----------:|
| 3072 x 32 KB strings | 3 MB | 100 MB | 19 MB (100 MB with `FID_MEM_RETURN=0`) |
| one 16 MB string | | +16 MB | released |
| 120 x 384 KB strings, chunks pinned | 100 MB | 134 MB | released (134 MB with `FID_MEM_RETURN=0`) |

The same on Linux (gcc -O2). A C stress harness (8 threads, 200k
alloc/free each, half the frees through another thread, sizes 1 B-3 MB)
ended at 4-6 MB resident on Windows and about 45 MB on Linux (glibc's own
arenas for the harness's bookkeeping included) against 803 MB on Linux
with trimming off, no data race
reported by ThreadSanitizer in the allocator's lists (one intended unlocked
read of the pool's "anything there" flag), 0 corrupted or non-zero reused
blocks; it ran in 2.4 s against 2.6 s with trimming off (fewer pages
touched).
