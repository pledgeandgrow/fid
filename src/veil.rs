// veil — Code coverage tool for the Fides programming language
//
// Instruments the interpreter to track which instructions, lines, and
// functions are executed. Reports coverage as text or HTML.
//
// Usage:
//   veil <file.fd> [--format text|html] [--output <dir>]
//
// What it tracks:
//   - Function coverage: which functions were called
//   - Line coverage: which source lines were executed
//   - Instruction coverage: which IR instructions ran

use std::collections::{BTreeMap, BTreeSet};
use std::fs;
use std::path::Path;
use std::sync::{Arc, Mutex};

use crate::compiler;
use crate::interp;
use crate::ir::{IrModule, IrFunction, ValueId};

// ─── Coverage data ──────────────────────────────────────────────────

#[derive(Debug, Default)]
pub struct CoverageData {
    /// All functions in the program
    pub all_functions: BTreeSet<String>,
    /// Functions that were called
    pub called_functions: BTreeSet<String>,
    /// All source lines that have executable instructions
    pub all_lines: BTreeSet<(String, usize)>,  // (func_name, line)
    /// Source lines that were executed
    pub covered_lines: BTreeSet<(String, usize)>,
    /// All instruction IDs
    pub all_instructions: BTreeSet<(String, u32)>,  // (func_name, inst_id)
    /// Instructions that were executed
    pub covered_instructions: BTreeSet<(String, u32)>,
    /// Per-function line counts: (total, covered)
    pub func_line_counts: BTreeMap<String, (usize, usize)>,
}

impl CoverageData {
    pub fn function_coverage_pct(&self) -> f64 {
        if self.all_functions.is_empty() { return 0.0; }
        100.0 * self.called_functions.len() as f64 / self.all_functions.len() as f64
    }

    pub fn line_coverage_pct(&self) -> f64 {
        if self.all_lines.is_empty() { return 0.0; }
        100.0 * self.covered_lines.len() as f64 / self.all_lines.len() as f64
    }

    pub fn instruction_coverage_pct(&self) -> f64 {
        if self.all_instructions.is_empty() { return 0.0; }
        100.0 * self.covered_instructions.len() as f64 / self.all_instructions.len() as f64
    }
}

// ─── Collect all coverage points from IR ────────────────────────────

pub fn collect_all_coverage(module: &IrModule) -> CoverageData {
    let mut data = CoverageData::default();

    for (name, func) in &module.functions {
        data.all_functions.insert(name.clone());

        for block in &func.blocks {
            for inst_id in &block.instructions {
                // Record instruction
                data.all_instructions.insert((name.clone(), inst_id.0));

                // Record source line if available
                if let Some(meta) = func.values.get(inst_id) {
                    if let Some(line) = meta.source_line {
                        data.all_lines.insert((name.clone(), line));
                    }
                }
            }
        }
    }

    data
}

// ─── Run with coverage tracking ─────────────────────────────────────

pub fn run_with_coverage(source: &str, source_path: &str) -> Result<(CoverageData, interp::RunResult, Option<String>), Vec<String>> {
    // Build IR
    let program = compiler::build_program_with_path(source, Some(source_path))?;
    let module = crate::lower::lower(&program);

    // Collect all coverage points
    let mut data = collect_all_coverage(&module);

    // Run interpreter with a coverage hook
    let covered_lines = Arc::new(Mutex::new(BTreeSet::new()));
    let covered_instructions = Arc::new(Mutex::new(BTreeSet::new()));
    let called_functions = Arc::new(Mutex::new(BTreeSet::new()));

    let cl = Arc::clone(&covered_lines);
    let ci = Arc::clone(&covered_instructions);
    let cf = Arc::clone(&called_functions);

    let hook = move |func_name: &str, _block: &str, source_line: usize, inst_id: u32,
                _values: &std::collections::HashMap<ValueId, interp::Value>,
                _func: &IrFunction| {
        cf.lock().unwrap().insert(func_name.to_string());
        if source_line > 0 {
            cl.lock().unwrap().insert((func_name.to_string(), source_line));
        }
        ci.lock().unwrap().insert((func_name.to_string(), inst_id));
        interp::DebugAction::Continue
    };

    let mut interpreter = interp::Interpreter::new(module);
    interpreter.set_debug_hook(Box::new(hook));
    let result = interpreter.run();
    let trace = interpreter.take_panic_stack_trace();

    data.covered_lines = covered_lines.lock().unwrap().clone();
    data.covered_instructions = covered_instructions.lock().unwrap().clone();
    data.called_functions = called_functions.lock().unwrap().clone();

    // Compute per-function line counts
    for func_name in &data.all_functions {
        let total = data.all_lines.iter()
            .filter(|(f, _)| f == func_name)
            .count();
        let covered = data.covered_lines.iter()
            .filter(|(f, _)| f == func_name)
            .count();
        data.func_line_counts.insert(func_name.clone(), (total, covered));
    }

    Ok((data, result, trace))
}

// ─── Text report ────────────────────────────────────────────────────

pub fn generate_text_report(data: &CoverageData, source: &str) -> String {
    let mut report = String::new();

    report.push_str("veil — Coverage Report\n");
    report.push_str("=======================\n\n");

    // Summary
    report.push_str("Summary\n");
    report.push_str("-------\n");
    report.push_str(&format!("Functions:     {}/{} ({:.1}%)\n",
        data.called_functions.len(), data.all_functions.len(), data.function_coverage_pct()));
    report.push_str(&format!("Lines:         {}/{} ({:.1}%)\n",
        data.covered_lines.len(), data.all_lines.len(), data.line_coverage_pct()));
    report.push_str(&format!("Instructions:  {}/{} ({:.1}%)\n",
        data.covered_instructions.len(), data.all_instructions.len(), data.instruction_coverage_pct()));
    report.push('\n');

    // Per-function breakdown
    report.push_str("Per-Function Coverage\n");
    report.push_str("---------------------\n");
    report.push_str(&format!("{:<30} {:>10} {:>10} {:>10}\n", "Function", "Lines", "Covered", "Percent"));
    report.push_str(&"-".repeat(62));
    report.push('\n');

    for (name, (total, covered)) in &data.func_line_counts {
        let pct = if *total > 0 { 100.0 * *covered as f64 / *total as f64 } else { 0.0 };
        let marker = if pct == 100.0 { " ✓" } else if pct == 0.0 { " ✗" } else { "" };
        report.push_str(&format!("{:<30} {:>10} {:>10} {:>9.1}%{}\n",
            name, total, covered, pct, marker));
    }
    report.push('\n');

    // Uncovered lines
    let uncovered: Vec<_> = data.all_lines.difference(&data.covered_lines).collect();
    if !uncovered.is_empty() {
        report.push_str("Uncovered Lines\n");
        report.push_str("---------------\n");
        for (func, line) in uncovered {
            report.push_str(&format!("  {}:{}\n", func, line));
        }
        report.push('\n');
    }

    // Source listing with coverage markers
    report.push_str("Source Coverage\n");
    report.push_str("---------------\n");
    let lines: Vec<&str> = source.lines().collect();
    for (i, line) in lines.iter().enumerate() {
        let line_num = i + 1;
        let is_executable = data.all_lines.iter().any(|(_, l)| *l == line_num);
        let is_covered = data.covered_lines.iter().any(|(_, l)| *l == line_num);

        let marker = if !is_executable { "  " }
            else if is_covered { "+ " }
            else { "! " };

        report.push_str(&format!("{}{:>4}  {}\n", marker, line_num, line));
    }

    report
}

// ─── HTML report ────────────────────────────────────────────────────

pub fn generate_html_report(data: &CoverageData, source: &str, filename: &str) -> String {
    let mut html = String::new();

    html.push_str("<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n");
    html.push_str(&format!("<title>Coverage Report — {}</title>\n", escape_html(filename)));
    html.push_str("<meta charset=\"utf-8\">\n");
    html.push_str("<style>\n");
    html.push_str(COVERAGE_CSS);
    html.push_str("</style>\n");
    html.push_str("</head>\n<body>\n");

    // Header
    html.push_str("<h1>Coverage Report</h1>\n");
    html.push_str(&format!("<p class=\"file\">{}</p>\n", escape_html(filename)));

    // Summary cards
    html.push_str("<div class=\"summary\">\n");
    html.push_str(&summary_card("Functions", data.called_functions.len(), data.all_functions.len(), data.function_coverage_pct()));
    html.push_str(&summary_card("Lines", data.covered_lines.len(), data.all_lines.len(), data.line_coverage_pct()));
    html.push_str(&summary_card("Instructions", data.covered_instructions.len(), data.all_instructions.len(), data.instruction_coverage_pct()));
    html.push_str("</div>\n");

    // Per-function table
    html.push_str("<h2>Per-Function Coverage</h2>\n");
    html.push_str("<table>\n<thead><tr><th>Function</th><th>Lines</th><th>Covered</th><th>Coverage</th></tr></thead>\n<tbody>\n");
    for (name, (total, covered)) in &data.func_line_counts {
        let pct = if *total > 0 { 100.0 * *covered as f64 / *total as f64 } else { 0.0 };
        let cls = if pct == 100.0 { "full" } else if pct == 0.0 { "none" } else { "partial" };
        html.push_str(&format!("<tr class=\"{}\"><td>{}</td><td>{}</td><td>{}</td><td>{:.1}%</td></tr>\n",
            cls, escape_html(name), total, covered, pct));
    }
    html.push_str("</tbody>\n</table>\n");

    // Source listing
    html.push_str("<h2>Source</h2>\n");
    html.push_str("<pre class=\"source\">\n");
    let lines: Vec<&str> = source.lines().collect();
    for (i, line) in lines.iter().enumerate() {
        let line_num = i + 1;
        let is_executable = data.all_lines.iter().any(|(_, l)| *l == line_num);
        let is_covered = data.covered_lines.iter().any(|(_, l)| *l == line_num);

        let cls = if !is_executable { "neutral" }
            else if is_covered { "covered" }
            else { "uncovered" };

        html.push_str(&format!("<span class=\"line {}\"><span class=\"lnum\">{}</span> {}</span>\n",
            cls, line_num, escape_html(line)));
    }
    html.push_str("</pre>\n");

    html.push_str("<footer><p>Generated by <a href=\"https://fides.dev\">veil</a> — Fides coverage tool</p></footer>\n");
    html.push_str("</body>\n</html>\n");

    html
}

fn summary_card(label: &str, covered: usize, total: usize, pct: f64) -> String {
    let cls = if pct == 100.0 { "full" } else if pct < 50.0 { "low" } else { "partial" };
    format!("<div class=\"card {}\"><div class=\"card-label\">{}</div><div class=\"card-value\">{:.1}%</div><div class=\"card-detail\">{} / {}</div></div>\n",
        cls, label, pct, covered, total)
}

fn escape_html(s: &str) -> String {
    s.replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;")
}

const COVERAGE_CSS: &str = r#"
body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif; margin: 0; padding: 20px; color: #333; }
h1 { color: #1a1a2e; }
.file { color: #888; font-family: monospace; }
.summary { display: flex; gap: 15px; margin: 20px 0; }
.card { padding: 15px 25px; border-radius: 8px; text-align: center; min-width: 150px; }
.card.full { background: #d4edda; border: 1px solid #c3e6cb; }
.card.partial { background: #fff3cd; border: 1px solid #ffeaa7; }
.card.low { background: #f8d7da; border: 1px solid #f5c6cb; }
.card-label { font-size: 13px; color: #666; }
.card-value { font-size: 28px; font-weight: bold; margin: 5px 0; }
.card-detail { font-size: 12px; color: #888; }
table { border-collapse: collapse; width: 100%; margin: 15px 0; }
th, td { padding: 8px 12px; text-align: left; border-bottom: 1px solid #eee; }
th { background: #f5f5f5; }
tr.full { background: #d4edda; }
tr.none { background: #f8d7da; }
tr.partial { background: #fff3cd; }
.source { background: #f8f8f8; padding: 15px; border-radius: 5px; overflow-x: auto; font-family: 'Fira Code', monospace; font-size: 13px; line-height: 1.6; }
.line { display: block; }
.line .lnum { display: inline-block; width: 40px; color: #aaa; text-align: right; padding-right: 10px; user-select: none; }
.line.covered { background: #e6ffed; }
.line.uncovered { background: #ffeef0; }
.line.neutral { background: transparent; }
footer { text-align: center; padding: 20px; color: #999; font-size: 13px; }
footer a { color: #1a1a2e; }
"#;

// ─── Main entry ─────────────────────────────────────────────────────

pub fn run(input: &Path, format: &str, output_dir: &Path) -> Result<(), String> {
    let source = fs::read_to_string(input)
        .map_err(|e| format!("Cannot read {}: {}", input.display(), e))?;
    let source_path = input.to_str().unwrap_or("");

    let (data, result, output) = run_with_coverage(&source, source_path)
        .map_err(|errors| errors.join("\n"))?;

    // Print program output
    if let Some(out) = &output {
        print!("{}", out);
    }

    // Generate report
    fs::create_dir_all(output_dir)
        .map_err(|e| format!("Cannot create output dir: {}", e))?;

    let filename = input.file_name().and_then(|s| s.to_str()).unwrap_or("program.fd");

    match format {
        "html" => {
            let html = generate_html_report(&data, &source, filename);
            let path = output_dir.join("coverage.html");
            fs::write(&path, html)
                .map_err(|e| format!("Cannot write {}: {}", path.display(), e))?;
            println!("\nCoverage report: {}", path.display());
        }
        _ => {
            let text = generate_text_report(&data, &source);
            let path = output_dir.join("coverage.txt");
            fs::write(&path, &text)
                .map_err(|e| format!("Cannot write {}: {}", path.display(), e))?;
            // Also print to stdout
            print!("{}", text);
            println!("\nCoverage report: {}", path.display());
        }
    }

    // Print summary to stderr
    eprintln!("\nCoverage: {:.1}% lines, {:.1}% functions, {:.1}% instructions",
        data.line_coverage_pct(), data.function_coverage_pct(), data.instruction_coverage_pct());

    Ok(())
}

pub fn print_help() {
    println!("veil — Code coverage tool for Fides");
    println!();
    println!("USAGE:");
    println!("    veil <file.fd> [--format text|html] [--output <dir>]");
    println!();
    println!("OPTIONS:");
    println!("    --format <format>   Report format: text (default) or html");
    println!("    --output <dir>      Output directory (default: coverage/)");
    println!("    --help, -h          Show this help");
    println!();
    println!("TRACKS:");
    println!("    - Function coverage: which functions were called");
    println!("    - Line coverage: which source lines were executed");
    println!("    - Instruction coverage: which IR instructions ran");
    println!();
    println!("EXAMPLES:");
    println!("    veil tests/struct.fd                      # Text report to stdout");
    println!("    veil tests/struct.fd --format html        # HTML report in coverage/");
    println!("    veil tests/struct.fd --output reports/    # Custom output directory");
}
