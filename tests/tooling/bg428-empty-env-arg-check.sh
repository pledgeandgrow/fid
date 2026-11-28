#!/bin/bash
# BG428 (docs/roadmap/bugs-roadmap.md): under `fid run`, env_var of a
# variable set to the empty string and arg_at of an empty argument are
# Some(""), as on C and both native backends (compiler/interp.fd p_getenv /
# p_argv give interp_empty_read's string, which does not compare equal to
# 0). Before: None for both.
#   FID=release/fid.exe bash tests/tooling/bg428-empty-env-arg-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
P=tests/bootstrap/test_bg428_empty_env_arg.fd
fail=0
out=$(FID_BG428_EMPTY= "$FID" run "$P" '' 2>/dev/null | tr -d '\r')
if echo "$out" | grep -qx "some 0"; then echo "ok   empty variable is Some(\"\")"; else echo "FAIL empty variable: $out"; fail=1; fi
if echo "$out" | grep -qx "arg 0"; then echo "ok   empty argument is Some(\"\")"; else echo "FAIL empty argument: $out"; fail=1; fi
out2=$(env -u FID_BG428_EMPTY "$FID" run "$P" 2>/dev/null | tr -d '\r')
if echo "$out2" | grep -qx "none" && echo "$out2" | grep -qx "noarg"; then echo "ok   unset variable and no argument are None"; else echo "FAIL unset: $out2"; fail=1; fi
out3=$(FID_BG428_EMPTY=ab "$FID" run "$P" xyz 2>/dev/null | tr -d '\r')
if echo "$out3" | grep -qx "some 2" && echo "$out3" | grep -qx "arg 3"; then echo "ok   set values"; else echo "FAIL set values: $out3"; fail=1; fi
exit $fail
