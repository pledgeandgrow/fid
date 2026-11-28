# Fid Language

[Fid](https://fid-lang.org) is a systems language that checks what it promises: borrow checking without lifetime annotations, capability grants on imports, effect tracking — compiling to native code with no LLVM and no GC. Its compiler, Fides, is written in Fid and builds itself to a byte-identical fixpoint.

This extension is the official VS Code support for Fid (`.fd`) files.

## Features

Everything beyond highlighting is powered by the compiler's own language server (`fid lsp`), so the answers come from the real parser and type checker — not a reimplementation:

- **Diagnostics** as you type, straight from the compiler, with **quick fixes** for the errors `fid fix` can repair
- **Completion**, **hover**, **go to definition**, **find references**, **rename**
- **Signature help** and **inlay hints** with parameter names
- **Formatting** — the same output as `fid fmt`
- **Syntax highlighting** — keywords, f-string interpolation, raw strings, effect annotations (`!IO`), safety annotations (`@conserve`), doc comments
- **Snippets** for the everyday shapes: `fn`, `struct`, `enum`, `impl`, `match`, `spawn`/`await`, loops

```fid
struct Point {
    x: i32,
    y: i32,
}

impl Point {
    fn new(x: i32, y: i32) -> Point {
        Point { x: x, y: y }
    }
}

fn main() {
    val p = Point::new(3, 4);
    println(f"p.x is {p.x}");
}
```

## Requirements

The extension highlights `.fd` files on its own. For diagnostics and everything else, install the Fid compiler — one of:

- **Windows**: the installer from [fid-lang.org/docs/install](https://fid-lang.org/docs/install) puts `fid` on your PATH
- **Linux / macOS / source**: the same page, or build the repository
- **No compiler at all?** Try the language in your browser first: [fid-lang.org/playground](https://fid-lang.org/playground)

When `fid` is not on your PATH, point the extension at it:

```json
{
  "fid.server.path": "/path/to/fid"
}
```

## Extension settings

| Setting | Default | What it does |
|---|---|---|
| `fid.server.path` | `fid` | The compiler that runs the language server (`fid lsp`). A full path when not on PATH. |

## Honest status

Fid is pre-1.0 (`v0.8.x`): the language server covers the methods above and is tested against the compiler on every commit, but the language itself is still moving — expect occasional rough edges, and please [file them](https://github.com/pledgeandgrow/fid/issues). This extension follows the compiler's releases.

## Links

- [fid-lang.org](https://fid-lang.org) — documentation, quick start, playground
- [github.com/pledgeandgrow/fid](https://github.com/pledgeandgrow/fid) — source, issues, discussions
- [Editor setup for Neovim, Helix, Emacs, Zed and more](https://fid-lang.org/docs/editor-setup)

## License

MIT — same as the compiler.
