---
type: "query"
date: "2026-11-25T22:11:45.101408+00:00"
question: "what can we do next"
contributor: "graphify"
source_nodes: ["midend-pass-suite-check.sh", "inline2-suite-check.sh", "db6_eh_check.sh", "iro_vec_sroa_one()"]
---

# Q: what can we do next

## Answer

Expanded from original query via vocab: llvm roadmap easy validation suite partly verify pass inline version unwind debug. Recommendation: close validation-first items before architecture work: run WP7/WP14 wp-defaults full suite, then version/LF11 and inline2 ME20/ME21 suites; use LF10 paramrange as the next pass validation; LF14 Vec SROA and DB2/DB6 missing-subprogram declline are the best focused debugging tasks; investigate the fidesc BG194 interpreter hang because it blocks verify.sh.

## Source Nodes

- midend-pass-suite-check.sh
- inline2-suite-check.sh
- db6_eh_check.sh
- iro_vec_sroa_one()