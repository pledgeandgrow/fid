//! Zed extension for Fides: the language server is `fid lsp`, the `fid`
//! found on the PATH of the worktree's shell (or the one named by the
//! `lsp.fid.binary.path` setting, which Zed applies itself).

use zed_extension_api::{self as zed, Result};

struct FidesExtension;

impl zed::Extension for FidesExtension {
    fn new() -> Self {
        FidesExtension
    }

    fn language_server_command(
        &mut self,
        _language_server_id: &zed::LanguageServerId,
        worktree: &zed::Worktree,
    ) -> Result<zed::Command> {
        let fid = worktree.which("fid").ok_or_else(|| {
            "fid is not on the PATH: install Fides (the installer puts fid on the PATH)".to_string()
        })?;
        Ok(zed::Command {
            command: fid,
            args: vec!["lsp".to_string()],
            env: worktree.shell_env(),
        })
    }
}

zed::register_extension!(FidesExtension);
