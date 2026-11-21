#!/bin/bash
# ci-wiring-check.sh — the gates that roadmap goals put into CI are still
# wired in (the `gates` job of .github/workflows/ci.yml runs this; it reads
# the tree only). Each check names the goal; a workflow edit that drops one
# fails here instead of silently.
#
#   RA16  ci.yml natgate: ALL=1 nat-gate.sh on x86_64-linux (aarch64-linux
#         runs on real hardware in tier1.yml, riscv64-linux under qemu in
#         suite-all.yml)
#   ME17  ci.yml: z3-solver installed, scripts/rules-smt.sh run
#   SP15  bench.yml: scripts/compile-gate.sh, base records, head checked
#   CU7   ci.yml: doctest.py --emit for the native-only examples
#   CU8   bench.yml: bench/check.sh --ab, and scripts/bench-gate-check.sh
#         (the gate fails on an injected slowdown)
#   N1    ci.yml: scripts/ark-check.sh on Linux and Windows
#   A10   build-installer.sh checks release-fresh.sh before packaging
#   N8    build-installer.sh builds ark/scribe/veil from tools/*.fd
#   N10   release.yml: stdlib-docs.sh --zip, the zip attached
#   A12   verify.sh reruns long tests; scripts/long-tests.txt names them
#   A4    verify-weekly.yml: fidesc test --compiler, and its pass counts
#         compared with the baseline (scripts/suite-fid-compare.sh)
#   RL4   ci.yml: miscompile-gate.sh; bench.yml: its --before step with the
#         base commit's fid for the entries the change fixes (--new-since)
#   SP14  bench.yml: egraph-bound-check.sh (release within 2x of --debug)
#   RL2   every lane of scripts/suite-all.sh run on push and pull request
#         (tier1.yml, suite-all.yml; scripts/suite-lanes-check.sh)
#   F5    ci.yml gates: stage0-names-check.sh (sources parse with fidesc,
#         which every fid build and fides-os's CI start from)
#   I6    verify-nightly.yml: soak_http.py over a socket under wrk
#   M8    ci.yml: sanitize.sh on a sample; verify-nightly.yml: on the suite
#   N2    ci.yml: quickstart-check.sh on Linux and Windows
#   N4    ci.yml: fmt-check.sh, with its tests/fmt fixtures
#   N11   ci.yml: projects-check.sh on Linux and Windows, the five projects
#   BM5   verify-weekly.yml: service-bench-check.sh, bench/service/run.sh
#   BM6   verify-weekly.yml: compile-speed.sh at 1M lines
#   BM9   bench.yml: the --ab run-time gate also measures bench/game and
#         bench/work (SUITES), each of which has a program to measure
#   TL6   ci.yml: build-no-toolchain-check.sh with wine (REQUIRE_WINE=1),
#         and the suite as Windows PEs built on Linux (cross-test.sh)
#   CI4   ci.yml natgate (and suite-all/tier2 riscv64): the target's C
#         library for qemu-user, REQUIRE_LOADER=1 (no dynamic test skipped)
set -u
cd "$(dirname "$0")/.." || exit 1
bad=0
need() {
  # need <goal> <file> <fixed text> [what]
  if [ ! -f "$2" ] || ! tr -d '\r' < "$2" | grep -qF -- "$3"; then
    echo "MISSING $1: $2 has no '$3'${4:+ ($4)}"
    bad=$((bad + 1))
  fi
}
W=.github/workflows
need RA16 $W/ci.yml "bash scripts/nat-gate.sh \${{ matrix.target }}"
need RA16 $W/ci.yml 'ALL: "1"'
need RA16 $W/ci.yml "target: [x86_64-linux]"
need ME17 $W/ci.yml "pip install --quiet z3-solver"
need ME17 $W/ci.yml "bash scripts/rules-smt.sh release/fid-dev"
need ME17 $W/ci.yml 'REQUIRE_Z3: "1"'
need SP15 $W/bench.yml "bash scripts/compile-gate.sh --save"
need SP15 $W/bench.yml "COMPILE_GATE_BASE"
need SP15 scripts/compile-gate.sh 'COMPILE_GATE_BASE:-'
need CU7 $W/ci.yml "doctest.py stdlib --fid ./release/fid-dev --emit --native-only"
need CU8 $W/bench.yml "bash bench/check.sh --ab"
need CU8 $W/bench.yml "bash scripts/bench-gate-check.sh"
need CU8 scripts/bench-gate-check.sh "bench_main_once"
need N1 $W/ci.yml "bash scripts/ark-check.sh ./release/fid-dev"
need N1 $W/ci.yml "bash scripts/ark-check.sh ./release/fid-dev.exe"
need A10 installer/build-installer.sh "bash scripts/release-fresh.sh release/fid.exe"
need N8 installer/build-installer.sh 'ccompile "tools/$tool.fd"'
for t in ark scribe veil; do need N8 tools/$t.fd "${t}_main()"; done
need N10 $W/release.yml "bash scripts/stdlib-docs.sh --zip"
need N10 $W/release.yml "dist/fides-stdlib-docs.zip"
need N10 $W/ci.yml "scripts/stdlib-docs.sh --check"
need A12 verify.sh "scripts/long-tests.sh"
need A12 scripts/long-tests.txt "test_spawn_gt"
need A4 $W/verify-weekly.yml "fidesc test --compiler"
need A4 $W/verify-weekly.yml "bash scripts/suite-fid-compare.sh"
if ! SELFTEST=1 bash scripts/suite-fid-compare.sh >/dev/null 2>&1; then
  echo "BROKEN A4: scripts/suite-fid-compare.sh selftest fails"; bad=$((bad + 1))
fi
# RL4: the before-step of the miscompile gate runs in CI with the base fid
need RL4 $W/ci.yml "bash scripts/miscompile-gate.sh"
need RL4 $W/bench.yml '--before "$BASE_FID" --new-since'
need RL4 scripts/miscompile-gate.sh "--new-since) NEW_SINCE"
need SP14 $W/bench.yml "bash scripts/egraph-bound-check.sh"
# F5/G4: every fid build starts from fidesc; the gates job checks first that
# the sources do not use a word fidesc reserves where it cannot parse it
need F5 $W/ci.yml "bash scripts/stage0-names-check.sh"
if ! SELFTEST=1 bash scripts/stage0-names-check.sh >/dev/null 2>&1; then
  echo "BROKEN F5: scripts/stage0-names-check.sh selftest fails"; bad=$((bad + 1))
fi
# M8: ASan/UBSan through C, a sample on every push and the suite nightly
need M8 $W/ci.yml "bash scripts/sanitize.sh hello"
need M8 $W/verify-nightly.yml "run: bash scripts/sanitize.sh"
# I6: the socket soak under an external load generator, nightly
need I6 $W/verify-nightly.yml "python3 scripts/verify/soak_http.py --fid release/fid-dev --duration 1800 --loadgen wrk"
need I6 tests/verify/soak_http_service.fd "http_serve_scoped(port, soak_route)"
# N2, N11: the guide and the example projects on Linux and Windows
need N2 $W/ci.yml "bash scripts/quickstart-check.sh ./release/fid-dev"
need N2 $W/ci.yml "bash scripts/quickstart-check.sh ./release/fid-dev.exe"
need N11 $W/ci.yml "bash scripts/projects-check.sh ./release/fid-dev"
need N11 $W/ci.yml "bash scripts/projects-check.sh ./release/fid-dev.exe"
for p in cli library service game cbind; do
  [ -f "tests/projects/$p/expected.txt" ] || { echo "MISSING N11: tests/projects/$p/expected.txt"; bad=$((bad + 1)); }
done
# N4: fmt on the tree, and its fixtures (tests/fmt/<name>_in.fd -> <name>.fd)
need N4 $W/ci.yml "bash scripts/fmt-check.sh ./release/fid-dev"
need N4 scripts/fmt-check.sh "tests/fmt/*_in.fd"
for fx in tests/fmt/*_in.fd; do
  [ -f "${fx%_in.fd}.fd" ] || { echo "MISSING N4: $fx has no ${fx%_in.fd}.fd"; bad=$((bad + 1)); }
done
# BM5/BM6: the weekly service benchmarks under wrk and the 1M-line size
need BM5 $W/verify-weekly.yml "bash scripts/service-bench-check.sh"
need BM5 $W/verify-weekly.yml "bash bench/service/run.sh | tee build/bench/service-linux.md"
need BM6 $W/verify-weekly.yml 'SIZES: "1000000"'
# BM9: the 5% gate over the BM3/BM4 sets too
need BM9 $W/bench.yml 'SUITES: "game work"'
need BM9 $W/verify-weekly.yml "bash scripts/bench-noise-check.sh"
need BM9 bench/check.sh 'AB_REUSE'
# batch 7: BM10 results history and the website, RT11 idle connections,
# VF4 PEIR_* toggles on Windows
need BM10 $W/verify-weekly.yml "bash bench/history.sh build/bench/run-all.md"
need BM10 $W/verify-weekly.yml "name: bench-weekly-history"
need BM10 $W/pages.yml "bash bench/page-merge.sh"
need RT11 $W/verify-weekly.yml "bash bench/idle/run.sh"
need VF4 $W/verify-weekly.yml "variant: [peir-global-ra, peir-coalesce, peir-volatile-ra, peir-xmm-ra, peir-rotate]"
# batch 8: AD3 fides-os booted weekly (not per push: its drift is not the
# commit's), PL7 the suite on macOS, TL10 the editor configs exercised
need AD3 $W/bare-metal.yml "repository: pledgeandgrow/fides-os"
need AD3 $W/bare-metal.yml "run: bash scripts/fides-os-boot.sh"
need AD3 $W/bare-metal.yml "run: SELFTEST=1 bash scripts/fides-os-boot.sh"
need AD3 $W/bare-metal.yml "run: FROM=compiler/main.fd bash scripts/stage0-names-check.sh"
need AD3 scripts/fides-os-boot.sh "KERNELS=(minimal shell shell-input)"
if ! SELFTEST=1 bash scripts/fides-os-boot.sh >/dev/null 2>&1; then
  echo "BROKEN AD3: scripts/fides-os-boot.sh selftest fails"; bad=$((bad + 1))
fi
need PL7 $W/macos-native.yml "bash scripts/cross-test.sh"
need PL7 $W/macos-native.yml "bash scripts/stage0-names-check.sh"
need TL10 $W/editors.yml "bash scripts/tl/editor-check.sh"
# batch 10: CE4 the error-message ratchet, M9 the native path in the nightly fuzz
need CE4 $W/ci.yml "run: python3 scripts/diag-coverage.py"
need M9 $W/verify-nightly.yml "run: bash scripts/fuzz/nightly.sh"
need M9 scripts/fuzz/nightly.sh 'BACKENDS="$BACKENDS,emit:x86_64-linux"'
need B9 $W/verify-weekly.yml "run: FIXPOINT=1 bash scripts/inc-check.sh"
# batch 11: VF2 floats/strings/enums against C, VF7 mutants kept current
need VF2 $W/verify-nightly.yml 'seed.outputs.seed }}" --ext'
need VF7 $W/ci.yml "run: python3 scripts/verify/variants.py check"
need VF8 $W/ci.yml "run: python3 scripts/verify/coverage.py --self-test"
need VF8 $W/verify-weekly.yml "--goal 90 --goal-modules all"
for b in game/fides/fasta game/fides/binarytrees work/fides/compiler; do
  [ -f "bench/$b.fd" ] || { echo "MISSING BM3/BM4: bench/$b.fd"; bad=$((bad + 1)); }
done
for s in game work; do
  ls bench/$s/fides/*.fd > /dev/null 2>&1 || { echo "MISSING BM9: bench/$s/fides/*.fd"; bad=$((bad + 1)); }
done
# batch 13: PL2 the suite on the four tier-1 platforms per commit, RL14 the
# fixpoint through our own backend weekly, VF11 the ownership calculus
need PL2 $W/tier1.yml "run: |"
need PL2 $W/tier1.yml 'FID="release/fid-dev$x" bash scripts/tier1.sh'
for p in windows-x64 linux-x64 linux-arm64 macos-arm64; do
  need PL2 $W/tier1.yml "platform: $p"
done
need PL2 scripts/cross-test.sh "aarch64-qemu-aarch64|arm64-qemu-aarch64"
# PL2: runtime names imported dynamically fail the binary check; tests a
# target cannot run (lane-skips.txt) and dynamic binaries without a loader
# for qemu are SKIP with a reason, each with its self-test in tier1.yml
need PL2 $W/tier1.yml "tests/tooling/bin-target-check-selftest.py"
need PL2 $W/tier1.yml "bash tests/tooling/pl2-lane-skips-check.sh"
need PL2 scripts/cross-test.sh 'LANE_SKIPS="${LANE_SKIPS:-scripts/lane-skips.txt}"'
need PL2 scripts/cross-test.sh "bin-target-check.py --loader -"
need PL2 scripts/bin-target-check.py "def runtime_imports(d):"
# CI4: the native Linux suite runs every test, the dynamic ones too (the
# target's C library for qemu-user, REQUIRE_LOADER): x86_64-linux in
# ci.yml natgate, riscv64-linux under qemu in suite-all.yml
need CI4 $W/ci.yml 'REQUIRE_LOADER: "1"'
need CI4 scripts/nat-gate.sh 'if [ "${REQUIRE_LOADER:-}" = 1 ]'
need CI4 $W/suite-all.yml "libc6-riscv64-cross"
# PL3: Tier 2 builds and runs x86_64-macos (Mach-O checked); riscv64-linux
# moved to suite-all.yml (both its lanes under qemu)
need PL3 $W/tier2.yml "target: x86_64-macos"
need PL3 $W/tier2.yml "python3 scripts/macho-check.py"
need PL3 $W/tier2.yml 'bash scripts/cross-test.sh "$TARGET"'
# PL3: all four through C (scripts/tier2.sh), Windows ARM64 and FreeBSD
# built on Linux and run on a windows-11-arm runner and in a FreeBSD VM
need PL3 $W/tier2.yml 'LANES="c:$TARGET" FID=release/fid JOBS=3 bash scripts/tier2.sh'
need PL3 $W/tier2.yml 'LANES="c:aarch64-windows c:x86_64-freebsd" FID=release/fid JOBS=4 bash scripts/tier2.sh'
need PL3 $W/tier2.yml "runs-on: windows-11-arm"
need PL3 $W/tier2.yml "uses: vmactions/freebsd-vm@v1"
need PL3 $W/tier2.yml "COMPARE_ONLY=1 OUT=build/tier2/lanes/c-aarch64-windows bash scripts/cross-test.sh aarch64-windows"
need PL3 $W/tier2.yml "COMPARE_ONLY=1 OUT=build/tier2/lanes/c-x86_64-freebsd bash scripts/cross-test.sh x86_64-freebsd"
need PL3 scripts/cross-test.sh 'if [ "${COMPARE_ONLY:-}" = 1 ]; then'
need PL3 scripts/tier2.sh 'DEF_LANES="c:aarch64-windows c:x86_64-macos c:x86_64-freebsd c:riscv64-linux"'
need PL3 $W/tier2.yml "run: FID=release/fid bash tests/tooling/tier2-targets-check.sh"
need PL3 $W/tier2.yml "run: bash tests/tooling/tier2_compare_check.sh"
# RL2: every lane scripts/suite-all.sh knows runs on every push and pull
# request (tier1.yml's platforms, suite-all.yml's riscv64 and wasm32 lanes)
need RL2 $W/suite-all.yml 'run: bash scripts/suite-all.sh ${{ matrix.lanes }}'
need RL2 $W/suite-all.yml 'lanes: "c:riscv64-linux native:riscv64-linux"'
need RL2 $W/suite-all.yml 'lanes: "c:wasm32-wasi"'
need RL2 $W/suite-all.yml "run: bash tests/tooling/wasm-c-2g61-check.sh release/fid"
need RL2 scripts/suite-all.sh 'if [ "${LIST_LANES:-}" = 1 ]; then'
if ! bash scripts/suite-lanes-check.sh > /dev/null 2>&1; then
  echo "MISSING RL2: scripts/suite-lanes-check.sh: a lane runs on no push+PR workflow"
  bash scripts/suite-lanes-check.sh 2>&1 | grep '^MISSING' | sed 's/^/  /'
  bad=$((bad + 1))
fi
if ! SELFTEST=1 bash scripts/suite-lanes-check.sh > /dev/null 2>&1; then
  echo "BROKEN RL2: scripts/suite-lanes-check.sh selftest fails"; bad=$((bad + 1))
fi
# F7/DB9: the macOS runtime services and FID_PROF run on the Macs
need F7 $W/macos-native.yml "bash tests/nattools/machine_batch8_check.sh release/fid"
need F7 $W/macos-native.yml "test_machine_os_services"
# TL6: a Linux fid builds Windows PEs with no toolchain, run under wine
need TL6 $W/ci.yml "run: bash tests/tooling/build-no-toolchain-check.sh"
need TL6 $W/ci.yml 'REQUIRE_WINE: "1"'
need TL6 $W/ci.yml "bash scripts/cross-test.sh x86_64-windows-pe"
need TL6 tests/tooling/build-no-toolchain-check.sh "--target windows-x64"
need TL6 scripts/cross-test.sh '[ "$HOST" = windows ] || ntarget=(--target x86_64-windows)'
need RL14 $W/verify-weekly.yml "bash scripts/native-fixpoint.sh"
need RL14 $W/ci.yml "run: bash scripts/native-fixpoint-check.sh"
need VF11 $W/ci.yml "run: python3 scripts/verify/own_model.py --size 4 --self-test"
need VF11 $W/verify-weekly.yml "run: python3 scripts/verify/own_model.py --size 5"
need VF11 $W/verify-weekly.yml "python3 scripts/verify/own_diff.py --fid release/fid-dev --size 4 --per 60 --rejected 10 --out build/own-diff"
need RL14 $W/verify-weekly.yml "build/native-fixpoint/drift.txt"
# RL13: repro.sh / repro-hosts.sh verdicts with stand-in compilers
need RL13 $W/ci.yml "run: bash tests/tooling/rl13-repro-check.sh"
# CE4: the coverage script's self-test, the expected-fail tests' output and
# the asm! register errors of emit --bare
need CE4 $W/ci.yml "run: python3 scripts/diag-coverage.py --self-test"
need CE4 $W/ci.yml "run: python3 scripts/diag-tests-check.py --fid ./release/fid-dev --jobs 4 tests/bootstrap/test_diag_*_fail.fd"
need CE4 $W/ci.yml "run: FID=./release/fid-dev bash tests/tooling/ce4-asm-register-check.sh"
# every test long-tests.txt names exists
for name in $(tr -d '\r' < scripts/long-tests.txt | grep -v '^#' | awk '{print $1}'); do
  if [ ! -f "tests/selfhost/$name.fd" ] && [ ! -f "tests/bootstrap/$name.fd" ]; then
    echo "MISSING A12: scripts/long-tests.txt names $name, which is no test"
    bad=$((bad + 1))
  fi
done
if [ "$bad" = 0 ]; then echo "ci-wiring-check: every gate is wired"; else echo "ci-wiring-check: $bad missing"; fi
exit "$bad"
