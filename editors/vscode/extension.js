// Starts `fid lsp` for .fd files. Highlighting comes from the bundled
// TextMate grammar and needs nothing else; everything smarter
// (diagnostics, completion, hover, rename, formatting, ...) is the
// compiler's own language server, so the server must start for them.
const vscode = require('vscode');
const { LanguageClient, TransportKind } = require('vscode-languageclient/node');

let client;

function activate(context) {
  const command = vscode.workspace.getConfiguration('fid').get('server.path', 'fid');
  const server = { command, args: ['lsp'], transport: TransportKind.stdio };
  client = new LanguageClient('fid', 'Fid', { run: server, debug: server }, {
    documentSelector: [{ scheme: 'file', language: 'fid' }],
    outputChannelName: 'Fid',
  });
  context.subscriptions.push(client);
  client.start().catch(async (err) => {
    const pick = await vscode.window.showErrorMessage(
      `Fid: could not start "${command} lsp" — diagnostics, completion and hover need the Fid compiler.`,
      'Install Fid',
      'Open Settings',
      'Details'
    );
    if (pick === 'Install Fid') {
      vscode.env.openExternal(vscode.Uri.parse('https://fid-lang.org/docs/install'));
    } else if (pick === 'Open Settings') {
      vscode.commands.executeCommand('workbench.action.openSettings', 'fid.server.path');
    } else if (pick === 'Details') {
      vscode.window.showErrorMessage(`Fid language server failed to start: ${err.message}`, { modal: true });
    }
  });
}

function deactivate() {
  return client ? client.stop() : undefined;
}

module.exports = { activate, deactivate };
