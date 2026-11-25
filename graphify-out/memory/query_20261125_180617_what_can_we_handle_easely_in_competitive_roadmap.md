---
type: "query"
date: "2026-11-25T18:06:17.042911+00:00"
question: "what can we handle easely in competitive roadmap"
contributor: "graphify"
outcome: "dead_end"
source_nodes: ["competitive-roadmap.md", "BM3: Benchmarks Game set (`SUITE=game RUNS=1 bash bench/run.sh`)"]
---

# Q: what can we handle easely in competitive roadmap

## Answer

Expanded from original query via graph vocab: [competitive, roadmap, completion, remaining, left, benchmark, editor, validation, zed, lsp, ark, veil]. The graph had useful verification nodes for BM3/BM4 but not enough item-level granularity for a full easiest-items answer; direct inspection of docs/roadmap/competitive-roadmap.md and docs/benchmarks.md was needed. Conclusion: easiest remaining competitive-roadmap work is closeout/validation rather than core compiler engineering: TL4 is effectively implemented and its probe passes; TL10 needs only Zed extension compilation/version update plus validation; BM3 and BM7 appear substantively complete but the roadmap entry is stale; SL17/SL4/TL6 are validation-lane closeouts; AD1 and VF5 are the best bounded coding tasks. CS, LG, SL6, PL and formal-verification items are not easy.

## Outcome

- Signal: dead_end

## Source Nodes

- competitive-roadmap.md
- BM3: Benchmarks Game set (`SUITE=game RUNS=1 bash bench/run.sh`)