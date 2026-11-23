# `fid build` on Linux and macOS (CB4)

On a Linux or macOS host, `fid build <file.fd> -o <out>` with no `--target`
builds for this machine through the machine layer (compiler/nat_ir.fd and
nat_enc.fd): a static ELF, or an ad-hoc signed Mach-O, written by Fides
itself, with no C compiler and no linker. The host is
`host_native_target()` in compiler/main.fd (`uname -m`; `FID_HOST_ARCH`
overrides it): x86_64/aarch64/riscv64-linux, x86_64/aarch64-macos. The build
prints `[build] Host target: <target>`.

When the machine layer rejects the program (most often a runtime builtin
that its runtime, stdlib/linux_runtime, does not have yet: "not available in
native Linux executables yet: ..."), the build goes on through the C route,
as `--via-c` would, and says so:

```
[build] note: native backend: not available in native Linux executables yet: p_xyz
[build] The machine layer cannot build this program yet: falling back to the C compiler (as --via-c)
```

The second front end runs without the native runtime merged and with `@cfg`
seeing the host (`native_runtime_reset` in compiler/compiler.fd). An
explicit `--target` never falls back. A crash inside the machine layer is
not caught: `--via-c` (or `FIDES_BUILD_VIA_C=1`) takes the C route from the
start.

`FID_NAT_REJECT=1` makes up a rejection of a host build, to test the
fallback with any program.

Checks:

- `tests/tooling/build-no-toolchain-check.sh`: with only shell tools on PATH,
  `fid build hello.fd` runs and prints what `fid run` prints; `--via-c` takes
  the C route; with a C compiler on PATH, a rejected build falls back.
- `tests/tooling/build-host-suite.sh`: `fid build` with no flags over the
  test suite, each executable's output and exit code compared with
  `fid run`'s; it reports how many built natively and how many fell back.
  On Windows run it in WSL with a Linux `fid`.
