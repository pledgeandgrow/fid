// C code generation from Fides IR
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
        $s.out.push_str(&$fmt.replace("{{", "{").replace("}}", "}"));
        $s.out.push('\n');
    }};
    ($s:expr, $fmt:expr, $($arg:expr),*) => {{
        $s.out.push_str(&format!($fmt, $($arg),*));
        $s.out.push('\n');
    }};
}

/// C bodies for core.fd's Vec_new/len/is_empty/get/set/push/pop, with core's
/// exact semantics: the header is {data, len, cap} (three i64), growth
/// doubles from 8 through p_malloc, a bad index panics with core's message.
/// Used only when the function has core's signature (an env parameter, i64
/// arguments, the same return type, and for get/set core's panic string),
/// so a user's own function of that name keeps its body.
fn fast_core_vec_body(name: &str, func: &IrFunction, c_ret: &str) -> Option<Vec<String>> {
    if func.params.iter().skip(1).any(|(_, t)| *t != IrType::I64) {
        return None;
    }
    let p: Vec<String> = func.params.iter().map(|(id, _)| format!("v{}", id.0)).collect();
    let checks_bounds = func.instructions.values().any(|i| {
        matches!(i, Inst::ConstStr { value } if value == "Vec index out of bounds")
    });
    let hdr = |v: &str| format!("    int64_t* __h = (int64_t*)(intptr_t){};", v);
    let oob = |i: &str| vec![
        format!("    if ({} >= __h[1]) p_panic(\"Vec index out of bounds\");", i),
        format!("    if ({} < 0) p_panic(\"Vec index out of bounds\");", i),
    ];
    let lines: Vec<String> = match (name, p.len(), c_ret) {
        ("Vec_new", 1, "int64_t") => vec![
            "    int64_t* __h = (int64_t*)p_malloc(24);".into(),
            "    __h[0] = 0; __h[1] = 0; __h[2] = 0;".into(),
            "    return (int64_t)(intptr_t)__h;".into(),
        ],
        ("Vec_len", 2, "int64_t") => vec![hdr(&p[1]), "    return __h[1];".into()],
        ("Vec_is_empty", 2, "bool") => vec![hdr(&p[1]), "    return __h[1] == 0;".into()],
        ("Vec_get", 3, "int64_t") if checks_bounds => {
            let mut l = vec![hdr(&p[1])];
            l.extend(oob(&p[2]));
            l.push(format!("    return ((int64_t*)(intptr_t)__h[0])[{}];", p[2]));
            l
        }
        ("Vec_set", 4, "void") if checks_bounds => {
            let mut l = vec![hdr(&p[1])];
            l.extend(oob(&p[2]));
            l.push(format!("    ((int64_t*)(intptr_t)__h[0])[{}] = {};", p[2], p[3]));
            l
        }
        ("Vec_push", 3, "void") => vec![
            hdr(&p[1]),
            "    int64_t __len = __h[1], __cap = __h[2];".into(),
            "    if (__len >= __cap) {".into(),
            "        int64_t __nc = __cap > 0 ? __cap * 2 : 8;".into(),
            "        int64_t* __nd = (int64_t*)p_malloc(__nc * 8);".into(),
            "        int64_t* __od = (int64_t*)(intptr_t)__h[0];".into(),
            "        for (int64_t __i = 0; __i < __len; __i++) __nd[__i] = __od[__i];".into(),
            "        if (__od) p_free(__od);".into(),
            "        __h[0] = (int64_t)(intptr_t)__nd;".into(),
            "        __h[2] = __nc;".into(),
            "    }".into(),
            format!("    ((int64_t*)(intptr_t)__h[0])[__len] = {};", p[2]),
            "    __h[1] = __len + 1;".into(),
        ],
        ("Vec_pop", 2, "int64_t") => vec![
            hdr(&p[1]),
            "    if (__h[1] == 0) return 0;".into(),
            "    __h[1] = __h[1] - 1;".into(),
            "    return ((int64_t*)(intptr_t)__h[0])[__h[1]];".into(),
        ],
        _ => return None,
    };
    Some(lines)
}

/// Check if a function name is an internal helper that should not be
/// registered for thread spawning. This includes:
/// - Functions starting with `p_` (runtime builtins)
/// - Small helper/utility functions that are never spawned as threads
/// Note: `__`-prefixed functions are NOT excluded here — some (like
/// `__par_sum_worker`) are spawned via p_thread_spawn.
fn is_internal_function(name: &str) -> bool {
    name.starts_with("p_")
        || name.starts_with("box_")
        || name.starts_with("type_")
        || name.starts_with("stmt_")
        || name.starts_with("expr_")
        || name.starts_with("opt_")
        || name.starts_with("str_")
        || name.starts_with("int_")
        || name.starts_with("char_")
        || name.starts_with("map_")
        || name.starts_with("vec_")
        || name.starts_with("c_")
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
    /// Byte ranges in `out` for each emitted function body
    /// (name, start, end) — for per-module TU splitting (Phase B).
    fn_marks: Vec<(String, usize, usize)>,
    /// Byte offset in `out` where the tail (main wrapper + runtime impl)
    /// begins.
    tail_mark: usize,
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
            fn_marks: Vec::new(),
            tail_mark: 0,
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

    /// `(T)(intptr_t)` for the C type `T` a value is declared with: converts
    /// an integer or a pointer to it, so an int/pointer mismatch between the
    /// IR and the declaration compiles under -Werror=int-conversion (GCC 14,
    /// Clang 16+). None for float and array types.
    fn int_ptr_cast(ty: Option<&IrType>) -> Option<String> {
        match ty {
            Some(IrType::F32) | Some(IrType::F64) | Some(IrType::Array(_, _)) => None,
            Some(IrType::Void) | Some(IrType::Never) | None => Some("(int64_t)(intptr_t)".to_string()),
            Some(t) => Some(format!("({})(intptr_t)", Self::c_type(t))),
        }
    }

    /// C type for a global variable. Struct types are emitted as values
    /// (not pointers) so that `memset` / `memcpy` can initialize them.
    fn c_type_global(ty: &IrType) -> String {
        match ty {
            IrType::Struct(name) => format!("struct {}_s", name),
            IrType::Array(elem, size) => format!("{}[{}]", Self::c_type_global(elem), size),
            _ => Self::c_type(ty),
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

        let to_is_int = matches!(to, IrType::I64 | IrType::I32 | IrType::I16 | IrType::I8 | IrType::Bool);

        // `Ptr(G)` arg → `G` param (Generic/Opaque): the arg is a `&mut T`-style
        // slot but the callee expects the handle value. Emit the pointee so
        // the callee sees the header, not the slot. Only fires when the
        // pointee IS the param's type — generic type vars (e.g. `K` in
        // HashMap<K,V>) legitimately hold raw pointers like str.
        let to_is_handle = matches!(to, IrType::Generic(_, _) | IrType::Opaque(_));
        if let (IrType::Ptr(inner), true) = (from, to_is_handle) {
            if **inner == *to {
                return format!("*(void**){}", val);
            }
        }

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
            // float → int: use type punning via union to preserve bit pattern
            (false, false, _) if from_is_float && to_is_int => {
                format!("((union {{ double d; int64_t i; }}){{ .d = {} }}).i", val)
            }
            // int → pointer: cast to void*
            (false, true, _) => format!("(void*){}", val),
            // pointer → int: cast to int64_t
            (true, false, _) => format!("(int64_t){}", val),
            // pointer → pointer: cast through void* to avoid aliasing warnings
            (true, true, false) => format!("(void*){}", val),
            // pointer → struct pointer (out-param): cast to void*
            (false, false, true) => format!("(void*){}", val),
            _ => val.to_string(),
        }
    }

    /// Sanitize a Fides function name for use as a C identifier.
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
            "remove" => "p_remove_user",
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

        // Emit struct definitions (must come before globals so struct types
        // are known when global variables are initialized)
        self.emit_struct_defs(module);

        // Emit module-level mutable globals
        self.emit_globals(module);

        // Emit function prototypes
        self.emit_prototypes(module);

        // Emit each function
        for (name, func) in &module.functions {
            let start = self.out.len();
            self.gen_function(name, func);
            self.fn_marks.push((name.clone(), start, self.out.len()));
        }

        self.tail_mark = self.out.len();
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
            ("p_char_to_str", &[I64]),
            ("p_to_str_vec", &[p()]),
            ("p_to_str_str", &[p()]),
            ("p_to_str_bool", &[Bool]),
            ("p_to_str_float", &[F64]),
            ("p_to_str_map", &[p()]),
            ("p_float_to_str", &[F64]),
            ("p_f64_to_bits", &[F64]),
            ("p_f32_to_bits", &[F32]),
            ("p_f64_from_bits", &[I64]),
            ("p_f32_from_bits", &[I64]),
            ("p_int_to_float", &[I64]),
            ("p_float_to_int", &[F64]),
            ("p_str_to_int", &[p()]),
            ("p_bool_to_str", &[Bool]),
            ("p_vec_to_str", &[p()]),
            ("p_str_concat", &[p(), p()]),
            ("p_str_join_n", &[p()]),
            ("p_sb_new", &[]),
            ("p_sb_append", &[p(), p()]),
            ("p_sb_to_str", &[p()]),
            ("p_print", &[I64]),
            ("p_print_str", &[p()]),
            ("p_eprint_str", &[p()]),
            ("p_print_int", &[I64]),
            ("p_print_bool", &[Bool]),
            ("p_print_float", &[F64]),
            ("p_print_newline", &[]),
            ("p_malloc", &[I64]),
            ("p_alloc", &[I64]),
            ("p_free", &[p()]),
            ("p_malloc_persistent", &[I64]),
            ("p_arena_save", &[]),
            ("p_arena_restore", &[I64]),
            ("p_region_free", &[p()]),
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
            ("p_lgdt", &[I64]),
            ("p_gdt_set_entry", &[I64, I64, I64, I64, I64]),
            ("p_serial_init", &[]),
            ("p_serial_write", &[I64]),
            ("p_serial_read", &[]),
            ("p_serial_ready", &[]),
            ("p_simd_add", &[I64, I64]),
            ("p_simd_sub", &[I64, I64]),
            ("p_simd_mul", &[I64, I64]),
            ("p_simd_div", &[I64, I64]),
            ("p_simd_dot", &[I64, I64]),
            ("p_simd_sum", &[I64]),
            ("p_simd_scale", &[I64, F64]),
            ("p_simd_from_vec", &[I64]),
            ("p_simd_to_str", &[I64]),
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
            ("p_file_read", &[p()]),
            ("p_file_write", &[p(), p()]),
            ("p_file_exists", &[p()]),
            ("p_file_append", &[p(), p()]),
            ("p_file_delete", &[p()]),
            ("p_file_size", &[p()]),
            ("p_file_hash", &[p()]),
            ("p_dir_list", &[p()]),
            ("p_is_dir", &[p()]),
            ("p_mkdir", &[p()]),
            ("p_rmdir", &[p()]),
            ("p_rename", &[p(), p()]),
            ("p_getpid", &[]),
            ("p_process_run", &[p()]),
            ("p_process_stdout", &[I64]),
            ("p_process_stderr", &[I64]),
            ("p_process_code", &[I64]),
            ("p_process_close", &[I64]),
            // Varargs functions: register the fixed prefix (non-varargs params).
            // p_thread_spawn(func_name: const char*, num_args: i64, ...args)
            // p_executor_spawn(exec: i64, func_name: const char*, num_args: i64, ...args)
            // Varargs args (beyond the fixed prefix) are cast to int64_t by the
            // call-site fallback logic (needed for void*→int64_t via va_arg).
            ("p_thread_spawn", &[p(), I64]),
            ("p_executor_spawn", &[I64, p(), I64]),
            // Channel functions
            ("p_chan_create", &[I64]),
            ("p_chan_send", &[I64, I64]),
            ("p_chan_recv", &[I64]),
            ("p_chan_try_recv", &[I64]),
            ("p_chan_close", &[I64]),
            ("p_chan_is_closed", &[I64]),
            // ML3: Autograd builtins
            ("p_grad_get", &[p()]),
            ("p_grad_zero", &[]),
            // ML11: Checkpoint builtins
            ("p_checkpoint_begin", &[]),
            ("p_checkpoint_end", &[]),
            // ML12: DP builtins
            ("p_dp_noise", &[p(), F64]),
            ("p_gaussian_noise", &[p(), F64]),
            ("p_dropout", &[p(), F64]),
            ("p_dp_budget_reset", &[F64]),
            ("p_dp_budget_check", &[F64, F64]),
            // ML13: Tensor parallelism builtins
            ("p_tp_rank", &[]),
            ("p_tp_world_size", &[]),
            ("p_all_reduce", &[p()]),
            ("p_broadcast", &[p()]),
            ("p_all_gather", &[p()]),
            ("p_scatter", &[p()]),
            ("p_send_tensor", &[p()]),
            ("p_recv_tensor", &[p()]),
            // ML9: KV Cache builtins
            ("p_kv_cache_new", &[I64, I64, I64, I64]),
            ("p_kv_cache_append", &[I64, I64, p(), p()]),
            ("p_kv_cache_get", &[I64, I64]),
            ("p_kv_cache_len", &[I64, I64]),
            ("p_kv_cache_clear", &[I64]),
            ("p_kv_cache_evict", &[I64, I64]),
            // ML10: Quantization builtins
            ("p_quantize_q8", &[p(), F64, I64]),
            ("p_dequantize_q8", &[p(), F64, I64]),
            ("p_qmatmul", &[p(), p()]),
            ("p_find_quant_params", &[p()]),
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
            "p_print", "p_print_str", "p_eprint_str", "p_print_int", "p_print_bool",
            "p_print_float", "p_print_newline", "p_flush_stdout", "p_free", "p_arena_restore", "p_region_free", "p_panic",
            "p_socket_close",
            "p_cli", "p_sti", "p_cr3_write", "p_invlpg", "p_page_map", "p_page_unmap",
            "p_phys_write64", "p_lgdt", "p_gdt_set_entry",
            "p_serial_init", "p_serial_write",
            "p_tensor_set",
            "p_json_set", "p_json_set_int", "p_json_set_float", "p_json_set_bool",
            "p_json_append", "p_json_append_int", "p_json_append_float",
            "p_http_send_response",
            "p_db_exec", "p_db_set", "p_db_mem_close",
            "p_rc_decr",
            "p_grad_zero", "p_checkpoint_begin", "p_checkpoint_end",
            "p_dp_budget_reset", "p_dp_budget_check", "p_send_tensor",
            "p_kv_cache_append", "p_kv_cache_clear", "p_kv_cache_evict",
        ];
        for name in void_ret {
            self.fn_ret_types.insert(name.to_string(), IrType::Void);
        }
        // Phase 10B: p_map_keys/p_map_values/p_string_split now return i64 (Fides Vec pointer)
        self.fn_ret_types.insert("p_map_keys".to_string(), IrType::I64);
        self.fn_ret_types.insert("p_map_values".to_string(), IrType::I64);
        self.fn_ret_types.insert("p_string_split".to_string(), IrType::I64);
        // String builder functions
        self.fn_ret_types.insert("p_sb_new".to_string(), IrType::I64);
        self.fn_ret_types.insert("p_sb_to_str".to_string(), IrType::Ptr(Box::new(IrType::U8)));
        self.fn_ret_types.insert("p_str_join_n".to_string(), IrType::Ptr(Box::new(IrType::U8)));
        // Phase 10B: SIMD functions now return i64 (Fides Vec pointer)
        for name in &["p_simd_add", "p_simd_sub", "p_simd_mul", "p_simd_div", "p_simd_scale", "p_simd_from_vec"] {
            self.fn_ret_types.insert(name.to_string(), IrType::I64);
        }
        // Rc/Arc refcount functions return i64 (the pointer value)
        self.fn_ret_types.insert("p_rc_incr".to_string(), IrType::I64);
        // ML13: TP rank/world_size return i64
        self.fn_ret_types.insert("p_tp_rank".to_string(), IrType::I64);
        self.fn_ret_types.insert("p_tp_world_size".to_string(), IrType::I64);
        // ML9: KV cache builtins
        self.fn_ret_types.insert("p_kv_cache_new".to_string(), IrType::I64);
        self.fn_ret_types.insert("p_kv_cache_get".to_string(), IrType::Opaque("Tensor".to_string()));
        self.fn_ret_types.insert("p_kv_cache_len".to_string(), IrType::I64);
        // ML10: Quantization builtins
        self.fn_ret_types.insert("p_quantize_q8".to_string(), IrType::Opaque("Tensor".to_string()));
        self.fn_ret_types.insert("p_dequantize_q8".to_string(), IrType::Opaque("Tensor".to_string()));
        self.fn_ret_types.insert("p_qmatmul".to_string(), IrType::Opaque("Tensor".to_string()));
        self.fn_ret_types.insert("p_find_quant_params".to_string(), IrType::Ptr(Box::new(IrType::U8)));
        // ML3: Autograd gradient retrieval returns Tensor
        self.fn_ret_types.insert("p_grad_get".to_string(), IrType::Opaque("Tensor".to_string()));
        // ML13: all_reduce/broadcast/all_gather/scatter return Tensor
        self.fn_ret_types.insert("p_all_reduce".to_string(), IrType::Opaque("Tensor".to_string()));
        self.fn_ret_types.insert("p_broadcast".to_string(), IrType::Opaque("Tensor".to_string()));
        self.fn_ret_types.insert("p_all_gather".to_string(), IrType::Opaque("Tensor".to_string()));
        self.fn_ret_types.insert("p_scatter".to_string(), IrType::Opaque("Tensor".to_string()));
        // ML12: DP noise/dropout return Tensor
        self.fn_ret_types.insert("p_dp_noise".to_string(), IrType::Opaque("Tensor".to_string()));
        self.fn_ret_types.insert("p_gaussian_noise".to_string(), IrType::Opaque("Tensor".to_string()));
        self.fn_ret_types.insert("p_dropout".to_string(), IrType::Opaque("Tensor".to_string()));
    }

    fn emit_runtime_header(&mut self) {
        wln!(self, "// Auto-generated C code from Fides IR");
        wln!(self, "// IEEE arithmetic exactly as written: no fused multiply-add contraction.");
        wln!(self, "#if defined(__clang__)");
        wln!(self, "#pragma STDC FP_CONTRACT OFF");
        wln!(self, "#elif defined(__GNUC__)");
        wln!(self, "#pragma GCC optimize (\"fp-contract=off\")");
        wln!(self, "#endif");
        wln!(self, "#include <stdio.h>");
        wln!(self, "#include <stdlib.h>");
        wln!(self, "#include <string.h>");
        wln!(self, "#ifndef _WIN32");
        wln!(self, "#include <alloca.h>");
        wln!(self, "#endif");
        wln!(self, "#include <stdint.h>");
        wln!(self, "#include <stdbool.h>");
        wln!(self, "#include <ctype.h>");
        wln!(self, "#include <math.h>");
        // Socket support for native HTTP server (Phase 5D)
        wln!(self, "#ifdef _WIN32");
        wln!(self, "#include <winsock2.h>");
        wln!(self, "#include <ws2tcpip.h>");
        wln!(self, "#include <io.h>");
        wln!(self, "#include <fcntl.h>");
        wln!(self, "// PLINK: ws2_32");
        wln!(self, "#else");
        wln!(self, "#include <sys/socket.h>");
        wln!(self, "#include <netinet/in.h>");
        wln!(self, "#include <arpa/inet.h>");
        wln!(self, "#include <unistd.h>");
        wln!(self, "#include <fcntl.h>");
        wln!(self, "#endif");
        // Directory listing + process capture (fs/process stdlib builtins)
        wln!(self, "#include <sys/stat.h>");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "#include <windows.h>");
        wln!(self, "#include <direct.h>");
        wln!(self, "#include <process.h>");
        wln!(self, "#else");
        wln!(self, "#include <dirent.h>");
        wln!(self, "#include <sys/wait.h>");
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
        // Forward-declare PTensor so it can be used in function declarations
        wln!(self, "typedef struct { double* data; int64_t len; int64_t ndim; int64_t shape[8]; } PTensor;");
        wln!(self, "");
    }

    /// Emit user-defined extern declarations (FFI)
    fn emit_extern_decls(&mut self, module: &IrModule) {
        if module.extern_decls.is_empty() {
            return;
        }

        // Emit #include directives — skip local headers that don't exist on
        // the filesystem. System headers (<...>) are always emitted (the C
        // compiler knows where to find them). For missing local headers,
        // we'll emit stub implementations below so the C code still compiles.
        let mut missing_includes: std::collections::HashSet<String> = std::collections::HashSet::new();
        let mut emitted_includes: std::collections::HashSet<String> = std::collections::HashSet::new();
        for ext in &module.extern_decls {
            if let Some(inc) = &ext.include {
                if emitted_includes.insert(inc.clone()) {
                    let is_system = inc.starts_with('<');
                    if is_system {
                        // System header — always emit, C compiler finds it
                        wln!(self, "#include {}", inc);
                    } else {
                        // Local header — check if file exists
                        let clean = inc.trim_matches(|c| c == '"' || c == '<' || c == '>');
                        let exists = std::path::Path::new(clean).exists();
                        if exists {
                            if inc.starts_with('"') {
                                wln!(self, "#include {}", inc);
                            } else {
                                wln!(self, "#include \"{}\"", inc);
                            }
                        } else {
                            missing_includes.insert(inc.clone());
                        }
                    }
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
            // If the include header is missing, emit a stub implementation
            let inc_missing = ext.include.as_ref()
                .map(|inc| missing_includes.contains(inc))
                .unwrap_or(false);
            // Check if this is a system header include — if so, the function
            // is already declared by the header. Only emit the #define rename,
            // not a conflicting forward declaration (e.g. printf is variadic
            // in <stdio.h> but the FFI decl has fixed args).
            let is_system_include = ext.include.as_ref()
                .map(|inc| inc.starts_with('<'))
                .unwrap_or(false);
            if inc_missing {
                let ret_default = match &ext.ret_ty {
                    ast::Type::F32 | ast::Type::F64 => "0.0",
                    _ => "0",
                };
                let body = if matches!(ext.ret_ty, ast::Type::Void) {
                    " { }".to_string()
                } else {
                    format!(" {{ return {}; }}", ret_default)
                };
                wln!(self, "{} {}({}){}", ret_c, ext.pledge_name, params_str, body);
            } else if is_system_include && ext.pledge_name != ext.c_name {
                // System header already declares the function — just emit the #define
                wln!(self, "#define {} {}", ext.pledge_name, ext.c_name);
            } else {
                wln!(self, "{} {}({});", ret_c, ext.c_name, params_str);
                if ext.pledge_name != ext.c_name {
                    wln!(self, "#define {} {}", ext.pledge_name, ext.c_name);
                }
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
        wln!(self, "int64_t p_vec_capacity(PVec* v);");
        wln!(self, "PMap* p_map_new(void);");
        wln!(self, "void p_map_insert(PMap* m, const char* key, int64_t val);");
        wln!(self, "int64_t p_map_get(PMap* m, const char* key);");
        wln!(self, "bool p_map_contains(PMap* m, const char* key);");
        wln!(self, "void p_map_remove(PMap* m, const char* key);");
        wln!(self, "int64_t p_map_len(PMap* m);");
        wln!(self, "bool p_map_is_empty(PMap* m);");
        wln!(self, "void p_map_clear(PMap* m);");
        wln!(self, "int64_t p_map_keys(PMap* m);");
        wln!(self, "int64_t p_map_values(PMap* m);");
        // ECS runtime (Phase 8B)
        wln!(self, "int64_t __ecs_create_entity(void);");
        wln!(self, "int64_t __ecs_entity_count(void);");
        wln!(self, "int64_t __ecs_has(const char* comp_name, int64_t entity);");
        wln!(self, "void* __ecs_get(const char* comp_name, int64_t entity);");
        wln!(self, "int64_t __ecs_add(const char* comp_name, int64_t entity, void* value);");
        wln!(self, "int64_t __ecs_remove(const char* comp_name, int64_t entity);");
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
        // Volatile memory access (MMIO) forward declarations
        wln!(self, "int64_t p_volatile_load(int64_t addr);");
        wln!(self, "void p_volatile_store(int64_t addr, int64_t val);");
        wln!(self, "int64_t p_volatile_load8(int64_t addr);");
        wln!(self, "int64_t p_volatile_load16(int64_t addr);");
        wln!(self, "int64_t p_volatile_load32(int64_t addr);");
        wln!(self, "void p_volatile_store8(int64_t addr, int64_t val);");
        wln!(self, "void p_volatile_store16(int64_t addr, int64_t val);");
        wln!(self, "void p_volatile_store32(int64_t addr, int64_t val);");
        wln!(self, "void p_lgdt(int64_t gdt_desc);");
        wln!(self, "void p_gdt_set_entry(int64_t gdt_ptr, int64_t index, int64_t base, int64_t limit, int64_t flags);");
        wln!(self, "void p_serial_init(void);");
        wln!(self, "void p_serial_write(int64_t c);");
        wln!(self, "int64_t p_serial_read(void);");
        wln!(self, "int64_t p_serial_ready(void);");
        // SIMD builtins (Phase 5C) — elementwise double4 operations on Fides Vec
        wln!(self, "int64_t p_simd_add(int64_t a, int64_t b);");
        wln!(self, "int64_t p_simd_sub(int64_t a, int64_t b);");
        wln!(self, "int64_t p_simd_mul(int64_t a, int64_t b);");
        wln!(self, "int64_t p_simd_div(int64_t a, int64_t b);");
        wln!(self, "double p_simd_dot(int64_t a, int64_t b);");
        wln!(self, "double p_simd_sum(int64_t a);");
        wln!(self, "int64_t p_simd_scale(int64_t a, double s);");
        wln!(self, "int64_t p_simd_from_vec(int64_t v);");
        wln!(self, "char* p_simd_to_str(int64_t a);");
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
        // ML3: Autograd builtins
        wln!(self, "void* p_grad_get(void* t);");
        wln!(self, "void p_grad_zero(void);");
        wln!(self, "void p_autograd_begin(void);");
        wln!(self, "void p_autograd_end(void);");
        wln!(self, "void p_autograd_backward(void* loss);");
        // ML11: Checkpoint builtins
        wln!(self, "void p_checkpoint_begin(void);");
        wln!(self, "void p_checkpoint_end(void);");
        // ML12: DP builtins
        wln!(self, "void* p_dp_noise(void* t, double sigma);");
        wln!(self, "void* p_gaussian_noise(void* shape_vec, double sigma);");
        wln!(self, "void* p_dropout(void* t, double drop_prob);");
        wln!(self, "void p_dp_budget_reset(double epsilon);");
        wln!(self, "void p_dp_budget_check(double epsilon, double limit);");
        // ML13: Tensor parallelism builtins
        wln!(self, "int64_t p_tp_rank(void);");
        wln!(self, "int64_t p_tp_world_size(void);");
        wln!(self, "void* p_all_reduce(void* t);");
        wln!(self, "void* p_broadcast(void* t);");
        wln!(self, "void* p_all_gather(void* t);");
        wln!(self, "void* p_scatter(void* t);");
        wln!(self, "void p_send_tensor(void* t);");
        wln!(self, "void* p_recv_tensor(void* shape_vec);");
        // ML9: KV Cache builtins
        wln!(self, "int64_t p_kv_cache_new(int64_t num_layers, int64_t max_seq_len, int64_t window_size, int64_t head_dim);");
        wln!(self, "void p_kv_cache_append(int64_t cache_id, int64_t layer, void* k, void* v);");
        wln!(self, "void* p_kv_cache_get(int64_t cache_id, int64_t layer);");
        wln!(self, "int64_t p_kv_cache_len(int64_t cache_id, int64_t layer);");
        wln!(self, "void p_kv_cache_clear(int64_t cache_id);");
        wln!(self, "void p_kv_cache_evict(int64_t cache_id, int64_t num_tokens);");
        // ML10: Quantization builtins
        wln!(self, "void* p_quantize_q8(void* t, double scale, int64_t zero_point);");
        wln!(self, "void* p_dequantize_q8(void* t, double scale, int64_t zero_point);");
        wln!(self, "void* p_qmatmul(void* a, void* b);");
        wln!(self, "char* p_find_quant_params(void* t);");
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
        wln!(self, "int64_t p_string_split(const char* s, const char* delim);");
        wln!(self, "int64_t p_string_parse_int(const char* s);");
        wln!(self, "char* p_string_char_at(const char* s, int64_t idx);");
        wln!(self, "char* p_string_substring(const char* s, int64_t start, int64_t end);");
        wln!(self, "char* p_to_str(int64_t val);");
        wln!(self, "char* p_char_to_str(int64_t val);");
        wln!(self, "char* p_to_str_vec(PVec* v);");
        wln!(self, "char* p_to_str_str(const char* s);");
        wln!(self, "char* p_to_str_bool(bool val);");
        wln!(self, "char* p_to_str_float(double val);");
        wln!(self, "char* p_to_str_map(PMap* m);");
        wln!(self, "char* p_float_to_str(double val);");
        wln!(self, "int64_t p_f64_to_bits(double val);");
        wln!(self, "int64_t p_f32_to_bits(float val);");
        wln!(self, "double p_f64_from_bits(int64_t bits);");
        wln!(self, "float p_f32_from_bits(int64_t bits);");
        wln!(self, "double p_int_to_float(int64_t val);");
        wln!(self, "int64_t p_float_to_int(double val);");
        wln!(self, "int64_t p_str_to_int(const char* s);");
        wln!(self, "double p_str_to_float(const char* s);");
        wln!(self, "char* p_bool_to_str(bool val);");
        wln!(self, "char* p_vec_to_str(PVec* v);");
        wln!(self, "char* p_str_concat(const char* a, const char* b);");
        wln!(self, "char* p_str_join_n(PVec* parts);");
        wln!(self, "void* p_sb_new(void);");
        wln!(self, "void p_sb_append(void* sb, const char* s);");
        wln!(self, "const char* p_sb_to_str(void* sb);");
        wln!(self, "void p_print(int64_t val);");
        wln!(self, "void p_print_str(const char* s);");
        wln!(self, "void p_print_int(int64_t val);");
        wln!(self, "void p_print_bool(bool val);");
        wln!(self, "void p_print_float(double val);");
        wln!(self, "void p_print_newline(void);");
        wln!(self, "void p_eprint_str(const char* s);");
        wln!(self, "void p_flush_stdout(void);");
        wln!(self, "int64_t p_stdin_eof(void);");
        wln!(self, "char* p_read_line(void);");
        wln!(self, "char* p_read_stdin(int64_t n);");
        wln!(self, "void p_panic(const char* msg);");
        self.gen_arith_helpers();
        wln!(self, "// Call stack for panic traces + recursion limit");
        wln!(self, "// Per-thread runtime state (arena, call stack): no locks needed.");
        wln!(self, "#if defined(_MSC_VER)");
        wln!(self, "#define P_THREAD_LOCAL __declspec(thread)");
        wln!(self, "#elif defined(__TINYC__)");
        wln!(self, "#define P_THREAD_LOCAL /* TCC has no thread-local storage */");
        wln!(self, "#else");
        wln!(self, "#define P_THREAD_LOCAL __thread");
        wln!(self, "#endif");
        wln!(self, "static P_THREAD_LOCAL const char* p_call_stack[2048];");
        wln!(self, "static P_THREAD_LOCAL int p_call_stack_depth = 0;");
        wln!(self, "static int p_max_recursion = 10000; // default limit");
        wln!(self, "void p_call_stack_push(const char* name) {{");
        wln!(self, "    if (p_call_stack_depth >= p_max_recursion) {{");
        wln!(self, "        fprintf(stderr, \"\\nRecursion limit exceeded (%d) in function '%s'\\n\", p_max_recursion, name);");
        wln!(self, "        fprintf(stderr, \"Call stack (last 20):\\n\");");
        wln!(self, "        int start = p_call_stack_depth > 20 ? p_call_stack_depth - 20 : 0;");
        wln!(self, "        for (int i = start; i < p_call_stack_depth; i++)");
        wln!(self, "            fprintf(stderr, \"  [%d] %s\\n\", i, p_call_stack[i]);");
        wln!(self, "        abort();");
        wln!(self, "    }}");
        wln!(self, "    if (p_call_stack_depth < 2048) p_call_stack[p_call_stack_depth++] = name;");
        wln!(self, "}}");
        wln!(self, "void p_call_stack_pop(void) {{ if (p_call_stack_depth > 0) p_call_stack_depth--; }}");
        wln!(self, "void p_set_max_recursion(int limit) {{ p_max_recursion = limit; }}");
        wln!(self, "void* p_malloc(int64_t size);");
        wln!(self, "void* p_alloc(int64_t size);");
        wln!(self, "void p_free(void* ptr);");
        wln!(self, "int64_t p_arena_save(void);");
        wln!(self, "void p_arena_restore(int64_t mark);");
        wln!(self, "void p_region_free(void* region);");
        wln!(self, "void p_memset(void* ptr, int val, int64_t size);");
        wln!(self, "void p_str_zeroize(void* s);");
        wln!(self, "void p_vec_zeroize(void* v);");
        // File I/O builtins
        wln!(self, "char* p_file_read(const char* path);");
        wln!(self, "int64_t p_file_write(const char* path, const char* content);");
        wln!(self, "int64_t p_file_write_bin(const char* path, const char* data, int64_t len);");
        wln!(self, "int64_t p_file_write_vec(const char* path, PVec* v);");
        wln!(self, "PVec* p_file_read_bytes(const char* path);");
        wln!(self, "bool p_file_exists(const char* path);");
        wln!(self, "int64_t p_file_append(const char* path, const char* content);");
        wln!(self, "int64_t p_file_delete(const char* path);");
        wln!(self, "int64_t p_file_size(const char* path);");
        wln!(self, "int64_t p_file_hash(const char* path);");
        // Environment variable builtins
        wln!(self, "char* p_getenv(const char* name);");
        // Command-line argument builtins — globals saved by main wrapper
        wln!(self, "static int __p_argc = 0;");
        wln!(self, "static char** __p_argv = NULL;");
        wln!(self, "int64_t p_argc(void);");
        wln!(self, "char* p_argv(int64_t index);");
        // Process control builtins
        wln!(self, "int64_t p_system(const char* cmd);");
        wln!(self, "int64_t p_process_run(const char* cmd);");
        wln!(self, "char* p_process_stdout(int64_t h);");
        wln!(self, "char* p_process_stderr(int64_t h);");
        wln!(self, "int64_t p_process_code(int64_t h);");
        wln!(self, "void p_process_close(int64_t h);");
        wln!(self, "int64_t p_getpid(void);");
        wln!(self, "void p_exit(int64_t code);");
        // Filesystem builtins beyond p_file_*
        wln!(self, "char* p_dir_list(const char* path);");
        wln!(self, "bool p_is_dir(const char* path);");
        wln!(self, "int64_t p_mkdir(const char* path);");
        wln!(self, "int64_t p_rmdir(const char* path);");
        wln!(self, "int64_t p_rename(const char* a, const char* b);");
        // Wall-clock timing builtins
        wln!(self, "int64_t p_clock_now(void);");
        wln!(self, "int64_t p_clock_now_us(void);");
        wln!(self, "int64_t p_clock_now_ms(void);");
        // Regex builtin
        wln!(self, "bool p_regex_match(const char* s, const char* pattern);");
        wln!(self, "const char* p_intern_str(const char* s);");
        // Async/await runtime (Phase 7.6)
        wln!(self, "int64_t p_future_ready(int64_t value);");
        wln!(self, "int64_t p_future_poll(int64_t future);");
        // Threads + atomics + mutex (Phase 8A)
        wln!(self, "int64_t p_thread_spawn(const char* func_name, int64_t num_args, ...);");
        // Phase A1: Executor + non-blocking task poll
        wln!(self, "int64_t p_executor_new(void);");
        wln!(self, "int64_t p_executor_spawn(int64_t exec, const char* func_name, int64_t num_args, ...);");
        wln!(self, "int64_t p_executor_block_on(int64_t exec, int64_t future);");
        wln!(self, "int64_t p_task_poll(int64_t future);");
        wln!(self, "bool p_task_is_ready(int64_t future);");
        wln!(self, "int64_t p_executor_run_all(int64_t exec);");
        wln!(self, "int64_t p_executor_cancel(int64_t exec);");
        wln!(self, "int64_t p_executor_poll_all(int64_t exec);");
        wln!(self, "int64_t p_task_poll_nonblocking(int64_t future);");
        // Phase A3: Non-blocking IO
        wln!(self, "int64_t p_net_connect(const char* host, int64_t port);");
        wln!(self, "int64_t p_net_read(int64_t sock, char* buf, int64_t len);");
        wln!(self, "int64_t p_net_write(int64_t sock, const char* buf, int64_t len);");
        wln!(self, "void p_net_close(int64_t sock);");
        wln!(self, "int64_t p_fs_read_async(const char* path, char* buf, int64_t len);");
        wln!(self, "int64_t p_shared_alloc(int64_t size);");
        wln!(self, "int64_t p_atomic_load(int64_t addr);");
        wln!(self, "void p_atomic_store(int64_t addr, int64_t val);");
        wln!(self, "bool p_atomic_cas(int64_t addr, int64_t expected, int64_t new_val);");
        wln!(self, "int64_t p_atomic_add(int64_t addr, int64_t val);");
        wln!(self, "int64_t p_atomic_sub(int64_t addr, int64_t val);");
        wln!(self, "int64_t p_rc_incr(int64_t rc_addr, int64_t val);");
        wln!(self, "void p_rc_decr(int64_t rc_addr, int64_t val);");
        wln!(self, "int64_t p_mutex_init(void);");
        wln!(self, "void p_mutex_lock(int64_t m);");
        wln!(self, "void p_mutex_unlock(int64_t m);");
        // Thread-local storage builtins
        wln!(self, "int64_t p_tls_create(void);");
        wln!(self, "int64_t p_tls_get(int64_t key);");
        wln!(self, "void p_tls_set(int64_t key, int64_t value);");
        // Channels (bounded MPSC queue)
        wln!(self, "int64_t p_chan_create(int64_t capacity);");
        wln!(self, "bool p_chan_send(int64_t ch, int64_t val);");
        wln!(self, "int64_t p_chan_recv(int64_t ch);");
        wln!(self, "int64_t p_chan_try_recv(int64_t ch);");
        wln!(self, "void p_chan_close(int64_t ch);");
        wln!(self, "bool p_chan_is_closed(int64_t ch);");
        wln!(self, "void p_thread_register(const char* name, int64_t (*fn)());");
        // Dynamic loading builtins (Phase 7.7)
        wln!(self, "int64_t p_dl_open(const char* path);");
        wln!(self, "int64_t p_dl_sym(int64_t handle, const char* symbol);");
        wln!(self, "void p_dl_close(int64_t handle);");
        wln!(self, "int64_t p_dl_call(int64_t func_ptr, int64_t arg);");
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
                    if let Some(Inst::ConstFnRef { name }) = func.instructions.get(&inst_id) {
                        if !self.string_consts.contains_key(name) {
                            let cname = format!("_str{}", idx);
                            idx += 1;
                            let escaped = Self::escape_c_string(name);
                            wln!(self, "static const char {}[] = \"{}\";", cname, escaped);
                            self.string_consts.insert(name.clone(), cname);
                        }
                    }
                }
            }
        }
        wln!(self, "");
    }

    /// Emit module-level mutable global variables as C file-scope variables.
    fn emit_globals(&mut self, module: &IrModule) {
        if module.globals.is_empty() { return; }
        wln!(self, "// ─── Module-level mutable globals ───");
        for (name, (ty, init)) in &module.globals {
            let c_ty = Self::c_type_global(ty);
            let c_name = Self::global_c_name(name);
            // Emit as a static global with initializer.
            // For struct types, use {0} initializer (zero-init all fields).
            let init_str = match ty {
                IrType::Struct(_) => "{0}".to_string(),
                IrType::Array(_, _) => "{0}".to_string(),
                _ => self.gen_global_init_expr(init, ty),
            };
            wln!(self, "static {} {} = {};", c_ty, c_name, init_str);
        }
        wln!(self, "");
    }

    /// Mangle a global name for C emission (avoid collisions with functions/locals)
    fn global_c_name(name: &str) -> String {
        format!("_g_{}", name)
    }

    /// Generate a C expression for a global initializer.
    fn gen_global_init_expr(&self, expr: &crate::ast::Expr, ty: &IrType) -> String {
        use crate::ast::Expr;
        match expr {
            Expr::IntLit(v) => format!("{}LL", v),
            Expr::FloatLit(v) => {
                if v.is_nan() { "NAN".to_string() }
                else if v.is_infinite() { if *v > 0.0 { "INFINITY".to_string() } else { "-INFINITY".to_string() } }
                else { format!("{}", v) }
            }
            Expr::BoolLit(b) => if *b { "true".to_string() } else { "false".to_string() },
            Expr::StringLit(s) => {
                let escaped = Self::escape_c_string(s);
                format!("\"{}\"", escaped)
            }
            _ => {
                // For complex initializers, emit 0 (will be initialized at runtime if needed)
                let _ = ty;
                "0".to_string()
            }
        }
    }

    fn emit_struct_defs(&mut self, module: &IrModule) {
        wln!(self, "// ─── Struct definitions ───");
        for (name, layout) in &module.struct_layouts {
            if layout.repr_c {
                // @repr(C) struct: emit with natural C types and explicit padding
                wln!(self, "struct {}_s {{", name);
                let mut prev_offset: u64 = 0;
                for (i, (_fname, field_ty, offset)) in layout.fields.iter().enumerate() {
                    // Emit padding bytes if needed to match the layout
                    if *offset as u64 > prev_offset {
                        let pad = *offset as u64 - prev_offset;
                        wln!(self, "    char _pad_{}[{}];", i, pad);
                    }
                    wln!(self, "    {} field_{};", Self::c_type(field_ty), i);
                    // Calculate field size
                    let field_size: u64 = if i + 1 < layout.fields.len() {
                        layout.fields[i + 1].2 as u64 - *offset as u64
                    } else {
                        layout.size - *offset as u64
                    };
                    prev_offset = *offset as u64 + field_size;
                }
                // Final padding to reach struct size
                if layout.size > prev_offset {
                    wln!(self, "    char _pad_end[{}];", layout.size - prev_offset);
                }
                wln!(self, "}} __attribute__((aligned({})));", layout.align);
                wln!(self, "");
                // Skip the default closing below — already emitted
                continue;
            } else {
                // Default Fides layout: uniform 8-byte slots
                wln!(self, "struct {}_s {{", name);
                for (i, (_fname, field_ty, offset)) in layout.fields.iter().enumerate() {
                    let field_size: u64 = if i + 1 < layout.fields.len() {
                        layout.fields[i + 1].2 as u64 - *offset as u64
                    } else {
                        layout.size - *offset as u64
                    };
                    if field_size > 8 {
                        let slots = field_size / 8;
                        wln!(self, "    int64_t field_{}[{}];", i, slots);
                    } else {
                        match field_ty {
                            IrType::Bool => {
                                wln!(self, "    int64_t field_{};", i);
                            }
                            _ => {
                                wln!(self, "    {} field_{};", Self::c_type(field_ty), i);
                            }
                        }
                    }
                }
            }
            wln!(self, "};");
            wln!(self, "");
        }
    }

    fn emit_prototypes(&mut self, module: &IrModule) {
        wln!(self, "// ─── Function prototypes ───");
        // Forward declaration for p_thread_register (used in main wrapper)
        wln!(self, "void p_thread_register(const char* name, int64_t (*fn)());");
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
            // Windows: vectored exception handler to print call stack on crash
            wln!(self, "#ifdef _WIN32");
            wln!(self, "#include <windows.h>");
            wln!(self, "LONG WINAPI __p_crash_handler(PEXCEPTION_POINTERS ep) {{");
            wln!(self, "    if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION ||");
            wln!(self, "        ep->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW) {{");
            wln!(self, "        fprintf(stderr, \"\\n=== CRASH: 0x%lX ===\\n\", (unsigned long)ep->ExceptionRecord->ExceptionCode);");
            wln!(self, "        if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {{");
            wln!(self, "            fprintf(stderr, \"Access violation at address 0x%llX\\n\",");
            wln!(self, "                    (unsigned long long)ep->ExceptionRecord->ExceptionInformation[1]);");
            wln!(self, "        }}");
            wln!(self, "        fprintf(stderr, \"Call stack depth: %d\\n\", p_call_stack_depth);");
            wln!(self, "        int start = p_call_stack_depth > 30 ? p_call_stack_depth - 30 : 0;");
            wln!(self, "        for (int i = start; i < p_call_stack_depth; i++)");
            wln!(self, "            fprintf(stderr, \"  [%d] %s\\n\", i, p_call_stack[i]);");
            wln!(self, "        fflush(stderr);");
            wln!(self, "    }}");
            wln!(self, "    return EXCEPTION_CONTINUE_SEARCH;");
            wln!(self, "}}");
            wln!(self, "#endif");
            // Linux: signal handler to print call stack on crash
            wln!(self, "#ifndef _WIN32");
            wln!(self, "#include <signal.h>");
            wln!(self, "static void __p_sig_handler(int sig) {{");
            wln!(self, "    const char* name = sig == SIGSEGV ? \"SIGSEGV\" : sig == SIGBUS ? \"SIGBUS\" : sig == SIGABRT ? \"SIGABRT\" : sig == SIGFPE ? \"SIGFPE\" : \"UNKNOWN\";");
            wln!(self, "    fprintf(stderr, \"\\n=== CRASH: signal %d (%s) ===\\n\", sig, name);");
            wln!(self, "    fprintf(stderr, \"Call stack depth: %d\\n\", p_call_stack_depth);");
            wln!(self, "    int start = p_call_stack_depth > 30 ? p_call_stack_depth - 30 : 0;");
            wln!(self, "    for (int i = start; i < p_call_stack_depth; i++)");
            wln!(self, "        fprintf(stderr, \"  [%d] %s\\n\", i, p_call_stack[i]);");
            wln!(self, "    fflush(stderr);");
            wln!(self, "    _exit(128 + sig);");
            wln!(self, "}}");
            wln!(self, "#endif");
            // Windows: wrap main in a thread with 16MB stack to prevent stack overflow
            // when processing large files (the self-hosted compiler needs >1MB stack)
            wln!(self, "int real_main(int argc, char** argv) {{");
            wln!(self, "#ifdef _WIN32");
            wln!(self, "    SetUnhandledExceptionFilter(__p_crash_handler);");
            wln!(self, "#else");
            wln!(self, "    signal(SIGSEGV, __p_sig_handler);");
            wln!(self, "    signal(SIGBUS, __p_sig_handler);");
            wln!(self, "    signal(SIGABRT, __p_sig_handler);");
            wln!(self, "    signal(SIGFPE, __p_sig_handler);");
            wln!(self, "#endif");
            wln!(self, "    setvbuf(stdout, NULL, _IONBF, 0);");
            wln!(self, "    setvbuf(stderr, NULL, _IONBF, 0);");
            // On Windows, set stdout/stderr to binary mode so that printf
            // doesn't translate \n to \r\n. This ensures output matches
            // the interpreter exactly (important for strings containing \r\n
            // like HTTP responses, where text-mode would double the \r).
            wln!(self, "#ifdef _WIN32");
            wln!(self, "    _setmode(_fileno(stdout), _O_BINARY);");
            wln!(self, "    _setmode(_fileno(stderr), _O_BINARY);");
            wln!(self, "#endif");
            wln!(self, "    __p_argc = argc; __p_argv = argv;");
            // Register functions for p_thread_spawn lookup.
            // Skip internal/helper functions that are never spawned as threads
            // to avoid overflowing the registration table.
            wln!(self, "    // Register functions for p_thread_spawn lookup");
            for (name, _) in &module.functions {
                if name != "main" && !is_internal_function(name) {
                    let c_name = self.sanitize_c_name(name);
                    wln!(self, "    p_thread_register(\"{}\", (int64_t(*)()){});", name, c_name);
                }
            }
            wln!(self, "    int64_t result = main_entry(NULL);");
            wln!(self, "    return (int)result;");
            wln!(self, "}}");
            wln!(self, "");
            // Windows: just call real_main directly.
            // Stack size is set via PE header patching (build-selfhost.ps1).
            wln!(self, "#ifdef _WIN32");
            wln!(self, "int main(int argc, char** argv) {{ return real_main(argc, argv); }}");
            wln!(self, "#else");
            wln!(self, "int main(int argc, char** argv) {{ return real_main(argc, argv); }}");
            wln!(self, "#endif");
            wln!(self, "");
        }
    }

    fn alloca_size(&self, ty: &IrType) -> u64 {
        let size = match ty {
            IrType::Struct(name) => self
                .struct_layouts
                .get(name)
                .map(|l| l.size)
                .unwrap_or(0),
            _ => ty.size(),
        };
        // Ensure at least 1 byte to avoid malloc(0) undefined behavior
        if size == 0 {
            8
        } else {
            size
        }
    }

    /// Checked integer arithmetic, one helper per type: the same meaning as
    /// the interpreter and the PE backend. A @wrapping function does not use
    /// add/sub/mul; dividing by zero panics everywhere.
    fn gen_arith_helpers(&mut self) {
        wln!(self, "#define P_OVF_ADD \"Integer overflow on addition\"");
        wln!(self, "#define P_OVF_SUB \"Integer overflow on subtraction\"");
        wln!(self, "#define P_OVF_MUL \"Integer overflow on multiplication\"");
        wln!(self, "#if (defined(__GNUC__) || defined(__clang__)) && !defined(__TINYC__)");
        wln!(self, "#define P_ARITH(T, N) \\");
        wln!(self, "static inline T p_add_##N(T a, T b) {{ T r; if (__builtin_add_overflow(a, b, &r)) p_panic(P_OVF_ADD); return r; }} \\");
        wln!(self, "static inline T p_sub_##N(T a, T b) {{ T r; if (__builtin_sub_overflow(a, b, &r)) p_panic(P_OVF_SUB); return r; }} \\");
        wln!(self, "static inline T p_mul_##N(T a, T b) {{ T r; if (__builtin_mul_overflow(a, b, &r)) p_panic(P_OVF_MUL); return r; }}");
        wln!(self, "P_ARITH(int8_t, i8) P_ARITH(int16_t, i16) P_ARITH(int32_t, i32) P_ARITH(int64_t, i64)");
        wln!(self, "P_ARITH(uint8_t, u8) P_ARITH(uint16_t, u16) P_ARITH(uint32_t, u32) P_ARITH(uint64_t, u64)");
        wln!(self, "#else");
        wln!(self, "#define P_NARROW(T, N, LO, HI) \\");
        wln!(self, "static inline T p_add_##N(T a, T b) {{ int64_t r = (int64_t)a + b; if (r < (LO) || r > (HI)) p_panic(P_OVF_ADD); return (T)r; }} \\");
        wln!(self, "static inline T p_sub_##N(T a, T b) {{ int64_t r = (int64_t)a - b; if (r < (LO) || r > (HI)) p_panic(P_OVF_SUB); return (T)r; }}");
        wln!(self, "#define P_NARROW_MUL(T, N, LO, HI) \\");
        wln!(self, "static inline T p_mul_##N(T a, T b) {{ int64_t r = (int64_t)a * b; if (r < (LO) || r > (HI)) p_panic(P_OVF_MUL); return (T)r; }}");
        wln!(self, "P_NARROW(int8_t, i8, -128, 127) P_NARROW_MUL(int8_t, i8, -128, 127)");
        wln!(self, "P_NARROW(int16_t, i16, -32768, 32767) P_NARROW_MUL(int16_t, i16, -32768, 32767)");
        wln!(self, "P_NARROW(int32_t, i32, -2147483648LL, 2147483647LL) P_NARROW_MUL(int32_t, i32, -2147483648LL, 2147483647LL)");
        wln!(self, "P_NARROW(uint8_t, u8, 0, 255) P_NARROW_MUL(uint8_t, u8, 0, 255)");
        wln!(self, "P_NARROW(uint16_t, u16, 0, 65535) P_NARROW_MUL(uint16_t, u16, 0, 65535)");
        wln!(self, "P_NARROW(uint32_t, u32, 0, 4294967295LL)");
        wln!(self, "static inline uint32_t p_mul_u32(uint32_t a, uint32_t b) {{ uint64_t r = (uint64_t)a * b; if (r > 4294967295ULL) p_panic(P_OVF_MUL); return (uint32_t)r; }}");
        wln!(self, "static inline int64_t p_add_i64(int64_t a, int64_t b) {{ int64_t r = (int64_t)((uint64_t)a + (uint64_t)b); if (((a ^ r) & (b ^ r)) < 0) p_panic(P_OVF_ADD); return r; }}");
        wln!(self, "static inline int64_t p_sub_i64(int64_t a, int64_t b) {{ int64_t r = (int64_t)((uint64_t)a - (uint64_t)b); if (((a ^ b) & (a ^ r)) < 0) p_panic(P_OVF_SUB); return r; }}");
        wln!(self, "static inline int64_t p_mul_i64(int64_t a, int64_t b) {{ int64_t r = (int64_t)((uint64_t)a * (uint64_t)b); if (a != 0 && ((a == -1 && b == INT64_MIN) || r / a != b)) p_panic(P_OVF_MUL); return r; }}");
        wln!(self, "static inline uint64_t p_add_u64(uint64_t a, uint64_t b) {{ uint64_t r = a + b; if (r < a) p_panic(P_OVF_ADD); return r; }}");
        wln!(self, "static inline uint64_t p_sub_u64(uint64_t a, uint64_t b) {{ if (b > a) p_panic(P_OVF_SUB); return a - b; }}");
        wln!(self, "static inline uint64_t p_mul_u64(uint64_t a, uint64_t b) {{ uint64_t r = a * b; if (a != 0 && r / a != b) p_panic(P_OVF_MUL); return r; }}");
        wln!(self, "#endif");
        wln!(self, "#define P_DIVS(T, N) \\");
        wln!(self, "static inline T p_div_##N(T a, T b) {{ if (b == 0) p_panic(\"Division by zero\"); if (b == -1) {{ if (a == (T)((uint64_t)1 << (sizeof(T) * 8 - 1))) p_panic(\"Integer overflow on division\"); return (T)(0 - (uint64_t)a); }} return (T)(a / b); }} \\");
        wln!(self, "static inline T p_wdiv_##N(T a, T b) {{ if (b == 0) p_panic(\"Division by zero\"); if (b == -1) return (T)(0 - (uint64_t)a); return (T)(a / b); }} \\");
        wln!(self, "static inline T p_mod_##N(T a, T b) {{ if (b == 0) p_panic(\"Modulo by zero\"); if (b == -1) return 0; return (T)(a % b); }}");
        wln!(self, "#define P_DIVU(T, N) \\");
        wln!(self, "static inline T p_div_##N(T a, T b) {{ if (b == 0) p_panic(\"Division by zero\"); return (T)(a / b); }} \\");
        wln!(self, "static inline T p_wdiv_##N(T a, T b) {{ return p_div_##N(a, b); }} \\");
        wln!(self, "static inline T p_mod_##N(T a, T b) {{ if (b == 0) p_panic(\"Modulo by zero\"); return (T)(a % b); }}");
        wln!(self, "P_DIVS(int8_t, i8) P_DIVS(int16_t, i16) P_DIVS(int32_t, i32) P_DIVS(int64_t, i64)");
        wln!(self, "P_DIVU(uint8_t, u8) P_DIVU(uint16_t, u16) P_DIVU(uint32_t, u32) P_DIVU(uint64_t, u64)");
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

        // Core's hottest Vec functions: a few C statements instead of the
        // lowered Fides body (much faster in the -O0 self-hosted build).
        if let Some(body) = fast_core_vec_body(name, func, &ret_ty) {
            for line in body {
                self.out.push_str(&line);
                self.out.push('\n');
            }
            wln!(self, "}");
            wln!(self, "");
            return;
        }

        // Push function name onto call stack for panic traces
        wln!(self, "    p_call_stack_push(\"{}\");", name);

        // Declare all SSA values as local variables (skip void/never and params),
        // in id order so the output does not depend on HashMap order.
        let mut values: Vec<_> = func.values.iter().collect();
        values.sort_by_key(|(id, _)| **id);
        for (id, meta) in values {
            if !func.params.iter().any(|(pid, _)| pid == id) {
                match meta.ty {
                    IrType::Void | IrType::Never => {
                        // Skip void variables — use placeholder
                        wln!(self, "    int64_t v{} = 0; /* placeholder for void */", id.0);
                    }
                    _ => {
                        let ty = Self::c_type(&meta.ty);
                        // Zero-initialize to prevent uninitialized variable bugs
                        // when code paths skip the assignment (e.g. conditional branches)
                        wln!(self, "    {} v{} = 0;", ty, id.0);
                    }
                }
                self.value_names.insert(id.0, format!("v{}", id.0));
            }
        }

        // Prologue alloca pre-pass: bind one stack slot per Inst::Alloca at
        // function entry. An alloca emitted inside a loop block burns fresh
        // stack on every iteration — alloca memory is only reclaimed at
        // function return — so high-trip-count loops overflow the native
        // stack. The slot is reused across iterations; the zero-init memset
        // stays at the instruction's own site so each iteration still sees
        // cleared storage.
        for block in &func.blocks {
            for &inst_id in &block.instructions {
                if let Some(Inst::Alloca { ty, .. }) = func.instructions.get(&inst_id) {
                    let size = self.alloca_size(ty);
                    wln!(self, "    v{} = alloca({});", inst_id.0, size);
                }
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
            Inst::ConstFnRef { name } => {
                // Function reference — emit the function name as a string for name-based lookup
                let const_name = self.string_consts.get(name).cloned().unwrap_or_else(|| {
                    let escaped = Self::escape_c_string(name);
                    format!("\"{}\"", escaped)
                });
                wln!(self, "    {} = (int64_t){};", dst, const_name);
            }
            Inst::ConstNull { .. } => {
                wln!(self, "    {} = NULL;", dst);
            }
            Inst::GlobalAddr { name, .. } => {
                // Take the address of the C global variable
                let cast = Self::int_ptr_cast(func.values.get(&id).map(|m| &m.ty))
                    .unwrap_or_else(|| "(int64_t)(intptr_t)".to_string());
                wln!(self, "    {} = {}&{};", dst, cast, Self::global_c_name(name));
            }
            Inst::BinOp { op, left, right, ty } => {
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
                // If the result type is a pointer/opaque, cast back from int64_t
                let result_is_ptr = is_ptr(Some(ty));
                // Float modulo: C doesn't support % on doubles, use fmod()
                let is_float_mod = matches!(op, BinOp::Mod) && matches!(ty, IrType::F64);
                // Integer arithmetic has one meaning (see gen_arith_helpers).
                let plain_operands = !is_ptr(l_ty) && !is_ptr(r_ty);
                let int_ty = if plain_operands { c_int_helper(ty) } else { None };
                let unsigned_cmp = matches!(op, BinOp::Lt | BinOp::LtEq | BinOp::Gt | BinOp::GtEq)
                    && (l_ty == Some(&IrType::U64) || r_ty == Some(&IrType::U64));
                if let (Some((n, t)), BinOp::Add | BinOp::Sub | BinOp::Mul | BinOp::Div | BinOp::Mod | BinOp::Shl | BinOp::Shr) = (int_ty, op) {
                    let wrapping = func.is_wrapping;
                    match op {
                        BinOp::Add | BinOp::Sub | BinOp::Mul if wrapping => {
                            wln!(self, "    {} = ({})((uint64_t){} {} (uint64_t){});", dst, t, l, cop, r);
                        }
                        BinOp::Add => wln!(self, "    {} = p_add_{}({}, {});", dst, n, l, r),
                        BinOp::Sub => wln!(self, "    {} = p_sub_{}({}, {});", dst, n, l, r),
                        BinOp::Mul => wln!(self, "    {} = p_mul_{}({}, {});", dst, n, l, r),
                        BinOp::Div if wrapping => wln!(self, "    {} = p_wdiv_{}({}, {});", dst, n, l, r),
                        BinOp::Div => wln!(self, "    {} = p_div_{}({}, {});", dst, n, l, r),
                        BinOp::Mod => wln!(self, "    {} = p_mod_{}({}, {});", dst, n, l, r),
                        BinOp::Shl => wln!(self, "    {} = ({})((uint64_t){} << ({} & 63));", dst, t, l, r),
                        _ if n.starts_with('u') => wln!(self, "    {} = ({})((uint64_t){} >> ({} & 63));", dst, t, l, r),
                        _ => wln!(self, "    {} = ({})((int64_t){} >> ({} & 63));", dst, t, l, r),
                    }
                } else if unsigned_cmp {
                    wln!(self, "    {} = (uint64_t){} {} (uint64_t){};", dst, l, cop, r);
                } else if is_float_mod {
                    wln!(self, "    {} = fmod({}, {});", dst, l, r);
                } else if result_is_ptr {
                    wln!(self, "    {} = (void*)(intptr_t)({} {} {});", dst, l, cop, r);
                } else {
                    wln!(self, "    {} = {} {} {};", dst, l, cop, r);
                }
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
                // The alloca() call itself was emitted once in the function
                // prologue (see gen_function's alloca pre-pass). Only the
                // zero-init runs here so each loop iteration still sees
                // cleared storage while the stack grows once per call.
                let size = self.alloca_size(ty);
                wln!(self, "    memset({}, 0, {});", dst, size);
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
            Inst::Store { ptr, value, store_ty: explicit_store_ty } => {
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
                // Prefer the explicit store_ty (from the instruction), then
                // the pointer's pointee type, then the value type.
                let store_ty = explicit_store_ty.clone()
                    .or(ptr_ty)
                    .or(val_ty)
                    .unwrap_or(IrType::I64);
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
                    IrType::Struct(name) => {
                        // Struct store: copy the full struct content, not just
                        // 8 bytes. The value is a pointer to the struct (structs
                        // are passed by pointer in Fides). Use memcpy to copy
                        // the struct's full size from the source pointer.
                        let size = self.struct_layouts.get(name).map(|l| l.size).unwrap_or(8);
                        wln!(self, "    memcpy({}, {}, {});", p, v, size);
                    }
                    _ => {
                        // I64, Void, Never, Array — store as int64_t
                        wln!(self, "    *(int64_t*){} = (int64_t){};", p, v);
                    }
                }
            }
            Inst::StructCopy { dst, src, ty } => {
                let d = self.val_name(*dst);
                let s = self.val_name(*src);
                let size = match ty {
                    IrType::Struct(name) => {
                        self.struct_layouts.get(name).map(|l| l.size).unwrap_or(8)
                    }
                    _ => ty.size(),
                };
                wln!(self, "    memcpy({}, {}, {});", d, s, size);
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
                // Pop call stack before returning (for panic traces)
                wln!(self, "    p_call_stack_pop();");
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
                            // Phase 10B: Vec is now a Fides type — call Vec_to_str
                            wln!(self, "    {{ char* _s = (char*)Vec_to_str(0, (int64_t){}); p_print_str(_s); }}", arg_str);
                        }
                        Some(IrType::Opaque(name)) if name == "Vec" => {
                            wln!(self, "    {{ char* _s = (char*)Vec_to_str(0, (int64_t){}); p_print_str(_s); }}", arg_str);
                        }
                        Some(IrType::Generic(name, _)) if name == "HashMap" => {
                            wln!(self, "    {{ char* _s = p_to_str_map((PMap*){}); p_print_str(_s); free(_s); }}", arg_str);
                        }
                        Some(IrType::Opaque(name)) if name == "HashMap" => {
                            wln!(self, "    {{ char* _s = p_to_str_map((PMap*){}); p_print_str(_s); free(_s); }}", arg_str);
                        }
                        Some(IrType::Opaque(name)) if name == "Tensor" => {
                            wln!(self, "    {{ char* _s = p_tensor_to_str((PTensor*){}); p_print_str(_s); free(_s); }}", arg_str);
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
                            // Phase 10B: Vec is now a Fides type — call Vec_to_str
                            wln!(self, "    {{ char* _s = (char*)Vec_to_str(0, (int64_t){}); p_print_str(_s); }}", arg_str);
                        }
                        Some(IrType::Opaque(name)) if name == "Vec" => {
                            wln!(self, "    {{ char* _s = (char*)Vec_to_str(0, (int64_t){}); p_print_str(_s); }}", arg_str);
                        }
                        Some(IrType::Generic(name, _)) if name == "HashMap" => {
                            wln!(self, "    {{ char* _s = p_to_str_map((PMap*){}); p_print_str(_s); free(_s); }}", arg_str);
                        }
                        Some(IrType::Opaque(name)) if name == "HashMap" => {
                            wln!(self, "    {{ char* _s = p_to_str_map((PMap*){}); p_print_str(_s); free(_s); }}", arg_str);
                        }
                        Some(IrType::Opaque(name)) if name == "Tensor" => {
                            wln!(self, "    {{ char* _s = p_tensor_to_str((PTensor*){}); p_print_str(_s); free(_s); }}", arg_str);
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
                            // Phase 10B: Vec is now a Fides type — call Vec_to_str
                            wln!(self, "    {} = (void*)Vec_to_str(0, (int64_t){});", dst, arg_str);
                        }
                        Some(IrType::Opaque(name)) if name == "Vec" => {
                            wln!(self, "    {} = (void*)Vec_to_str(0, (int64_t){});", dst, arg_str);
                        }
                        Some(IrType::Generic(name, _)) if name == "HashMap" => {
                            wln!(self, "    {} = (void*)p_to_str_map((PMap*){});", dst, arg_str);
                        }
                        Some(IrType::Opaque(name)) if name == "HashMap" => {
                            wln!(self, "    {} = (void*)p_to_str_map((PMap*){});", dst, arg_str);
                        }
                        Some(IrType::Opaque(name)) if name == "Tensor" => {
                            wln!(self, "    {} = (void*)p_tensor_to_str((PTensor*){});", dst, arg_str);
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
                        let arg_ty = func.values.get(a).map(|m| m.ty.clone()).unwrap_or(IrType::I64);
                        match param_types {
                            Some(pts) if i < pts.len() => {
                                Self::cast_arg(&base, &arg_ty, &pts[i])
                            }
                            _ => {
                                // No parameter type info for this arg position.
                                // For varargs functions (p_thread_spawn, p_executor_spawn),
                                // cast pointer-typed args to int64_t so they pass
                                // correctly through C varargs (va_arg(ap, int64_t)).
                                // Without this, void* values (Vec, HashMap, etc.)
                                // are misread by va_arg on some platforms.
                                // For other functions (e.g. FFI externs), pass as-is
                                // to avoid breaking pointer-parameter calls.
                                let is_varargs = fname == "p_thread_spawn" || fname == "p_executor_spawn";
                                let is_rc_func = fname == "p_rc_incr" || fname == "p_rc_decr";
                                if (is_varargs || is_rc_func) && matches!(arg_ty, IrType::Ptr(_) | IrType::Opaque(_) | IrType::Generic(_, _)) {
                                    format!("(int64_t){}", base)
                                } else {
                                    base
                                }
                            }
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
                            // Phase 10B: when destination is a pointer type but
                            // the callee returns int64_t (e.g. Fides Vec functions
                            // that return i64 but are typed as Opaque("Vec")),
                            // add a (void*) cast to satisfy C type checking.
                            let dst_is_ptr = matches!(dst_ty, Some(IrType::Ptr(_)) | Some(IrType::Opaque(_)) | Some(IrType::Generic(_, _)));
                            let callee_returns_int = callee_c_ret == "int64_t" || callee_c_ret == "int32_t" || callee_c_ret == "int16_t" || callee_c_ret == "int8_t";
                            // A float destination fed by an int- or pointer-returning
                            // callee holds the 8-byte word a generic container stored
                            // (Vec_get on a Vec<f64>): reinterpret the bits, as the
                            // self-hosted backend does, instead of converting them.
                            let callee_returns_word = callee_returns_int || callee_c_ret.ends_with('*');
                            let pun = match dst_ty {
                                Some(IrType::F64) if callee_returns_word => Some(("int64_t", "double", "d")),
                                Some(IrType::F32) if callee_returns_word => Some(("int32_t", "float", "f")),
                                _ => None,
                            };
                            if let Some((int_ty, float_ty, member)) = pun {
                                wln!(self, "    {} = ((union {{ {} i; {} {}; }}){{ .i = ({})(intptr_t){}({}) }}).{};",
                                    dst, int_ty, float_ty, member, int_ty, c_fname, args_joined, member);
                                return;
                            }
                            let cast = match dst_ty {
                                Some(IrType::Ptr(_)) | Some(IrType::Opaque(_)) | Some(IrType::Generic(_, _)) => {
                                    if dst_is_ptr && callee_returns_int {
                                        "(void*)".to_string()
                                    } else {
                                        "".to_string()
                                    }
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
                // Closures in Fides use string-based dispatch: the closure
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
                let mut candidates: Vec<&String> = self.fn_param_types
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
                candidates.sort(); // HashMap order: keep the output reproducible

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
                    // Check the actual function's return type, not just the
                    // CallPtr's expected type. This prevents assigning void
                    // function results to a variable.
                    let fn_ret = self.fn_ret_types.get(fname).cloned().unwrap_or(ret_ty.clone());
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
                            call_args.push(format!("(double)(intptr_t){}", arg));
                        } else if cast == "float" {
                            call_args.push(format!("(float)(intptr_t){}", arg));
                        } else if cast == "bool" {
                            call_args.push(format!("(bool){}", arg));
                        } else if cast.contains('[') {
                            // Array types (e.g. int64_t[3]) — can't cast to array
                            // type in C. Pass as pointer instead.
                            call_args.push(format!("(void*){}", arg));
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
                    match &fn_ret {
                        IrType::Void | IrType::Never => {
                            dispatch.push_str(&format!("            {};\n", call_expr));
                        }
                        ty if Self::is_aggregate_ret(ty) => {
                            // Aggregate types are returned via output pointer
                            // in C, so the function returns void.
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
                    let mut candidates: Vec<String> = self.user_functions.iter()
                        .filter(|name| name.ends_with(&format!("_{}", method)))
                        .cloned()
                        .collect();
                    candidates.sort(); // HashSet order: keep the output reproducible
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
            Inst::BitCast { value, from, to } => {
                let v = self.val_name(*value);
                // For int→int or float→float of same size, no cast needed.
                // For pointer→pointer, cast through void*.
                // For int→pointer or pointer→int, use appropriate cast.
                let from_is_ptr = matches!(from, IrType::Ptr(_) | IrType::Opaque(_) | IrType::Generic(_, _));
                let to_is_ptr = matches!(to, IrType::Ptr(_) | IrType::Opaque(_) | IrType::Generic(_, _));
                if from_is_ptr || to_is_ptr {
                    let declared = func.values.get(&id).map(|m| m.ty.clone()).unwrap_or_else(|| to.clone());
                    match Self::int_ptr_cast(Some(&declared)) {
                        Some(cast) => wln!(self, "    {} = {}{};", dst, cast, v),
                        None => wln!(self, "    {} = (void*){};", dst, v),
                    }
                } else {
                    wln!(self, "    {} = {};", dst, v);
                }
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
                        // Phase 10B: Vec is now a raw i64 pointer — call Fides Vec_to_str
                        // Note: Vec_to_str returns arena-allocated string (via p_str_concat/p_malloc) — do NOT free
                        wln!(self, "    {{ char* _s = (char*)Vec_to_str(0, (int64_t){}); p_print_str(_s); }}", v);
                    }
                    IrType::Opaque(name) if name == "Vec" => {
                        // Phase 10B: Vec is now a raw i64 pointer — call Fides Vec_to_str
                        // Note: Vec_to_str returns arena-allocated string (via p_str_concat/p_malloc) — do NOT free
                        wln!(self, "    {{ char* _s = (char*)Vec_to_str(0, (int64_t){}); p_print_str(_s); }}", v);
                    }
                    IrType::Generic(name, _) if name == "HashMap" => {
                        // Phase 10B: HashMap is now a raw i64 pointer — call Fides HashMap_to_str
                        // Note: HashMap_to_str returns arena-allocated string — do NOT free
                        wln!(self, "    {{ char* _s = (char*)HashMap_to_str(0, (int64_t){}); p_print_str(_s); }}", v);
                    }
                    IrType::Opaque(name) if name == "String" => {
                        // Phase 10B: String from Vec_get is an i64 pointer — cast to char*
                        wln!(self, "    p_print_str((char*){});", v);
                    }
                    IrType::Opaque(name) if name == "Tensor" => {
                        wln!(self, "    {{ char* _s = p_tensor_to_str((void*){}); p_print_str(_s); free(_s); }}", v);
                    }
                    _ => {
                        wln!(self, "    p_print_int((int64_t){});", v);
                    }
                }
            }
            Inst::PersistLoad { default, .. } => {
                // Persistent load — just use the default value (no persistence in C backend)
                let d = self.val_name(*default);
                wln!(self, "    {} = {};", dst, d);
            }
            Inst::PersistSave { .. } => {
                // Persistent save — no-op in C backend
            }
            Inst::TxnBegin | Inst::TxnEnd => {
                // Transaction begin/end — no-op in C backend (rollback not supported)
            }
            Inst::UnsafeBegin | Inst::UnsafeEnd => {
                // Unsafe block markers — no-op in C backend
            }
            Inst::GradBegin { .. } => {
                // ML3: Enable gradient tracking — clear tape and grads, set mode on
                wln!(self, "    p_autograd_begin();");
            }
            Inst::GradEnd => {
                // ML3: Disable gradient tracking
                wln!(self, "    p_autograd_end();");
            }
            Inst::Backward { loss } => {
                // ML3: Run backward pass — walk tape in reverse, compute gradients
                // The loss value is ignored (seed is always 1.0), pass NULL
                let _ = loss;
                wln!(self, "    p_autograd_backward(NULL);");
            }
            Inst::Panic { message, .. } => {
                wln!(self, "    p_panic(\"{}\");", message);
            }
            Inst::Asm { template, ret_ty, .. } => {
                // C backend: emit asm as a comment — the interpreter treats asm as a no-op
                // (just prints to stderr), so emitting real __asm__ would cause mismatches
                // and potentially crash on privileged instructions (hlt, cli, sti).
                // For x86-64 target, we could emit __asm__ volatile for safe instructions,
                // but matching interpreter behavior (no-op) is more correct for testing.
                let escaped = template.replace("/*", "").replace("*/", "");
                wln!(self, "    /* asm: {} */", escaped);
                // If asm has a return value, assign 0 (matches interpreter behavior)
                if !matches!(ret_ty, IrType::Void) {
                    wln!(self, "    {} = 0LL;", dst);
                }
            }
            Inst::RegionBegin { name, is_gpu, .. } => {
                // Region begin — no-op in C backend for CPU regions.
                // GPU regions (ML1) emit a marker comment for tooling.
                if *is_gpu {
                    wln!(self, "    /* GPU region begin: {} */", name);
                }
            }
            Inst::RegionEnd { name, is_gpu, .. } => {
                // Region end — no-op in C backend for CPU regions.
                // GPU regions (ML1) would free GPU memory here.
                if *is_gpu {
                    wln!(self, "    /* GPU region end: {} (all GPU memory freed) */", name);
                }
            }
        }
    }

    fn emit_runtime_impl(&mut self) {
        wln!(self, "");
        wln!(self, "// ─── Runtime implementation ───");
        wln!(self, "");
        // Vec runtime
        wln!(self, "PVec* p_vec_new(void) {{");
        wln!(self, "    PVec* v = (PVec*)p_malloc(sizeof(PVec));");
        wln!(self, "    v->data = NULL; v->len = 0; v->cap = 0;");
        wln!(self, "    return v;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "void p_vec_push(PVec* v, void* val) {{");
        wln!(self, "    if (!v) return;");
        wln!(self, "    if (v->len >= v->cap) {{");
        wln!(self, "        v->cap = v->cap == 0 ? 8 : v->cap * 2;");
        wln!(self, "        void** new_data = (void**)p_malloc(v->cap * (int64_t)sizeof(void*));");
        wln!(self, "        if (v->data && v->len > 0) {{ memcpy(new_data, v->data, v->len * sizeof(void*)); }}");
        wln!(self, "        v->data = new_data;");
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
        wln!(self, "    PMap* m = (PMap*)p_malloc(sizeof(PMap));");
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
        wln!(self, "        char** new_keys = (char**)p_malloc(m->cap * (int64_t)sizeof(char*));");
        wln!(self, "        int64_t* new_vals = (int64_t*)p_malloc(m->cap * (int64_t)sizeof(int64_t));");
        wln!(self, "        if (m->keys && m->len > 0) {{ memcpy(new_keys, m->keys, m->len * sizeof(char*)); memcpy(new_vals, m->vals, m->len * sizeof(int64_t)); }}");
        wln!(self, "        m->keys = new_keys;");
        wln!(self, "        m->vals = new_vals;");
        wln!(self, "    }}");
        wln!(self, "    {{ size_t klen = strlen(key); char* kcopy = (char*)p_malloc((int64_t)(klen + 1)); memcpy(kcopy, key, klen + 1); m->keys[m->len] = kcopy; }}");
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
        wln!(self, "// Phase 10B: p_map_keys returns a Fides Vec (i64 pointer)");
        wln!(self, "int64_t p_map_keys(PMap* m) {{");
        wln!(self, "    PVec* v = p_vec_new();");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++)");
        wln!(self, "        p_vec_push(v, (void*)m->keys[i]);");
        wln!(self, "    return (int64_t)v;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "int64_t p_map_values(PMap* m) {{");
        wln!(self, "    PVec* v = p_vec_new();");
        wln!(self, "    for (int64_t i = 0; i < m->len; i++) {{");
        wln!(self, "        p_vec_push(v, (void*)m->vals[i]);");
        wln!(self, "    }}");
        wln!(self, "    return (int64_t)v;");
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
        wln!(self, "    char* buf = (char*)p_malloc(32);");
        wln!(self, "    snprintf(buf, 32, \"%lld\", (long long)val);");
        wln!(self, "    return buf;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "char* p_str_concat(const char* a, const char* b) {{");
        wln!(self, "    size_t la = strlen(a), lb = strlen(b);");
        wln!(self, "    // Issue 25: overflow check before allocation");
        wln!(self, "    if (la > SIZE_MAX - lb - 1) {{ char* r = (char*)p_malloc(1); r[0] = 0; return r; }}");
        wln!(self, "    size_t len = la + lb;");
        wln!(self, "    char* result = (char*)p_malloc((int64_t)(len + 1));");
        wln!(self, "    if (!result) {{ char* r = (char*)p_malloc(1); r[0] = 0; return r; }}");
        wln!(self, "    strcpy(result, a);");
        wln!(self, "    strcat(result, b);");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "");
        // Efficient join: calculates total length first, allocates once, copies.
        wln!(self, "char* p_str_join_n(PVec* parts) {{");
        wln!(self, "    size_t total = 1;");
        wln!(self, "    for (int64_t i = 0; i < parts->len; i++) {{");
        wln!(self, "        size_t pl = strlen((char*)parts->data[i]);");
        wln!(self, "        if (pl > SIZE_MAX - total) pl = SIZE_MAX - total; // Issue 25: overflow guard");
        wln!(self, "        total += pl;");
        wln!(self, "    }}");
        wln!(self, "    char* result = (char*)p_malloc((int64_t)total);");
        wln!(self, "    if (!result) {{ char* r = (char*)p_malloc(1); r[0] = 0; return r; }}");
        wln!(self, "    result[0] = 0;");
        wln!(self, "    for (int64_t i = 0; i < parts->len; i++) {{");
        wln!(self, "        strcat(result, (char*)parts->data[i]);");
        wln!(self, "    }}");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "");
        // Mutable string builder — uses malloc (not arena) for efficient appending.
        // Avoids the quadratic memory blowup from p_str_concat in long sequences.
        wln!(self, "typedef struct {{ char* buf; int64_t len; int64_t cap; }} p_sb;");
        wln!(self, "void* p_sb_new(void) {{");
        wln!(self, "    p_sb* sb = (p_sb*)malloc(sizeof(p_sb));");
        wln!(self, "    sb->cap = 4096;");
        wln!(self, "    sb->len = 0;");
        wln!(self, "    sb->buf = (char*)malloc(sb->cap);");
        wln!(self, "    sb->buf[0] = 0;");
        wln!(self, "    return sb;");
        wln!(self, "}}");
        wln!(self, "void p_sb_append(void* vsb, const char* s) {{");
        wln!(self, "    p_sb* sb = (p_sb*)vsb;");
        wln!(self, "    size_t slen = strlen(s);");
        wln!(self, "    if (sb->len + (int64_t)slen + 1 > sb->cap) {{");
        wln!(self, "        while (sb->len + (int64_t)slen + 1 > sb->cap) sb->cap *= 2;");
        wln!(self, "        sb->buf = (char*)realloc(sb->buf, sb->cap);");
        wln!(self, "    }}");
        wln!(self, "    memcpy(sb->buf + sb->len, s, slen);");
        wln!(self, "    sb->len += (int64_t)slen;");
        wln!(self, "    sb->buf[sb->len] = 0;");
        wln!(self, "}}");
        wln!(self, "const char* p_sb_to_str(void* vsb) {{");
        wln!(self, "    p_sb* sb = (p_sb*)vsb;");
        wln!(self, "    size_t len = (size_t)sb->len;");
        wln!(self, "    char* result = (char*)p_malloc((int64_t)(len + 1));");
        wln!(self, "    memcpy(result, sb->buf, len);");
        wln!(self, "    result[len] = 0;");
        wln!(self, "    free(sb->buf);");
        wln!(self, "    free(sb);");
        wln!(self, "    return result;");
        wln!(self, "}}");
        wln!(self, "");
        wln!(self, "void p_print(int64_t val) { printf(\"%lld\", (long long)val); }");
        wln!(self, "void p_print_str(const char* s) { printf(\"%s\", s); }");
        wln!(self, "void p_eprint_str(const char* s) { fprintf(stderr, \"%s\", s); fflush(stderr); }");
        wln!(self, "void p_flush_stdout(void) { fflush(stdout); }");
        wln!(self, "int64_t p_stdin_eof(void) { return (int64_t)feof(stdin); }");
        wln!(self, "char* p_read_line(void) {");
        wln!(self, "    static char buf[65536];");
        wln!(self, "    if (fgets(buf, sizeof(buf), stdin) == NULL) return NULL;");
        wln!(self, "    size_t len = strlen(buf);");
        wln!(self, "    if (len > 0 && buf[len-1] == '\\n') buf[len-1] = '\\0';");
        wln!(self, "    return buf;");
        wln!(self, "}");
        wln!(self, "char* p_read_stdin(int64_t n) {");
        wln!(self, "    char* buf = (char*)malloc(n + 1);");
        wln!(self, "    if (!buf) return NULL;");
        wln!(self, "    int64_t read = 0;");
        wln!(self, "    while (read < n) {");
        wln!(self, "        int c = fgetc(stdin);");
        wln!(self, "        if (c == EOF) break;");
        wln!(self, "        buf[read] = (char)c;");
        wln!(self, "        read++;");
        wln!(self, "    }");
        wln!(self, "    buf[read] = '\\0';");
        wln!(self, "    return buf;");
        wln!(self, "}");
        wln!(self, "void p_print_int(int64_t val) { printf(\"%lld\", (long long)val); }");
        wln!(self, "void p_print_bool(bool val) { printf(\"%s\", val ? \"true\" : \"false\"); }");
        wln!(self, "void p_print_float(double val) {{ char* _s = p_to_str_float(val); printf(\"%s\", _s); free(_s); }}");
        wln!(self, "void p_print_newline(void) { printf(\"\\n\"); }");
        wln!(self, "void p_panic(const char* msg) {{");
        wln!(self, "    fprintf(stderr, \"Panic: %s\\n\", msg);");
        wln!(self, "    if (p_call_stack_depth > 0) {{");
        wln!(self, "        fprintf(stderr, \"stack trace:\\n\");");
        wln!(self, "        for (int i = p_call_stack_depth - 1; i >= 0; i--) {{");
        wln!(self, "            if (i == p_call_stack_depth - 1)");
        wln!(self, "                fprintf(stderr, \"    → %s\\n\", p_call_stack[i]);");
        wln!(self, "            else");
        wln!(self, "                fprintf(stderr, \"    ↳ called from %s\\n\", p_call_stack[i]);");
        wln!(self, "        }}");
        wln!(self, "    }}");
        wln!(self, "    fflush(stdout);");
        wln!(self, "    exit(1);");
        wln!(self, "}}");
        // Chunk-based arena: linked list of fixed-size blocks.
        // New chunks are added to the chain when the current chunk is full.
        // This NEVER invalidates existing pointers (unlike realloc-based growth).
        // p_arena_save/restore work by tracking the chunk index + offset.
        wln!(self, "#define __P_ARENA_CHUNK_SIZE (4*1024*1024)");
        wln!(self, "typedef struct __p_arena_chunk { char* base; char* ptr; char* end; struct __p_arena_chunk* next; } __p_arena_chunk;");
        wln!(self, "static P_THREAD_LOCAL __p_arena_chunk* __p_arena_head = NULL;");
        wln!(self, "static P_THREAD_LOCAL __p_arena_chunk* __p_arena_cur = NULL;");
        wln!(self, "static __p_arena_chunk* __p_arena_new_chunk(void) {");
        wln!(self, "    __p_arena_chunk* c = (__p_arena_chunk*)malloc(sizeof(__p_arena_chunk));");
        wln!(self, "    if (!c) return NULL;");
        wln!(self, "    c->base = (char*)malloc(__P_ARENA_CHUNK_SIZE);");
        wln!(self, "    if (!c->base) { free(c); return NULL; }");
        wln!(self, "    c->ptr = c->base;");
        wln!(self, "    c->end = c->base + __P_ARENA_CHUNK_SIZE;");
        wln!(self, "    c->next = NULL;");
        wln!(self, "    return c;");
        wln!(self, "}");
        wln!(self, "void* p_malloc(int64_t size) {");
        wln!(self, "    if (size < 0) { fprintf(stderr, \"p_malloc: negative size %lld\\n\", (long long)size); abort(); }");
        wln!(self, "    size_t sz = (size_t)size;");
        wln!(self, "    sz = (sz + 7) & ~((size_t)7);");
        wln!(self, "    if (sz > __P_ARENA_CHUNK_SIZE) {");
        wln!(self, "        __p_arena_chunk* c = (__p_arena_chunk*)malloc(sizeof(__p_arena_chunk));");
        wln!(self, "        if (!c) { fprintf(stderr, \"p_malloc: OOM allocating chunk metadata (%zu bytes)\\n\", sz); abort(); }");
        wln!(self, "        c->base = (char*)malloc(sz);");
        wln!(self, "        if (!c->base) { free(c); fprintf(stderr, \"p_malloc: OOM allocating %zu bytes\\n\", sz); abort(); }");
        wln!(self, "        c->ptr = c->base + sz;");
        wln!(self, "        c->end = c->base + sz;");
        wln!(self, "        c->next = NULL;");
        wln!(self, "        if (__p_arena_cur) __p_arena_cur->next = c; else __p_arena_head = c;");
        wln!(self, "        __p_arena_cur = c;");
        wln!(self, "        return c->base;");
        wln!(self, "    }");
        wln!(self, "    if (!__p_arena_cur) { __p_arena_cur = __p_arena_new_chunk(); __p_arena_head = __p_arena_cur; }");
        wln!(self, "    if (!__p_arena_cur) { fprintf(stderr, \"p_malloc: OOM — failed to create first arena chunk\\n\"); abort(); }");
        wln!(self, "    if (__p_arena_cur->ptr + sz > __p_arena_cur->end) {");
        wln!(self, "        __p_arena_chunk* nc = __p_arena_new_chunk();");
        wln!(self, "        if (!nc) { fprintf(stderr, \"p_malloc: OOM — failed to allocate new arena chunk\\n\"); abort(); }");
        wln!(self, "        __p_arena_cur->next = nc;");
        wln!(self, "        __p_arena_cur = nc;");
        wln!(self, "    }");
        wln!(self, "    void* result = __p_arena_cur->ptr;");
        wln!(self, "    __p_arena_cur->ptr += sz;");
        wln!(self, "    return result;");
        wln!(self, "}");
        wln!(self, "void* p_malloc_persistent(int64_t size) { if (size < 0) return NULL; return malloc((size_t)size); }");
        wln!(self, "void* p_alloc(int64_t size) { return p_malloc(size); }");
        // Note: p_alloc is also forward-declared as int64_t p_alloc(int64_t) from
        // the fn_param_types table. TCC treats void* and int64_t as compatible
        // pointer-sized types, but to be safe we match the forward declaration.
        wln!(self, "void p_free(void* ptr) { /* arena memory — no individual frees */ }");
        // stdlib regions allocate from the arena, so it releases them too.
        wln!(self, "void p_region_free(void* region) { (void)region; }");
        wln!(self, "void p_memset(void* ptr, int val, int64_t size) { if (ptr && size > 0) memset(ptr, val, (size_t)size); }");
        // @zeroize: wipe a string's bytes when it lives in the arena (string
        // literals are read-only and part of the binary anyway), and a Vec's
        // whole element buffer. Volatile stores so the wipe is not elided.
        wln!(self, "void p_str_zeroize(void* s) {{");
        wln!(self, "    if (!s) return;");
        wln!(self, "    for (__p_arena_chunk* c = __p_arena_head; c; c = c->next) {{");
        wln!(self, "        if ((char*)s >= c->base && (char*)s < c->end) {{");
        wln!(self, "            volatile char* p = (volatile char*)s;");
        wln!(self, "            while (p < (volatile char*)c->end && *p) {{ *p = 0; p++; }}");
        wln!(self, "            return;");
        wln!(self, "        }}");
        wln!(self, "    }}");
        wln!(self, "}}");
        wln!(self, "void p_vec_zeroize(void* v) {{");
        wln!(self, "    PVec* pv = (PVec*)v;");
        wln!(self, "    if (!pv || !pv->data) return;");
        wln!(self, "    volatile char* p = (volatile char*)pv->data;");
        wln!(self, "    for (int64_t i = 0; i < pv->cap * (int64_t)sizeof(void*); i++) p[i] = 0;");
        wln!(self, "}}");
        // Arena mark: (1-based chunk index << 32) | offset in that chunk.
        // Restoring frees the chunks after the marked one: they only hold
        // memory allocated after the save.
        wln!(self, "int64_t p_arena_save(void) {");
        wln!(self, "    int64_t idx = 0;");
        wln!(self, "    for (__p_arena_chunk* c = __p_arena_head; c; c = c->next) {");
        wln!(self, "        idx++;");
        wln!(self, "        if (c == __p_arena_cur) return (idx << 32) | (int64_t)(c->ptr - c->base);");
        wln!(self, "    }");
        wln!(self, "    return 0;");
        wln!(self, "}");
        wln!(self, "void p_arena_restore(int64_t mark) {");
        wln!(self, "    int64_t idx = mark >> 32, off = mark & 0xffffffffLL, i = 0;");
        wln!(self, "    __p_arena_chunk* keep = __p_arena_head;");
        wln!(self, "    if (!keep) return;");
        wln!(self, "    if (idx == 0) { keep->ptr = keep->base; }");
        wln!(self, "    else {");
        wln!(self, "        for (__p_arena_chunk* c = __p_arena_head; c; c = c->next) { if (++i == idx) { keep = c; break; } }");
        wln!(self, "        if (i != idx) return;");
        wln!(self, "        keep->ptr = keep->base + off;");
        wln!(self, "    }");
        wln!(self, "    __p_arena_chunk* c = keep->next;");
        wln!(self, "    keep->next = NULL;");
        wln!(self, "    while (c) { __p_arena_chunk* n = c->next; free(c->base); free(c); c = n; }");
        wln!(self, "    __p_arena_cur = keep;");
        wln!(self, "}");
        // File I/O builtins
        wln!(self, "char* p_file_read(const char* path) {{");
        wln!(self, "    FILE* f = fopen(path, \"rb\");");
        wln!(self, "    if (!f) return NULL;");
        wln!(self, "    fseek(f, 0, SEEK_END); long sz = ftell(f);");
        wln!(self, "    if (sz < 0) {{ fclose(f); return NULL; }}");
        wln!(self, "    fseek(f, 0, SEEK_SET);");
        wln!(self, "    char* buf = (char*)malloc((size_t)sz + 1);");
        wln!(self, "    if (!buf) {{ fclose(f); return NULL; }}");
        wln!(self, "    size_t rd = fread(buf, 1, (size_t)sz, f); buf[rd] = 0; fclose(f);");
        wln!(self, "    return buf;");
        wln!(self, "}}");
        wln!(self, "int64_t p_file_write(const char* path, const char* content) {{");
        wln!(self, "    FILE* f = fopen(path, \"wb\");");
        wln!(self, "    if (!f) return -1;");
        wln!(self, "    fputs(content, f); fclose(f); return 0;");
        wln!(self, "}}");
        wln!(self, "int64_t p_file_write_bin(const char* path, const char* data, int64_t len) {{");
        wln!(self, "    FILE* f = fopen(path, \"wb\");");
        wln!(self, "    if (!f) return -1;");
        wln!(self, "    fwrite(data, 1, (size_t)len, f); fclose(f); return 0;");
        wln!(self, "}}");
        // Every byte of a file as one Vec element (binary-safe: keeps 0x00).
        wln!(self, "PVec* p_file_read_bytes(const char* path) {{");
        wln!(self, "    PVec* v = p_vec_new();");
        wln!(self, "    FILE* f = fopen(path, \"rb\");");
        wln!(self, "    if (!f) return v;");
        wln!(self, "    int c;");
        wln!(self, "    while ((c = fgetc(f)) != EOF) p_vec_push(v, (void*)(intptr_t)c);");
        wln!(self, "    fclose(f); return v;");
        wln!(self, "}}");
        wln!(self, "int64_t p_file_write_vec(const char* path, PVec* v) {{");
        wln!(self, "    FILE* f = fopen(path, \"wb\");");
        wln!(self, "    if (!f) return -1;");
        wln!(self, "    if (v) {{ int64_t i; for (i = 0; i < v->len; i++) {{ unsigned char b = (unsigned char)(int64_t)v->data[i]; fwrite(&b, 1, 1, f); }} }}");
        wln!(self, "    fclose(f); return 0;");
        wln!(self, "}}");
        wln!(self, "bool p_file_exists(const char* path) {{ FILE* f = fopen(path, \"rb\"); if (f) {{ fclose(f); return true; }} return false; }}");
        wln!(self, "int64_t p_file_append(const char* path, const char* content) {{");
        wln!(self, "    FILE* f = fopen(path, \"ab\");");
        wln!(self, "    if (!f) return -1;");
        wln!(self, "    fputs(content, f); fclose(f); return 0;");
        wln!(self, "}}");
        wln!(self, "int64_t p_file_delete(const char* path) {{ return remove(path) == 0 ? 0 : -1; }}");
        wln!(self, "int64_t p_file_size(const char* path) {{ FILE* f = fopen(path, \"rb\"); if (!f) return -1; fseek(f, 0, SEEK_END); int64_t sz = ftell(f); fclose(f); return sz < 0 ? -1 : sz; }}");
        wln!(self, "int64_t p_file_hash(const char* path) { FILE* f = fopen(path, \"rb\"); if (!f) return 0; uint64_t h = 0xcbf29ce484222325ULL; unsigned char buf[65536]; size_t n; while ((n = fread(buf, 1, sizeof buf, f)) > 0) { for (size_t i = 0; i < n; i++) { h ^= buf[i]; h *= 0x100000001b3ULL; } } fclose(f); return (int64_t)h; }");
        // Environment variable builtins
        wln!(self, "char* p_getenv(const char* name) {{ return getenv(name); }}");
        // Command-line argument builtins — globals declared in emit_runtime_types
        wln!(self, "int64_t p_argc(void) {{ return (int64_t)__p_argc; }}");
        wln!(self, "char* p_argv(int64_t index) {{ if (index < 0 || index >= __p_argc) return NULL; return __p_argv[index]; }}");
        // Process control builtins
        wln!(self, "int64_t p_system(const char* cmd) {{ return (int64_t)system(cmd); }}");
        wln!(self, "int64_t p_getpid(void) {{");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "    return (int64_t)_getpid();");
        wln!(self, "#else");
        wln!(self, "    return (int64_t)getpid();");
        wln!(self, "#endif");
        wln!(self, "}}");
        // Captured subprocess results: run stores (out, err, code) in a slot
        // table; the readers return the fields; close frees the slot.
        wln!(self, "typedef struct {{ char* out; char* err; int64_t code; int used; }} PProc;");
        wln!(self, "static PProc __p_proc_table[128];");
        wln!(self, "static char* __p_read_all(const char* path) {{");
        wln!(self, "    FILE* f = fopen(path, \"rb\");");
        wln!(self, "    if (!f) {{ char* e = (char*)malloc(1); e[0] = 0; return e; }}");
        wln!(self, "    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);");
        wln!(self, "    if (n < 0) n = 0;");
        wln!(self, "    char* buf = (char*)malloc((size_t)n + 1);");
        wln!(self, "    size_t got = fread(buf, 1, (size_t)n, f);");
        wln!(self, "    buf[got] = 0; fclose(f); return buf;");
        wln!(self, "}}");
        wln!(self, "int64_t p_process_run(const char* cmd) {{");
        wln!(self, "    if (!cmd || !*cmd) return 0;");
        wln!(self, "    static int ctr = 0;");
        wln!(self, "    int slot = -1;");
        wln!(self, "    for (int i = 0; i < 128; i++) if (!__p_proc_table[i].used) {{ slot = i; break; }}");
        wln!(self, "    if (slot < 0) return 0;");
        wln!(self, "    char of[128], ef[128], full[8192];");
        wln!(self, "    snprintf(of, sizeof of, \".fides_proc_%ld_%d.out\", (long)p_getpid(), ctr);");
        wln!(self, "    snprintf(ef, sizeof ef, \".fides_proc_%ld_%d.err\", (long)p_getpid(), ctr);");
        wln!(self, "    ctr++;");
        wln!(self, "    snprintf(full, sizeof full, \"( %s ) > \\\"%s\\\" 2> \\\"%s\\\"\", cmd, of, ef);");
        wln!(self, "    int st = system(full);");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "    __p_proc_table[slot].code = (int64_t)st;");
        wln!(self, "#else");
        wln!(self, "    __p_proc_table[slot].code = WIFEXITED(st) ? (int64_t)WEXITSTATUS(st) : -1;");
        wln!(self, "#endif");
        wln!(self, "    __p_proc_table[slot].out = __p_read_all(of);");
        wln!(self, "    __p_proc_table[slot].err = __p_read_all(ef);");
        wln!(self, "    remove(of); remove(ef);");
        wln!(self, "    __p_proc_table[slot].used = 1;");
        wln!(self, "    return (int64_t)slot + 1;");
        wln!(self, "}}");
        wln!(self, "char* p_process_stdout(int64_t h) {{");
        wln!(self, "    if (h < 1 || h > 128 || !__p_proc_table[h-1].used) return \"\";");
        wln!(self, "    return __p_proc_table[h-1].out;");
        wln!(self, "}}");
        wln!(self, "char* p_process_stderr(int64_t h) {{");
        wln!(self, "    if (h < 1 || h > 128 || !__p_proc_table[h-1].used) return \"\";");
        wln!(self, "    return __p_proc_table[h-1].err;");
        wln!(self, "}}");
        wln!(self, "int64_t p_process_code(int64_t h) {{");
        wln!(self, "    if (h < 1 || h > 128 || !__p_proc_table[h-1].used) return -1;");
        wln!(self, "    return __p_proc_table[h-1].code;");
        wln!(self, "}}");
        wln!(self, "void p_process_close(int64_t h) {{");
        wln!(self, "    if (h < 1 || h > 128 || !__p_proc_table[h-1].used) return;");
        wln!(self, "    free(__p_proc_table[h-1].out); free(__p_proc_table[h-1].err);");
        wln!(self, "    __p_proc_table[h-1].used = 0;");
        wln!(self, "}}");
        wln!(self, "void p_exit(int64_t code) {{ exit((int)code); }}");
        // Filesystem builtins beyond p_file_*
        wln!(self, "int64_t p_mkdir(const char* path) {{");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "    return _mkdir(path) == 0 ? 0 : -1;");
        wln!(self, "#else");
        wln!(self, "    return mkdir(path, 0755) == 0 ? 0 : -1;");
        wln!(self, "#endif");
        wln!(self, "}}");
        wln!(self, "int64_t p_rmdir(const char* path) {{");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "    return _rmdir(path) == 0 ? 0 : -1;");
        wln!(self, "#else");
        wln!(self, "    return rmdir(path) == 0 ? 0 : -1;");
        wln!(self, "#endif");
        wln!(self, "}}");
        wln!(self, "int64_t p_rename(const char* a, const char* b) {{ return rename(a, b) == 0 ? 0 : -1; }}");
        wln!(self, "bool p_is_dir(const char* path) {{");
        wln!(self, "    struct stat st;");
        wln!(self, "    if (stat(path, &st) != 0) return false;");
        wln!(self, "    return (st.st_mode & S_IFMT) == S_IFDIR;");
        wln!(self, "}}");
        wln!(self, "char* p_dir_list(const char* path) {{");
        wln!(self, "    // entry names joined by '\\n', no \".\"/\"..\"; \"\" on failure");
        wln!(self, "    size_t need = 1;");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "    WIN32_FIND_DATAA fd; char pat[2056];");
        wln!(self, "    snprintf(pat, sizeof pat, \"%s\\\\*\", path);");
        wln!(self, "    HANDLE h = FindFirstFileA(pat, &fd);");
        wln!(self, "    if (h == INVALID_HANDLE_VALUE) {{ char* e = (char*)malloc(1); e[0] = 0; return e; }}");
        wln!(self, "    do {{");
        wln!(self, "        if (strcmp(fd.cFileName, \".\") && strcmp(fd.cFileName, \"..\"))");
        wln!(self, "            need += strlen(fd.cFileName) + 1;");
        wln!(self, "    }} while (FindNextFileA(h, &fd));");
        wln!(self, "    FindClose(h);");
        wln!(self, "    char* out = (char*)malloc(need);");
        wln!(self, "    out[0] = 0;");
        wln!(self, "    h = FindFirstFileA(pat, &fd);");
        wln!(self, "    if (h == INVALID_HANDLE_VALUE) return out;");
        wln!(self, "    int first = 1;");
        wln!(self, "    do {{");
        wln!(self, "        if (strcmp(fd.cFileName, \".\") && strcmp(fd.cFileName, \"..\")) {{");
        wln!(self, "            if (!first) strcat(out, \"\\n\");");
        wln!(self, "            strcat(out, fd.cFileName); first = 0;");
        wln!(self, "        }}");
        wln!(self, "    }} while (FindNextFileA(h, &fd));");
        wln!(self, "    FindClose(h);");
        wln!(self, "    return out;");
        wln!(self, "#else");
        wln!(self, "    DIR* d = opendir(path);");
        wln!(self, "    if (!d) {{ char* e = (char*)malloc(1); e[0] = 0; return e; }}");
        wln!(self, "    struct dirent* ent;");
        wln!(self, "    while ((ent = readdir(d)) != NULL) {{");
        wln!(self, "        if (strcmp(ent->d_name, \".\") && strcmp(ent->d_name, \"..\"))");
        wln!(self, "            need += strlen(ent->d_name) + 1;");
        wln!(self, "    }}");
        wln!(self, "    rewinddir(d);");
        wln!(self, "    char* out = (char*)malloc(need);");
        wln!(self, "    out[0] = 0;");
        wln!(self, "    int first = 1;");
        wln!(self, "    while ((ent = readdir(d)) != NULL) {{");
        wln!(self, "        if (strcmp(ent->d_name, \".\") && strcmp(ent->d_name, \"..\")) {{");
        wln!(self, "            if (!first) strcat(out, \"\\n\");");
        wln!(self, "            strcat(out, ent->d_name); first = 0;");
        wln!(self, "        }}");
        wln!(self, "    }}");
        wln!(self, "    closedir(d);");
        wln!(self, "    return out;");
        wln!(self, "#endif");
        wln!(self, "}}");
        // Wall-clock timing builtins — use clock_gettime(CLOCK_REALTIME)
        wln!(self, "#if defined(__linux__) || defined(__APPLE__)");
        wln!(self, "#include <time.h>");
        wln!(self, "int64_t p_clock_now(void) {{ struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); return (int64_t)ts.tv_sec * 1000000000LL + (int64_t)ts.tv_nsec; }}");
        wln!(self, "int64_t p_clock_now_us(void) {{ struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); return (int64_t)ts.tv_sec * 1000000LL + (int64_t)ts.tv_nsec / 1000; }}");
        wln!(self, "int64_t p_clock_now_ms(void) {{ struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); return (int64_t)ts.tv_sec * 1000LL + (int64_t)ts.tv_nsec / 1000000; }}");
        wln!(self, "#else");
        wln!(self, "#include <time.h>");
        wln!(self, "int64_t p_clock_now(void) {{ return (int64_t)time(NULL) * 1000000000LL; }}");
        wln!(self, "int64_t p_clock_now_us(void) {{ return (int64_t)time(NULL) * 1000000LL; }}");
        wln!(self, "int64_t p_clock_now_ms(void) {{ return (int64_t)time(NULL) * 1000LL; }}");
        wln!(self, "#endif");
        // Regex builtin — simple wildcard match (? and *)
        wln!(self, "bool p_regex_match(const char* s, const char* pattern) {{");
        wln!(self, "    if (!s || !pattern) return false;");
        wln!(self, "    int64_t si = 0, pi = 0, s_len = (int64_t)strlen(s), p_len = (int64_t)strlen(pattern);");
        wln!(self, "    int64_t star_s = -1, star_p = -1;");
        wln!(self, "    while (si < s_len) {{");
        wln!(self, "        if (pi < p_len && (pattern[pi] == '?' || pattern[pi] == s[si])) {{ si++; pi++; }}");
        wln!(self, "        else if (pi < p_len && pattern[pi] == '*') {{ star_p = pi; star_s = si; pi++; }}");
        wln!(self, "        else if (star_p >= 0) {{ pi = star_p + 1; si = star_s + 1; star_s = si; }}");
        wln!(self, "        else return false;");
        wln!(self, "    }}");
        wln!(self, "    while (pi < p_len && pattern[pi] == '*') pi++;");
        wln!(self, "    return pi == p_len;");
        wln!(self, "}}");
        // ── Global string interner ──────────────────────────────────────────
        wln!(self, "static char** __p_intern_table = NULL;");
        wln!(self, "static int64_t __p_intern_cap = 0;");
        wln!(self, "static int64_t __p_intern_count = 0;");
        wln!(self, "static int64_t __p_intern_hash(const char* s) {{");
        wln!(self, "    int64_t h = 1469598103934665603ULL;");
        wln!(self, "    while (*s) {{ h ^= (unsigned char)*s; h *= 1099511628211ULL; s++; }}");
        wln!(self, "    return h;");
        wln!(self, "}}");
        wln!(self, "static void __p_intern_grow(void) {{");
        wln!(self, "    int64_t new_cap = __p_intern_cap == 0 ? 4096 : __p_intern_cap * 2;");
        wln!(self, "    char** new_table = (char**)calloc(new_cap, sizeof(char*));");
        wln!(self, "    if (!new_table) {{ fprintf(stderr, \"p_intern: OOM\\n\"); abort(); }}");
        wln!(self, "    for (int64_t i = 0; i < __p_intern_cap; i++) {{");
        wln!(self, "        if (__p_intern_table[i]) {{");
        wln!(self, "            int64_t h = __p_intern_hash(__p_intern_table[i]) & (new_cap - 1);");
        wln!(self, "            while (new_table[h]) h = (h + 1) & (new_cap - 1);");
        wln!(self, "            new_table[h] = __p_intern_table[i];");
        wln!(self, "        }}");
        wln!(self, "    }}");
        wln!(self, "    free(__p_intern_table);");
        wln!(self, "    __p_intern_table = new_table;");
        wln!(self, "    __p_intern_cap = new_cap;");
        wln!(self, "}}");
        wln!(self, "const char* p_intern_str(const char* s) {{");
        wln!(self, "    if (!s) return NULL;");
        wln!(self, "    if (__p_intern_count * 2 >= __p_intern_cap) __p_intern_grow();");
        wln!(self, "    int64_t mask = __p_intern_cap - 1;");
        wln!(self, "    int64_t h = __p_intern_hash(s) & mask;");
        wln!(self, "    while (__p_intern_table[h]) {{");
        wln!(self, "        if (strcmp(__p_intern_table[h], s) == 0) {{");
        wln!(self, "            return __p_intern_table[h];");
        wln!(self, "        }}");
        wln!(self, "        h = (h + 1) & mask;");
        wln!(self, "    }}");
        wln!(self, "    __p_intern_table[h] = (char*)s;");
        wln!(self, "    __p_intern_count++;");
        wln!(self, "    return s;");
        wln!(self, "}}");
        // Async/await runtime (Phase 7.6 + 8A)
        // Future model: p_future_ready wraps a value directly (eager).
        // p_thread_spawn returns a negative handle (-1, -2, ...) for thread-based futures.
        // p_future_poll: if future >= 0, return it (eager). If negative, join the thread.
        wln!(self, "#ifdef _WIN32");
        wln!(self, "typedef void* pthread_t;");
        wln!(self, "typedef struct { void* unused; } pthread_mutex_t;");
        wln!(self, "typedef struct { void* unused; } pthread_cond_t;");
        wln!(self, "typedef int pthread_key_t;");
        wln!(self, "static inline int pthread_create(pthread_t* t, void* attr, void* (*fn)(void*), void* arg) { (void)attr; void* r = fn(arg); *t = r; return 0; }");
        wln!(self, "static inline int pthread_join(pthread_t t, void** ret) { if (ret) *ret = t; return 0; }");
        wln!(self, "static inline int pthread_mutex_init(pthread_mutex_t* m, void* a) { (void)m; (void)a; return 0; }");
        wln!(self, "static inline int pthread_mutex_lock(pthread_mutex_t* m) { (void)m; return 0; }");
        wln!(self, "static inline int pthread_mutex_unlock(pthread_mutex_t* m) { (void)m; return 0; }");
        wln!(self, "static inline int pthread_cond_init(pthread_cond_t* c, void* a) { (void)c; (void)a; return 0; }");
        wln!(self, "static inline int pthread_cond_wait(pthread_cond_t* c, pthread_mutex_t* m) { (void)c; (void)m; return 0; }");
        wln!(self, "static inline int pthread_cond_signal(pthread_cond_t* c) { (void)c; return 0; }");
        wln!(self, "static inline int pthread_cond_broadcast(pthread_cond_t* c) { (void)c; return 0; }");
        wln!(self, "static void* __p_tls_values[1024];");
        wln!(self, "static int __p_tls_next = 0;");
        wln!(self, "static inline int pthread_key_create(pthread_key_t* k, void(*d)(void*)) { (void)d; if (__p_tls_next >= 1024) return -1; *k = __p_tls_next++; return 0; }");
        wln!(self, "static inline void* pthread_getspecific(pthread_key_t k) { return (k >= 0 && k < 1024) ? __p_tls_values[k] : NULL; }");
        wln!(self, "static inline int pthread_setspecific(pthread_key_t k, const void* v) { if (k < 0 || k >= 1024) return -1; __p_tls_values[k] = (void*)v; return 0; }");
        wln!(self, "#else");
        wln!(self, "#include <pthread.h>");
        wln!(self, "#endif");
        wln!(self, "int64_t p_future_ready(int64_t value) { return value; }");
        // Thread join table — maps negative handle to pthread_t
        wln!(self, "static pthread_t p_thread_handles[256];");
        wln!(self, "static int p_thread_handle_count = 0;");
        wln!(self, "static int64_t p_task_results[256];");
        wln!(self, "int64_t p_future_poll(int64_t future) {");
        wln!(self, "    if (future >= 0) return future; // eager future");
        wln!(self, "    // Thread-based future: join and get result");
        wln!(self, "    int idx = -(future + 1);");
        wln!(self, "    if (idx < 0 || idx >= p_thread_handle_count) return 0;");
        wln!(self, "    if (p_thread_handles[idx] == 0) return p_task_results[idx]; // already joined");
        wln!(self, "    void* ret = NULL;");
        wln!(self, "    pthread_join(p_thread_handles[idx], &ret);");
        wln!(self, "    p_task_results[idx] = (int64_t)(intptr_t)ret;");
        wln!(self, "    p_thread_handles[idx] = 0;");
        wln!(self, "    return p_task_results[idx];");
        wln!(self, "}");
        // ─── Phase 8A: Threads + atomics + mutex (using pthread) ───
        wln!(self, "#ifndef _WIN32");
        wln!(self, "#include <stdatomic.h>");
        wln!(self, "#endif");
        wln!(self, "#ifdef _WIN32");
        wln!(self, "typedef long atomic_int;");
        wln!(self, "typedef long long atomic_int_least64_t;");
        wln!(self, "static inline long long _p_atomic_load64(atomic_int_least64_t* a) { return *a; }");
        wln!(self, "static inline void _p_atomic_store64(atomic_int_least64_t* a, long long v) { *a = v; }");
        wln!(self, "static inline long long _p_atomic_add64(atomic_int_least64_t* a, long long v) { long long o = *a; *a += v; return o; }");
        wln!(self, "static inline long long _p_atomic_sub64(atomic_int_least64_t* a, long long v) { long long o = *a; *a -= v; return o; }");
        wln!(self, "static inline int _p_atomic_cas64(atomic_int_least64_t* a, long long* e, long long v) { if (*a == *e) { *a = v; return 1; } *e = *a; return 0; }");
        wln!(self, "#define atomic_load(a) _p_atomic_load64((atomic_int_least64_t*)(a))");
        wln!(self, "#define atomic_store(a, v) _p_atomic_store64((atomic_int_least64_t*)(a), (v))");
        wln!(self, "#define atomic_fetch_add(a, v) _p_atomic_add64((atomic_int_least64_t*)(a), (v))");
        wln!(self, "#define atomic_fetch_sub(a, v) _p_atomic_sub64((atomic_int_least64_t*)(a), (v))");
        wln!(self, "#define atomic_compare_exchange_strong(a, e, v) _p_atomic_cas64((atomic_int_least64_t*)(a), (e), (v))");
        wln!(self, "#define atomic_init(a, v) (*(a) = (v))");
        wln!(self, "#endif");
        wln!(self, "#include <stdarg.h>");
        // Thread entry point — calls a Fides function by name with i64 args.
        // The function lookup uses a simple dispatch table generated below.
        wln!(self, "typedef struct { const char* name; int64_t (*fn)(); } p_thread_entry_t;");
        wln!(self, "static p_thread_entry_t p_thread_entries[2048];");
        wln!(self, "static int p_thread_entry_count = 0;");
        wln!(self, "void p_thread_register(const char* name, int64_t (*fn)()) {");
        wln!(self, "    if (p_thread_entry_count < 2048) {");
        wln!(self, "        p_thread_entries[p_thread_entry_count].name = name;");
        wln!(self, "        p_thread_entries[p_thread_entry_count].fn = fn;");
        wln!(self, "        p_thread_entry_count++;");
        wln!(self, "    }");
        wln!(self, "}");
        wln!(self, "typedef struct { int64_t (*fn)(); int64_t nargs; int64_t args[8]; atomic_int* done; } p_thread_arg_t;");
        wln!(self, "static void* p_thread_wrapper(void* arg) {");
        wln!(self, "    p_thread_arg_t* ta = (p_thread_arg_t*)arg;");
        wln!(self, "    int64_t result = 0;");
        wln!(self, "    // Fides functions have an implicit __env param (void*) as first arg.");
        wln!(self, "    // Pass NULL for __env, then the user-provided args.");
        wln!(self, "    switch (ta->nargs) {");
        wln!(self, "        case 0: { typedef int64_t (*f0)(void*); f0 f = (f0)ta->fn; result = f(NULL); break; }");
        wln!(self, "        case 1: { typedef int64_t (*f1)(void*,int64_t); f1 f = (f1)ta->fn; result = f(NULL, ta->args[0]); break; }");
        wln!(self, "        case 2: { typedef int64_t (*f2)(void*,int64_t,int64_t); f2 f = (f2)ta->fn; result = f(NULL, ta->args[0], ta->args[1]); break; }");
        wln!(self, "        case 3: { typedef int64_t (*f3)(void*,int64_t,int64_t,int64_t); f3 f = (f3)ta->fn; result = f(NULL, ta->args[0], ta->args[1], ta->args[2]); break; }");
        wln!(self, "        default: { typedef int64_t (*f4)(void*,int64_t,int64_t,int64_t,int64_t); f4 f = (f4)ta->fn; result = f(NULL, ta->args[0], ta->args[1], ta->args[2], ta->args[3]); break; }");
        wln!(self, "    }");
        wln!(self, "    if (ta->done) atomic_store(ta->done, 1);");
        wln!(self, "    return (void*)(intptr_t)result;");
        wln!(self, "}");
        // Phase A1: Executor + non-blocking task poll
        // Executor: just an ID — tasks are tracked via the thread handle table.
        // Task done flags: maps future handle (negative index) to done flag.
        wln!(self, "static atomic_int p_task_done_flags[256];");
        wln!(self, "static int p_executor_count = 0;");
        wln!(self, "int64_t p_executor_new(void) { return (int64_t)(p_executor_count++); }");
        wln!(self, "int64_t p_executor_spawn(int64_t exec, const char* func_name, int64_t num_args, ...) {");
        wln!(self, "    (void)exec;");
        wln!(self, "    int64_t (*fn)() = NULL;");
        wln!(self, "    for (int i = 0; i < p_thread_entry_count; i++) {");
        wln!(self, "        if (strcmp(p_thread_entries[i].name, func_name) == 0) { fn = p_thread_entries[i].fn; break; }");
        wln!(self, "    }");
        wln!(self, "    if (!fn) return -1;");
        wln!(self, "    p_thread_arg_t* ta = (p_thread_arg_t*)malloc(sizeof(p_thread_arg_t));");
        wln!(self, "    ta->fn = fn; ta->nargs = num_args;");
        wln!(self, "    va_list ap; va_start(ap, num_args);");
        wln!(self, "    for (int64_t i = 0; i < num_args && i < 8; i++) ta->args[i] = va_arg(ap, int64_t);");
        wln!(self, "    va_end(ap);");
        wln!(self, "    int idx = p_thread_handle_count++;");
        wln!(self, "    atomic_init(&p_task_done_flags[idx], 0);");
        wln!(self, "    ta->done = &p_task_done_flags[idx];");
        wln!(self, "    pthread_t tid; pthread_create(&tid, NULL, p_thread_wrapper, ta);");
        wln!(self, "    p_thread_handles[idx] = tid;");
        wln!(self, "    return -(int64_t)(idx + 1);");
        wln!(self, "}");
        wln!(self, "int64_t p_task_poll(int64_t future) {");
        wln!(self, "    if (future >= 0) return future; // eager future");
        wln!(self, "    int idx = -(future + 1);");
        wln!(self, "    if (idx < 0 || idx >= p_thread_handle_count) return 0;");
        wln!(self, "    if (p_thread_handles[idx] == 0) return p_task_results[idx]; // already joined");
        wln!(self, "    // A1: block until done (join). A2 will add non-blocking poll.");
        wln!(self, "    void* ret = NULL; pthread_join(p_thread_handles[idx], &ret);");
        wln!(self, "    p_task_results[idx] = (int64_t)(intptr_t)ret;");
        wln!(self, "    p_thread_handles[idx] = 0;");
        wln!(self, "    return p_task_results[idx];");
        wln!(self, "}");
        wln!(self, "bool p_task_is_ready(int64_t future) {");
        wln!(self, "    if (future >= 0) return true;");
        wln!(self, "    int idx = -(future + 1);");
        wln!(self, "    if (idx < 0 || idx >= p_thread_handle_count) return false;");
        wln!(self, "    return atomic_load(&p_task_done_flags[idx]) != 0;");
        wln!(self, "}");
        wln!(self, "int64_t p_executor_block_on(int64_t exec, int64_t future) {");
        wln!(self, "    (void)exec;");
        wln!(self, "    return p_future_poll(future);");
        wln!(self, "}");
        // Cancel the tasks of a scope: a task cannot be stopped mid-way, so
        // wait for any still running (no task outlives its scope), drop their
        // results (0, as in the interpreter), and return how many were cancelled.
        wln!(self, "int64_t p_executor_cancel(int64_t exec) {");
        wln!(self, "    (void)exec;");
        wln!(self, "    int64_t count = 0;");
        wln!(self, "    for (int i = 0; i < p_thread_handle_count; i++) {");
        wln!(self, "        if (p_thread_handles[i] == 0) continue; // already joined");
        wln!(self, "        int was_done = atomic_load(&p_task_done_flags[i]);");
        wln!(self, "        void* ret = NULL; pthread_join(p_thread_handles[i], &ret);");
        wln!(self, "        p_task_results[i] = was_done ? (int64_t)(intptr_t)ret : 0;");
        wln!(self, "        if (!was_done) count++;");
        wln!(self, "        p_thread_handles[i] = 0;");
        wln!(self, "    }");
        wln!(self, "    return count;");
        wln!(self, "}");
        wln!(self, "int64_t p_executor_run_all(int64_t exec) {");
        wln!(self, "    (void)exec;");
        wln!(self, "    int64_t count = 0;");
        wln!(self, "    for (int i = 0; i < p_thread_handle_count; i++) {");
        wln!(self, "        if (p_thread_handles[i] == 0) continue; // already joined");
        wln!(self, "        void* ret = NULL; pthread_join(p_thread_handles[i], &ret);");
        wln!(self, "        p_task_results[i] = (int64_t)(intptr_t)ret;");
        wln!(self, "        p_thread_handles[i] = 0; count++;");
        wln!(self, "    }");
        wln!(self, "    return count;");
        wln!(self, "}");
        // A2: Non-blocking poll — returns PENDING_SENTINEL if task not done
        wln!(self, "int64_t p_task_poll_nonblocking(int64_t future) {");
        wln!(self, "    if (future >= 0) return future; // eager future");
        wln!(self, "    int idx = -(future + 1);");
        wln!(self, "    if (idx < 0 || idx >= p_thread_handle_count) return 0;");
        wln!(self, "    if (atomic_load(&p_task_done_flags[idx]) == 0) {");
        wln!(self, "        return (int64_t)0x8000000000000001LL; // PENDING_SENTINEL");
        wln!(self, "    }");
        wln!(self, "    if (p_thread_handles[idx] == 0) return p_task_results[idx]; // already joined");
        wln!(self, "    void* ret = NULL; pthread_join(p_thread_handles[idx], &ret);");
        wln!(self, "    p_task_results[idx] = (int64_t)(intptr_t)ret;");
        wln!(self, "    p_thread_handles[idx] = 0;");
        wln!(self, "    return p_task_results[idx];");
        wln!(self, "}");
        // A2: Non-blocking poll all — returns count of still-pending tasks
        wln!(self, "int64_t p_executor_poll_all(int64_t exec) {");
        wln!(self, "    (void)exec;");
        wln!(self, "    int64_t pending = 0;");
        wln!(self, "    for (int i = 0; i < p_thread_handle_count; i++) {");
        wln!(self, "        if (p_thread_handles[i] == 0) continue; // already joined");
        wln!(self, "        if (atomic_load(&p_task_done_flags[i]) == 0) { pending++; }");
        wln!(self, "        else { void* ret = NULL; pthread_join(p_thread_handles[i], &ret); p_task_results[i] = (int64_t)(intptr_t)ret; p_thread_handles[i] = 0; }");
        wln!(self, "    }");
        wln!(self, "    return pending;");
        wln!(self, "}");
        // ─── Phase A3: Non-blocking IO (using threads for async) ───
        // Each async IO operation spawns a thread that performs the IO
        // and stores the result. The returned Future handle can be polled.
        wln!(self, "#ifndef _WIN32");
        wln!(self, "#include <sys/socket.h>");
        wln!(self, "#include <netinet/in.h>");
        wln!(self, "#include <arpa/inet.h>");
        wln!(self, "#include <unistd.h>");
        wln!(self, "#include <fcntl.h>");
        wln!(self, "#endif");
        // Thread arg for async IO
        wln!(self, "typedef struct { int64_t sock; char* buf; int64_t len; int64_t result; atomic_int* done; int op; } p_io_arg_t;");
        wln!(self, "// op: 0=connect, 1=read, 2=write, 3=fs_read");
        wln!(self, "static void* p_io_worker(void* arg) {");
        wln!(self, "    p_io_arg_t* ia = (p_io_arg_t*)arg;");
        wln!(self, "    int64_t result = 0;");
        wln!(self, "    switch (ia->op) {");
        wln!(self, "        case 1: { // read");
        wln!(self, "            ssize_t n = recv((int)ia->sock, ia->buf, (size_t)ia->len, 0);");
        wln!(self, "            result = (n < 0) ? -1 : (int64_t)n;");
        wln!(self, "            break;");
        wln!(self, "        }");
        wln!(self, "        case 2: { // write");
        wln!(self, "            ssize_t n = send((int)ia->sock, ia->buf, (size_t)ia->len, 0);");
        wln!(self, "            result = (n < 0) ? -1 : (int64_t)n;");
        wln!(self, "            break;");
        wln!(self, "        }");
        wln!(self, "        case 3: { // fs_read");
        wln!(self, "            FILE* f = fopen((const char*)ia->sock, \"rb\");");
        wln!(self, "            if (f) {");
        wln!(self, "                size_t n = fread(ia->buf, 1, (size_t)ia->len, f);");
        wln!(self, "                result = (int64_t)n;");
        wln!(self, "                fclose(f);");
        wln!(self, "            } else { result = -1; }");
        wln!(self, "            break;");
        wln!(self, "        }");
        wln!(self, "        default: result = 0;");
        wln!(self, "    }");
        wln!(self, "    ia->result = result;");
        wln!(self, "    if (ia->done) atomic_store(ia->done, 1);");
        wln!(self, "    return (void*)(intptr_t)result;");
        wln!(self, "}");
        wln!(self, "// Helper: spawn an IO thread and return a Future handle");
        wln!(self, "static int64_t p_io_spawn(int op, int64_t sock, char* buf, int64_t len) {");
        wln!(self, "    int idx = p_thread_handle_count++;");
        wln!(self, "    atomic_init(&p_task_done_flags[idx], 0);");
        wln!(self, "    p_io_arg_t* ia = (p_io_arg_t*)malloc(sizeof(p_io_arg_t));");
        wln!(self, "    ia->sock = sock; ia->buf = buf; ia->len = len; ia->result = 0;");
        wln!(self, "    ia->done = &p_task_done_flags[idx]; ia->op = op;");
        wln!(self, "    pthread_t tid; pthread_create(&tid, NULL, p_io_worker, ia);");
        wln!(self, "    p_thread_handles[idx] = tid;");
        wln!(self, "    p_task_results[idx] = 0; // will be set by p_task_poll after join");
        wln!(self, "    return -(int64_t)(idx + 1);");
        wln!(self, "}");
        wln!(self, "int64_t p_net_connect(const char* host, int64_t port) {");
        wln!(self, "    int sock = socket(AF_INET, SOCK_STREAM, 0);");
        wln!(self, "    if (sock < 0) return -1;");
        wln!(self, "    struct sockaddr_in addr = {0};");
        wln!(self, "    addr.sin_family = AF_INET;");
        wln!(self, "    addr.sin_port = htons((uint16_t)port);");
        wln!(self, "    inet_pton(AF_INET, host, &addr.sin_addr);");
        wln!(self, "    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {");
        wln!(self, "        close(sock); return -1;");
        wln!(self, "    }");
        wln!(self, "    // Return the socket FD as an eager future (positive = ready)");
        wln!(self, "    return (int64_t)sock;");
        wln!(self, "}");
        wln!(self, "int64_t p_net_read(int64_t sock, char* buf, int64_t len) {");
        wln!(self, "    return p_io_spawn(1, sock, buf, len);");
        wln!(self, "}");
        wln!(self, "int64_t p_net_write(int64_t sock, const char* buf, int64_t len) {");
        wln!(self, "    return p_io_spawn(2, sock, (char*)buf, len);");
        wln!(self, "}");
        wln!(self, "void p_net_close(int64_t sock) {");
        wln!(self, "    close((int)sock);");
        wln!(self, "}");
        wln!(self, "int64_t p_fs_read_async(const char* path, char* buf, int64_t len) {");
        wln!(self, "    // Use sock field to pass the path pointer (op=3 reads it as path)");
        wln!(self, "    return p_io_spawn(3, (int64_t)(intptr_t)path, buf, len);");
        wln!(self, "}");
        wln!(self, "int64_t p_thread_spawn(const char* func_name, int64_t num_args, ...) {");
        wln!(self, "    // Look up function by name");
        wln!(self, "    int64_t (*fn)() = NULL;");
        wln!(self, "    for (int i = 0; i < p_thread_entry_count; i++) {");
        wln!(self, "        if (strcmp(p_thread_entries[i].name, func_name) == 0) {");
        wln!(self, "            fn = p_thread_entries[i].fn;");
        wln!(self, "            break;");
        wln!(self, "        }");
        wln!(self, "    }");
        wln!(self, "    if (!fn) return -1;");
        wln!(self, "    p_thread_arg_t* ta = (p_thread_arg_t*)malloc(sizeof(p_thread_arg_t));");
        wln!(self, "    ta->fn = fn;");
        wln!(self, "    ta->nargs = num_args;");
        wln!(self, "    va_list ap; va_start(ap, num_args);");
        wln!(self, "    for (int64_t i = 0; i < num_args && i < 8; i++) ta->args[i] = va_arg(ap, int64_t);");
        wln!(self, "    va_end(ap);");
        wln!(self, "    int idx = p_thread_handle_count++;");
        wln!(self, "    atomic_init(&p_task_done_flags[idx], 0);");
        wln!(self, "    ta->done = &p_task_done_flags[idx];");
        wln!(self, "    pthread_t tid;");
        wln!(self, "    pthread_create(&tid, NULL, p_thread_wrapper, ta);");
        wln!(self, "    // Store thread handle and return negative index as Future");
        wln!(self, "    p_thread_handles[idx] = tid;");
        wln!(self, "    return -(int64_t)(idx + 1);");
        wln!(self, "}");
        // Shared memory — simple malloc-based, atomics use C11 stdatomic
        wln!(self, "int64_t p_shared_alloc(int64_t size) { if (size < 0) return 0; return (int64_t)(intptr_t)malloc((size_t)size); }");
        wln!(self, "int64_t p_atomic_load(int64_t addr) { return atomic_load((atomic_int_least64_t*)(intptr_t)addr); }");
        wln!(self, "void p_atomic_store(int64_t addr, int64_t val) { atomic_store((atomic_int_least64_t*)(intptr_t)addr, val); }");
        wln!(self, "bool p_atomic_cas(int64_t addr, int64_t expected, int64_t new_val) {");
        wln!(self, "    return atomic_compare_exchange_strong((atomic_int_least64_t*)(intptr_t)addr, &expected, new_val);");
        wln!(self, "}");
        wln!(self, "int64_t p_atomic_add(int64_t addr, int64_t val) { return atomic_fetch_add((atomic_int_least64_t*)(intptr_t)addr, val); }");
        wln!(self, "int64_t p_atomic_sub(int64_t addr, int64_t val) { return atomic_fetch_sub((atomic_int_least64_t*)(intptr_t)addr, val); }");
        wln!(self, "int64_t p_rc_incr(int64_t rc_addr, int64_t val) { (*(int64_t*)(intptr_t)rc_addr)++; return val; }");
        wln!(self, "void p_rc_decr(int64_t rc_addr, int64_t val) {");
        wln!(self, "    (void)val;");
        wln!(self, "    int64_t* rc = (int64_t*)(intptr_t)rc_addr;");
        wln!(self, "    (*rc)--;");
        wln!(self, "    if (*rc <= 0) { free((void*)(intptr_t)rc_addr); }");
        wln!(self, "}");
        // Mutex — use pthread_mutex_t allocated on heap
        wln!(self, "int64_t p_mutex_init(void) {");
        wln!(self, "    pthread_mutex_t* m = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));");
        wln!(self, "    pthread_mutex_init(m, NULL);");
        wln!(self, "    return (int64_t)(intptr_t)m;");
        wln!(self, "}");
        wln!(self, "void p_mutex_lock(int64_t m) { pthread_mutex_lock((pthread_mutex_t*)(intptr_t)m); }");
        wln!(self, "void p_mutex_unlock(int64_t m) { pthread_mutex_unlock((pthread_mutex_t*)(intptr_t)m); }");
        // ─── Thread-local storage (pthread keys) ───
        wln!(self, "int64_t p_tls_create(void) {{ pthread_key_t key; pthread_key_create(&key, NULL); return (int64_t)key; }}");
        wln!(self, "int64_t p_tls_get(int64_t key) {{ return (int64_t)(intptr_t)pthread_getspecific((pthread_key_t)key); }}");
        wln!(self, "void p_tls_set(int64_t key, int64_t value) {{ pthread_setspecific((pthread_key_t)key, (void*)(intptr_t)value); }}");
        // ─── Channels (bounded MPSC queue with mutex + condvar) ───
        // Channel layout: { mutex, cond_not_empty, cond_not_full, head, tail, count, capacity, closed, buffer[capacity] }
        // head = next read index, tail = next write index, count = current items
        // (pthread types already defined above)
        wln!(self, "typedef struct {");
        wln!(self, "    pthread_mutex_t mutex;");
        wln!(self, "    pthread_cond_t not_empty;");
        wln!(self, "    pthread_cond_t not_full;");
        wln!(self, "    int64_t head;");
        wln!(self, "    int64_t tail;");
        wln!(self, "    int64_t count;");
        wln!(self, "    int64_t capacity;");
        wln!(self, "    int closed;");
        wln!(self, "    int64_t buffer[]; // flexible array member");
        wln!(self, "} p_channel_t;");
        wln!(self, "// PENDING_SENTINEL — returned by p_chan_try_recv when channel is empty");
        wln!(self, "#define P_CHAN_PENDING ((int64_t)0x8000000000000001LL)");
        wln!(self, "int64_t p_chan_create(int64_t capacity) {");
        wln!(self, "    if (capacity < 1) capacity = 1;");
        wln!(self, "    p_channel_t* ch = (p_channel_t*)malloc(sizeof(p_channel_t) + capacity * sizeof(int64_t));");
        wln!(self, "    pthread_mutex_init(&ch->mutex, NULL);");
        wln!(self, "    pthread_cond_init(&ch->not_empty, NULL);");
        wln!(self, "    pthread_cond_init(&ch->not_full, NULL);");
        wln!(self, "    ch->head = 0; ch->tail = 0; ch->count = 0;");
        wln!(self, "    ch->capacity = capacity;");
        wln!(self, "    ch->closed = 0;");
        wln!(self, "    return (int64_t)(intptr_t)ch;");
        wln!(self, "}");
        wln!(self, "bool p_chan_send(int64_t handle, int64_t val) {");
        wln!(self, "    p_channel_t* ch = (p_channel_t*)(intptr_t)handle;");
        wln!(self, "    if (!ch) return false;");
        wln!(self, "    pthread_mutex_lock(&ch->mutex);");
        wln!(self, "    // Block until there's space or the channel is closed");
        wln!(self, "    while (ch->count == ch->capacity && !ch->closed) {");
        wln!(self, "        pthread_cond_wait(&ch->not_full, &ch->mutex);");
        wln!(self, "    }");
        wln!(self, "    if (ch->closed) {");
        wln!(self, "        pthread_mutex_unlock(&ch->mutex);");
        wln!(self, "        return false;");
        wln!(self, "    }");
        wln!(self, "    ch->buffer[ch->tail] = val;");
        wln!(self, "    ch->tail = (ch->tail + 1) % ch->capacity;");
        wln!(self, "    ch->count++;");
        wln!(self, "    pthread_cond_signal(&ch->not_empty);");
        wln!(self, "    pthread_mutex_unlock(&ch->mutex);");
        wln!(self, "    return true;");
        wln!(self, "}");
        wln!(self, "int64_t p_chan_recv(int64_t handle) {");
        wln!(self, "    p_channel_t* ch = (p_channel_t*)(intptr_t)handle;");
        wln!(self, "    if (!ch) return 0;");
        wln!(self, "    pthread_mutex_lock(&ch->mutex);");
        wln!(self, "    // Block until there's an item or the channel is closed and empty");
        wln!(self, "    while (ch->count == 0 && !ch->closed) {");
        wln!(self, "        pthread_cond_wait(&ch->not_empty, &ch->mutex);");
        wln!(self, "    }");
        wln!(self, "    if (ch->count == 0) {");
        wln!(self, "        // Closed and empty");
        wln!(self, "        pthread_mutex_unlock(&ch->mutex);");
        wln!(self, "        return P_CHAN_PENDING;");
        wln!(self, "    }");
        wln!(self, "    int64_t val = ch->buffer[ch->head];");
        wln!(self, "    ch->head = (ch->head + 1) % ch->capacity;");
        wln!(self, "    ch->count--;");
        wln!(self, "    pthread_cond_signal(&ch->not_full);");
        wln!(self, "    pthread_mutex_unlock(&ch->mutex);");
        wln!(self, "    return val;");
        wln!(self, "}");
        wln!(self, "int64_t p_chan_try_recv(int64_t handle) {");
        wln!(self, "    p_channel_t* ch = (p_channel_t*)(intptr_t)handle;");
        wln!(self, "    if (!ch) return P_CHAN_PENDING;");
        wln!(self, "    pthread_mutex_lock(&ch->mutex);");
        wln!(self, "    if (ch->count == 0) {");
        wln!(self, "        int closed = ch->closed;");
        wln!(self, "        pthread_mutex_unlock(&ch->mutex);");
        wln!(self, "        return closed ? 0 : P_CHAN_PENDING;");
        wln!(self, "    }");
        wln!(self, "    int64_t val = ch->buffer[ch->head];");
        wln!(self, "    ch->head = (ch->head + 1) % ch->capacity;");
        wln!(self, "    ch->count--;");
        wln!(self, "    pthread_cond_signal(&ch->not_full);");
        wln!(self, "    pthread_mutex_unlock(&ch->mutex);");
        wln!(self, "    return val;");
        wln!(self, "}");
        wln!(self, "void p_chan_close(int64_t handle) {");
        wln!(self, "    p_channel_t* ch = (p_channel_t*)(intptr_t)handle;");
        wln!(self, "    if (!ch) return;");
        wln!(self, "    pthread_mutex_lock(&ch->mutex);");
        wln!(self, "    ch->closed = 1;");
        wln!(self, "    // Wake up all blocked senders and receivers");
        wln!(self, "    pthread_cond_broadcast(&ch->not_empty);");
        wln!(self, "    pthread_cond_broadcast(&ch->not_full);");
        wln!(self, "    pthread_mutex_unlock(&ch->mutex);");
        wln!(self, "}");
        wln!(self, "bool p_chan_is_closed(int64_t handle) {");
        wln!(self, "    p_channel_t* ch = (p_channel_t*)(intptr_t)handle;");
        wln!(self, "    if (!ch) return true;");
        wln!(self, "    pthread_mutex_lock(&ch->mutex);");
        wln!(self, "    int closed = ch->closed;");
        wln!(self, "    pthread_mutex_unlock(&ch->mutex);");
        wln!(self, "    return closed;");
        wln!(self, "}");
        // Dynamic loading builtins (Phase 7.7) — simulated to match interpreter
        wln!(self, "// Simulated dlopen: returns sequential handles (1, 2, ...) to match interpreter");
        wln!(self, "static int64_t dl_handle_counter = 0;");
        wln!(self, "int64_t p_dl_open(const char* path) {");
        wln!(self, "    if (!path || !*path) return 0;");
        wln!(self, "    return ++dl_handle_counter;");
        wln!(self, "}");
        wln!(self, "int64_t p_dl_sym(int64_t handle, const char* symbol) {");
        wln!(self, "    if (handle == 0) return 0;");
        wln!(self, "    if (!symbol || !*symbol) return 0;");
        wln!(self, "    return 0x1000 + handle * 0x100;");
        wln!(self, "}");
        wln!(self, "void p_dl_close(int64_t handle) {");
        wln!(self, "    (void)handle; // no-op in simulation");
        wln!(self, "}");
        wln!(self, "int64_t p_dl_call(int64_t func_ptr, int64_t arg) {");
        wln!(self, "    (void)func_ptr; // simulated: return arg unchanged");
        wln!(self, "    return arg;");
        wln!(self, "}");
        // Allocator builtins (Phase 2A.8) — simplified stubs
        wln!(self, "void* p_allocator_create(void* arg) { (void)arg; return (void*)1; }");
        wln!(self, "int64_t p_allocator_alloc(void* alloc, int64_t size) { (void)alloc; if (size < 0) return 0; return (int64_t)(intptr_t)malloc((size_t)size); }");
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
        wln!(self, "    if (!s || n < 0) return strdup(\"\");");
        wln!(self, "    size_t slen = strlen(s);");
        wln!(self, "    if (slen == 0 || n == 0) return strdup(\"\");");
        wln!(self, "    if (slen > SIZE_MAX / (size_t)n) return strdup(\"\");");
        wln!(self, "    size_t total = slen * (size_t)n + 1;");
        wln!(self, "    char* result = (char*)malloc(total);");
        wln!(self, "    if (!result) return strdup(\"\");");
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
        wln!(self, "int64_t p_string_split(const char* s, const char* delim) {{");
        wln!(self, "    PVec* v = p_vec_new();");
        wln!(self, "    if (!s || !delim) return (int64_t)v;");
        wln!(self, "    const char* start = s;");
        wln!(self, "    size_t dlen = strlen(delim);");
        wln!(self, "    while (*start) {{");
        wln!(self, "        const char* p = strstr(start, delim);");
        wln!(self, "        if (!p) {{ p_vec_push(v, (void*)strdup(start)); break; }}");
        wln!(self, "        size_t len = p - start;");
        wln!(self, "        char* part = (char*)malloc(len + 1);");
        wln!(self, "        memcpy(part, start, len); part[len] = 0;");
        wln!(self, "        p_vec_push(v, (void*)part);");
        wln!(self, "        start = p + dlen;");
        wln!(self, "    }}");
        wln!(self, "    return (int64_t)v;");
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
        wln!(self, "int64_t p_vec_capacity(PVec* v) {{ return v ? v->cap : 0; }}");
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
        wln!(self, "char* p_char_to_str(int64_t val) {{ char* s = (char*)malloc(2); s[0] = (char)val; s[1] = 0; return s; }}");
        wln!(self, "char* p_to_str_bool(bool val) {{ return strdup(val ? \"true\" : \"false\"); }}");
        wln!(self, "char* p_to_str_float(double val) {{");
        wln!(self, "    char* buf = (char*)malloc(512);");
        wln!(self, "    // Match Rust's {{}} format: shortest representation that round-trips.");
        wln!(self, "    // Rust's Display for f64 always uses fixed-point (never scientific notation).");
        wln!(self, "    if (val == 0.0) {{ snprintf(buf, 512, \"0\"); return buf; }}");
        wln!(self, "    // Calculate required max precision from exponent.");
        wln!(self, "    // For 2.22e-16, we need prec=32 (16 zeros + 16 sig digits).");
        wln!(self, "    int exp = 0;");
        wln!(self, "    double absval = fabs(val);");
        wln!(self, "    if (absval >= 1.0) {{ while (absval >= 10.0) {{ absval /= 10.0; exp++; }} }}");
        wln!(self, "    else {{ while (absval < 1.0) {{ absval *= 10.0; exp--; }} }}");
        wln!(self, "    int max_prec = 17 + (exp < 0 ? -exp : 0);");
        wln!(self, "    if (max_prec > 350) max_prec = 350;");
        wln!(self, "    // Try increasing precision with %f format (no scientific notation)");
        wln!(self, "    for (int prec = 0; prec <= max_prec; prec++) {{");
        wln!(self, "        snprintf(buf, 512, \"%.*f\", prec, val);");
        wln!(self, "        double parsed = strtod(buf, NULL);");
        wln!(self, "        if (parsed == val) {{");
        wln!(self, "            // Trim trailing zeros after decimal point (e.g., \"10.0\" -> \"10\")");
        wln!(self, "            char* dot = strchr(buf, '.');");
        wln!(self, "            if (dot) {{");
        wln!(self, "                char* end = buf + strlen(buf) - 1;");
        wln!(self, "                while (end > dot && *end == '0') *end-- = '\\0';");
        wln!(self, "                if (end == dot) *end = '\\0'; /* remove trailing dot */");
        wln!(self, "            }}");
        wln!(self, "            return buf;");
        wln!(self, "        }}");
        wln!(self, "    }}");
        wln!(self, "    // If no precision round-trips, use max precision");
        wln!(self, "    snprintf(buf, 512, \"%.*f\", max_prec, val);");
        wln!(self, "    return buf;");
        wln!(self, "}}");
        // Aliases for the new naming convention used by fstring lowering
        wln!(self, "char* p_float_to_str(double val) {{ return p_to_str_float(val); }}");
        wln!(self, "char* p_bool_to_str(bool val) {{ return p_to_str_bool(val); }}");
        wln!(self, "char* p_vec_to_str(PVec* v) {{ return p_to_str_vec(v); }}");
        // SH-4.1: String to float parsing
        wln!(self, "double p_str_to_float(const char* s) {{ if (!s) return 0.0; return atof(s); }}");
        // Float bit-pattern conversion (B5: preserve NaN/Inf/-0.0 in ConstFloat)
        wln!(self, "int64_t p_f64_to_bits(double val) {{ union {{ double d; int64_t i; }} u; u.d = val; return u.i; }}");
        wln!(self, "int64_t p_f32_to_bits(float val) {{ union {{ float f; int32_t i; }} u; u.f = val; return (int64_t)u.i; }}");
        wln!(self, "double p_f64_from_bits(int64_t bits) {{ union {{ double d; int64_t i; }} u; u.i = bits; return u.d; }}");
        wln!(self, "float p_f32_from_bits(int64_t bits) {{ union {{ float f; int32_t i; }} u; u.i = (int32_t)bits; return u.f; }}");
        wln!(self, "double p_int_to_float(int64_t val) {{ return (double)val; }}");
        wln!(self, "int64_t p_float_to_int(double val) {{ return (int64_t)val; }}");
        wln!(self, "int64_t p_str_to_int(const char* s) {{ if (!s) return 0; return atoll(s); }}");

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
        wln!(self, "    if (n >= (int64_t)sizeof(buf)) n = sizeof(buf) - 1;");
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
        // Volatile memory access (MMIO) — uses volatile pointers to prevent optimization
        wln!(self, "int64_t p_volatile_load(int64_t addr) {{ volatile int64_t* p = (volatile int64_t*)(intptr_t)addr; return *p; }}");
        wln!(self, "void p_volatile_store(int64_t addr, int64_t val) {{ volatile int64_t* p = (volatile int64_t*)(intptr_t)addr; *p = val; }}");
        wln!(self, "int64_t p_volatile_load8(int64_t addr) {{ volatile uint8_t* p = (volatile uint8_t*)(intptr_t)addr; return (int64_t)*p; }}");
        wln!(self, "int64_t p_volatile_load16(int64_t addr) {{ volatile uint16_t* p = (volatile uint16_t*)(intptr_t)addr; return (int64_t)*p; }}");
        wln!(self, "int64_t p_volatile_load32(int64_t addr) {{ volatile uint32_t* p = (volatile uint32_t*)(intptr_t)addr; return (int64_t)*p; }}");
        wln!(self, "void p_volatile_store8(int64_t addr, int64_t val) {{ volatile uint8_t* p = (volatile uint8_t*)(intptr_t)addr; *p = (uint8_t)val; }}");
        wln!(self, "void p_volatile_store16(int64_t addr, int64_t val) {{ volatile uint16_t* p = (volatile uint16_t*)(intptr_t)addr; *p = (uint16_t)val; }}");
        wln!(self, "void p_volatile_store32(int64_t addr, int64_t val) {{ volatile uint32_t* p = (volatile uint32_t*)(intptr_t)addr; *p = (uint32_t)val; }}");
        wln!(self, "void p_lgdt(int64_t gdt_desc) {{ (void)gdt_desc; }}");
        wln!(self, "void p_gdt_set_entry(int64_t gdt_ptr, int64_t index, int64_t base, int64_t limit, int64_t flags) {{ (void)gdt_ptr; (void)index; (void)base; (void)limit; (void)flags; }}");
        wln!(self, "void p_serial_init(void) {{}}");
        wln!(self, "void p_serial_write(int64_t c) {{ (void)c; }}");
        wln!(self, "int64_t p_serial_read(void) {{ return 0; }}");
        wln!(self, "int64_t p_serial_ready(void) {{ return 0; }}");

        // ─── SIMD builtins (Phase 5C) — operate on Vec<f64> ───
        wln!(self, "");
        wln!(self, "// SIMD builtins — operate on Fides Vec (i64 pointer, f64 stored as i64 bits)");
        wln!(self, "// Helper: read double from Fides Vec element (stored as i64 bit pattern)");
        self.out.push_str("static double _fvec_get_f64(int64_t v, int i) {\n");
        self.out.push_str("    int64_t val = (int64_t)p_vec_get((PVec*)v, i);\n");
        self.out.push_str("    union { int64_t i; double d; } u; u.i = val; return u.d;\n");
        self.out.push_str("}\n");
        self.out.push_str("static int64_t _fvec_new() { return (int64_t)p_vec_new(); }\n");
        self.out.push_str("static void _fvec_push_f64(int64_t v, double d) {\n");
        self.out.push_str("    union { double d; int64_t i; } u; u.d = d; p_vec_push((PVec*)v, (void*)u.i);\n");
        self.out.push_str("}\n");
        wln!(self, "int64_t p_simd_add(int64_t a, int64_t b) {{");
        wln!(self, "    int64_t r = _fvec_new();");
        wln!(self, "    int n = (int)p_vec_len((PVec*)a);");
        wln!(self, "    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) + _fvec_get_f64(b,i));");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "int64_t p_simd_sub(int64_t a, int64_t b) {{");
        wln!(self, "    int64_t r = _fvec_new();");
        wln!(self, "    int n = (int)p_vec_len((PVec*)a);");
        wln!(self, "    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) - _fvec_get_f64(b,i));");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "int64_t p_simd_mul(int64_t a, int64_t b) {{");
        wln!(self, "    int64_t r = _fvec_new();");
        wln!(self, "    int n = (int)p_vec_len((PVec*)a);");
        wln!(self, "    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) * _fvec_get_f64(b,i));");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "int64_t p_simd_div(int64_t a, int64_t b) {{");
        wln!(self, "    int64_t r = _fvec_new();");
        wln!(self, "    int n = (int)p_vec_len((PVec*)a);");
        wln!(self, "    for (int i = 0; i < n; i++) {{ double db = _fvec_get_f64(b,i); _fvec_push_f64(r, db != 0 ? _fvec_get_f64(a,i) / db : 0); }}");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "double p_simd_dot(int64_t a, int64_t b) {{");
        wln!(self, "    int n = (int)p_vec_len((PVec*)a); double s = 0;");
        wln!(self, "    for (int i = 0; i < n; i++) s += _fvec_get_f64(a,i) * _fvec_get_f64(b,i);");
        wln!(self, "    return s;");
        wln!(self, "}}");
        wln!(self, "double p_simd_sum(int64_t a) {{");
        wln!(self, "    int n = (int)p_vec_len((PVec*)a); double s = 0;");
        wln!(self, "    for (int i = 0; i < n; i++) s += _fvec_get_f64(a,i);");
        wln!(self, "    return s;");
        wln!(self, "}}");
        wln!(self, "int64_t p_simd_scale(int64_t a, double s) {{");
        wln!(self, "    int64_t r = _fvec_new();");
        wln!(self, "    int n = (int)p_vec_len((PVec*)a);");
        wln!(self, "    for (int i = 0; i < n; i++) _fvec_push_f64(r, _fvec_get_f64(a,i) * s);");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "int64_t p_simd_from_vec(int64_t v) {{");
        wln!(self, "    int64_t r = _fvec_new();");
        wln!(self, "    int n = (int)p_vec_len((PVec*)v);");
        wln!(self, "    for (int i = 0; i < n; i++) _fvec_push_f64(r, (double)(int64_t)p_vec_get((PVec*)v, i));");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "char* p_simd_to_str(int64_t a) {{");
        wln!(self, "    if (!a) return strdup(\"[]\");");
        wln!(self, "    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;");
        wln!(self, "    result[pos++] = '[';");
        wln!(self, "    int64_t n = p_vec_len((PVec*)a);");
        wln!(self, "    for (int64_t i = 0; i < n; i++) {{");
        wln!(self, "        if (i > 0) {{ if (pos+2>=cap){{cap*=2;result=(char*)realloc(result,cap);}} result[pos++]=','; result[pos++]=' '; }}");
        wln!(self, "        double d = _fvec_get_f64(a, i);");
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
        wln!(self, "// PTensor typedef already emitted in header section");

        // ─── ML3: Autograd tape data structures (must be before tensor builtins) ───
        wln!(self, "typedef enum { AG_ADD, AG_SUB, AG_MUL, AG_MATMUL, AG_SCALE, AG_RELU, AG_SIGMOID, AG_SUM, AG_MEAN, AG_NEG } AGOpKind;");
        wln!(self, "typedef struct {");
        wln!(self, "    AGOpKind op;");
        wln!(self, "    void* result;  void* a;  void* b;  double scalar;");
        wln!(self, "    int64_t a_shape[8]; int64_t a_ndim;");
        wln!(self, "    int64_t b_shape[8]; int64_t b_ndim;");
        wln!(self, "    double* stored_data; int64_t stored_len; int is_checkpoint;");
        wln!(self, "} AGEntry;");
        wln!(self, "static AGEntry* ag_tape = NULL;");
        wln!(self, "static int64_t ag_tape_cap = 0;");
        wln!(self, "static int64_t ag_tape_len = 0;");
        wln!(self, "static int ag_mode = 0;");
        wln!(self, "static int ag_checkpoint_mode = 0;");
        wln!(self, "static void** ag_grad_ptrs = NULL;");
        wln!(self, "static PTensor** ag_grads = NULL;");
        wln!(self, "static int64_t ag_grad_cap = 0;");
        wln!(self, "static int64_t ag_grad_len = 0;");
        wln!(self, "static double ag_dp_budget = 0.0;");
        wln!(self, "static double ag_dp_limit = 0.0;");
        wln!(self, "static int ag_tp_rank = 0;");
        wln!(self, "static int ag_tp_world_size = 1;");
        wln!(self, "static PTensor* ag_get_grad(void* ptr) {{");
        wln!(self, "    for (int64_t i = 0; i < ag_grad_len; i++) if (ag_grad_ptrs[i] == ptr) return ag_grads[i];");
        wln!(self, "    return NULL;");
        wln!(self, "}}");
        wln!(self, "static void ag_accum_grad(void* ptr, PTensor* grad) {{");
        wln!(self, "    PTensor* ex = ag_get_grad(ptr);");
        wln!(self, "    if (ex) {{ for (int64_t i = 0; i < ex->len && i < grad->len; i++) ex->data[i] += grad->data[i]; }}");
        wln!(self, "    else {{");
        wln!(self, "        if (ag_grad_len >= ag_grad_cap) {{");
        wln!(self, "            int64_t new_cap = ag_grad_cap == 0 ? 64 : ag_grad_cap * 2;");
        wln!(self, "            ag_grad_ptrs = (void**)realloc(ag_grad_ptrs, new_cap * sizeof(void*));");
        wln!(self, "            ag_grads = (PTensor**)realloc(ag_grads, new_cap * sizeof(PTensor*));");
        wln!(self, "            ag_grad_cap = new_cap;");
        wln!(self, "        }}");
        wln!(self, "        ag_grad_ptrs[ag_grad_len] = ptr; ag_grads[ag_grad_len] = grad; ag_grad_len++;");
        wln!(self, "    }}");
        wln!(self, "}}");
        wln!(self, "static PTensor* ag_clone_tensor(PTensor* t) {{");
        wln!(self, "    if (!t) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = t->ndim; r->len = t->len;");
        wln!(self, "    for (int64_t i = 0; i < t->ndim && i < 8; i++) r->shape[i] = t->shape[i];");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < r->len; i++) r->data[i] = t->data[i];");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "static PTensor* ag_zeros_like(PTensor* t) {{");
        wln!(self, "    if (!t) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = t->ndim; r->len = t->len;");
        wln!(self, "    for (int64_t i = 0; i < t->ndim && i < 8; i++) r->shape[i] = t->shape[i];");
        wln!(self, "    r->data = (double*)calloc(r->len, sizeof(double));");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "static PTensor* ag_filled_shape(int64_t* shape, int64_t ndim, double val) {{");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = ndim; int64_t len = 1;");
        wln!(self, "    for (int64_t i = 0; i < ndim && i < 8; i++) {{ r->shape[i] = shape[i]; len *= shape[i]; }}");
        wln!(self, "    r->len = len; r->data = (double*)malloc(len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < len; i++) r->data[i] = val;");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "static void ag_record(AGOpKind op, void* result, void* a, void* b, double s) {{");
        wln!(self, "    if (!ag_mode) return;");
        wln!(self, "    if (ag_tape_len >= ag_tape_cap) {{");
        wln!(self, "        int64_t new_cap = ag_tape_cap == 0 ? 64 : ag_tape_cap * 2;");
        wln!(self, "        ag_tape = (AGEntry*)realloc(ag_tape, new_cap * sizeof(AGEntry));");
        wln!(self, "        ag_tape_cap = new_cap;");
        wln!(self, "    }}");
        wln!(self, "    AGEntry* e = &ag_tape[ag_tape_len++];");
        wln!(self, "    e->op = op; e->result = result; e->a = a; e->b = b; e->scalar = s;");
        wln!(self, "    e->stored_data = NULL; e->stored_len = 0; e->is_checkpoint = 0;");
        wln!(self, "    PTensor* ta = (PTensor*)a;");
        wln!(self, "    if (ta) {{ e->a_ndim = ta->ndim; for (int64_t i = 0; i < ta->ndim && i < 8; i++) e->a_shape[i] = ta->shape[i]; }} else e->a_ndim = 0;");
        wln!(self, "    PTensor* tb = (PTensor*)b;");
        wln!(self, "    if (tb) {{ e->b_ndim = tb->ndim; for (int64_t i = 0; i < tb->ndim && i < 8; i++) e->b_shape[i] = tb->shape[i]; }} else e->b_ndim = 0;");
        wln!(self, "}}");

        // Helper: ensure ag_tape has room for one more entry. Returns pointer
        // to the next slot and increments ag_tape_len. Returns NULL if not in
        // ag_mode or allocation fails.
        wln!(self, "static AGEntry* ag_grow(void) {{");
        wln!(self, "    if (!ag_mode) return NULL;");
        wln!(self, "    if (ag_tape_len >= ag_tape_cap) {{");
        wln!(self, "        int64_t new_cap = ag_tape_cap == 0 ? 64 : ag_tape_cap * 2;");
        wln!(self, "        AGEntry* nt = (AGEntry*)realloc(ag_tape, new_cap * sizeof(AGEntry));");
        wln!(self, "        if (!nt) return NULL;");
        wln!(self, "        ag_tape = nt; ag_tape_cap = new_cap;");
        wln!(self, "    }}");
        wln!(self, "    return &ag_tape[ag_tape_len++];");
        wln!(self, "}}");

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
        wln!(self, "    ag_record(AG_ADD, r, a, b, 0);");
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
        wln!(self, "    ag_record(AG_MUL, r, a, b, 0);");
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
        wln!(self, "    ag_record(AG_MATMUL, r, a, b, 0);");
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
        wln!(self, "    {{ AGEntry* e = ag_grow();");
        wln!(self, "        if (e) {{");
        wln!(self, "        e->op = AG_RELU; e->result = r; e->a = t; e->b = NULL; e->scalar = 0;");
        wln!(self, "        e->a_ndim = pt->ndim; for (int64_t i = 0; i < pt->ndim && i < 8; i++) e->a_shape[i] = pt->shape[i];");
        wln!(self, "        e->b_ndim = 0;");
        wln!(self, "        if (ag_checkpoint_mode) {{ e->is_checkpoint = 1; e->stored_len = pt->len; e->stored_data = (double*)malloc(pt->len * sizeof(double)); for (int64_t i = 0; i < pt->len; i++) e->stored_data[i] = pt->data[i]; }}");
        wln!(self, "        else {{ e->is_checkpoint = 0; e->stored_len = r->len; e->stored_data = (double*)malloc(r->len * sizeof(double)); for (int64_t i = 0; i < r->len; i++) e->stored_data[i] = r->data[i]; }}");
        wln!(self, "        }}");
        wln!(self, "    }}");
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
        wln!(self, "    {{ AGEntry* e = ag_grow();");
        wln!(self, "        if (e) {{");
        wln!(self, "        e->op = AG_SIGMOID; e->result = r; e->a = t; e->b = NULL; e->scalar = 0;");
        wln!(self, "        e->a_ndim = pt->ndim; for (int64_t i = 0; i < pt->ndim && i < 8; i++) e->a_shape[i] = pt->shape[i];");
        wln!(self, "        e->b_ndim = 0;");
        wln!(self, "        if (ag_checkpoint_mode) {{ e->is_checkpoint = 1; e->stored_len = pt->len; e->stored_data = (double*)malloc(pt->len * sizeof(double)); for (int64_t i = 0; i < pt->len; i++) e->stored_data[i] = pt->data[i]; }}");
        wln!(self, "        else {{ e->is_checkpoint = 0; e->stored_len = r->len; e->stored_data = (double*)malloc(r->len * sizeof(double)); for (int64_t i = 0; i < r->len; i++) e->stored_data[i] = r->data[i]; }}");
        wln!(self, "        }}");
        wln!(self, "    }}");
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
        wln!(self, "    ag_record(AG_SCALE, r, t, NULL, s);");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "double p_tensor_sum(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt) return 0; double s=0; for (int64_t i=0;i<pt->len;i++) s+=pt->data[i]; ag_record(AG_SUM, (void*)pt, t, NULL, 0); return s; }}");
        wln!(self, "double p_tensor_mean(void* t) {{ PTensor* pt = (PTensor*)t; if (!pt || pt->len==0) return 0; double r=p_tensor_sum(t)/pt->len; if (ag_mode && ag_tape_len > 0) {{ AGEntry* e=&ag_tape[ag_tape_len-1]; e->op=AG_MEAN; e->scalar=(double)pt->len; }} return r; }}");
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
        wln!(self, "    // Same text as the interpreter (src/tensor.rs to_json): integral");
        wln!(self, "    // values print as integers, others in shortest fixed-point form.");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    if (!pt) return strdup(\"{{}}\");");
        wln!(self, "    size_t cap = 64 + (size_t)pt->ndim * 24 + (size_t)pt->len * 8; char* result = (char*)malloc(cap); size_t pos = 0;");
        wln!(self, "    pos += snprintf(result+pos, cap-pos, \"{{\\\"shape\\\":[\");");
        wln!(self, "    for (int64_t i = 0; i < pt->ndim; i++) {{");
        wln!(self, "        if (i > 0) result[pos++] = ',';");
        wln!(self, "        pos += snprintf(result+pos, cap-pos, \"%lld\", (long long)pt->shape[i]);");
        wln!(self, "    }}");
        wln!(self, "    pos += snprintf(result+pos, cap-pos, \"],\\\"data\\\":[\");");
        wln!(self, "    for (int64_t i = 0; i < pt->len; i++) {{");
        wln!(self, "        double d = pt->data[i];");
        wln!(self, "        char nb[40]; const char* s;");
        wln!(self, "        if (d == trunc(d) && isfinite(d) && fabs(d) < 1e15) {{ snprintf(nb, sizeof nb, \"%lld\", (long long)d); s = nb; }}");
        wln!(self, "        else s = p_to_str_float(d);");
        wln!(self, "        size_t l = strlen(s);");
        wln!(self, "        if (pos + l + 4 > cap) {{ while (pos + l + 4 > cap) cap *= 2; result = (char*)realloc(result, cap); }}");
        wln!(self, "        if (i > 0) result[pos++] = ',';");
        wln!(self, "        memcpy(result+pos, s, l); pos += l;");
        wln!(self, "    }}");
        wln!(self, "    result[pos++] = ']'; result[pos++] = '}}'; result[pos] = 0;");
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
        wln!(self, "            char buf[32]; double _v=pt->data[i]; if (_v==floor(_v)&&isfinite(_v)&&fabs(_v)<1e15) snprintf(buf,sizeof(buf),\"%lld\",(long long)_v); else snprintf(buf,sizeof(buf),\"%.4f\",_v); size_t bl=strlen(buf);");
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
        wln!(self, "            char buf[32]; double _v=pt->data[r*cols+c]; if (_v==floor(_v)&&isfinite(_v)&&fabs(_v)<1e15) snprintf(buf,sizeof(buf),\"%lld\",(long long)_v); else snprintf(buf,sizeof(buf),\"%.4f\",_v); size_t bl=strlen(buf);");
        wln!(self, "            if (pos+bl+1>=cap){{cap=(cap+bl)*2;result=(char*)realloc(result,cap);}}");
        wln!(self, "            memcpy(result+pos,buf,bl); pos+=bl;");
        wln!(self, "        }}");
        wln!(self, "        if (pos+1>=cap){{cap+=2;result=(char*)realloc(result,cap);}} result[pos++]=']';");
        wln!(self, "    }}");
        wln!(self, "    if (pos+2>=cap){{cap+=2;result=(char*)realloc(result,cap);}}");
        wln!(self, "    result[pos++]=']'; result[pos]=0;");
        wln!(self, "    return result;");
        wln!(self, "}}");

        // ─── ML3: Autograd tape runtime (functions only — data structures defined above) ───
        wln!(self, "");

        // Begin/end autograd mode
        wln!(self, "void p_autograd_begin(void) {{");
        wln!(self, "    ag_mode = 1; ag_tape_len = 0; ag_grad_len = 0;");
        wln!(self, "}}");
        wln!(self, "void p_autograd_end(void) {{");
        wln!(self, "    ag_mode = 0;");
        wln!(self, "}}");

        // Backward pass: walk tape in reverse
        wln!(self, "void p_autograd_backward(void* loss) {{");
        wln!(self, "    (void)loss; // seed is always 1.0 (d(loss)/d(loss) = 1)");
        wln!(self, "    double seed = 1.0;");
        wln!(self, "    // Find the last scalar-producing op (sum/mean) to seed");
        wln!(self, "    int64_t seeded_idx = -1;");
        wln!(self, "    for (int64_t i = ag_tape_len - 1; i >= 0; i--) {{");
        wln!(self, "        if (ag_tape[i].op == AG_SUM) {{");
        wln!(self, "            PTensor* g = ag_filled_shape(ag_tape[i].a_shape, ag_tape[i].a_ndim, seed);");
        wln!(self, "            ag_accum_grad(ag_tape[i].a, g); seeded_idx = i; break;");
        wln!(self, "        }}");
        wln!(self, "        if (ag_tape[i].op == AG_MEAN) {{");
        wln!(self, "            PTensor* g = ag_filled_shape(ag_tape[i].a_shape, ag_tape[i].a_ndim, seed / ag_tape[i].scalar);");
        wln!(self, "            ag_accum_grad(ag_tape[i].a, g); seeded_idx = i; break;");
        wln!(self, "        }}");
        wln!(self, "    }}");
        wln!(self, "    if (seeded_idx < 0 && ag_tape_len > 0) {{");
        wln!(self, "        PTensor* g = (PTensor*)malloc(sizeof(PTensor)); g->ndim=1; g->len=1; g->shape[0]=1; g->data=(double*)malloc(sizeof(double)); g->data[0]=seed;");
        wln!(self, "        ag_accum_grad(ag_tape[ag_tape_len-1].result, g);");
        wln!(self, "    }}");
        wln!(self, "    // Walk tape in reverse");
        wln!(self, "    for (int64_t i = ag_tape_len - 1; i >= 0; i--) {{");
        wln!(self, "        if (i == seeded_idx) continue; // skip already-seeded op");
        wln!(self, "        AGEntry* e = &ag_tape[i];");
        wln!(self, "        PTensor* grad_out = ag_get_grad(e->result);");
        wln!(self, "        if (!grad_out) continue;");
        wln!(self, "        switch (e->op) {{");
        wln!(self, "            case AG_ADD: {{");
        wln!(self, "                ag_accum_grad(e->a, ag_clone_tensor(grad_out));");
        wln!(self, "                ag_accum_grad(e->b, ag_clone_tensor(grad_out));");
        wln!(self, "                break;");
        wln!(self, "            }}");
        wln!(self, "            case AG_SUB: {{");
        wln!(self, "                ag_accum_grad(e->a, ag_clone_tensor(grad_out));");
        wln!(self, "                PTensor* ng = ag_clone_tensor(grad_out); for (int64_t j=0;j<ng->len;j++) ng->data[j]=-ng->data[j];");
        wln!(self, "                ag_accum_grad(e->b, ng);");
        wln!(self, "                break;");
        wln!(self, "            }}");
        wln!(self, "            case AG_MUL: {{");
        wln!(self, "                ag_accum_grad(e->a, ag_clone_tensor(grad_out));");
        wln!(self, "                ag_accum_grad(e->b, ag_clone_tensor(grad_out));");
        wln!(self, "                break;");
        wln!(self, "            }}");
        wln!(self, "            case AG_MATMUL: {{");
        wln!(self, "                PTensor* ag = ag_filled_shape(e->a_shape, e->a_ndim, 1.0);");
        wln!(self, "                PTensor* bg = ag_filled_shape(e->b_shape, e->b_ndim, 1.0);");
        wln!(self, "                ag_accum_grad(e->a, ag); ag_accum_grad(e->b, bg);");
        wln!(self, "                break;");
        wln!(self, "            }}");
        wln!(self, "            case AG_SCALE: {{");
        wln!(self, "                PTensor* ga = ag_clone_tensor(grad_out); for (int64_t j=0;j<ga->len;j++) ga->data[j]*=e->scalar;");
        wln!(self, "                ag_accum_grad(e->a, ga);");
        wln!(self, "                break;");
        wln!(self, "            }}");
        wln!(self, "            case AG_RELU: {{");
        wln!(self, "                // Recompute mask from stored data");
        wln!(self, "                PTensor* ga = ag_clone_tensor(grad_out);");
        wln!(self, "                if (e->stored_data) {{");
        wln!(self, "                    for (int64_t j=0;j<ga->len && j<e->stored_len;j++) {{");
        wln!(self, "                        double val = e->is_checkpoint ? (e->stored_data[j] > 0 ? e->stored_data[j] : 0) : e->stored_data[j];");
        wln!(self, "                        ga->data[j] *= (val > 0) ? 1.0 : 0.0;");
        wln!(self, "                    }}");
        wln!(self, "                }}");
        wln!(self, "                ag_accum_grad(e->a, ga);");
        wln!(self, "                break;");
        wln!(self, "            }}");
        wln!(self, "            case AG_SIGMOID: {{");
        wln!(self, "                PTensor* ga = ag_clone_tensor(grad_out);");
        wln!(self, "                if (e->stored_data) {{");
        wln!(self, "                    for (int64_t j=0;j<ga->len && j<e->stored_len;j++) {{");
        wln!(self, "                        double s = e->is_checkpoint ? (1.0/(1.0+exp(-e->stored_data[j]))) : e->stored_data[j];");
        wln!(self, "                        ga->data[j] *= s * (1.0 - s);");
        wln!(self, "                    }}");
        wln!(self, "                }}");
        wln!(self, "                ag_accum_grad(e->a, ga);");
        wln!(self, "                break;");
        wln!(self, "            }}");
        wln!(self, "            case AG_SUM: {{");
        wln!(self, "                PTensor* g = ag_filled_shape(e->a_shape, e->a_ndim, seed);");
        wln!(self, "                ag_accum_grad(e->a, g);");
        wln!(self, "                break;");
        wln!(self, "            }}");
        wln!(self, "            case AG_MEAN: {{");
        wln!(self, "                PTensor* g = ag_filled_shape(e->a_shape, e->a_ndim, seed / e->scalar);");
        wln!(self, "                ag_accum_grad(e->a, g);");
        wln!(self, "                break;");
        wln!(self, "            }}");
        wln!(self, "            case AG_NEG: {{");
        wln!(self, "                PTensor* ng = ag_clone_tensor(grad_out); for (int64_t j=0;j<ng->len;j++) ng->data[j]=-ng->data[j];");
        wln!(self, "                ag_accum_grad(e->a, ng);");
        wln!(self, "                break;");
        wln!(self, "            }}");
        wln!(self, "        }}");
        wln!(self, "    }}");
        wln!(self, "}}");

        // Get gradient for a tensor
        wln!(self, "void* p_grad_get(void* t) {{");
        wln!(self, "    PTensor* g = ag_get_grad(t);");
        wln!(self, "    if (g) return g;");
        wln!(self, "    // No gradient — return zero tensor of same shape");
        wln!(self, "    PTensor* pt = (PTensor*)t;");
        wln!(self, "    if (!pt) return NULL;");
        wln!(self, "    return (void*)ag_zeros_like(pt);");
        wln!(self, "}}");
        wln!(self, "void p_grad_zero(void) {{");
        wln!(self, "    ag_grad_len = 0;");
        wln!(self, "}}");

        // ML11: Checkpoint mode
        wln!(self, "void p_checkpoint_begin(void) {{ ag_checkpoint_mode = 1; }}");
        wln!(self, "void p_checkpoint_end(void) {{ ag_checkpoint_mode = 0; }}");

        // ML12: DP builtins
        wln!(self, "void p_dp_budget_reset(double epsilon) {{ ag_dp_budget = 0.0; ag_dp_limit = epsilon; }}");
        wln!(self, "void p_dp_budget_check(double epsilon, double limit) {{");
        wln!(self, "    ag_dp_budget += epsilon;");
        wln!(self, "    if (ag_dp_budget > limit) {{ fprintf(stderr, \"DP budget exhausted: consumed %g > limit %g\\n\", ag_dp_budget, limit); exit(1); }}");
        wln!(self, "}}");
        wln!(self, "void* p_dp_noise(void* t, double sigma) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t; if (!pt) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = pt->ndim; r->len = pt->len;");
        wln!(self, "    for (int64_t i = 0; i < pt->ndim && i < 8; i++) r->shape[i] = pt->shape[i];");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < r->len; i++) {{");
        wln!(self, "        double val = pt->data[i];");
        wln!(self, "        double u1 = 0.0001 + fabs(fmod(val * 1000.0, 1.0)) * 0.9998;");
        wln!(self, "        double u2 = 0.0001 + fabs(fmod(sin(val) * 1000.0, 1.0)) * 0.9998;");
        wln!(self, "        double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);");
        wln!(self, "        r->data[i] = val + sigma * z;");
        wln!(self, "    }}");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_gaussian_noise(void* shape_vec, double sigma) {{");
        wln!(self, "    // shape_vec is a Fides Vec (i64 pointer to 24-byte block)");
        wln!(self, "    int64_t* sv = (int64_t*)shape_vec;");
        wln!(self, "    int64_t ndim = sv ? sv[0] : 0;");
        wln!(self, "    int64_t* data_ptr = sv ? (int64_t*)sv[1] : NULL;");
        wln!(self, "    int64_t shape[8]; int64_t len = 1;");
        wln!(self, "    for (int64_t i = 0; i < ndim && i < 8; i++) {{ shape[i] = data_ptr ? data_ptr[i] : 1; len *= shape[i]; }}");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = ndim; r->len = len;");
        wln!(self, "    for (int64_t i = 0; i < ndim && i < 8; i++) r->shape[i] = shape[i];");
        wln!(self, "    r->data = (double*)malloc(len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < len; i++) {{");
        wln!(self, "        double seed = (double)i * 0.1;");
        wln!(self, "        double u1 = 0.0001 + fabs(sin(seed)) * 0.9998;");
        wln!(self, "        double u2 = 0.0001 + fabs(cos(seed)) * 0.9998;");
        wln!(self, "        double z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);");
        wln!(self, "        r->data[i] = sigma * z;");
        wln!(self, "    }}");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_dropout(void* t, double drop_prob) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t; if (!pt) return NULL;");
        wln!(self, "    PTensor* r = ag_clone_tensor(pt);");
        wln!(self, "    double scale = 1.0 / (1.0 - drop_prob);");
        wln!(self, "    for (int64_t i = 0; i < r->len; i++) {{");
        wln!(self, "        double s = fabs(sin((double)i * 7.0));");
        wln!(self, "        if (s < drop_prob) r->data[i] = 0.0; else r->data[i] *= scale;");
        wln!(self, "    }}");
        wln!(self, "    return r;");
        wln!(self, "}}");

        // ML13: Tensor parallelism builtins (single-rank: identity/no-ops)
        wln!(self, "int64_t p_tp_rank(void) {{ return ag_tp_rank; }}");
        wln!(self, "int64_t p_tp_world_size(void) {{ return ag_tp_world_size; }}");
        wln!(self, "void* p_all_reduce(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t; if (!pt) return NULL;");
        wln!(self, "    return (void*)ag_clone_tensor(pt);");
        wln!(self, "}}");
        wln!(self, "void* p_broadcast(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t; if (!pt) return NULL;");
        wln!(self, "    return (void*)ag_clone_tensor(pt);");
        wln!(self, "}}");
        wln!(self, "void* p_all_gather(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t; if (!pt) return NULL;");
        wln!(self, "    return (void*)ag_clone_tensor(pt);");
        wln!(self, "}}");
        wln!(self, "void* p_scatter(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t; if (!pt) return NULL;");
        wln!(self, "    return (void*)ag_clone_tensor(pt);");
        wln!(self, "}}");
        wln!(self, "void p_send_tensor(void* t) {{ (void)t; /* no-op in single-rank */ }}");
        wln!(self, "void* p_recv_tensor(void* shape_vec) {{");
        wln!(self, "    int64_t* sv = (int64_t*)shape_vec;");
        wln!(self, "    int64_t ndim = sv ? sv[0] : 0;");
        wln!(self, "    int64_t* data_ptr = sv ? (int64_t*)sv[1] : NULL;");
        wln!(self, "    int64_t shape[8]; int64_t len = 1;");
        wln!(self, "    for (int64_t i = 0; i < ndim && i < 8; i++) {{ shape[i] = data_ptr ? data_ptr[i] : 1; len *= shape[i]; }}");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = ndim; r->len = len;");
        wln!(self, "    for (int64_t i = 0; i < ndim && i < 8; i++) r->shape[i] = shape[i];");
        wln!(self, "    r->data = (double*)calloc(len, sizeof(double));");
        wln!(self, "    return r;");
        wln!(self, "}}");

        // ─── ML9: KV Cache runtime ───
        wln!(self, "");
        wln!(self, "typedef struct { int64_t seq_len; PTensor* k_data; PTensor* v_data; } KVLayer;");
        wln!(self, "typedef struct { int64_t num_layers; int64_t max_seq_len; int64_t window_size; int64_t head_dim; KVLayer* layers; } KVCache;");
        wln!(self, "static KVCache* kv_caches[16];");
        wln!(self, "static int64_t kv_cache_count = 0;");
        wln!(self, "int64_t p_kv_cache_new(int64_t num_layers, int64_t max_seq_len, int64_t window_size, int64_t head_dim) {{");
        wln!(self, "    (void)max_seq_len;");
        wln!(self, "    if (kv_cache_count >= 16) return -1;");
        wln!(self, "    KVCache* c = (KVCache*)malloc(sizeof(KVCache));");
        wln!(self, "    c->num_layers = num_layers; c->max_seq_len = max_seq_len;");
        wln!(self, "    c->window_size = window_size; c->head_dim = head_dim;");
        wln!(self, "    c->layers = (KVLayer*)calloc(num_layers, sizeof(KVLayer));");
        wln!(self, "    for (int64_t i = 0; i < num_layers; i++) {{ c->layers[i].seq_len = 0; c->layers[i].k_data = NULL; c->layers[i].v_data = NULL; }}");
        wln!(self, "    int64_t id = kv_cache_count; kv_caches[kv_cache_count++] = c;");
        wln!(self, "    return id;");
        wln!(self, "}}");
        wln!(self, "void p_kv_cache_append(int64_t cache_id, int64_t layer, void* k, void* v) {");
        wln!(self, "    if (cache_id < 0 || cache_id >= kv_cache_count) return;");
        wln!(self, "    KVCache* c = kv_caches[cache_id];");
        wln!(self, "    if (layer < 0 || layer >= c->num_layers) return;");
        wln!(self, "    PTensor* kt = (PTensor*)k; PTensor* vt = (PTensor*)v;");
        wln!(self, "    if (!kt || !vt) return;");
        wln!(self, "    KVLayer* L = &c->layers[layer];");
        wln!(self, "    if (!L->k_data) {");
        wln!(self, "        L->k_data = (PTensor*)calloc(1, sizeof(PTensor)); L->k_data->ndim = 2; L->k_data->shape[1] = c->head_dim;");
        wln!(self, "        L->v_data = (PTensor*)calloc(1, sizeof(PTensor)); L->v_data->ndim = 2; L->v_data->shape[1] = c->head_dim;");
        wln!(self, "    }");
        wln!(self, "    // Append every new value, then drop the oldest beyond the limit.");
        wln!(self, "    PTensor* dst[2] = { L->k_data, L->v_data };");
        wln!(self, "    PTensor* src[2] = { kt, vt };");
        wln!(self, "    int64_t max_tokens = c->window_size > 0 && c->window_size < c->max_seq_len ? c->window_size : c->max_seq_len;");
        wln!(self, "    int64_t max_vals = max_tokens * c->head_dim;");
        wln!(self, "    if (max_vals < 0) max_vals = 0;");
        wln!(self, "    for (int t = 0; t < 2; t++) {");
        wln!(self, "        int64_t total = dst[t]->len + src[t]->len;");
        wln!(self, "        dst[t]->data = (double*)realloc(dst[t]->data, (total > 0 ? total : 1) * sizeof(double));");
        wln!(self, "        for (int64_t i = 0; i < src[t]->len; i++) dst[t]->data[dst[t]->len + i] = src[t]->data[i];");
        wln!(self, "        dst[t]->len = total;");
        wln!(self, "        if (dst[t]->len > max_vals) {");
        wln!(self, "            int64_t excess = dst[t]->len - max_vals;");
        wln!(self, "            memmove(dst[t]->data, dst[t]->data + excess, max_vals * sizeof(double));");
        wln!(self, "            dst[t]->len = max_vals;");
        wln!(self, "        }");
        wln!(self, "    }");
        wln!(self, "    int64_t hd = c->head_dim > 0 ? c->head_dim : 1;");
        wln!(self, "    L->seq_len = L->k_data->len / hd;");
        wln!(self, "    L->k_data->shape[0] = L->seq_len; L->v_data->shape[0] = L->seq_len;");
        wln!(self, "}");
        wln!(self, "void* p_kv_cache_get(int64_t cache_id, int64_t layer) {{");
        wln!(self, "    if (cache_id < 0 || cache_id >= kv_cache_count) return NULL;");
        wln!(self, "    KVCache* c = kv_caches[cache_id];");
        wln!(self, "    if (layer < 0 || layer >= c->num_layers) return NULL;");
        wln!(self, "    if (!c->layers[layer].k_data) return NULL;");
        wln!(self, "    int64_t sl = c->layers[layer].seq_len; int64_t hd = c->head_dim;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = 2; r->shape[0] = sl; r->shape[1] = hd * 2; r->len = sl * hd * 2;");
        wln!(self, "    r->data = (double*)malloc(r->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < sl * hd; i++) {{ r->data[i] = c->layers[layer].k_data->data[i]; r->data[sl * hd + i] = c->layers[layer].v_data->data[i]; }}");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "int64_t p_kv_cache_len(int64_t cache_id, int64_t layer) {{");
        wln!(self, "    if (cache_id < 0 || cache_id >= kv_cache_count) return 0;");
        wln!(self, "    KVCache* c = kv_caches[cache_id];");
        wln!(self, "    if (layer < 0 || layer >= c->num_layers) return 0;");
        wln!(self, "    return c->layers[layer].seq_len;");
        wln!(self, "}}");
        wln!(self, "void p_kv_cache_clear(int64_t cache_id) {{");
        wln!(self, "    if (cache_id < 0 || cache_id >= kv_cache_count) return;");
        wln!(self, "    KVCache* c = kv_caches[cache_id];");
        wln!(self, "    for (int64_t i = 0; i < c->num_layers; i++) c->layers[i].seq_len = 0;");
        wln!(self, "}}");
        wln!(self, "void p_kv_cache_evict(int64_t cache_id, int64_t num_tokens) {{");
        wln!(self, "    if (cache_id < 0 || cache_id >= kv_cache_count) return;");
        wln!(self, "    KVCache* c = kv_caches[cache_id];");
        wln!(self, "    for (int64_t i = 0; i < c->num_layers; i++) {{");
        wln!(self, "        if (c->layers[i].seq_len > num_tokens) c->layers[i].seq_len -= num_tokens;");
        wln!(self, "        else c->layers[i].seq_len = 0;");
        wln!(self, "    }}");
        wln!(self, "}}");
        // ─── ML10: Quantization runtime ───
        wln!(self, "");
        wln!(self, "void* p_quantize_q8(void* t, double scale, int64_t zero_point) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t; if (!pt) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = pt->ndim; r->len = pt->len;");
        wln!(self, "    for (int64_t i = 0; i < pt->ndim && i < 8; i++) r->shape[i] = pt->shape[i];");
        wln!(self, "    r->data = (double*)malloc(pt->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < pt->len; i++) {{ double q = round(pt->data[i] / scale) + zero_point; if (q < -128) q = -128; if (q > 127) q = 127; r->data[i] = q; }}");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_dequantize_q8(void* t, double scale, int64_t zero_point) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t; if (!pt) return NULL;");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = pt->ndim; r->len = pt->len;");
        wln!(self, "    for (int64_t i = 0; i < pt->ndim && i < 8; i++) r->shape[i] = pt->shape[i];");
        wln!(self, "    r->data = (double*)malloc(pt->len * sizeof(double));");
        wln!(self, "    for (int64_t i = 0; i < pt->len; i++) r->data[i] = (pt->data[i] - zero_point) * scale;");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "void* p_qmatmul(void* a, void* b) {{");
        wln!(self, "    PTensor* ta = (PTensor*)a; PTensor* tb = (PTensor*)b;");
        wln!(self, "    if (!ta || !tb) return NULL;");
        wln!(self, "    int64_t m = ta->shape[0], k = ta->shape[1], n = tb->shape[1];");
        wln!(self, "    PTensor* r = (PTensor*)malloc(sizeof(PTensor));");
        wln!(self, "    r->ndim = 2; r->shape[0] = m; r->shape[1] = n; r->len = m * n;");
        wln!(self, "    r->data = (double*)calloc(m * n, sizeof(double));");
        wln!(self, "    // Issue 12: Use INT32 integer accumulation for quantized values.");
        wln!(self, "    // Quantized tensors store int8 values as doubles (clamped to [-128, 127]).");
        wln!(self, "    // The accumulation is done in int32 to avoid overflow, then rescaled.");
        wln!(self, "    // This gives the same result as float matmul but with INT8 inputs.");
        wln!(self, "    for (int64_t i = 0; i < m; i++) for (int64_t j = 0; j < n; j++) {{");
        wln!(self, "        int32_t acc = 0;");
        wln!(self, "        for (int64_t x = 0; x < k; x++) {{");
        wln!(self, "            int32_t qa = (int32_t)(ta->data[i*k+x]);  // already quantized int8");
        wln!(self, "            int32_t qb = (int32_t)(tb->data[x*n+j]);  // already quantized int8");
        wln!(self, "            acc += qa * qb;  // INT8 x INT8 -> INT32 accumulate");
        wln!(self, "        }}");
        wln!(self, "        r->data[i*n+j] = (double)acc;  // store as int32 result (caller dequantizes)");
        wln!(self, "    }}");
        wln!(self, "    return r;");
        wln!(self, "}}");
        wln!(self, "char* p_find_quant_params(void* t) {{");
        wln!(self, "    PTensor* pt = (PTensor*)t; if (!pt || pt->len == 0) return strdup(\"1,0\");");
        wln!(self, "    double mn = pt->data[0], mx = pt->data[0];");
        wln!(self, "    for (int64_t i = 1; i < pt->len; i++) {{ if (pt->data[i] < mn) mn = pt->data[i]; if (pt->data[i] > mx) mx = pt->data[i]; }}");
        wln!(self, "    double abs_max = fabs(mn) > fabs(mx) ? fabs(mn) : fabs(mx);");
        wln!(self, "    double scale = abs_max > 0.0 ? abs_max / 127.0 : 1.0;");
        wln!(self, "    int64_t zp = 0;");
        wln!(self, "    char* r = (char*)malloc(64); snprintf(r, 64, \"%.17g,%lld\", scale, (long long)zp); return r;");
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
        wln!(self, "    size_t klen = strlen(key);");
        wln!(self, "    const char* p = s;");
        wln!(self, "    while (*p) {{");
        wln!(self, "        if (*p == '\"') {{");
        wln!(self, "            const char* ks = p + 1;");
        wln!(self, "            const char* ke = ks;");
        wln!(self, "            while (*ke && *ke != '\"') {{ if (*ke == '\\\\' && ke[1]) ke += 2; else ke++; }}");
        wln!(self, "            if (*ke == '\"' && (size_t)(ke - ks) == klen && strncmp(ks, key, klen) == 0) {{");
        wln!(self, "                const char* c = ke + 1;");
        wln!(self, "                while (*c == ' ' || *c == '\\t') c++;");
        wln!(self, "                if (*c == ':') {{");
        wln!(self, "                    c++;");
        wln!(self, "                    while (*c == ' ' || *c == '\\t') c++;");
        wln!(self, "                    if (*c == '\"') {{");
        wln!(self, "                        const char* end = c + 1;");
        wln!(self, "                        while (*end) {{ if (*end == '\\\\' && end[1]) end += 2; else if (*end == '\"') break; else end++; }}");
        wln!(self, "                        size_t len = end - c + 1;");
        wln!(self, "                        char* result = (char*)malloc(len+1); memcpy(result, c, len); result[len] = 0;");
        wln!(self, "                        return result;");
        wln!(self, "                    }}");
        wln!(self, "                    if (*c == '{{' || *c == '[') {{");
        wln!(self, "                        char open = *c; char close = open == '{{' ? '}}' : ']';");
        wln!(self, "                        int depth = 1; const char* end = c + 1;");
        wln!(self, "                        while (*end && depth > 0) {{");
        wln!(self, "                            if (*end == '\"') {{ const char* se = end + 1; while (*se) {{ if (*se == '\\\\' && se[1]) se += 2; else if (*se == '\"') {{ end = se; break; }} else se++; }} if (*se != '\"') {{ end = se; break; }} end++; continue; }}");
        wln!(self, "                            if (*end == open) depth++;");
        wln!(self, "                            if (*end == close) depth--;");
        wln!(self, "                            if (depth > 0) end++;");
        wln!(self, "                        }}");
        wln!(self, "                        size_t len = end - c + 1;");
        wln!(self, "                        char* result = (char*)malloc(len+1); memcpy(result, c, len); result[len] = 0;");
        wln!(self, "                        char* canonical = p_json_parse(result); free(result);");
        wln!(self, "                        return canonical;");
        wln!(self, "                    }}");
        wln!(self, "                    const char* end = c;");
        wln!(self, "                    while (*end && *end != ',' && *end != '}}' && *end != ']' && *end != ' ' && *end != '\\n') end++;");
        wln!(self, "                    size_t len = end - c;");
        wln!(self, "                    char* result = (char*)malloc(len+1); memcpy(result, c, len); result[len] = 0;");
        wln!(self, "                    return result;");
        wln!(self, "                }}");
        wln!(self, "            }}");
        wln!(self, "            p = ke + 1;");
        wln!(self, "        }} else {{ p++; }}");
        wln!(self, "    }}");
        wln!(self, "    return strdup(\"null\");");
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
        wln!(self, "    const char* reason = \"OK\";");
        wln!(self, "    if (status == 200) reason = \"OK\";");
        wln!(self, "    else if (status == 201) reason = \"Created\";");
        wln!(self, "    else if (status == 204) reason = \"No Content\";");
        wln!(self, "    else if (status == 301) reason = \"Moved Permanently\";");
        wln!(self, "    else if (status == 302) reason = \"Found\";");
        wln!(self, "    else if (status == 304) reason = \"Not Modified\";");
        wln!(self, "    else if (status == 400) reason = \"Bad Request\";");
        wln!(self, "    else if (status == 401) reason = \"Unauthorized\";");
        wln!(self, "    else if (status == 403) reason = \"Forbidden\";");
        wln!(self, "    else if (status == 404) reason = \"Not Found\";");
        wln!(self, "    else if (status == 405) reason = \"Method Not Allowed\";");
        wln!(self, "    else if (status == 409) reason = \"Conflict\";");
        wln!(self, "    else if (status == 413) reason = \"Payload Too Large\";");
        wln!(self, "    else if (status == 429) reason = \"Too Many Requests\";");
        wln!(self, "    else if (status == 500) reason = \"Internal Server Error\";");
        wln!(self, "    else if (status == 501) reason = \"Not Implemented\";");
        wln!(self, "    else if (status == 502) reason = \"Bad Gateway\";");
        wln!(self, "    else if (status == 503) reason = \"Service Unavailable\";");
        wln!(self, "    else if (status == 504) reason = \"Gateway Timeout\";");
        wln!(self, "    snprintf(header, sizeof(header), \"HTTP/1.1 %lld %s\\r\\nContent-Type: text/plain\\r\\nContent-Length: %zu\\r\\n\\r\\n\", (long long)status, reason, strlen(body));");
        wln!(self, "    p_socket_write(fd, header, strlen(header));");
        wln!(self, "    p_socket_write(fd, body, strlen(body));");
        wln!(self, "}}");
        wln!(self, "char* p_http_build_response(int64_t status, const char* content_type, const char* body) {{");
        wln!(self, "    (void)content_type;");
        wln!(self, "    size_t blen = strlen(body);");
        wln!(self, "    char* resp = (char*)malloc(blen + 256);");
        wln!(self, "    const char* reason = \"OK\";");
        wln!(self, "    if (status == 200) reason = \"OK\";");
        wln!(self, "    else if (status == 201) reason = \"Created\";");
        wln!(self, "    else if (status == 204) reason = \"No Content\";");
        wln!(self, "    else if (status == 301) reason = \"Moved Permanently\";");
        wln!(self, "    else if (status == 302) reason = \"Found\";");
        wln!(self, "    else if (status == 304) reason = \"Not Modified\";");
        wln!(self, "    else if (status == 400) reason = \"Bad Request\";");
        wln!(self, "    else if (status == 401) reason = \"Unauthorized\";");
        wln!(self, "    else if (status == 403) reason = \"Forbidden\";");
        wln!(self, "    else if (status == 404) reason = \"Not Found\";");
        wln!(self, "    else if (status == 405) reason = \"Method Not Allowed\";");
        wln!(self, "    else if (status == 409) reason = \"Conflict\";");
        wln!(self, "    else if (status == 413) reason = \"Payload Too Large\";");
        wln!(self, "    else if (status == 429) reason = \"Too Many Requests\";");
        wln!(self, "    else if (status == 500) reason = \"Internal Server Error\";");
        wln!(self, "    else if (status == 501) reason = \"Not Implemented\";");
        wln!(self, "    else if (status == 502) reason = \"Bad Gateway\";");
        wln!(self, "    else if (status == 503) reason = \"Service Unavailable\";");
        wln!(self, "    else if (status == 504) reason = \"Gateway Timeout\";");
        wln!(self, "    snprintf(resp, blen+256, \"HTTP/1.1 %lld %s\\r\\nContent-Length: %zu\\r\\n\\r\\n%s\", (long long)status, reason, blen, body);");
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
        wln!(self, "static int64_t p_db_mem_find(PMemDB* mdb, const char* k, size_t kl) {");
        wln!(self, "    for (int64_t i = 0; i < mdb->len; i++) {");
        wln!(self, "        if (strlen(mdb->keys[i]) == kl && memcmp(mdb->keys[i], k, kl) == 0) return i;");
        wln!(self, "    }");
        wln!(self, "    return -1;");
        wln!(self, "}");
        wln!(self, "int64_t p_db_mem_exec(void* db, const char* query) {");
        wln!(self, "    PMemDB* mdb = (PMemDB*)db;");
        wln!(self, "    if (!mdb || !query) return 0;");
        wln!(self, "    const char* p = query;");
        wln!(self, "    while (*p == ' ') p++;");
        wln!(self, "    char cmd[16] = {0}; int ci = 0;");
        wln!(self, "    while (*p && *p != ' ' && ci < 15) cmd[ci++] = tolower(*p++);");
        wln!(self, "    while (*p == ' ') p++;");
        wln!(self, "    const char* kstart = p;");
        wln!(self, "    while (*p && *p != ' ') p++;");
        wln!(self, "    size_t klen = p - kstart;");
        wln!(self, "    while (*p == ' ') p++;");
        wln!(self, "    const char* vstart = p;");
        wln!(self, "    while (*p) p++;");
        wln!(self, "    size_t vlen = p - vstart;");
        wln!(self, "    if ((strcmp(cmd, \"insert\") == 0 || strcmp(cmd, \"set\") == 0) && klen > 0) {");
        wln!(self, "        char* v = (char*)malloc(vlen+1); memcpy(v, vstart, vlen); v[vlen] = 0;");
        wln!(self, "        int64_t at = p_db_mem_find(mdb, kstart, klen);");
        wln!(self, "        if (at >= 0) { free(mdb->vals[at]); mdb->vals[at] = v; return 1; }");
        wln!(self, "        if (mdb->len >= mdb->cap) { mdb->cap = mdb->cap ? mdb->cap*2 : 8; mdb->keys = (char**)realloc(mdb->keys, mdb->cap*sizeof(char*)); mdb->vals = (char**)realloc(mdb->vals, mdb->cap*sizeof(char*)); }");
        wln!(self, "        mdb->keys[mdb->len] = (char*)malloc(klen+1); memcpy(mdb->keys[mdb->len], kstart, klen); mdb->keys[mdb->len][klen] = 0;");
        wln!(self, "        mdb->vals[mdb->len] = v;");
        wln!(self, "        mdb->len++;");
        wln!(self, "        return 1;");
        wln!(self, "    }");
        wln!(self, "    return 0;");
        wln!(self, "}");
        wln!(self, "char* p_db_mem_query(void* db, const char* query) {");
        wln!(self, "    PMemDB* mdb = (PMemDB*)db;");
        wln!(self, "    if (!mdb) return strdup(\"[]\");");
        wln!(self, "    const char* w = query ? strstr(query, \"key=\") : NULL;");
        wln!(self, "    if (w) {");
        wln!(self, "        const char* k = w + 4;");
        wln!(self, "        while (*k == '\\'' || *k == '\"') k++;");
        wln!(self, "        size_t kl = 0;");
        wln!(self, "        while (k[kl] && k[kl] != ' ' && k[kl] != ';' && k[kl] != '\\'' && k[kl] != '\"') kl++;");
        wln!(self, "        int64_t at = p_db_mem_find(mdb, k, kl);");
        wln!(self, "        return strdup(at >= 0 ? mdb->vals[at] : \"\");");
        wln!(self, "    }");
        wln!(self, "    size_t cap = 64; char* result = (char*)malloc(cap); size_t pos = 0;");
        wln!(self, "    result[pos++] = '[';");
        wln!(self, "    for (int64_t i = 0; i < mdb->len; i++) {");
        wln!(self, "        if (i > 0) result[pos++] = ',';");
        wln!(self, "        size_t kl = strlen(mdb->keys[i]);");
        wln!(self, "        if (pos+kl+4 >= cap) { cap = (cap+kl)*2; result = (char*)realloc(result, cap); }");
        wln!(self, "        result[pos++] = '\"'; memcpy(result+pos, mdb->keys[i], kl); pos += kl; result[pos++] = '\"';");
        wln!(self, "    }");
        wln!(self, "    result[pos++] = ']'; result[pos] = 0;");
        wln!(self, "    return result;");
        wln!(self, "}");
        wln!(self, "void p_db_mem_close(void* db) {{");
        wln!(self, "    PMemDB* mdb = (PMemDB*)db;");
        wln!(self, "    if (mdb) {{ for (int64_t i = 0; i < mdb->len; i++) {{ free(mdb->keys[i]); free(mdb->vals[i]); }} free(mdb->keys); free(mdb->vals); free(mdb); }}");
        wln!(self, "}}");

        // ─── ECS runtime ───
        // Simple component storage: array of component tables, each table is
        // an array of (entity_id, value_ptr) pairs. Component names are matched
        // by string comparison. This is O(n) per lookup but sufficient for tests.
        wln!(self, "");
        wln!(self, "typedef struct { int64_t entity; void* value; } EcsEntry;");
        wln!(self, "typedef struct { char name[64]; EcsEntry* entries; int64_t len; int64_t cap; } EcsComponent;");
        wln!(self, "static EcsComponent* p_ecs_components = NULL;");
        wln!(self, "static int64_t p_ecs_component_count = 0;");
        wln!(self, "static int64_t p_ecs_component_cap = 0;");
        wln!(self, "static int64_t p_ecs_entity_count = 0;");
        wln!(self, "");
        wln!(self, "static EcsComponent* p_ecs_find_component(const char* name) {");
        wln!(self, "    for (int64_t i = 0; i < p_ecs_component_count; i++) {");
        wln!(self, "        if (strncmp(p_ecs_components[i].name, name, 63) == 0) return &p_ecs_components[i];");
        wln!(self, "    }");
        wln!(self, "    return NULL;");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "static EcsComponent* p_ecs_get_or_create_component(const char* name) {");
        wln!(self, "    EcsComponent* c = p_ecs_find_component(name);");
        wln!(self, "    if (c) return c;");
        wln!(self, "    if (p_ecs_component_count >= p_ecs_component_cap) {");
        wln!(self, "        p_ecs_component_cap = p_ecs_component_cap == 0 ? 8 : p_ecs_component_cap * 2;");
        wln!(self, "        p_ecs_components = (EcsComponent*)realloc(p_ecs_components, p_ecs_component_cap * sizeof(EcsComponent));");
        wln!(self, "    }");
        wln!(self, "    c = &p_ecs_components[p_ecs_component_count++];");
        wln!(self, "    strncpy(c->name, name, 63); c->name[63] = 0;");
        wln!(self, "    c->entries = NULL; c->len = 0; c->cap = 0;");
        wln!(self, "    return c;");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "int64_t __ecs_create_entity(void) {");
        wln!(self, "    return p_ecs_entity_count++;");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "int64_t __ecs_entity_count(void) {");
        wln!(self, "    return p_ecs_entity_count;");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "int64_t __ecs_has(const char* comp_name, int64_t entity) {");
        wln!(self, "    EcsComponent* c = p_ecs_find_component(comp_name);");
        wln!(self, "    if (!c) return 0;");
        wln!(self, "    for (int64_t i = 0; i < c->len; i++) {");
        wln!(self, "        if (c->entries[i].entity == entity) return 1;");
        wln!(self, "    }");
        wln!(self, "    return 0;");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "void* __ecs_get(const char* comp_name, int64_t entity) {");
        wln!(self, "    EcsComponent* c = p_ecs_find_component(comp_name);");
        wln!(self, "    if (!c) return NULL;");
        wln!(self, "    for (int64_t i = 0; i < c->len; i++) {");
        wln!(self, "        if (c->entries[i].entity == entity) return c->entries[i].value;");
        wln!(self, "    }");
        wln!(self, "    return NULL;");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "int64_t __ecs_add(const char* comp_name, int64_t entity, void* value) {");
        wln!(self, "    EcsComponent* c = p_ecs_get_or_create_component(comp_name);");
        wln!(self, "    // Check if entity already exists — update value");
        wln!(self, "    for (int64_t i = 0; i < c->len; i++) {");
        wln!(self, "        if (c->entries[i].entity == entity) { c->entries[i].value = value; return 0; }");
        wln!(self, "    }");
        wln!(self, "    // Add new entry");
        wln!(self, "    if (c->len >= c->cap) {");
        wln!(self, "        c->cap = c->cap == 0 ? 8 : c->cap * 2;");
        wln!(self, "        c->entries = (EcsEntry*)realloc(c->entries, c->cap * sizeof(EcsEntry));");
        wln!(self, "    }");
        wln!(self, "    c->entries[c->len].entity = entity;");
        wln!(self, "    c->entries[c->len].value = value;");
        wln!(self, "    c->len++;");
        wln!(self, "    return 0;");
        wln!(self, "}");
        wln!(self, "");
        wln!(self, "int64_t __ecs_remove(const char* comp_name, int64_t entity) {");
        wln!(self, "    EcsComponent* c = p_ecs_find_component(comp_name);");
        wln!(self, "    if (!c) return 0;");
        wln!(self, "    for (int64_t i = 0; i < c->len; i++) {");
        wln!(self, "        if (c->entries[i].entity == entity) {");
        wln!(self, "            c->entries[i] = c->entries[c->len - 1];");
        wln!(self, "            c->len--;");
        wln!(self, "            return 0;");
        wln!(self, "        }");
        wln!(self, "    }");
        wln!(self, "    return 0;");
        wln!(self, "}");
    }
}

pub fn generate_c(module: &IrModule) -> String {
    let mut gen = CCodeGen::new();
    gen.generate(module)
}

/// Per-module translation units (Phase B). Emits the full program once,
/// then splits on recorded boundaries: every unit gets the shared prelude
/// (includes, types, struct defs, prototypes, string constants) plus the
/// bodies of its own functions; the `main` unit also gets the tail (main
/// wrapper + runtime impl) and owns the global definitions. Other units
/// see globals as `extern` declarations — duplicated `static` globals
/// would silently fork state across TUs.
///
/// `func_modules` maps lowered function names to their origin module;
/// unknown names (comptime/derive/mono products) fall back to the base
/// name before a `__` mangled suffix, then to "main".
/// Returns units as (module_name, c_source), sorted by name.
pub fn generate_c_units(
    module: &IrModule,
    func_modules: &std::collections::HashMap<String, String>,
) -> Vec<(String, String)> {
    let mut gen = CCodeGen::new();
    let full = gen.generate(module);

    let first_fn_start = gen.fn_marks.iter().map(|(_, s, _)| *s).min().unwrap_or(gen.tail_mark);
    let prelude = &full[..first_fn_start];
    let mut tail = full[gen.tail_mark..].to_string();

    // The prelude carries a few small runtime *definitions* (call-stack
    // helpers, p_set_max_recursion, ...). They must live in exactly one TU
    // or the linker sees duplicate symbols — move them to the tail (main
    // unit) and leave a prototype behind.
    let mut prelude_stripped = String::with_capacity(prelude.len());
    let mut protos = String::new();
    let mut moved_defs = String::new();
    {
        let defs = crate::inc::c_fn_defs(prelude);
        let mut cursor = 0usize;
        for (_, s, brace, e) in defs {
            prelude_stripped.push_str(&prelude[cursor..s]);
            let header = prelude[s..brace].trim();
            protos.push_str(header);
            protos.push_str(";\n");
            moved_defs.push_str(&prelude[s..e]);
            moved_defs.push('\n');
            cursor = e;
        }
        prelude_stripped.push_str(&prelude[cursor..]);
        prelude_stripped.push_str(&protos);
        tail = format!("{}\n{}", moved_defs, tail);
    }
    let prelude = &prelude_stripped[..];

    let flat = std::env::var_os("FIDES_INC_FLAT").is_some();
    // The unit that owns the entry point, globals definitions, and the
    // runtime tail. This is the module `main` lives in — NOT always a unit
    // literally named "main" (a file `test_caps.fd` produces a `test_caps`
    // unit and no "main" unit at all).
    let owner = if flat {
        "main".to_string()
    } else {
        func_modules.get("main").cloned().unwrap_or_else(|| "main".to_string())
    };
    let unit_of = |name: &str| -> String {
        if flat { return "main".to_string(); }
        if let Some(m) = func_modules.get(name) {
            return m.clone();
        }
        if let Some(base) = name.split("__").next() {
            if let Some(m) = func_modules.get(base) {
                return m.clone();
            }
        }
        owner.clone()
    };

    // Group function bodies by unit.
    let mut bodies: std::collections::BTreeMap<String, String> = std::collections::BTreeMap::new();
    for (name, s, e) in &gen.fn_marks {
        let unit = unit_of(name);
        bodies.entry(unit).or_default().push_str(&full[*s..*e]);
    }

    // Which _strN constants each function references — replicates
    // emit_string_consts' ordering so names match. String consts are
    // `static`: emitting them per-unit (rather than once in the shared
    // prelude) keeps a string edit from invalidating every unit.
    let mut str_value_names: std::collections::HashMap<String, String> = std::collections::HashMap::new();
    let mut fn_str_names: std::collections::HashMap<String, std::collections::BTreeSet<String>> =
        std::collections::HashMap::new();
    {
        let mut idx = 0;
        for (fname, func) in &module.functions {
            for block in &func.blocks {
                for &inst_id in &block.instructions {
                    let key = match func.instructions.get(&inst_id) {
                        Some(crate::ir::Inst::ConstStr { value }) => Some(value.clone()),
                        Some(crate::ir::Inst::ConstFnRef { name }) => Some(name.clone()),
                        _ => None,
                    };
                    if let Some(k) = key {
                        let entry = str_value_names.entry(k).or_insert_with(|| {
                            let n = format!("_str{}", idx);
                            idx += 1;
                            n
                        });
                        fn_str_names.entry(fname.clone()).or_default().insert(entry.clone());
                    }
                }
            }
        }
    }

    // Split prelude into normal lines, global definitions, and string
    // constants. Globals → extern in non-owner units (a duplicated static
    // would fork state); string consts are re-emitted per-unit below.
    let mut pre_common = String::new();
    let mut global_defs: Vec<String> = Vec::new();
    let mut global_externs: Vec<String> = Vec::new();
    let mut str_decls: std::collections::HashMap<String, String> = std::collections::HashMap::new();
    for line in prelude.lines() {
        let t = line.trim_start();
        if t.starts_with("static ") && t.contains(" _g_") {
            let decl = t["static ".len()..].split_once(" = ").map(|(d, _)| d).unwrap_or(&t["static ".len()..]);
            let decl = decl.trim_end_matches(';');
            global_defs.push(format!("{};", decl));
            global_externs.push(format!("extern {};", decl));
        } else if t.starts_with("static const char _str") {
            if let Some(rest) = t.strip_prefix("static const char ") {
                let name = rest.split('[').next().unwrap_or("").trim().to_string();
                str_decls.insert(name, line.to_string());
            }
        } else {
            pre_common.push_str(line);
            pre_common.push('\n');
        }
    }

    // String consts referenced by each unit's functions.
    let mut unit_strs: std::collections::BTreeMap<String, std::collections::BTreeSet<String>> =
        std::collections::BTreeMap::new();
    for (name, _, _) in &gen.fn_marks {
        let unit = unit_of(name);
        if let Some(used) = fn_str_names.get(name) {
            unit_strs.entry(unit).or_default().extend(used.iter().cloned());
        }
    }

    let mut units: Vec<(String, String)> = Vec::new();
    for (name, body) in &bodies {
        let mut code = pre_common.clone();
        if let Some(used) = unit_strs.get(name) {
            for sname in used {
                if let Some(decl) = str_decls.get(sname) {
                    code.push_str(decl);
                    code.push('\n');
                }
            }
        }
        if *name == owner {
            for d in &global_defs {
                code.push_str(d);
                code.push('\n');
            }
        } else {
            for e in &global_externs {
                code.push_str(e);
                code.push('\n');
            }
        }
        code.push_str(body);
        if *name == owner {
            code.push_str(&tail);
        }
        units.push((name.clone(), code));
    }
    units
}

/// The suffix and C type of the checked-arithmetic helpers for `ty`.
fn c_int_helper(ty: &IrType) -> Option<(&'static str, &'static str)> {
    match ty {
        IrType::I8 => Some(("i8", "int8_t")),
        IrType::I16 => Some(("i16", "int16_t")),
        IrType::I32 => Some(("i32", "int32_t")),
        IrType::I64 => Some(("i64", "int64_t")),
        IrType::U8 => Some(("u8", "uint8_t")),
        IrType::U16 => Some(("u16", "uint16_t")),
        IrType::U32 => Some(("u32", "uint32_t")),
        IrType::U64 => Some(("u64", "uint64_t")),
        _ => None,
    }
}
