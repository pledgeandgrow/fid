// C header parser — auto-generate Fides FFI bindings from C headers
//
// Parses C function declarations from .h files and generates Fides
// `extern "C"` declarations. Supports basic C types, pointers, and
// function prototypes.
//
// Usage: fidesc parse-header <file.h> [-o output.p]

use crate::ast::{ExternDecl, Param, Type};
use std::collections::HashSet;

/// Parse a C header file and extract function declarations.
pub fn parse_header(source: &str, include: &str) -> Vec<ExternDecl> {
    let mut decls = Vec::new();
    let mut seen_names: HashSet<String> = HashSet::new();

    // Strip comments
    let cleaned = strip_comments(source);

    // Tokenize into lines and look for function declarations
    // A C function declaration looks like:
    //   ret_type name(params);
    // We need to handle multi-line declarations
    let lines: Vec<&str> = cleaned.lines().collect();
    let mut i = 0;
    while i < lines.len() {
        let line = lines[i].trim();

        // Skip preprocessor directives, empty lines, typedefs, structs, etc.
        if line.is_empty() || line.starts_with('#') || line.starts_with("typedef")
            || line.starts_with("struct") || line.starts_with("enum")
            || line.starts_with("union") || line.starts_with("extern")
            || line.starts_with("static") || line.starts_with("inline")
        {
            i += 1;
            continue;
        }

        // Try to parse a function declaration
        // Collect lines until we find a semicolon
        let mut full_decl = line.to_string();
        while !full_decl.contains(';') && i + 1 < lines.len() {
            i += 1;
            full_decl.push(' ');
            full_decl.push_str(lines[i].trim());
        }

        if full_decl.contains('(') && full_decl.contains(')') && full_decl.contains(';') {
            if let Some(decl) = parse_c_function(&full_decl, include) {
                if seen_names.insert(decl.c_name.clone()) {
                    decls.push(decl);
                }
            }
        }

        i += 1;
    }

    decls
}

/// Strip C comments (/* ... */ and // ...)
fn strip_comments(source: &str) -> String {
    let mut result = String::new();
    let mut chars = source.chars().peekable();
    while let Some(c) = chars.next() {
        if c == '/' {
            if let Some(&next) = chars.peek() {
                if next == '/' {
                    // Line comment
                    chars.next();
                    while let Some(&c) = chars.peek() {
                        if c == '\n' { break; }
                        chars.next();
                    }
                    continue;
                } else if next == '*' {
                    // Block comment
                    chars.next();
                    while let Some(c2) = chars.next() {
                        if c2 == '*' {
                            if let Some(&'/') = chars.peek() {
                                chars.next();
                                break;
                            }
                        }
                    }
                    continue;
                }
            }
        }
        result.push(c);
    }
    result
}

/// Parse a single C function declaration into an ExternDecl
fn parse_c_function(decl: &str, include: &str) -> Option<ExternDecl> {
    // Find the function name and parameters
    // Pattern: ret_type name(params);
    let decl = decl.trim().trim_end_matches(';').trim();

    // Find the opening paren
    let paren_pos = decl.find('(')?;
    let before_paren = decl[..paren_pos].trim();
    let params_str = decl[paren_pos+1..].trim_end_matches(')').trim();

    // The function name is the last identifier before the paren
    let name_start = before_paren.rfind(|c: char| c.is_whitespace())?;
    let (ret_type_str, name) = before_paren.split_at(name_start);
    let name = name.trim();
    let ret_type_str = ret_type_str.trim();

    // Skip if name is empty or starts with non-identifier
    if name.is_empty() || !name.chars().next().map_or(false, |c| c.is_alphabetic() || c == '_') {
        return None;
    }

    // Skip common non-function patterns
    if name == "if" || name == "while" || name == "for" || name == "switch" {
        return None;
    }

    let ret_ty = parse_c_type(ret_type_str)?;
    let params = parse_c_params(params_str)?;

    Some(ExternDecl {
        abi: "C".to_string(),
        c_name: name.to_string(),
        pledge_name: name.to_string(),
        params,
        ret_ty,
        include: Some(include.to_string()),
        link_lib: None,
    })
}

/// Parse a C type string into a Fides Type
fn parse_c_type(c_type: &str) -> Option<Type> {
    let c_type = c_type.trim();
    // Remove qualifiers
    let c_type = c_type
        .replace("const ", "")
        .replace("volatile ", "")
        .replace("register ", "")
        .trim()
        .to_string();

    // Handle pointer types
    if c_type.ends_with('*') {
        let base = c_type.trim_end_matches('*').trim();
        if base == "char" || base == "void" {
            return Some(Type::Named("str".to_string()));
        }
        // Generic pointer → *u8
        return Some(Type::Ref {
            inner: Box::new(parse_c_type(base)?),
            is_mut: true,
        });
    }

    // Map C types to Fides types
    match c_type.as_str() {
        "int" | "signed int" | "signed" => Some(Type::I32),
        "long" | "signed long" | "long int" => Some(Type::I64),
        "long long" | "long long int" | "signed long long" => Some(Type::I64),
        "short" | "short int" | "signed short" => Some(Type::I16),
        "char" | "signed char" => Some(Type::I8),
        "unsigned" | "unsigned int" => Some(Type::U32),
        "unsigned long" | "unsigned long int" => Some(Type::U64),
        "unsigned long long" | "unsigned long long int" => Some(Type::U64),
        "unsigned short" | "unsigned short int" => Some(Type::U16),
        "unsigned char" => Some(Type::U8),
        "float" => Some(Type::F32),
        "double" => Some(Type::F64),
        "long double" => Some(Type::F64),
        "void" => Some(Type::Void),
        "size_t" => Some(Type::U64),
        "ssize_t" => Some(Type::I64),
        "int8_t" => Some(Type::I8),
        "int16_t" => Some(Type::I16),
        "int32_t" => Some(Type::I32),
        "int64_t" => Some(Type::I64),
        "uint8_t" => Some(Type::U8),
        "uint16_t" => Some(Type::U16),
        "uint32_t" => Some(Type::U32),
        "uint64_t" => Some(Type::U64),
        _ => {
            // Unknown type — map to opaque pointer
            Some(Type::Named(c_type))
        }
    }
}

/// Parse C function parameters
fn parse_c_params(params_str: &str) -> Option<Vec<Param>> {
    if params_str.trim().is_empty() || params_str.trim() == "void" {
        return Some(Vec::new());
    }

    let mut params = Vec::new();
    // Split by comma, handling nested parens
    let mut depth = 0;
    let mut current = String::new();
    for c in params_str.chars() {
        match c {
            '(' => { depth += 1; current.push(c); }
            ')' => { depth -= 1; current.push(c); }
            ',' if depth == 0 => {
                if let Some(p) = parse_c_param(&current) {
                    params.push(p);
                }
                current.clear();
            }
            _ => current.push(c),
        }
    }
    if !current.trim().is_empty() {
        if let Some(p) = parse_c_param(&current) {
            params.push(p);
        }
    }

    Some(params)
}

/// Parse a single C parameter
fn parse_c_param(param: &str) -> Option<Param> {
    let param = param.trim();
    // Remove qualifiers
    let param = param
        .replace("const ", "")
        .replace("restrict", "")
        .trim()
        .to_string();

    // Handle pointer types — name is after the last *
    if param.contains('*') {
        let parts: Vec<&str> = param.rsplitn(2, '*').collect();
        if parts.len() == 2 {
            let base_type = parts[1].trim();
            let name = parts[0].trim();
            let ty = if base_type == "char" {
                Type::Named("str".to_string())
            } else {
                Type::Ref {
                    inner: Box::new(parse_c_type(base_type)?),
                    is_mut: true,
                }
            };
            return Some(Param {
                name: if name.is_empty() { format!("arg{}", 0) } else { name.to_string() },
                ty,
                is_pub: false,
                annotations: Vec::new(),
            });
        }
    }

    // Non-pointer: split type and name
    // Find the last whitespace
    let last_space = param.rfind(|c: char| c.is_whitespace())?;
    let (type_str, name) = param.split_at(last_space);
    let name = name.trim();
    let ty = parse_c_type(type_str)?;

    Some(Param {
        name: if name.is_empty() { "arg".to_string() } else { name.to_string() },
        ty,
        is_pub: false,
        annotations: Vec::new(),
    })
}

/// Generate Fides source code from extern declarations
pub fn generate_pledge_bindings(decls: &[ExternDecl], include: &str) -> String {
    let mut out = String::new();
    out.push_str(&format!("// Auto-generated Fides FFI bindings from C header\n"));
    out.push_str(&format!("extern \"C\" include \"{}\" {{\n", include));
    for decl in decls {
        let ret = type_to_pledge(&decl.ret_ty);
        let params: Vec<String> = decl.params.iter()
            .map(|p| format!("{}: {}", p.name, type_to_pledge(&p.ty)))
            .collect();
        let params_str = if params.is_empty() {
            String::new()
        } else {
            params.join(", ")
        };
        out.push_str(&format!("    pf {}({}) -> {};\n", decl.c_name, params_str, ret));
    }
    out.push_str("}\n");
    out
}

fn type_to_pledge(ty: &Type) -> String {
    match ty {
        Type::I8 => "i8".to_string(),
        Type::I16 => "i16".to_string(),
        Type::I32 => "i32".to_string(),
        Type::I64 => "i64".to_string(),
        Type::U8 => "u8".to_string(),
        Type::U16 => "u16".to_string(),
        Type::U32 => "u32".to_string(),
        Type::U64 => "u64".to_string(),
        Type::F32 => "f32".to_string(),
        Type::F64 => "f64".to_string(),
        Type::Bool => "bool".to_string(),
        Type::Void => "void".to_string(),
        Type::Str => "str".to_string(),
        Type::Named(n) => n.clone(),
        Type::Ref { inner, .. } => format!("&{}", type_to_pledge(inner)),
        _ => "*u8".to_string(),
    }
}
