import re
import sys

def extract_functions(filename):
    with open(filename, 'r', encoding='utf-8', errors='replace') as f:
        lines = f.readlines()
    funcs = {}
    i = 0
    while i < len(lines):
        line = lines[i].rstrip()
        # Match function definition: type name(params) {
        m = re.match(r'^(?:static\s+)?(?:inline\s+)?[\w\s\*]+?\b(\w+)\s*\([^)]*\)\s*\{', line)
        if m and not line.startswith('//') and not line.startswith('#'):
            fname = m.group(1)
            start = i
            depth = 1
            i += 1
            while i < len(lines) and depth > 0:
                if '{' in lines[i]: depth += lines[i].count('{')
                if '}' in lines[i]: depth -= lines[i].count('}')
                i += 1
            funcs[fname] = (start, i - start)
        else:
            i += 1
    return funcs

boot = extract_functions(sys.argv[1])
self = extract_functions(sys.argv[2])

all_names = sorted(set(boot.keys()) | set(self.keys()))
diffs = []
for name in all_names:
    bl = boot.get(name, (0, 0))[1]
    sl = self.get(name, (0, 0))[1]
    if bl != sl:
        diffs.append((name, bl, sl, bl - sl))

diffs.sort(key=lambda x: -abs(x[3]))
print(f"Total functions: boot={len(boot)} self={len(self)}")
print(f"Functions with different lengths: {len(diffs)}")
print(f"\nTop 30 by abs diff:")
print(f"{'Function':<50} {'Boot':>6} {'Self':>6} {'Diff':>6}")
for name, bl, sl, d in diffs[:30]:
    print(f"{name:<50} {bl:>6} {sl:>6} {d:>6}")
