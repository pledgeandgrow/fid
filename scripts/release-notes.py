#!/usr/bin/env python3
# CU6: generate release notes from docs/reference/changelog.md.
# Usage: release-notes.py [--all] [changelog-path]
#   Prints the newest '## ' section (the upcoming release) as markdown.
#   Lines mentioning 'breaking' are collected into a Breaking changes
#   section at the top, matching stability.md's promise to name them.
import sys, re

def sections(lines):
    out, cur = [], []
    for l in lines:
        if l.startswith('## ') and cur:
            out.append(cur); cur = [l]
        else:
            if cur or l.startswith('## '): cur.append(l)
    if cur: out.append(cur)
    return out

def render(sec):
    body = [l.rstrip() for l in sec]
    title = body[0][3:].strip()
    breaking = [l for l in body[1:] if re.search(r'(?i)breaking', l)]
    keep = [l for l in body[1:] if not re.search(r'(?i)breaking', l)]
    out = ['# Release notes — ' + title, '']
    if breaking:
        out += ['## Breaking changes', '']
        out += breaking + ['']
    out += [l for l in keep]
    return '\n'.join(out).rstrip() + '\n'

path = next((a for a in sys.argv[1:] if not a.startswith('-')),
            'docs/reference/changelog.md')
lines = open(path, encoding='utf-8').read().splitlines()
secs = sections(lines)
if not secs: sys.exit('no sections in ' + path)
if '--all' in sys.argv:
    for s in secs: print(render(s))
else:
    print(render(secs[0]), end='')
