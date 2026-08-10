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
    /// Whether the current function's C return type is void (true) or
    /// non-void (false).  Set at the start of `gen_function`.
    cur_fn_c_ret_is_void: bool,
    /// The current function's C return type string (e.g. "int64_t", "void",
    /// "bool").  Used to cast return values when the IR value type doesn't
    /// match the C return type (e.g. returning a pointer from an int64_t
    /// function).
    cur_fn_c_ret_type: String,
    /// Function parameter types, keyed by IR function name.
    /// Used at call sites to cast arguments to the correct C type
    /// (e.g. integer 0 → `(void*)0` for pointer parameters).
    fn_param_types: HashMap<String, Vec<IrType>>,
    /// Struct layouts from the module — needed because `IrType::Struct(name).size()`
    /// returns 0 (placeholder), and `GetFieldPtr` needs real field offsets
    /// (fields are not always 8 bytes).
    struct_layouts: HashMap<String, StructLayout>,
    /// Set of user-defined function names (from the module).  Used to
    /// distinguish user functions from runtime functions in CallPtr dispatch.
    user_functions: std::collections::HashSet<String>,
    /// Function return types, keyed by IR function name.  Used for CallPtr
    /// dispatch to filter candidates by return type.
    fn_ret_types: HashMap<String, IrType>,
}

impl CCodeGen {
    fn new() -> Self {
        CCodeGen {
            out: String::new(),
            value_names: HashMap::new(),
            string_consts: HashMap::new(),
            label_counter: 0,
            temp_counter: 0,
            cur_fn_c_ret_is_void: true,
            cur_fn_c_ret_type: "void".to_string(),
            fn_param_types: HashMap::new(),
            struct_layouts: HashMap::new(),
            user_functions: std::collections::HashSet::new(),
            fn_ret_types: HashMap::new(),
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

    /// Returns true if the type uses the out-parameter calling convention
    /// (struct/array returns are lowered to a hidden first parameter pointer
    /// and a void return). The C declaration must use `void` as the return
    /// type for these, matching the IR's `Return { value: None }` terminators.
    fn is_aggregate_ret(ty: &IrType) -> bool {
        matches!(ty, IrType::Struct(_) | IrType::Array(_, _))
    }

    /// C return type for a function — aggregate types are `void` because the
    /// result is returned via a hidden out-parameter (first function argument).
    fn c_ret_type(ty: &IrType) -> String {
        if Self::is_aggregate_ret(ty) {
            "void".to_string()
        } else {
            Self::c_type(ty)
        }
    }

    /// Cast a call argument from its IR type to the called function's
    /// parameter type.  The main case is integer → pointer (the hidden
    /// context pointer is `ConstInt 0` typed as I64 but the parameter is
    /// `*u8`/`void*`).
    fn cast_arg(val: &str, from: &IrType, to: &IrType) -> String {
        let from_is_ptr = matches!(from, IrType::Ptr(_) | IrType::Opaque(_) | IrType::Generic(_, _));
        let to_is_ptr = matches!(to, IrType::Ptr(_) | IrType::Opaque(_) | IrType::Generic(_, _));
        let to_is_struct = Self::is_aggregate_ret(to);

        match (from_is_ptr, to_is_ptr, to_is_struct) {
            // int → pointer: cast to void*
            (false, true, _) => format!("(void*){}", val),
            // pointer → int: cast to int64_t
            (true, false, _) => format!("(int64_t){}", val),
            // pointer → struct pointer (out-param): cast to void*
            (false, false, true) => format!("(void*){}", val),
            _ => val.to_string(),
        }
    }

    /// Sanitize a Pledge function name for use as a C identifier.
    /// Renames functions that collide with C keywords or C runtime names.
    fn sanitize_c_name(name: &str) -> &str {
        match name {
            "main" => "main_entry",
            "abs" => "p_abs",
            "max" => "p_max",
            "min" => "p_min",
            // C keywords that can't be function names
            "double" => "p_double",
            "float" => "p_float",
            "int" => "p_int",
            "char" => "p_char",
            "long" => "p_long",
            "short" => "p_short",
            "unsigned" => "p_unsigned",
            "signed" => "p_signed",
            "void" => "p_void",
            "bool" => "p_bool",
            "auto" => "p_auto",
            "register" => "p_register",
            "static" => "p_static",
            "extern" => "p_extern",
            "const" => "p_const",
            "volatile" => "p_volatile",
            "struct" => "p_struct",
            "union" => "p_union",
            "enum" => "p_enum",
            "typedef" => "p_typedef",
            "sizeof" => "p_sizeof",
            "return" => "p_return",
            "break" => "p_break",
            "continue" => "p_continue",
            "for" => "p_for",
            "while" => "p_while",
            "do" => "p_do",
            "if" => "p_if",
            "else" => "p_else",
            "switch" => "p_switch",
            "case" => "p_case",
            "default" => "p_default",
            "goto" => "p_goto",
            "inline" => "p_inline",
            "restrict" => "p_restrict",
            other => other,
        }
    }

    /// Escape a string for use as a C string literal.
    /// Handles all special characters: backslash, double quote, newline,
    /// tab, carriage return, null, and other control characters.
    fn escape_c_string(s: &str) -> String {
        let mut out = String::with_capacity(s.len() + 4);
        for c in s.chars() {
            match c {
                '\\' => out.push_str("\\\\"),
                '"' => out.push_str("\\\""),
                '\n' => out.push_str("\\n"),
                '\t' => out.push_str("\\t"),
                '\r' => out.push_str("\\r"),
                '\0' => out.push_str("\\0"),
                c if (c as u32) < 0x20 => {
                    out.push_str(&format!("\\x{:02x}", c as u32));
                }
                c => out.push(c),
            }
        }
        out
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
        // Collect function parameter types for call-site argument casting
        self.fn_param_types.clear();
        for (name, func) in &module.functions {
            self.fn_param_types.insert(
                name.clone(),
                func.params.iter().map(|(_, ty)| ty.clone()).collect(),
            );
        }
        // Collect struct layouts from module
        self.struct_layouts.clear();
        for (name, layout) in &module.struct_layouts {
            self.struct_layouts.insert(name.clone(), layout.clone());
        }
        // Collect user-defined function names (for CallPtr dispatch)
        self.user_functions.clear();
        self.fn_ret_types.clear();
        for (name, func) in &module.functions {
            self.user_functions.insert(name.clone());
            self.fn_ret_types.insert(name.clone(), func.ret_ty.clone());
        }
        // Runtime functions (not in module.functions) — needed so that
        // call-site argument casting knows their parameter types.
        self.insert_runtime_sigs();

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

    /// Insert parameter type signatures for runtime functions so that
    /// call-site argument casting can handle int→pointer conversions
    /// (e.g. `p_vec_push(vec, 42)` — the `42` is I64 but the parameter is
    /// `void*`).
    fn insert_runtime_sigs(&mut self) {
        use IrType::*;
        let p = || Ptr(Box::new(U8));
        let sigs: &[(&str, &[IrType])] = &[
            ("p_vec_new", &[]),
            ("p_vec_push", &[p(), p()]),
            ("p_vec_get", &[p(), I64]),
            ("p_vec_len", &[p()]),
            ("p_vec_pop", &[p()]),
            ("p_vec_is_empty", &[p()]),
            ("p_vec_clear", &[p()]),
            ("p_vec_clone", &[p()]),
            ("p_vec_contains", &[p(), p()]),
            ("p_vec_extend", &[p(), p()]),
            ("p_vec_first", &[p()]),
            ("p_vec_last", &[p()]),
            ("p_vec_insert", &[p(), I64, p()]),
            ("p_vec_remove", &[p(), I64]),
            ("p_vec_reverse", &[p()]),
            ("p_vec_sort", &[p()]),
            ("p_vec_truncate", &[p(), I64]),
            ("p_map_new", &[]),
            ("p_map_insert", &[p(), p(), I64]),
            ("p_map_get", &[p(), p()]),
            ("p_map_get_or", &[p(), p(), I64]),
            ("p_map_contains", &[p(), p()]),
            ("p_map_remove", &[p(), p()]),
            ("p_map_len", &[p()]),
            ("p_map_is_empty", &[p()]),
            ("p_map_clear", &[p()]),
            ("p_map_clone", &[p()]),
            ("p_map_keys", &[p()]),
            ("p_map_values", &[p()]),
            ("p_string_len", &[p()]),
            ("p_string_is_empty", &[p()]),
            ("p_string_push_str", &[p(), p()]),
            ("p_string_from", &[p()]),
            ("p_string_slice", &[p(), I64, I64]),
            ("p_string_contains", &[p(), p()]),
            ("p_string_starts_with", &[p(), p()]),
            ("p_string_ends_with", &[p(), p()]),
            ("p_string_find", &[p(), p()]),
            ("p_string_trim", &[p()]),
            ("p_string_to_uppercase", &[p()]),
            ("p_string_to_lowercase", &[p()]),
            ("p_string_repeat", &[p(), I64]),
            ("p_string_replace", &[p(), p(), p()]),
            ("p_string_split", &[p(), p()]),
            ("p_string_parse_int", &[p()]),
            ("p_string_char_at", &[p(), I64]),
            ("p_string_substring", &[p(), I64, I64]),
            ("p_to_str", &[I64]),
            ("p_to_str_vec", &[p()]),
            ("p_to_str_str", &[p()]),
            ("p_to_str_bool", &[Bool]),
            ("p_to_str_float", &[F64]),
            ("p_str_concat", &[p(), p()]),
            ("p_print", &[I64]),
            ("p_print_str", &[p()]),
            ("p_print_int", &[I64]),
            ("p_print_bool", &[Bool]),
            ("p_print_float", &[F64]),
            ("p_print_newline", &[]),
            ("p_malloc", &[I64]),
            ("p_alloc", &[I64]),
            ("p_free", &[p()]),
            ("p_panic", &[p()]),
            ("println", &[p(), p()]),
        ];
        for (name, params) in sigs {
            self.fn_param_types
                .insert(name.to_string(), params.to_vec());
        }
        // Runtime function return types — most return int64_t or void*,
        // but some return void (mutating operations).
        let void_ret: &[&str] = &[
            "p_vec_push", "p_vec_clear", "p_vec_insert",
            "p_vec_reverse", "p_vec_sort", "p_vec_truncate", "p_vec_extend",
            "p_map_insert", "p_map_remove", "p_map_clear",
            "p_print", "p_print_str", "p_print_int", "p_print_bool",
            "p_print_float", "p_print_newline", "p_free", "p_panic",
        ];
        for name in void_ret {
            self.fn_ret_types.insert(name.to_string(), IrType::Void);
        }
    }

    fn emit_runtime_header(&mut self) {
        wln!(self, "// Auto-generated C code from Pledge IR");
        wln!(self, "#include <stdio.h>");
        wln!(self, "#include <stdlib.h>");
        wln!(self, "#include <string.h>");
        wln!(self, "#include <stdint.h>");
        wln!(self, "#include <stdbool.h>");
        wln!(self, "#include <ctype.h>");
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
        wln!(self, "bool p_string_contains(const char* s, const char* sub);");
        wln!(self, "bool p_string_starts_with(const char* s, const char* prefix);");
        wln!(self, "bool p_string_ends_with(const char* s, const char* suffix);");
        wln!(self, "int64_t p_string_find(const char* s, const char* sub);");
        wln!(self, "char* p_string_trim(const char* s);");
        wln!(self, "char* p_string_to_uppercase(const char* s);");
        wln!(self, "char* p_string_to_lowercase(const char* s);");
        wln!(self, "char* p_string_repeat(const char* s, int64_t n);");
        wln!(self, "char* p_string_replace(const char* s, const char* from, const char* to);");
        wln!(self, "PVec* p_string_split(const char* s, const char* delim);");
        wln!(self, "int64_t p_string_parse_int(const char* s);");
        wln!(self, "char* p_string_char_at(const char* s, int64_t idx);");
        wln!(self, "char* p_string_substring(const char* s, int64_t start, int64_t end);");
        wln!(self, "char* p_to_str(int64_t val);");
        wln!(self, "char* p_to_str_vec(PVec* v);");
        wln!(self, "char* p_to_str_str(const char* s);");
        wln!(self, "char* p_to_str_bool(bool val);");
        wln!(self, "char* p_to_str_float(double val);");
        wln!(self, "char* p_to_str_map(PMap* m);");
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
        wln!(self, "void p_vec_clear(PVec* v);");
        wln!(self, "PVec* p_vec_clone(PVec* v);");
        wln!(self, "bool p_vec_contains(PVec* v, void* val);");
        wln!(self, "void p_vec_extend(PVec* v, PVec* other);");
        wln!(self, "void* p_vec_first(PVec* v);");
        wln!(self, "void* p_vec_last(PVec* v);");
        wln!(self, "void p_vec_insert(PVec* v, int64_t idx, void* val);");
        wln!(self, "void* p_vec_remove(PVec* v, int64_t idx);");
        wln!(self, "void p_vec_reverse(PVec* v);");
        wln!(self, "void p_vec_sort(PVec* v);");
        wln!(self, "void p_vec_truncate(PVec* v, int64_t len);");
        wln!(self, "PMap* p_map_clone(PMap* m);");
        wln!(self, "int64_t p_map_get_or(PMap* m, const char* key, int64_t default_val);");
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
                            let escaped = Self::escape_c_string(value);
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
            let proto_name = Self::sanitize_c_name(name);
            let ret_ty = if name == "main" { "int64_t".to_string() } else { Self::c_ret_type(&func.ret_ty) };
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

        // Rename functions that conflict with C keywords or C runtime names
        let c_name = Self::sanitize_c_name(name);

        // Register params
        for (id, _) in &func.params {
            self.value_names.insert(id.0, format!("v{}", id.0));
        }

        let ret_ty = if name == "main" {
            "int64_t".to_string()
        } else {
            Self::c_ret_type(&func.ret_ty)
        };

        // Track whether the C return type is void, so that `Return { value:
        // None }` (which appears in void IR functions, struct-returning
        // functions using the out-param convention, and dead blocks) emits the
        // correct C statement.
        self.cur_fn_c_ret_is_void = ret_ty == "void";
        self.cur_fn_c_ret_type = ret_ty.clone();

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
        if self.cur_fn_c_ret_is_void {
            wln!(self, "    return;");
        } else {
            wln!(self, "    return 0;");
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
            Inst::ConstFloat { bits, ty } => {
                // Use a union compound literal to reinterpret the IEEE 754
                // bits as a float — assigning the integer directly to a
                // double variable would convert it, not preserve the bits.
                let suffix = if *ty == IrType::F32 { "U" } else { "ULL" };
                let union_ty = if *ty == IrType::F32 { "float" } else { "double" };
                wln!(self, "    {} = ((union {{ uint{}_t u; {} f; }}){{ .u = {}{} }}).f;",
                    dst, if *ty == IrType::F32 { 32 } else { 64 }, union_ty, bits, suffix);
            }
            Inst::ConstBool { value } => {
                wln!(self, "    {} = {};", dst, if *value { "true" } else { "false" });
            }
            Inst::ConstStr { value } => {
                let const_name = self.string_consts.get(value).cloned().unwrap_or_else(|| {
                    // Fallback: inline string
                    let escaped = Self::escape_c_string(value);
                    format!("\"{}\"", escaped)
                });
                wln!(self, "    {} = {};", dst, const_name);
            }
            Inst::ConstNull { .. } => {
                wln!(self, "    {} = NULL;", dst);
            }
            Inst::BinOp { op, left, right, ty: _ } => {
                let l_raw = self.val_name(*left);
                let r_raw = self.val_name(*right);
                // Cast pointer-typed operands to int64_t for arithmetic —
                // the IR treats opaque/generic values as integers, but C
                // doesn't allow arithmetic on void*.
                let l_ty = func.values.get(left).map(|m| &m.ty);
                let r_ty = func.values.get(right).map(|m| &m.ty);
                let is_ptr = |ty: Option<&IrType>| {
                    matches!(ty, Some(IrType::Ptr(_)) | Some(IrType::Opaque(_)) | Some(IrType::Generic(_, _)))
                };
                let l = if is_ptr(l_ty) { format!("(int64_t){}", l_raw) } else { l_raw };
                let r = if is_ptr(r_ty) { format!("(int64_t){}", r_raw) } else { r_raw };
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
                let size = match ty {
                    IrType::Struct(name) => {
                        self.struct_layouts.get(name).map(|l| l.size).unwrap_or(0)
                    }
                    _ => ty.size(),
                };
                // Ensure at least 1 byte to avoid malloc(0) undefined behavior
                let size = if size == 0 { 8 } else { size };
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
                // Type-aware store: use the pointer's pointee type (if
                // available) to determine the store size.  This is critical
                // for correctness — a BinOp may produce an int64_t result
                // even when the operands are int32_t, and storing 8 bytes
                // into a 4-byte field would corrupt adjacent data.
                // Falls back to the value's type if the pointer type is
                // unknown (e.g. opaque pointers).
                let ptr_ty = func.values.get(ptr).and_then(|m| match &m.ty {
                    IrType::Ptr(inner) => Some(inner.as_ref().clone()),
                    _ => None,
                });
                let val_ty = func.values.get(value).map(|m| m.ty.clone());
                // Prefer the pointer's pointee type; fall back to value type
                let store_ty = ptr_ty.or(val_ty).unwrap_or(IrType::I64);
                match &store_ty {
                    IrType::Ptr(_) | IrType::Opaque(_) | IrType::Generic(_, _) => {
                        wln!(self, "    *(void**){} = (void*){};", p, v);
                    }
                    IrType::F64 => {
                        wln!(self, "    *(double*){} = {};", p, v);
                    }
                    IrType::F32 => {
                        wln!(self, "    *(float*){} = {};", p, v);
                    }
                    IrType::Bool => {
                        wln!(self, "    *(bool*){} = {};", p, v);
                    }
                    IrType::I8 => {
                        wln!(self, "    *(int8_t*){} = (int8_t){};", p, v);
                    }
                    IrType::I16 => {
                        wln!(self, "    *(int16_t*){} = (int16_t){};", p, v);
                    }
                    IrType::I32 => {
                        wln!(self, "    *(int32_t*){} = (int32_t){};", p, v);
                    }
                    IrType::U8 => {
                        wln!(self, "    *(uint8_t*){} = (uint8_t){};", p, v);
                    }
                    IrType::U16 => {
                        wln!(self, "    *(uint16_t*){} = (uint16_t){};", p, v);
                    }
                    IrType::U32 => {
                        wln!(self, "    *(uint32_t*){} = (uint32_t){};", p, v);
                    }
                    IrType::U64 => {
                        wln!(self, "    *(uint64_t*){} = (uint64_t){};", p, v);
                    }
                    _ => {
                        // I64, Void, Never, Struct, Array — store as int64_t
                        wln!(self, "    *(int64_t*){} = (int64_t){};", p, v);
                    }
                }
            }
            Inst::GetFieldPtr { struct_ptr, field_index, .. } => {
                let s = self.val_name(*struct_ptr);
                // Look up the actual field offset from the struct layout.
                // The struct type is inferred from the pointer's IR type.
                // Falls back to field_index * 8 if the struct can't be
                // determined (e.g. opaque pointers to unknown structs).
                let offset = func.values.get(struct_ptr)
                    .and_then(|m| match &m.ty {
                        IrType::Ptr(inner) => Some(inner.as_ref().clone()),
                        _ => None,
                    })
                    .and_then(|pt| match pt {
                        IrType::Struct(name) => {
                            self.struct_layouts.get(&name).and_then(|layout| {
                                layout.fields.get(*field_index as usize).map(|(_, _, off)| *off as u64)
                            })
                        }
                        _ => None,
                    })
                    .unwrap_or((*field_index as u64) * 8);
                wln!(self, "    {} = (char*){} + {};", dst, s, offset);
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
                        let val_ty = func.values.get(v).map(|m| m.ty.clone());
                        let ret = &self.cur_fn_c_ret_type;
                        // Cast the return value if its IR type doesn't match
                        // the C return type (e.g. returning a pointer from an
                        // int64_t function, or an integer from a pointer
                        // function).
                        let val_is_ptr = matches!(val_ty, Some(IrType::Ptr(_)) | Some(IrType::Opaque(_)) | Some(IrType::Generic(_, _)));
                        let ret_is_ptr = ret == "void*" || ret.starts_with("struct ");
                        let expr = match (val_is_ptr, ret_is_ptr) {
                            (true, false) => format!("({}){}", ret, vn),
                            (false, true) => format!("(void*){}", vn),
                            _ => vn,
                        };
                        wln!(self, "    return {};", expr);
                    }
                    None => {
                        if self.cur_fn_c_ret_is_void {
                            wln!(self, "    return;");
                        } else {
                            // Return { value: None } in a non-void C function.
                            // This only happens in dead/unreachable blocks
                            // (the real returns have `value: Some(..)`), so
                            // emit a safe default value.
                            wln!(self, "    return 0;");
                        }
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
                        Some(IrType::Opaque(name)) if name == "any" => {
                            wln!(self, "    p_print_int((int64_t){});", arg_str);
                        }
                        Some(IrType::Generic(name, _)) if name == "Vec" => {
                            wln!(self, "    {{ char* _s = p_to_str_vec((PVec*){}); p_print_str(_s); free(_s); }}", arg_str);
                        }
                        Some(IrType::Generic(name, _)) if name == "HashMap" => {
                            wln!(self, "    {{ char* _s = p_to_str_map((PMap*){}); p_print_str(_s); free(_s); }}", arg_str);
                        }
                        Some(IrType::Generic(_, _)) => {
                            wln!(self, "    p_print_int((int64_t){});", arg_str);
                        }
                        Some(IrType::Ptr(_)) | Some(IrType::Opaque(_)) => {
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
                    // Note: print does NOT add a newline — println does that.
                    wln!(self, "    {} = 0;", dst);
                    return;
                }
                // Special-case: println is polymorphic — dispatch based on arg type
                // println has 2 args: (ctx, value) — we dispatch on the value
                if fname == "println" && args.len() >= 2 {
                    let arg_val = args[1];
                    let arg_ty = func.values.get(&arg_val).map(|m| &m.ty);
                    let arg_str = self.val_name(arg_val);
                    match arg_ty {
                        Some(IrType::Opaque(name)) if name == "any" => {
                            wln!(self, "    p_print_int((int64_t){});", arg_str);
                        }
                        Some(IrType::Generic(name, _)) if name == "Vec" => {
                            wln!(self, "    {{ char* _s = p_to_str_vec((PVec*){}); p_print_str(_s); free(_s); }}", arg_str);
                        }
                        Some(IrType::Generic(name, _)) if name == "HashMap" => {
                            wln!(self, "    {{ char* _s = p_to_str_map((PMap*){}); p_print_str(_s); free(_s); }}", arg_str);
                        }
                        Some(IrType::Generic(_, _)) => {
                            wln!(self, "    p_print_int((int64_t){});", arg_str);
                        }
                        Some(IrType::Ptr(_)) | Some(IrType::Opaque(_)) => {
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
                    if !matches!(ret_ty, IrType::Void | IrType::Never) {
                        wln!(self, "    {} = 0;", dst);
                    }
                    return;
                }
                // Special-case: p_to_str is polymorphic — dispatch based on arg type
                // The frontend emits p_to_str for all types, but we need to call
                // the right variant (p_to_str, p_to_str_float, p_to_str_vec, p_to_str_map)
                if fname == "p_to_str" && !args.is_empty() {
                    let arg_val = args[0];
                    let arg_ty = func.values.get(&arg_val).map(|m| &m.ty);
                    let arg_str = self.val_name(arg_val);
                    // The destination is always a char* (void* in C),
                    // so cast the return to void* not int64_t.
                    match arg_ty {
                        Some(IrType::F64) | Some(IrType::F32) => {
                            wln!(self, "    {} = (void*)p_to_str_float({});", dst, arg_str);
                        }
                        Some(IrType::Generic(name, _)) if name == "Vec" => {
                            wln!(self, "    {} = (void*)p_to_str_vec((PVec*){});", dst, arg_str);
                        }
                        Some(IrType::Generic(name, _)) if name == "HashMap" => {
                            wln!(self, "    {} = (void*)p_to_str_map((PMap*){});", dst, arg_str);
                        }
                        Some(IrType::Bool) => {
                            wln!(self, "    {} = (void*)p_to_str_bool({});", dst, arg_str);
                        }
                        Some(IrType::Ptr(_)) | Some(IrType::Opaque(_)) => {
                            wln!(self, "    {} = (void*)p_to_str_str((char*){});", dst, arg_str);
                        }
                        _ => {
                            wln!(self, "    {} = (void*)p_to_str((int64_t){});", dst, arg_str);
                        }
                    }
                    return;
                }
                let c_fname = Self::sanitize_c_name(fname);
                // Look up the called function's parameter types so we can
                // cast arguments to the correct C type (e.g. the hidden
                // context pointer is passed as I64 0 but the parameter is
                // void*).
                let param_types = self.fn_param_types.get(fname);
                let arg_strs: Vec<String> = args
                    .iter()
                    .enumerate()
                    .map(|(i, a)| {
                        let base = self.val_name(*a);
                        match param_types {
                            Some(pts) if i < pts.len() => {
                                Self::cast_arg(&base, &func.values.get(a).map(|m| m.ty.clone()).unwrap_or(IrType::I64), &pts[i])
                            }
                            _ => base,
                        }
                    })
                    .collect();
                let args_joined = arg_strs.join(", ");
                // Check the called function's actual C return type.
                // The IR ret_ty may say I64 but the function actually
                // returns void (e.g. println).  In that case, call the
                // function without assignment and set dst to 0.
                let callee_c_ret = self.fn_ret_types
                    .get(fname)
                    .map(|t| Self::c_ret_type(t))
                    .unwrap_or_else(|| Self::c_ret_type(ret_ty));
                if callee_c_ret == "void" {
                    wln!(self, "    {}({});", c_fname, args_joined);
                    if !matches!(ret_ty, IrType::Void | IrType::Never) {
                        wln!(self, "    {} = 0;", dst);
                    }
                } else {
                    match ret_ty {
                        IrType::Void | IrType::Never => {
                            wln!(self, "    {}({});", c_fname, args_joined);
                        }
                        _ => {
                            let dst_ty = func.values.get(&id).map(|m| &m.ty);
                            let cast = match dst_ty {
                                Some(IrType::Ptr(_)) | Some(IrType::Opaque(_)) | Some(IrType::Generic(_, _)) => {
                                    "".to_string()
                                }
                                Some(IrType::F64) => "(double)".to_string(),
                                Some(IrType::F32) => "(float)".to_string(),
                                Some(IrType::Bool) => "(bool)".to_string(),
                                _ => "(int64_t)".to_string(),
                            };
                            wln!(self, "    {} = {}{}({});", dst, cast, c_fname, args_joined);
                        }
                    }
                }
            }
            Inst::CallPtr { fn_ptr, args, ret_ty, has_env, .. } => {
                let fp = self.val_name(*fn_ptr);
                // Closures in Pledge use string-based dispatch: the closure
                // struct's field 0 is a char* pointing to the function name.
                // We generate an inline if/else chain that compares the name
                // and calls the appropriate C function.
                let arg_strs: Vec<String> = args
                    .iter()
                    .map(|a| self.val_name(*a))
                    .collect();

                // Find candidate functions: user functions whose param count
                // matches args.len() + 1 (the +1 is for the ctx/env param)
                // AND whose return type matches the CallPtr's ret_ty.
                let expected_param_count = args.len() + 1;
                let candidates: Vec<&String> = self.fn_param_types
                    .iter()
                    .filter(|(name, pts)| {
                        self.user_functions.contains(*name)
                            && pts.len() == expected_param_count
                            && self.fn_ret_types.get(*name).map_or(false, |rt| {
                                // Match by C return type category (void vs non-void)
                                let c_rt = Self::c_ret_type(rt);
                                let c_expected = Self::c_ret_type(ret_ty);
                                c_rt == c_expected
                            })
                    })
                    .map(|(name, _)| name)
                    .collect();

                // Build the dispatch block — generate an if/else if chain
                // using a single string to avoid brace matching issues with
                // the wln! macro (which doesn't process {{ }} in the
                // single-argument form).
                let mut dispatch = String::new();
                dispatch.push_str("    {\n");
                dispatch.push_str(&format!("        if (!{}) abort();\n", fp));
                dispatch.push_str(&format!("        char* _fn_name = *(char**){};\n", fp));
                dispatch.push_str("        if (!_fn_name) abort();\n");
                let mut first = true;
                for &fname in &candidates {
                    let c_fname = Self::sanitize_c_name(fname);
                    let param_types = self.fn_param_types.get(fname);
                    // Build call args, casting each to the candidate function's
                    // parameter type.  The first param is always the ctx/env
                    // pointer (void*).
                    let mut call_args: Vec<String> = Vec::new();
                    // Arg 0: env pointer or 0
                    if *has_env {
                        call_args.push(format!("(void*){}", fp));
                    } else {
                        call_args.push("(void*)0".to_string());
                    }
                    // Args 1..N: the explicit CallPtr args, cast to match
                    // the candidate function's param types.
                    for (i, arg) in arg_strs.iter().enumerate() {
                        let param_idx = i + 1; // skip ctx param
                        let cast = param_types
                            .and_then(|pts| pts.get(param_idx))
                            .map(|pt| Self::c_type(pt))
                            .unwrap_or_else(|| "int64_t".to_string());
                        if cast == "void*" || cast.starts_with("struct ") {
                            call_args.push(format!("(void*){}", arg));
                        } else if cast == "double" {
                            call_args.push(format!("(double){}", arg));
                        } else if cast == "float" {
                            call_args.push(format!("(float){}", arg));
                        } else if cast == "bool" {
                            call_args.push(format!("(bool){}", arg));
                        } else {
                            call_args.push(format!("({}){}", cast, arg));
                        }
                    }
                    let call_expr = format!("{}({})", c_fname, call_args.join(", "));
                    if first {
                        dispatch.push_str(&format!(
                            "        if (strcmp(_fn_name, \"{}\") == 0) {{\n",
                            fname
                        ));
                        first = false;
                    } else {
                        dispatch.push_str(&format!(
                            "        }} else if (strcmp(_fn_name, \"{}\") == 0) {{\n",
                            fname
                        ));
                    }
                    match ret_ty {
                        IrType::Void | IrType::Never => {
                            dispatch.push_str(&format!("            {};\n", call_expr));
                        }
                        _ => {
                            dispatch.push_str(&format!("            {} = {};\n", dst, call_expr));
                        }
                    }
                }
                if first {
                    // No candidates found — this can happen if the closure
                    // targets a runtime function (not a user function).
                    // Fall back to a direct function pointer call with
                    // best-effort casting.  This is unsafe but matches the
                    // interpreter's behavior of calling through the pointer.
                    let args_cast: Vec<String> = args
                        .iter()
                        .map(|a| {
                            let base = self.val_name(*a);
                            format!("(void*){}", base)
                        })
                        .collect();
                    let args_joined = args_cast.join(", ");
                    // Pass env pointer as first arg
                    let full_args = if *has_env {
                        format!("(void*){}, {}", fp, args_joined)
                    } else {
                        args_joined
                    };
                    match ret_ty {
                        IrType::Void | IrType::Never => {
                            dispatch.push_str(&format!(
                                "        ((void(*)(void*)){})({});\n",
                                fp, full_args
                            ));
                        }
                        _ => {
                            dispatch.push_str(&format!(
                                "        {} = (int64_t)((int64_t(*)(void*)){})({});\n",
                                dst, fp, full_args
                            ));
                        }
                    }
                } else {
                    dispatch.push_str("        } else { abort(); }\n");
                }
                dispatch.push_str("    }\n");
                self.out.push_str(&dispatch);
            }
            Inst::MakeDyn { type_name, trait_name, .. } => {
                wln!(self, "    /* make dyn {} as {} */", type_name, trait_name);
                wln!(self, "    {} = 0; /* MakeDyn not yet supported in C codegen */", dst);
            }
            Inst::DynCall { trait_name, method, args, ret_ty, .. } => {
                let arg_strs: Vec<String> = args.iter().map(|a| self.val_name(*a)).collect();
                let void_ret = Self::c_ret_type(ret_ty) == "void";
                let mut dispatch = format!("    /* dyncall {}::{} */\n", trait_name, method);
                if !arg_strs.is_empty() {
                    let type_arg = &arg_strs[0];
                    let candidates: Vec<String> = self.user_functions.iter()
                        .filter(|name| name.ends_with(&format!("_{}", method)))
                        .cloned()
                        .collect();
                    dispatch.push_str(&format!("    {{ const char* _t = (const char*){};\n", type_arg));
                    dispatch.push_str("    if (false) {}\n");
                    for cname in &candidates {
                        let type_name = cname.rsplitn(2, '_').nth(1).unwrap_or("");
                        let call_args = arg_strs[1..].join(", ");
                        if void_ret {
                            dispatch.push_str(&format!(
                                "    else if (strcmp(_t, \"{}\") == 0) {}({});\n",
                                type_name, cname, call_args
                            ));
                        } else {
                            dispatch.push_str(&format!(
                                "    else if (strcmp(_t, \"{}\") == 0) {} = {}({});\n",
                                type_name, dst, cname, call_args
                            ));
                        }
                    }
                    if !void_ret { dispatch.push_str("    else { abort(); }\n"); }
                    dispatch.push_str("    }\n");
                }
                self.out.push_str(&dispatch);
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
            Inst::Asm { template, .. } => {
                // C backend: emit inline assembly using GCC __asm__ syntax
                let escaped = template.replace("\\", "\\\\").replace("\"", "\\\"");
                wln!(self, "    __asm__ volatile(\"{}\");", escaped);
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
        wln!(self, "    if (!v) return;");
        wln!(self, "    if (v->len >= v->cap) {{");
        wln!(self, "        v->cap = v->cap == 0 ? 8 : v->cap * 2;");
        wln!(self, "        v->data = (void**)realloc(v->data, v->cap * sizeof(void*));");
        wln!(self, "    }}");
        wln!(self, "    v->data[v->len++] = val;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "void* p_vec_get(PVec* v, int64_t idx) {{");
        wln!(self, "    if (!v || idx < 0 || idx >= v->len) return NULL;");
        wln!(self, "    return v->data[idx];");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "int64_t p_vec_len(PVec* v) {{ return v ? v->len : 0; }}");
        wln!(self, "bool p_vec_is_empty(PVec* v) {{ return v ? v->len == 0 : true; }}");
        wln!(self, "void p_vec_clear(PVec* v) {{ if (v) v->len = 0; }}");
        wln!(self, "int64_t p_vec_pop(PVec* v) {{");
        wln!(self, "    if (!v || v->len == 0) return 0;");
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
        wln!(self, "    if (!m || !key) return;");
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
        wln!(self, "    if (!m || !key) return 0;");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) {{");
        wln!(self, "        if (strcmp(m->keys[i], key) == 0) return m->vals[i];");
        wln!(self, "    }}");
        wln!(self, "    return 0;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "bool p_map_contains(PMap* m, const char* key) {{");
        wln!(self, "    if (!m || !key) return false;");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) {{");
        wln!(self, "        if (strcmp(m->keys[i], key) == 0) return true;");
        wln!(self, "    }}");
        wln!(self, "    return false;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "void p_map_remove(PMap* m, const char* key) {{");
        wln!(self, "    if (!m || !key) return;");
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
        wln!(self, "int64_t p_map_len(PMap* m) {{ return m ? m->len : 0; }}");
        wln!(self, "bool p_map_is_empty(PMap* m) {{ return m ? m->len == 0 : true; }}");
        wln!(self, "void p_map_clear(PMap* m) {{ if (m) m->len = 0; }}");
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
        wln!(self, "int64_t p_string_len(const char* s) {{ return s ? (int64_t)strlen(s) : 0; }}");
        wln!(self, "bool p_string_is_empty(const char* s) {{ return !s || s[0] == 0; }}");
        wln!(self, "");
        wln!(self, "char* p_string_push_str(const char* s, const char* other) {{");
        wln!(self, "    if (!s) s = \"\"; if (!other) other = \"\";");
        wln!(self, "    size_t len = strlen(s) + strlen(other);");
        wln!(self, "    char* result = (char*)malloc(len + 1);");
        wln!(self, "    strcpy(result, s);");
        wln!(self, "    strcat(result, other);");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "char* p_string_from(const char* s) {{ return s ? strdup(s) : strdup(\"\"); }}");
        wln!(self, "");
        wln!(self, "char* p_string_slice(const char* s, int64_t start, int64_t end) {{");
        wln!(self, "    if (!s) return strdup(\"\");");
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
        wln!(self, "");
        // String functions
        wln!(self, "bool p_string_contains(const char* s, const char* sub) {{ return s && sub && strstr(s, sub) != NULL; }}");
        wln!(self, "bool p_string_starts_with(const char* s, const char* prefix) {{");
        wln!(self, "    if (!s || !prefix) return false;");
        wln!(self, "    size_t plen = strlen(prefix);");
        wln!(self, "    return strncmp(s, prefix, plen) == 0;");
        wln!(self, "}}");
        wln!(self, "bool p_string_ends_with(const char* s, const char* suffix) {{");
        wln!(self, "    if (!s || !suffix) return false;");
        wln!(self, "    size_t slen = strlen(s), flen = strlen(suffix);");
        wln!(self, "    if (flen > slen) return false;");
        wln!(self, "    return strcmp(s + slen - flen, suffix) == 0;");
        wln!(self, "}}");
        wln!(self, "int64_t p_string_find(const char* s, const char* sub) {{");
        wln!(self, "    if (!s || !sub) return -1;");
        wln!(self, "    const char* p = strstr(s, sub);");
        wln!(self, "    return p ? (int64_t)(p - s) : -1;");
        wln!(self, "}}");
        wln!(self, "char* p_string_trim(const char* s) {{");
        wln!(self, "    if (!s) return strdup(\"\");");
        wln!(self, "    while (*s == ' ' || *s == '\\t' || *s == '\\n' || *s == '\\r') s++;");
        wln!(self, "    size_t len = strlen(s);");
        wln!(self, "    while (len > 0 && (s[len-1] == ' ' || s[len-1] == '\\t' || s[len-1] == '\\n' || s[len-1] == '\\r')) len--;");
        wln!(self, "    char* result = (char*)malloc(len + 1);");
        wln!(self, "    memcpy(result, s, len);");
        wln!(self, "    result[len] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "char* p_string_to_uppercase(const char* s) {{");
        wln!(self, "    if (!s) return strdup(\"\");");
        wln!(self, "    size_t len = strlen(s);");
        wln!(self, "    char* result = (char*)malloc(len + 1);");
        wln!(self, "    for (size_t i = 0; i < len; i++) result[i] = toupper((unsigned char)s[i]);");
        wln!(self, "    result[len] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "char* p_string_to_lowercase(const char* s) {{");
        wln!(self, "    if (!s) return strdup(\"\");");
        wln!(self, "    size_t len = strlen(s);");
        wln!(self, "    char* result = (char*)malloc(len + 1);");
        wln!(self, "    for (size_t i = 0; i < len; i++) result[i] = tolower((unsigned char)s[i]);");
        wln!(self, "    result[len] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "char* p_string_repeat(const char* s, int64_t n) {{");
        wln!(self, "    if (!s) return strdup(\"\");");
        wln!(self, "    size_t slen = strlen(s);");
        wln!(self, "    char* result = (char*)malloc(slen * (size_t)n + 1);");
        wln!(self, "    result[0] = 0;");
        wln!(self, "    for (int64_t i = 0; i < n; i++) strcat(result, s);");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "char* p_string_replace(const char* s, const char* from, const char* to) {{");
        wln!(self, "    if (!s) return strdup(\"\");");
        wln!(self, "    if (!from || !to || !*from) return strdup(s);");
        wln!(self, "    size_t slen = strlen(s), flen = strlen(from), tlen = strlen(to);");
        wln!(self, "    char* result = (char*)malloc(slen * (tlen > flen ? tlen : flen) + 1);");  // overestimate
        wln!(self, "    size_t ri = 0; size_t si = 0;");
        wln!(self, "    while (si < slen) {{");
        wln!(self, "        if (slen - si >= flen && strncmp(s + si, from, flen) == 0) {{");
        wln!(self, "            memcpy(result + ri, to, tlen); ri += tlen; si += flen;");
        wln!(self, "        }} else {{ result[ri++] = s[si++]; }}");
        wln!(self, "    }}");
        wln!(self, "    result[ri] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "PVec* p_string_split(const char* s, const char* delim) {{");
        wln!(self, "    PVec* v = p_vec_new();");
        wln!(self, "    if (!s || !delim) return v;");
        wln!(self, "    const char* start = s;");
        wln!(self, "    size_t dlen = strlen(delim);");
        wln!(self, "    while (*start) {{");
        wln!(self, "        const char* p = strstr(start, delim);");
        wln!(self, "        if (!p) {{ p_vec_push(v, strdup(start)); break; }}");
        wln!(self, "        size_t len = p - start;");
        wln!(self, "        char* part = (char*)malloc(len + 1);");
        wln!(self, "        memcpy(part, start, len); part[len] = 0;");
        wln!(self, "        p_vec_push(v, part);");
        wln!(self, "        start = p + dlen;");
        wln!(self, "    }}");
        wln!(self, "    return v;");
        wln!(self, "}}");
        wln!(self, "int64_t p_string_parse_int(const char* s) {{ return s ? (int64_t)atoll(s) : 0; }}");
        wln!(self, "char* p_string_char_at(const char* s, int64_t idx) {{");
        wln!(self, "    if (!s) return strdup(\"\");");
        wln!(self, "    char* result = (char*)malloc(2);");
        wln!(self, "    result[0] = s[idx]; result[1] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "char* p_string_substring(const char* s, int64_t start, int64_t end) {{");
        wln!(self, "    return p_string_slice(s, start, end);");
        wln!(self, "}}");
        wln!(self, "");
        // Additional vec functions
        wln!(self, "PVec* p_vec_clone(PVec* v) {{");
        wln!(self, "    PVec* c = p_vec_new();");
        wln!(self, "    for (int64_t i = 0; i < v->len; i++) p_vec_push(c, v->data[i]);");
        wln!(self, "    return c;");
        wln!(self, "}}");
        wln!(self, "bool p_vec_contains(PVec* v, void* val) {{");
        wln!(self, "    for (int64_t i = 0; i < v->len; i++) if (v->data[i] == val) return true;");
        wln!(self, "    return false;");
        wln!(self, "}}");
        wln!(self, "void p_vec_extend(PVec* v, PVec* other) {{");
        wln!(self, "    for (int64_t i = 0; i < other->len; i++) p_vec_push(v, other->data[i]);");
        wln!(self, "}}");
        wln!(self, "void* p_vec_first(PVec* v) {{ return v->len > 0 ? v->data[0] : NULL; }}");
        wln!(self, "void* p_vec_last(PVec* v) {{ return v->len > 0 ? v->data[v->len - 1] : NULL; }}");
        wln!(self, "void p_vec_insert(PVec* v, int64_t idx, void* val) {{");
        wln!(self, "    if (idx < 0 || idx > v->len) return;");
        wln!(self, "    if (v->len >= v->cap) {{ v->cap = v->cap == 0 ? 8 : v->cap * 2; v->data = (void**)realloc(v->data, v->cap * sizeof(void*)); }}");
        wln!(self, "    memmove(&v->data[idx+1], &v->data[idx], (v->len - idx) * sizeof(void*));");
        wln!(self, "    v->data[idx] = val; v->len++;");
        wln!(self, "}}");
        wln!(self, "void* p_vec_remove(PVec* v, int64_t idx) {{");
        wln!(self, "    if (idx < 0 || idx >= v->len) return NULL;");
        wln!(self, "    void* val = v->data[idx];");
        wln!(self, "    memmove(&v->data[idx], &v->data[idx+1], (v->len - idx - 1) * sizeof(void*));");
        wln!(self, "    v->len--;");
        wln!(self, "    return val;");
        wln!(self, "}}");
        wln!(self, "void p_vec_reverse(PVec* v) {{");
        wln!(self, "    for (int64_t i = 0, j = v->len - 1; i < j; i++, j--) {{ void* t = v->data[i]; v->data[i] = v->data[j]; v->data[j] = t; }}");
        wln!(self, "}}");
        wln!(self, "void p_vec_sort(PVec* v) {{");
        wln!(self, "    for (int64_t i = 0; i < v->len; i++)");
        wln!(self, "        for (int64_t j = i+1; j < v->len; j++)");
        wln!(self, "            if ((int64_t)v->data[i] > (int64_t)v->data[j]) {{ void* t = v->data[i]; v->data[i] = v->data[j]; v->data[j] = t; }}");
        wln!(self, "}}");
        wln!(self, "void p_vec_truncate(PVec* v, int64_t len) {{ if (len < v->len) v->len = len; }}");
        wln!(self, "");
        // Additional map functions
        wln!(self, "PMap* p_map_clone(PMap* m) {{");
        wln!(self, "    PMap* c = p_map_new();");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) p_map_insert(c, m->keys[i], m->vals[i]);");
        wln!(self, "    return c;");
        wln!(self, "}}");
        wln!(self, "int64_t p_map_get_or(PMap* m, const char* key, int64_t default_val) {{");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) if (strcmp(m->keys[i], key) == 0) return m->vals[i];");
        wln!(self, "    return default_val;");
        wln!(self, "}}");
        wln!(self, "");
        // p_to_str variants
        wln!(self, "char* p_to_str_vec(PVec* v) {{");
        wln!(self, "    char* result = strdup(\"[\");");
        wln!(self, "    for (int64_t i = 0; i < v->len; i++) {{");
        wln!(self, "        if (i > 0) {{ char* tmp = p_str_concat(result, \", \"); free(result); result = tmp; }}");
        wln!(self, "        char* s = p_to_str((int64_t)v->data[i]);");
        wln!(self, "        char* tmp = p_str_concat(result, s); free(result); free(s); result = tmp;");
        wln!(self, "    }}");
        wln!(self, "    char* tmp = p_str_concat(result, \"]\"); free(result); result = tmp;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "char* p_to_str_map(PMap* m) {{");
        wln!(self, "    if (!m) return strdup(\"{{}}\");");
        wln!(self, "    // Sort keys for deterministic output");
        wln!(self, "    int64_t* order = (int64_t*)malloc(m->len * sizeof(int64_t));");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) order[i] = i;");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++)");
        wln!(self, "        for (int64_t j = i+1; j < m->len; j++)");
        wln!(self, "            if (strcmp(m->keys[order[i]], m->keys[order[j]]) > 0) {{ int64_t t = order[i]; order[i] = order[j]; order[j] = t; }}");
        wln!(self, "    char* result = strdup(\"{\");");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) {{");
        wln!(self, "        if (i > 0) {{ char* tmp = p_str_concat(result, \", \"); free(result); result = tmp; }}");
        wln!(self, "        char* tmp = p_str_concat(result, m->keys[order[i]]); free(result); result = tmp;");
        wln!(self, "        tmp = p_str_concat(result, \": \"); free(result); result = tmp;");
        wln!(self, "        char* vs = p_to_str(m->vals[order[i]]);");
        wln!(self, "        tmp = p_str_concat(result, vs); free(result); free(vs); result = tmp;");
        wln!(self, "    }}");
        wln!(self, "    char* tmp = p_str_concat(result, \"}\"); free(result); result = tmp;");
        wln!(self, "    free(order);");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "char* p_to_str_str(const char* s) {{ return strdup(s); }}");
        wln!(self, "char* p_to_str_bool(bool val) {{ return strdup(val ? \"true\" : \"false\"); }}");
        wln!(self, "char* p_to_str_float(double val) {{");
        wln!(self, "    char* buf = (char*)malloc(32);");
        wln!(self, "    snprintf(buf, 32, \"%g\", val);");
        wln!(self, "    return buf;");
        wln!(self, "}}");
    }
}

pub fn generate_c(module: &IrModule) -> String {
    let mut gen = CCodeGen::new();
    gen.generate(module)
}
