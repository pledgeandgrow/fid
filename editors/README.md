# Editor support

Every editor below talks to the same language server, `fid lsp` (diagnostics,
go to definition, hover, completion, rename). `fid` must be on the `PATH`.

| Editor | Files | Status |
|---|---|---|
| VS Code | `vscode/` (an extension) | tested in VS Code |
| Neovim 0.10+ | `neovim/fides.lua` | written, not yet tested |
| Helix | `helix/languages.toml` | written, not yet tested |
| Emacs 29+ (eglot) | `emacs/fides.el` | written, not yet tested |
| Sublime Text (LSP package) | `sublime/LSP-fides.sublime-settings` | written, not yet tested |
| JetBrains IDEs | see below | not yet tested |
| Zed | see below | not yet done |

## JetBrains (IntelliJ, CLion, RustRover...)

Install the LSP4IJ plugin, then *Settings > Languages & Frameworks > Language
Servers > +*: command `fid lsp`, and map the file name pattern `*.fd` to it.

## Zed

Zed loads a language only from an extension that also ships a Tree-sitter
grammar; Fides has none yet (roadmap TL10). Until then Zed can open `.fd` files
as plain text.
