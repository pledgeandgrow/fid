// scribe — Documentation generator for the Fides programming language
//
// Parses .fd source files, extracts doc comments (///), and generates
// HTML documentation.
//
// Usage:
//   scribe <file.fd> [--output <dir>] [--format html|markdown]
//   scribe <dir>   [--output <dir>]
//
// Output:
//   HTML files in the specified output directory (default: docs/)

use std::fs;
use std::path::Path;

// ─── Doc extraction ─────────────────────────────────────────────────

/// A documented item extracted from source
#[derive(Debug, Clone)]
pub struct DocItem {
    pub kind: String,       // "function", "struct", "enum", "impl", "const", "trait"
    pub name: String,
    pub signature: String,  // source-level signature
    pub doc: String,        // doc comment text (/// lines joined)
    pub line: usize,        // source line number
    pub fields: Vec<(String, String, String)>, // (name, type, doc) for struct fields
    pub params: Vec<(String, String)>,         // (name, type) for function params
    pub ret_type: String,
    pub methods: Vec<DocItem>,  // for impl blocks
    pub variants: Vec<(String, String)>, // (name, doc) for enum variants
    pub is_pub: bool,
}

/// Extract doc comments and items from source text
pub fn extract_docs(source: &str) -> Vec<DocItem> {
    let lines: Vec<&str> = source.lines().collect();
    let mut items = Vec::new();
    let mut i = 0;

    while i < lines.len() {
        // Collect doc comments (/// prefix)
        let mut doc_lines: Vec<String> = Vec::new();
        while i < lines.len() {
            let trimmed = lines[i].trim_start();
            if let Some(rest) = trimmed.strip_prefix("///") {
                doc_lines.push(rest.trim_start().to_string());
                i += 1;
            } else if trimmed.starts_with("//") {
                // Regular comment — skip
                i += 1;
            } else if trimmed.is_empty() {
                i += 1;
            } else {
                break;
            }
        }

        if i >= lines.len() {
            break;
        }

        let line = lines[i].trim_start();
        let doc = doc_lines.join("\n");

        // Detect item type
        if line.starts_with("pub fn ") || line.starts_with("fn ") {
            let (item, end) = parse_function(&lines, i, doc);
            if let Some(it) = item {
                items.push(it);
            }
            i = end;
        } else if line.starts_with("pub struct ") || line.starts_with("struct ") {
            let (item, end) = parse_struct(&lines, i, doc);
            if let Some(it) = item {
                items.push(it);
            }
            i = end;
        } else if line.starts_with("pub enum ") || line.starts_with("enum ") {
            let (item, end) = parse_enum(&lines, i, doc);
            if let Some(it) = item {
                items.push(it);
            }
            i = end;
        } else if line.starts_with("impl ") {
            let (item, end) = parse_impl(&lines, i, doc);
            if let Some(it) = item {
                items.push(it);
            }
            i = end;
        } else if line.starts_with("pub fn ") || line.starts_with("fn ") {
            // Already handled above
            i += 1;
        } else if line.starts_with("pub const ") || line.starts_with("const ") {
            let (item, end) = parse_const(&lines, i, doc);
            if let Some(it) = item {
                items.push(it);
            }
            i = end;
        } else if line.starts_with("cap ") || line.starts_with("pub cap ") {
            let (item, end) = parse_trait(&lines, i, doc);
            if let Some(it) = item {
                items.push(it);
            }
            i = end;
        } else {
            i += 1;
        }
    }

    items
}

fn parse_function(lines: &[&str], start: usize, doc: String) -> (Option<DocItem>, usize) {
    let line = lines[start].trim_start();
    let is_pub = line.starts_with("pub ");
    let sig_start = if is_pub { 4 } else { 0 };
    let sig = &line[sig_start..];

    // Extract name: "fn name(" or "fn name<T>("
    let name = extract_name_after(sig, "fn ");
    if name.is_empty() {
        return (None, start + 1);
    }

    // Extract params by finding the opening ( and matching )
    let (params, ret_type) = extract_params_and_return(line);

    // Find end of function (opening { or ; for declarations)
    let mut end = start;
    let mut brace_depth = 0;
    let mut found_brace = false;
    while end < lines.len() {
        let l = lines[end];
        for ch in l.chars() {
            if ch == '{' {
                brace_depth += 1;
                found_brace = true;
            } else if ch == '}' {
                brace_depth -= 1;
            }
        }
        end += 1;
        if found_brace && brace_depth == 0 {
            break;
        }
        // Declaration without body
        if !found_brace && l.trim_end().ends_with(';') {
            break;
        }
    }

    let item = DocItem {
        kind: "function".to_string(),
        name,
        signature: line.trim().to_string(),
        doc,
        line: start + 1,
        fields: Vec::new(),
        params,
        ret_type,
        methods: Vec::new(),
        variants: Vec::new(),
        is_pub,
    };

    (Some(item), end)
}

fn parse_struct(lines: &[&str], start: usize, doc: String) -> (Option<DocItem>, usize) {
    let line = lines[start].trim_start();
    let is_pub = line.starts_with("pub ");
    let sig = if is_pub { &line[4..] } else { line };

    let name = extract_name_after(sig, "struct ");
    if name.is_empty() {
        return (None, start + 1);
    }

    // Parse fields inside { }
    let mut fields = Vec::new();
    let mut end = start;
    let mut in_body = false;
    let mut brace_depth = 0;
    while end < lines.len() {
        let l = lines[end].trim();
        for ch in l.chars() {
            if ch == '{' { brace_depth += 1; in_body = true; }
            else if ch == '}' { brace_depth -= 1; }
        }
        if in_body && brace_depth > 0 && !l.starts_with('{') && !l.starts_with('}') {
            // Parse field: "pub x: f64," or "x: i64,"
            let field_line = l.trim_end_matches(',');
            if let Some(colon_pos) = field_line.find(':') {
                let field_name = field_line[..colon_pos].trim().trim_start_matches("pub ").trim().to_string();
                let field_type = field_line[colon_pos+1..].trim().to_string();
                if !field_name.is_empty() && !field_name.starts_with("//") {
                    fields.push((field_name, field_type, String::new()));
                }
            }
        }
        end += 1;
        if in_body && brace_depth == 0 {
            break;
        }
    }

    let item = DocItem {
        kind: "struct".to_string(),
        name,
        signature: line.trim().to_string(),
        doc,
        line: start + 1,
        fields,
        params: Vec::new(),
        ret_type: String::new(),
        methods: Vec::new(),
        variants: Vec::new(),
        is_pub,
    };

    (Some(item), end)
}

fn parse_enum(lines: &[&str], start: usize, doc: String) -> (Option<DocItem>, usize) {
    let line = lines[start].trim_start();
    let is_pub = line.starts_with("pub ");
    let sig = if is_pub { &line[4..] } else { line };

    let name = extract_name_after(sig, "enum ");
    if name.is_empty() {
        return (None, start + 1);
    }

    // Parse variants inside { }
    let mut variants = Vec::new();
    let mut end = start;
    let mut in_body = false;
    let mut brace_depth = 0;
    while end < lines.len() {
        let l = lines[end].trim();
        for ch in l.chars() {
            if ch == '{' { brace_depth += 1; in_body = true; }
            else if ch == '}' { brace_depth -= 1; }
        }
        if in_body && brace_depth > 0 && !l.starts_with('{') && !l.starts_with('}') && !l.is_empty() {
            let var_line = l.trim_end_matches(',');
            // Variant: "Ok(i64)" or "None" or "Ok(i64, str)"
            let var_name = var_line.split(|c: char| !c.is_alphanumeric() && c != '_').next().unwrap_or("").to_string();
            if !var_name.is_empty() {
                variants.push((var_name, String::new()));
            }
        }
        end += 1;
        if in_body && brace_depth == 0 {
            break;
        }
    }

    let item = DocItem {
        kind: "enum".to_string(),
        name,
        signature: line.trim().to_string(),
        doc,
        line: start + 1,
        fields: Vec::new(),
        params: Vec::new(),
        ret_type: String::new(),
        methods: Vec::new(),
        variants,
        is_pub,
    };

    (Some(item), end)
}

fn parse_impl(lines: &[&str], start: usize, doc: String) -> (Option<DocItem>, usize) {
    let line = lines[start].trim_start();
    let name = extract_name_after(line, "impl ");
    // Could be "impl Trait for Struct" — extract struct name
    let struct_name = if let Some(fpos) = name.find(" for ") {
        name[fpos+5..].trim().to_string()
    } else {
        name.trim().to_string()
    };

    // Parse methods inside { }
    let mut methods = Vec::new();
    let mut end = start;
    let mut brace_depth = 0;
    let mut in_body = false;
    let mut method_start;
    let mut method_doc: Vec<String> = Vec::new();
    while end < lines.len() {
        let l = lines[end].trim_start();

        // Collect doc comments for methods
        if let Some(rest) = l.strip_prefix("///") {
            method_doc.push(rest.trim().to_string());
            end += 1;
            continue;
        }
        if l.starts_with("//") || l.is_empty() {
            if !l.starts_with("///") {
                method_doc.clear();
            }
            end += 1;
            continue;
        }

        for ch in l.chars() {
            if ch == '{' { brace_depth += 1; in_body = true; }
            else if ch == '}' { brace_depth -= 1; }
        }

        // Detect method: "pub fn name(" or "fn name("
        if in_body && brace_depth == 1 && (l.starts_with("pub fn ") || l.starts_with("fn ")) {
            method_start = end;
            let method_doc_str = method_doc.join("\n");
            method_doc.clear();
            let (item, m_end) = parse_function(lines, method_start, method_doc_str);
            if let Some(it) = item {
                methods.push(it);
            }
            end = m_end;
            continue;
        }

        end += 1;
        if in_body && brace_depth == 0 {
            break;
        }
    }

    let item = DocItem {
        kind: "impl".to_string(),
        name: struct_name.clone(),
        signature: line.trim().to_string(),
        doc,
        line: start + 1,
        fields: Vec::new(),
        params: Vec::new(),
        ret_type: String::new(),
        methods,
        variants: Vec::new(),
        is_pub: true,
    };

    (Some(item), end)
}

fn parse_const(lines: &[&str], start: usize, doc: String) -> (Option<DocItem>, usize) {
    let line = lines[start].trim_start();
    let is_pub = line.starts_with("pub ");
    let sig = if is_pub { &line[4..] } else { line };
    let name = extract_name_after(sig, "const ");

    let item = DocItem {
        kind: "const".to_string(),
        name,
        signature: line.trim().to_string(),
        doc,
        line: start + 1,
        fields: Vec::new(),
        params: Vec::new(),
        ret_type: String::new(),
        methods: Vec::new(),
        variants: Vec::new(),
        is_pub,
    };

    (Some(item), start + 1)
}

fn parse_trait(lines: &[&str], start: usize, doc: String) -> (Option<DocItem>, usize) {
    let line = lines[start].trim_start();
    let is_pub = line.starts_with("pub ");
    let sig = if is_pub { &line[4..] } else { line };
    let name = extract_name_after(sig, "cap ");

    // Find end
    let mut end = start;
    let mut brace_depth = 0;
    while end < lines.len() {
        for ch in lines[end].chars() {
            if ch == '{' { brace_depth += 1; }
            else if ch == '}' { brace_depth -= 1; }
        }
        end += 1;
        if brace_depth == 0 && end > start + 1 {
            break;
        }
    }

    let item = DocItem {
        kind: "trait".to_string(),
        name,
        signature: line.trim().to_string(),
        doc,
        line: start + 1,
        fields: Vec::new(),
        params: Vec::new(),
        ret_type: String::new(),
        methods: Vec::new(),
        variants: Vec::new(),
        is_pub,
    };

    (Some(item), end)
}

// ─── Helpers ────────────────────────────────────────────────────────

fn extract_name_after(s: &str, prefix: &str) -> String {
    if let Some(pos) = s.find(prefix) {
        let after = &s[pos + prefix.len()..];
        // Name ends at (, <, {, space, or ;
        let end = after.find(|c: char| c == '(' || c == '<' || c == '{' || c == ' ' || c == ';').unwrap_or(after.len());
        after[..end].trim().to_string()
    } else {
        String::new()
    }
}

fn extract_params_and_return(line: &str) -> (Vec<(String, String)>, String) {
    let mut params = Vec::new();
    let mut ret_type = String::new();

    // Find params between first ( and matching )
    if let Some(start) = line.find('(') {
        let mut depth = 1;
        let mut end = start + 1;
        let bytes = line.as_bytes();
        while end < bytes.len() && depth > 0 {
            if bytes[end] == b'(' { depth += 1; }
            else if bytes[end] == b')' { depth -= 1; }
            end += 1;
        }
        // A parameter list that runs on to the next line has no closing paren here
        let params_str = if depth == 0 { &line[start+1..end-1] } else { &line[start+1..] };
        for param in params_str.split(',') {
            let param = param.trim();
            if param.is_empty() { continue; }
            if let Some(colon) = param.find(':') {
                let name = param[..colon].trim()
                    .trim_start_matches("pub ").trim().to_string();
                let ty = param[colon+1..].trim().to_string();
                params.push((name, ty));
            }
        }

        // Return type after -> 
        let after_parens = &line[end..];
        if let Some(arrow) = after_parens.find("->") {
            let ret = after_parens[arrow+2..].trim();
            let ret_end = ret.find(|c: char| c == '{' || c == ';').unwrap_or(ret.len());
            ret_type = ret[..ret_end].trim().to_string();
        }
    }

    (params, ret_type)
}

fn escape_html(s: &str) -> String {
    s.replace('&', "&amp;")
     .replace('<', "&lt;")
     .replace('>', "&gt;")
     .replace('"', "&quot;")
}

// ─── HTML generation ────────────────────────────────────────────────

pub fn generate_html(items: &[DocItem], module_name: &str) -> String {
    let mut html = String::new();

    html.push_str("<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n");
    html.push_str(&format!("<title>{} — Fides Documentation</title>\n", escape_html(module_name)));
    html.push_str("<meta charset=\"utf-8\">\n");
    html.push_str("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n");
    html.push_str("<style>\n");
    html.push_str(CSS);
    html.push_str("</style>\n");
    html.push_str("</head>\n<body>\n");

    html.push_str(&format!("<h1>{}</h1>\n", escape_html(module_name)));
    html.push_str("<nav class=\"sidebar\">\n<ul>\n");
    for item in items {
        if item.is_pub || item.kind == "impl" {
            html.push_str(&format!("<li><a href=\"#{}\">{} <span class=\"kind\">{}</span></a></li>\n",
                escape_html(&item.name), escape_html(&item.name), item.kind));
        }
    }
    html.push_str("</ul>\n</nav>\n");

    html.push_str("<main>\n");

    // Group items by kind
    let mut functions = Vec::new();
    let mut structs = Vec::new();
    let mut enums = Vec::new();
    let mut impls = Vec::new();
    let mut consts = Vec::new();
    let mut traits = Vec::new();

    for item in items {
        match item.kind.as_str() {
            "function" if item.is_pub => functions.push(item),
            "struct" if item.is_pub => structs.push(item),
            "enum" if item.is_pub => enums.push(item),
            "impl" => impls.push(item),
            "const" if item.is_pub => consts.push(item),
            "trait" => traits.push(item),
            _ => {}
        }
    }

    if !functions.is_empty() {
        html.push_str("<h2 id=\"functions\">Functions</h2>\n");
        for item in functions {
            html.push_str(&render_function(item));
        }
    }

    if !structs.is_empty() {
        html.push_str("<h2 id=\"structs\">Structs</h2>\n");
        for item in structs {
            html.push_str(&render_struct(item));
        }
    }

    if !enums.is_empty() {
        html.push_str("<h2 id=\"enums\">Enums</h2>\n");
        for item in enums {
            html.push_str(&render_enum(item));
        }
    }

    if !impls.is_empty() {
        html.push_str("<h2 id=\"impls\">Implementations</h2>\n");
        for item in impls {
            html.push_str(&render_impl(item));
        }
    }

    if !consts.is_empty() {
        html.push_str("<h2 id=\"consts\">Constants</h2>\n");
        for item in consts {
            html.push_str(&render_const(item));
        }
    }

    if !traits.is_empty() {
        html.push_str("<h2 id=\"traits\">Traits</h2>\n");
        for item in traits {
            html.push_str(&render_trait(item));
        }
    }

    html.push_str("</main>\n");
    html.push_str("<footer>\n<p>Generated by <a href=\"https://fides.dev\">scribe</a> — Fides documentation generator</p>\n</footer>\n");
    html.push_str("</body>\n</html>\n");

    html
}

fn render_function(item: &DocItem) -> String {
    let mut html = String::new();
    html.push_str(&format!("<section id=\"{}\">\n", escape_html(&item.name)));
    html.push_str(&format!("<h3>{}</h3>\n", escape_html(&item.name)));
    if !item.doc.is_empty() {
        html.push_str(&format!("<div class=\"doc\">{}</div>\n", escape_html(&item.doc).replace('\n', "<br>\n")));
    }
    html.push_str("<pre class=\"signature\">");
    html.push_str(&escape_html(&item.signature));
    html.push_str("</pre>\n");

    if !item.params.is_empty() {
        html.push_str("<h4>Parameters</h4>\n<table>\n");
        for (name, ty) in &item.params {
            html.push_str(&format!("<tr><td class=\"name\">{}</td><td class=\"type\">{}</td></tr>\n",
                escape_html(name), escape_html(ty)));
        }
        html.push_str("</table>\n");
    }

    if !item.ret_type.is_empty() {
        html.push_str(&format!("<h4>Returns</h4>\n<p class=\"type\">{}</p>\n", escape_html(&item.ret_type)));
    }

    html.push_str(&format!("<p class=\"location\">Defined at line {}</p>\n", item.line));
    html.push_str("</section>\n");
    html
}

fn render_struct(item: &DocItem) -> String {
    let mut html = String::new();
    html.push_str(&format!("<section id=\"{}\">\n", escape_html(&item.name)));
    html.push_str(&format!("<h3>{}</h3>\n", escape_html(&item.name)));
    if !item.doc.is_empty() {
        html.push_str(&format!("<div class=\"doc\">{}</div>\n", escape_html(&item.doc).replace('\n', "<br>\n")));
    }
    html.push_str("<pre class=\"signature\">");
    html.push_str(&escape_html(&item.signature));
    html.push_str("</pre>\n");

    if !item.fields.is_empty() {
        html.push_str("<h4>Fields</h4>\n<table>\n");
        for (name, ty, doc) in &item.fields {
            html.push_str(&format!("<tr><td class=\"name\">{}</td><td class=\"type\">{}</td><td class=\"doc\">{}</td></tr>\n",
                escape_html(name), escape_html(ty), escape_html(doc)));
        }
        html.push_str("</table>\n");
    }

    html.push_str(&format!("<p class=\"location\">Defined at line {}</p>\n", item.line));
    html.push_str("</section>\n");
    html
}

fn render_enum(item: &DocItem) -> String {
    let mut html = String::new();
    html.push_str(&format!("<section id=\"{}\">\n", escape_html(&item.name)));
    html.push_str(&format!("<h3>{}</h3>\n", escape_html(&item.name)));
    if !item.doc.is_empty() {
        html.push_str(&format!("<div class=\"doc\">{}</div>\n", escape_html(&item.doc).replace('\n', "<br>\n")));
    }
    html.push_str("<pre class=\"signature\">");
    html.push_str(&escape_html(&item.signature));
    html.push_str("</pre>\n");

    if !item.variants.is_empty() {
        html.push_str("<h4>Variants</h4>\n<ul>\n");
        for (name, doc) in &item.variants {
            html.push_str(&format!("<li><span class=\"name\">{}</span> <span class=\"doc\">{}</span></li>\n",
                escape_html(name), escape_html(doc)));
        }
        html.push_str("</ul>\n");
    }

    html.push_str(&format!("<p class=\"location\">Defined at line {}</p>\n", item.line));
    html.push_str("</section>\n");
    html
}

fn render_impl(item: &DocItem) -> String {
    let mut html = String::new();
    html.push_str(&format!("<section id=\"impl-{}\">\n", escape_html(&item.name)));
    html.push_str(&format!("<h3>impl {}</h3>\n", escape_html(&item.name)));
    if !item.doc.is_empty() {
        html.push_str(&format!("<div class=\"doc\">{}</div>\n", escape_html(&item.doc).replace('\n', "<br>\n")));
    }

    if !item.methods.is_empty() {
        html.push_str("<h4>Methods</h4>\n");
        for method in &item.methods {
            html.push_str(&render_function(method));
        }
    }

    html.push_str("</section>\n");
    html
}

fn render_const(item: &DocItem) -> String {
    let mut html = String::new();
    html.push_str(&format!("<section id=\"{}\">\n", escape_html(&item.name)));
    html.push_str(&format!("<h3>{}</h3>\n", escape_html(&item.name)));
    if !item.doc.is_empty() {
        html.push_str(&format!("<div class=\"doc\">{}</div>\n", escape_html(&item.doc).replace('\n', "<br>\n")));
    }
    html.push_str("<pre class=\"signature\">");
    html.push_str(&escape_html(&item.signature));
    html.push_str("</pre>\n");
    html.push_str(&format!("<p class=\"location\">Defined at line {}</p>\n", item.line));
    html.push_str("</section>\n");
    html
}

fn render_trait(item: &DocItem) -> String {
    let mut html = String::new();
    html.push_str(&format!("<section id=\"{}\">\n", escape_html(&item.name)));
    html.push_str(&format!("<h3>cap {}</h3>\n", escape_html(&item.name)));
    if !item.doc.is_empty() {
        html.push_str(&format!("<div class=\"doc\">{}</div>\n", escape_html(&item.doc).replace('\n', "<br>\n")));
    }
    html.push_str("<pre class=\"signature\">");
    html.push_str(&escape_html(&item.signature));
    html.push_str("</pre>\n");
    html.push_str(&format!("<p class=\"location\">Defined at line {}</p>\n", item.line));
    html.push_str("</section>\n");
    html
}

const CSS: &str = r#"
body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif; margin: 0; padding: 0; color: #333; }
h1 { background: #1a1a2e; color: #fff; padding: 20px 30px; margin: 0; }
h2 { border-bottom: 2px solid #1a1a2e; padding-bottom: 5px; margin-top: 40px; }
h3 { color: #1a1a2e; margin-top: 30px; }
h4 { color: #555; margin-top: 15px; }
.sidebar { float: right; width: 250px; background: #f5f5f5; padding: 15px; margin: 20px; border-radius: 8px; }
.sidebar ul { list-style: none; padding: 0; }
.sidebar li { padding: 4px 0; }
.sidebar a { text-decoration: none; color: #1a1a2e; }
.sidebar a:hover { text-decoration: underline; }
main { margin: 20px 30px; max-width: 800px; }
section { background: #fff; padding: 15px; margin: 10px 0; border: 1px solid #eee; border-radius: 8px; }
.signature { background: #f8f8f8; padding: 10px; border-radius: 5px; overflow-x: auto; font-family: 'Fira Code', monospace; font-size: 14px; }
.doc { color: #666; margin: 10px 0; line-height: 1.5; }
table { border-collapse: collapse; width: 100%; margin: 10px 0; }
td, th { padding: 8px 12px; text-align: left; border-bottom: 1px solid #eee; }
td.name { font-weight: bold; width: 30%; }
td.type { font-family: monospace; color: #c0392b; }
.kind { font-size: 11px; color: #999; }
.location { font-size: 12px; color: #aaa; }
footer { text-align: center; padding: 20px; color: #999; font-size: 13px; }
footer a { color: #1a1a2e; }
"#;

// ─── Markdown generation ────────────────────────────────────────────

pub fn generate_markdown(items: &[DocItem], module_name: &str) -> String {
    let mut md = String::new();
    md.push_str(&format!("# {} — Fides Documentation\n\n", module_name));

    // Table of contents
    md.push_str("## Contents\n\n");
    for item in items {
        if item.is_pub || item.kind == "impl" {
            md.push_str(&format!("- [{}](#{}) ({})\n", item.name, item.name.to_lowercase(), item.kind));
        }
    }
    md.push('\n');

    for item in items {
        if !item.is_pub && item.kind != "impl" { continue; }
        md.push_str(&format!("## {}\n\n", item.name));
        if !item.doc.is_empty() {
            md.push_str(&format!("{}\n\n", item.doc));
        }
        md.push_str(&format!("```fides\n{}\n```\n\n", item.signature));

        match item.kind.as_str() {
            "function" => {
                if !item.params.is_empty() {
                    md.push_str("**Parameters:**\n\n");
                    for (name, ty) in &item.params {
                        md.push_str(&format!("- `{}`: `{}`\n", name, ty));
                    }
                    md.push('\n');
                }
                if !item.ret_type.is_empty() {
                    md.push_str(&format!("**Returns:** `{}`\n\n", item.ret_type));
                }
            }
            "struct" => {
                if !item.fields.is_empty() {
                    md.push_str("**Fields:**\n\n");
                    md.push_str("| Name | Type |\n|------|------|\n");
                    for (name, ty, _) in &item.fields {
                        md.push_str(&format!("| `{}` | `{}` |\n", name, ty));
                    }
                    md.push('\n');
                }
            }
            "enum" => {
                if !item.variants.is_empty() {
                    md.push_str("**Variants:**\n\n");
                    for (name, _) in &item.variants {
                        md.push_str(&format!("- `{}`\n", name));
                    }
                    md.push('\n');
                }
            }
            "impl" => {
                if !item.methods.is_empty() {
                    md.push_str("**Methods:**\n\n");
                    for method in &item.methods {
                        md.push_str(&format!("### {}\n\n", method.name));
                        if !method.doc.is_empty() {
                            md.push_str(&format!("{}\n\n", method.doc));
                        }
                        md.push_str(&format!("```fides\n{}\n```\n\n", method.signature));
                    }
                }
            }
            _ => {}
        }

        md.push_str(&format!("*Defined at line {}*\n\n---\n\n", item.line));
    }

    md.push_str("*Generated by [scribe](https://fides.dev) — Fides documentation generator*\n");
    md
}

// ─── File processing ────────────────────────────────────────────────

pub fn process_file(input: &Path, output_dir: &Path, format: &str) -> Result<(), String> {
    let source = fs::read_to_string(input)
        .map_err(|e| format!("Cannot read {}: {}", input.display(), e))?;

    let items = extract_docs(&source);

    // Extract module name from source or filename
    let module_name = source.lines()
        .find(|l| l.trim_start().starts_with("module "))
        .and_then(|l| {
            let s = l.trim_start();
            let s = &s[7..]; // skip "module "
            Some(s.trim_end_matches(';').trim().to_string())
        })
        .unwrap_or_else(|| {
            input.file_stem()
                .and_then(|s| s.to_str())
                .unwrap_or("module")
                .to_string()
        });

    fs::create_dir_all(output_dir)
        .map_err(|e| format!("Cannot create output dir: {}", e))?;

    let output_file = match format {
        "markdown" | "md" => {
            let content = generate_markdown(&items, &module_name);
            let path = output_dir.join(format!("{}.md", module_name));
            fs::write(&path, content)
                .map_err(|e| format!("Cannot write {}: {}", path.display(), e))?;
            path
        }
        _ => {
            let content = generate_html(&items, &module_name);
            let path = output_dir.join(format!("{}.html", module_name));
            fs::write(&path, content)
                .map_err(|e| format!("Cannot write {}: {}", path.display(), e))?;
            path
        }
    };

    println!("Generated {} ({} items)", output_file.display(), items.len());
    Ok(())
}

pub fn process_directory(input_dir: &Path, output_dir: &Path, format: &str) -> Result<(), String> {
    let entries = fs::read_dir(input_dir)
        .map_err(|e| format!("Cannot read directory: {}", e))?;

    let mut count = 0;
    for entry in entries {
        let entry = entry.map_err(|e| format!("Error reading entry: {}", e))?;
        let path = entry.path();
        if path.extension().and_then(|e| e.to_str()) == Some("fd") {
            process_file(&path, output_dir, format)?;
            count += 1;
        }
    }

    if count == 0 {
        return Err(format!("No .fd files found in {}", input_dir.display()));
    }

    println!("Generated documentation for {} file(s) in {}", count, output_dir.display());
    Ok(())
}

// ─── Help ───────────────────────────────────────────────────────────

pub fn print_help() {
    println!("scribe — Documentation generator for Fides");
    println!();
    println!("USAGE:");
    println!("    scribe <file.fd> [--output <dir>] [--format html|markdown]");
    println!("    scribe <dir>    [--output <dir>] [--format html|markdown]");
    println!();
    println!("OPTIONS:");
    println!("    --output <dir>      Output directory (default: docs/)");
    println!("    --format <format>   Output format: html (default) or markdown");
    println!("    --help, -h          Show this help");
    println!();
    println!("EXAMPLES:");
    println!("    scribe stdlib/math.fd                    # Generate HTML docs");
    println!("    scribe stdlib/ --output docs/            # Generate docs for all stdlib");
    println!("    scribe stdlib/ --format markdown         # Generate Markdown docs");
}
