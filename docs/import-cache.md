# The import cache (CB1)

A compile of a large program spends much of its front end reading the
modules it imports: `fid check compiler/main.fd --time` reported 29 s of
`imports` out of 137 s (Nov 9, compiler built at -O0), nearly all of it
lexing and parsing the 60 compiler modules. The in-process modcache
(CB2) only helps a long-lived process (`fid serve`, `fid fix`, the LSP).
The import cache keeps each imported module's parse on disk, so a later
one-shot compile (`check`, `run`, `ccompile`, `dump-c`, `emit`, `build`)
neither lexes nor parses a module whose text has not changed.

## What is stored

`compiler/compiler.fd`, section "CB1: import cache on disk". When
`cached_parse` (the import loop's parse of a module) misses, the module is
parsed as before and its `Program` is written with the AST serializer
(`compiler/ast_ser.fd`, the one the core artifact uses) to
`fid-imp-<hash of path and compiler>.bin` in `FID_IMPCACHE_DIR`, else
`TEMP`/`TMPDIR`/`/tmp`. The file is written under another name and renamed,
so a reader never sees half a file. One file per module path: a new key
overwrites it.

## The key

A module's parse depends on its text and on what the parser knows when it
starts: the struct and enum names (`Name { .. }` is a literal or a block)
and the names modules are imported under (`name::f(..)` is a call of an
imported module's function or a method call). Only names that occur as
words of the module's text can change its parse. The key is the hash of:

- the text (FNV-1a, as `hash_source`),
- for each distinct word of the text, in order, whether it is a known
  struct, enum or import name (names that are not one word are all folded
  in),
- the compiler executable's hash (`inc_compiler_hash`): a rebuilt compiler
  starts over.

## Parser state

A parse also changes parser globals that later parses and passes read. On a
hit they are put back:

- the import-name set: the module's `use` names are parsed again as
  `use <name>;` lines (`imp_replay_names`);
- the `module::name` calls the parse recorded: kept with the entry and
  added to `g_imp_qcalls`, which `cc_qualify_shadowed_imports` reads before
  the parser's own list.

A parse that numbered a turbofish call (`f::<T>(..)`, global numbering),
saw `@secret`/`@sensitive` (the secrecy pass's fast skip) or an `@import`
of a C header is not kept. Because the parser's import-name set lives as
long as the process, the cache is used only by the first build of a
process, and never with the modcache. A failed parse turns it off for the
rest of the build.

## Switches and output

- `FID_IMPCACHE=off`: no read, no write (compare output with and without).
- `FID_IMPCACHE_DIR=<dir>`: where the files go.
- stderr: `[build] import cache: H hit(s), P parsed, S saved` after the
  imports of a build that used it.

The core artifact (CS2) is version 3 now: when nothing is merged into core
(`check`, `run`, `ccompile`, `dump-c`) it stores core once instead of the
same Program twice, so a hit decodes one Program.

## Checking it

`FID=<fid> bash tests/tooling/cb1-impcache-check.sh` (fixture
`tests/projects/cb1_impcache`): same output and same C with the cache off,
cold and warm; a warm build hits every module; a program knowing another
name that occurs in a module parses it again; an edited module is parsed
again; damaged files are ignored; the core artifact is written once and
read. `TIME=1` also times `fid check compiler/main.fd --time` cold and warm.

## Not done (CB1 is larger)

Type checking, comptime substitution, lowering and the C generator are not
cached or parallel, and builds are not incremental per function
(competitive CS1, CS4-CS6, complaint CB3). Decoding is not yet as fast as
it could be: `ar_i64` in `compiler/ast_ser.fd` allocates a Vec for every
integer it reads.
