# Editor support

Every editor below talks to the same language server, `fid lsp` (diagnostics,
go to definition, hover, completion, rename, references, formatting,
signature help, inlay hints with parameter names, and quick fixes for the
diagnostics `fid fix` can repair). `fid` must be on the `PATH`.
`scripts/tl/lsp_probe.py <fid>` checks the server's answers without an editor.

| Editor | Files | Status |
|---|---|---|
| VS Code | `vscode/` (an extension) | tested in VS Code |
| Neovim 0.10+ | `neovim/fides.lua` | written, not yet tested |
| Helix | `helix/languages.toml` | written, not yet tested |
| Emacs 29+ (eglot) | `emacs/fides.el` | written, not yet tested |
| Sublime Text (LSP package) | `sublime/LSP-fides.sublime-settings`, `sublime/Fides.sublime-syntax` | written, not yet tested |
| JetBrains IDEs | see below | not yet tested |
| Zed | `zed/` (an extension) with the grammar `tree-sitter-fides/` | written, not yet tested in Zed |

`scripts/tl/editor-check.sh` checks the configs without the editors (and,
with them installed, runs them headless); `tests/tooling/zed-check.sh`
checks the grammar and the Zed extension.

## Sublime Text

Copy `sublime/Fides.sublime-syntax` into the `Packages/User` folder: it gives
`.fd` files the scope `source.fides` that the LSP client's `selector` names
(without it a `.fd` file opens as Plain Text and the server never starts).

## JetBrains (IntelliJ, CLion, RustRover...)

Install the LSP4IJ plugin, then *Settings > Languages & Frameworks > Language
Servers > +*: command `fid lsp`, and map the file name pattern `*.fd` to it.

## Zed

Zed loads a language only from an extension that ships a Tree-sitter grammar.
`zed/` is that extension: `extension.toml` names the grammar
`tree-sitter-fides/` of this repository, `languages/fides/` holds the language
config and the highlights, brackets, indents and outline queries, and
`src/lib.rs` starts the language server, `fid lsp`, with the `fid` on the
worktree's PATH.

Zed compiles the grammar from its generated parser, so generate it first
(the tree-sitter CLI: `npm install -g tree-sitter-cli`):

```
cd editors/tree-sitter-fides
tree-sitter generate      # writes src/parser.c
tree-sitter test          # the corpus in test/corpus/
```

Then in Zed: *Extensions > Install Dev Extension* and pick `editors/zed`
(Rust must be installed through rustup). `extension.toml` fetches the grammar
from the repository at `rev = "main"`, so `src/` must be committed there; pin
`rev` to that commit before publishing the extension. The same grammar and
`tree-sitter-fides/queries/highlights.scm` also serve Neovim
(nvim-treesitter) and Helix.
