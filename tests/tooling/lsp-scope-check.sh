#!/bin/bash
# TL4 (docs/roadmap/competitive-roadmap.md): scope-aware rename and
# references, references across files, method/imported/type inlay hints of
# `fid lsp` (tests/tooling/lsp_scope_probe.py drives the server).
#   FID=release/fid.exe bash tests/tooling/lsp-scope-check.sh
FID=${FID:-release/fid.exe}
PY=python3
command -v python3 > /dev/null || PY=python
exec "$PY" tests/tooling/lsp_scope_probe.py "$FID"
