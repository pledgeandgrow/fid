#!/bin/bash
# BG53 (docs/roadmap/bugs-roadmap.md): `fid lsp` decodes the document URI
# (%3A, %20, %2B, ...) before using it as a path, so a sibling module's
# `use` resolves (tests/tooling/lsp_uri_probe.py drives the server).
#   FID=release/fid.exe bash tests/tooling/lsp-uri-check.sh
FID=${FID:-release/fid.exe}
PY=python3
command -v python3 > /dev/null || PY=python
exec "$PY" tests/tooling/lsp_uri_probe.py "$FID"
