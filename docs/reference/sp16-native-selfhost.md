# SP16: the compiler built by its own backend — what stopped it, what is slow

Own-backend roadmap SP16 (docs/roadmap/llvm-roadmap.md): the compiler
itself built by `fid emit` (pe_ir, or the machine layer with `--nat` /
`--target <arch>-linux`) instead of the C path, and fast.

Earlier work (Nov 16): every builtin and extern the compiler reaches
resolves on the machine layer (tests/nattools/selfhost_builtins_check.py),
the main stack is 256 MB (`--nat` PE reserve, RLIMIT_STACK raised on
Linux), and tests/nattools/machine_sp16_check.sh checks those parts.

## Nov 17: the parser crashed in every native image

`tests/nattools/selfhost_front_unit.fd` (the compiler's own lexer and
parser) built by `fid emit` crashed on its first expression, on pe_ir and
on the machine layer alike, while the C path's image ran:

    Panic: Invalid memory access (null or dangling pointer) at 0x6
        - Vec_share
        -| called from __own_dup_8
        -| called from __own_dup_53
        -| called from Parser_parse_primary

Bisected with the Nov 16 compiler on a snapshot of HEAD's `compiler/` and
`stdlib/` (the working tree's core had the regions hooks that compiler does
not know):

| build | result |
|-------|--------|
| `emit --nat` | crash |
| `emit` (pe_ir) | crash |
| `emit --nat --debug` (no mid-end) | right |
| every iropt pass named off, but not the default-on iropt_me ones | crash |
| also `loops,sra,loadhoist,forward,matchfacts,inline2,...` off | right |
| `FID_IROPT_OFF=sra` only, `--nat` and pe_ir | right |

The cause is the mid-end's scalar replacement of aggregates
(compiler/iropt_me.fd `ime_sra`, ME26). `return ParseResultExpr::Ok(e)`
builds the result in a stack struct: field 0 the tag, then
`Store(fieldptr(r, 1), expr_alloca, Some(Struct "Expr"))`, a store with an
explicit struct type that copies every word of the Expr into the payload
(lower.fd, enum literal, "Struct/enum-typed field"), then a word-by-word
copy of the result into the caller's slot. `ime_sra` takes every field
address that is only loaded from and stored to as one word: the stored
Expr became its *address* in an i64 local, and payload words 2..9 were
never written. The fix (in compiler/iropt_me.fd, the mid-end's file): a
field address is a word only when each load of it has a scalar type and
each store has no explicit type or a scalar one.

Regression test: tests/bootstrap/test_midend_sra_inline_payload.fd. With
the Nov 16 compiler it prints garbage and panics in `__own_drop_*` on
`--nat`, panics on pe_ir, and is right with `FID_IROPT_OFF=sra`.

With SRA off, the natively built front end prints the same token counts,
checksums and declaration counts as the C path's image on hello.fd,
compiler/token.fd, lexer.fd, parser.fd and nat_ir.fd.

## Speed of the natively built front end

Same snapshot, SRA off, lexer.fd + parser.fd + nat_ir.fd + lower.fd,
CPU time of the process (Windows, a busy machine; process start is about
180 ms of wall time for every image):

| image | user CPU |
|-------|----------|
| `emit --nat` | 440–550 ms |
| `emit` (pe_ir) | 480–590 ms |
| C path, `zig cc -O1` | 125 ms |

`FID_PROF=1` on the `--nat` image (333 samples, self time):

| function | self | note |
|----------|------|------|
| p_zero_words | 9.9% | the lowering clears each dropped owning struct local |
| p_str_hdr_len | 8.4% (17.1% total) | header check of every string length |
| p_str_len_word | 6.6% | a one-line predicate, called, not inlined |
| p_str_count_of | 4.5% | the same |
| pert_str_equal | 3.9% (18.6% total) | `==` on strings: two pe_len calls first |
| __own_drop_N, __own_dup_N, Vec_share, Str_dup | ~15% | ownership traffic |

What was done here: the machine layer stores the zeros itself
(compiler/nat_ir.fd `ng_zero_words_inline`): `p_zero_words(p, n)` with a
constant n of at most 16 words (4 with `--size`) is n word stores, no
call. `FID_NAT_OFF=zerowords` keeps the call. Test:
tests/bootstrap/test_machine_zero_words.fd and
tests/nattools/machine_zero_words_check.sh (output with and without, and
fewer calls in main's machine IR).

Still open for "as fast as the C path":

- The mid-end's inliner (iropt.fd `iro_inline`) inlines one-block bodies
  only, so `p_str_len_word` (`w >= 0 && w < 2^40`, two blocks) and
  `p_str_count_of` stay calls inside every string length; inline2 skips
  them too. zig cc inlines all of them.
- `pert_str_equal` (stdlib/linux_runtime/base.fd) takes both lengths
  through `pe_len` -> `p_str_hdr_len` before it compares; comparing the
  two header words first, when both strings are headered, would skip
  most of it.
- The whole compiler has not yet been built by `fid emit`
  (scripts/native-fixpoint.sh) with a compiler that has the SRA fix.

## How to check

    bash tests/nattools/machine_zero_words_check.sh $FID
    bash tests/nattools/machine_sp16_check.sh $FID          # BIG=1: timings
    BACKEND=nat FID=$FID bash scripts/native-fixpoint.sh
    BACKEND=pe  FID=$FID bash scripts/native-fixpoint.sh
