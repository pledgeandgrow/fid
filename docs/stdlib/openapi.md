[All modules](index.md)

# openapi -- Fides Documentation

## Contents

- [ApiParam](#ApiParam) (struct)
- [ApiResponse](#ApiResponse) (struct)
- [ApiRoute](#ApiRoute) (struct)
- [ApiDoc](#ApiDoc) (struct)
- [ApiRequest](#ApiRequest) (struct)
- [openapi_new](#openapi_new) (function)
- [openapi_route](#openapi_route) (function)
- [openapi_param](#openapi_param) (function)
- [openapi_body](#openapi_body) (function)
- [openapi_response](#openapi_response) (function)
- [openapi_schema](#openapi_schema) (function)
- [openapi_to_json](#openapi_to_json) (function)
- [openapi_to_yaml](#openapi_to_yaml) (function)
- [json_to_yaml](#json_to_yaml) (function)
- [openapi_parse](#openapi_parse) (function)
- [openapi_request](#openapi_request) (function)
- [openapi_req_query](#openapi_req_query) (function)
- [openapi_req_header](#openapi_req_header) (function)
- [openapi_match_path](#openapi_match_path) (function)
- [openapi_find_route](#openapi_find_route) (function)
- [openapi_check_request](#openapi_check_request) (function)

## ApiParam

```fides
pub struct ApiParam {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `location` | `str,  // path | query | header | cookie` |
| `required` | `bool` |
| `schema` | `str,  // JSON Schema text` |
| `description` | `str` |

*Defined at line 34*

---

## ApiResponse

```fides
pub struct ApiResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `status` | `str,  // "200", "404", "default"` |
| `description` | `str` |
| `schema` | `str,  // "" when the response has no JSON body` |

*Defined at line 42*

---

## ApiRoute

```fides
pub struct ApiRoute {
```

**Fields:**

| Name | Type |
|------|------|
| `method` | `str,  // lowercase: get post put patch delete head options` |
| `path` | `str` |
| `operation_id` | `str` |
| `summary` | `str` |
| `tags` | `Vec<str>` |
| `params` | `Vec<ApiParam>` |
| `body_schema` | `str,  // "" when there is no request body` |
| `body_required` | `bool` |
| `responses` | `Vec<ApiResponse>` |

*Defined at line 48*

---

## ApiDoc

```fides
pub struct ApiDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `title` | `str` |
| `version` | `str` |
| `description` | `str` |
| `servers` | `Vec<str>` |
| `routes` | `Vec<ApiRoute>` |
| `schema_names` | `Vec<str>` |
| `schemas` | `Vec<str>` |

*Defined at line 60*

---

## ApiRequest

An incoming request, as data.

```fides
pub struct ApiRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `method` | `str` |
| `path` | `str` |
| `query_names` | `Vec<str>` |
| `query_vals` | `Vec<str>` |
| `header_names` | `Vec<str>` |
| `header_vals` | `Vec<str>` |
| `body` | `str,  // "" when absent` |

*Defined at line 71*

---

## openapi_new

```fides
pub fn openapi_new(title: str, version: str) -> ApiDoc {
```

**Parameters:**

- `title`: `str`
- `version`: `str`

**Returns:** `ApiDoc`

*Defined at line 83*

---

## openapi_route

```fides
pub fn openapi_route(method: str, path: str, operation_id: str) -> ApiRoute {
```

**Parameters:**

- `method`: `str`
- `path`: `str`
- `operation_id`: `str`

**Returns:** `ApiRoute`

*Defined at line 103*

---

## openapi_param

```fides
pub fn openapi_param(r: &mut ApiRoute, name: str, location: str, required: bool, schema: str) {
```

**Parameters:**

- `r`: `&mut ApiRoute`
- `name`: `str`
- `location`: `str`
- `required`: `bool`
- `schema`: `str`

*Defined at line 109*

---

## openapi_body

```fides
pub fn openapi_body(r: &mut ApiRoute, schema: str, required: bool) {
```

**Parameters:**

- `r`: `&mut ApiRoute`
- `schema`: `str`
- `required`: `bool`

*Defined at line 116*

---

## openapi_response

```fides
pub fn openapi_response(r: &mut ApiRoute, status: str, description: str, schema: str) {
```

**Parameters:**

- `r`: `&mut ApiRoute`
- `status`: `str`
- `description`: `str`
- `schema`: `str`

*Defined at line 121*

---

## openapi_schema

Add a shared schema under #/components/schemas/<name>.

```fides
pub fn openapi_schema(d: &mut ApiDoc, name: str, schema: str) {
```

**Parameters:**

- `d`: `&mut ApiDoc`
- `name`: `str`
- `schema`: `str`

*Defined at line 128*

---

## openapi_to_json

The document as canonical JSON text (keys sorted).

```fides
pub fn openapi_to_json(d: &ApiDoc) -> str {
```

**Parameters:**

- `d`: `&ApiDoc`

**Returns:** `str`

*Defined at line 199*

---

## openapi_to_yaml

The document as YAML (block style).

```fides
pub fn openapi_to_yaml(d: &ApiDoc) -> str {
```

**Parameters:**

- `d`: `&ApiDoc`

**Returns:** `str`

*Defined at line 348*

---

## json_to_yaml

Any JSON text as block-style YAML.

```fides
pub fn json_to_yaml(v: str) -> str {
```

**Parameters:**

- `v`: `str`

**Returns:** `str`

*Defined at line 353*

---

## openapi_parse

Read a JSON OpenAPI document back into the model. Unknown members are
ignored. An invalid document gives an empty model titled "".

```fides
pub fn openapi_parse(text: str) -> ApiDoc {
```

**Parameters:**

- `text`: `str`

**Returns:** `ApiDoc`

*Defined at line 372*

---

## openapi_request

```fides
pub fn openapi_request(method: str, path: str) -> ApiRequest {
```

**Parameters:**

- `method`: `str`
- `path`: `str`

**Returns:** `ApiRequest`

*Defined at line 454*

---

## openapi_req_query

```fides
pub fn openapi_req_query(r: &mut ApiRequest, name: str, value: str) {
```

**Parameters:**

- `r`: `&mut ApiRequest`
- `name`: `str`
- `value`: `str`

*Defined at line 460*

---

## openapi_req_header

```fides
pub fn openapi_req_header(r: &mut ApiRequest, name: str, value: str) {
```

**Parameters:**

- `r`: `&mut ApiRequest`
- `name`: `str`
- `value`: `str`

*Defined at line 465*

---

## openapi_match_path

Match a concrete path against a template; the captured parameters as
[name, value, name, value...], or ["!"] when it does not match.

```fides
pub fn openapi_match_path(template: str, path: str) -> Vec<str> {
```

**Parameters:**

- `template`: `str`
- `path`: `str`

**Returns:** `Vec<str>`

*Defined at line 472*

---

## openapi_find_route

The index of the route a request maps to, or -1. A literal segment
beats a template one: routes are tried with the fewest {} first.

```fides
pub fn openapi_find_route(d: &ApiDoc, method: str, path: str) -> i64 {
```

**Parameters:**

- `d`: `&ApiDoc`
- `method`: `str`
- `path`: `str`

**Returns:** `i64`

*Defined at line 504*

---

## openapi_check_request

Validate a request against the document: [] when it is acceptable,
otherwise one message per problem (the first says when no operation
matches).

```fides
pub fn openapi_check_request(d: &ApiDoc, req: &ApiRequest) -> Vec<str> {
```

**Parameters:**

- `d`: `&ApiDoc`
- `req`: `&ApiRequest`

**Returns:** `Vec<str>`

*Defined at line 591*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
