# fides-os boot test, Nov 17 (competitive-roadmap AD3)

`scripts/fides-os-boot.sh` builds the fides-os kernels with the fid under
test (`fid emit kernel/<k>.fd --boot --load 0x1000000`) and boots them
with `qemu-system-x86_64 -kernel`. CI runs it in the `fides-os` job of
`.github/workflows/bare-metal.yml` on every push, every pull request and
once a week.

## What is checked

| case | kernel | COM1 must show |
|------|--------|----------------|
| `minimal` | kernel/minimal.fd | exactly `Hello from Fides OS!` |
| `shell` | kernel/shell.fd | the banner `Fides OS v0.1`, the last line of the command list, and the log ending at the prompt `> ` (every init function and the start-up tests ran) |
| `shell-input` | kernel/shell.fd (same build) | typed through the PS/2 keyboard with QEMU's `sendkey`: `> echo hi` then the line ` hi`, `mem`'s `Memory: kernel at 0x100000`, a `Timer ticks: N` line with N above 0, and the prompt again |

`shell-input` covers the keyboard and timer interrupt handlers, the scancode
ring buffer and the command loop. The other two cases only print.

For `shell-input`, QEMU's monitor reads from a FIFO. The script waits for
the prompt, sends the keys 0.2 s apart, waits until the output has not
grown for 2 s, and then sends `quit`.

`SELFTEST=1 bash scripts/fides-os-boot.sh` checks the output matcher on
built-in samples: a good boot, a boot that never reached the prompt, a
timer that never ticked, and an `echo` that printed nothing.
`scripts/ci-wiring-check.sh` runs that self-test and checks that the CI job
is still wired.

## The CI job's stage-0 step

The job builds fid with the Rust bootstrap compiler, which cannot parse
statements that start with some of fid's names (`guard = ...`). It starts
with `FROM=compiler/main.fd bash scripts/stage0-names-check.sh`, which
scans only the files that build reads: stdlib/core.fd and the `use` closure
of compiler/main.fd. With that scope, a stdlib module that the build never
parses can no longer stop the job. ci.yml's F5 step still scans the whole
tree.

On Nov 17 the scoped check finds 4 statements: compiler/comptime_eval.fd
2851, compiler/egraph.fd 2040, and compiler/ssa.fd 196 and 533, each a
`guard = guard + 1;`. Until these are renamed, the job stops at that step,
and so does every other CI build from stage 0.

## Run on Nov 17

This machine is Windows with QEMU 11.1.0 in WSL. The compiler was the
session's stage-0 fid, and the fides-os checkout was f14544d.

    FID=<fid> OUT=<dir> bash scripts/fides-os-boot.sh
    built    minimal: 13480 bytes in 0 s
    ok       minimal: COM1 has every expected line (1 checks)
    built    shell: 431464 bytes in 21 s
    ok       shell: COM1 has every expected line (3 checks)
    ok       shell-input: COM1 has every expected line (6 checks)
    fides-os-boot: 3/3 kernels ok

The same kernels booted inside WSL with the script running there too, as
on CI's Linux runner (`BUILD=0`): 3/3 ok.

Negative check: minimal.bin was copied in as shell.bin and run with
`BUILD=0 TIMEOUT=10`. Both shell cases failed with
`COM1 does not match "has:Fides OS v0.1"`, and the run ended 0/2.
