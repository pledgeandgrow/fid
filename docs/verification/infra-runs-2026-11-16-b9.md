# Infra runs, Nov 16 (batch 9): RL3, RL12, RL13, RL15, RL25

The compiler used is the agents' build `fidh1.exe` (Nov 15 23:01), built
before the current working tree's driver and backend changes, on Windows
(zig cc 0.16, WSL, Node). No compiler was built for these runs.

## RL25: ABI against C, floats mixed with integers

New test `tests/bootstrap/test_verify_abi_c_mixed.fd`: `ldexp(f64, i32)`,
`frexp(f64, *i32)`, `modf(f64, *f64)`, `strtod(*u8, *i64)`, `fmod(f64, f64)`,
and a C result fed into the next call with an integer in between. Every
value is exact in binary and printed scaled to an integer. On Win64 the
i32 of `ldexp` goes in edx (by position), on System V in edi: a backend
that mixes the two conventions prints wrong numbers.

| Lane | Result |
|------|--------|
| C path, Windows (`ccompile`, zig cc) | pass: all 7 lines as expected |
| pe_ir (`emit`) | does not build: `ldexp`, `frexp`, `modf` are not in pe_ir's msvcrt import list (`pe_msvcrt_imports`, compiler/pe_ir_enc.fd); `fmod` and `strtod` are |
| `emit --nat` | does not build: nat_ir has no C import on Windows for names without a runtime counterpart |
| `emit --target x86_64-linux` | does not build with fidh1, and neither does `test_machine_cimport.fd` (same "not available in native Linux executables yet"): fidh1 predates or lacks the CI2/TG15 dynamic imports of the current source, so this is to be rerun with the final build |
| WASM (`ccompile --target wasm32-wasi`) | does not build with fidh1, and neither does `hello.fd` (the C includes mman.h/signal.h and uses SO_ERROR without the WASI emulation defines): a driver/C-runtime problem of that build, not of the test |
| macOS | no host |

The first version used `extern "C" link "m"` without `include "<math.h>"`:
the C path then declared `frexp(double, void*)` and zig cc rejected the
conflict with math.h. The test includes `<math.h>` as the other libm tests
do, and uses the C names directly (no `as` alias), the form
`test_machine_cimport.fd` uses on the machine layer.

## RL13: deterministic output, R1

`fid ccompile tests/bootstrap/hello.fd` twice (different directories and
TEMP, two seconds apart, `FIDES_NO_CACHE=1`) with fidh1: the executables
differ at file offset 0x80 (the PE time stamp) and around 0x61E00 (the
debug directory's PDB GUID), and a `.pdb` is written. fidh1 does not
contain the working tree's R1 fix (compiler/main.fd `repro_win_flags`:
`-Wl,-Brepro` and `-s` with zig cc, `--no-insert-timestamp` with gcc,
`SOURCE_DATE_EPOCH=0`; not in HEAD either). To close R1 with the final
build:

    FID=release/fid.exe NAT_TARGETS="" scripts/verify/repro.sh tests/bootstrap/hello.fd

The two-host part needs a second host:

    MANIFEST=build/m-win.txt scripts/verify/repro.sh      # Windows
    MANIFEST=build/m-linux.txt scripts/verify/repro.sh    # Linux
    scripts/verify/repro.sh --compare build/m-win.txt build/m-linux.txt

## RL3, RL12, RL15: not run

- RL3 (`NATIVE=1 scripts/pass-matrix.sh <target>`): the whole suite once
  per switch (about 55 switches) on each target is hours per target and
  needs the final compiler (the switch list comes from the current source,
  which fidh1 is not built from). Not run.
- RL12 (`scripts/verify/coverage.py --goal 90`): needs an instrumented
  compiler build, which this batch may not make. Not run.
- RL15 (`COMPILER=1 scripts/sanitize.sh`): builds the compiler's C with
  ASan/UBSan; needs that build and Linux or macOS clang (none on this host
  outside WSL). Not run.
