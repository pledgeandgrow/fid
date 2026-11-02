// Starts `fid lsp` for .fd files.
const vscode = require('vscode');
const { LanguageClient, TransportKind } = require('vscode-languageclient/node');

let client;

function activate(context) {
  const command = vscode.workspace.getConfiguration('fid').get('server.path', 'fid');
  const server = { command, args: ['lsp'], transport: TransportKind.stdio };
  client = new LanguageClient('fid', 'Fid', { run: server, debug: server }, {
    documentSelector: [{ scheme: 'file', language: 'fid' }],
  });
  context.subscriptions.push(client);
  client.start().catch((err) => {
    vscode.window.showErrorMessage(
      `Fid: could not start "${command} lsp". Install Fid, or set fid.server.path. (${err.message})`
    );
  });
}

function deactivate() {
  return client ? client.stop() : undefined;
}

module.exports = { activate, deactivate };
