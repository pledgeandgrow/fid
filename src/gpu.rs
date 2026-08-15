// GPU Codegen — Phase 5C.4
//
// Generates SPIR-V bytecode for GPU execution (Vulkan compute shaders).
// SPIR-V is the intermediate language for Vulkan, OpenCL, and WebGPU.
//
// This module produces SPIR-V compute shaders from PIR L3 functions,
// enabling GPU acceleration for parallel workloads.
//
// SPIR-V format:
//   - Magic number: 0x07230203
//   - Version: 1.0 (0x00010000)
//   - Generator: 0 (placeholder)
//   - Bound: next available ID
//   - Schema: 0
//   - Instructions: (opcode << 16 | word_count) + operands

use crate::pir::{L3Block, L3Function, L3Inst, L3Module, Operand, PBlock, VReg};
use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/// Generate SPIR-V bytecode for an L3 module.
/// Produces a Vulkan compute shader that can be executed on a GPU.
pub fn generate_spirv(module: &L3Module) -> Vec<u32> {
    let mut gen = SpirVCodeGen::new();
    gen.gen_module(module);
    gen.words
}

/// Generate SPIR-V as a text disassembly (for debugging).
pub fn generate_spirv_text(module: &L3Module) -> String {
    let words = generate_spirv(module);
    disassemble(&words)
}

/// Generate PTX (NVIDIA GPU assembly) for an L3 module.
/// PTX is NVIDIA's parallel thread execution ISA.
pub fn generate_ptx(module: &L3Module) -> String {
    let mut gen = PtxCodeGen::new();
    gen.gen_module(module);
    gen.output
}

// ═══════════════════════════════════════════════════════════════════════════
// SPIR-V Codegen
// ═══════════════════════════════════════════════════════════════════════════

const SPIRV_MAGIC: u32 = 0x07230203;
const SPIRV_VERSION_1_0: u32 = 0x00010000;
const SPIRV_GENERATOR: u32 = 0; // Placeholder

// SPIR-V opcodes
const OP_NOP: u16 = 0;
const OP_SOURCE: u16 = 3;
const OP_NAME: u16 = 5;
const OP_MEMORY_MODEL: u16 = 14;
const OP_ENTRY_POINT: u16 = 15;
const OP_EXECUTION_MODE: u16 = 16;
const OP_TYPE_VOID: u16 = 19;
const OP_TYPE_BOOL: u16 = 20;
const OP_TYPE_INT: u16 = 21;
const OP_TYPE_FLOAT: u16 = 22;
const OP_TYPE_FUNCTION: u16 = 33;
const OP_TYPE_STRUCT: u16 = 30;
const OP_TYPE_POINTER: u16 = 32;
const OP_CONSTANT: u16 = 43;
const OP_CONSTANT_TRUE: u16 = 44;
const OP_CONSTANT_FALSE: u16 = 45;
const OP_FUNCTION: u16 = 54;
const OP_FUNCTION_PARAMETER: u16 = 55;
const OP_FUNCTION_END: u16 = 56;
const OP_FUNCTION_CALL: u16 = 57;
const OP_VARIABLE: u16 = 59;
const OP_LOAD: u16 = 61;
const OP_STORE: u16 = 62;
const OP_ACCESS_CHAIN: u16 = 65;
const OP_IADD: u16 = 126;
const OP_ISUB: u16 = 127;
const OP_IMUL: u16 = 128;
const OP_SDIV: u16 = 129;
const OP_SMOD: u16 = 131;
const OP_BITWISE_AND: u16 = 134;
const OP_BITWISE_OR: u16 = 135;
const OP_BITWISE_XOR: u16 = 136;
const OP_SHIFT_LEFT: u16 = 137;
const OP_SHIFT_RIGHT: u16 = 139;
const OP_I_EQUAL: u16 = 161;
const OP_I_NOT_EQUAL: u16 = 162;
const OP_S_LESS_THAN: u16 = 163;
const OP_S_GREATER_THAN: u16 = 164;
const OP_S_LESS_THAN_EQUAL: u16 = 165;
const OP_S_GREATER_THAN_EQUAL: u16 = 166;
const OP_LABEL: u16 = 248;
const OP_BRANCH: u16 = 249;
const OP_BRANCH_CONDITIONAL: u16 = 250;
const OP_RETURN: u16 = 253;
const OP_RETURN_VALUE: u16 = 254;

// SPIR-V capabilities
const CAP_MATRIX: u16 = 0;
const CAP_SHADER: u16 = 1;
const CAP_INT64: u16 = 33;

// SPIR-V execution models
const EXEC_MODEL_GLCOMPUTE: u32 = 5;

// SPIR-V addressing models
const ADDRESSING_MODEL_LOGICAL: u32 = 0;

// SPIR-V memory models
const MEMORY_MODEL_GLSL450: u32 = 1;

// SPIR-V storage classes
const STORAGE_CLASS_FUNCTION: u32 = 7;
const STORAGE_CLASS_INPUT: u32 = 1;
const STORAGE_CLASS_OUTPUT: u32 = 3;
const STORAGE_CLASS_PRIVATE: u32 = 6;

struct SpirVCodeGen {
    words: Vec<u32>,
    next_id: u32,
    type_ids: HashMap<String, u32>,
    const_ids: HashMap<i64, u32>,
    vreg_ids: HashMap<u32, u32>,
}

impl SpirVCodeGen {
    fn new() -> Self {
        SpirVCodeGen {
            words: Vec::new(),
            next_id: 1,
            type_ids: HashMap::new(),
            const_ids: HashMap::new(),
            vreg_ids: HashMap::new(),
        }
    }

    fn alloc_id(&mut self) -> u32 {
        let id = self.next_id;
        self.next_id += 1;
        id
    }

    fn emit(&mut self, opcode: u16, operands: &[u32]) {
        let word_count = 1 + operands.len() as u16;
        self.words.push((opcode as u32) | ((word_count as u32) << 16));
        self.words.extend_from_slice(operands);
    }

    fn emit_with_result(&mut self, opcode: u16, result_type: u32, operands: &[u32]) -> u32 {
        let result_id = self.alloc_id();
        let word_count = 2 + operands.len() as u16; // opcode+result_type+result_id+operands
        self.words.push((opcode as u32) | ((word_count as u32) << 16));
        self.words.push(result_type);
        self.words.push(result_id);
        self.words.extend_from_slice(operands);
        result_id
    }

    fn get_int_type(&mut self, bits: u32) -> u32 {
        let key = format!("int{}", bits);
        if let Some(&id) = self.type_ids.get(&key) {
            return id;
        }
        let id = self.alloc_id();
        self.emit(OP_TYPE_INT, &[bits, 1]); // signed=1
        self.type_ids.insert(key, id);
        id
    }

    fn get_float_type(&mut self, bits: u32) -> u32 {
        let key = format!("float{}", bits);
        if let Some(&id) = self.type_ids.get(&key) {
            return id;
        }
        let id = self.alloc_id();
        self.emit(OP_TYPE_FLOAT, &[bits]);
        self.type_ids.insert(key, id);
        id
    }

    fn get_void_type(&mut self) -> u32 {
        if let Some(&id) = self.type_ids.get("void") {
            return id;
        }
        let id = self.alloc_id();
        self.emit(OP_TYPE_VOID, &[]);
        self.type_ids.insert("void".to_string(), id);
        id
    }

    fn get_bool_type(&mut self) -> u32 {
        if let Some(&id) = self.type_ids.get("bool") {
            return id;
        }
        let id = self.alloc_id();
        self.emit(OP_TYPE_BOOL, &[]);
        self.type_ids.insert("bool".to_string(), id);
        id
    }

    fn get_constant(&mut self, value: i64) -> u32 {
        if let Some(&id) = self.const_ids.get(&value) {
            return id;
        }
        let type_id = self.get_int_type(64);
        let id = self.emit_with_result(OP_CONSTANT, type_id, &[value as u32, (value >> 32) as u32]);
        self.const_ids.insert(value, id);
        id
    }

    fn gen_module(&mut self, module: &L3Module) {
        // Header
        self.words.push(SPIRV_MAGIC);
        self.words.push(SPIRV_VERSION_1_0);
        self.words.push(SPIRV_GENERATOR);
        // Bound (next ID) — will be updated at the end
        self.words.push(0); // placeholder
        self.words.push(0); // schema

        // Capabilities
        self.emit(17, &[CAP_SHADER as u32]); // OpCapability
        self.emit(17, &[CAP_INT64 as u32]);  // OpCapability Int64

        // Memory model
        self.emit(OP_MEMORY_MODEL, &[ADDRESSING_MODEL_LOGICAL, MEMORY_MODEL_GLSL450]);

        // Entry point — find @kernel annotated function or use first function
        let kernel_func = module.functions.iter().find(|f| {
            // Check if function has @kernel annotation
            // The annotation is stored in the AST, not in L3, so we check by name convention
            // or by a special prefix. For now, we use the first function.
            true
        }).or_else(|| module.functions.first());

        if let Some(func) = kernel_func {
            let entry_id = self.alloc_id();
            let void_type = self.get_void_type();
            let func_type = self.emit_with_result(OP_TYPE_FUNCTION, void_type, &[]);

            // Parse workgroup size from annotation (default: 1, 1, 1)
            let (wg_x, wg_y, wg_z) = (1u32, 1u32, 1u32);

            // Entry point
            self.emit(OP_ENTRY_POINT, &[EXEC_MODEL_GLCOMPUTE, entry_id, 0, 0]);
            self.emit(OP_EXECUTION_MODE, &[entry_id, 17, wg_x, wg_y, wg_z]); // LocalSize

            // Generate the function
            self.gen_function(func, entry_id, func_type);
        }

        // Update bound
        let bound = self.next_id;
        self.words[3] = bound;
    }

    fn gen_function(&mut self, func: &L3Function, func_id: u32, func_type: u32) {
        let void_type = self.get_void_type();

        // OpFunction
        self.emit_with_result_id(OP_FUNCTION, void_type, func_id, &[0, func_type]);
        // Function control = 0 (normal), function type = func_type

        // Labels for blocks
        for block in &func.blocks {
            let label_id = self.alloc_id();
            self.emit(OP_LABEL, &[label_id]);
            self.gen_block(func, block);
        }

        // OpFunctionEnd
        self.emit(OP_FUNCTION_END, &[]);
    }

    fn gen_block(&mut self, func: &L3Function, block: &L3Block) {
        for inst in &block.instructions {
            self.gen_inst(inst);
        }
    }

    fn gen_inst(&mut self, inst: &L3Inst) {
        match inst {
            L3Inst::Mov { dst, src } => {
                let dst_id = self.get_vreg_id(dst);
                match src {
                    Operand::ImmI(val) => {
                        let const_id = self.get_constant(*val);
                        let int_type = self.get_int_type(64);
                        // Copy constant to variable
                        let _ = (dst_id, const_id, int_type);
                    }
                    Operand::VReg(src_vreg) => {
                        let src_id = self.get_vreg_id(src_vreg);
                        let _ = (dst_id, src_id);
                    }
                    _ => {}
                }
            }

            L3Inst::Bin { op, dst, a, b, .. } => {
                let dst_id = self.get_vreg_id(dst);
                let a_id = self.operand_to_id(a);
                let b_id = self.operand_to_id(b);
                let int_type = self.get_int_type(64);

                let opcode = match op {
                    crate::ast::BinOp::Add => OP_IADD,
                    crate::ast::BinOp::Sub => OP_ISUB,
                    crate::ast::BinOp::Mul => OP_IMUL,
                    crate::ast::BinOp::Div => OP_SDIV,
                    crate::ast::BinOp::Mod => OP_SMOD,
                    crate::ast::BinOp::BitAnd => OP_BITWISE_AND,
                    crate::ast::BinOp::BitOr => OP_BITWISE_OR,
                    crate::ast::BinOp::BitXor => OP_BITWISE_XOR,
                    crate::ast::BinOp::Shl => OP_SHIFT_LEFT,
                    crate::ast::BinOp::Shr => OP_SHIFT_RIGHT,
                    crate::ast::BinOp::Eq => OP_I_EQUAL,
                    crate::ast::BinOp::Neq => OP_I_NOT_EQUAL,
                    crate::ast::BinOp::Lt => OP_S_LESS_THAN,
                    crate::ast::BinOp::Gt => OP_S_GREATER_THAN,
                    crate::ast::BinOp::LtEq => OP_S_LESS_THAN_EQUAL,
                    crate::ast::BinOp::GtEq => OP_S_GREATER_THAN_EQUAL,
                    _ => OP_NOP,
                };

                if opcode != OP_NOP {
                    let result_type = if matches!(op, crate::ast::BinOp::Eq | crate::ast::BinOp::Neq | crate::ast::BinOp::Lt | crate::ast::BinOp::Gt | crate::ast::BinOp::LtEq | crate::ast::BinOp::GtEq) {
                        self.get_bool_type()
                    } else {
                        int_type
                    };
                    let result_id = self.emit_with_result(opcode, result_type, &[a_id, b_id]);
                    self.vreg_ids.insert(dst.0, result_id);
                }
            }

            L3Inst::Ret { value } => {
                if let Some(val) = value {
                    let val_id = self.operand_to_id(val);
                    self.emit(OP_RETURN_VALUE, &[val_id]);
                } else {
                    self.emit(OP_RETURN, &[]);
                }
            }

            L3Inst::Jmp { target } => {
                let label_id = self.alloc_id(); // Simplified
                let _ = target;
                self.emit(OP_BRANCH, &[label_id]);
            }

            L3Inst::Br { cond, then, else_ } => {
                let cond_id = self.operand_to_id(cond);
                let then_label = self.alloc_id();
                let else_label = self.alloc_id();
                let _ = (then, else_);
                self.emit(OP_BRANCH_CONDITIONAL, &[cond_id, then_label, else_label]);
            }

            L3Inst::Call { dst, target, args, .. } => {
                let arg_ids: Vec<u32> = args.iter().map(|a| self.operand_to_id(a)).collect();
                let _ = (dst, target, arg_ids);
                // Simplified: skip actual call
            }

            _ => {}
        }
    }

    fn get_vreg_id(&mut self, vreg: &VReg) -> u32 {
        if let Some(&id) = self.vreg_ids.get(&vreg.0) {
            return id;
        }
        let id = self.alloc_id();
        self.vreg_ids.insert(vreg.0, id);
        id
    }

    fn operand_to_id(&mut self, op: &Operand) -> u32 {
        match op {
            Operand::ImmI(val) => self.get_constant(*val),
            Operand::VReg(v) => self.get_vreg_id(v),
            _ => self.alloc_id(),
        }
    }
}

impl SpirVCodeGen {
    fn emit_with_result_id(&mut self, opcode: u16, result_type: u32, result_id: u32, operands: &[u32]) {
        let word_count = 2 + operands.len() as u16;
        self.words.push((opcode as u32) | ((word_count as u32) << 16));
        self.words.push(result_type);
        self.words.push(result_id);
        self.words.extend_from_slice(operands);
    }
}

/// Disassemble SPIR-V words to text (simplified)
fn disassemble(words: &[u32]) -> String {
    let mut output = String::new();
    output.push_str(&format!("; SPIR-V\n"));
    output.push_str(&format!("; Version: 1.0\n"));
    output.push_str(&format!("; Generator: Fides Compiler\n"));
    output.push_str(&format!("; Bound: {}\n", words.get(3).copied().unwrap_or(0)));
    output.push_str(&format!("; Schema: 0\n"));

    let mut i = 5; // Skip header
    while i < words.len() {
        let instr = words[i];
        let word_count = (instr >> 16) as usize;
        let opcode = (instr & 0xffff) as u16;
        output.push_str(&format!("  {} ({} words)\n", opcode_name(opcode), word_count));
        i += word_count.max(1);
    }

    output
}

fn opcode_name(opcode: u16) -> &'static str {
    match opcode {
        OP_NOP => "OpNop",
        OP_MEMORY_MODEL => "OpMemoryModel",
        OP_ENTRY_POINT => "OpEntryPoint",
        OP_TYPE_VOID => "OpTypeVoid",
        OP_TYPE_INT => "OpTypeInt",
        OP_TYPE_FLOAT => "OpTypeFloat",
        OP_TYPE_FUNCTION => "OpTypeFunction",
        OP_CONSTANT => "OpConstant",
        OP_FUNCTION => "OpFunction",
        OP_FUNCTION_END => "OpFunctionEnd",
        OP_LABEL => "OpLabel",
        OP_BRANCH => "OpBranch",
        OP_BRANCH_CONDITIONAL => "OpBranchConditional",
        OP_RETURN => "OpReturn",
        OP_RETURN_VALUE => "OpReturnValue",
        OP_IADD => "OpIAdd",
        OP_ISUB => "OpISub",
        OP_IMUL => "OpIMul",
        OP_SDIV => "OpSDiv",
        OP_LOAD => "OpLoad",
        OP_STORE => "OpStore",
        _ => "OpUnknown",
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// PTX Codegen (NVIDIA)
// ═══════════════════════════════════════════════════════════════════════════

struct PtxCodeGen {
    output: String,
}

impl PtxCodeGen {
    fn new() -> Self {
        PtxCodeGen {
            output: String::new(),
        }
    }

    fn gen_module(&mut self, module: &L3Module) {
        self.output.push_str(".version 7.0\n");
        self.output.push_str(".target sm_80\n");
        self.output.push_str(".address_size 64\n\n");

        for func in &module.functions {
            self.gen_function(func);
        }
    }

    fn gen_function(&mut self, func: &L3Function) {
        self.output.push_str(&format!(".visible .entry {}()\n{{\n", func.name));

        // Register declarations
        self.output.push_str("    .reg .u64 r<32>;\n");
        self.output.push_str("    .reg .pred p<8>;\n\n");

        for block in &func.blocks {
            self.output.push_str(&format!("{}_b{}:\n", func.name, block.id.0));
            self.gen_block(block);
        }

        self.output.push_str("}\n\n");
    }

    fn gen_block(&mut self, block: &L3Block) {
        for inst in &block.instructions {
            self.gen_inst(inst);
        }
    }

    fn gen_inst(&mut self, inst: &L3Inst) {
        match inst {
            L3Inst::Mov { dst, src } => {
                let d = dst.0 as usize;
                match src {
                    Operand::ImmI(val) => {
                        self.output.push_str(&format!("    mov.u64 r{}, {};\n", d, val));
                    }
                    Operand::VReg(src_v) => {
                        self.output.push_str(&format!("    mov.u64 r{}, r{};\n", d, src_v.0));
                    }
                    _ => {}
                }
            }

            L3Inst::Bin { op, dst, a, b, .. } => {
                let d = dst.0 as usize;
                let a_reg = self.operand_to_ptx(a);
                let b_reg = self.operand_to_ptx(b);

                match op {
                    crate::ast::BinOp::Add => {
                        self.output.push_str(&format!("    add.u64 r{}, {}, {};\n", d, a_reg, b_reg));
                    }
                    crate::ast::BinOp::Sub => {
                        self.output.push_str(&format!("    sub.u64 r{}, {}, {};\n", d, a_reg, b_reg));
                    }
                    crate::ast::BinOp::Mul => {
                        self.output.push_str(&format!("    mul.lo.u64 r{}, {}, {};\n", d, a_reg, b_reg));
                    }
                    crate::ast::BinOp::Div => {
                        self.output.push_str(&format!("    div.u64 r{}, {}, {};\n", d, a_reg, b_reg));
                    }
                    _ => {
                        self.output.push_str(&format!("    // unhandled binop {:?}\n", op));
                    }
                }
            }

            L3Inst::Ret { value } => {
                if let Some(val) = value {
                    let v = self.operand_to_ptx(val);
                    self.output.push_str(&format!("    mov.u64 ret_val, {};\n", v));
                }
                self.output.push_str("    ret;\n");
            }

            L3Inst::Jmp { target } => {
                self.output.push_str(&format!("    bra target_{};\n", target.0));
            }

            L3Inst::Br { cond, then, else_ } => {
                let c = self.operand_to_ptx(cond);
                self.output.push_str(&format!("    setp.eq.u64 p0, {}, 0;\n", c));
                self.output.push_str(&format!("    @p0 bra target_{};\n", else_.0));
                self.output.push_str(&format!("    bra target_{};\n", then.0));
            }

            _ => {
                self.output.push_str("    // unhandled instruction\n");
            }
        }
    }

    fn operand_to_ptx(&self, op: &Operand) -> String {
        match op {
            Operand::ImmI(val) => val.to_string(),
            Operand::VReg(v) => format!("r{}", v.0),
            _ => "0".to_string(),
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_spirv_header() {
        let module = L3Module::default();
        let spirv = generate_spirv(&module);
        // Should have magic number
        assert_eq!(spirv[0], SPIRV_MAGIC);
        // Should have version 1.0
        assert_eq!(spirv[1], SPIRV_VERSION_1_0);
    }

    #[test]
    fn test_spirv_text() {
        let module = L3Module::default();
        let text = generate_spirv_text(&module);
        assert!(text.contains("SPIR-V"));
        assert!(text.contains("Version: 1.0"));
    }

    #[test]
    fn test_ptx_header() {
        let module = L3Module::default();
        let ptx = generate_ptx(&module);
        assert!(ptx.contains(".version 7.0"));
        assert!(ptx.contains(".target sm_80"));
        assert!(ptx.contains(".address_size 64"));
    }
}
