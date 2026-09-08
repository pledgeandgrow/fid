# kernel — Fides Documentation

## Contents

- [gdt_init_flat](#gdt_init_flat) (function)
- [gdt_load](#gdt_load) (function)
- [idt_set_handler](#idt_set_handler) (function)
- [idt_load](#idt_load) (function)
- [page_map](#page_map) (function)
- [page_map_flags](#page_map_flags) (function)
- [page_unmap](#page_unmap) (function)
- [page_identity_map_range](#page_identity_map_range) (function)
- [serial_init](#serial_init) (function)
- [serial_putc](#serial_putc) (function)
- [serial_puts](#serial_puts) (function)
- [serial_print](#serial_print) (function)
- [serial_print_int](#serial_print_int) (function)
- [serial_available](#serial_available) (function)
- [serial_getc](#serial_getc) (function)
- [cli](#cli) (function)
- [sti](#sti) (function)
- [hlt](#hlt) (function)
- [halt_forever](#halt_forever) (function)
- [inb](#inb) (function)
- [outb](#outb) (function)
- [phys_read64](#phys_read64) (function)
- [phys_write64](#phys_write64) (function)
- [cr0_read](#cr0_read) (function)
- [cr0_write](#cr0_write) (function)
- [cr3_read](#cr3_read) (function)
- [cr3_write](#cr3_write) (function)
- [cr4_read](#cr4_read) (function)
- [cr4_write](#cr4_write) (function)
- [invlpg](#invlpg) (function)

## gdt_init_flat

```fides
pub fn gdt_init_flat(gdt_ptr: i64) {
```

**Parameters:**

- `gdt_ptr`: `i64`

*Defined at line 31*

---

## gdt_load

```fides
pub fn gdt_load(gdt_ptr: i64, num_entries: i64) {
```

**Parameters:**

- `gdt_ptr`: `i64`
- `num_entries`: `i64`

*Defined at line 42*

---

## idt_set_handler

```fides
pub fn idt_set_handler(idt_ptr: i64, vector: i64, handler: i64) {
```

**Parameters:**

- `idt_ptr`: `i64`
- `vector`: `i64`
- `handler`: `i64`

*Defined at line 81*

---

## idt_load

```fides
pub fn idt_load(idt_ptr: i64, num_entries: i64) {
```

**Parameters:**

- `idt_ptr`: `i64`
- `num_entries`: `i64`

*Defined at line 87*

---

## page_map

```fides
pub fn page_map(pml4: i64, virt: i64, phys: i64) {
```

**Parameters:**

- `pml4`: `i64`
- `virt`: `i64`
- `phys`: `i64`

*Defined at line 118*

---

## page_map_flags

```fides
pub fn page_map_flags(pml4: i64, virt: i64, phys: i64, flags: i64) {
```

**Parameters:**

- `pml4`: `i64`
- `virt`: `i64`
- `phys`: `i64`
- `flags`: `i64`

*Defined at line 123*

---

## page_unmap

```fides
pub fn page_unmap(pml4: i64, virt: i64) {
```

**Parameters:**

- `pml4`: `i64`
- `virt`: `i64`

*Defined at line 128*

---

## page_identity_map_range

```fides
pub fn page_identity_map_range(pml4: i64, start: i64, num_pages: i64) {
```

**Parameters:**

- `pml4`: `i64`
- `start`: `i64`
- `num_pages`: `i64`

*Defined at line 134*

---

## serial_init

```fides
pub fn serial_init() {
```

*Defined at line 148*

---

## serial_putc

```fides
pub fn serial_putc(c: i64) {
```

**Parameters:**

- `c`: `i64`

*Defined at line 153*

---

## serial_puts

```fides
pub fn serial_puts(str_ptr: i64) {
```

**Parameters:**

- `str_ptr`: `i64`

*Defined at line 159*

---

## serial_print

```fides
pub fn serial_print(s: str) {
```

**Parameters:**

- `s`: `str`

*Defined at line 172*

---

## serial_print_int

```fides
pub fn serial_print_int(n: i64) {
```

**Parameters:**

- `n`: `i64`

*Defined at line 183*

---

## serial_available

```fides
pub fn serial_available() -> bool {
```

**Returns:** `bool`

*Defined at line 208*

---

## serial_getc

```fides
pub fn serial_getc() -> i64 {
```

**Returns:** `i64`

*Defined at line 213*

---

## cli

```fides
pub fn cli() {
```

*Defined at line 220*

---

## sti

```fides
pub fn sti() {
```

*Defined at line 225*

---

## hlt

```fides
pub fn hlt() {
```

*Defined at line 230*

---

## halt_forever

```fides
pub fn halt_forever() {
```

*Defined at line 235*

---

## inb

```fides
pub fn inb(port: i64) -> i64 {
```

**Parameters:**

- `port`: `i64`

**Returns:** `i64`

*Defined at line 244*

---

## outb

```fides
pub fn outb(port: i64, val: i64) {
```

**Parameters:**

- `port`: `i64`
- `val`: `i64`

*Defined at line 249*

---

## phys_read64

```fides
pub fn phys_read64(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 256*

---

## phys_write64

```fides
pub fn phys_write64(addr: i64, val: i64) {
```

**Parameters:**

- `addr`: `i64`
- `val`: `i64`

*Defined at line 261*

---

## cr0_read

```fides
pub fn cr0_read() -> i64 { return p_cr0_read(); }
```

**Returns:** `i64`

*Defined at line 267*

---

## cr0_write

```fides
pub fn cr0_write(val: i64) { p_cr0_write(val); }
```

**Parameters:**

- `val`: `i64`

*Defined at line 268*

---

## cr3_read

```fides
pub fn cr3_read() -> i64 { return p_cr3_read(); }
```

**Returns:** `i64`

*Defined at line 269*

---

## cr3_write

```fides
pub fn cr3_write(val: i64) { p_cr3_write(val); }
```

**Parameters:**

- `val`: `i64`

*Defined at line 270*

---

## cr4_read

```fides
pub fn cr4_read() -> i64 { return p_cr4_read(); }
```

**Returns:** `i64`

*Defined at line 271*

---

## cr4_write

```fides
pub fn cr4_write(val: i64) { p_cr4_write(val); }
```

**Parameters:**

- `val`: `i64`

*Defined at line 272*

---

## invlpg

```fides
pub fn invlpg(addr: i64) { p_invlpg(addr); }
```

**Parameters:**

- `addr`: `i64`

*Defined at line 273*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
