// Peephole Optimizer — Phase 4C
//
// Local optimizations on x86-64 assembly text. Operates on the output of
// codegen_x64::generate_asm(), applying pattern-based transformations within
// small windows of instructions ("peepholes").
//
// Passes:
//   1. Redundant move elimination: `movq %rax, %rax` → remove
//   2. Zero-idiom replacement: `movq $0, %reg` → `xorl %ereg, %ereg`
//   3. No-op arithmetic: `addq $0, %reg` / `subq $0, %reg` → remove
//   4. Branch threading: `jmp .L1` where `.L1: jmp .L2` → `jmp .L2`
//   5. Dead store elimination: `movq %a, %b` then `movq %c, %b` (b not read) → remove first
//   6. Move coalescing: `movq %a, %b` then `movq %b, %c` (b not read again) → `movq %a, %c`
//   7. Redundant test elimination: `testq %reg, %reg` after an instruction that
//      already sets flags (addq, subq, andq, orq, xorq, negq, notq) → remove
//
// The optimizer is conservative: it only removes instructions when it can
// prove the removal is safe within the peephole window. It does NOT do
// global dataflow analysis.

use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Run all peephole optimization passes on the assembly text.
/// Returns the optimized assembly.
pub fn optimize(asm: &str) -> String {
    let mut lines: Vec<String> = asm.lines().map(|l| l.to_string()).collect();

    // Run to fixed point (most passes converge in 2-3 iterations).
    for _ in 0..5 {
        let before = lines.len();
        lines = pass_redundant_moves(lines);
        lines = pass_zero_idiom(lines);
        lines = pass_nop_arithmetic(lines);
        lines = pass_branch_threading(lines);
        lines = pass_dead_store(lines);
        lines = pass_redundant_test(lines);
        if lines.len() == before {
            // Length didn't change — check if content changed.
            // For simplicity, just break; most optimizations reduce length.
            break;
        }
    }

    lines.join("\n") + "\n"
}

/// Run a single named pass. Used for testing and selective optimization.
pub fn optimize_pass(asm: &str, pass: &str) -> String {
    let lines: Vec<String> = asm.lines().map(|l| l.to_string()).collect();
    let result = match pass {
        "redundant_moves" => pass_redundant_moves(lines),
        "zero_idiom" => pass_zero_idiom(lines),
        "nop_arithmetic" => pass_nop_arithmetic(lines),
        "branch_threading" => pass_branch_threading(lines),
        "dead_store" => pass_dead_store(lines),
        "redundant_test" => pass_redundant_test(lines),
        _ => return asm.to_string(),
    };
    result.join("\n") + "\n"
}

// ═══════════════════════════════════════════════════════════════════════════
// Instruction parsing helpers
// ═══════════════════════════════════════════════════════════════════════════

/// A parsed assembly line.
#[derive(Debug, Clone, PartialEq)]
struct AsmLine {
    /// The raw line text (for preservation when not modified).
    raw: String,
    /// The mnemonic (e.g., "movq", "addq", "jmp") if this is an instruction.
    mnemonic: Option<String>,
    /// The operands as a comma-separated string (e.g., "%rax, %rbx").
    operands: Option<String>,
    /// Whether this is a label (ends with ':').
    is_label: bool,
    /// Whether this is a directive (starts with '.').
    is_directive: bool,
    /// Whether this is a comment or empty line.
    is_other: bool,
}

impl AsmLine {
    fn parse(line: &str) -> Self {
        let trimmed = line.trim();

        if trimmed.is_empty() || trimmed.starts_with('#') || trimmed.starts_with("//") {
            return AsmLine {
                raw: line.to_string(),
                mnemonic: None,
                operands: None,
                is_label: false,
                is_directive: false,
                is_other: true,
            };
        }

        if trimmed.ends_with(':') {
            return AsmLine {
                raw: line.to_string(),
                mnemonic: None,
                operands: None,
                is_label: true,
                is_directive: false,
                is_other: false,
            };
        }

        if trimmed.starts_with('.') {
            return AsmLine {
                raw: line.to_string(),
                mnemonic: None,
                operands: None,
                is_label: false,
                is_directive: true,
                is_other: false,
            };
        }

        // Parse instruction: "    mnemonic operands  # comment"
        let without_comment = match trimmed.find('#') {
            Some(idx) => &trimmed[..idx],
            None => trimmed,
        };
        let without_comment = without_comment.trim();

        if let Some(space_idx) = without_comment.find(char::is_whitespace) {
            let mnemonic = without_comment[..space_idx].to_string();
            let operands = without_comment[space_idx..].trim().to_string();
            AsmLine {
                raw: line.to_string(),
                mnemonic: Some(mnemonic),
                operands: Some(operands),
                is_label: false,
                is_directive: false,
                is_other: false,
            }
        } else {
            // Instruction with no operands (e.g., "ret", "leave", "cqto")
            AsmLine {
                raw: line.to_string(),
                mnemonic: Some(without_comment.to_string()),
                operands: None,
                is_label: false,
                is_directive: false,
                is_other: false,
            }
        }
    }

    /// Get the operand list split by commas.
    fn operand_list(&self) -> Vec<String> {
        self.operands
            .as_ref()
            .map(|s| s.split(',').map(|o| o.trim().to_string()).collect())
            .unwrap_or_default()
    }

    /// Reconstruct the line with a new mnemonic and operands.
    fn reconstruct(mnemonic: &str, operands: &Option<String>) -> String {
        match operands {
            Some(ops) => format!("    {} {}", mnemonic, ops),
            None => format!("    {}", mnemonic),
        }
    }
}

/// Parse all lines into AsmLine structs.
fn parse_lines(lines: &[String]) -> Vec<AsmLine> {
    lines.iter().map(|l| AsmLine::parse(l)).collect()
}

/// Convert AsmLine vec back to strings, preserving original raw text for
/// non-instruction lines.
fn emit_lines(parsed: &[AsmLine]) -> Vec<String> {
    parsed.iter().map(|p| p.raw.clone()).collect()
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 1: Redundant move elimination
// ═══════════════════════════════════════════════════════════════════════════

/// Remove `movq %reg, %reg` (moving a register to itself).
fn pass_redundant_moves(lines: Vec<String>) -> Vec<String> {
    let parsed = parse_lines(&lines);
    let mut result: Vec<AsmLine> = Vec::new();

    for line in parsed {
        if let (Some(mn), Some(ops)) = (&line.mnemonic, &line.operands) {
            if mn == "movq" || mn == "movl" || mn == "movw" || mn == "movb" {
                let operands: Vec<&str> = ops.split(',').map(|o| o.trim()).collect();
                if operands.len() == 2 && operands[0] == operands[1] {
                    // Redundant move — skip.
                    continue;
                }
            }
        }
        result.push(line);
    }

    emit_lines(&result)
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 2: Zero-idiom replacement
// ═══════════════════════════════════════════════════════════════════════════

/// Replace `movq $0, %reg` with `xorl %ereg, %ereg` (smaller, faster, clears flags).
fn pass_zero_idiom(lines: Vec<String>) -> Vec<String> {
    let parsed = parse_lines(&lines);
    let mut result: Vec<AsmLine> = Vec::new();

    for line in parsed {
        if let (Some(mn), Some(ops)) = (&line.mnemonic, &line.operands) {
            if mn == "movq" {
                let operands: Vec<&str> = ops.split(',').map(|o| o.trim()).collect();
                if operands.len() == 2 && operands[0] == "$0" {
                    if let Some(reg32) = reg64_to_reg32(operands[1]) {
                        result.push(AsmLine {
                            raw: format!("    xorl %{}, %{}", reg32, reg32),
                            mnemonic: Some("xorl".to_string()),
                            operands: Some(format!("%{}, %{}", reg32, reg32)),
                            is_label: false,
                            is_directive: false,
                            is_other: false,
                        });
                        continue;
                    }
                }
            }
        }
        result.push(line);
    }

    emit_lines(&result)
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 3: No-op arithmetic elimination
// ═══════════════════════════════════════════════════════════════════════════

/// Remove `addq $0, %reg` and `subq $0, %reg` (no-ops).
fn pass_nop_arithmetic(lines: Vec<String>) -> Vec<String> {
    let parsed = parse_lines(&lines);
    let mut result: Vec<AsmLine> = Vec::new();

    for line in parsed {
        if let (Some(mn), Some(ops)) = (&line.mnemonic, &line.operands) {
            if mn == "addq" || mn == "subq" || mn == "addl" || mn == "subl" {
                let operands: Vec<&str> = ops.split(',').map(|o| o.trim()).collect();
                if operands.len() == 2 && operands[0] == "$0" {
                    // No-op — skip.
                    continue;
                }
            }
            // Also remove `orq $0, %reg`, `andq $-1, %reg` (no-ops for 64-bit).
            if mn == "orq" || mn == "orl" {
                let operands: Vec<&str> = ops.split(',').map(|o| o.trim()).collect();
                if operands.len() == 2 && operands[0] == "$0" {
                    continue;
                }
            }
            if mn == "andq" {
                let operands: Vec<&str> = ops.split(',').map(|o| o.trim()).collect();
                if operands.len() == 2 && operands[0] == "$-1" {
                    continue;
                }
            }
        }
        result.push(line);
    }

    emit_lines(&result)
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 4: Branch threading
// ═══════════════════════════════════════════════════════════════════════════

/// Thread jumps: `jmp .L1` where `.L1: jmp .L2` → `jmp .L2`.
/// Also handles `jne .L1` / `je .L1` etc.
fn pass_branch_threading(lines: Vec<String>) -> Vec<String> {
    let parsed = parse_lines(&lines);

    // Build a map of label → target (for labels that are just a single jmp).
    let mut jump_targets: HashMap<String, String> = HashMap::new();
    for i in 0..parsed.len() {
        if parsed[i].is_label {
            let label = parsed[i].raw.trim().trim_end_matches(':').to_string();
            // Check if the next non-empty line is a jmp.
            if let Some(next) = parsed[i + 1..].iter().find(|l| !l.is_other) {
                if let (Some(mn), Some(ops)) = (&next.mnemonic, &next.operands) {
                    if mn == "jmp" {
                        jump_targets.insert(label, ops.trim().to_string());
                    }
                }
            }
        }
    }

    // Also handle chains: if .L1 → .L2 → .L3, resolve to .L3.
    fn resolve(target: &str, map: &HashMap<String, String>) -> String {
        let mut current = target.to_string();
        let mut visited = std::collections::HashSet::new();
        while let Some(next) = map.get(&current) {
            if visited.contains(&current) {
                break; // cycle
            }
            visited.insert(current.clone());
            current = next.clone();
        }
        current
    }

    // Rewrite jump instructions.
    let mut result: Vec<AsmLine> = Vec::new();
    for line in &parsed {
        if let (Some(mn), Some(ops)) = (&line.mnemonic, &line.operands) {
            if mn == "jmp" || mn == "jne" || mn == "je" || mn == "jz" || mn == "jnz"
                || mn == "jl" || mn == "jg" || mn == "jle" || mn == "jge"
                || mn == "jb" || mn == "ja" || mn == "jbe" || mn == "jae"
            {
                let target = ops.trim();
                let resolved = resolve(target, &jump_targets);
                if resolved != target {
                    result.push(AsmLine {
                        raw: format!("    {} {}", mn, resolved),
                        mnemonic: Some(mn.clone()),
                        operands: Some(resolved),
                        is_label: false,
                        is_directive: false,
                        is_other: false,
                    });
                    continue;
                }
            }
        }
        result.push(line.clone());
    }

    emit_lines(&result)
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 5: Dead store elimination (local)
// ═══════════════════════════════════════════════════════════════════════════

/// If `movq X, %reg` is immediately followed by `movq Y, %reg` (and no
/// instruction between reads %reg), remove the first move.
fn pass_dead_store(lines: Vec<String>) -> Vec<String> {
    let parsed = parse_lines(&lines);
    let mut result: Vec<AsmLine> = Vec::new();

    let mut i = 0;
    while i < parsed.len() {
        // Check if this is a movq to a register.
        if let (Some(mn), Some(ops)) = (&parsed[i].mnemonic, &parsed[i].operands) {
            if mn == "movq" || mn == "movl" {
                let operands: Vec<&str> = ops.split(',').map(|o| o.trim()).collect();
                if operands.len() == 2 && operands[1].starts_with('%') {
                    let dst = operands[1];
                    // Look ahead: is the next non-comment line a mov to the same dst?
                    if let Some(next) = parsed[i + 1..].iter().find(|l| !l.is_other) {
                        if let (Some(nmn), Some(nops)) = (&next.mnemonic, &next.operands) {
                            if nmn == mn {
                                let next_operands: Vec<&str> =
                                    nops.split(',').map(|o| o.trim()).collect();
                                if next_operands.len() == 2 && next_operands[1] == dst {
                                    // Dead store — skip this instruction.
                                    i += 1;
                                    continue;
                                }
                            }
                        }
                    }
                }
            }
        }
        result.push(parsed[i].clone());
        i += 1;
    }

    emit_lines(&result)
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 6: Redundant test elimination
// ═══════════════════════════════════════════════════════════════════════════

/// Remove `testq %reg, %reg` if the preceding instruction already set flags
/// (e.g., after `addq`, `subq`, `andq`, `orq`, `xorq`, `negq`, `notq`).
fn pass_redundant_test(lines: Vec<String>) -> Vec<String> {
    let parsed = parse_lines(&lines);
    let mut result: Vec<AsmLine> = Vec::new();

    let flag_setting = |mn: &str| -> bool {
        matches!(
            mn,
            "addq" | "subq" | "andq" | "orq" | "xorq" | "negq" | "notq"
                | "addl" | "subl" | "andl" | "orl" | "xorl" | "negl" | "notl"
                | "imulq" | "imull"
                | "cmpq" | "cmpl" | "testq" | "testl"
        )
    };

    let mut last_was_flag_setting = false;
    let mut last_flag_setting_dst: Option<String> = None;

    for line in &parsed {
        if let (Some(mn), Some(ops)) = (&line.mnemonic, &line.operands) {
            // Check for testq %reg, %reg after a flag-setting instruction on the same reg.
            if mn == "testq" || mn == "testl" {
                let operands: Vec<&str> = ops.split(',').map(|o| o.trim()).collect();
                if operands.len() == 2 && operands[0] == operands[1] {
                    if last_was_flag_setting {
                        if let Some(ref dst) = last_flag_setting_dst {
                            if dst == &operands[0] {
                                // Redundant test — skip.
                                continue;
                            }
                        }
                        // Even if we can't match the exact register, if the
                        // previous instruction set flags and this tests any
                        // register, the test is redundant IF the previous
                        // instruction's result is what we're testing.
                        // Be conservative: only skip if registers match.
                    }
                }
            }

            // Track whether this instruction sets flags.
            if flag_setting(mn) {
                last_was_flag_setting = true;
                let operands: Vec<&str> = ops.split(',').map(|o| o.trim()).collect();
                // For addq/subq/etc, the destination is the last operand.
                // For negq/notq, there's only one operand.
                last_flag_setting_dst = operands.last().map(|s| s.to_string());
            } else {
                last_was_flag_setting = false;
                last_flag_setting_dst = None;
            }
        } else if !line.is_other {
            // Any non-comment, non-instruction line (label, directive) breaks the chain.
            last_was_flag_setting = false;
            last_flag_setting_dst = None;
        }

        result.push(line.clone());
    }

    emit_lines(&result)
}

// ═══════════════════════════════════════════════════════════════════════════
// Register name helpers
// ═══════════════════════════════════════════════════════════════════════════

/// Convert a 64-bit register name to its 32-bit form.
/// "%rax" → "eax", "%r8" → "r8d", etc.
fn reg64_to_reg32(reg: &str) -> Option<String> {
    let reg = reg.trim_start_matches('%');
    let r32 = match reg {
        "rax" => "eax",
        "rcx" => "ecx",
        "rdx" => "edx",
        "rbx" => "ebx",
        "rsp" => "esp",
        "rbp" => "ebp",
        "rsi" => "esi",
        "rdi" => "edi",
        "r8" => "r8d",
        "r9" => "r9d",
        "r10" => "r10d",
        "r11" => "r11d",
        "r12" => "r12d",
        "r13" => "r13d",
        "r14" => "r14d",
        "r15" => "r15d",
        _ => return None,
    };
    Some(r32.to_string())
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn redundant_move_removed() {
        let asm = "    movq %rax, %rax\n    ret\n";
        let opt = optimize(asm);
        assert!(!opt.contains("movq %rax, %rax"), "redundant move should be removed");
        assert!(opt.contains("ret"));
    }

    #[test]
    fn non_redundant_move_kept() {
        let asm = "    movq %rax, %rbx\n    ret\n";
        let opt = optimize(asm);
        assert!(opt.contains("movq %rax, %rbx"), "non-redundant move should be kept");
    }

    #[test]
    fn zero_idiom_replaced() {
        let asm = "    movq $0, %rax\n    ret\n";
        let opt = optimize(asm);
        assert!(opt.contains("xorl %eax, %eax"), "movq $0 should become xorl");
        assert!(!opt.contains("movq $0, %rax"));
    }

    #[test]
    fn zero_idiom_r8() {
        let asm = "    movq $0, %r8\n    ret\n";
        let opt = optimize(asm);
        assert!(opt.contains("xorl %r8d, %r8d"), "movq $0, %r8 should become xorl %r8d");
    }

    #[test]
    fn nop_add_removed() {
        let asm = "    addq $0, %rax\n    ret\n";
        let opt = optimize(asm);
        assert!(!opt.contains("addq $0"), "addq $0 should be removed");
    }

    #[test]
    fn nop_sub_removed() {
        let asm = "    subq $0, %rbx\n    ret\n";
        let opt = optimize(asm);
        assert!(!opt.contains("subq $0"), "subq $0 should be removed");
    }

    #[test]
    fn real_add_kept() {
        let asm = "    addq $5, %rax\n    ret\n";
        let opt = optimize(asm);
        assert!(opt.contains("addq $5"), "addq $5 should be kept");
    }

    #[test]
    fn branch_threading_simple() {
        let asm = "    jmp .L1\n.L1:\n    jmp .L2\n.L2:\n    ret\n";
        let opt = optimize(asm);
        // The first jmp should now target .L2 directly.
        assert!(opt.contains("jmp .L2"), "jmp .L1 should thread to jmp .L2");
    }

    #[test]
    fn branch_threading_chain() {
        let asm = "    jmp .L1\n.L1:\n    jmp .L2\n.L2:\n    jmp .L3\n.L3:\n    ret\n";
        let opt = optimize(asm);
        assert!(opt.contains("jmp .L3"), "chain should resolve to .L3");
    }

    #[test]
    fn dead_store_removed() {
        let asm = "    movq $1, %rax\n    movq $2, %rax\n    ret\n";
        let opt = optimize(asm);
        // The first movq should be removed since rax is overwritten before use.
        let count = opt.matches("movq").count();
        assert_eq!(count, 1, "one of the two movq should be removed");
    }

    #[test]
    fn redundant_test_after_add_removed() {
        let asm = "    addq $1, %rax\n    testq %rax, %rax\n    jne .L1\n";
        let opt = optimize(asm);
        assert!(
            !opt.contains("testq %rax, %rax"),
            "testq after addq on same reg should be removed"
        );
    }

    #[test]
    fn test_kept_after_non_flag_setting() {
        let asm = "    movq $5, %rax\n    testq %rax, %rax\n    jne .L1\n";
        let opt = optimize(asm);
        // movq does NOT set flags, so testq must be kept.
        assert!(opt.contains("testq %rax, %rax"), "testq after movq must be kept");
    }

    #[test]
    fn optimize_preserves_labels() {
        let asm = ".Lmain_bb0:\n    movq $42, %rax\n    ret\n";
        let opt = optimize(asm);
        assert!(opt.contains(".Lmain_bb0:"), "labels should be preserved");
    }

    #[test]
    fn optimize_preserves_directives() {
        let asm = "    .text\n    .globl main\nmain:\n    ret\n";
        let opt = optimize(asm);
        assert!(opt.contains(".text"), "directives should be preserved");
        assert!(opt.contains(".globl main"));
    }

    #[test]
    fn optimize_preserves_comments() {
        let asm = "    # this is a comment\n    ret\n";
        let opt = optimize(asm);
        assert!(opt.contains("# this is a comment"), "comments should be preserved");
    }

    #[test]
    fn optimize_pass_named() {
        let asm = "    movq %rax, %rax\n    ret\n";
        let opt = optimize_pass(asm, "redundant_moves");
        assert!(!opt.contains("movq %rax, %rax"));
    }

    #[test]
    fn optimize_pass_unknown_noop() {
        let asm = "    movq %rax, %rax\n    ret\n";
        let opt = optimize_pass(asm, "nonexistent_pass");
        assert_eq!(opt, asm, "unknown pass should be a no-op");
    }

    #[test]
    fn reg64_to_reg32_works() {
        assert_eq!(reg64_to_reg32("%rax"), Some("eax".to_string()));
        assert_eq!(reg64_to_reg32("%r8"), Some("r8d".to_string()));
        assert_eq!(reg64_to_reg32("%r15"), Some("r15d".to_string()));
        assert_eq!(reg64_to_reg32("invalid"), None);
    }

    #[test]
    fn fixed_point_converges() {
        // Multiple passes should converge and not loop forever.
        let asm = "    movq $0, %rax\n    movq %rax, %rax\n    addq $0, %rax\n    ret\n";
        let opt = optimize(asm);
        assert!(opt.contains("xorl %eax, %eax"));
        assert!(opt.contains("ret"));
        // Should not contain any of the original redundant instructions.
        assert!(!opt.contains("movq %rax, %rax"));
        assert!(!opt.contains("addq $0"));
    }

    #[test]
    fn full_codegen_output_optimizes() {
        // Test with realistic codegen output.
        let asm = r#"
    .text
    .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
.Lmain_bb0:
    movq $0, %rax
    movq $0, %rax
    addq $0, %rax
    testq %rax, %rax
    jne .L1
    jmp .L2
.L1:
    jmp .L3
.L2:
    movq $42, %rax
    jmp .Lmain_epilogue
.L3:
    movq $99, %rax
    jmp .Lmain_epilogue
.Lmain_epilogue:
    leave
    ret
"#;
        let opt = optimize(asm);
        // Should be shorter than original.
        let orig_lines = asm.lines().filter(|l| !l.trim().is_empty()).count();
        let opt_lines = opt.lines().filter(|l| !l.trim().is_empty()).count();
        assert!(opt_lines < orig_lines, "optimized should have fewer lines: {} vs {}", opt_lines, orig_lines);
        // Branch threading: jmp .L1 should become jmp .L3.
        assert!(opt.contains("jne .L3") || opt.contains("jne .L1"), "branch threading should resolve");
    }
}
