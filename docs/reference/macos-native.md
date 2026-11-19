# macOS executables from the machine layer

`fid emit <file> --target aarch64-macos` (or `x86_64-macos`) writes a
signed 64-bit Mach-O with no C compiler and no linker: compiler/nat_ir.fd
makes the code, compiler/nat_enc.fd `nat_macho_img` lays out the image
through compiler/image_writer.fd. This page covers two parts of it:
calling C functions in system libraries, and universal (fat) binaries.

## C functions in dylibs (TG15)

The runtime (stdlib/linux_runtime/ with darwin.fd on top) calls
libSystem through a fixed list of imports (`ng_mac_imports`). A function
of an `extern "C"` block that the runtime does not provide
(`pert_c_<name>`, `nat_c_<name>`) becomes an import of its own:

- a GOT slot in `__DATA`, which dyld fills at load time from the bind
  opcodes of `LC_DYLD_INFO_ONLY` (the classic bind opcodes, not chained
  fixups: dyld still reads them for executables on both architectures);
- the dylib: `link "<lib>"` names it (`nat_macho_dylib_path`). No `link`,
  or `c`, `m`, `pthread`, `dl`, `System`: libSystem. A path (`/...`,
  `@rpath/...`, `@executable_path/...`) stays as written. `Name.framework`
  or `framework:Name`: `/System/Library/Frameworks/Name.framework/Name`.
  `lib<name>.dylib`-style names (`z.1.dylib`): under /usr/lib. Anything
  else: `/usr/lib/lib<name>.dylib`;
- one `LC_LOAD_DYLIB` per dylib, libSystem first (ordinal 1), the others
  in order of first use (ordinals 2 and up, `SET_DYLIB_ORDINAL_IMM`, or
  `_ULEB` past 15: `img_macho_bind_ords`);
- a stub per (function, signature) moves the arguments from Fides'
  internal convention to the C one (System V on x86-64, AAPCS64 on arm64)
  and calls through the slot: the stubs of the Linux targets
  (`ng_gen_ldyn_stubs`).

An entry of `nat_macho`'s import list is the bare symbol (libSystem) or
`<dylib path>|<symbol>` (`nat_macho_import`). An image importing only
from libSystem is byte for byte what earlier compilers wrote.

Limits: up to 6 integer and 8 floating-point arguments; no variadic C
functions on arm64 (Apple passes variadic arguments on the stack); a
missing symbol or dylib stops the program at launch (dyld's message), as
binds are not lazy.

Checks: `fid run tests/nattools/macho_dylib_unit.fd` (the writer, any
host); `python3 tests/nattools/macho_imports_check.py <exe> <names...>
<dylib paths...>` on an emitted file (also `otool -l` / `dyld_info
-fixups` when present); tests/nattools/macho_dylib_prog.fd on a Mac.

## Universal binaries (L2B)

`fid emit <file> --target universal -o app` (also `universal-macos`,
`universal-darwin`, `universal-apple-darwin`; `fid build` the same)
writes one file with an x86-64 and an arm64 slice: `lipo -info app` says
`x86_64 arm64`. The compiler builds the program once per slice (itself,
with `--target x86_64-macos` and `--target aarch64-macos` and every
other flag kept, so `@cfg` sees each slice's architecture), then
`nat_macho_fat` (compiler/nat_enc.fd) writes the big-endian fat header
(magic `0xCAFEBABE`, one `fat_arch` per slice, alignment 2^14) and each
slice at a 16 KB boundary. Each slice keeps its own ad-hoc code
signature; the wrapper is not signed (macOS does not sign fat headers).
`--cpu <name>` goes to the slices whose target knows the name (an Apple
core to the arm64 slice, an x86-64 level to the other); a name neither
knows is an error.

Checks: `fid run tests/nattools/macho_fat_unit.fd` (the writer);
`FID=<fid> bash tests/nattools/universal_check.sh` (the driver: a fat
file whose slices equal the thin builds; on a Mac, `lipo -info` and both
slices run).
