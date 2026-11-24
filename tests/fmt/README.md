# fmt fixtures (release-roadmap N4)

`scripts/fmt-check.sh` (step 0) formats each `<name>_in.fd` here and
compares the result with `<name>.fd`; `<name>.fd` must pass
`fid fmt --check` (formatting is stable), and both must print the same
under `fid run`.

- `toplevel`: a `;` at the top level ends its line. `fid fmt` used to keep
  top-level declarations on one line (`module m; use core; use vec;`), put
  the comment of the next line after them as a trailing comment, and,
  after a blank line, indent the next declaration by one space; a second
  run then joined that line too, so the output was not stable. Copies of
  the tree formatted by that version still show it (` use vec;`). A
  trailing comment after a top-level `;` stays there; a doc comment
  joined after one goes back to its own line.
- `comments`: a trailing comment stays after the `;`, `{`, `}` or field
  `,` it follows (it used to move under that line, as if it explained the
  next one); a comment of its own line keeps its own line, also after code
  with no `;` and inside an argument list (indented one level more); a doc
  comment or a section rule (`// ---`) that the old formatter joined onto
  the line before goes back to its own line. Also generic definitions:
  `<T, const N: usize>` keeps `const N` apart (it printed `constN`, a token
  change `fid fmt` refused) and the fields of `struct Grid<T, ..> {` go one
  per line. Both files print `2` and `19 g`.
- `prefix` (known bug 2g.62): a prefix `&`, `&mut`, `*`, `-`, `!` sticks
  to its operand (`& v` and `- 1` before), a postfix `?` to what it
  follows, a binary operator keeps its spaces, and a declaration after an
  annotation (and a blank line) starts its own line with no stray space.
  An annotation also keeps its own line when no blank line follows it
  (`@pure` / `fn f`): tests/bootstrap/test_fmt_prefix_ops.fd and
  tests/tooling/tools-2g62-check.sh check that.
- `literals` (BG101, BG102, BG103, BG107): what the tokenizer keeps whole.
  An f-string runs to its closing `"` past the strings inside its
  interpolations, as the lexer reads it (`f"{tag("k")}"` printed
  `f"{tag(" k ")}"`, a meaning change the token check missed: it used the
  same tokenizer); a lifetime name is one word (`&' a i64`); `'\x41'` and
  `'\u{1F600}'` are one char literal each; a block comment after `/` gets
  a space before it (`10 //* two */ 2` was a line comment, which the token
  check refused). Both files print `k and v`, `5`, `65`, `128512`, `5`.
- `semicolons` (BG174): a statement written with no `;` keeps its own
  line when the line ends where a statement can end (a name, a literal,
  `)`, `]`, `?`) and the next starts one (a name, a literal, `(`, `!`,
  `@`). The printer broke lines only after `; { }`, so reference.md
  §1.10's own example printed on one line and `val z = id(4)` with `(z)`
  on the next line became the call `id(4)(z)` (the parser reads a `(` on
  the next line as a new statement). A line that ends in an operator or
  a next line that starts with one is still joined (`w = w` / `- 1`,
  `x == 1` / `&& z == 4`). Both files print `1both` and `4 4 5`.
