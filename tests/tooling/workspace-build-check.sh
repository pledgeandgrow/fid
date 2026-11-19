#!/bin/bash
# L5F (docs/roadmap/roadmap.md, workspaces): `fid build` with no file at a
# workspace root builds every member (compiler/main.fd cmd_build hands a
# project directory to ark_main; compiler/ark.fd ark_workspace_run runs
# `fid build` in each member). The fixture's members are [[lib]] targets of
# kind "check", so no C compiler is needed.
#   FID=release/fid.exe bash tests/tooling/workspace-build-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
case "$FID" in
    /*|[A-Za-z]:*) ;;
    */*) FID="$(pwd)/$FID" ;;
esac
OUT=$(mktemp)
fail=0
( cd tests/projects/l5f_workspace && "$FID" build ) > "$OUT" 2>&1
code=$?
has() {
    if grep -qF -- "$2" "$OUT"; then echo "ok   $1"; else echo "FAIL $1: no line with: $2"; fail=1; fi
}
if [ $code -eq 0 ]; then echo "ok   fid build at the root succeeds"; else echo "FAIL fid build at the root: exit $code"; fail=1; fi
has "the members are found" "Workspace: 2 member(s)"
has "member app is built" "== app"
has "member libs/util is built" "== libs/util"
has "app's target" "ws_app"
has "util's target" "ws_util"
if [ "$(grep -c "Built 1 target(s)" "$OUT")" -eq 2 ]; then echo "ok   one target built in each member"; else echo "FAIL not one target built in each member"; fail=1; fi
# --dry-run is passed to every member
( cd tests/projects/l5f_workspace && "$FID" build --dry-run ) > "$OUT" 2>&1
if grep -qF " check src/util.fd" "$OUT" && grep -qF " check src/main.fd" "$OUT"; then echo "ok   flags reach the members"; else echo "FAIL --dry-run did not reach the members"; fail=1; fi
if [ $fail -ne 0 ]; then echo "--- output ---"; cat "$OUT"; fi
rm -f "$OUT"
exit $fail
