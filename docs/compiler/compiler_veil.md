[All modules](index.md)

# compiler::veil -- Fides Documentation

## Contents

- [CovLineEntry](#CovLineEntry) (struct)
- [CovLineEntry_new](#CovLineEntry_new) (function)
- [FuncCount](#FuncCount) (struct)
- [FuncCount_new](#FuncCount_new) (function)
- [CoverageData](#CoverageData) (struct)
- [CoverageData_new](#CoverageData_new) (function)
- [CoverageRunResult](#CoverageRunResult) (struct)
- [CoverageRunResult_new](#CoverageRunResult_new) (function)
- [CoverageData_function_coverage_pct](#CoverageData_function_coverage_pct) (function)
- [CoverageData_line_coverage_pct](#CoverageData_line_coverage_pct) (function)
- [CoverageData_instruction_coverage_pct](#CoverageData_instruction_coverage_pct) (function)
- [CoverageData_mark_function_called](#CoverageData_mark_function_called) (function)
- [CoverageData_mark_line_covered](#CoverageData_mark_line_covered) (function)
- [CoverageData_mark_instruction_covered](#CoverageData_mark_instruction_covered) (function)
- [CoverageData_compute_func_counts](#CoverageData_compute_func_counts) (function)
- [collect_all_coverage](#collect_all_coverage) (function)
- [run_with_coverage](#run_with_coverage) (function)
- [veil_run](#veil_run) (function)
- [veil_print_help](#veil_print_help) (function)
- [generate_text_report](#generate_text_report) (function)
- [COVERAGE_CSS:](#COVERAGE_CSS:) (const)
- [generate_html_report](#generate_html_report) (function)
- [veil_main](#veil_main) (function)

## CovLineEntry

```fides
pub struct CovLineEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `str` |
| `b` | `i64` |

*Defined at line 10*

---

## CovLineEntry_new

```fides
pub fn CovLineEntry_new(a: str, b: i64) -> CovLineEntry {
```

**Parameters:**

- `a`: `str`
- `b`: `i64`

**Returns:** `CovLineEntry`

*Defined at line 15*

---

## FuncCount

```fides
pub struct FuncCount {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `total` | `i64` |
| `covered` | `i64` |

*Defined at line 21*

---

## FuncCount_new

```fides
pub fn FuncCount_new(name: str, total: i64, covered: i64) -> FuncCount {
```

**Parameters:**

- `name`: `str`
- `total`: `i64`
- `covered`: `i64`

**Returns:** `FuncCount`

*Defined at line 27*

---

## CoverageData

```fides
pub struct CoverageData {
```

**Fields:**

| Name | Type |
|------|------|
| `all_functions` | `Vec<str>` |
| `called_functions` | `Vec<str>` |
| `all_lines` | `Vec<CovLineEntry>` |
| `covered_lines` | `Vec<CovLineEntry>` |
| `all_instructions` | `Vec<CovLineEntry>` |
| `covered_instructions` | `Vec<CovLineEntry>` |
| `func_line_counts` | `Vec<FuncCount>` |
| `branches` | `str` |

*Defined at line 33*

---

## CoverageData_new

```fides
pub fn CoverageData_new() -> CoverageData {
```

**Returns:** `CoverageData`

*Defined at line 46*

---

## CoverageRunResult

Result of run_with_coverage: structural coverage data plus the interpreter
run outcome and an optional panic stack trace.
Ported from src/veil.rs:88 (Rust returns (CoverageData, RunResult, Option<String>)).

```fides
pub struct CoverageRunResult {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `CoverageData` |
| `status` | `str` |
| `trace` | `OptStr,  // panic stack trace if the run panicked` |

*Defined at line 55*

---

## CoverageRunResult_new

```fides
pub fn CoverageRunResult_new(data: CoverageData, status: str, trace: OptStr) -> CoverageRunResult {
```

**Parameters:**

- `data`: `CoverageData`
- `status`: `str`
- `trace`: `OptStr`

**Returns:** `CoverageRunResult`

*Defined at line 62*

---

## CoverageData_function_coverage_pct

```fides
pub fn CoverageData_function_coverage_pct(self: &CoverageData) -> f64 {
```

**Parameters:**

- `self`: `&CoverageData`

**Returns:** `f64`

*Defined at line 85*

---

## CoverageData_line_coverage_pct

```fides
pub fn CoverageData_line_coverage_pct(self: &CoverageData) -> f64 {
```

**Parameters:**

- `self`: `&CoverageData`

**Returns:** `f64`

*Defined at line 92*

---

## CoverageData_instruction_coverage_pct

```fides
pub fn CoverageData_instruction_coverage_pct(self: &CoverageData) -> f64 {
```

**Parameters:**

- `self`: `&CoverageData`

**Returns:** `f64`

*Defined at line 99*

---

## CoverageData_mark_function_called

```fides
pub fn CoverageData_mark_function_called(self: &mut CoverageData, name: str) -> void {
```

**Parameters:**

- `self`: `&mut CoverageData`
- `name`: `str`

**Returns:** `void`

*Defined at line 203*

---

## CoverageData_mark_line_covered

```fides
pub fn CoverageData_mark_line_covered(self: &mut CoverageData, func_name: str, line: i64) -> void {
```

**Parameters:**

- `self`: `&mut CoverageData`
- `func_name`: `str`
- `line`: `i64`

**Returns:** `void`

*Defined at line 209*

---

## CoverageData_mark_instruction_covered

```fides
pub fn CoverageData_mark_instruction_covered(self: &mut CoverageData, func_name: str, inst_id: i64) -> void {
```

**Parameters:**

- `self`: `&mut CoverageData`
- `func_name`: `str`
- `inst_id`: `i64`

**Returns:** `void`

*Defined at line 216*

---

## CoverageData_compute_func_counts

```fides
pub fn CoverageData_compute_func_counts(self: &mut CoverageData) -> void {
```

**Parameters:**

- `self`: `&mut CoverageData`

**Returns:** `void`

*Defined at line 223*

---

## collect_all_coverage

```fides
pub fn collect_all_coverage(module: &IrModule) -> CoverageData {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `CoverageData`

*Defined at line 259*

---

## run_with_coverage

Run the interpreter on `source` while collecting coverage data.
Ported from src/veil.rs:88 (Rust: run_with_coverage).

Steps:
1. Build IR from source via lower_to_ir_module.
2. Collect all structural coverage points via collect_all_coverage.
3. Run the interpreter with a debug hook that records covered
lines/instructions/functions.
4. Return (CoverageData, RunResult, Option<StackTrace>).

Interpreter_set_debug_hook makes the interpreter record each instruction
it runs (once); those give the covered instructions, functions and
source lines.

```fides
pub fn run_with_coverage(source: str, source_path: str) -> CoverageRunResult {
```

**Parameters:**

- `source`: `str`
- `source_path`: `str`

**Returns:** `CoverageRunResult`

*Defined at line 505*

---

## veil_run

```fides
pub fn veil_run(input_path: str, format: str, output_dir: str) -> str {
```

**Parameters:**

- `input_path`: `str`
- `format`: `str`
- `output_dir`: `str`

**Returns:** `str`

*Defined at line 580*

---

## veil_print_help

```fides
pub fn veil_print_help() -> str {
```

**Returns:** `str`

*Defined at line 646*

---

## generate_text_report

```fides
pub fn generate_text_report(data: &CoverageData, source: str) -> str {
```

**Parameters:**

- `data`: `&CoverageData`
- `source`: `str`

**Returns:** `str`

*Defined at line 650*

---

## COVERAGE_CSS:

```fides
pub const COVERAGE_CSS: str = "body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif; margin: 0; padding: 20px; color: #333; }\nh1 { color: #1a1a2e; }\n.file { color: #888; font-family: monospace; }\n.summary { display: flex; gap: 15px; margin: 20px 0; }\n.card { padding: 15px 25px; border-radius: 8px; text-align: center; min-width: 150px; }\n.card.full { background: #d4edda; border: 1px solid #c3e6cb; }\n.card.partial { background: #fff3cd; border: 1px solid #ffeaa7; }\n.card.low { background: #f8d7da; border: 1px solid #f5c6cb; }\n.card-label { font-size: 13px; color: #666; }\n.card-value { font-size: 28px; font-weight: bold; margin: 5px 0; }\n.card-detail { font-size: 12px; color: #888; }\ntable { border-collapse: collapse; width: 100%; margin: 15px 0; }\nth, td { padding: 8px 12px; text-align: left; border-bottom: 1px solid #eee; }\nth { background: #f5f5f5; }\ntr.full { background: #d4edda; }\ntr.none { background: #f8d7da; }\ntr.partial { background: #fff3cd; }\n.source { background: #f8f8f8; padding: 15px; border-radius: 5px; overflow-x: auto; font-family: 'Fira Code', monospace; font-size: 13px; line-height: 1.6; }\n.line { display: block; }\n.line .lnum { display: inline-block; width: 40px; color: #aaa; text-align: right; padding-right: 10px; user-select: none; }\n.line.covered { background: #e6ffed; }\n.line.uncovered { background: #ffeef0; }\n.line.neutral { background: transparent; }\nfooter { text-align: center; padding: 20px; color: #999; font-size: 13px; }\nfooter a { color: #1a1a2e; }\n";
```

*Defined at line 731*

---

## generate_html_report

```fides
pub fn generate_html_report(data: &CoverageData, source: str, filename: str) -> str {
```

**Parameters:**

- `data`: `&CoverageData`
- `source`: `str`
- `filename`: `str`

**Returns:** `str`

*Defined at line 732*

---

## veil_main

Standalone `veil.exe` entry; also serves `fid veil` (skips the
leading "veil" argument, and a "--" separator from `fid run`).

```fides
pub fn veil_main() -> i64 {
```

**Returns:** `i64`

*Defined at line 800*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
