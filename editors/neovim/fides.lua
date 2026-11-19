-- Fides in Neovim (0.10+): .fd files, and `fid lsp` as their language server.
-- Copy to ~/.config/nvim/plugin/fides.lua, or require it from init.lua.
vim.filetype.add({ extension = { fd = "fides" } })

vim.api.nvim_create_autocmd("FileType", {
  pattern = "fides",
  callback = function(args)
    vim.bo[args.buf].commentstring = "// %s"
    vim.lsp.start({
      name = "fid",
      cmd = { vim.g.fid_path or "fid", "lsp" },
      root_dir = vim.fs.root(args.buf, { "ark.toml", ".git" }) or vim.fn.getcwd(),
      -- parameter-name hints at call sites (fid lsp answers inlayHint);
      -- quick fixes come with vim.lsp.buf.code_action()
      on_attach = function(_, bufnr)
        if vim.lsp.inlay_hint then
          vim.lsp.inlay_hint.enable(true, { bufnr = bufnr })
        end
      end,
    })
  end,
})
