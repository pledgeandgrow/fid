(function_item
  (visibility)? @context
  "fn" @context
  name: (_) @name) @item

(struct_item
  (visibility)? @context
  "struct" @context
  name: (_) @name) @item

(enum_item
  (visibility)? @context
  "enum" @context
  name: (_) @name) @item

(cap_item
  (visibility)? @context
  "cap" @context
  name: (_) @name) @item

(impl_item
  "impl" @context
  type: (_) @name) @item

(const_item
  (visibility)? @context
  "const" @context
  name: (_) @name) @item

(type_alias
  (visibility)? @context
  "type" @context
  name: (_) @name) @item

(domain_item
  kind: _ @context
  name: (_) @name) @item
