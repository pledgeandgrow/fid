// Register Allocator — Phase 4C
//
// Linear scan register allocator for x86-64. Consumes PIR L3 (which uses
// unbounded virtual registers) and maps each VReg to either a physical
// register or a stack slot.
//
// Algorithm:
//   1. Compute live intervals for each VReg (start = first def, end = last use).
//   2. Sort intervals by start point.
//   3. Walk intervals in order; assign a free physical register, or spill
//      the interval with the farthest end point to a stack slot.
//
// x86-64 register file (System V AMD64 calling convention):
//   General purpose (caller-saved): RAX, RCX, RDX, RSI, RDI, R8, R9, R10, R11
//   General purpose (callee-saved): RBX, R12, R13, R14, R15, RBP
//   We allocate from caller-saved first (no save/restore overhead), then
//   callee-saved (requires prologue/epilogue saves).
//
// For the bootstrap we use a simplified calling convention:
//   - Args in RDI, RSI, RDX, RCX, R8, R9 (first 6 integer/pointer args)
//   - Return in RAX
//   - 8 caller-saved + 6 callee-saved = 14 allocatable GP registers
//
// Entry points:
//   allocate(&L3Function) -> Allocation  — run the allocator on one function
//   allocate_module(&L3Module) -> ModuleAllocation  — run on all functions

use crate::pir::{L3Function, L3Inst, L3Module, Operand, PBlock, VReg};
use crate::ast::BinOp;
use std::collections::{HashMap, HashSet};

// ═══════════════════════════════════════════════════════════════════════════
// Physical registers
// ═══════════════════════════════════════════════════════════════════════════

/// A physical x86-64 general-purpose register.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, PartialOrd, Ord)]
#[repr(u8)]
pub enum PhysReg {
    Rax = 0,
    Rcx = 1,
    Rdx = 2,
    Rbx = 3,
    Rsp = 4,
    Rbp = 5,
    Rsi = 6,
    Rdi = 7,
    R8 = 8,
    R9 = 9,
    R10 = 10,
    R11 = 11,
    R12 = 12,
    R13 = 13,
    R14 = 14,
    R15 = 15,
    // SSE/AVX registers for SIMD
    Xmm0 = 16,
    Xmm1 = 17,
    Xmm2 = 18,
    Xmm3 = 19,
    Xmm4 = 20,
    Xmm5 = 21,
    Xmm6 = 22,
    Xmm7 = 23,
}

impl PhysReg {
    /// The 4-bit encoding used in ModRM reg/rm fields.
    pub fn encoding(self) -> u8 {
        self as u8
    }

    /// Registers available for allocation (not RSP, not RBP).
    /// Caller-saved first, then callee-saved.
    pub fn allocatable() -> &'static [PhysReg] {
        &[
            // Caller-saved (no prologue save needed)
            PhysReg::Rax,
            PhysReg::Rcx,
            PhysReg::Rdx,
            PhysReg::Rsi,
            PhysReg::Rdi,
            PhysReg::R8,
            PhysReg::R9,
            PhysReg::R10,
            PhysReg::R11,
            // Callee-saved (need prologue push / epilogue pop)
            PhysReg::Rbx,
            PhysReg::R12,
            PhysReg::R13,
            PhysReg::R14,
            PhysReg::R15,
        ]
    }

    /// Is this register callee-saved?
    pub fn is_callee_saved(self) -> bool {
        matches!(
            self,
            PhysReg::Rbx | PhysReg::R12 | PhysReg::R13 | PhysReg::R14 | PhysReg::R15
        )
    }

    /// Argument registers in System V AMD64 order (first 6 integer args).
    pub fn arg_regs() -> &'static [PhysReg] {
        &[PhysReg::Rdi, PhysReg::Rsi, PhysReg::Rdx, PhysReg::Rcx, PhysReg::R8, PhysReg::R9]
    }

    /// Return value register.
    pub fn return_reg() -> PhysReg {
        PhysReg::Rax
    }

    /// Name for assembly output.
    pub fn name(self) -> &'static str {
        match self {
            PhysReg::Rax => "rax",
            PhysReg::Rcx => "rcx",
            PhysReg::Rdx => "rdx",
            PhysReg::Rbx => "rbx",
            PhysReg::Rsp => "rsp",
            PhysReg::Rbp => "rbp",
            PhysReg::Rsi => "rsi",
            PhysReg::Rdi => "rdi",
            PhysReg::R8 => "r8",
            PhysReg::R9 => "r9",
            PhysReg::R10 => "r10",
            PhysReg::R11 => "r11",
            PhysReg::R12 => "r12",
            PhysReg::R13 => "r13",
            PhysReg::R14 => "r14",
            PhysReg::R15 => "r15",
            PhysReg::Xmm0 => "xmm0",
            PhysReg::Xmm1 => "xmm1",
            PhysReg::Xmm2 => "xmm2",
            PhysReg::Xmm3 => "xmm3",
            PhysReg::Xmm4 => "xmm4",
            PhysReg::Xmm5 => "xmm5",
            PhysReg::Xmm6 => "xmm6",
            PhysReg::Xmm7 => "xmm7",
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Live intervals
// ═══════════════════════════════════════════════════════════════════════════

/// A live interval [start, end] for a virtual register.
/// `start` is the instruction index where the VReg is defined;
/// `end` is the instruction index of its last use.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct LiveInterval {
    pub vreg: VReg,
    pub start: u32,
    pub end: u32,
}

impl LiveInterval {
    /// Does this interval overlap with another?
    pub fn overlaps(&self, other: &LiveInterval) -> bool {
        !(self.end < other.start || other.end < self.start)
    }

    /// Is this interval live at instruction index `pos`?
    pub fn contains(&self, pos: u32) -> bool {
        pos >= self.start && pos <= self.end
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Allocation result
// ═══════════════════════════════════════════════════════════════════════════

/// Where a VReg lives after allocation.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Location {
    /// Allocated to a physical register.
    Reg(PhysReg),
    /// Spilled to a stack slot at byte offset from RBP.
    Spill(u32),
    /// Rematerializable constant — reload the immediate at each use site
    /// instead of keeping it in a register or stack slot. (F-1A)
    Remat(i64),
}

/// The result of allocating one function.
#[derive(Debug, Clone)]
pub struct Allocation {
    /// VReg → location mapping.
    pub vreg_to_loc: HashMap<VReg, Location>,
    /// Callee-saved registers used (need prologue push / epilogue pop).
    pub used_callee_saved: Vec<PhysReg>,
    /// Total spill area size in bytes (added to frame size).
    pub spill_size: u32,
    /// Argument VRegs that arrive in physical registers.
    pub arg_locations: Vec<Location>,
}

impl Allocation {
    /// Look up where a VReg lives.
    pub fn loc(&self, vreg: VReg) -> Location {
        self.vreg_to_loc
            .get(&vreg)
            .copied()
            .unwrap_or(Location::Spill(0))
    }

    /// Look up the physical register for a VReg, if any.
    pub fn reg(&self, vreg: VReg) -> Option<PhysReg> {
        match self.loc(vreg) {
            Location::Reg(r) => Some(r),
            _ => None,
        }
    }
}

/// Allocation results for an entire module.
#[derive(Debug, Clone, Default)]
pub struct ModuleAllocation {
    pub func_allocations: HashMap<String, Allocation>,
}

// ═══════════════════════════════════════════════════════════════════════════
// Linear scan allocator
// ═══════════════════════════════════════════════════════════════════════════

/// Run the linear scan allocator on a single function.
/// `pure_funcs` is the set of pure function names — calls to these don't
/// clobber caller-saved registers (F-1D: effect-aware regalloc).
pub fn allocate(func: &L3Function, pure_funcs: &HashSet<String>, no_alloc_funcs: &HashSet<String>) -> Allocation {
    // 1. Compute live intervals.
    let intervals = compute_live_intervals(func);

    // 1b. Build a map of constant VRegs — VRegs whose sole definition is
    // `Mov { src: Operand::ImmI(c) }`. These are rematerializable: instead
    // of spilling them to the stack under register pressure, we reload the
    // immediate at each use site (1 instruction, no stack slot). (F-1A)
    let constant_vregs = compute_constant_vregs(func);

    // 2. Map argument VRegs to their incoming physical registers.
    let arg_regs = PhysReg::arg_regs();
    let mut arg_locations: Vec<Location> = Vec::new();
    for (i, (vreg, _)) in func.params.iter().enumerate() {
        if i < arg_regs.len() {
            arg_locations.push(Location::Reg(arg_regs[i]));
        } else {
            // Extra args are on the stack (above RBP). Offset: 16 + (i-6)*8.
            let offset = 16 + (i - arg_regs.len()) as u32 * 8;
            arg_locations.push(Location::Spill(offset));
        }
    }

    // 3. Pre-assign argument VRegs to their physical registers or stack slots.
    let mut vreg_to_loc: HashMap<VReg, Location> = HashMap::new();
    let mut preassigned_reg: HashMap<VReg, PhysReg> = HashMap::new();
    let mut preassigned_all: HashSet<VReg> = HashSet::new();
    for (i, (vreg, _)) in func.params.iter().enumerate() {
        if i < arg_regs.len() {
            vreg_to_loc.insert(*vreg, Location::Reg(arg_regs[i]));
            preassigned_reg.insert(*vreg, arg_regs[i]);
        } else {
            let offset = 16 + (i - arg_regs.len()) as u32 * 8;
            vreg_to_loc.insert(*vreg, Location::Spill(offset));
        }
        preassigned_all.insert(*vreg);
    }

    // 4. Sort intervals by start point, then by vreg for determinism.
    // (HashMap iteration order is non-deterministic due to random seeding,
    // so we need a secondary sort key to ensure deterministic allocation.)
    let mut sorted: Vec<LiveInterval> = intervals.values().cloned().collect();
    sorted.sort_by_key(|i| (i.start, i.vreg.0));

    // 5. Linear scan.
    // `active` holds intervals currently assigned to a physical register,
    // sorted by end point. `free_pool` holds available physical registers.
    let mut active: Vec<LiveInterval> = Vec::new();
    let mut free_pool: Vec<PhysReg> = PhysReg::allocatable().to_vec();
    // Remove pre-assigned arg registers from the free pool — they're
    // occupied by incoming args at function entry.
    for &r in preassigned_reg.values() {
        free_pool.retain(|&p| p != r);
    }

    let mut spill_offset: u32 = func.frame_size as u32 + 8;
    let mut used_callee_saved: Vec<PhysReg> = Vec::new();

    for interval in &sorted {
        // Handle pre-assigned VRegs (function params).
        if preassigned_all.contains(&interval.vreg) {
            if let Some(&reg) = preassigned_reg.get(&interval.vreg) {
                // Check if the pre-assigned register is clobbered during
                // this VReg's live range (e.g., a call clobbers caller-saved
                // registers). If so, spill the VReg to the stack — the
                // codegen prologue will save the arg register to the spill
                // slot, and uses will load from the stack.
                if is_reg_clobbered_in_range(func, interval.start, interval.end, reg, pure_funcs, no_alloc_funcs) {
                    let aligned = (spill_offset + 7) & !7;
                    spill_offset = aligned + 8;
                    vreg_to_loc.insert(interval.vreg, Location::Spill(aligned));
                    // Update arg_locations so the codegen prologue saves
                    // the arg register to the spill slot.
                    for (i, (vreg_param, _)) in func.params.iter().enumerate() {
                        if vreg_param == &interval.vreg {
                            if i < arg_locations.len() {
                                arg_locations[i] = Location::Spill(aligned);
                            }
                        }
                    }
                    // Return the pre-assigned register to the free pool.
                    if !free_pool.contains(&reg) {
                        free_pool.push(reg);
                    }
                    free_pool.sort_by_key(|r| r.encoding());
                } else {
                    // Register is safe for the entire live range — keep it.
                    active.push(*interval);
                    active.sort_by_key(|i| i.end);
                }
            }
            continue;
        }

        // Expire old intervals: any active interval whose end < this start
        // is no longer live; free its register.
        let mut to_free: Vec<PhysReg> = Vec::new();
        active.retain(|a| {
            if a.end < interval.start {
                if let Some(Location::Reg(r)) = vreg_to_loc.get(&a.vreg) {
                    to_free.push(*r);
                }
                false // remove from active
            } else {
                true
            }
        });
        for r in to_free {
            if !preassigned_reg.values().any(|&p| p == r) {
                if !free_pool.contains(&r) {
                    free_pool.push(r);
                }
            }
        }
        free_pool.sort_by_key(|r| r.encoding());

        // Try to assign a free register that is not clobbered during this interval.
        let assigned_reg = free_pool.iter().find(|&&r| {
            !is_reg_clobbered_in_range(func, interval.start, interval.end, r, pure_funcs, no_alloc_funcs)
        }).copied();
        if let Some(reg) = assigned_reg {
            free_pool.retain(|&r| r != reg);
            vreg_to_loc.insert(interval.vreg, Location::Reg(reg));
            active.push(*interval);
            active.sort_by_key(|i| i.end);
            if reg.is_callee_saved() && !used_callee_saved.contains(&reg) {
                used_callee_saved.push(reg);
            }
        } else {
            // No free register — spill. Choose the interval with the farthest
            // end point among (active + current). If the current interval has
            // the farthest end, spill it. Otherwise, spill the farthest-active
            // and give its register to the current.
            let mut candidates: Vec<&LiveInterval> = active.iter().collect();
            candidates.push(interval);
            candidates.sort_by_key(|i| i.end);

            let spill_vreg = candidates.last().unwrap().vreg;
            let is_current = spill_vreg == interval.vreg;

            if is_current {
                // Spill the current interval.
                // F-1A: If this VReg is a constant, rematerialize it instead
                // of consuming a stack slot. The constant is reloaded at each
                // use site by the codegen.
                if let Some(&c) = constant_vregs.get(&interval.vreg) {
                    vreg_to_loc.insert(interval.vreg, Location::Remat(c));
                } else {
                    let aligned = (spill_offset + 7) & !7;
                    spill_offset = aligned + 8;
                    vreg_to_loc.insert(interval.vreg, Location::Spill(aligned));
                }
            } else {
                // Spill the farthest-active, take its register.
                let reg = match vreg_to_loc.get(&spill_vreg) {
                    Some(Location::Reg(r)) => *r,
                    _ => {
                        // Fallback: just spill current.
                        // F-1A: remat constants instead of spilling.
                        if let Some(&c) = constant_vregs.get(&interval.vreg) {
                            vreg_to_loc.insert(interval.vreg, Location::Remat(c));
                        } else {
                            let aligned = (spill_offset + 7) & !7;
                            spill_offset = aligned + 8;
                            vreg_to_loc.insert(interval.vreg, Location::Spill(aligned));
                        }
                        continue;
                    }
                };
                // Check if the register is clobbered in the current interval's range.
                if is_reg_clobbered_in_range(func, interval.start, interval.end, reg, pure_funcs, no_alloc_funcs) {
                    // Can't use this register — spill current instead.
                    // F-1A: remat constants instead of spilling.
                    if let Some(&c) = constant_vregs.get(&interval.vreg) {
                        vreg_to_loc.insert(interval.vreg, Location::Remat(c));
                    } else {
                        let aligned = (spill_offset + 7) & !7;
                        spill_offset = aligned + 8;
                        vreg_to_loc.insert(interval.vreg, Location::Spill(aligned));
                    }
                    continue;
                }
                // F-1A: If the evicted VReg is a constant, rematerialize it
                // instead of consuming a stack slot.
                if let Some(&c) = constant_vregs.get(&spill_vreg) {
                    vreg_to_loc.insert(spill_vreg, Location::Remat(c));
                } else {
                    let aligned = (spill_offset + 7) & !7;
                    spill_offset = aligned + 8;
                    vreg_to_loc.insert(spill_vreg, Location::Spill(aligned));
                }
                active.retain(|a| a.vreg != spill_vreg);

                vreg_to_loc.insert(interval.vreg, Location::Reg(reg));
                active.push(*interval);
                active.sort_by_key(|i| i.end);
                if reg.is_callee_saved() && !used_callee_saved.contains(&reg) {
                    used_callee_saved.push(reg);
                }
            }
        }
    }

    // Sort callee-saved registers by encoding for deterministic prologue.
    used_callee_saved.sort_by_key(|r| r.encoding());

    Allocation {
        vreg_to_loc,
        used_callee_saved,
        spill_size: spill_offset - (func.frame_size as u32 + 8),
        arg_locations,
    }
}

/// Run the allocator on all functions in a module.
pub fn allocate_module(module: &L3Module) -> ModuleAllocation {
    let mut result = ModuleAllocation::default();
    for f in &module.functions {
        result.func_allocations.insert(f.name.clone(), allocate(f, &module.pure_functions, &module.no_alloc_functions));
    }
    result
}

// ═══════════════════════════════════════════════════════════════════════════
// Live interval computation
// ═══════════════════════════════════════════════════════════════════════════

/// Compute the set of VRegs that hold a constant value and are safe to
/// rematerialize. A VReg is rematerializable if its sole definition is
/// `Mov { src: Operand::ImmI(c) }` — the constant can be reloaded at each
/// use site with a single `movq $c, %reg` instruction, avoiding both
/// register pressure and stack spill slots. (F-1A)
fn compute_constant_vregs(func: &L3Function) -> HashMap<VReg, i64> {
    let mut constants: HashMap<VReg, i64> = HashMap::new();
    for block in &func.blocks {
        for inst in &block.instructions {
            if let L3Inst::Mov { dst, src: Operand::ImmI(c) } = inst {
                constants.insert(*dst, *c);
            }
        }
    }
    constants
}

/// Compute live intervals for all VRegs in a function.
///
/// We use a simple linear-scan-friendly approach: flatten all instructions
/// into a single instruction index space (block by block), then for each
/// VReg, find its first def and last use.
///
/// This is NOT a precise liveness analysis (no dataflow). It's the classic
/// linear-scan simplification: each VReg's interval spans from its first
/// definition to its last use across the whole function. This is
/// conservative (intervals may be longer than necessary) but correct.
fn compute_live_intervals(func: &L3Function) -> HashMap<VReg, LiveInterval> {
    let mut intervals: HashMap<VReg, LiveInterval> = HashMap::new();
    let mut idx: u32 = 0;

    // Helper to record a def (start of interval).
    let mut def = |vreg: VReg, pos: u32, intervals: &mut HashMap<VReg, LiveInterval>| {
        let entry = intervals.entry(vreg).or_insert(LiveInterval {
            vreg,
            start: pos,
            end: pos,
        });
        if pos < entry.start {
            entry.start = pos;
        }
    };

    // Helper to record a use (extends end of interval).
    let mut use_ = |vreg: VReg, pos: u32, intervals: &mut HashMap<VReg, LiveInterval>| {
        let entry = intervals.entry(vreg).or_insert(LiveInterval {
            vreg,
            start: pos,
            end: pos,
        });
        if pos > entry.end {
            entry.end = pos;
        }
    };

    // Function params are live from instruction 0.
    for (vreg, _) in &func.params {
        def(*vreg, 0, &mut intervals);
    }

    // Track block start/end indices for loop handling.
    let mut block_ranges: Vec<(PBlock, u32, u32)> = Vec::new();

    for block in &func.blocks {
        let block_start = idx;
        // Block params are defined at block entry.
        for (vreg, _) in &block.params {
            def(*vreg, idx, &mut intervals);
        }

        for inst in &block.instructions {
            // Record defs (the destination VReg).
            if let Some(dst) = inst_def(inst) {
                def(dst, idx, &mut intervals);
            }

            // Record uses (all VReg operands).
            for vreg in inst_uses(inst) {
                use_(vreg, idx, &mut intervals);
            }

            idx += 1;
        }
        block_ranges.push((block.id, block_start, idx));
    }

    // ── Loop handling ──
    // A back-edge is a jump from a later block to an earlier block.
    // If a VReg is used in a loop header (the target of a back-edge),
    // its interval must extend to cover the entire loop body, because
    // the value must survive each iteration.
    let block_end = |bid: PBlock| -> u32 {
        block_ranges.iter()
            .find(|(id, _, _)| *id == bid)
            .map(|(_, _, end)| *end)
            .unwrap_or(0)
    };
    let block_start = |bid: PBlock| -> u32 {
        block_ranges.iter()
            .find(|(id, _, _)| *id == bid)
            .map(|(_, start, _)| *start)
            .unwrap_or(0)
    };

    // Find back-edges: terminators that jump to a block with a lower index.
    // For nested loops, blocks in inner loops may appear after the outer
    // loop's back-edge source in the linear ordering. To handle this,
    // we compute the maximum end across ALL back-edges targeting the same
    // loop header, and also extend to cover any blocks reachable from the
    // loop header that come after the back-edge source.
    let max_block_end = block_ranges.iter().map(|(_, _, end)| *end).max().unwrap_or(0);
    for block in &func.blocks {
        let my_end = block_end(block.id);
        let targets: Vec<PBlock> = match block.instructions.last() {
            Some(L3Inst::Jmp { target }) => vec![*target],
            Some(L3Inst::Br { then, else_, .. }) => vec![*then, *else_],
            _ => vec![],
        };
        for target in targets {
            let target_start = block_start(target);
            if target_start < my_end {
                // This is a back-edge: block → target (target is earlier).
                // Extend all VRegs used in the target block to cover the
                // entire loop body. For nested loops, inner loop blocks may
                // appear after the back-edge source, so we extend to the
                // end of the function (conservative but correct).
                let loop_end = max_block_end;
                // Find all VRegs used in the target block and extend their intervals.
                for tblock in &func.blocks {
                    if tblock.id == target {
                        for inst in &tblock.instructions {
                            for vreg in inst_uses(inst) {
                                if let Some(entry) = intervals.get_mut(&vreg) {
                                    if loop_end > entry.end {
                                        entry.end = loop_end;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    intervals
}

/// Physical registers implicitly clobbered by the codegen for each instruction.
/// The regalloc must not keep live vregs in these registers across the instruction.
///
/// F-1D: Effect-Aware Register Allocation
/// For calls to pure functions (in `pure_funcs`), we DON'T clobber caller-saved
/// registers. A pure function can't modify external state, so caller-saved
/// registers don't need to be preserved. This is a Fides-specific optimization
/// that LLVM cannot do — LLVM must assume any call clobbers everything.
fn inst_clobbers(inst: &L3Inst, pure_funcs: &HashSet<String>, no_alloc_funcs: &HashSet<String>) -> Vec<PhysReg> {
    match inst {
        // Store: gen_store loads value into RAX, pointer into RCX
        L3Inst::Store { .. } => vec![PhysReg::Rax, PhysReg::Rcx],
        // ElemAddr: gen_elem_addr uses scratch_reg (RCX or RDX) for index
        L3Inst::ElemAddr { .. } => vec![PhysReg::Rcx, PhysReg::Rdx],
        // Load: gen_load may use RAX as scratch when ptr is not in a register
        // or when dst is spilled
        L3Inst::Load { .. } => vec![PhysReg::Rax],
        // Branch: gen_branch loads condition into RAX
        L3Inst::Br { .. } => vec![PhysReg::Rax],
        // Print: call to p_print_int/p_print_str clobbers caller-saved
        L3Inst::Print { .. } => vec![PhysReg::Rax, PhysReg::Rcx, PhysReg::Rdx,
            PhysReg::Rsi, PhysReg::Rdi, PhysReg::R8, PhysReg::R9, PhysReg::R10, PhysReg::R11],
        // F-1D: Calls to pure functions don't clobber caller-saved registers.
        // The pure function can't modify external state, so our caller-saved
        // registers are safe. This lets the regalloc keep live values in
        // caller-saved registers across pure calls — fewer spills/reloads.
        L3Inst::Call { target, .. } if pure_funcs.contains(target) => {
            // Pure function: only RAX is clobbered (it holds the return value).
            vec![PhysReg::Rax]
        }
        // I-2D: Calls to @no-alloc functions don't call p_malloc/p_alloc, so
        // they don't need allocator state preservation. Treat like pure calls
        // for clobbering — only RAX clobbered (return value).
        L3Inst::Call { target, .. } if no_alloc_funcs.contains(target) => {
            vec![PhysReg::Rax]
        }
        // Non-pure calls clobber all caller-saved registers
        L3Inst::Call { .. } | L3Inst::CallPtr { .. } | L3Inst::DynCall { .. } => {
            vec![PhysReg::Rax, PhysReg::Rcx, PhysReg::Rdx,
                PhysReg::Rsi, PhysReg::Rdi, PhysReg::R8, PhysReg::R9, PhysReg::R10, PhysReg::R11]
        }
        // Panic: calls p_panic
        L3Inst::Panic { .. } => vec![PhysReg::Rax, PhysReg::Rcx, PhysReg::Rdx,
            PhysReg::Rsi, PhysReg::Rdi, PhysReg::R8, PhysReg::R9, PhysReg::R10, PhysReg::R11],
        // Bin: gen_bin uses scratch registers for Mul, Div, Mod, Shift, And, Or.
        // Mul uses scratch_reg(dst) = RCX or RDX.
        // Div/Mod clobber RAX, RDX, and RCX.
        // Shift uses RCX (CL) for non-immediate counts.
        // And/Or use scratch_reg(dst) = RCX or RDX.
        // Add/Sub/BitAnd/BitOr/BitXor only use scratch for exotic operand types.
        // Comparisons use scratch for exotic operand types.
        // To be safe, clobber RCX and RDX for all binops, plus RAX for Div/Mod.
        L3Inst::Bin { op, .. } => {
            match op {
                BinOp::Div | BinOp::Mod => {
                    vec![PhysReg::Rax, PhysReg::Rcx, PhysReg::Rdx]
                }
                _ => {
                    vec![PhysReg::Rcx, PhysReg::Rdx]
                }
            }
        }
        // Unary: Neg (float) clobbers RAX and RCX. Not clobbers RAX.
        L3Inst::Unary { op, .. } => {
            match op {
                crate::ast::UnaryOp::Neg => vec![PhysReg::Rax, PhysReg::Rcx],
                crate::ast::UnaryOp::Not => vec![PhysReg::Rax],
                _ => vec![],
            }
        }
        _ => vec![],
    }
}

/// Check if a register is clobbered by any instruction in the given range.
fn is_reg_clobbered_in_range(
    func: &L3Function,
    start: u32,
    end: u32,
    reg: PhysReg,
    pure_funcs: &HashSet<String>,
    no_alloc_funcs: &HashSet<String>,
) -> bool {
    let mut idx: u32 = 0;
    for block in &func.blocks {
        for inst in &block.instructions {
            if idx >= start && idx <= end {
                if inst_clobbers(inst, pure_funcs, no_alloc_funcs).contains(&reg) {
                    return true;
                }
            }
            idx += 1;
        }
    }
    false
}

/// The VReg defined by an instruction (its destination), if any.
fn inst_def(inst: &L3Inst) -> Option<VReg> {
    match inst {
        L3Inst::Mov { dst, .. }
        | L3Inst::AddrOfSlot { dst, .. }
        | L3Inst::GlobalAddr { dst, .. }
        | L3Inst::Bin { dst, .. }
        | L3Inst::Unary { dst, .. }
        | L3Inst::StackAlloc { dst, .. }
        | L3Inst::Load { dst, .. }
        | L3Inst::FieldAddr { dst, .. }
        | L3Inst::ElemAddr { dst, .. }
        | L3Inst::MakeDyn { dst, .. }
        | L3Inst::Cast { dst, .. } => Some(*dst),
        L3Inst::Call { dst, .. } | L3Inst::CallPtr { dst, .. } | L3Inst::DynCall { dst, .. } => {
            *dst
        }
        L3Inst::TryUnwrap { ok_dst, .. } => Some(*ok_dst),
        L3Inst::Store { .. }
        | L3Inst::Jmp { .. }
        | L3Inst::Br { .. }
        | L3Inst::Ret { .. }
        | L3Inst::Unreachable
        | L3Inst::Print { .. }
        | L3Inst::Panic { .. } => None,
    }
}

/// All VRegs used (read) by an instruction.
fn inst_uses(inst: &L3Inst) -> Vec<VReg> {
    let mut uses = Vec::new();
    let mut collect = |op: &Operand, uses: &mut Vec<VReg>| {
        if let Operand::VReg(r) = op {
            uses.push(*r);
        }
    };

    match inst {
        L3Inst::Mov { src, .. } => collect(src, &mut uses),
        L3Inst::AddrOfSlot { .. } => {}
        L3Inst::GlobalAddr { .. } => {}
        L3Inst::Bin { a, b, .. } => {
            collect(a, &mut uses);
            collect(b, &mut uses);
        }
        L3Inst::Unary { src, .. } => collect(src, &mut uses),
        L3Inst::StackAlloc { .. } => {}
        L3Inst::Load { ptr, .. } => collect(ptr, &mut uses),
        L3Inst::Store { ptr, val, .. } => {
            collect(ptr, &mut uses);
            collect(val, &mut uses);
        }
        L3Inst::FieldAddr { base, .. } => collect(base, &mut uses),
        L3Inst::ElemAddr { base, index, .. } => {
            collect(base, &mut uses);
            collect(index, &mut uses);
        }
        L3Inst::Jmp { .. } => {}
        L3Inst::Br { cond, .. } => collect(cond, &mut uses),
        L3Inst::Ret { value: Some(v) } => collect(v, &mut uses),
        L3Inst::Ret { value: None } => {}
        L3Inst::Unreachable => {}
        L3Inst::Call { args, .. } | L3Inst::DynCall { args, .. } => {
            for a in args {
                collect(a, &mut uses);
            }
        }
        L3Inst::CallPtr { fn_ptr, args, .. } => {
            collect(fn_ptr, &mut uses);
            for a in args {
                collect(a, &mut uses);
            }
        }
        L3Inst::MakeDyn { value, .. } => collect(value, &mut uses),
        L3Inst::Cast { src, .. } => collect(src, &mut uses),
        L3Inst::TryUnwrap { value, .. } => collect(value, &mut uses),
        L3Inst::Print { val, .. } => collect(val, &mut uses),
        L3Inst::Panic { .. } => {}
    }

    uses
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;
    use crate::pir::{L3Block, L3Function, L3Inst, Operand, VReg};
    use crate::ir::{BlockId, IrType};

    fn vreg(n: u32) -> VReg {
        VReg(n)
    }

    fn make_func(name: &str, blocks: Vec<L3Block>) -> L3Function {
        L3Function {
            name: name.to_string(),
            params: Vec::new(),
            ret_ty: IrType::I64,
            blocks,
            entry: BlockId(0),
            frame_size: 0,
            next_vreg: 100,
            is_kernel: false,
            workgroup_size: [1, 1, 1],
        }
    }

    #[test]
    fn interval_overlap() {
        let a = LiveInterval { vreg: vreg(0), start: 0, end: 5 };
        let b = LiveInterval { vreg: vreg(1), start: 3, end: 10 };
        let c = LiveInterval { vreg: vreg(2), start: 6, end: 8 };
        assert!(a.overlaps(&b));
        assert!(!a.overlaps(&c));
        assert!(b.overlaps(&c));
    }

    #[test]
    fn interval_contains() {
        let i = LiveInterval { vreg: vreg(0), start: 2, end: 7 };
        assert!(i.contains(2));
        assert!(i.contains(7));
        assert!(i.contains(5));
        assert!(!i.contains(1));
        assert!(!i.contains(8));
    }

    #[test]
    fn allocates_single_vreg_to_register() {
        // One block: v0 = Mov #42; Ret v0
        let block = L3Block {
            id: BlockId(0),
            instructions: vec![
                L3Inst::Mov { dst: vreg(0), src: Operand::ImmI(42) },
                L3Inst::Ret { value: Some(Operand::VReg(vreg(0))) },
            ],
            predecessors: Vec::new(),
            params: Vec::new(),
        };
        let func = make_func("test", vec![block]);
        let alloc = allocate(&func, &HashSet::new(), &HashSet::new());
        assert!(alloc.reg(vreg(0)).is_some(), "v0 should get a register");
    }

    #[test]
    fn allocates_non_overlapping_to_same_reg() {
        // v0 lives [0,1], v1 lives [2,3] — should share a register.
        let block = L3Block {
            id: BlockId(0),
            instructions: vec![
                L3Inst::Mov { dst: vreg(0), src: Operand::ImmI(1) },
                L3Inst::Ret { value: Some(Operand::VReg(vreg(0))) },
                L3Inst::Mov { dst: vreg(1), src: Operand::ImmI(2) },
                L3Inst::Ret { value: Some(Operand::VReg(vreg(1))) },
            ],
            predecessors: Vec::new(),
            params: Vec::new(),
        };
        let func = make_func("test", vec![block]);
        let alloc = allocate(&func, &HashSet::new(), &HashSet::new());
        let r0 = alloc.reg(vreg(0));
        let r1 = alloc.reg(vreg(1));
        assert!(r0.is_some() && r1.is_some());
        // Non-overlapping intervals may reuse the same register.
        // (Not required, but likely with linear scan.)
    }

    #[test]
    fn spills_when_pressure_exceeds_registers() {
        // Create more live VRegs than physical registers (14 allocatable).
        // Use 20 VRegs all live simultaneously. Define them via non-constant
        // sources (loads from a stack slot) so they are NOT rematerializable
        // — the spill path must be exercised, not the remat path (F-1A).
        let mut insts = Vec::new();
        // Define 20 VRegs as loads from slot 0 (non-constant → must spill).
        for i in 0..20 {
            insts.push(L3Inst::Load {
                dst: vreg(i),
                ptr: Operand::Slot(crate::pir::Slot(0)),
                ty: IrType::I64,
            });
        }
        // Use all 20 (keeps them all live until the end)
        for i in 0..20 {
            insts.push(L3Inst::Ret {
                value: Some(Operand::VReg(vreg(i))),
            });
        }
        let block = L3Block {
            id: BlockId(0),
            instructions: insts,
            predecessors: Vec::new(),
            params: Vec::new(),
        };
        let func = make_func("test", vec![block]);
        let alloc = allocate(&func, &HashSet::new(), &HashSet::new());

        let in_reg: usize = (0..20).filter(|i| alloc.reg(vreg(*i)).is_some()).count();
        let in_spill: usize = (0..20).filter(|i| matches!(alloc.loc(vreg(*i)), Location::Spill(_))).count();
        assert!(in_reg <= 14, "at most 14 VRegs in registers, got {}", in_reg);
        assert!(in_spill >= 6, "at least 6 VRegs spilled, got {}", in_spill);
        assert_eq!(in_reg + in_spill, 20);
    }

    #[test]
    fn rematerializes_constants_instead_of_spilling() {
        // F-1A: When register pressure exceeds the number of physical
        // registers, constant VRegs (defined by Mov ImmI) should be
        // rematerialized, not spilled to the stack.
        let mut insts = Vec::new();
        // Define 20 constant VRegs — all would be spilled without remat.
        for i in 0..20 {
            insts.push(L3Inst::Mov {
                dst: vreg(i),
                src: Operand::ImmI(i as i64),
            });
        }
        // Use all 20 (keeps them all live until the end)
        for i in 0..20 {
            insts.push(L3Inst::Ret {
                value: Some(Operand::VReg(vreg(i))),
            });
        }
        let block = L3Block {
            id: BlockId(0),
            instructions: insts,
            predecessors: Vec::new(),
            params: Vec::new(),
        };
        let func = make_func("test", vec![block]);
        let alloc = allocate(&func, &HashSet::new(), &HashSet::new());

        let in_reg: usize = (0..20).filter(|i| alloc.reg(vreg(*i)).is_some()).count();
        let in_spill: usize = (0..20).filter(|i| matches!(alloc.loc(vreg(*i)), Location::Spill(_))).count();
        let in_remat: usize = (0..20).filter(|i| matches!(alloc.loc(vreg(*i)), Location::Remat(_))).count();
        // Constants that don't fit in registers are rematerialized, not spilled.
        assert!(in_reg <= 14, "at most 14 VRegs in registers, got {}", in_reg);
        assert_eq!(in_spill, 0, "constants should never be spilled, got {} spilled", in_spill);
        assert_eq!(in_reg + in_remat, 20, "all VRegs should be in reg or remat, got {}+{}", in_reg, in_remat);
        // Verify the rematerialized constants have the right values.
        for i in 0..20 {
            if let Location::Remat(c) = alloc.loc(vreg(i)) {
                assert_eq!(c, i as i64, "remat constant for v{} should be {}", i, i);
            }
        }
    }

    #[test]
    fn function_params_get_arg_registers() {
        // Function with 3 params — should get RDI, RSI, RDX.
        let func = L3Function {
            name: "test".to_string(),
            params: vec![
                (vreg(0), IrType::I64),
                (vreg(1), IrType::I64),
                (vreg(2), IrType::I64),
            ],
            ret_ty: IrType::I64,
            blocks: vec![L3Block {
                id: BlockId(0),
                instructions: vec![L3Inst::Ret {
                    value: Some(Operand::VReg(vreg(0))),
                }],
                predecessors: Vec::new(),
                params: Vec::new(),
            }],
            entry: BlockId(0),
            frame_size: 0,
            next_vreg: 10,
            is_kernel: false,
            workgroup_size: [1, 1, 1],
        };
        let alloc = allocate(&func, &HashSet::new(), &HashSet::new());
        assert_eq!(alloc.reg(vreg(0)), Some(PhysReg::Rdi));
        assert_eq!(alloc.reg(vreg(1)), Some(PhysReg::Rsi));
        assert_eq!(alloc.reg(vreg(2)), Some(PhysReg::Rdx));
    }

    #[test]
    fn extra_params_spill_to_stack() {
        // Function with 10 params — first 6 in registers, rest on stack.
        let params: Vec<(VReg, IrType)> = (0..10)
            .map(|i| (vreg(i), IrType::I64))
            .collect();
        let func = L3Function {
            name: "test".to_string(),
            params,
            ret_ty: IrType::I64,
            blocks: vec![L3Block {
                id: BlockId(0),
                instructions: vec![L3Inst::Ret {
                    value: Some(Operand::VReg(vreg(0))),
                }],
                predecessors: Vec::new(),
                params: Vec::new(),
            }],
            entry: BlockId(0),
            frame_size: 0,
            next_vreg: 20,
            is_kernel: false,
            workgroup_size: [1, 1, 1],
        };
        let alloc = allocate(&func, &HashSet::new(), &HashSet::new());
        // First 6 in registers
        for i in 0..6 {
            assert!(alloc.reg(vreg(i)).is_some(), "param {} should be in a register", i);
        }
        // Rest on stack
        for i in 6..10 {
            assert!(
                matches!(alloc.loc(vreg(i)), Location::Spill(_)),
                "param {} should be spilled", i
            );
        }
    }

    #[test]
    fn inst_def_and_uses() {
        // Bin: defines dst, uses a and b
        let inst = L3Inst::Bin {
            op: crate::ast::BinOp::Add,
            dst: vreg(3),
            a: Operand::VReg(vreg(1)),
            b: Operand::VReg(vreg(2)),
            ty: IrType::I64,
        };
        assert_eq!(inst_def(&inst), Some(vreg(3)));
        let uses = inst_uses(&inst);
        assert!(uses.contains(&vreg(1)));
        assert!(uses.contains(&vreg(2)));

        // Ret: no def, uses the value
        let inst = L3Inst::Ret {
            value: Some(Operand::VReg(vreg(5))),
        };
        assert_eq!(inst_def(&inst), None);
        assert!(inst_uses(&inst).contains(&vreg(5)));

        // Store: no def, uses ptr and val
        let inst = L3Inst::Store {
            ptr: Operand::VReg(vreg(1)),
            val: Operand::VReg(vreg(2)),
            ty: IrType::I64,
        };
        assert_eq!(inst_def(&inst), None);
        let uses = inst_uses(&inst);
        assert!(uses.contains(&vreg(1)));
        assert!(uses.contains(&vreg(2)));
    }

    #[test]
    fn phys_reg_names() {
        assert_eq!(PhysReg::Rax.name(), "rax");
        assert_eq!(PhysReg::R15.name(), "r15");
        assert_eq!(PhysReg::Rdi.name(), "rdi");
    }

    #[test]
    fn phys_reg_callee_saved() {
        assert!(PhysReg::Rbx.is_callee_saved());
        assert!(PhysReg::R12.is_callee_saved());
        assert!(!PhysReg::Rax.is_callee_saved());
        assert!(!PhysReg::Rdi.is_callee_saved());
    }

    #[test]
    fn arg_regs_order() {
        let regs = PhysReg::arg_regs();
        assert_eq!(regs.len(), 6);
        assert_eq!(regs[0], PhysReg::Rdi);
        assert_eq!(regs[1], PhysReg::Rsi);
        assert_eq!(regs[2], PhysReg::Rdx);
        assert_eq!(regs[3], PhysReg::Rcx);
        assert_eq!(regs[4], PhysReg::R8);
        assert_eq!(regs[5], PhysReg::R9);
    }

    #[test]
    fn allocatable_excludes_rsp_rbp() {
        let alloc = PhysReg::allocatable();
        assert!(!alloc.contains(&PhysReg::Rsp));
        assert!(!alloc.contains(&PhysReg::Rbp));
        assert_eq!(alloc.len(), 14);
    }

    #[test]
    fn f1d_pure_call_does_not_clobber_caller_saved() {
        // F-1D: A call to a pure function should only clobber RAX (return value),
        // not all caller-saved registers. This lets the regalloc keep live values
        // in caller-saved registers across pure calls.
        let pure_funcs: HashSet<String> = {
            let mut s = HashSet::new();
            s.insert("pure_func".to_string());
            s
        };
        let pure_call = L3Inst::Call {
            dst: Some(vreg(0)),
            target: "pure_func".to_string(),
            args: vec![],
            ret_ty: IrType::I64,
        };
        let no_alloc_funcs: HashSet<String> = HashSet::new();
        let clobbers = inst_clobbers(&pure_call, &pure_funcs, &no_alloc_funcs);
        // Pure call only clobbers RAX (return value register).
        assert_eq!(clobbers, vec![PhysReg::Rax],
            "pure call should only clobber RAX, got {:?}", clobbers);

        // Non-pure call clobbers all caller-saved registers.
        let nonpure_call = L3Inst::Call {
            dst: Some(vreg(0)),
            target: "impure_func".to_string(),
            args: vec![],
            ret_ty: IrType::I64,
        };
        let clobbers = inst_clobbers(&nonpure_call, &pure_funcs, &no_alloc_funcs);
        assert!(clobbers.contains(&PhysReg::Rcx), "non-pure call should clobber RCX");
        assert!(clobbers.contains(&PhysReg::Rdx), "non-pure call should clobber RDX");
        assert!(clobbers.contains(&PhysReg::Rsi), "non-pure call should clobber RSI");
        assert!(clobbers.contains(&PhysReg::Rdi), "non-pure call should clobber RDI");
    }

    #[test]
    fn f1d_pure_call_allows_caller_saved_across_call() {
        // F-1D: A VReg live across a pure call should be able to stay in a
        // caller-saved register (e.g., RCX), since the pure call doesn't
        // clobber it. Without F-1D, the VReg would be spilled.
        let pure_funcs: HashSet<String> = {
            let mut s = HashSet::new();
            s.insert("pure_func".to_string());
            s
        };
        // v0 = load from slot 0
        // v1 = call pure_func()  -- pure, doesn't clobber RCX
        // v2 = binop add v0, v0  -- uses v0 which should still be in RCX
        let block = L3Block {
            id: BlockId(0),
            instructions: vec![
                L3Inst::Load {
                    dst: vreg(0),
                    ptr: Operand::Slot(crate::pir::Slot(0)),
                    ty: IrType::I64,
                },
                L3Inst::Call {
                    dst: Some(vreg(1)),
                    target: "pure_func".to_string(),
                    args: vec![],
                    ret_ty: IrType::I64,
                },
                L3Inst::Bin {
                    op: crate::ast::BinOp::Add,
                    dst: vreg(2),
                    a: Operand::VReg(vreg(0)),
                    b: Operand::VReg(vreg(0)),
                    ty: IrType::I64,
                },
                L3Inst::Ret {
                    value: Some(Operand::VReg(vreg(2))),
                },
            ],
            predecessors: Vec::new(),
            params: Vec::new(),
        };
        let func = make_func("test", vec![block]);
        let alloc = allocate(&func, &pure_funcs, &HashSet::new());

        // v0 should get a caller-saved register (not spilled), because the
        // pure call doesn't clobber it.
        let v0_loc = alloc.loc(vreg(0));
        assert!(
            matches!(v0_loc, Location::Reg(_)),
            "v0 should be in a register (not spilled) across pure call, got {:?}",
            v0_loc
        );
    }
}
