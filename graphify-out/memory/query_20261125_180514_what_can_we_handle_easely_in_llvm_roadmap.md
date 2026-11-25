---
type: "query"
date: "2026-11-25T18:05:14.886384+00:00"
question: "what can we handle easely in llvm roadmap"
contributor: "graphify"
outcome: "useful"
source_nodes: ["llvm-roadmap.md", "Track LF — The language's guarantees as optimizer facts (25)", "Track WP — The whole program (20)", "Track DB — Debugging and visibility (10)"]
---

# Q: what can we handle easely in llvm roadmap

## Answer

Expanded via vocab: [llvm roadmap easy low risk item goal backend native pass register allocation]. Easiest items are validation/completion rather than new architecture: finish the already-written LF8 range-set fix in compiler/checks.fd and run range-set/range-dom/range-loop; close WP7+WP14 through tests/tooling/wp-defaults-check.sh SUITE=1 NAT=1; validate LF10 paramrange and LF16 soleown with the midend pass suite; run the Windows DB6 pe_unwind_check leg. LF14 is the easiest small coding/debugging task (instrument iro_vec_sroa_one). Avoid MI14/MI17, TG16, ME3, SP cache/speed goals, and macOS-only legs if looking for easy work.

## Outcome

- Signal: useful

## Source Nodes

- llvm-roadmap.md
- Track LF — The language's guarantees as optimizer facts (25)
- Track WP — The whole program (20)
- Track DB — Debugging and visibility (10)