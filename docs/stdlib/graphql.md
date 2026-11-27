[All modules](index.md)

# graphql -- Fides Documentation

## Contents

- [GT_PUNCT:](#GT_PUNCT:) (const)
- [GT_NAME:](#GT_NAME:) (const)
- [GT_INT:](#GT_INT:) (const)
- [GT_FLOAT:](#GT_FLOAT:) (const)
- [GT_STRING:](#GT_STRING:) (const)
- [GT_EOF:](#GT_EOF:) (const)
- [GT_ERROR:](#GT_ERROR:) (const)
- [GqlTok](#GqlTok) (struct)
- [gql_lex](#gql_lex) (function)
- [GN_DOCUMENT:](#GN_DOCUMENT:) (const)
- [GN_OPERATION:](#GN_OPERATION:) (const)
- [GN_VARDEF:](#GN_VARDEF:) (const)
- [GN_SELSET:](#GN_SELSET:) (const)
- [GN_FIELD:](#GN_FIELD:) (const)
- [GN_SPREAD:](#GN_SPREAD:) (const)
- [GN_INLINE:](#GN_INLINE:) (const)
- [GN_FRAGMENT:](#GN_FRAGMENT:) (const)
- [GN_ARG:](#GN_ARG:) (const)
- [GN_DIRECTIVE:](#GN_DIRECTIVE:) (const)
- [GN_VAR:](#GN_VAR:) (const)
- [GN_INT:](#GN_INT:) (const)
- [GN_FLOAT:](#GN_FLOAT:) (const)
- [GN_STRING:](#GN_STRING:) (const)
- [GN_BOOL:](#GN_BOOL:) (const)
- [GN_NULL:](#GN_NULL:) (const)
- [GN_ENUM:](#GN_ENUM:) (const)
- [GN_LIST:](#GN_LIST:) (const)
- [GN_OBJECT:](#GN_OBJECT:) (const)
- [GqlNode](#GqlNode) (struct)
- [GqlDoc](#GqlDoc) (struct)
- [gql_parse](#gql_parse) (function)
- [gql_operations](#gql_operations) (function)
- [gql_value_json](#gql_value_json) (function)
- [GqlField](#GqlField) (struct)
- [GqlType](#GqlType) (struct)
- [GqlSchema](#GqlSchema) (struct)
- [gql_parse_schema](#gql_parse_schema) (function)
- [gql_type_index](#gql_type_index) (function)
- [gql_field_names](#gql_field_names) (function)
- [gql_execute](#gql_execute) (function)

## GT_PUNCT:

```fides
pub const GT_PUNCT: i64 = 0;
```

*Defined at line 44*

---

## GT_NAME:

```fides
pub const GT_NAME: i64 = 1;
```

*Defined at line 45*

---

## GT_INT:

```fides
pub const GT_INT: i64 = 2;
```

*Defined at line 46*

---

## GT_FLOAT:

```fides
pub const GT_FLOAT: i64 = 3;
```

*Defined at line 47*

---

## GT_STRING:

```fides
pub const GT_STRING: i64 = 4;
```

*Defined at line 48*

---

## GT_EOF:

```fides
pub const GT_EOF: i64 = 5;
```

*Defined at line 49*

---

## GT_ERROR:

```fides
pub const GT_ERROR: i64 = 6;
```

*Defined at line 50*

---

## GqlTok

```fides
pub struct GqlTok {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `text` | `str` |
| `line` | `i64` |
| `block` | `bool,  // a block string (descriptions)` |

*Defined at line 52*

---

## gql_lex

Tokenize GraphQL source. A lexical error puts a GT_ERROR token (its
text says what was wrong) before the final GT_EOF.

```fides
pub fn gql_lex(src: str) -> Vec<GqlTok> {
```

**Parameters:**

- `src`: `str`

**Returns:** `Vec<GqlTok>`

*Defined at line 152*

---

## GN_DOCUMENT:

```fides
pub const GN_DOCUMENT: i64 = 0;
```

*Defined at line 328*

---

## GN_OPERATION:

```fides
pub const GN_OPERATION: i64 = 1;  // op: query | mutation | subscription
```

*Defined at line 329*

---

## GN_VARDEF:

```fides
pub const GN_VARDEF: i64 = 2;  // name, type_ref, a = default value or -1
```

*Defined at line 330*

---

## GN_SELSET:

```fides
pub const GN_SELSET: i64 = 3;  // children = selections
```

*Defined at line 331*

---

## GN_FIELD:

```fides
pub const GN_FIELD: i64 = 4;  // name, alias, args, dirs, sel
```

*Defined at line 332*

---

## GN_SPREAD:

```fides
pub const GN_SPREAD: i64 = 5;  // name, dirs
```

*Defined at line 333*

---

## GN_INLINE:

```fides
pub const GN_INLINE: i64 = 6;  // type_cond, dirs, sel
```

*Defined at line 334*

---

## GN_FRAGMENT:

```fides
pub const GN_FRAGMENT: i64 = 7;  // name, type_cond, dirs, sel
```

*Defined at line 335*

---

## GN_ARG:

```fides
pub const GN_ARG: i64 = 8;  // name, a = value
```

*Defined at line 336*

---

## GN_DIRECTIVE:

```fides
pub const GN_DIRECTIVE: i64 = 9;  // name, args
```

*Defined at line 337*

---

## GN_VAR:

```fides
pub const GN_VAR: i64 = 10;  // value: $name
```

*Defined at line 338*

---

## GN_INT:

```fides
pub const GN_INT: i64 = 11;
```

*Defined at line 339*

---

## GN_FLOAT:

```fides
pub const GN_FLOAT: i64 = 12;
```

*Defined at line 340*

---

## GN_STRING:

```fides
pub const GN_STRING: i64 = 13;
```

*Defined at line 341*

---

## GN_BOOL:

```fides
pub const GN_BOOL: i64 = 14;
```

*Defined at line 342*

---

## GN_NULL:

```fides
pub const GN_NULL: i64 = 15;
```

*Defined at line 343*

---

## GN_ENUM:

```fides
pub const GN_ENUM: i64 = 16;
```

*Defined at line 344*

---

## GN_LIST:

```fides
pub const GN_LIST: i64 = 17;  // children = values
```

*Defined at line 345*

---

## GN_OBJECT:

```fides
pub const GN_OBJECT: i64 = 18;  // children = object fields (GN_ARG)
```

*Defined at line 346*

---

## GqlNode

```fides
pub struct GqlNode {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `name` | `str` |
| `alias` | `str` |
| `op` | `str` |
| `type_ref` | `str` |
| `type_cond` | `str` |
| `value` | `str` |
| `a` | `i64` |
| `sel` | `i64` |
| `children` | `Vec<i64>` |
| `args` | `Vec<i64>` |
| `dirs` | `Vec<i64>` |

*Defined at line 348*

---

## GqlDoc

```fides
pub struct GqlDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `nodes` | `Vec<GqlNode>` |
| `root` | `i64` |

*Defined at line 363*

---

## gql_parse

Parse an executable document (operations and fragments).

```fides
pub fn gql_parse(src: str) -> GqlDoc {
```

**Parameters:**

- `src`: `str`

**Returns:** `GqlDoc`

*Defined at line 702*

---

## gql_operations

The operations' names and kinds, as "query Name" lines (anonymous
operations are "query").

```fides
pub fn gql_operations(d: &GqlDoc) -> Vec<str> {
```

**Parameters:**

- `d`: `&GqlDoc`

**Returns:** `Vec<str>`

*Defined at line 749*

---

## gql_value_json

A value node as JSON text; variables are read from `vars` (a JSON
object), a missing one is null.

```fides
pub fn gql_value_json(nodes: &Vec<GqlNode>, id: i64, vars: str) -> str {
```

**Parameters:**

- `nodes`: `&Vec<GqlNode>`
- `id`: `i64`
- `vars`: `str`

**Returns:** `str`

*Defined at line 773*

---

## GqlField

```fides
pub struct GqlField {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `type_ref` | `str` |
| `arg_names` | `Vec<str>` |
| `arg_types` | `Vec<str>` |
| `arg_defaults` | `Vec<str>,  // JSON, "" when none` |

*Defined at line 820*

---

## GqlType

kind: OBJECT INTERFACE UNION ENUM INPUT_OBJECT SCALAR

```fides
pub struct GqlType {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `str` |
| `name` | `str` |
| `fields` | `Vec<GqlField>` |
| `values` | `Vec<str>` |
| `members` | `Vec<str>` |
| `interfaces` | `Vec<str>` |

*Defined at line 829*

---

## GqlSchema

```fides
pub struct GqlSchema {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `types` | `Vec<GqlType>` |
| `query` | `str` |
| `mutation` | `str` |

*Defined at line 838*

---

## gql_parse_schema

Parse a schema in SDL. Without a `schema { }` block the roots are the
types named Query and Mutation.

```fides
pub fn gql_parse_schema(src: str) -> GqlSchema {
```

**Parameters:**

- `src`: `str`

**Returns:** `GqlSchema`

*Defined at line 902*

---

## gql_type_index

The type named `name`, or -1.

```fides
pub fn gql_type_index(s: &GqlSchema, name: str) -> i64 {
```

**Parameters:**

- `s`: `&GqlSchema`
- `name`: `str`

**Returns:** `i64`

*Defined at line 1040*

---

## gql_field_names

The field names of a type ([] when the type is unknown).

```fides
pub fn gql_field_names(s: &GqlSchema, type_name: str) -> Vec<str> {
```

**Parameters:**

- `s`: `&GqlSchema`
- `type_name`: `str`

**Returns:** `Vec<str>`

*Defined at line 1052*

---

## gql_execute

Execute `query` over `schema`: `variables` is a JSON object, `op_name`
picks the operation ("" for the only one), `resolve` the resolver.
Returns the response JSON: {"errors":[...],"data":...} or {"data":...}.

```fides
pub fn gql_execute(schema: &GqlSchema, query: str, variables: str, op_name: str, resolve: fn(str, str, str) -> str) -> str {
```

**Parameters:**

- `schema`: `&GqlSchema`
- `query`: `str`
- `variables`: `str`
- `op_name`: `str`
- `resolve`: `fn(str`

**Returns:** `str`

*Defined at line 1562*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
