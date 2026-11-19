# Memory flame graphs

`fid run <file.fd> --mem` (and `fid ccompile ... --mem`) prints a memory
profile at exit: see runtime-reports.md, "Memory and allocation profile".
If the environment variable `FID_MEM_FOLDED` names a file, the program also
writes there the bytes allocated under each whole stack of Fides functions,
as collapsed stacks, one line per stack, outermost function first:

```
main;keep 512
main;fill 500
main;greet;p_str_alloc 48
```

Flame graph tools read this format as it is:

```bash
FID_MEM_FOLDED=mem.folded fid run app.fd --mem
flamegraph.pl --countname=bytes --title="bytes allocated" mem.folded > mem.svg
# or: inferno-flamegraph < mem.folded > mem.svg; or open it in speedscope
```

A width is the bytes asked for, not the bytes live at exit. Up to 32 Fides
frames are kept per stack (the innermost ones in deeper recursion); a stack
without any Fides frame is `(no Fides frame)`; the table holds 3072 stacks,
and bytes from further stacks are one `(other stacks)` line. Without
`FID_MEM_FOLDED` nothing is written and the stack is not walked a second
time. The code is `__p_ms_note` / `__p_ms_write` in `compiler/codegen_c.fd`
(under `P_MEMPROF`); `tests/memprof/check_folded.sh` checks it.

The CPU profile has the same output: `fid profile` (competitive TL7).
