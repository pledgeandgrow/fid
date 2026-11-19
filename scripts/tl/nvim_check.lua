-- nvim_check.lua — editors/neovim/fides.lua exercised in a headless Neovim
-- (competitive-roadmap TL10). Run by scripts/tl/editor-check.sh:
--
--   nvim --headless -u NONE -i NONE -l scripts/tl/nvim_check.lua <config> <file.fd>
--
-- `fid` must be on PATH (the config starts `fid lsp`). Checks, one
-- "ok"/"FAIL" line each: the .fd file gets filetype "fides" and
-- commentstring "// %s", the client "fid" attaches, the server publishes
-- diagnostics for the file's error, and inlay hints are on for the buffer.
-- Exit status 1 on any FAIL.
local config, file = arg[1], arg[2]
local fails = 0
local function check(ok, what, detail)
  if ok then
    io.stdout:write("ok   nvim: " .. what .. "\n")
  else
    fails = fails + 1
    io.stdout:write("FAIL nvim: " .. what .. (detail and (" (" .. detail .. ")") or "") .. "\n")
  end
end

vim.cmd("filetype on")
dofile(config)
vim.cmd.edit(vim.fn.fnameescape(file))
local buf = vim.api.nvim_get_current_buf()
check(vim.bo[buf].filetype == "fides", "filetype of .fd is fides", "got " .. vim.bo[buf].filetype)
check(vim.bo[buf].commentstring == "// %s", "commentstring", "got " .. vim.bo[buf].commentstring)

local get_clients = vim.lsp.get_clients or vim.lsp.get_active_clients
local attached = vim.wait(30000, function()
  return #get_clients({ name = "fid", bufnr = buf }) > 0
end, 100)
check(attached, "client fid attached")
if attached then
  local c = get_clients({ name = "fid", bufnr = buf })[1]
  check(c.config.cmd[2] == "lsp", "server command is `fid lsp`", table.concat(c.config.cmd, " "))
  local got = vim.wait(30000, function() return #vim.diagnostic.get(buf) > 0 end, 100)
  check(got, "diagnostics published for the file's error")
  if got then
    local d = vim.diagnostic.get(buf)[1]
    io.stdout:write("     first diagnostic: line " .. (d.lnum + 1) .. ": " .. d.message:gsub("\n.*", "") .. "\n")
  end
  if vim.lsp.inlay_hint and vim.lsp.inlay_hint.is_enabled then
    -- 0.10 takes a bufnr, 0.11 a filter table
    local okc, on = pcall(vim.lsp.inlay_hint.is_enabled, { bufnr = buf })
    if not okc then on = vim.lsp.inlay_hint.is_enabled(buf) end
    check(on, "inlay hints enabled on the buffer")
  else
    check(false, "inlay hints enabled on the buffer", "this Neovim has no vim.lsp.inlay_hint (0.10+ needed)")
  end
end
for _, c in ipairs(get_clients()) do c.stop() end
vim.wait(2000, function() return #get_clients() == 0 end, 100)
os.exit(fails == 0 and 0 or 1)
