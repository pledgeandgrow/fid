# Infra runs, Nov 16 2026 (batch 6: L10D, BM5, BM6, BM8, BM9)

Runs on a busy shared Windows 11 machine (other sessions' compilers
running, Git Bash load average 8–13), `fid` = an agent's one-stage build of
the Nov 16 tree. Tables: docs/benchmarks.md, Nov 16 section.

| Goal | Check | Result |
|---|---|---|
| L10D | `node scripts/playground-embed-check.mjs` | 15 ok: share links in the playground's form, dedent, modes, init, example.html's 3 presets (hello, 5050, E044 rejected), tokens/ast/ir/effects, unknown mode |
| L10D | docs/playground/example.html in a browser, assets from localhost | autorun preset printed, IR mode, diagnostic shown (ANSI stripped), iframe of embed.html ran its `?code=` |
| BM5 | `scripts/service-bench-check.sh` | Fides (C), Go and Node selftests give the same bodies; queries clamp; run.sh: every cell a number, every reply checked |
| BM5 | `fid emit bench/service/fides/server.fd` (Windows) | fails: pe_ir has no p_signal_check / p_signal_register (stdlib/http.fd) |
| BM6 | `bench/compile-speed.sh`, hello / 10k / 100k | first attempt: Fides and Zig failed at 10k (functions named f32, f64, f80, f128); gen_lines.py now names them w0, w1, ...; second run complete |
| BM6 | 1M lines | not run (about an hour per Fides build at these rates); verify-weekly runs it in a step of its own |
| BM9 | game and work sets with fidh1 | all 7 build with emit and ccompile and print the same number; added to bench.yml's --ab gate (`SUITES: "game work"`) |
| BM9 | `scripts/bench-noise-check.sh`, 2 rounds, 5% | no false failure; largest difference sieve +5.1% (passed on the 5 ms slack) |
| BM8 | quiet-machine Windows run | not done: the machine was never quiet |
| all | `scripts/ci-wiring-check.sh` | every gate wired (BM5, BM6, BM9 lines added) |
