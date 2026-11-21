import sys
path = sys.argv[1]
s = open(path, encoding="utf-8").read()
depth = 0
line = 1
i = 0
state = None
bad = None
while i < len(s):
    c = s[i]
    if c == "\n":
        line += 1
        i += 1
        continue
    if state == "str":
        if c == "\\":
            i += 2
            continue
        if c == '"':
            state = None
        i += 1
        continue
    if state == "lc":
        state = None if c == "\n" else "lc"
        i += 1
        continue
    if c == '"':
        state = "str"
        i += 1
        continue
    if c == "/" and i + 1 < len(s) and s[i + 1] == "/":
        state = "lc"
        continue
    if c == "{":
        depth += 1
    elif c == "}":
        depth -= 1
        if depth < 0:
            bad = line
            break
    i += 1
print(path, "final depth", depth, "first-negative line", bad)
