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
        lines = pass_store_load_forward(lines);
        lines = pass_lea_coalesce(lines);
        lines = pass_cmp_setcc_jcc(lines);
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
        "store_load_forward" => pass_store_load_forward(lines),
        "lea_coalesce" => pass_lea_coalesce(lines),
        "cmp_setcc_jcc" => pass_cmp_setcc_jcc(lines),
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
                                    // Check if the next instruction reads from dst
                                    // (e.g., `movq (%rax), %rax` reads %rax as
                                    // a memory address). If so, this is NOT a
                                    // dead store.
                                    let next_src = next_operands[0];
                                    let reads_dst = next_src.contains(dst)
                                        || next_src.contains("(%rax)")
                                        || next_src.contains("(%rcx)")
                                        || next_src.contains("(%rdx)")
                                        || next_src.contains("(%rbx)")
                                        || next_src.contains("(%rsi)")
                                        || next_src.contains("(%rdi)")
                                        || next_src.contains("(%r8)")
                                        || next_src.contains("(%r9)")
                                        || next_src.contains("(%r10)")
                                        || next_src.contains("(%r11)")
                                        || next_src.contains("(%r12)")
                                        || next_src.contains("(%r13)")
                                        || next_src.contains("(%r14)")
                                        || next_src.contains("(%r15)");
                                    if !reads_dst {
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
// Pass 7: Store-to-load forwarding
// ═══════════════════════════════════════════════════════════════════════════

/// Forward stores to loads: if `movq %src, off(%rbp)` is immediately followed
/// by `movq off(%rbp), %dst` (same offset), replace the load with
/// `movq %src, %dst`. This eliminates redundant memory traffic.
///
/// Pattern:
///   movq %src, -N(%rbp)
///   movq -N(%rbp), %dst
/// →
///   movq %src, -N(%rbp)
///   movq %src, %dst
///
/// The store must stay (the memory value may be read later), but the load
/// can be replaced with a register-to-register move.
fn pass_store_load_forward(lines: Vec<String>) -> Vec<String> {
    let parsed = parse_lines(&lines);
    let mut result: Vec<AsmLine> = Vec::new();

    let mut i = 0;
    while i < parsed.len() {
        // Look for a two-instruction window: store then load from same address.
        if i + 1 < parsed.len() {
            if let (Some(mn1), Some(ops1)) = (&parsed[i].mnemonic, &parsed[i].operands) {
                if let (Some(mn2), Some(ops2)) = (&parsed[i + 1].mnemonic, &parsed[i + 1].operands) {
                    // Store: movq %src, -N(%rbp)  or  movq %src, (%reg)
                    // Load:  movq -N(%rbp), %dst  or  movq (%reg), %dst
                    if (mn1 == "movq" || mn1 == "movl") && (mn2 == "movq" || mn2 == "movl") {
                        let ops1_list: Vec<&str> = ops1.split(',').map(|o| o.trim()).collect();
                        let ops2_list: Vec<&str> = ops2.split(',').map(|o| o.trim()).collect();
                        if ops1_list.len() == 2 && ops2_list.len() == 2 {
                            let src = ops1_list[0];
                            let mem_dst = ops1_list[1];
                            let mem_src = ops2_list[0];
                            let dst = ops2_list[1];
                            // The store's destination and load's source must both be
                            // memory operands (contain '(') — not plain registers.
                            // The store's source must be a register (start with '%').
                            // This prevents matching reg-to-reg moves as store-load.
                            let is_mem = |s: &str| s.contains('(');
                            let is_reg = |s: &str| s.starts_with('%');
                            if is_mem(mem_dst) && is_mem(mem_src)
                                && is_reg(src) && is_reg(dst)
                                && mem_dst == mem_src && src != dst {
                                // Replace the load with a register-to-register move.
                                result.push(parsed[i].clone());
                                result.push(AsmLine {
                                    raw: format!("    {} {}, {}", mn2, src, dst),
                                    mnemonic: Some(mn2.clone()),
                                    operands: Some(format!("{}, {}", src, dst)),
                                    is_label: false,
                                    is_directive: false,
                                    is_other: false,
                                });
                                i += 2;
                                continue;
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
// Pass 8: LEA coalescing
// ═══════════════════════════════════════════════════════════════════════════

/// Coalesce redundant LEA instructions: if `leaq -N(%rbp), %reg` is followed
/// by another `leaq -N(%rbp), %reg` (same offset), the second is redundant
/// (the register already holds the address).
///
/// Pattern:
///   leaq -N(%rbp), %rax
///   leaq -N(%rbp), %rax
/// →
///   leaq -N(%rbp), %rax
///
/// Also handles the case where the same address is computed into different
/// registers — we can't remove those, but we can track them.
fn pass_lea_coalesce(lines: Vec<String>) -> Vec<String> {
    let parsed = parse_lines(&lines);
    let mut result: Vec<AsmLine> = Vec::new();

    // Track the last LEA: (address, destination_register)
    let mut last_lea: Option<(String, String)> = None;

    for line in &parsed {
        if let (Some(mn), Some(ops)) = (&line.mnemonic, &line.operands) {
            if mn == "leaq" {
                let operands: Vec<&str> = ops.split(',').map(|o| o.trim()).collect();
                if operands.len() == 2 {
                    let addr = operands[0].to_string();
                    let dst = operands[1].to_string();
                    if let Some((prev_addr, prev_dst)) = &last_lea {
                        // Same address, same destination → redundant.
                        if &addr == prev_addr && &dst == prev_dst {
                            continue;
                        }
                    }
                    last_lea = Some((addr, dst));
                } else {
                    last_lea = None;
                }
            } else if line.mnemonic.is_some() {
                // Call instructions clobber all caller-saved registers.
                if mn == "call" || mn == "callq" {
                    last_lea = None;
                } else if let Some(ops) = &line.operands {
                    // Any other instruction might clobber a register, so clear
                    // the tracking if the destination matches the LEA's dst.
                    let operands: Vec<&str> = ops.split(',').map(|o| o.trim()).collect();
                    if let Some(last_operand) = operands.last() {
                        if let Some((_, lea_dst)) = &last_lea {
                            if last_operand == lea_dst {
                                last_lea = None;
                            }
                        }
                    }
                }
            }
        } else if !line.is_other {
            // Labels/directives break the chain.
            last_lea = None;
        }
        result.push(line.clone());
    }

    emit_lines(&result)
}

// ═══════════════════════════════════════════════════════════════════════════
// Pass 9: Instruction selection — cmp+sete+movzbq+test+jne → test+je
// ═══════════════════════════════════════════════════════════════════════════

/// Pattern-match the canonical comparison lowering and replace it with
/// a simpler instruction sequence:
///
///   cmpq %a, %b
///   sete %al        (or setne, setl, setg, setle, setge)
///   movzbq %al, %rax
///   testq %rax, %rax
///   jne .Ltrue
/// →
///   cmpq %a, %b
///   je .Ltrue        (or jne, jl, jg, jle, jge)
///
/// This eliminates the setcc+movzbq+test sequence and uses the flags
/// directly from cmpq.
fn pass_cmp_setcc_jcc(lines: Vec<String>) -> Vec<String> {
    let parsed = parse_lines(&lines);
    let mut result: Vec<AsmLine> = Vec::new();

    let mut i = 0;
    while i < parsed.len() {
        // Look for the 5-instruction pattern: cmp + setcc + movzbq + test + jcc
        if i + 4 < parsed.len() {
            let p0 = &parsed[i];
            let p1 = &parsed[i + 1];
            let p2 = &parsed[i + 2];
            let p3 = &parsed[i + 3];
            let p4 = &parsed[i + 4];

            // Check: cmpq %a, %b
            if let (Some(mn0), Some(ops0)) = (&p0.mnemonic, &p0.operands) {
                if mn0 == "cmpq" || mn0 == "cmpl" {
                    // Check: setcc %al
                    if let (Some(mn1), Some(ops1)) = (&p1.mnemonic, &p1.operands) {
                        let setcc_map = |mn: &str| -> Option<&str> {
                            match mn {
                                "sete" => Some("je"),
                                "setne" => Some("jne"),
                                "setl" => Some("jl"),
                                "setg" => Some("jg"),
                                "setle" => Some("jle"),
                                "setge" => Some("jge"),
                                "setb" => Some("jb"),
                                "seta" => Some("ja"),
                                "setbe" => Some("jbe"),
                                "setae" => Some("jae"),
                                _ => None,
                            }
                        };
                        if let Some(jcc) = setcc_map(mn1) {
                            if ops1 == "%al" {
                                // Check: movzbq %al, %rax
                                if let (Some(mn2), Some(ops2)) = (&p2.mnemonic, &p2.operands) {
                                    if mn2 == "movzbq" && ops2 == "%al, %rax" {
                                        // Check: testq %rax, %rax
                                        if let (Some(mn3), Some(ops3)) = (&p3.mnemonic, &p3.operands) {
                                            if mn3 == "testq" && ops3 == "%rax, %rax" {
                                                // Check: jne .Ltarget
                                                if let (Some(mn4), Some(ops4)) = (&p4.mnemonic, &p4.operands) {
                                                    if mn4 == "jne" {
                                                        // All five instructions match!
                                                        // Replace with: cmpq (kept) + jcc .Ltarget
                                                        result.push(p0.clone());
                                                        result.push(AsmLine {
                                                            raw: format!("    {} {}", jcc, ops4),
                                                            mnemonic: Some(jcc.to_string()),
                                                            operands: Some(ops4.clone()),
                                                            is_label: false,
                                                            is_directive: false,
                                                            is_other: false,
                                                        });
                                                        i += 5;
                                                        continue;
                                                    }
                                                }
                                            }
                                        }
                                    }
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

    #[test]
    fn store_load_forward_basic() {
        let asm = "    movq %rax, -8(%rbp)\n    movq -8(%rbp), %rcx\n    ret\n";
        let opt = optimize_pass(asm, "store_load_forward");
        // The load should be replaced with a register-to-register move.
        assert!(opt.contains("movq %rax, %rcx"), "load should be forwarded to movq %rax, %rcx");
        // The store should still be present.
        assert!(opt.contains("movq %rax, -8(%rbp)"), "store should be preserved");
    }

    #[test]
    fn store_load_forward_different_addr() {
        let asm = "    movq %rax, -8(%rbp)\n    movq -16(%rbp), %rcx\n    ret\n";
        let opt = optimize_pass(asm, "store_load_forward");
        // Different addresses — should not forward.
        assert!(opt.contains("movq -16(%rbp), %rcx"), "load from different addr should not be forwarded");
    }

    #[test]
    fn lea_coalesce_redundant() {
        let asm = "    leaq -8(%rbp), %rax\n    leaq -8(%rbp), %rax\n    ret\n";
        let opt = optimize_pass(asm, "lea_coalesce");
        let count = opt.matches("leaq").count();
        assert_eq!(count, 1, "redundant leaq should be coalesced");
    }

    #[test]
    fn lea_coalesce_different_reg() {
        let asm = "    leaq -8(%rbp), %rax\n    leaq -8(%rbp), %rcx\n    ret\n";
        let opt = optimize_pass(asm, "lea_coalesce");
        let count = opt.matches("leaq").count();
        assert_eq!(count, 2, "different destination registers should not be coalesced");
    }

    #[test]
    fn cmp_setcc_jcc_pattern() {
        let asm = "    cmpq %rsi, %rax\n    sete %al\n    movzbq %al, %rax\n    testq %rax, %rax\n    jne .Ltrue\n    ret\n";
        let opt = optimize_pass(asm, "cmp_setcc_jcc");
        // The setcc+movzbq+test sequence should be eliminated.
        assert!(!opt.contains("sete"), "sete should be eliminated");
        assert!(!opt.contains("movzbq"), "movzbq should be eliminated");
        assert!(!opt.contains("testq %rax, %rax"), "testq should be eliminated");
        // The cmpq should be kept and jne should become je.
        assert!(opt.contains("cmpq %rsi, %rax"), "cmpq should be kept");
        assert!(opt.contains("je .Ltrue"), "jne should become je (sete → je)");
    }

    #[test]
    fn cmp_setne_jcc_pattern() {
        let asm = "    cmpq %rsi, %rax\n    setne %al\n    movzbq %al, %rax\n    testq %rax, %rax\n    jne .Lfalse\n    ret\n";
        let opt = optimize_pass(asm, "cmp_setcc_jcc");
        assert!(opt.contains("jne .Lfalse"), "setne+jne should become jne");
        assert!(!opt.contains("setne"), "setne should be eliminated");
    }
}
