const fs = require('fs');
function extractFunc(lines, name) {
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i].replace(/\r$/, '').trimEnd();
    // Match: name(...) {
    if (line.match(new RegExp('^' + name + '\\s*\\([^;]*\\)\\s*\\{\\s*$'))) {
      let depth = 1; let j = i + 1;
      while (j < lines.length && depth > 0) {
        depth += (lines[j].match(/{/g) || []).length;
        depth -= (lines[j].match(/}/g) || []).length;
        j++;
      }
      return lines.slice(i, j);
    }
  }
  return null;
}
const boot = fs.readFileSync('pe_bootstrap.c', 'utf8').split('\n');
const self = fs.readFileSync('pe_selfhost.c', 'utf8').split('\n');
const fname = process.argv[2] || 'IrFunctionEntry_new';
const bf = extractFunc(boot, fname);
const sf = extractFunc(self, fname);
console.log('=== BOOTSTRAP (' + (bf ? bf.length : 0) + ' lines) ===');
if (bf) for (let i = 0; i < bf.length; i++) console.log((i+1).toString().padStart(4) + ': ' + bf[i].replace(/\r/,''));
console.log('=== SELFHOST (' + (sf ? sf.length : 0) + ' lines) ===');
if (sf) for (let i = 0; i < sf.length; i++) console.log((i+1).toString().padStart(4) + ': ' + sf[i].replace(/\r/,''));
