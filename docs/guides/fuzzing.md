# Fuzzing with `fid test --fuzz`

A test file can hold fuzz targets beside its `fn test_*()` and
`fn bench_*()` functions: a function named `fuzz_*` that takes the input
bytes.

```fid
fn fuzz_parse(data: Vec<u8>) {
    // panic (or crash) on an input the code under test gets wrong
}
```

| Command | What it does |
|---|---|
| `fid test f.fd` | the tests, and every saved input of each fuzz target (below) as a test of its own |
| `fid test f.fd --fuzz` | also fuzzes each target: 100 inputs (`--seeds N`) |
| `fid fuzz f.fd` | the fuzz targets only, 1000 inputs each |
| `fid test f.fd -run fuzz_parse/<file>` | one saved input |

Options of a fuzz run: `--seeds N` (inputs per target), `--fuzz-seed N`
(seeds the mutator; 1 by default, so a run is the same each time),
`--fuzz-maxlen N` (longest input, 4096 bytes), `--fuzz-std` (counts the
blocks of stdlib modules too).

## How it works

The harness `fid test` builds is compiled with `ccompile --fuzz-cov`
(compiler/compiler.fd `fuzz_cov_program`): every block of the file's own
code (a function body, each branch of an `if`, also the missing `else`,
each loop body, each `match` arm) starts with a call that marks it
reached. Core and the stdlib are left alone unless `--fuzz-std` is given;
functions with declared effects or safety annotations are left as written.

The fuzz loop (compiler/main.fd `fz_harness_lib`, `fz_harness_driver`)
runs in one process:

1. The first inputs: the target's saved inputs, the empty input, a zero
   byte, four short random inputs and a long one.
2. Then, until the count is reached: a corpus input (the newest half the
   time) is mutated 1 to 4 times: a bit flipped, a byte set at random or to
   a byte parsers stop at (0, 255, digits, quotes, brackets...), a byte
   moved by up to 16, bytes inserted, deleted or copied within the input,
   a splice with another corpus input, the input cut short or grown, or a
   literal of the test file written over the input or inserted
   (`fz_dict_hex`: its integer literals as one or two bytes, its string
   literals of up to 32 bytes, as AFL's auto-dictionary).
3. An input that reaches a block no input reached before joins the
   corpus. So a check of a 3-byte magic value is passed one byte at a
   time, in a few thousand inputs; blind random inputs would need about
   16 million.

The run prints `N inputs, corpus C, B blocks reached`.

## Failing inputs are kept

Each input is written to a file before it runs. When a run fails (a
panic, a crash), `fid test` saves that input as
`testdata/fuzz/<target>/<hash>` beside the test file (the hash is FNV-1a
of the bytes, 16 hex digits), as Go does, and prints how to run it again.
From then on every `fid test` runs each saved input as the test
`<target>/<hash>`, with or without `--fuzz`: it fails until the bug is
fixed, and stays as a regression test afterwards. A fuzz run starts from
the saved inputs, so it reaches what they reach at once. A target one of
whose saved inputs fails is not fuzzed again until it passes.

Check them in with the test file.

## Checks

`scripts/tooling/sl13-fuzz-check.sh` (run by
`scripts/driver-goals-check.sh`) fuzzes `scripts/tooling/sl13_fuzz_tests.fd`:
the magic input is found within 20000 inputs and saved, the same input on
a second run, the saved input fails `fid test` until the bug is fixed and
passes after, it seeds the next fuzz run, and a passing target's corpus
grows.

Known bug (Nov 21, 2026, known-bugs 2g.83): `ccompile --fuzz-cov` panics
with `Double free` in the compiler (from `build_program_with_path_borrowck`),
so the coverage build of that check dies: no target is found, no input is
saved and no corpus grows.
