[All modules](index.md)

# compiler::diag -- Fides Documentation

## Contents

- [Severity](#Severity) (enum)
- [Severity](#Severity) (impl)
- [Severity_error](#Severity_error) (function)
- [Severity_warning](#Severity_warning) (function)
- [Severity_note](#Severity_note) (function)
- [Severity_help](#Severity_help) (function)
- [ErrCode](#ErrCode) (enum)
- [ErrCode](#ErrCode) (impl)
- [ErrCode_unterminated_string](#ErrCode_unterminated_string) (function)
- [ErrCode_unterminated_fstring](#ErrCode_unterminated_fstring) (function)
- [ErrCode_unknown_escape](#ErrCode_unknown_escape) (function)
- [ErrCode_unexpected_char](#ErrCode_unexpected_char) (function)
- [ErrCode_expected_token](#ErrCode_expected_token) (function)
- [ErrCode_expected_ident](#ErrCode_expected_ident) (function)
- [ErrCode_expected_type](#ErrCode_expected_type) (function)
- [ErrCode_invalid_assignment_target](#ErrCode_invalid_assignment_target) (function)
- [ErrCode_undefined_variable](#ErrCode_undefined_variable) (function)
- [ErrCode_undefined_function](#ErrCode_undefined_function) (function)
- [ErrCode_undefined_struct](#ErrCode_undefined_struct) (function)
- [ErrCode_undefined_field](#ErrCode_undefined_field) (function)
- [ErrCode_type_mismatch](#ErrCode_type_mismatch) (function)
- [ErrCode_if_condition_not_bool](#ErrCode_if_condition_not_bool) (function)
- [ErrCode_arg_count_mismatch](#ErrCode_arg_count_mismatch) (function)
- [ErrCode_missing_return](#ErrCode_missing_return) (function)
- [ErrCode_init_before_assign](#ErrCode_init_before_assign) (function)
- [ErrCode_not_linear](#ErrCode_not_linear) (function)
- [ErrCode_linear_consumed](#ErrCode_linear_consumed) (function)
- [ErrCode_linear_in_loop](#ErrCode_linear_in_loop) (function)
- [ErrCode_borrow_conflict](#ErrCode_borrow_conflict) (function)
- [ErrCode_mutable_borrow_conflict](#ErrCode_mutable_borrow_conflict) (function)
- [ErrCode_capability_denied](#ErrCode_capability_denied) (function)
- [ErrCode_sandbox_violation](#ErrCode_sandbox_violation) (function)
- [ErrCode_conservation_violation](#ErrCode_conservation_violation) (function)
- [ErrCode_gas_exceeded](#ErrCode_gas_exceeded) (function)
- [ErrCode_not_crashfree](#ErrCode_not_crashfree) (function)
- [ErrCode_contract_violation](#ErrCode_contract_violation) (function)
- [ErrCode_medical_violation](#ErrCode_medical_violation) (function)
- [ErrCode_effect_mismatch](#ErrCode_effect_mismatch) (function)
- [ErrCode_no_main](#ErrCode_no_main) (function)
- [ErrCode_internal](#ErrCode_internal) (function)
- [ErrCode_custom](#ErrCode_custom) (function)
- [Diag](#Diag) (struct)
- [Diag_error](#Diag_error) (function)
- [Diag_warning](#Diag_warning) (function)
- [Diag_with_source](#Diag_with_source) (function)
- [Diag_with_span](#Diag_with_span) (function)
- [Diag_with_help](#Diag_with_help) (function)
- [Diag_with_note](#Diag_with_note) (function)
- [Diag_error_full](#Diag_error_full) (function)
- [Diag_render](#Diag_render) (function)
- [DiagList](#DiagList) (struct)
- [DiagList_new](#DiagList_new) (function)
- [DiagList_push](#DiagList_push) (function)
- [DiagList_is_empty](#DiagList_is_empty) (function)
- [DiagList_len](#DiagList_len) (function)
- [DiagList_has_errors](#DiagList_has_errors) (function)
- [DiagList_render](#DiagList_render) (function)
- [diag_extract_line](#diag_extract_line) (function)
- [diag_error_with_source](#diag_error_with_source) (function)
- [diag_levenshtein](#diag_levenshtein) (function)
- [diag_suggest_did_you_mean](#diag_suggest_did_you_mean) (function)

## Severity

```fides
pub enum Severity {
```

**Variants:**

- `Error`
- `Warning`
- `Note`
- `Help`

*Defined at line 27*

---

## Severity

```fides
impl Severity {
```

**Methods:**

### label

```fides
pub fn label(self: Severity) -> str {
```

### color_code

```fides
pub fn color_code(self: Severity) -> str {
```

### is_error

```fides
pub fn is_error(self: Severity) -> bool {
```

*Defined at line 34*

---

## Severity_error

```fides
pub fn Severity_error() -> Severity {
```

**Returns:** `Severity`

*Defined at line 83*

---

## Severity_warning

```fides
pub fn Severity_warning() -> Severity {
```

**Returns:** `Severity`

*Defined at line 86*

---

## Severity_note

```fides
pub fn Severity_note() -> Severity {
```

**Returns:** `Severity`

*Defined at line 89*

---

## Severity_help

```fides
pub fn Severity_help() -> Severity {
```

**Returns:** `Severity`

*Defined at line 92*

---

## ErrCode

```fides
pub enum ErrCode {
```

**Variants:**

- `// Lexer errors`
- `UnterminatedString`
- `UnterminatedFString`
- `UnknownEscape`
- `UnexpectedChar`
- `// Parser errors`
- `ExpectedToken`
- `ExpectedIdent`
- `ExpectedType`
- `InvalidAssignmentTarget`
- `// Type errors`
- `UndefinedVariable`
- `UndefinedFunction`
- `UndefinedStruct`
- `UndefinedField`
- `TypeMismatch`
- `IfConditionNotBool`
- `ArgCountMismatch`
- `MissingReturn`
- `InitBeforeAssign`
- `NotLinear`
- `LinearConsumed`
- `LinearInLoop`
- `// Borrow checker`
- `BorrowConflict`
- `MutableBorrowConflict`
- `// Safety/contract errors`
- `CapabilityDenied`
- `SandboxViolation`
- `ConservationViolation`
- `GasExceeded`
- `NotCrashfree`
- `ContractViolation`
- `MedicalViolation`
- `// Effect errors`
- `EffectMismatch`
- `// Misc`
- `NoMain`
- `Internal`
- `Custom`

*Defined at line 100*

---

## ErrCode

```fides
impl ErrCode {
```

**Methods:**

### code

```fides
pub fn code(self: ErrCode) -> str {
```

*Defined at line 149*

---

## ErrCode_unterminated_string

```fides
pub fn ErrCode_unterminated_string() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 264*

---

## ErrCode_unterminated_fstring

```fides
pub fn ErrCode_unterminated_fstring() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 267*

---

## ErrCode_unknown_escape

```fides
pub fn ErrCode_unknown_escape() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 270*

---

## ErrCode_unexpected_char

```fides
pub fn ErrCode_unexpected_char() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 273*

---

## ErrCode_expected_token

```fides
pub fn ErrCode_expected_token() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 276*

---

## ErrCode_expected_ident

```fides
pub fn ErrCode_expected_ident() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 279*

---

## ErrCode_expected_type

```fides
pub fn ErrCode_expected_type() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 282*

---

## ErrCode_invalid_assignment_target

```fides
pub fn ErrCode_invalid_assignment_target() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 285*

---

## ErrCode_undefined_variable

```fides
pub fn ErrCode_undefined_variable() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 288*

---

## ErrCode_undefined_function

```fides
pub fn ErrCode_undefined_function() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 291*

---

## ErrCode_undefined_struct

```fides
pub fn ErrCode_undefined_struct() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 294*

---

## ErrCode_undefined_field

```fides
pub fn ErrCode_undefined_field() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 297*

---

## ErrCode_type_mismatch

```fides
pub fn ErrCode_type_mismatch() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 300*

---

## ErrCode_if_condition_not_bool

```fides
pub fn ErrCode_if_condition_not_bool() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 303*

---

## ErrCode_arg_count_mismatch

```fides
pub fn ErrCode_arg_count_mismatch() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 306*

---

## ErrCode_missing_return

```fides
pub fn ErrCode_missing_return() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 309*

---

## ErrCode_init_before_assign

```fides
pub fn ErrCode_init_before_assign() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 312*

---

## ErrCode_not_linear

```fides
pub fn ErrCode_not_linear() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 315*

---

## ErrCode_linear_consumed

```fides
pub fn ErrCode_linear_consumed() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 318*

---

## ErrCode_linear_in_loop

```fides
pub fn ErrCode_linear_in_loop() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 321*

---

## ErrCode_borrow_conflict

```fides
pub fn ErrCode_borrow_conflict() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 324*

---

## ErrCode_mutable_borrow_conflict

```fides
pub fn ErrCode_mutable_borrow_conflict() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 327*

---

## ErrCode_capability_denied

```fides
pub fn ErrCode_capability_denied() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 330*

---

## ErrCode_sandbox_violation

```fides
pub fn ErrCode_sandbox_violation() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 333*

---

## ErrCode_conservation_violation

```fides
pub fn ErrCode_conservation_violation() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 336*

---

## ErrCode_gas_exceeded

```fides
pub fn ErrCode_gas_exceeded() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 339*

---

## ErrCode_not_crashfree

```fides
pub fn ErrCode_not_crashfree() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 342*

---

## ErrCode_contract_violation

```fides
pub fn ErrCode_contract_violation() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 345*

---

## ErrCode_medical_violation

```fides
pub fn ErrCode_medical_violation() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 348*

---

## ErrCode_effect_mismatch

```fides
pub fn ErrCode_effect_mismatch() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 351*

---

## ErrCode_no_main

```fides
pub fn ErrCode_no_main() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 354*

---

## ErrCode_internal

```fides
pub fn ErrCode_internal() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 357*

---

## ErrCode_custom

```fides
pub fn ErrCode_custom() -> ErrCode {
```

**Returns:** `ErrCode`

*Defined at line 360*

---

## Diag

```fides
pub struct Diag {
```

**Fields:**

| Name | Type |
|------|------|
| `severity` | `Severity` |
| `code` | `ErrCode` |
| `message` | `str` |
| `file` | `str` |
| `line` | `i64` |
| `col` | `i64` |
| `source_line` | `str` |
| `span_len` | `i64` |
| `has_help` | `bool` |
| `help` | `str` |
| `notes` | `Vec` |

*Defined at line 372*

---

## Diag_error

Create a new error diagnostic.

```fides
pub fn Diag_error(code: ErrCode, msg: str, file: str, line: i64, col: i64) -> Diag {
```

**Parameters:**

- `code`: `ErrCode`
- `msg`: `str`
- `file`: `str`
- `line`: `i64`
- `col`: `i64`

**Returns:** `Diag`

*Defined at line 396*

---

## Diag_warning

Create a new warning diagnostic.

```fides
pub fn Diag_warning(code: ErrCode, msg: str, file: str, line: i64, col: i64) -> Diag {
```

**Parameters:**

- `code`: `ErrCode`
- `msg`: `str`
- `file`: `str`
- `line`: `i64`
- `col`: `i64`

**Returns:** `Diag`

*Defined at line 403*

---

## Diag_with_source

Set the source line text for context display.
Note: Due to an interpreter bug with struct str field returns,
use direct field assignment (d.source_line = line) in interpreter code.
This builder function works correctly with C codegen.

```fides
pub fn Diag_with_source(d: Diag, line: str) -> Diag {
```

**Parameters:**

- `d`: `Diag`
- `line`: `str`

**Returns:** `Diag`

*Defined at line 413*

---

## Diag_with_span

Set the span length (number of characters underlined).

```fides
pub fn Diag_with_span(d: Diag, len: i64) -> Diag {
```

**Parameters:**

- `d`: `Diag`
- `len`: `i64`

**Returns:** `Diag`

*Defined at line 421*

---

## Diag_with_help

Add a help suggestion.
Note: Due to an interpreter bug with struct str field returns,
use direct field assignment (d.has_help = true; d.help = help) in interpreter code.

```fides
pub fn Diag_with_help(d: Diag, help: str) -> Diag {
```

**Parameters:**

- `d`: `Diag`
- `help`: `str`

**Returns:** `Diag`

*Defined at line 434*

---

## Diag_with_note

Add a secondary note.

```fides
pub fn Diag_with_note(d: Diag, note: str) -> Diag {
```

**Parameters:**

- `d`: `Diag`
- `note`: `str`

**Returns:** `Diag`

*Defined at line 442*

---

## Diag_error_full

Create a fully-specified error diagnostic with all fields.
This avoids the builder pattern which has an interpreter bug with str fields.

```fides
pub fn Diag_error_full(code: ErrCode, msg: str, file: str, line: i64, col: i64, source_line: str, span_len: i64, help_text: str, has_help: bool,) -> Diag {
```

**Parameters:**

- `code`: `ErrCode`
- `msg`: `str`
- `file`: `str`
- `line`: `i64`
- `col`: `i64`
- `source_line`: `str`
- `span_len`: `i64`
- `help_text`: `str`
- `has_help`: `bool`

**Returns:** `Diag`

*Defined at line 450*

---

## Diag_render

Render the diagnostic as a formatted string.

If use_color is true, uses ANSI escape codes.
If source_line is non-empty, shows the source context with a caret.

```fides
pub fn Diag_render(d: Diag, use_color: bool) -> str {
```

**Parameters:**

- `d`: `Diag`
- `use_color`: `bool`

**Returns:** `str`

*Defined at line 486*

---

## DiagList

```fides
pub struct DiagList {
```

**Fields:**

| Name | Type |
|------|------|
| `rendered` | `Vec` |
| `error_count` | `i64` |

*Defined at line 607*

---

## DiagList_new

```fides
pub fn DiagList_new() -> DiagList {
```

**Returns:** `DiagList`

*Defined at line 614*

---

## DiagList_push

```fides
pub fn DiagList_push(dl: &mut DiagList, d: Diag, use_color: bool) -> void {
```

**Parameters:**

- `dl`: `&mut DiagList`
- `d`: `Diag`
- `use_color`: `bool`

**Returns:** `void`

*Defined at line 620*

---

## DiagList_is_empty

```fides
pub fn DiagList_is_empty(dl: DiagList) -> bool {
```

**Parameters:**

- `dl`: `DiagList`

**Returns:** `bool`

*Defined at line 627*

---

## DiagList_len

```fides
pub fn DiagList_len(dl: DiagList) -> i64 {
```

**Parameters:**

- `dl`: `DiagList`

**Returns:** `i64`

*Defined at line 631*

---

## DiagList_has_errors

```fides
pub fn DiagList_has_errors(dl: DiagList) -> bool {
```

**Parameters:**

- `dl`: `DiagList`

**Returns:** `bool`

*Defined at line 635*

---

## DiagList_render

Render all diagnostics as a single string.

```fides
pub fn DiagList_render(dl: DiagList) -> str {
```

**Parameters:**

- `dl`: `DiagList`

**Returns:** `str`

*Defined at line 640*

---

## diag_extract_line

Extract the source line at the given line number (1-based) from a source string.

```fides
pub fn diag_extract_line(source: str, line: i64) -> str {
```

**Parameters:**

- `source`: `str`
- `line`: `i64`

**Returns:** `str`

*Defined at line 654*

---

## diag_error_with_source

Create an error Diag with source context from a source string.
Extracts the source line at the given line number and sets the span.

```fides
pub fn diag_error_with_source(code: ErrCode, msg: str, file: str, line: i64, col: i64, source: str, span_len: i64,) -> Diag {
```

**Parameters:**

- `code`: `ErrCode`
- `msg`: `str`
- `file`: `str`
- `line`: `i64`
- `col`: `i64`
- `source`: `str`
- `span_len`: `i64`

**Returns:** `Diag`

*Defined at line 683*

---

## diag_levenshtein

Simple Levenshtein distance for "did you mean" suggestions.
Returns the edit distance between two strings.

```fides
pub fn diag_levenshtein(a: str, b: str) -> i64 {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `i64`

*Defined at line 699*

---

## diag_suggest_did_you_mean

Suggest the closest match from a list of candidates.
Returns a help string like "did you mean 'foo'?" if a close match is found.
Returns empty string if no close match (distance > 2).

```fides
pub fn diag_suggest_did_you_mean(input: str, candidates: Vec) -> str {
```

**Parameters:**

- `input`: `str`
- `candidates`: `Vec`

**Returns:** `str`

*Defined at line 760*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
