// C code generation from Pledge IR
//
// Strategy: translate IR functions to C functions, with a runtime library
// implementing p_vec_*, p_map_*, p_string_* in C. This produces portable
// native binaries via gcc/clang.

use crate::ir::*;
use crate::ast::{BinOp, UnaryOp};
use std::collections::HashMap;
use std::fmt::Write;

/// Macro for formatted writeln to self.out
macro_rules! wln {
    ($s:expr, $fmt:expr) => {{
        $s.out.push_str($fmt);
        $s.out.push('\n');
    }};
    ($s:expr, $fmt:expr, $($arg:expr),*) => {{
        $s.out.push_str(&format!($fmt, $($arg),*));
        $s.out.push('\n');
    }};
}

pub struct CCodeGen {
    out: String,
    /// Map from ValueId to C variable name
    value_names: HashMap<u32, String>,
    /// String constants collected from the module
    string_consts: HashMap<String, String>,
    /// Counter for unique labels
    label_counter: u32,
    /// Counter for temp variables
    temp_counter: u32,
}

impl CCodeGen {
    fn new() -> Self {
        CCodeGen {
            out: String::new(),
            value_names: HashMap::new(),
            string_consts: HashMap::new(),
            label_counter: 0,
            temp_counter: 0,
        }
    }

    fn c_type(ty: &IrType) -> String {
        match ty {
            IrType::I8 => "int8_t".to_string(),
            IrType::I16 => "int16_t".to_string(),
            IrType::I32 => "int32_t".to_string(),
            IrType::I64 => "int64_t".to_string(),
            IrType::U8 => "uint8_t".to_string(),
            IrType::U16 => "uint16_t".to_string(),
            IrType::U32 => "uint32_t".to_string(),
            IrType::U64 => "uint64_t".to_string(),
            IrType::F32 => "float".to_string(),
            IrType::F64 => "double".to_string(),
            IrType::Bool => "bool".to_string(),
            IrType::Ptr(_) => "void*".to_string(),
            IrType::Struct(name) => format!("struct {}_s*", name),
            IrType::Array(elem, size) => format!("{}[{}]", Self::c_type(elem), size),
            IrType::Void => "void".to_string(),
            IrType::Never => "void".to_string(),
            IrType::Opaque(name) => match name.as_str() {
                "String" => "const char*".to_string(),
                "Vec" | "HashMap" => "void*".to_string(),
                _ => "void*".to_string(),
            },
            IrType::Generic(name, _) => match name.as_str() {
                "String" => "const char*".to_string(),
                "Vec" | "HashMap" => "void*".to_string(),
                _ => "void*".to_string(),
            },
        }
    }

    fn val_name(&self, id: ValueId) -> String {
        self.value_names
            .get(&id.0)
            .cloned()
            .unwrap_or_else(|| format!("v{}", id.0))
    }

    fn fresh_label(&mut self) -> String {
        let l = format!("L{}", self.label_counter);
        self.label_counter += 1;
        l
    }

    fn fresh_temp(&mut self) -> String {
        let t = format!("t{}", self.temp_counter);
        self.temp_counter += 1;
        t
    }

    pub fn generate(&mut self, module: &IrModule) -> String {
        // Emit runtime header
        self.emit_runtime_header();

        // Emit string constants
        self.emit_string_consts(module);

        // Emit struct definitions
        self.emit_struct_defs(module);

        // Emit function prototypes
        self.emit_prototypes(module);

        // Emit each function
        for (name, func) in &module.functions {
            self.gen_function(name, func);
        }

        // Emit main wrapper
        self.emit_main_wrapper(module);

        // Emit runtime implementation
        self.emit_runtime_impl();

        self.out.clone()
    }

    fn emit_runtime_header(&mut self) {
        wln!(self, "// Auto-generated C code from Pledge IR");
        wln!(self, "#include <stdio.h>");
        wln!(self, "#include <stdlib.h>");
        wln!(self, "#include <string.h>");
        wln!(self, "#include <stdint.h>");
        wln!(self, "#include <stdbool.h>");
        wln!(self, "");
        // Undefine MSVC macros that conflict with function names
        wln!(self, "#ifdef max");
        wln!(self, "#undef max");
        wln!(self, "#endif");
        wln!(self, "#ifdef min");
        wln!(self, "#undef min");
        wln!(self, "#endif");
        wln!(self, "#ifdef abs");
        wln!(self, "#undef abs");
        wln!(self, "#endif");
        wln!(self, "");
        wln!(self, "// ─── Runtime types ───");
        wln!(self, "typedef struct { void** data; int64_t len; int64_t cap; } PVec;");
        wln!(self, "typedef struct { char** keys; int64_t* vals; int64_t len; int64_t cap; } PMap;");
        wln!(self, "");
        wln!(self, "// ─── Runtime function declarations ───");
        wln!(self, "PVec* p_vec_new(void);");
        wln!(self, "void p_vec_push(PVec* v, void* val);");
        wln!(self, "void* p_vec_get(PVec* v, int64_t idx);");
        wln!(self, "int64_t p_vec_len(PVec* v);");
        wln!(self, "int64_t p_vec_pop(PVec* v);");
        wln!(self, "bool p_vec_is_empty(PVec* v);");
        wln!(self, "void p_vec_clear(PVec* v);");
        wln!(self, "PMap* p_map_new(void);");
        wln!(self, "void p_map_insert(PMap* m, const char* key, int64_t val);");
        wln!(self, "int64_t p_map_get(PMap* m, const char* key);");
        wln!(self, "bool p_map_contains(PMap* m, const char* key);");
        wln!(self, "void p_map_remove(PMap* m, const char* key);");
        wln!(self, "int64_t p_map_len(PMap* m);");
        wln!(self, "bool p_map_is_empty(PMap* m);");
        wln!(self, "void p_map_clear(PMap* m);");
        wln!(self, "PVec* p_map_keys(PMap* m);");
        wln!(self, "PVec* p_map_values(PMap* m);");
        wln!(self, "int64_t p_string_len(const char* s);");
        wln!(self, "bool p_string_is_empty(const char* s);");
        wln!(self, "char* p_string_push_str(const char* s, const char* other);");
        wln!(self, "char* p_string_from(const char* s);");
        wln!(self, "char* p_string_slice(const char* s, int64_t start, int64_t end);");
        wln!(self, "char* p_to_str(int64_t val);");
        wln!(self, "char* p_str_concat(const char* a, const char* b);");
        wln!(self, "void p_print(int64_t val);");
        wln!(self, "void p_print_str(const char* s);");
        wln!(self, "void p_print_int(int64_t val);");
        wln!(self, "void p_print_bool(bool val);");
        wln!(self, "void p_print_float(double val);");
        wln!(self, "void p_print_newline(void);");
        wln!(self, "void p_panic(const char* msg);");
        wln!(self, "void* p_malloc(int64_t size);");
        wln!(self, "void p_free(void* ptr);");
        wln!(self, "int64_t print(int64_t val);");
        wln!(self, "int64_t print_str(const char* s);");
        wln!(self, "");
    }

    fn emit_string_consts(&mut self, module: &IrModule) {
        wln!(self, "// ─── String constants ───");
        let mut idx = 0;
        for func in module.functions.values() {
            for block in &func.blocks {
                for &inst_id in &block.instructions {
                    if let Some(Inst::ConstStr { value }) = func.instructions.get(&inst_id) {
                        if !self.string_consts.contains_key(value) {
                            let name = format!("_str{}", idx);
                            idx += 1;
                            let escaped = value.replace('\\', "\\\\").replace('"', "\\\"").replace('\n', "\\n");
                            wln!(self, "static const char {}[] = \"{}\";", name, escaped);
                            self.string_consts.insert(value.clone(), name);
                        }
                    }
                }
            }
        }
        wln!(self, "");
    }

    fn emit_struct_defs(&mut self, module: &IrModule) {
        wln!(self, "// ─── Struct definitions ───");
        for (name, layout) in &module.struct_layouts {
            wln!(self, "struct {}_s {{", name);
            for (i, (_fname, field_ty, _offset)) in layout.fields.iter().enumerate() {
                wln!(self, "    {} field_{};", Self::c_type(field_ty), i);
            }
            wln!(self, "};");
            wln!(self, "");
        }
    }

    fn emit_prototypes(&mut self, module: &IrModule) {
        wln!(self, "// ─── Function prototypes ───");
        for (name, func) in &module.functions {
            let proto_name = match name.as_str() {
                "main" => "main_entry",
                "abs" => "p_abs",
                "max" => "p_max",
                "min" => "p_min",
                other => other,
            };
            let ret_ty = if name == "main" { "int64_t".to_string() } else { Self::c_type(&func.ret_ty) };
            let params: Vec<String> = func
                .params
                .iter()
                .map(|(id, ty)| format!("{} v{}", Self::c_type(ty), id.0))
                .collect();
            wln!(self, "{} {}({});", ret_ty, proto_name, params.join(", "));
        }
        wln!(self, "");
    }

    fn emit_main_wrapper(&mut self, module: &IrModule) {
        if module.functions.contains_key("main") {
            wln!(self, "// ─── C entry point ───");
            wln!(self, "int main(int argc, char** argv) {");
            wln!(self, "    int64_t result = main_entry(NULL);");
            wln!(self, "    return (int)result;");
            wln!(self, "}");
            wln!(self, "");
        }
    }

    fn gen_function(&mut self, name: &str, func: &IrFunction) {
        self.value_names.clear();
        self.temp_counter = 0;
        self.label_counter = 0;

        // Rename functions that conflict with C/MSVC intrinsics
        let c_name = match name {
            "main" => "main_entry",
            "abs" => "p_abs",
            "max" => "p_max",
            "min" => "p_min",
            other => other,
        };

        // Register params
        for (id, _) in &func.params {
            self.value_names.insert(id.0, format!("v{}", id.0));
        }

        let ret_ty = if name == "main" {
            "int64_t".to_string()
        } else {
            Self::c_type(&func.ret_ty)
        };

        let func_name = c_name;

        let params: Vec<String> = func
            .params
            .iter()
            .map(|(id, ty)| format!("{} v{}", Self::c_type(ty), id.0))
            .collect();

        wln!(self, "{} {}({}) {{", ret_ty, func_name, params.join(", "));

        // Declare all SSA values as local variables (skip void/never and params)
        for (id, meta) in &func.values {
            if !func.params.iter().any(|(pid, _)| pid == id) {
                match meta.ty {
                    IrType::Void | IrType::Never => {
                        // Skip void variables — use placeholder
                        wln!(self, "    int64_t v{}; /* placeholder for void */", id.0);
                    }
                    _ => {
                        let ty = Self::c_type(&meta.ty);
                        wln!(self, "    {} v{};", ty, id.0);
                    }
                }
                self.value_names.insert(id.0, format!("v{}", id.0));
            }
        }

        // Generate blocks
        for (i, block) in func.blocks.iter().enumerate() {
            if i > 0 {
                wln!(self, "  bb{}:", block.id.0);
            }
            for &inst_id in &block.instructions {
                if let Some(inst) = func.instructions.get(&inst_id) {
                    self.gen_inst(inst, inst_id, func);
                }
            }
        }

        // Default return for functions that fall through without explicit return
        match &func.ret_ty {
            IrType::Void | IrType::Never => {
                wln!(self, "    return;");
            }
            _ => {
                wln!(self, "    return 0;");
            }
        }
        wln!(self, "}");
        wln!(self, "");
    }

    fn gen_inst(&mut self, inst: &Inst, id: ValueId, func: &IrFunction) {
        let dst = self.val_name(id);
        match inst {
            Inst::ConstInt { value, ty: _ } => {
                wln!(self, "    {} = {}LL;", dst, value);
            }
            Inst::ConstFloat { bits, ty: _ } => {
                wln!(self, "    {} = {}LL;", dst, bits);
            }
            Inst::ConstBool { value } => {
                wln!(self, "    {} = {};", dst, if *value { "true" } else { "false" });
            }
            Inst::ConstStr { value } => {
                let const_name = self.string_consts.get(value).cloned().unwrap_or_else(|| {
                    // Fallback: inline string
                    let escaped = value.replace('\\', "\\\\").replace('"', "\\\"").replace('\n', "\\n");
                    format!("\"{}\"", escaped)
                });
                wln!(self, "    {} = {};", dst, const_name);
            }
            Inst::ConstNull { .. } => {
                wln!(self, "    {} = NULL;", dst);
            }
            Inst::BinOp { op, left, right, ty: _ } => {
                let l = self.val_name(*left);
                let r = self.val_name(*right);
                let cop = match op {
                    BinOp::Add => "+",
                    BinOp::Sub => "-",
                    BinOp::Mul => "*",
                    BinOp::Div => "/",
                    BinOp::Mod => "%",
                    BinOp::BitAnd => "&",
                    BinOp::BitOr => "|",
                    BinOp::BitXor => "^",
                    BinOp::Shl => "<<",
                    BinOp::Shr => ">>",
                    BinOp::Eq => "==",
                    BinOp::Neq => "!=",
                    BinOp::Lt => "<",
                    BinOp::LtEq => "<=",
                    BinOp::Gt => ">",
                    BinOp::GtEq => ">=",
                    BinOp::And => "&&",
                    BinOp::Or => "||",
                };
                wln!(self, "    {} = {} {} {};", dst, l, cop, r);
            }
            Inst::UnaryOp { op, operand, ty: _ } => {
                let o = self.val_name(*operand);
                let uop = match op {
                    UnaryOp::Neg => "-",
                    UnaryOp::Not => "!",
                    UnaryOp::BitNot => "~",
                };
                wln!(self, "    {} = {}{};", dst, uop, o);
            }
            Inst::Alloca { ty, .. } => {
                let size = ty.size();
                wln!(self, "    {} = malloc({});", dst, size);
            }
            Inst::Load { ptr, ty } => {
                let p = self.val_name(*ptr);
                match ty {
                    IrType::Opaque(_) | IrType::Generic(_, _) => {
                        wln!(self, "    {} = *(void**){};", dst, p);
                    }
                    _ => {
                        let ct = Self::c_type(ty);
                        wln!(self, "    {} = *({}*){};", dst, ct, p);
                    }
                }
            }
            Inst::Store { ptr, value } => {
                let p = self.val_name(*ptr);
                let v = self.val_name(*value);
                wln!(self, "    *(void**){} = (void*){};", p, v);
            }
            Inst::GetFieldPtr { struct_ptr, field_index, .. } => {
                let s = self.val_name(*struct_ptr);
                wln!(self, "    {} = (char*){} + {};", dst, s, field_index * 8);
            }
            Inst::GetElemPtr { array_ptr, index, elem_ty, .. } => {
                let a = self.val_name(*array_ptr);
                let i = self.val_name(*index);
                let size = elem_ty.size();
                wln!(self, "    {} = (char*){} + ({} * {});", dst, a, i, size);
            }
            Inst::Jump { target } => {
                wln!(self, "    goto bb{};", target.0);
            }
            Inst::Branch { cond, then, else_ } => {
                let c = self.val_name(*cond);
                wln!(self, "    if ({}) goto bb{};", c, then.0);
                wln!(self, "    else goto bb{};", else_.0);
            }
            Inst::Return { value } => {
                match value {
                    Some(v) => {
                        let vn = self.val_name(*v);
                        wln!(self, "    return {};", vn);
                    }
                    None => {
                        wln!(self, "    return;");
                    }
                }
            }
            Inst::Unreachable => {
                wln!(self, "    abort();");
            }
            Inst::Call { func: fname, args, ret_ty, .. } => {
                // Special-case: print is polymorphic — dispatch based on arg type
                if fname == "print" && !args.is_empty() {
                    let arg_val = args[0];
                    let arg_ty = func.values.get(&arg_val).map(|m| &m.ty);
                    let arg_str = self.val_name(arg_val);
                    match arg_ty {
                        Some(IrType::Ptr(_)) | Some(IrType::Opaque(_)) | Some(IrType::Generic(_, _)) => {
                            wln!(self, "    p_print_str({});", arg_str);
                        }
                        Some(IrType::F64) => {
                            wln!(self, "    p_print_float({});", arg_str);
                        }
                        Some(IrType::Bool) => {
                            wln!(self, "    p_print_bool({});", arg_str);
                        }
                        _ => {
                            wln!(self, "    p_print_int((int64_t){});", arg_str);
                        }
                    }
                    wln!(self, "    p_print_newline();");
                    wln!(self, "    {} = 0;", dst);
                    return;
                }
                let c_fname = match fname.as_str() {
                    "abs" => "p_abs",
                    "max" => "p_max",
                    "min" => "p_min",
                    other => other,
                };
                let arg_strs: Vec<String> = args.iter().map(|a| self.val_name(*a)).collect();
                let args_joined = arg_strs.join(", ");
                match ret_ty {
                    IrType::Void | IrType::Never => {
                        wln!(self, "    {}({});", c_fname, args_joined);
                    }
                    _ => {
                        wln!(self, "    {} = {}({});", dst, c_fname, args_joined);
                    }
                }
            }
            Inst::CallPtr { fn_ptr, args, ret_ty, .. } => {
                let fp = self.val_name(*fn_ptr);
                let arg_strs: Vec<String> = args.iter().map(|a| self.val_name(*a)).collect();
                let args_joined = arg_strs.join(", ");
                match ret_ty {
                    IrType::Void | IrType::Never => {
                        wln!(self, "    ((void(*)(void*)){})({});", fp, args_joined);
                    }
                    _ => {
                        wln!(self, "    {} = ((int64_t(*)(void*)){})({});", dst, fp, args_joined);
                    }
                }
            }
            Inst::BitCast { value, from: _, to: _ } => {
                let v = self.val_name(*value);
                wln!(self, "    {} = (void*){};", dst, v);
            }
            Inst::IntExtend { value, from: _, to: _ } => {
                let v = self.val_name(*value);
                wln!(self, "    {} = (int64_t){};", dst, v);
            }
            Inst::IntTrunc { value, from: _, to: _ } => {
                let v = self.val_name(*value);
                wln!(self, "    {} = (int32_t){};", dst, v);
            }
            Inst::IntToFloat { value, from: _, to: _ } => {
                let v = self.val_name(*value);
                wln!(self, "    {} = (double){};", dst, v);
            }
            Inst::FloatToInt { value, from: _, to: _ } => {
                let v = self.val_name(*value);
                wln!(self, "    {} = (int64_t){};", dst, v);
            }
            Inst::TryUnwrap { .. } => {
                wln!(self, "    // TryUnwrap not implemented in C backend");
            }
            Inst::Print { value, .. } => {
                let v = self.val_name(*value);
                wln!(self, "    printf(\"%lld\\n\", (long long){});", v);
            }
            Inst::Panic { message, .. } => {
                wln!(self, "    fprintf(stderr, \"Panic: {}\\n\"); abort();", message);
            }
        }
    }

    fn emit_runtime_impl(&mut self) {
        wln!(self, "");
        wln!(self, "// ─── Runtime implementation ───");
        wln!(self, "");
        // Vec runtime
        wln!(self, "PVec* p_vec_new(void) {{");
        wln!(self, "    PVec* v = (PVec*)malloc(sizeof(PVec));");
        wln!(self, "    v->data = NULL; v->len = 0; v->cap = 0;");
        wln!(self, "    return v;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "void p_vec_push(PVec* v, void* val) {{");
        wln!(self, "    if (v->len >= v->cap) {{");
        wln!(self, "        v->cap = v->cap == 0 ? 8 : v->cap * 2;");
        wln!(self, "        v->data = (void**)realloc(v->data, v->cap * sizeof(void*));");
        wln!(self, "    }}");
        wln!(self, "    v->data[v->len++] = val;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "void* p_vec_get(PVec* v, int64_t idx) {{");
        wln!(self, "    if (idx < 0 || idx >= v->len) return NULL;");
        wln!(self, "    return v->data[idx];");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "int64_t p_vec_len(PVec* v) {{ return v->len; }}");
        wln!(self, "bool p_vec_is_empty(PVec* v) {{ return v->len == 0; }}");
        wln!(self, "void p_vec_clear(PVec* v) {{ v->len = 0; }}");
        wln!(self, "int64_t p_vec_pop(PVec* v) {{");
        wln!(self, "    if (v->len == 0) return 0;");
        wln!(self, "    v->len--;");
        wln!(self, "    return (int64_t)v->data[v->len];");
        wln!(self, "}}");
        wln!(self, "");
        // Map runtime (simple linear probe)
        wln!(self, "PMap* p_map_new(void) {{");
        wln!(self, "    PMap* m = (PMap*)malloc(sizeof(PMap));");
        wln!(self, "    m->keys = NULL; m->vals = NULL; m->len = 0; m->cap = 0;");
        wln!(self, "    return m;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "void p_map_insert(PMap* m, const char* key, int64_t val) {{");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) {{");
        wln!(self, "        if (strcmp(m->keys[i], key) == 0) {{ m->vals[i] = val; return; }}");
        wln!(self, "    }}");
        wln!(self, "    if (m->len >= m->cap) {{");
        wln!(self, "        m->cap = m->cap == 0 ? 8 : m->cap * 2;");
        wln!(self, "        m->keys = (char**)realloc(m->keys, m->cap * sizeof(char*));");
        wln!(self, "        m->vals = (int64_t*)realloc(m->vals, m->cap * sizeof(int64_t));");
        wln!(self, "    }}");
        wln!(self, "    m->keys[m->len] = strdup(key);");
        wln!(self, "    m->vals[m->len] = val;");
        wln!(self, "    m->len++;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "int64_t p_map_get(PMap* m, const char* key) {{");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) {{");
        wln!(self, "        if (strcmp(m->keys[i], key) == 0) return m->vals[i];");
        wln!(self, "    }}");
        wln!(self, "    return 0;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "bool p_map_contains(PMap* m, const char* key) {{");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) {{");
        wln!(self, "        if (strcmp(m->keys[i], key) == 0) return true;");
        wln!(self, "    }}");
        wln!(self, "    return false;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "void p_map_remove(PMap* m, const char* key) {{");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) {{");
        wln!(self, "        if (strcmp(m->keys[i], key) == 0) {{");
        wln!(self, "            free(m->keys[i]);");
        wln!(self, "            m->keys[i] = m->keys[m->len-1];");
        wln!(self, "            m->vals[i] = m->vals[m->len-1];");
        wln!(self, "            m->len--;");
        wln!(self, "            return;");
        wln!(self, "        }}");
        wln!(self, "    }}");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "int64_t p_map_len(PMap* m) {{ return m->len; }}");
        wln!(self, "bool p_map_is_empty(PMap* m) {{ return m->len == 0; }}");
        wln!(self, "void p_map_clear(PMap* m) {{ m->len = 0; }}");
        wln!(self, "");
        wln!(self, "PVec* p_map_keys(PMap* m) {{");
        wln!(self, "    PVec* v = p_vec_new();");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++)");
        wln!(self, "        p_vec_push(v, (void*)m->keys[i]);");
        wln!(self, "    return v;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "PVec* p_map_values(PMap* m) {{");
        wln!(self, "    PVec* v = p_vec_new();");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) {{");
        wln!(self, "    int64_t* p = (int64_t*)malloc(sizeof(int64_t));");
        wln!(self, "    *p = m->vals[i];");
        wln!(self, "    p_vec_push(v, p);");
        wln!(self, "    }}");
        wln!(self, "    return v;");
        wln!(self, "}}");
        wln!(self, "");
        // String runtime
        wln!(self, "int64_t p_string_len(const char* s) {{ return (int64_t)strlen(s); }}");
        wln!(self, "bool p_string_is_empty(const char* s) {{ return s[0] == 0; }}");
        wln!(self, "");
        wln!(self, "char* p_string_push_str(const char* s, const char* other) {{");
        wln!(self, "    size_t len = strlen(s) + strlen(other);");
        wln!(self, "    char* result = (char*)malloc(len + 1);");
        wln!(self, "    strcpy(result, s);");
        wln!(self, "    strcat(result, other);");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "char* p_string_from(const char* s) {{ return strdup(s); }}");
        wln!(self, "");
        wln!(self, "char* p_string_slice(const char* s, int64_t start, int64_t end) {{");
        wln!(self, "    int64_t len = (int64_t)strlen(s);");
        wln!(self, "    if (start < 0) start = 0;");
        wln!(self, "    if (end > len) end = len;");
        wln!(self, "    if (start > end) start = end;");
        wln!(self, "    int64_t slen = end - start;");
        wln!(self, "    char* result = (char*)malloc(slen + 1);");
        wln!(self, "    memcpy(result, s + start, slen);");
        wln!(self, "    result[slen] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "char* p_to_str(int64_t val) {{");
        wln!(self, "    char* buf = (char*)malloc(32);");
        wln!(self, "    snprintf(buf, 32, \"%lld\", (long long)val);");
        wln!(self, "    return buf;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "char* p_str_concat(const char* a, const char* b) {{");
        wln!(self, "    size_t len = strlen(a) + strlen(b);");
        wln!(self, "    char* result = (char*)malloc(len + 1);");
        wln!(self, "    strcpy(result, a);");
        wln!(self, "    strcat(result, b);");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "void p_print(int64_t val) { printf(\"%lld\\n\", (long long)val); }");
        wln!(self, "void p_print_str(const char* s) { printf(\"%s\", s); }");
        wln!(self, "void p_print_int(int64_t val) { printf(\"%lld\", (long long)val); }");
        wln!(self, "void p_print_bool(bool val) { printf(\"%s\", val ? \"true\" : \"false\"); }");
        wln!(self, "void p_print_float(double val) { printf(\"%g\", val); }");
        wln!(self, "void p_print_newline(void) { printf(\"\\n\"); }");
        wln!(self, "void p_panic(const char* msg) { fprintf(stderr, \"%s\\n\", msg); abort(); }");
        wln!(self, "void* p_malloc(int64_t size) { return malloc((size_t)size); }");
        wln!(self, "void p_free(void* ptr) { free(ptr); }");
        wln!(self, "int64_t print(int64_t val) { printf(\"%lld\\n\", (long long)val); return 0; }");
        wln!(self, "int64_t print_str(const char* s) { printf(\"%s\\n\", s); return 0; }");
    }
}

pub fn generate_c(module: &IrModule) -> String {
    let mut gen = CCodeGen::new();
    gen.generate(module)
}
