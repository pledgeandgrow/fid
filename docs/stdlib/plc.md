[All modules](index.md)

# plc -- Fides Documentation

## Contents

- [PLC_BOOL:](#PLC_BOOL:) (const)
- [PLC_INT:](#PLC_INT:) (const)
- [PLC_REAL:](#PLC_REAL:) (const)
- [PLC_TIME:](#PLC_TIME:) (const)
- [PLC_TON:](#PLC_TON:) (const)
- [PLC_TOF:](#PLC_TOF:) (const)
- [PLC_TP:](#PLC_TP:) (const)
- [PLC_CTU:](#PLC_CTU:) (const)
- [PLC_CTD:](#PLC_CTD:) (const)
- [PLC_RTRIG:](#PLC_RTRIG:) (const)
- [PLC_FTRIG:](#PLC_FTRIG:) (const)
- [PlcProgram](#PlcProgram) (struct)
- [PlcIo](#PlcIo) (struct)
- [plc_compile](#plc_compile) (function)
- [plc_io_new](#plc_io_new) (function)
- [plc_set_input](#plc_set_input) (function)
- [plc_get_output](#plc_get_output) (function)
- [plc_scan](#plc_scan) (function)
- [plc_get](#plc_get) (function)
- [plc_set](#plc_set) (function)
- [LAD_NO:](#LAD_NO:) (const)
- [LAD_NC:](#LAD_NC:) (const)
- [LAD_SERIES:](#LAD_SERIES:) (const)
- [LAD_PARALLEL:](#LAD_PARALLEL:) (const)
- [COIL_OUT:](#COIL_OUT:) (const)
- [COIL_NEG:](#COIL_NEG:) (const)
- [COIL_SET:](#COIL_SET:) (const)
- [COIL_RESET:](#COIL_RESET:) (const)
- [Ladder](#Ladder) (struct)
- [ladder_new](#ladder_new) (function)
- [lad_contact](#lad_contact) (function)
- [lad_series](#lad_series) (function)
- [lad_parallel](#lad_parallel) (function)
- [lad_rung](#lad_rung) (function)
- [lad_eval](#lad_eval) (function)
- [ladder_scan](#ladder_scan) (function)

## PLC_BOOL:

```fides
pub const PLC_BOOL: i64 = 0;
```

*Defined at line 41*

---

## PLC_INT:

```fides
pub const PLC_INT: i64 = 1;
```

*Defined at line 42*

---

## PLC_REAL:

```fides
pub const PLC_REAL: i64 = 2;
```

*Defined at line 43*

---

## PLC_TIME:

```fides
pub const PLC_TIME: i64 = 3;
```

*Defined at line 44*

---

## PLC_TON:

```fides
pub const PLC_TON: i64 = 10;
```

*Defined at line 45*

---

## PLC_TOF:

```fides
pub const PLC_TOF: i64 = 11;
```

*Defined at line 46*

---

## PLC_TP:

```fides
pub const PLC_TP: i64 = 12;
```

*Defined at line 47*

---

## PLC_CTU:

```fides
pub const PLC_CTU: i64 = 13;
```

*Defined at line 48*

---

## PLC_CTD:

```fides
pub const PLC_CTD: i64 = 14;
```

*Defined at line 49*

---

## PLC_RTRIG:

```fides
pub const PLC_RTRIG: i64 = 15;
```

*Defined at line 50*

---

## PLC_FTRIG:

```fides
pub const PLC_FTRIG: i64 = 16;
```

*Defined at line 51*

---

## PlcProgram

A compiled ST program with its variables and runtime state.

```fides
pub struct PlcProgram {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `error` | `str` |
| `var_names` | `Vec<str>` |
| `var_types` | `Vec<i64>` |
| `var_addr` | `Vec<str>` |
| `values` | `Vec<f64>` |
| `index` | `HashMap<str` |
| `kind` | `Vec<i64>` |
| `op` | `Vec<i64>` |
| `num` | `Vec<f64>` |
| `num2` | `Vec<f64>` |
| `ty` | `Vec<i64>` |
| `kid_start` | `Vec<i64>` |
| `kid_count` | `Vec<i64>` |
| `kids` | `Vec<i64>` |
| `body` | `i64` |
| `now_ms` | `f64` |
| `scans` | `i64` |
| `steps` | `i64` |
| `max_steps` | `i64` |

*Defined at line 98*

---

## PlcIo

Process images keyed by address without the '%' (e.g. "IX0.0").

```fides
pub struct PlcIo {
```

**Fields:**

| Name | Type |
|------|------|
| `inputs` | `HashMap<str` |
| `outputs` | `HashMap<str` |

*Defined at line 126*

---

## plc_compile

Compile ST source. On a syntax error the program has `error` set (with
the line) and runs nothing.

```fides
pub fn plc_compile(src: str) -> PlcProgram {
```

**Parameters:**

- `src`: `str`

**Returns:** `PlcProgram`

*Defined at line 1091*

---

## plc_io_new

```fides
pub fn plc_io_new() -> PlcIo {
```

**Returns:** `PlcIo`

*Defined at line 1586*

---

## plc_set_input

```fides
pub fn plc_set_input(io: &mut PlcIo, addr: str, v: f64) {
```

**Parameters:**

- `io`: `&mut PlcIo`
- `addr`: `str`
- `v`: `f64`

*Defined at line 1600*

---

## plc_get_output

Output image value (0 when never written).

```fides
pub fn plc_get_output(io: &PlcIo, addr: str) -> f64 {
```

**Parameters:**

- `io`: `&PlcIo`
- `addr`: `str`

**Returns:** `f64`

*Defined at line 1605*

---

## plc_scan

One scan at time `now_ms`: inputs -> program -> outputs. Returns false
when the program is stopped with an error.

```fides
pub fn plc_scan(prog: &mut PlcProgram, io: &mut PlcIo, now_ms: f64) -> bool {
```

**Parameters:**

- `prog`: `&mut PlcProgram`
- `io`: `&mut PlcIo`
- `now_ms`: `f64`

**Returns:** `bool`

*Defined at line 1616*

---

## plc_get

Value of a variable by name (case-insensitive, `inst.member` allowed).

```fides
pub fn plc_get(prog: &PlcProgram, name: str) -> f64 {
```

**Parameters:**

- `prog`: `&PlcProgram`
- `name`: `str`

**Returns:** `f64`

*Defined at line 1648*

---

## plc_set

```fides
pub fn plc_set(prog: &mut PlcProgram, name: str, v: f64) {
```

**Parameters:**

- `prog`: `&mut PlcProgram`
- `name`: `str`
- `v`: `f64`

*Defined at line 1656*

---

## LAD_NO:

```fides
pub const LAD_NO: i64 = 0;
```

*Defined at line 1665*

---

## LAD_NC:

```fides
pub const LAD_NC: i64 = 1;
```

*Defined at line 1666*

---

## LAD_SERIES:

```fides
pub const LAD_SERIES: i64 = 2;
```

*Defined at line 1667*

---

## LAD_PARALLEL:

```fides
pub const LAD_PARALLEL: i64 = 3;
```

*Defined at line 1668*

---

## COIL_OUT:

```fides
pub const COIL_OUT: i64 = 0;
```

*Defined at line 1670*

---

## COIL_NEG:

```fides
pub const COIL_NEG: i64 = 1;
```

*Defined at line 1671*

---

## COIL_SET:

```fides
pub const COIL_SET: i64 = 2;
```

*Defined at line 1672*

---

## COIL_RESET:

```fides
pub const COIL_RESET: i64 = 3;
```

*Defined at line 1673*

---

## Ladder

Ladder networks as an arena of elements; each rung is a network root
and its coils.

```fides
pub struct Ladder {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `Vec<i64>` |
| `addr` | `Vec<str>` |
| `kid_start` | `Vec<i64>` |
| `kid_count` | `Vec<i64>` |
| `kids` | `Vec<i64>` |
| `rung_net` | `Vec<i64>` |
| `coil_start` | `Vec<i64>` |
| `coil_count` | `Vec<i64>` |
| `coil_addr` | `Vec<str>` |
| `coil_kind` | `Vec<i64>` |

*Defined at line 1677*

---

## ladder_new

```fides
pub fn ladder_new() -> Ladder {
```

**Returns:** `Ladder`

*Defined at line 1690*

---

## lad_contact

A normally open (closed when `nc`) contact on a bit address.

```fides
pub fn lad_contact(l: &mut Ladder, addr: str, nc: bool) -> i64 {
```

**Parameters:**

- `l`: `&mut Ladder`
- `addr`: `str`
- `nc`: `bool`

**Returns:** `i64`

*Defined at line 1710*

---

## lad_series

```fides
pub fn lad_series(l: &mut Ladder, parts: Vec<i64>) -> i64 {
```

**Parameters:**

- `l`: `&mut Ladder`
- `parts`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 1718*

---

## lad_parallel

```fides
pub fn lad_parallel(l: &mut Ladder, parts: Vec<i64>) -> i64 {
```

**Parameters:**

- `l`: `&mut Ladder`
- `parts`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 1722*

---

## lad_rung

A rung: network `net` driving coils (parallel `addrs` and `kinds`).

```fides
pub fn lad_rung(l: &mut Ladder, net: i64, addrs: Vec<str>, kinds: Vec<i64>) {
```

**Parameters:**

- `l`: `&mut Ladder`
- `net`: `i64`
- `addrs`: `Vec<str>`
- `kinds`: `Vec<i64>`

*Defined at line 1727*

---

## lad_eval

Power flow through element e.

```fides
pub fn lad_eval(l: &Ladder, mem: &HashMap<str, f64>, e: i64) -> bool {
```

**Parameters:**

- `l`: `&Ladder`
- `mem`: `&HashMap<str`
- `e`: `i64`

**Returns:** `bool`

*Defined at line 1748*

---

## ladder_scan

Evaluate every rung in order over `mem` (bit address -> 0/1).

```fides
pub fn ladder_scan(l: &Ladder, mem: &mut HashMap<str, f64>) {
```

**Parameters:**

- `l`: `&Ladder`
- `mem`: `&mut HashMap<str`

*Defined at line 1777*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
