[All modules](index.md)

# save_game -- Fides Documentation

## Contents

- [SAVE_HEADER_LEN:](#SAVE_HEADER_LEN:) (const)
- [SAVE_CONTAINER:](#SAVE_CONTAINER:) (const)
- [save_crc32_update](#save_crc32_update) (function)
- [save_crc32](#save_crc32) (function)
- [SaveWriter](#SaveWriter) (struct)
- [save_writer](#save_writer) (function)
- [save_put_u8](#save_put_u8) (function)
- [save_put_bool](#save_put_bool) (function)
- [save_put_u32](#save_put_u32) (function)
- [save_put_i64](#save_put_i64) (function)
- [save_put_f64](#save_put_f64) (function)
- [save_put_str](#save_put_str) (function)
- [SaveReader](#SaveReader) (struct)
- [save_reader](#save_reader) (function)
- [save_get_u8](#save_get_u8) (function)
- [save_get_bool](#save_get_bool) (function)
- [save_get_u32](#save_get_u32) (function)
- [save_get_i64](#save_get_i64) (function)
- [save_get_f64](#save_get_f64) (function)
- [save_get_str](#save_get_str) (function)
- [save_reader_done](#save_reader_done) (function)
- [SaveGame](#SaveGame) (struct)
- [save_game_new](#save_game_new) (function)
- [save_game_find](#save_game_find) (function)
- [save_game_set](#save_game_set) (function)
- [save_game_get](#save_game_get) (function)
- [save_game_has](#save_game_has) (function)
- [save_game_remove](#save_game_remove) (function)
- [save_game_reader](#save_game_reader) (function)
- [save_game_encode](#save_game_encode) (function)
- [SaveLoad](#SaveLoad) (struct)
- [save_game_decode](#save_game_decode) (function)
- [SaveMigrate](#SaveMigrate) (struct)
- [save_game_migrate](#save_game_migrate) (function)
- [save_game_write_file](#save_game_write_file) (function)
- [save_game_read_file](#save_game_read_file) (function)

## SAVE_HEADER_LEN:

```fides
pub const SAVE_HEADER_LEN: i64 = 24;
```

*Defined at line 45*

---

## SAVE_CONTAINER:

```fides
pub const SAVE_CONTAINER: i64 = 1;
```

*Defined at line 46*

---

## save_crc32_update

Continue a CRC-32 over bytes[from..to); start with crc = 0.

```fides
pub fn save_crc32_update(crc: i64, bytes: &Vec<i64>, from: i64, to: i64) -> i64 {
```

**Parameters:**

- `crc`: `i64`
- `bytes`: `&Vec<i64>`
- `from`: `i64`
- `to`: `i64`

**Returns:** `i64`

*Defined at line 74*

---

## save_crc32

CRC-32 (IEEE, reflected, as zlib/PNG/ZIP) of all of `bytes`.

```fides
pub fn save_crc32(bytes: &Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 86*

---

## SaveWriter

```fides
pub struct SaveWriter {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec<i64>` |

*Defined at line 92*

---

## save_writer

```fides
pub fn save_writer() -> SaveWriter {
```

**Returns:** `SaveWriter`

*Defined at line 96*

---

## save_put_u8

```fides
pub fn save_put_u8(w: &mut SaveWriter, v: i64) {
```

**Parameters:**

- `w`: `&mut SaveWriter`
- `v`: `i64`

*Defined at line 102*

---

## save_put_bool

```fides
pub fn save_put_bool(w: &mut SaveWriter, v: bool) {
```

**Parameters:**

- `w`: `&mut SaveWriter`
- `v`: `bool`

*Defined at line 106*

---

## save_put_u32

```fides
pub fn save_put_u32(w: &mut SaveWriter, v: i64) {
```

**Parameters:**

- `w`: `&mut SaveWriter`
- `v`: `i64`

*Defined at line 114*

---

## save_put_i64

```fides
pub fn save_put_i64(w: &mut SaveWriter, v: i64) {
```

**Parameters:**

- `w`: `&mut SaveWriter`
- `v`: `i64`

*Defined at line 122*

---

## save_put_f64

```fides
pub fn save_put_f64(w: &mut SaveWriter, v: f64) {
```

**Parameters:**

- `w`: `&mut SaveWriter`
- `v`: `f64`

*Defined at line 130*

---

## save_put_str

```fides
pub fn save_put_str(w: &mut SaveWriter, s: str) {
```

**Parameters:**

- `w`: `&mut SaveWriter`
- `s`: `str`

*Defined at line 134*

---

## SaveReader

Reads typed values back in order; `ok` turns false (and stays) when a
read runs past the end.

```fides
pub struct SaveReader {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec<i64>` |
| `pos` | `i64` |
| `ok` | `bool` |

*Defined at line 146*

---

## save_reader

```fides
pub fn save_reader(bytes: Vec<i64>) -> SaveReader {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `SaveReader`

*Defined at line 152*

---

## save_get_u8

```fides
pub fn save_get_u8(r: &mut SaveReader) -> i64 {
```

**Parameters:**

- `r`: `&mut SaveReader`

**Returns:** `i64`

*Defined at line 166*

---

## save_get_bool

```fides
pub fn save_get_bool(r: &mut SaveReader) -> bool {
```

**Parameters:**

- `r`: `&mut SaveReader`

**Returns:** `bool`

*Defined at line 174*

---

## save_get_u32

```fides
pub fn save_get_u32(r: &mut SaveReader) -> i64 {
```

**Parameters:**

- `r`: `&mut SaveReader`

**Returns:** `i64`

*Defined at line 178*

---

## save_get_i64

```fides
pub fn save_get_i64(r: &mut SaveReader) -> i64 {
```

**Parameters:**

- `r`: `&mut SaveReader`

**Returns:** `i64`

*Defined at line 188*

---

## save_get_f64

```fides
pub fn save_get_f64(r: &mut SaveReader) -> f64 {
```

**Parameters:**

- `r`: `&mut SaveReader`

**Returns:** `f64`

*Defined at line 202*

---

## save_get_str

```fides
pub fn save_get_str(r: &mut SaveReader) -> str {
```

**Parameters:**

- `r`: `&mut SaveReader`

**Returns:** `str`

*Defined at line 206*

---

## save_reader_done

Every byte has been read.

```fides
pub fn save_reader_done(r: &SaveReader) -> bool {
```

**Parameters:**

- `r`: `&SaveReader`

**Returns:** `bool`

*Defined at line 222*

---

## SaveGame

```fides
pub struct SaveGame {
```

**Fields:**

| Name | Type |
|------|------|
| `version` | `i64` |
| `names` | `Vec<str>` |
| `data` | `Vec<Vec<i64>>` |

*Defined at line 228*

---

## save_game_new

```fides
pub fn save_game_new(version: i64) -> SaveGame {
```

**Parameters:**

- `version`: `i64`

**Returns:** `SaveGame`

*Defined at line 234*

---

## save_game_find

Index of section `name`, -1 when absent.

```fides
pub fn save_game_find(sg: &SaveGame, name: str) -> i64 {
```

**Parameters:**

- `sg`: `&SaveGame`
- `name`: `str`

**Returns:** `i64`

*Defined at line 241*

---

## save_game_set

Set (add or replace) section `name`.

```fides
pub fn save_game_set(sg: &mut SaveGame, name: str, bytes: Vec<i64>) {
```

**Parameters:**

- `sg`: `&mut SaveGame`
- `name`: `str`
- `bytes`: `Vec<i64>`

*Defined at line 253*

---

## save_game_get

Section `name` (empty when absent).

```fides
pub fn save_game_get(sg: &SaveGame, name: str) -> Vec<i64> {
```

**Parameters:**

- `sg`: `&SaveGame`
- `name`: `str`

**Returns:** `Vec<i64>`

*Defined at line 264*

---

## save_game_has

```fides
pub fn save_game_has(sg: &SaveGame, name: str) -> bool {
```

**Parameters:**

- `sg`: `&SaveGame`
- `name`: `str`

**Returns:** `bool`

*Defined at line 272*

---

## save_game_remove

Remove section `name` (no-op when absent).

```fides
pub fn save_game_remove(sg: &mut SaveGame, name: str) {
```

**Parameters:**

- `sg`: `&mut SaveGame`
- `name`: `str`

*Defined at line 277*

---

## save_game_reader

A reader over section `name`.

```fides
pub fn save_game_reader(sg: &SaveGame, name: str) -> SaveReader {
```

**Parameters:**

- `sg`: `&SaveGame`
- `name`: `str`

**Returns:** `SaveReader`

*Defined at line 286*

---

## save_game_encode

The file bytes of `sg`.

```fides
pub fn save_game_encode(sg: &SaveGame) -> Vec<i64> {
```

**Parameters:**

- `sg`: `&SaveGame`

**Returns:** `Vec<i64>`

*Defined at line 311*

---

## SaveLoad

```fides
pub struct SaveLoad {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `save` | `SaveGame` |
| `from_backup` | `bool,  // save_game_read_file used path.bak` |

*Defined at line 352*

---

## save_game_decode

Parse save bytes: magic, container format, length, CRC, sections.

```fides
pub fn save_game_decode(b: &Vec<i64>) -> SaveLoad {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `SaveLoad`

*Defined at line 366*

---

## SaveMigrate

```fides
pub struct SaveMigrate {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `save` | `SaveGame` |
| `steps` | `i64` |

*Defined at line 432*

---

## save_game_migrate

Bring `sg` up to `target`: `step(&mut save)` is called with a save at
version N and must change it into version N + 1 (setting `version`),
returning false if it cannot. A save newer than `target`, a failing
step, or one that does not advance the version by exactly one, is an
error. (The step works in place: a struct passed by value through a
function value is not supported by every Fides back end.)

```fides
pub fn save_game_migrate(sg: &SaveGame, target: i64, step: fn(&mut SaveGame) -> bool) -> SaveMigrate {
```

**Parameters:**

- `sg`: `&SaveGame`
- `target`: `i64`
- `step`: `fn(&mut SaveGame) -> bool`

**Returns:** `SaveMigrate`

*Defined at line 445*

---

## save_game_write_file

Write `sg` to `path` crash-safely (temp file, verify, backup, rename).
False when any step fails; the old file is then still in place (or in
`path.bak`).

```fides
pub fn save_game_write_file(path: str, sg: &SaveGame) -> bool {
```

**Parameters:**

- `path`: `str`
- `sg`: `&SaveGame`

**Returns:** `bool`

*Defined at line 498*

---

## save_game_read_file

Read and check `path`; when it is missing or damaged, try `path.bak`
(left by a write that was interrupted).

```fides
pub fn save_game_read_file(path: str) -> SaveLoad {
```

**Parameters:**

- `path`: `str`

**Returns:** `SaveLoad`

*Defined at line 534*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
