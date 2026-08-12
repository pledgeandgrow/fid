// C code generation from Pledge IR
//
// Strategy: translate IR functions to C functions, with a runtime library
// implementing p_vec_*, p_map_*, p_string_* in C. This produces portable
// native binaries via gcc/clang.

use crate::ir::*;
use crate::ast::{self, BinOp, UnaryOp};
use std::collections::HashMap;

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
    /// User function parameter types (from the module).  Used for argument
    /// casting when calling user functions with array/struct params.
    user_fn_param_types: HashMap<String, Vec<IrType>>,
    /// Function return types, keyed by IR function name.  Used for CallPtr
    /// dispatch to filter candidates by return type.
    fn_ret_types: HashMap<String, IrType>,
    /// Set of user-defined extern function names (FFI). These should NOT be
    /// sanitized (they keep their original C names).
    extern_func_names: std::collections::HashSet<String>,
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
            user_fn_param_types: HashMap::new(),
            fn_ret_types: HashMap::new(),
            extern_func_names: std::collections::HashSet::new(),
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

    /// Format a C parameter declaration with the variable name in the right position.
    /// For array types, C requires `type name[size]` not `type[size] name`.
    fn c_param_decl(ty: &IrType, name: &str) -> String {
        match ty {
            IrType::Array(elem, size) => format!("{} {}[{}]", Self::c_type(elem), name, size),
            _ => format!("{} {}", Self::c_type(ty), name),
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
        let from_is_float = matches!(from, IrType::F32 | IrType::F64);
        let to_is_ptr = matches!(to, IrType::Ptr(_) | IrType::Opaque(_) | IrType::Generic(_, _));
        let to_is_array = matches!(to, IrType::Array(_, _));
        let to_is_struct = Self::is_aggregate_ret(to);

        // Array params decay to pointers in C — cast to element pointer type
        if to_is_array {
            if let IrType::Array(elem, _) = to {
                return format!("({}*){}", Self::c_type(elem), val);
            }
        }

        match (from_is_ptr, to_is_ptr, to_is_struct) {
            // float → pointer: use type punning via union (double and void* are both 8 bytes)
            (false, true, _) if from_is_float => {
                format!("((union {{ double d; void* p; }}){{ .d = {} }}).p", val)
            }
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
    fn sanitize_c_name(&self, name: &str) -> String {
        // Don't sanitize user-defined extern function names (FFI)
        if self.extern_func_names.contains(name) {
            return name.to_string();
        }
        let sanitized = match name {
            "main" => "main_entry",
            "abs" => "p_abs",
            "max" => "p_max",
            "min" => "p_min",
            "div" => "p_div",
            "exit" => "p_exit",
            "printf" => "p_printf",
            "malloc" => "p_malloc_user",
            "free" => "p_free_user",
            "realloc" => "p_realloc",
            "calloc" => "p_calloc",
            "abort" => "p_abort",
            "atoi" => "p_atoi",
            "atof" => "p_atof",
            "strlen" => "p_strlen_user",
            "strcmp" => "p_strcmp_user",
            "strcpy" => "p_strcpy_user",
            "memcpy" => "p_memcpy_user",
            "memset" => "p_memset_user",
            "pow" => "p_pow",
            "sqrt" => "p_sqrt",
            "sin" => "p_sin",
            "cos" => "p_cos",
            "tan" => "p_tan",
            "log" => "p_log",
            "exp" => "p_exp_user",
            "floor" => "p_floor",
            "ceil" => "p_ceil",
            "round" => "p_round",
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
            other => return other.to_string(),
        };
        sanitized.to_string()
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
        self.user_fn_param_types.clear();
        self.fn_ret_types.clear();
        self.extern_func_names.clear();
        for (name, func) in &module.functions {
            self.user_functions.insert(name.clone());
            self.user_fn_param_types.insert(
                name.clone(),
                func.params.iter().map(|(_, ty)| ty.clone()).collect(),
            );
            self.fn_ret_types.insert(name.clone(), func.ret_ty.clone());
        }
        // Collect extern function names (these should NOT be sanitized)
        for ext in &module.extern_decls {
            self.extern_func_names.insert(ext.pledge_name.clone());
        }
        // Runtime functions (not in module.functions) — needed so that
        // call-site argument casting knows their parameter types.
        self.insert_runtime_sigs();

        // Emit runtime header
        self.emit_runtime_header();

        // Emit user-defined extern includes and declarations
        self.emit_extern_decls(module);

        // Emit runtime types and declarations
        self.emit_runtime_types();

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
            ("p_to_str_map", &[p()]),
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
            ("p_vec_set", &[p(), I64, p()]),
            ("p_cli", &[]),
            ("p_sti", &[]),
            ("p_cr0_read", &[]),
            ("p_cr3_read", &[]),
            ("p_cr4_read", &[]),
            ("p_cr3_write", &[I64]),
            ("p_invlpg", &[I64]),
            ("p_page_map", &[I64, I64, I64, I64]),
            ("p_page_unmap", &[I64, I64]),
            ("p_phys_read64", &[I64]),
            ("p_phys_write64", &[I64, I64]),
            ("p_simd_add", &[p(), p()]),
            ("p_simd_sub", &[p(), p()]),
            ("p_simd_mul", &[p(), p()]),
            ("p_simd_div", &[p(), p()]),
            ("p_simd_dot", &[p(), p()]),
            ("p_simd_sum", &[p()]),
            ("p_simd_scale", &[p(), F64]),
            ("p_simd_from_vec", &[p()]),
            ("p_simd_to_str", &[p()]),
            ("p_tensor_new", &[I64, p()]),
            ("p_tensor_from", &[p(), I64, p()]),
            ("p_tensor_zeros", &[p()]),
            ("p_tensor_ones", &[p()]),
            ("p_tensor_filled", &[p(), F64]),
            ("p_tensor_from_vec", &[p(), p()]),
            ("p_tensor_clone", &[p()]),
            ("p_tensor_at", &[p(), I64]),
            ("p_tensor_get", &[p(), p()]),
            ("p_tensor_set", &[p(), p(), F64]),
            ("p_tensor_add", &[p(), p()]),
            ("p_tensor_sub", &[p(), p()]),
            ("p_tensor_mul", &[p(), p()]),
            ("p_tensor_matmul", &[p(), p()]),
            ("p_tensor_transpose", &[p()]),
            ("p_tensor_relu", &[p()]),
            ("p_tensor_sigmoid", &[p()]),
            ("p_tensor_softmax", &[p()]),
            ("p_tensor_scale", &[p(), F64]),
            ("p_tensor_sum", &[p()]),
            ("p_tensor_mean", &[p()]),
            ("p_tensor_min", &[p()]),
            ("p_tensor_max", &[p()]),
            ("p_tensor_dot", &[p(), p()]),
            ("p_tensor_reshape", &[p(), p()]),
            ("p_tensor_len", &[p()]),
            ("p_tensor_ndim", &[p()]),
            ("p_tensor_shape", &[p()]),
            ("p_tensor_size", &[p()]),
            ("p_tensor_to_str", &[p()]),
            ("p_tensor_to_json", &[p()]),
            ("p_json_parse", &[p()]),
            ("p_json_new_object", &[]),
            ("p_json_new_array", &[]),
            ("p_json_set", &[p(), p(), p()]),
            ("p_json_set_int", &[p(), p(), I64]),
            ("p_json_set_float", &[p(), p(), F64]),
            ("p_json_set_bool", &[p(), p(), I64]),
            ("p_json_append", &[p(), p()]),
            ("p_json_append_int", &[p(), I64]),
            ("p_json_append_float", &[p(), F64]),
            ("p_json_dump", &[p()]),
            ("p_json_dump_pretty", &[p()]),
            ("p_json_pretty", &[p()]),
            ("p_json_string", &[p()]),
            ("p_json_type", &[p()]),
            ("p_json_get", &[p(), p()]),
            ("p_json_get_int", &[p(), p()]),
            ("p_json_get_float", &[p(), p()]),
            ("p_json_get_bool", &[p(), p()]),
            ("p_json_at", &[p(), I64]),
            ("p_json_len", &[p()]),
            ("p_json_object_get", &[p(), p()]),
            ("p_http_listen", &[I64]),
            ("p_http_recv_request", &[I64]),
            ("p_http_send_response", &[I64, I64, p()]),
            ("p_http_build_response", &[I64, p(), p()]),
            ("p_http_parse_request", &[p()]),
            ("p_db_connect", &[p()]),
            ("p_db_exec", &[p(), p()]),
            ("p_db_query", &[p(), p()]),
            ("p_db_get", &[p(), p()]),
            ("p_db_set", &[p(), p(), p()]),
            ("p_db_mem_open", &[I64]),
            ("p_db_mem_exec", &[p(), p()]),
            ("p_db_mem_query", &[p(), p()]),
            ("p_db_mem_close", &[p()]),
            ("p_hlt", &[]),
            ("p_socket_write_str", &[I64, p()]),
        ];
        for (name, params) in sigs {
            self.fn_param_types
                .insert(name.to_string(), params.to_vec());
        }
        // Runtime function return types — most return int64_t or void*,
        // but some return void (mutating operations).
        let void_ret: &[&str] = &[
            "p_vec_push", "p_vec_clear", "p_vec_insert", "p_vec_set",
            "p_vec_reverse", "p_vec_sort", "p_vec_truncate", "p_vec_extend",
            "p_map_insert", "p_map_remove", "p_map_clear",
            "p_print", "p_print_str", "p_print_int", "p_print_bool",
            "p_print_float", "p_print_newline", "p_free", "p_panic",
            "p_socket_close",
            "p_cli", "p_sti", "p_cr3_write", "p_invlpg", "p_page_map", "p_page_unmap",
            "p_phys_write64",
            "p_tensor_set",
            "p_json_set", "p_json_set_int", "p_json_set_float", "p_json_set_bool",
            "p_json_append", "p_json_append_int", "p_json_append_float",
            "p_http_send_response",
            "p_db_exec", "p_db_set", "p_db_mem_close",
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
        wln!(self, "#include <math.h>");
        // Socket support for native HTTP server (Phase 5D)
        wln!(self, "#ifdef _WIN32");
        wln!(self, "#include <winsock2.h>");
        wln!(self, "#include <ws2tcpip.h>");
        wln!(self, "#pragma comment(lib, \"ws2_32.lib\")");
        wln!(self, "#else");
        wln!(self, "#include <sys/socket.h>");
        wln!(self, "#include <netinet/in.h>");
        wln!(self, "#include <arpa/inet.h>");
        wln!(self, "#include <unistd.h>");
        wln!(self, "#include <fcntl.h>");
        wln!(self, "#endif");
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
    }

    /// Emit user-defined extern declarations (FFI)
    fn emit_extern_decls(&mut self, module: &IrModule) {
        if module.extern_decls.is_empty() {
            return;
        }

        // Emit #include directives
        let mut emitted_includes: std::collections::HashSet<String> = std::collections::HashSet::new();
        for ext in &module.extern_decls {
            if let Some(inc) = &ext.include {
                if emitted_includes.insert(inc.clone()) {
                    wln!(self, "#include {}", inc);
                }
            }
        }
        if !emitted_includes.is_empty() {
            wln!(self, "");
        }

        // Emit link library directives (parsed by the ccompile command)
        let mut emitted_libs: std::collections::HashSet<String> = std::collections::HashSet::new();
        for ext in &module.extern_decls {
            if let Some(lib) = &ext.link_lib {
                if emitted_libs.insert(lib.clone()) {
                    wln!(self, "// PLINK: {}", lib);
                }
            }
        }

        // Emit extern function declarations
        wln!(self, "// ─── User-defined extern declarations (FFI) ───");
        for ext in &module.extern_decls {
            let ret_c = self.c_type_from_ast(&ext.ret_ty);
            let params_c: Vec<String> = ext.params.iter()
                .map(|p| format!("{} {}", self.c_type_from_ast(&p.ty), p.name))
                .collect();
            let params_str = if params_c.is_empty() {
                "void".to_string()
            } else {
                params_c.join(", ")
            };
            // Emit the C declaration with the C function name
            wln!(self, "{} {}({});", ret_c, ext.c_name, params_str);
            // If pledge_name differs from c_name, emit a macro alias
            if ext.pledge_name != ext.c_name {
                wln!(self, "#define {} {}", ext.pledge_name, ext.c_name);
            }
        }
        wln!(self, "");
    }

    /// Convert an AST type to a C type string
    fn c_type_from_ast(&self, ty: &ast::Type) -> String {
        match ty {
            ast::Type::I8 => "int8_t".to_string(),
            ast::Type::I16 => "int16_t".to_string(),
            ast::Type::I32 => "int32_t".to_string(),
            ast::Type::I64 => "int64_t".to_string(),
            ast::Type::I128 => "__int128".to_string(),
            ast::Type::U8 => "uint8_t".to_string(),
            ast::Type::U16 => "uint16_t".to_string(),
            ast::Type::U32 => "uint32_t".to_string(),
            ast::Type::U64 => "uint64_t".to_string(),
            ast::Type::U128 => "unsigned __int128".to_string(),
            ast::Type::F32 => "float".to_string(),
            ast::Type::F64 => "double".to_string(),
            ast::Type::Bool => "bool".to_string(),
            ast::Type::Void => "void".to_string(),
            ast::Type::Str => "const char*".to_string(),
            ast::Type::Named(n) if n == "str" => "const char*".to_string(),
            ast::Type::Named(n) => n.clone(),
            ast::Type::Ref { inner, .. } => format!("{}*", self.c_type_from_ast(inner)),
            _ => "void*".to_string(),
        }
    }

    /// Emit runtime types and function declarations (continuation of emit_runtime_header)
    fn emit_runtime_types(&mut self) {
        wln!(self, "// ─── Runtime types ───");
        wln!(self, "typedef struct { void** data; int64_t len; int64_t cap; } PVec;");
        wln!(self, "typedef struct { char** keys; int64_t* vals; int64_t len; int64_t cap; } PMap;");
        wln!(self, "");
        wln!(self, "// ─── Runtime function declarations ───");
        wln!(self, "PVec* p_vec_new(void);");
        wln!(self, "void p_vec_push(PVec* v, void* val);");
        wln!(self, "void* p_vec_get(PVec* v, int64_t idx);");
        wln!(self, "void p_vec_set(PVec* v, int64_t idx, void* val);");
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
        // Bare metal builtins (Phase 5A) — stubs for C codegen
        wln!(self, "void p_cli(void);");
        wln!(self, "void p_sti(void);");
        wln!(self, "void p_hlt(void);");
        wln!(self, "int64_t p_cr0_read(void);");
        wln!(self, "int64_t p_cr3_read(void);");
        wln!(self, "int64_t p_cr4_read(void);");
        wln!(self, "void p_cr3_write(int64_t val);");
        wln!(self, "void p_invlpg(int64_t addr);");
        wln!(self, "void p_page_map(int64_t cr3, int64_t virt, int64_t phys, int64_t flags);");
        wln!(self, "void p_page_unmap(int64_t cr3, int64_t virt);");
        wln!(self, "int64_t p_phys_read64(int64_t addr);");
        wln!(self, "void p_phys_write64(int64_t addr, int64_t val);");
        // SIMD builtins (Phase 5C) — elementwise double4 operations
        wln!(self, "void* p_simd_add(void* a, void* b);");
        wln!(self, "void* p_simd_sub(void* a, void* b);");
        wln!(self, "void* p_simd_mul(void* a, void* b);");
        wln!(self, "void* p_simd_div(void* a, void* b);");
        wln!(self, "double p_simd_dot(void* a, void* b);");
        wln!(self, "double p_simd_sum(void* a);");
        wln!(self, "void* p_simd_scale(void* a, double s);");
        wln!(self, "void* p_simd_from_vec(void* v);");
        wln!(self, "char* p_simd_to_str(void* a);");
        // Tensor builtins (Phase 5C)
        wln!(self, "void* p_tensor_new(int64_t ndim, int64_t* shape);");
        wln!(self, "void* p_tensor_from(void* data, int64_t ndim, int64_t* shape);");
        wln!(self, "void* p_tensor_zeros(void* shape_vec);");
        wln!(self, "void* p_tensor_ones(void* shape_vec);");
        wln!(self, "void* p_tensor_filled(void* shape_vec, double val);");
        wln!(self, "void* p_tensor_from_vec(void* v, void* shape_vec);");
        wln!(self, "void* p_tensor_clone(void* t);");
        wln!(self, "double p_tensor_at(void* t, int64_t idx);");
        wln!(self, "double p_tensor_get(void* t, void* idx_vec);");
        wln!(self, "void p_tensor_set(void* t, void* idx_vec, double val);");
        wln!(self, "void* p_tensor_add(void* a, void* b);");
        wln!(self, "void* p_tensor_sub(void* a, void* b);");
        wln!(self, "void* p_tensor_mul(void* a, void* b);");
        wln!(self, "void* p_tensor_matmul(void* a, void* b);");
        wln!(self, "void* p_tensor_transpose(void* t);");
        wln!(self, "void* p_tensor_relu(void* t);");
        wln!(self, "void* p_tensor_sigmoid(void* t);");
        wln!(self, "void* p_tensor_softmax(void* t);");
        wln!(self, "void* p_tensor_scale(void* t, double s);");
        wln!(self, "double p_tensor_sum(void* t);");
        wln!(self, "double p_tensor_mean(void* t);");
        wln!(self, "double p_tensor_min(void* t);");
        wln!(self, "double p_tensor_max(void* t);");
        wln!(self, "double p_tensor_dot(void* a, void* b);");
        wln!(self, "void* p_tensor_reshape(void* t, void* shape_vec);");
        wln!(self, "int64_t p_tensor_len(void* t);");
        wln!(self, "int64_t p_tensor_ndim(void* t);");
        wln!(self, "void* p_tensor_shape(void* t);");
        wln!(self, "int64_t p_tensor_size(void* t);");
        wln!(self, "char* p_tensor_to_str(void* t);");
        wln!(self, "char* p_tensor_to_json(void* t);");
        // JSON builtins (Phase 5D)
        wln!(self, "char* p_json_parse(const char* s);");
        wln!(self, "void* p_json_new_object(void);");
        wln!(self, "void* p_json_new_array(void);");
        wln!(self, "void p_json_set(void* j, const char* key, const char* val);");
        wln!(self, "void p_json_set_int(void* j, const char* key, int64_t val);");
        wln!(self, "void p_json_set_float(void* j, const char* key, double val);");
        wln!(self, "void p_json_set_bool(void* j, const char* key, int64_t val);");
        wln!(self, "void p_json_append(void* j, const char* val);");
        wln!(self, "void p_json_append_int(void* j, int64_t val);");
        wln!(self, "void p_json_append_float(void* j, double val);");
        wln!(self, "char* p_json_dump(void* j);");
        wln!(self, "char* p_json_dump_pretty(void* j);");
        wln!(self, "char* p_json_pretty(void* j);");
        wln!(self, "char* p_json_string(void* j);");
        wln!(self, "const char* p_json_type(void* j);");
        wln!(self, "char* p_json_get(void* j, const char* key);");
        wln!(self, "int64_t p_json_get_int(void* j, const char* key);");
        wln!(self, "double p_json_get_float(void* j, const char* key);");
        wln!(self, "int64_t p_json_get_bool(void* j, const char* key);");
        wln!(self, "void* p_json_at(void* j, int64_t idx);");
        wln!(self, "int64_t p_json_len(void* j);");
        wln!(self, "void* p_json_object_get(void* j, const char* key);");
        // HTTP builtins (Phase 5D) — already have socket builtins
        wln!(self, "int64_t p_http_listen(int64_t port);");
        wln!(self, "char* p_http_recv_request(int64_t fd);");
        wln!(self, "void p_http_send_response(int64_t fd, int64_t status, const char* body);");
        wln!(self, "char* p_http_build_response(int64_t status, const char* content_type, const char* body);");
        wln!(self, "char* p_http_parse_request(const char* request);");
        // DB builtins (Phase 5D)
        wln!(self, "void* p_db_connect(const char* conn_str);");
        wln!(self, "void p_db_exec(void* db, const char* query);");
        wln!(self, "void* p_db_query(void* db, const char* query);");
        wln!(self, "char* p_db_get(void* db, const char* key);");
        wln!(self, "void p_db_set(void* db, const char* key, const char* val);");
        wln!(self, "void* p_db_mem_open(void);");
        wln!(self, "int64_t p_db_mem_exec(void* db, const char* query);");
        wln!(self, "char* p_db_mem_query(void* db, const char* query);");
        wln!(self, "void p_db_mem_close(void* db);");
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
        // Allocator builtins (Phase 2A.8)
        wln!(self, "void* p_allocator_create(void* arg);");
        wln!(self, "int64_t p_allocator_alloc(void* alloc, int64_t size);");
        wln!(self, "void p_allocator_free(void* alloc, int64_t ptr);");
        wln!(self, "int64_t p_allocator_stats(void* alloc);");
        // Socket builtins (native HTTP server, Phase 5D)
        wln!(self, "int64_t p_socket_listen(int64_t port);");
        wln!(self, "int64_t p_socket_accept(int64_t fd);");
        wln!(self, "int64_t p_socket_read(int64_t fd, char* buf, int64_t max_len);");
        wln!(self, "int64_t p_socket_write(int64_t fd, const char* buf, int64_t len);");
        wln!(self, "int64_t p_socket_write_str(int64_t fd, const char* buf);");
        wln!(self, "void p_socket_close(int64_t fd);");
        wln!(self, "char* p_socket_recv_str(int64_t fd);");
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
            let proto_name = self.sanitize_c_name(name);
            let ret_ty = if name == "main" { "int64_t".to_string() } else { Self::c_ret_type(&func.ret_ty) };
            let params: Vec<String> = func
                .params
                .iter()
                .map(|(id, ty)| Self::c_param_decl(ty, &format!("v{}", id.0)))
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
        let c_name = self.sanitize_c_name(name);

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
            .map(|(id, ty)| Self::c_param_decl(ty, &format!("v{}", id.0)))
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
                        // If the C function returns void, discard the value
                        if self.cur_fn_c_ret_is_void {
                            wln!(self, "    return;");
                        } else {
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
                // Special-case: p_socket_write is called with 2 args (fd, str) by stdlib
                // but the C implementation needs 3 args (fd, buf, len)
                if fname == "p_socket_write" && args.len() == 2 {
                    let arg0 = self.val_name(args[0]);
                    let arg1 = self.val_name(args[1]);
                    wln!(self, "    {} = p_socket_write_str({}, {});", dst, arg0, arg1);
                    return;
                }
                let c_fname = self.sanitize_c_name(fname);
                // Look up the called function's parameter types so we can
                // cast arguments to the correct C type (e.g. the hidden
                // context pointer is passed as I64 0 but the parameter is
                // void*).
                // Check runtime function signatures first, then user functions.
                let param_types = self.fn_param_types.get(fname)
                    .or_else(|| self.user_fn_param_types.get(fname));
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
                    let c_fname = self.sanitize_c_name(fname);
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
            Inst::MakeDyn { value, type_name, trait_name, .. } => {
                let val_str = self.val_name(*value);
                // Represent dyn as a malloc'd struct: { char* type_name; void* ptr; }
                // Use raw push_str to avoid {{ escaping in inline struct definition
                self.out.push_str(&format!("    {{ /* make dyn {} as {} */\n", type_name, trait_name));
                self.out.push_str("        struct { const char* type_name; void* ptr; }* _dyn = malloc(sizeof(*_dyn));\n");
                self.out.push_str(&format!("        _dyn->type_name = \"{}\";\n", type_name));
                self.out.push_str(&format!("        _dyn->ptr = (void*){};\n", val_str));
                self.out.push_str(&format!("        {} = (void*)_dyn;\n", dst));
                self.out.push_str("    }\n");
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
                    // The first arg is a dyn object: { const char* type_name; void* ptr; }
                    dispatch.push_str(&format!("    {{ struct {{ const char* type_name; void* ptr; }}* _dyn = (void*){};\n", type_arg));
                    dispatch.push_str("    const char* _t = _dyn ? _dyn->type_name : \"\";\n");
                    dispatch.push_str("    void* _recv = _dyn ? _dyn->ptr : NULL;\n");
                    dispatch.push_str("    if (false) {}\n");
                    for cname in &candidates {
                        let type_name = cname.rsplitn(2, '_').nth(1).unwrap_or("");
                        // Pass NULL (ctx) + receiver + remaining args
                        let mut call_args = vec!["NULL".to_string(), "_recv".to_string()];
                        call_args.extend(arg_strs[1..].iter().cloned());
                        let call_args_str = call_args.join(", ");
                        if void_ret {
                            dispatch.push_str(&format!(
                                "    else if (strcmp(_t, \"{}\") == 0) {}({});\n",
                                type_name, cname, call_args_str
                            ));
                        } else {
                            dispatch.push_str(&format!(
                                "    else if (strcmp(_t, \"{}\") == 0) {} = {}({});\n",
                                type_name, dst, cname, call_args_str
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
            Inst::Print { value, ty, .. } => {
                let v = self.val_name(*value);
                // Dispatch based on type — print does NOT add a newline
                match ty {
                    IrType::F64 => {
                        wln!(self, "    p_print_float({});", v);
                    }
                    IrType::Bool => {
                        wln!(self, "    p_print_bool({});", v);
                    }
                    IrType::Ptr(_) => {
                        wln!(self, "    p_print_str({});", v);
                    }
                    IrType::Generic(name, _) if name == "Vec" => {
                        wln!(self, "    {{ char* _s = p_to_str_vec((PVec*){}); p_print_str(_s); free(_s); }}", v);
                    }
                    IrType::Generic(name, _) if name == "HashMap" => {
                        wln!(self, "    {{ char* _s = p_to_str_map((PMap*){}); p_print_str(_s); free(_s); }}", v);
                    }
                    IrType::Opaque(name) if name == "String" => {
                        wln!(self, "    p_print_str({});", v);
                    }
                    _ => {
                        wln!(self, "    p_print_int((int64_t){});", v);
                    }
                }
            }
            Inst::Panic { message, .. } => {
                wln!(self, "    fprintf(stderr, \"Panic: {}\\n\"); abort();", message);
            }
            Inst::Asm { template, .. } => {
                // C backend: emit inline assembly using GCC __asm__ syntax
                let escaped = template.replace("\\", "\\\\").replace("\"", "\\\"");
                wln!(self, "    __asm__ volatile(\"{}\");", escaped);
            }
            Inst::RegionBegin { .. } | Inst::RegionEnd { .. } => {
                // Region begin/end — no-op in C backend (C manages its own memory)
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
        wln!(self, "void p_vec_set(PVec* v, int64_t idx, void* val) {{");
        wln!(self, "    if (!v || idx < 0 || idx >= v->len) return;");
        wln!(self, "    v->data[idx] = val;");
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
        wln!(self, "void p_print(int64_t val) { printf(\"%lld\", (long long)val); }");
        wln!(self, "void p_print_str(const char* s) { printf(\"%s\", s); }");
        wln!(self, "void p_print_int(int64_t val) { printf(\"%lld\", (long long)val); }");
        wln!(self, "void p_print_bool(bool val) { printf(\"%s\", val ? \"true\" : \"false\"); }");
        wln!(self, "void p_print_float(double val) { printf(\"%g\", val); }");
        wln!(self, "void p_print_newline(void) { printf(\"\\n\"); }");
        wln!(self, "void p_panic(const char* msg) { fprintf(stderr, \"%s\\n\", msg); abort(); }");
        wln!(self, "void* p_malloc(int64_t size) { return malloc((size_t)size); }");
        wln!(self, "void p_free(void* ptr) { free(ptr); }");
        // Allocator builtins (Phase 2A.8) — simplified stubs
        wln!(self, "void* p_allocator_create(void* arg) { (void)arg; return (void*)1; }");
        wln!(self, "int64_t p_allocator_alloc(void* alloc, int64_t size) { (void)alloc; return (int64_t)(intptr_t)malloc((size_t)size); }");
        wln!(self, "void p_allocator_free(void* alloc, int64_t ptr) { (void)alloc; free((void*)(intptr_t)ptr); }");
        wln!(self, "int64_t p_allocator_stats(void* alloc) { (void)alloc; return 0; }");
        wln!(self, "int64_t print(int64_t val) { printf(\"%lld\", (long long)val); return 0; }");
        wln!(self, "int64_t print_str(const char* s) { printf(\"%s\", s); return 0; }");
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
        wln!(self, "    if (!m) return strdup(\"{}\");");
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

        // ─── Socket builtins (native HTTP server, Phase 5D) ───
        wln!(self, "");
        wln!(self, "// Socket operations — native POSIX/Winsock implementation");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "static int ws_initialized = 0;");
        wln!(self, "static void init_winsock(void) {");
        wln!(self, "    if (!ws_initialized) { WSADATA d; WSAStartup(MAKEWORD(2,2), &d); ws_initialized = 1; }");
        wln!(self, "}");
        wln!(self, "#endif");
        wln!(self, "");
        wln!(self, "int64_t p_socket_listen(int64_t port) {");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "    init_winsock();");
        wln!(self, "    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);");
        wln!(self, "    if (s == INVALID_SOCKET) return -1;");
        wln!(self, "#else");
        wln!(self, "    int s = socket(AF_INET, SOCK_STREAM, 0);");
        wln!(self, "    if (s < 0) return -1;");
        wln!(self, "#endif");
        wln!(self, "    int opt = 1;");
        wln!(self, "    setsockopt((int)s, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));");
        wln!(self, "    struct sockaddr_in addr;");
        wln!(self, "    memset(&addr, 0, sizeof(addr));");
        wln!(self, "    addr.sin_family = AF_INET;");
        wln!(self, "    addr.sin_addr.s_addr = INADDR_ANY;");
        wln!(self, "    addr.sin_port = htons((int)port);");
        wln!(self, "    if (bind((int)s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "        closesocket(s);");
        wln!(self, "#else");
        wln!(self, "        close(s);");
        wln!(self, "#endif");
        wln!(self, "        return -1;");
        wln!(self, "    }");
        wln!(self, "    if (listen((int)s, 10) < 0) {");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "        closesocket(s);");
        wln!(self, "#else");
        wln!(self, "        close(s);");
        wln!(self, "#endif");
        wln!(self, "        return -1;");
        wln!(self, "    }");
        wln!(self, "    return (int64_t)s;");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "int64_t p_socket_accept(int64_t fd) {");
        wln!(self, "    struct sockaddr_in client;");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "    int clen = sizeof(client);");
        wln!(self, "    SOCKET c = accept((SOCKET)fd, (struct sockaddr*)&client, &clen);");
        wln!(self, "    return (c == INVALID_SOCKET) ? -1 : (int64_t)c;");
        wln!(self, "#else");
        wln!(self, "    socklen_t clen = sizeof(client);");
        wln!(self, "    int c = accept((int)fd, (struct sockaddr*)&client, &clen);");
        wln!(self, "    return (int64_t)c;");
        wln!(self, "#endif");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "int64_t p_socket_read(int64_t fd, char* buf, int64_t max_len) {");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "    int n = recv((SOCKET)fd, buf, (int)max_len, 0);");
        wln!(self, "#else");
        wln!(self, "    ssize_t n = read((int)fd, buf, (size_t)max_len);");
        wln!(self, "#endif");
        wln!(self, "    return (n < 0) ? -1 : (int64_t)n;");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "int64_t p_socket_write(int64_t fd, const char* buf, int64_t len) {");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "    int n = send((SOCKET)fd, buf, (int)len, 0);");
        wln!(self, "#else");
        wln!(self, "    ssize_t n = write((int)fd, buf, (size_t)len);");
        wln!(self, "#endif");
        wln!(self, "    return (n < 0) ? -1 : (int64_t)n;");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "int64_t p_socket_write_str(int64_t fd, const char* buf) {");
        wln!(self, "    return p_socket_write(fd, buf, strlen(buf));");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "void p_socket_close(int64_t fd) {");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "    closesocket((SOCKET)fd);");
        wln!(self, "#else");
        wln!(self, "    close((int)fd);");
        wln!(self, "#endif");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "char* p_socket_recv_str(int64_t fd) {");
        wln!(self, "    char buf[4096];");
        wln!(self, "    int64_t n = p_socket_read(fd, buf, sizeof(buf)-1);");
        wln!(self, "    if (n <= 0) return strdup(\"\");");
        wln!(self, "    buf[n] = 0;");
        wln!(self, "    return strdup(buf);");
        wln!(self, "}");

        // ─── Bare metal builtins (Phase 5A) — stubs for C codegen ───
        wln!(self, "");
        wln!(self, "// Bare metal stubs — no-ops in C codegen (use --bare for real hardware)");
        wln!(self, "void p_cli(void) {{}}");
        wln!(self, "void p_sti(void) {{}}");
        wln!(self, "void p_hlt(void) {{}}");
        wln!(self, "int64_t p_cr0_read(void) {{ return 0; }}");
        wln!(self, "int64_t p_cr3_read(void) {{ return 0; }}");
        wln!(self, "int64_t p_cr4_read(void) {{ return 0; }}");
        wln!(self, "void p_cr3_write(int64_t val) {{ (void)val; }}");
        wln!(self, "void p_invlpg(int64_t addr) {{ (void)addr; }}");
        wln!(self, "void p_page_map(int64_t cr3, int64_t virt, int64_t phys, int64_t flags) {{ (void)cr3; (void)virt; (void)phys; (void)flags; }}");
        wln!(self, "void p_page_unmap(int64_t cr3, int64_t virt) {{ (void)cr3; (void)virt; }}");
        wln!(self, "static int64_t _phys_mem[65536];");
        wln!(self, "int64_t p_phys_read64(int64_t addr) {{ return _phys_mem[(addr/8) & 65535]; }}");
        wln!(self, "void p_phys_write64(int64_t addr, int64_t val) {{ _phys_mem[(addr/8) & 65535] = val; }}");

        // ─── SIMD builtins (Phase 5C) — operate on Vec<f64> ───
        wln!(self, "");
        wln!(self, "// SIMD builtins — operate on PVec (Vec<f64>)");
        wln!(self, "// Helper: read double from PVec element (stored as void* via type punning)");
        self.out.push_str("static double _pvec_get_f64(PVec* v, int i) {\n");
        self.out.push_str("    if (!v || i < 0 || i >= v->len) return 0;\n");
        self.out.push_str("    union { void* p; double d; } u; u.p = v->data[i]; return u.d;\n");
        self.out.push_str("}\n");
        self.out.push_str("static void _pvec_push_f64(PVec* v, double d) {\n");
        self.out.push_str("    union { void* p; double d; } u; u.d = d; p_vec_push(v, u.p);\n");
        self.out.push_str("}\n");
        wln!(self, "void* p_simd_add(void* a, void* b) {{");
        wln!(self, "    PVec* va = (PVec*)a; PVec* vb = (PVec*)b; PVec* r = p_vec_new();");
        wln!(self, "    int n = va ? va->len : 0;");
        wln!(self, "    for (int i = 0; i < n; i++) _pvec_push_f64(r, _pvec_get_f64(va,i) + _pvec_get_f64(vb,i));");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_simd_sub(void* a, void* b) {{");
        wln!(self, "    PVec* va = (PVec*)a; PVec* vb = (PVec*)b; PVec* r = p_vec_new();");
        wln!(self, "    int n = va ? va->len : 0;");
        wln!(self, "    for (int i = 0; i < n; i++) _pvec_push_f64(r, _pvec_get_f64(va,i) - _pvec_get_f64(vb,i));");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_simd_mul(void* a, void* b) {{");
        wln!(self, "    PVec* va = (PVec*)a; PVec* vb = (PVec*)b; PVec* r = p_vec_new();");
        wln!(self, "    int n = va ? va->len : 0;");
        wln!(self, "    for (int i = 0; i < n; i++) _pvec_push_f64(r, _pvec_get_f64(va,i) * _pvec_get_f64(vb,i));");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_simd_div(void* a, void* b) {{");
        wln!(self, "    PVec* va = (PVec*)a; PVec* vb = (PVec*)b; PVec* r = p_vec_new();");
        wln!(self, "    int n = va ? va->len : 0;");
        wln!(self, "    for (int i = 0; i < n; i++) {{ double db = _pvec_get_f64(vb,i); _pvec_push_f64(r, db != 0 ? _pvec_get_f64(va,i) / db : 0); }}");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "double p_simd_dot(void* a, void* b) {{");
        wln!(self, "    PVec* va = (PVec*)a; PVec* vb = (PVec*)b;");
        wln!(self, "    int n = va ? va->len : 0; double s = 0;");
        wln!(self, "    for (int i = 0; i < n; i++) s += _pvec_get_f64(va,i) * _pvec_get_f64(vb,i);");
        wln!(self, "    return s;");
        wln!(self, "}}");
        wln!(self, "double p_simd_sum(void* a) {{");
        wln!(self, "    PVec* va = (PVec*)a;");
        wln!(self, "    int n = va ? va->len : 0; double s = 0;");
        wln!(self, "    for (int i = 0; i < n; i++) s += _pvec_get_f64(va,i);");
        wln!(self, "    return s;");
        wln!(self, "}}");
        wln!(self, "void* p_simd_scale(void* a, double s) {{");
        wln!(self, "    PVec* va = (PVec*)a; PVec* r = p_vec_new();");
        wln!(self, "    int n = va ? va->len : 0;");
        wln!(self, "    for (int i = 0; i < n; i++) _pvec_push_f64(r, _pvec_get_f64(va,i) * s);");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_simd_from_vec(void* v) {{ (void)v; return p_vec_new(); }}");
        wln!(self, "char* p_simd_to_str(void* a) {{");
        wln!(self, "    PVec* va = (PVec*)a;");
        wln!(self, "    if (!va) return strdup(\"[]\");");
        wln!(self, "    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;");
        wln!(self, "    result[pos++] = '[';");
        wln!(self, "    for (int64_t i = 0; i < va->len; i++) {{");
        wln!(self, "        if (i > 0) {{ if (pos+2>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; result[pos++]=' '; }}");
        wln!(self, "        double d = _pvec_get_f64(va, i);");
        wln!(self, "        char buf[32];");
        wln!(self, "        if (d == (int64_t)d) snprintf(buf,sizeof(buf),\"%lld\",(long long)d);");
        wln!(self, "        else snprintf(buf,sizeof(buf),\"%g\",d);");
        wln!(self, "        size_t bl=strlen(buf);");
        wln!(self, "        if (pos+bl+1>=cap){{cap=(cap+bl)*2;result=(char*)realloc(result,cap);}}");
        wln!(self, "        memcpy(result+pos,buf,bl); pos+=bl;");
        wln!(self, "    }}");
        wln!(self, "    if (pos+2>=cap){{cap+=2;result=(char*)realloc(result,cap);}}");
        wln!(self, "    result[pos++]=']'; result[pos]=0;");
        wln!(self, "    return result;");
        wln!(self, "}}");

        // ─── Tensor builtins (Phase 5C) ───
        wln!(self, "");
        wln!(self, "// Tensor builtins — simple flat array with shape metadata");
        wln!(self, "typedef struct { double* data; int64_t len; int64_t ndim; int64_t shape[8]; } PTensor;");
        wln!(self, "void* p_tensor_new(int64_t ndim, int64_t* shape) {{");
        wln!(self, "    PTensor* t = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    t->ndim = ndim;");
        wln!(self, "    t->len = 1;");
        wln!(self, "    for (int64_t i = 0; i < ndim && i < 8; i++) {{ t->shape[i] = shape[i]; t->len *= shape[i]; }}");
        wln!(self, "    t->data = (double*)calloc(t->len, sizeof(double));");
        wln!(self, "    return t;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_from(void* data, int64_t ndim, int64_t* shape) {{");
        wln!(self, "    (void)data;");
        wln!(self, "    return p_tensor_new(ndim, shape);");
        wln!(self, "}}");
        wln!(self, "double p_tensor_at(void* t, int64_t idx) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    if (!pt || idx < 0 || idx >= pt->len) return 0;");
        wln!(self, "    return pt->data[idx];");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_add(void* a, void* b) {{");
        wln!(self, "    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;");
        wln!(self, "    if (!ta || !tb) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = ta->ndim; r->len = ta->len;");
        wln!(self, "    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = ta->shape[i];");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < r->len; i++) r->data[i] = ta->data[i] + tb->data[i];");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_mul(void* a, void* b) {{");
        wln!(self, "    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;");
        wln!(self, "    if (!ta || !tb) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = ta->ndim; r->len = ta->len;");
        wln!(self, "    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = ta->shape[i];");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < r->len; i++) r->data[i] = ta->data[i] * tb->data[i];");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_matmul(void* a, void* b) {{");
        wln!(self, "    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;");
        wln!(self, "    if (!ta || !tb || ta->ndim < 2 || tb->ndim < 2) return NULL;");
        wln!(self, "    int64_t M = ta->shape[0], K = ta->shape[1], N = tb->shape[1];");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = 2; r->len = M * N; r->shape[0] = M; r->shape[1] = N;");
        wln!(self, "    r->data = (double*)calloc(r->len, sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < M; i++)");
        wln!(self, "        for (int64_t j = 0; j < N; j++)");
        wln!(self, "            for (int64_t k = 0; k < K; k++)");
        wln!(self, "                r->data[i*N+j] += ta->data[i*K+k] * tb->data[k*N+j];");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_transpose(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    if (!pt || pt->ndim < 2) return t;");
        wln!(self, "    int64_t M = pt->shape[0], N = pt->shape[1];");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = 2; r->len = M * N; r->shape[0] = N; r->shape[1] = M;");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < M; i++)");
        wln!(self, "        for (int64_t j = 0; j < N; j++)");
        wln!(self, "            r->data[j*M+i] = pt->data[i*N+j];");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_relu(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    if (!pt) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = pt->ndim; r->len = pt->len;");
        wln!(self, "    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < r->len; i++) r->data[i] = pt->data[i] > 0 ? pt->data[i] : 0;");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_softmax(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    if (!pt) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = pt->ndim; r->len = pt->len;");
        wln!(self, "    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    double max = pt->data[0];");
        wln!(self, "    for (int64_t i = 1; i < pt->len; i++) if (pt->data[i] > max) max = pt->data[i];");
        wln!(self, "    double sum = 0;");
        wln!(self, "    for (int64_t i = 0; i < pt->len; i++) {{ r->data[i] = exp(pt->data[i] - max); sum += r->data[i]; }}");
        wln!(self, "    for (int64_t i = 0; i < pt->len; i++) r->data[i] /= sum;");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "int64_t p_tensor_len(void* t) {{ PTensor* pt = (PTensor*)t; return pt ? pt->len : 0; }}");
        wln!(self, "int64_t p_tensor_ndim(void* t) {{ PTensor* pt = (PTensor*)t; return pt ? pt->ndim : 0; }}");
        wln!(self, "void* p_tensor_shape(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    PVec* v = (PVec*)p_vec_new();");
        wln!(self, "    if (pt) for (int64_t i = 0; i < pt->ndim; i++) {{ int64_t d = pt->shape[i]; p_vec_push(v, (void*)d); }}");
        wln!(self, "    return v;");
        wln!(self, "}}");
        wln!(self, "int64_t p_tensor_size(void* t) {{ return p_tensor_len(t); }}");
        wln!(self, "void* p_tensor_zeros(void* shape_vec) {{");
        wln!(self, "    PVec* sv = (PVec*)shape_vec;");
        wln!(self, "    if (!sv) return p_tensor_new(0, NULL);");
        wln!(self, "    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;");
        wln!(self, "    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];");
        wln!(self, "    return p_tensor_new(ndim, shape);");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_ones(void* shape_vec) {{");
        wln!(self, "    PVec* sv = (PVec*)shape_vec;");
        wln!(self, "    if (!sv) return p_tensor_new(0, NULL);");
        wln!(self, "    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;");
        wln!(self, "    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];");
        wln!(self, "    PTensor* t = (PTensor*)p_tensor_new(ndim, shape);");
        wln!(self, "    if (t) for (int64_t i = 0; i < t->len; i++) t->data[i] = 1.0;");
        wln!(self, "    return t;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_filled(void* shape_vec, double val) {{");
        wln!(self, "    PVec* sv = (PVec*)shape_vec;");
        wln!(self, "    if (!sv) return p_tensor_new(0, NULL);");
        wln!(self, "    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;");
        wln!(self, "    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];");
        wln!(self, "    PTensor* t = (PTensor*)p_tensor_new(ndim, shape);");
        wln!(self, "    if (t) for (int64_t i = 0; i < t->len; i++) t->data[i] = val;");
        wln!(self, "    return t;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_from_vec(void* v, void* shape_vec) {{");
        wln!(self, "    PVec* sv = (PVec*)shape_vec;");
        wln!(self, "    if (!sv) return p_tensor_new(0, NULL);");
        wln!(self, "    int64_t shape[8]; int64_t ndim = sv->len < 8 ? sv->len : 8;");
        wln!(self, "    for (int64_t i = 0; i < ndim; i++) shape[i] = (int64_t)sv->data[i];");
        wln!(self, "    PTensor* t = (PTensor*)p_tensor_new(ndim, shape);");
        wln!(self, "    PVec* pv = (PVec*)v;");
        wln!(self, "    if (t && pv) for (int64_t i = 0; i < t->len && i < pv->len; i++) t->data[i] = (double)(int64_t)pv->data[i];");
        wln!(self, "    return t;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_clone(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    if (!pt) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = pt->ndim; r->len = pt->len;");
        wln!(self, "    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < r->len; i++) r->data[i] = pt->data[i];");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "double p_tensor_get(void* t, void* idx_vec) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    PVec* iv = (PVec*)idx_vec;");
        wln!(self, "    if (!pt || !iv || iv->len == 0) return 0;");
        wln!(self, "    int64_t idx = 0;");
        wln!(self, "    for (int64_t i = 0; i < pt->ndim && i < (int64_t)iv->len; i++) {{");
        wln!(self, "        int64_t dim = (int64_t)iv->data[i];");
        wln!(self, "        int64_t stride = 1;");
        wln!(self, "        for (int64_t j = i+1; j < pt->ndim; j++) stride *= pt->shape[j];");
        wln!(self, "        idx += dim * stride;");
        wln!(self, "    }}");
        wln!(self, "    if (idx < 0 || idx >= pt->len) return 0;");
        wln!(self, "    return pt->data[idx];");
        wln!(self, "}}");
        wln!(self, "void p_tensor_set(void* t, void* idx_vec, double val) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    PVec* iv = (PVec*)idx_vec;");
        wln!(self, "    if (!pt || !iv || iv->len == 0) return;");
        wln!(self, "    int64_t idx = 0;");
        wln!(self, "    for (int64_t i = 0; i < pt->ndim && i < (int64_t)iv->len; i++) {{");
        wln!(self, "        int64_t dim = (int64_t)iv->data[i];");
        wln!(self, "        int64_t stride = 1;");
        wln!(self, "        for (int64_t j = i+1; j < pt->ndim; j++) stride *= pt->shape[j];");
        wln!(self, "        idx += dim * stride;");
        wln!(self, "    }}");
        wln!(self, "    if (idx < 0 || idx >= pt->len) return;");
        wln!(self, "    pt->data[idx] = val;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_sub(void* a, void* b) {{");
        wln!(self, "    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;");
        wln!(self, "    if (!ta || !tb) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = ta->ndim; r->len = ta->len;");
        wln!(self, "    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = ta->shape[i];");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < r->len; i++) r->data[i] = ta->data[i] - tb->data[i];");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_sigmoid(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    if (!pt) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = pt->ndim; r->len = pt->len;");
        wln!(self, "    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < r->len; i++) r->data[i] = 1.0 / (1.0 + exp(-pt->data[i]));");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_tensor_scale(void* t, double s) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    if (!pt) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = pt->ndim; r->len = pt->len;");
        wln!(self, "    for (int64_t i = 0; i < r->ndim && i < 8; i++) r->shape[i] = pt->shape[i];");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < r->len; i++) r->data[i] = pt->data[i] * s;");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "double p_tensor_sum(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt) return 0; double s=0; for (int64_t i=0;i<pt->len;i++) s+=pt->data[i]; return s; }}");
        wln!(self, "double p_tensor_mean(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt || pt->len==0) return 0; return p_tensor_sum(t)/pt->len; }}");
        wln!(self, "double p_tensor_min(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt || pt->len==0) return 0; double m=pt->data[0]; for (int64_t i=1;i<pt->len;i++) if (pt->data[i]<m) m=pt->data[i]; return m; }}");
        wln!(self, "double p_tensor_max(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt || pt->len==0) return 0; double m=pt->data[0]; for (int64_t i=1;i<pt->len;i++) if (pt->data[i]>m) m=pt->data[i]; return m; }}");
        wln!(self, "double p_tensor_dot(void* a, void* b) {{ PTensor* ta=(PTensor*)a; PTensor* tb=(PTensor*)b; if (!ta||!tb) return 0; double s=0; for (int64_t i=0;i<ta->len&&i<tb->len;i++) s+=ta->data[i]*tb->data[i]; return s; }}");
        wln!(self, "void* p_tensor_reshape(void* t, void* shape_vec) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t; if (!pt) return NULL;");
        wln!(self, "    PVec* sv = (PVec*)shape_vec;");
        wln!(self, "    PTensor* r = (PTensor*)p_tensor_clone(t);");
        wln!(self, "    if (sv) {{");
        wln!(self, "        r->ndim = sv->len < 8 ? sv->len : 8; r->len = 1;");
        wln!(self, "        for (int64_t i = 0; i < r->ndim; i++) {{ r->shape[i] = (int64_t)sv->data[i]; r->len *= r->shape[i]; }}");
        wln!(self, "    }}");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "char* p_tensor_to_json(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    if (!pt) return strdup(\"{}\");");
        wln!(self, "    size_t cap = 128; char* result = (char*)malloc(cap); size_t pos = 0;");
        wln!(self, "    pos += snprintf(result+pos, cap-pos, \"{\\\"shape\\\":[\");");
        wln!(self, "    for (int64_t i = 0; i < pt->ndim; i++) {{");
        wln!(self, "        if (i > 0) pos += snprintf(result+pos, cap-pos, \",\");");
        wln!(self, "        pos += snprintf(result+pos, cap-pos, \"%lld\", (long long)pt->shape[i]);");
        wln!(self, "    }}");
        wln!(self, "    pos += snprintf(result+pos, cap-pos, \"],\\\"data\\\":[\");");
        wln!(self, "    for (int64_t i = 0; i < pt->len; i++) {{");
        wln!(self, "        if (i > 0) pos += snprintf(result+pos, cap-pos, \",\");");
        wln!(self, "        double d = pt->data[i];");
        wln!(self, "        if (d == (int64_t)d && d < 1e15 && d > -1e15) pos += snprintf(result+pos, cap-pos, \"%lld\", (long long)d);");
        wln!(self, "        else pos += snprintf(result+pos, cap-pos, \"%g\", d);");
        wln!(self, "    }}");
        wln!(self, "    pos += snprintf(result+pos, cap-pos, \"]}\");");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "char* p_tensor_to_str(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    if (!pt) return strdup(\"[]\");");
        wln!(self, "    size_t cap = 128; char* result = (char*)malloc(cap); size_t pos = 0;");
        wln!(self, "    if (pt->ndim <= 1) {{");
        wln!(self, "        result[pos++] = '[';");
        wln!(self, "        for (int64_t i = 0; i < pt->len; i++) {{");
        wln!(self, "            if (i > 0) {{ if (pos+2>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; result[pos++]=' '; }}");
        wln!(self, "            char buf[32]; snprintf(buf,sizeof(buf),\"%g\",pt->data[i]); size_t bl=strlen(buf);");
        wln!(self, "            if (pos+bl+1>=cap){{cap=(cap+bl)*2;result=(char*)realloc(result,cap);}}");
        wln!(self, "            memcpy(result+pos,buf,bl); pos+=bl;");
        wln!(self, "        }}");
        wln!(self, "        if (pos+2>=cap){{cap+=2;result=(char*)realloc(result,cap);}}");
        wln!(self, "        result[pos++]=']'; result[pos]=0;");
        wln!(self, "        return result;");
        wln!(self, "    }}");
        wln!(self, "    // 2D tensor: print as nested brackets");
        wln!(self, "    int64_t rows = pt->shape[0], cols = pt->ndim >= 2 ? pt->shape[1] : 1;");
        wln!(self, "    result[pos++] = '[';");
        wln!(self, "    for (int64_t r = 0; r < rows; r++) {{");
        wln!(self, "        if (r > 0) {{ if (pos+2>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; result[pos++]=' '; }}");
        wln!(self, "        if (pos+1>=cap){{cap+=2;result=(char*)realloc(result,cap);}} result[pos++]='[';");
        wln!(self, "        for (int64_t c = 0; c < cols; c++) {{");
        wln!(self, "            if (c > 0) {{ if (pos+2>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; result[pos++]=' '; }}");
        wln!(self, "            char buf[32]; snprintf(buf,sizeof(buf),\"%g\",pt->data[r*cols+c]); size_t bl=strlen(buf);");
        wln!(self, "            if (pos+bl+1>=cap){{cap=(cap+bl)*2;result=(char*)realloc(result,cap);}}");
        wln!(self, "            memcpy(result+pos,buf,bl); pos+=bl;");
        wln!(self, "        }}");
        wln!(self, "        if (pos+1>=cap){{cap+=2;result=(char*)realloc(result,cap);}} result[pos++]=']';");
        wln!(self, "    }}");
        wln!(self, "    if (pos+2>=cap){{cap+=2;result=(char*)realloc(result,cap);}}");
        wln!(self, "    result[pos++]=']'; result[pos]=0;");
        wln!(self, "    return result;");
        wln!(self, "}}");

        // ─── JSON builtins (Phase 5D) — simple JSON implementation ───
        wln!(self, "");
        wln!(self, "// JSON builtins — simple key-value store");
        wln!(self, "typedef struct { char** keys; char** vals; int64_t len; int64_t cap; int64_t is_array; } PJson;");
        wln!(self, "static char* _json_dump_obj(PJson* pj); // forward decl");
        wln!(self, "void _json_append(PJson* j, const char* key, const char* val) {{");
        wln!(self, "    if (j->len >= j->cap) {{ j->cap = j->cap == 0 ? 8 : j->cap * 2; j->keys = (char**)realloc(j->keys, j->cap * sizeof(char*)); j->vals = (char**)realloc(j->vals, j->cap * sizeof(char*)); }}");
        wln!(self, "    j->keys[j->len] = key ? strdup(key) : strdup(\"\"); j->vals[j->len] = val ? strdup(val) : strdup(\"\"); j->len++;");
        wln!(self, "}}");
        wln!(self, "char* p_json_parse(const char* s) {{");
        wln!(self, "    if (!s) return strdup(\"\");");
        wln!(self, "    // Simple JSON parser: extracts string and number values");
        wln!(self, "    PJson* j = (PJson*)p_json_new_object();");
        wln!(self, "    const char* p = s;");
        wln!(self, "    while (*p) {{");
        wln!(self, "        // Skip whitespace and structural chars");
        wln!(self, "        while (*p && *p != '\"' && *p != '-' && (*p < '0' || *p > '9') && *p != 't' && *p != 'f' && *p != 'n') p++;");
        wln!(self, "        if (!*p) break;");
        wln!(self, "        // Try to parse a key (string followed by colon)");
        wln!(self, "        if (*p == '\"' && p > s && p[-1] != ':') {{");
        wln!(self, "            p++; const char* kstart = p;");
        wln!(self, "            while (*p && *p != '\"') p++;");
        wln!(self, "            size_t klen = p - kstart;");
        wln!(self, "            if (*p) p++;");
        wln!(self, "            // Skip whitespace and colon");
        wln!(self, "            while (*p && *p != ':') p++;");
        wln!(self, "            if (*p) p++;");
        wln!(self, "            while (*p == ' ' || *p == '\\t') p++;");
        wln!(self, "            // Parse value");
        wln!(self, "            char* key = (char*)malloc(klen+1); memcpy(key, kstart, klen); key[klen] = 0;");
        wln!(self, "            if (*p == '\"') {{");
        wln!(self, "                p++; const char* vstart = p;");
        wln!(self, "                while (*p && *p != '\"') p++;");
        wln!(self, "                size_t vlen = p - vstart;");
        wln!(self, "                if (*p) p++;");
        wln!(self, "                char* val = (char*)malloc(vlen+1); memcpy(val, vstart, vlen); val[vlen] = 0;");
        wln!(self, "                _json_append(j, key, val); free(key); free(val);");
        wln!(self, "            }} else if (*p == 't') {{");
        wln!(self, "                _json_append(j, key, \"true\"); free(key); p += 4;");
        wln!(self, "            }} else if (*p == 'f') {{");
        wln!(self, "                _json_append(j, key, \"false\"); free(key); p += 5;");
        wln!(self, "            }} else if (*p == 'n') {{");
        wln!(self, "                _json_append(j, key, \"null\"); free(key); p += 4;");
        wln!(self, "            }} else {{");
        wln!(self, "                const char* vstart = p;");
        wln!(self, "                while (*p && *p != ',' && *p != '}' && *p != ']' && *p != ' ') p++;");
        wln!(self, "                size_t vlen = p - vstart;");
        wln!(self, "                char* val = (char*)malloc(vlen+1); memcpy(val, vstart, vlen); val[vlen] = 0;");
        wln!(self, "                _json_append(j, key, val); free(key); free(val);");
        wln!(self, "            }}");
        wln!(self, "        }} else {{");
        wln!(self, "            p++;");
        wln!(self, "        }}");
        wln!(self, "    }}");
        wln!(self, "    char* result = _json_dump_obj(j);");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "void* p_json_new_object(void) {{");
        wln!(self, "    PJson* j = (PJson*)malloc(sizeof(PJson));");
        wln!(self, "    j->keys = NULL; j->vals = NULL; j->len = 0; j->cap = 0; j->is_array = 0;");
        wln!(self, "    return j;");
        wln!(self, "}}");
        wln!(self, "void* p_json_new_array(void) {{");
        wln!(self, "    PJson* j = (PJson*)p_json_new_object(); j->is_array = 1; return j;");
        wln!(self, "}}");
        wln!(self, "void p_json_set(void* j, const char* key, const char* val) {{ _json_append((PJson*)j, key, val); }}");
        wln!(self, "void p_json_set_int(void* j, const char* key, int64_t val) {{ char buf[32]; snprintf(buf,sizeof(buf),\"%lld\",(long long)val); _json_append((PJson*)j, key, buf); }}");
        wln!(self, "void p_json_set_float(void* j, const char* key, double val) {{ char buf[32]; snprintf(buf,sizeof(buf),\"%g\",val); _json_append((PJson*)j, key, buf); }}");
        wln!(self, "void p_json_set_bool(void* j, const char* key, int64_t val) {{ _json_append((PJson*)j, key, val ? \"true\" : \"false\"); }}");
        wln!(self, "void p_json_append(void* j, const char* val) {{ _json_append((PJson*)j, NULL, val); }}");
        wln!(self, "void p_json_append_int(void* j, int64_t val) {{ char buf[32]; snprintf(buf,sizeof(buf),\"%lld\",(long long)val); _json_append((PJson*)j, NULL, buf); }}");
        wln!(self, "void p_json_append_float(void* j, double val) {{ char buf[32]; snprintf(buf,sizeof(buf),\"%g\",val); _json_append((PJson*)j, NULL, buf); }}");
        wln!(self, "// Internal: dump PJson object to string (sorted keys, no quotes on numbers/bools)");
        wln!(self, "static char* _json_dump_obj(PJson* pj) {{");
        wln!(self, "    if (!pj) return strdup(\"{}\");");
        wln!(self, "    // Sort keys (simple insertion sort) for objects");
        wln!(self, "    if (!pj->is_array) {{");
        wln!(self, "        for (int64_t i = 1; i < pj->len; i++) {{");
        wln!(self, "            char* k = pj->keys[i]; char* v = pj->vals[i];");
        wln!(self, "            int64_t j = i - 1;");
        wln!(self, "            while (j >= 0 && strcmp(pj->keys[j], k) > 0) {{ pj->keys[j+1] = pj->keys[j]; pj->vals[j+1] = pj->vals[j]; j--; }}");
        wln!(self, "            pj->keys[j+1] = k; pj->vals[j+1] = v;");
        wln!(self, "        }}");
        wln!(self, "    }}");
        wln!(self, "    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;");
        wln!(self, "    result[pos++] = pj->is_array ? '[' : '{';");
        wln!(self, "    for (int64_t i = 0; i < pj->len; i++) {{");
        wln!(self, "        if (i > 0) {{ if (pos+1>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; }}");
        wln!(self, "        if (!pj->is_array) {{ size_t kl=strlen(pj->keys[i]); if (pos+kl+4>=cap){{cap=(cap+kl)*2;result=(char*)realloc(result,cap);}} result[pos++]='\"'; memcpy(result+pos,pj->keys[i],kl); pos+=kl; result[pos++]='\"'; result[pos++]=':'; }}");
        wln!(self, "        // Don't quote numbers, booleans, null");
        wln!(self, "        const char* v = pj->vals[i];");
        wln!(self, "        bool is_numeric = (*v >= '0' && *v <= '9') || *v == '-';");
        wln!(self, "        bool is_bool = strcmp(v, \"true\") == 0 || strcmp(v, \"false\") == 0;");
        wln!(self, "        bool is_null = strcmp(v, \"null\") == 0;");
        wln!(self, "        bool need_quotes = !is_numeric && !is_bool && !is_null;");
        wln!(self, "        size_t vl=strlen(v); if (pos+vl+3>=cap){{cap=(cap+vl)*2;result=(char*)realloc(result,cap);}}");
        wln!(self, "        if (need_quotes) {{ result[pos++]='\"'; memcpy(result+pos,v,vl); pos+=vl; result[pos++]='\"'; }}");
        wln!(self, "        else {{ memcpy(result+pos,v,vl); pos+=vl; }}");
        wln!(self, "    }}");
        wln!(self, "    if (pos+2>=cap){{cap+=2;result=(char*)realloc(result,cap);}}");
        wln!(self, "    result[pos++] = pj->is_array ? ']' : '}'; result[pos] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "// User-facing: parse JSON string and re-dump in canonical form");
        wln!(self, "char* p_json_dump(void* j) {{");
        wln!(self, "    return p_json_parse((const char*)j);");
        wln!(self, "}}");
        wln!(self, "char* p_json_dump_pretty(void* j) {{ return p_json_dump(j); }}");
        wln!(self, "char* p_json_pretty(void* j) {{");
        wln!(self, "    // Pretty-print with 2-space indentation (canonicalize first)");
        wln!(self, "    char* canonical = p_json_parse((const char*)j);");
        wln!(self, "    const char* s = canonical;");
        wln!(self, "    if (!s) return strdup(\"\");");
        wln!(self, "    size_t cap = strlen(s) * 4 + 64; char* result = (char*)malloc(cap); size_t pos = 0;");
        wln!(self, "    int indent = 0; bool in_str = false;");
        wln!(self, "    while (*s) {{");
        wln!(self, "        if (*s == '\"' && (s == (const char*)j || s[-1] != '\\\\')) in_str = !in_str;");
        wln!(self, "        if (!in_str) {{");
        wln!(self, "            if (*s == '{' || *s == '[') {{");
        wln!(self, "                result[pos++] = *s++; result[pos++] = '\\n';");
        wln!(self, "                indent++;");
        wln!(self, "                for (int i = 0; i < indent*2; i++) result[pos++] = ' ';");
        wln!(self, "                continue;");
        wln!(self, "            }}");
        wln!(self, "            if (*s == '}' || *s == ']') {{");
        wln!(self, "                result[pos++] = '\\n'; indent--;");
        wln!(self, "                for (int i = 0; i < indent*2; i++) result[pos++] = ' ';");
        wln!(self, "                result[pos++] = *s++;");
        wln!(self, "                continue;");
        wln!(self, "            }}");
        wln!(self, "            if (*s == ',') {{");
        wln!(self, "                result[pos++] = *s++; result[pos++] = '\\n';");
        wln!(self, "                for (int i = 0; i < indent*2; i++) result[pos++] = ' ';");
        wln!(self, "                continue;");
        wln!(self, "            }}");
        wln!(self, "            if (*s == ':') {{ result[pos++] = *s++; result[pos++] = ' '; continue; }}");
        wln!(self, "        }}");
        wln!(self, "        if (pos >= cap-1) {{ cap *= 2; result = (char*)realloc(result, cap); }}");
        wln!(self, "        result[pos++] = *s++;");
        wln!(self, "    }}");
        wln!(self, "    result[pos] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "char* p_json_string(void* j) {{");
        wln!(self, "    // Serialize a plain string as a JSON string literal (with quotes + escaping)");
        wln!(self, "    const char* s = (const char*)j;");
        wln!(self, "    if (!s) return strdup(\"\\\"\\\"\");");
        wln!(self, "    size_t cap = strlen(s) * 2 + 4; char* result = (char*)malloc(cap); size_t pos = 0;");
        wln!(self, "    result[pos++] = '\"';");
        wln!(self, "    while (*s) {{");
        wln!(self, "        if (*s == '\"' || *s == '\\\\') {{ if (pos >= cap-2) {{ cap *= 2; result = (char*)realloc(result, cap); }} result[pos++] = '\\\\'; }}");
        wln!(self, "        if (*s == '\\n') {{ if (pos >= cap-2) {{ cap *= 2; result = (char*)realloc(result, cap); }} result[pos++] = '\\\\'; result[pos++] = 'n'; s++; continue; }}");
        wln!(self, "        if (pos >= cap-1) {{ cap *= 2; result = (char*)realloc(result, cap); }}");
        wln!(self, "        result[pos++] = *s++;");
        wln!(self, "    }}");
        wln!(self, "    result[pos++] = '\"'; result[pos] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "const char* p_json_type(void* j) {{");
        wln!(self, "    const char* s = (const char*)j;");
        wln!(self, "    if (!s) return \"null\";");
        wln!(self, "    while (*s == ' ') s++;");
        wln!(self, "    if (*s == '{') return \"object\";");
        wln!(self, "    if (*s == '[') return \"array\";");
        wln!(self, "    if (*s == '\"') return \"string\";");
        wln!(self, "    if (*s == 't' || *s == 'f') return \"boolean\";");
        wln!(self, "    if (*s == 'n') return \"null\";");
        wln!(self, "    return \"number\";");
        wln!(self, "}}");
        wln!(self, "char* p_json_get(void* j, const char* key) {{");
        wln!(self, "    const char* s = (const char*)j;");
        wln!(self, "    if (!s || !key) return strdup(\"null\");");
        wln!(self, "    char pattern[256]; snprintf(pattern, sizeof(pattern), \"\\\"%s\\\":\", key);");
        wln!(self, "    const char* found = strstr(s, pattern);");
        wln!(self, "    if (!found) return strdup(\"null\");");
        wln!(self, "    found += strlen(pattern);");
        wln!(self, "    while (*found == ' ') found++;");
        wln!(self, "    // Return the raw JSON value (with quotes for strings, as json::dump would)");
        wln!(self, "    if (*found == '\"') {{");
        wln!(self, "        // String value — return with quotes (handle escaped quotes)");
        wln!(self, "        const char* end = found + 1;");
        wln!(self, "        while (*end) {{ if (*end == '\\\\' && end[1]) end += 2; else if (*end == '\"') break; else end++; }}");
        wln!(self, "        size_t len = end - found + 1; // include closing quote");
        wln!(self, "        char* result = (char*)malloc(len+1); memcpy(result, found, len); result[len] = 0;");
        wln!(self, "        return result;");
        wln!(self, "    }}");
        wln!(self, "    if (*found == '{' || *found == '[') {{");
        wln!(self, "        // Nested object/array — find matching close brace");
        wln!(self, "        char open = *found; char close = open == '{' ? '}' : ']';");
        wln!(self, "        int depth = 1; const char* end = found + 1;");
        wln!(self, "        while (*end && depth > 0) {{");
        wln!(self, "            if (*end == open) depth++;");
        wln!(self, "            if (*end == close) depth--;");
        wln!(self, "            if (depth > 0) end++;");
        wln!(self, "        }}");
        wln!(self, "        size_t len = end - found + 1;");
        wln!(self, "        char* result = (char*)malloc(len+1); memcpy(result, found, len); result[len] = 0;");
        wln!(self, "        // Canonicalize by parsing and re-dumping");
        wln!(self, "        char* canonical = p_json_parse(result); free(result);");
        wln!(self, "        return canonical;");
        wln!(self, "    }}");
        wln!(self, "    // Number, boolean, or null");
        wln!(self, "    const char* end = found;");
        wln!(self, "    while (*end && *end != ',' && *end != '}' && *end != ']' && *end != ' ') end++;");
        wln!(self, "    size_t len = end - found;");
        wln!(self, "    char* result = (char*)malloc(len+1); memcpy(result, found, len); result[len] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "int64_t p_json_get_int(void* j, const char* key) {{ char* v = p_json_get(j, key); int64_t r = atoll(v); free(v); return r; }}");
        wln!(self, "double p_json_get_float(void* j, const char* key) {{ char* v = p_json_get(j, key); double r = atof(v); free(v); return r; }}");
        wln!(self, "int64_t p_json_get_bool(void* j, const char* key) {{ char* v = p_json_get(j, key); int64_t r = strcmp(v, \"true\") == 0; free(v); return r; }}");
        wln!(self, "void* p_json_at(void* j, int64_t idx) {{");
        wln!(self, "    const char* s = (const char*)j;");
        wln!(self, "    if (!s || idx < 0) return strdup(\"null\");");
        wln!(self, "    // Find array start");
        wln!(self, "    while (*s && *s != '[') s++;");
        wln!(self, "    if (*s != '[') return strdup(\"null\");");
        wln!(self, "    s++; // skip '['");
        wln!(self, "    int64_t cur = 0;");
        wln!(self, "    while (*s) {{");
        wln!(self, "        while (*s == ' ' || *s == '\\t') s++;");
        wln!(self, "        if (*s == ']' || !*s) return strdup(\"null\");");
        wln!(self, "        if (cur == idx) {{");
        wln!(self, "            // Found the element — extract it");
        wln!(self, "            if (*s == '\"') {{");
        wln!(self, "                const char* end = s + 1;");
        wln!(self, "                while (*end && *end != '\"') end++;");
        wln!(self, "                size_t len = end - s + 1; // include closing quote");
        wln!(self, "                char* result = (char*)malloc(len+1); memcpy(result, s, len); result[len] = 0;");
        wln!(self, "                return result;");
        wln!(self, "            }}");
        wln!(self, "            const char* end = s;");
        wln!(self, "            while (*end && *end != ',' && *end != ']') end++;");
        wln!(self, "            size_t len = end - s;");
        wln!(self, "            char* result = (char*)malloc(len+1); memcpy(result, s, len); result[len] = 0;");
        wln!(self, "            return result;");
        wln!(self, "        }}");
        wln!(self, "        // Skip this element");
        wln!(self, "        if (*s == '\"') {{");
        wln!(self, "            s++; while (*s && *s != '\"') s++; if (*s) s++;");
        wln!(self, "        }} else {{");
        wln!(self, "            while (*s && *s != ',' && *s != ']') s++;");
        wln!(self, "        }}");
        wln!(self, "        if (*s == ',') s++;");
        wln!(self, "        cur++;");
        wln!(self, "    }}");
        wln!(self, "    return strdup(\"null\");");
        wln!(self, "}}");
        wln!(self, "int64_t p_json_len(void* j) {{");
        wln!(self, "    const char* s = (const char*)j;");
        wln!(self, "    if (!s) return 0;");
        wln!(self, "    int64_t count = 0; int depth = 0; bool in_str = false;");
        wln!(self, "    while (*s) {{");
        wln!(self, "        if (*s == '\"' && (s == (const char*)j || s[-1] != '\\\\')) in_str = !in_str;");
        wln!(self, "        if (!in_str) {{");
        wln!(self, "            if (*s == '{' || *s == '[') depth++;");
        wln!(self, "            if (*s == '}' || *s == ']') depth--;");
        wln!(self, "            if (*s == ',' && depth == 1) count++;");
        wln!(self, "        }}");
        wln!(self, "        s++;");
        wln!(self, "    }}");
        wln!(self, "    return count + 1;");
        wln!(self, "}}");
        wln!(self, "void* p_json_object_get(void* j, const char* key) {{ return p_json_get(j, key); }}");

        // ─── HTTP builtins (Phase 5D) ───
        wln!(self, "");
        wln!(self, "int64_t p_http_listen(int64_t port) {{ return p_socket_listen(port); }}");
        wln!(self, "char* p_http_recv_request(int64_t fd) {{ return p_socket_recv_str(fd); }}");
        wln!(self, "void p_http_send_response(int64_t fd, int64_t status, const char* body) {{");
        wln!(self, "    char header[256];");
        wln!(self, "    snprintf(header, sizeof(header), \"HTTP/1.1 %lld OK\\r\\nContent-Type: text/plain\\r\\nContent-Length: %zu\\r\\n\\r\\n\", (long long)status, strlen(body));");
        wln!(self, "    p_socket_write(fd, header, strlen(header));");
        wln!(self, "    p_socket_write(fd, body, strlen(body));");
        wln!(self, "}}");
        wln!(self, "char* p_http_build_response(int64_t status, const char* content_type, const char* body) {{");
        wln!(self, "    (void)content_type;");
        wln!(self, "    size_t blen = strlen(body);");
        wln!(self, "    char* resp = (char*)malloc(blen + 256);");
        wln!(self, "    snprintf(resp, blen+256, \"HTTP/1.1 %lld OK\\r\\nContent-Length: %zu\\r\\n\\r\\n%s\", (long long)status, blen, body);");
        wln!(self, "    return resp;");
        wln!(self, "}}");
        wln!(self, "char* p_http_parse_request(const char* request) {{");
        wln!(self, "    if (!request) return strdup(\"{}\");");
        wln!(self, "    // Parse request line: METHOD PATH HTTP/VERSION");
        wln!(self, "    const char* p = request;");
        wln!(self, "    const char* line_end = strstr(p, \"\\r\\n\");");
        wln!(self, "    if (!line_end) line_end = p + strlen(p);");
        wln!(self, "    // Extract method");
        wln!(self, "    const char* sp1 = strchr(p, ' ');");
        wln!(self, "    char method[16] = {{0}};");
        wln!(self, "    if (sp1 && sp1 < line_end) {{ size_t ml = sp1 - p; if (ml > 15) ml = 15; memcpy(method, p, ml); }}");
        wln!(self, "    // Extract path");
        wln!(self, "    const char* path_start = sp1 ? sp1 + 1 : p;");
        wln!(self, "    const char* sp2 = strchr(path_start, ' ');");
        wln!(self, "    char path[1024] = {{0}};");
        wln!(self, "    if (sp2 && sp2 < line_end) {{ size_t pl = sp2 - path_start; if (pl > 1023) pl = 1023; memcpy(path, path_start, pl); }}");
        wln!(self, "    // Extract version");
        wln!(self, "    const char* ver_start = sp2 ? sp2 + 1 : p;");
        wln!(self, "    char version[16] = {{0}};");
        wln!(self, "    if (ver_start < line_end) {{ size_t vl = line_end - ver_start; if (vl > 15) vl = 15; memcpy(version, ver_start, vl); }}");
        wln!(self, "    // Strip HTTP/ prefix");
        wln!(self, "    const char* v = version;");
        wln!(self, "    if (strncmp(v, \"HTTP/\", 5) == 0) v += 5;");
        wln!(self, "    // Parse headers until empty line");
        wln!(self, "    p = line_end + 2; // skip \\r\\n");
        wln!(self, "    size_t cap = 1024; char* result = (char*)malloc(cap); size_t pos = 0;");
        wln!(self, "    pos += snprintf(result+pos, cap-pos, \"{\\\"method\\\":\\\"%s\\\",\\\"path\\\":\\\"%s\\\",\\\"version\\\":\\\"%s\\\",\\\"headers\\\":{\", method, path, v);");
        wln!(self, "    int first_hdr = 1;");
        wln!(self, "    while (*p) {{");
        wln!(self, "        const char* next_line = strstr(p, \"\\r\\n\");");
        wln!(self, "        if (!next_line) break;");
        wln!(self, "        if (next_line == p) {{ p += 2; break; }} // empty line = end of headers");
        wln!(self, "        // Find colon");
        wln!(self, "        const char* colon = strchr(p, ':');");
        wln!(self, "        if (colon && colon < next_line) {{");
        wln!(self, "            size_t kl = colon - p;");
        wln!(self, "            const char* val_start = colon + 1;");
        wln!(self, "            while (*val_start == ' ') val_start++;");
        wln!(self, "            size_t vl = next_line - val_start;");
        wln!(self, "            if (!first_hdr) pos += snprintf(result+pos, cap-pos, \",\");");
        wln!(self, "            first_hdr = 0;");
        wln!(self, "            if (pos + kl + vl + 10 >= cap) {{ cap = (cap + kl + vl) * 2; result = (char*)realloc(result, cap); }}");
        wln!(self, "            result[pos++] = '\"'; memcpy(result+pos, p, kl); pos += kl; result[pos++] = '\"'; result[pos++] = ':'; result[pos++] = '\"'; memcpy(result+pos, val_start, vl); pos += vl; result[pos++] = '\"';");
        wln!(self, "        }}");
        wln!(self, "        p = next_line + 2;");
        wln!(self, "    }}");
        wln!(self, "    pos += snprintf(result+pos, cap-pos, \"},\\\"body\\\":\\\"\");");
        wln!(self, "    // Extract body (everything after the empty line) and escape for JSON");
        wln!(self, "    if (*p) {{");
        wln!(self, "        while (*p) {{");
        wln!(self, "            if (pos >= cap-2) {{ cap *= 2; result = (char*)realloc(result, cap); }}");
        wln!(self, "            if (*p == '\"') {{ result[pos++] = '\\\\'; result[pos++] = '\"'; }}");
        wln!(self, "            else if (*p == '\\\\') {{ result[pos++] = '\\\\'; result[pos++] = '\\\\'; }}");
        wln!(self, "            else if (*p == '\\n') {{ result[pos++] = '\\\\'; result[pos++] = 'n'; }}");
        wln!(self, "            else if (*p == '\\r') {{ result[pos++] = '\\\\'; result[pos++] = 'r'; }}");
        wln!(self, "            else if (*p == '\\t') {{ result[pos++] = '\\\\'; result[pos++] = 't'; }}");
        wln!(self, "            else result[pos++] = *p;");
        wln!(self, "            p++;");
        wln!(self, "        }}");
        wln!(self, "    }}");
        wln!(self, "    pos += snprintf(result+pos, cap-pos, \"\\\"}\");");
        wln!(self, "    return result;");
        wln!(self, "}}");

        // ─── DB builtins (Phase 5D) ───
        wln!(self, "");
        wln!(self, "// DB builtins — in-memory key-value store");
        wln!(self, "typedef struct { char** keys; char** vals; int64_t len; int64_t cap; } PDB;");
        wln!(self, "void* p_db_connect(const char* conn_str) {{");
        wln!(self, "    (void)conn_str;");
        wln!(self, "    PDB* db = (PDB*)malloc(sizeof(PDB));");
        wln!(self, "    db->keys = NULL; db->vals = NULL; db->len = 0; db->cap = 0;");
        wln!(self, "    return db;");
        wln!(self, "}}");
        wln!(self, "void p_db_exec(void* db, const char* query) {{ (void)db; (void)query; }}");
        wln!(self, "void* p_db_query(void* db, const char* query) {{ (void)query; return db; }}");
        wln!(self, "char* p_db_get(void* db, const char* key) {{");
        wln!(self, "    PDB* pdb = (PDB*)db;");
        wln!(self, "    if (!pdb) return strdup(\"\");");
        wln!(self, "    for (int64_t i = 0; i < pdb->len; i++) if (strcmp(pdb->keys[i], key) == 0) return strdup(pdb->vals[i]);");
        wln!(self, "    return strdup(\"\");");
        wln!(self, "}}");
        wln!(self, "void p_db_set(void* db, const char* key, const char* val) {{");
        wln!(self, "    PDB* pdb = (PDB*)db;");
        wln!(self, "    if (!pdb) return;");
        wln!(self, "    for (int64_t i = 0; i < pdb->len; i++) if (strcmp(pdb->keys[i], key) == 0) {{ free(pdb->vals[i]); pdb->vals[i] = strdup(val); return; }}");
        wln!(self, "    if (pdb->len >= pdb->cap) {{ pdb->cap = pdb->cap == 0 ? 8 : pdb->cap * 2; pdb->keys = (char**)realloc(pdb->keys, pdb->cap * sizeof(char*)); pdb->vals = (char**)realloc(pdb->vals, pdb->cap * sizeof(char*)); }}");
        wln!(self, "    pdb->keys[pdb->len] = strdup(key); pdb->vals[pdb->len] = strdup(val); pdb->len++;");
        wln!(self, "}}");
        // ─── Memory DB implementation ───
        self.out.push_str("typedef struct { char** keys; char** vals; int64_t len; int64_t cap; } PMemDB;\n");
        wln!(self, "void* p_db_mem_open(void) {{");
        wln!(self, "    PMemDB* db = (PMemDB*)calloc(1, sizeof(PMemDB));");
        wln!(self, "    return db;");
        wln!(self, "}}");
        wln!(self, "int64_t p_db_mem_exec(void* db, const char* query) {{");
        wln!(self, "    PMemDB* mdb = (PMemDB*)db;");
        wln!(self, "    if (!mdb || !query) return 0;");
        wln!(self, "    const char* p = query;");
        wln!(self, "    while (*p == ' ') p++;");
        wln!(self, "    char cmd[16] = {{0}}; int ci = 0;");
        wln!(self, "    while (*p && *p != ' ' && ci < 15) cmd[ci++] = tolower(*p++);");
        wln!(self, "    while (*p == ' ') p++;");
        wln!(self, "    const char* kstart = p;");
        wln!(self, "    while (*p && *p != ' ') p++;");
        wln!(self, "    size_t klen = p - kstart;");
        wln!(self, "    while (*p == ' ') p++;");
        wln!(self, "    const char* vstart = p;");
        wln!(self, "    while (*p) p++;");
        wln!(self, "    size_t vlen = p - vstart;");
        wln!(self, "    if ((strcmp(cmd, \"insert\") == 0 || strcmp(cmd, \"set\") == 0) && klen > 0) {{");
        wln!(self, "        if (mdb->len >= mdb->cap) {{ mdb->cap = mdb->cap ? mdb->cap*2 : 8; mdb->keys = (char**)realloc(mdb->keys, mdb->cap*sizeof(char*)); mdb->vals = (char**)realloc(mdb->vals, mdb->cap*sizeof(char*)); }}");
        wln!(self, "        mdb->keys[mdb->len] = (char*)malloc(klen+1); memcpy(mdb->keys[mdb->len], kstart, klen); mdb->keys[mdb->len][klen] = 0;");
        wln!(self, "        mdb->vals[mdb->len] = (char*)malloc(vlen+1); memcpy(mdb->vals[mdb->len], vstart, vlen); mdb->vals[mdb->len][vlen] = 0;");
        wln!(self, "        mdb->len++;");
        wln!(self, "        return 1;");
        wln!(self, "    }}");
        wln!(self, "    return 0;");
        wln!(self, "}}");
        wln!(self, "char* p_db_mem_query(void* db, const char* query) {{");
        wln!(self, "    (void)query;");
        wln!(self, "    PMemDB* mdb = (PMemDB*)db;");
        wln!(self, "    if (!mdb) return strdup(\"[]\");");
        wln!(self, "    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;");
        wln!(self, "    result[pos++] = '[';");
        wln!(self, "    for (int64_t i = 0; i < mdb->len; i++) {{");
        wln!(self, "        if (i > 0) result[pos++] = ',';");
        wln!(self, "        size_t kl = strlen(mdb->keys[i]);");
        wln!(self, "        if (pos+kl+4 >= cap) {{ cap = (cap+kl)*2; result = (char*)realloc(result, cap); }}");
        wln!(self, "        result[pos++] = '\"'; memcpy(result+pos, mdb->keys[i], kl); pos += kl; result[pos++] = '\"';");
        wln!(self, "    }}");
        wln!(self, "    result[pos++] = ']'; result[pos] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "void p_db_mem_close(void* db) {{");
        wln!(self, "    PMemDB* mdb = (PMemDB*)db;");
        wln!(self, "    if (mdb) {{ for (int64_t i = 0; i < mdb->len; i++) {{ free(mdb->keys[i]); free(mdb->vals[i]); }} free(mdb->keys); free(mdb->vals); free(mdb); }}");
        wln!(self, "}}");
    }
}

pub fn generate_c(module: &IrModule) -> String {
    let mut gen = CCodeGen::new();
    gen.generate(module)
}
