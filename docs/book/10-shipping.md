# 10. Shipping

A Fid program ships as one executable. There is no runtime to install
next to it and no virtual machine: the standard library and the runtime
are compiled into the file. This chapter is about getting that file built
for the machines you deploy to, and checking it before it leaves your
hands.

## Two ways to build

| | `fid emit` | `fid ccompile` |
|---|---|---|
| Needs | nothing | a C compiler in `CC` (zig cc covers every target) |
| Writes | the executable directly | C, then calls the C compiler |
| Targets | Windows x64, Linux x86-64 / AArch64 / RISC-V 64, macOS, WASM (with `fid wasm`) | whatever your C compiler targets |
| Links C libraries (`extern "C" link`) | no | yes |
| Use it when | you want a small static binary and no toolchain | you need a C library, or `emit` refuses something |

Both run the same front end, so a program either compiles on both or is
rejected with the same errors. They have separate back ends, so test the
one you ship.

## A program to ship

```fides
// wc.fd: count lines, words and bytes of a file
fn is_space(b: i64) -> bool {
    return b == ' ' || b == '\n' || b == '\t' || b == '\r';
}

fn main() -> i64 {
    val path: str = arg_at(1) ?? "";
    if str_len(path) == 0 {
        println("usage: wc <file>");
        return 2;
    }
    val text: str = read_file(path) ?? "";
    var lines: i64 = 0;
    var words: i64 = 0;
    var in_word: bool = false;
    for i in 0..str_len(text) {
        val b: i64 = str_byte_at(text, i);
        if b == '\n' { lines = lines + 1; }
        if is_space(b) {
            in_word = false;
        } else if !in_word {
            in_word = true;
            words = words + 1;
        }
    }
    println(f"{lines} {words} {str_len(text)} {path}");
    return 0;
}
```

`main`'s return value is the exit status, and `arg_at(i)` returns the
`i`-th command-line argument (0 is the program itself) as an
`Option<str>`.

### Windows

```
$ fid emit wc.fd -o wc.exe
Built native executable: wc.exe
$ printf 'one two three\nfour five\n' > poem.txt
$ ./wc.exe poem.txt
2 5 24 poem.txt
$ ./wc.exe; echo $?
usage: wc <file>
2
```

That `wc.exe` is 36,864 bytes and names only two DLLs, `kernel32.dll`
and `msvcrt.dll`, both part of Windows.

### Linux, from any host

```
$ fid emit wc.fd --target x86_64-linux -o wc
Built native executable: wc
$ ./wc poem.txt              # on Linux, or `wsl ./wc poem.txt` on Windows
2 5 24 poem.txt
```

The result is a static ELF (50,752 bytes here) with no libc dependency:
copy it to the server and run it. `--target aarch64-linux` and
`--target riscv64-linux` work the same way; test those under
`qemu-aarch64`/`qemu-riscv64` or on the hardware.

Through C, `zig cc` gives a static musl executable for the same targets:

```bash
CC="zig cc" fid ccompile wc.fd --target aarch64-linux -o wc-arm64
```

This one is far bigger (about 4.9 MB) because it carries debug
information; strip it if size matters.

### macOS

```bash
fid emit wc.fd --target aarch64-macos -o wc-mac      # or x86_64-macos
```

writes a Mach-O executable from any host. Be aware that no macOS build
has yet been *run* on a Mac by the project (`docs/reference/platforms.md`
lists it as present, not supported). Test on a Mac before you ship to
one, and expect to sign it yourself.

### WebAssembly

```
$ fid wasm wc.fd -o wc.wasm
Emitted WASM binary: wc.wasm (51718 bytes)
$ node scripts/wasm-run.mjs wc.wasm poem.txt
2 5 24 poem.txt
```

The module targets WASI preview 1 and runs under any WASI host (Node,
wasmtime, wasmer). Tasks switch stacks (since Nov 18), threads run to
their end when started, and there are no processes, signals or new
sockets. WASI through C
(`CC="zig cc" fid ccompile --target wasm32-wasi`) also works: the
`SO_ERROR` failure this chapter reported on Nov 15 no longer reproduces
(known-bugs 2g.61, Nov 20).

## What the platforms are worth

`docs/reference/platforms.md` sorts targets into tiers by how much they
are tested. Roughly:

- **Windows x64** through `emit`, the interpreter and C, and **Linux
  x64** through the interpreter and C are the best tested.
- **Linux x64, AArch64 and RISC-V 64 native** (`emit --target …-linux`)
  pass most of the test suite (around 270 of 317 tests matched the
  interpreter in the last run) and are built without a C toolchain.
- **macOS and WASI through C** (`ccompile --target`) build and are
  partly tested (macos.yml has run the suite on macOS runners since
  Nov 20); **native macOS, `fid wasm` and bare metal** build, and are not
  yet supported.

Ship on a tier-1 or tier-2 target, and keep your own tests running on
exactly that target.

## Before you ship

**1. Run your tests on the build you ship.** `fid test` runs through C.
If you ship `emit` binaries, also run the program built with `emit` on
real inputs and compare its output with `fid run`'s. That is what the
project itself does for every target (`scripts/cross-test.sh` compares
each test's output and exit code with the interpreter's); for your
program, a short shell loop over sample inputs is enough.

**2. Check memory.** `fid run app.fd --leaks` and `--checked-alloc`
(chapter 4) under a realistic workload.

**3. Know your unsafe code.** `fid audit app.fd` lists every `unsafe`
site your program reaches.

**4. Decide what a panic means.** A panic exits with status 1 and a
message on stderr (chapter 6). For a service, that means a supervisor
restarts it: systemd `Restart=on-failure`, a container restart policy,
a Windows service recovery action.

**5. Record what built it.** Put `fid version` in the build log, and
pin the compiler: the language is not yet stable
(`docs/reference/stability.md`). Builds are incremental — an unchanged
build says `… is up to date` — so a clean release build is
`--no-cache` (or `FIDES_NO_CACHE=1`).

**6. Publish checksums.** `sha256sum wc wc.exe > SHA256SUMS.txt`, so
people can check what they downloaded.

## Configuration at run time

Read the environment with `env_var(name)` and files with `read_file`
(both `Option<str>`, chapter 6). The `args`, `env`, `toml`, `ini` and
`json` modules in `stdlib/` parse the rest; chapter 9 shows a service
wiring them together with the precedence flag > environment > file.

## Shipping the compiler itself

If you build Fides from source instead of using the installer,
`RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh` builds it in three stages
and checks that stages 2 and 3 produce identical C (the fixpoint) before
installing `release/fid.exe`. The project's own release steps —
installer, signing, checksums — are in `docs/reference/releasing.md`.

## Common mistakes

- **`emit` on Windows refusing a program.** If `fid emit` says
  `not available in native Windows executables: <name>`, the program
  uses a C function the Windows back end does not have (in builds of
  Nov 15, `pe_c_memchr` for `str_find`; a Nov 21 build has it). Build
  that program with `ccompile`, or with `emit --target x86_64-linux` for
  Linux.
- **Shipping a `ccompile --debug` build.** It is `-O0`. The default is
  `-O2`.
- **Expecting `extern "C"` libraries in an `emit` build.** Only
  `ccompile` links them.
- **Trusting a target you haven't run.** "It built" is not "it works" on
  macOS or RISC-V. Run it there.
