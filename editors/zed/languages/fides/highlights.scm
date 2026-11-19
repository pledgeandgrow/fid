; Fides highlights (Zed, Neovim, Helix). The general patterns come first;
; a later, more specific pattern wins.
; editors/zed/languages/fides/highlights.scm is a copy of this file
; (tests/tooling/zed-check.sh checks that they are the same).

(identifier) @variable

((identifier) @constant
  (#match? @constant "^[A-Z][A-Z0-9_]+$"))

((identifier) @variable.special
  (#eq? @variable.special "self"))

(type_identifier) @type
(primitive_type) @type.builtin
(never_type) @type.builtin

(scoped_identifier
  path: (identifier) @type
  (#match? @type "^[A-Z]"))

(type_parameter name: (identifier) @type)
(effects (identifier) @type)
(domain_item name: (identifier) @type)
(query_statement (type_identifier) @type)

(module_declaration (path (identifier) @namespace))
(use_path (identifier) @namespace)

; functions and calls
(function_item name: (identifier) @function)
(extern_function name: (identifier) @function)
(call_expression function: (identifier) @function)
(call_expression function: (scoped_identifier name: (identifier) @function))
(call_expression function: (field_expression field: (identifier) @function.method))
(generic_function function: (identifier) @function)
(macro_invocation macro: (identifier) @function.special)
(region_option name: (identifier) @function)

; fields, variants, parameters
(field_expression field: (identifier) @property)
(optional_field_expression field: (identifier) @property)
(field_declaration name: (identifier) @property)
(field_initializer field: (identifier) @property)
(named_argument name: (identifier) @property)
(enum_variant name: (identifier) @variant)
(tuple_struct_pattern type: (identifier) @variant)
(tuple_struct_pattern type: (scoped_identifier name: (identifier) @variant))
(parameter name: (identifier) @variable.parameter)
(binding name: (identifier) @variable.parameter)

; annotations and attributes
(annotation "@" @attribute)
(annotation (identifier) @attribute)
(attribute_item) @attribute

; literals and comments
(string_literal) @string
(raw_string_literal) @string
(format_string) @string
(escape_sequence) @string.escape
(char_literal) @string.special
(integer_literal) @number
(float_literal) @number
(boolean_literal) @boolean
(wildcard_pattern) @variable.special

(line_comment) @comment
(block_comment) @comment
((line_comment) @comment.doc
  (#match? @comment.doc "^///"))

[
  "module"
  "use"
  "as"
  "with"
  "pub"
  "async"
  "comptime"
  "interrupt"
  "fn"
  "const"
  "given"
  "ensure"
  "struct"
  "enum"
  "impl"
  "for"
  "type"
  "cap"
  "var"
  "static"
  "val"
  "extern"
  "include"
  "link"
  "layer"
  "model"
  "component"
  "system"
  "state"
  "on"
  "dyn"
  "mut"
  "persist"
  "return"
  "break"
  "continue"
  "while"
  "in"
  "loop"
  "guard"
  "defer"
  "region"
  "gpu"
  "kv_cache"
  "query"
  "grad"
  "unsafe"
  "sandbox"
  "txn"
  "spawn"
  "scope"
  "select"
  "parallel"
  "handle"
  "await"
  "yield"
  "do"
  "is"
  "if"
  "else"
  "match"
  "gen"
  "try"
  "resume"
  "backward"
] @keyword

[
  "+"
  "-"
  "*"
  "/"
  "%"
  "=="
  "!="
  "<"
  "<="
  ">"
  ">="
  "<<"
  ">>"
  "&"
  "&&"
  "|"
  "||"
  "|>"
  "^"
  "~"
  "~>"
  "!"
  ".."
  "..="
  "..."
  "??"
  "?"
  "?."
  "="
  "+="
  "-="
  "*="
  "/="
  "%="
  "&="
  "|="
  "^="
  "<<="
  ">>="
  "->"
  "=>"
] @operator

["(" ")" "[" "]" "{" "}"] @punctuation.bracket

["," ";" ":" "::" "." "#"] @punctuation.delimiter
