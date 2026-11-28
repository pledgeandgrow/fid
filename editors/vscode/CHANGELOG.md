# Changelog

## 0.1.1

- File icon for `.fd` files (language default icon, used by file icon themes
  that fall back to language icons)

## 0.1.0

Initial release.

- Syntax highlighting for `.fd` files: keywords, f-string interpolation,
  raw strings (`r"..."`, `r#"..."#`), effect and safety annotations,
  doc comments
- Language server support through the compiler itself (`fid lsp`):
  diagnostics, completion, hover, go to definition, references, rename,
  quick fixes, signature help, inlay hints, formatting
- Snippets for functions, structs, enums, impl blocks, match, loops,
  spawn/await and bindings
- Comment toggling, bracket matching, auto-closing pairs, indentation
  and region folding
