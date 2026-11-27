[All modules](index.md)

# power -- Fides Documentation

## Contents

- [PllConfig](#PllConfig) (struct)
- [pll_check](#pll_check) (function)
- [pll_solve](#pll_solve) (function)
- [pll_cfgr](#pll_cfgr) (function)
- [ahb_prescaler_ok](#ahb_prescaler_ok) (function)
- [apb_prescaler_ok](#apb_prescaler_ok) (function)
- [BusClocks](#BusClocks) (struct)
- [flash_wait_states](#flash_wait_states) (function)
- [bus_clocks](#bus_clocks) (function)
- [apb_prescaler_for](#apb_prescaler_for) (function)
- [CLK_SOURCE:](#CLK_SOURCE:) (const)
- [CLK_MUL:](#CLK_MUL:) (const)
- [CLK_DIV:](#CLK_DIV:) (const)
- [CLK_MUX:](#CLK_MUX:) (const)
- [CLK_GATE:](#CLK_GATE:) (const)
- [ClkNode](#ClkNode) (struct)
- [ClockTree](#ClockTree) (struct)
- [clktree_new](#clktree_new) (function)
- [clk_source](#clk_source) (function)
- [clk_mul](#clk_mul) (function)
- [clk_div](#clk_div) (function)
- [clk_mux](#clk_mux) (function)
- [clk_gate](#clk_gate) (function)
- [clk_set_ratio](#clk_set_ratio) (function)
- [clk_set](#clk_set) (function)
- [clk_select](#clk_select) (function)
- [clk_freq](#clk_freq) (function)
- [clk_find](#clk_find) (function)
- [clk_check](#clk_check) (function)
- [PM_RUN:](#PM_RUN:) (const)
- [PM_SLEEP:](#PM_SLEEP:) (const)
- [PM_STOP:](#PM_STOP:) (const)
- [PM_STANDBY:](#PM_STANDBY:) (const)
- [WAKE_IRQ:](#WAKE_IRQ:) (const)
- [WAKE_EXTI:](#WAKE_EXTI:) (const)
- [WAKE_RTC:](#WAKE_RTC:) (const)
- [WAKE_PIN:](#WAKE_PIN:) (const)
- [WAKE_IWDG:](#WAKE_IWDG:) (const)
- [WAKE_RESET:](#WAKE_RESET:) (const)
- [pm_name](#pm_name) (function)
- [PowerSim](#PowerSim) (struct)
- [powersim_new](#powersim_new) (function)
- [power_enter](#power_enter) (function)
- [power_wake](#power_wake) (function)
- [power_restore_pll](#power_restore_pll) (function)
- [power_average_na](#power_average_na) (function)
- [power_battery_hours](#power_battery_hours) (function)

## PllConfig

```fides
pub struct PllConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `m` | `i64` |
| `n` | `i64` |
| `p` | `i64` |
| `q` | `i64` |
| `vco` | `i64` |
| `sys` | `i64` |
| `usb` | `i64` |

*Defined at line 38*

---

## pll_check

Check one configuration against the limits; "" when valid.

```fides
pub fn pll_check(fin: i64, m: i64, n: i64, p: i64, q: i64, sys_max: i64) -> str {
```

**Parameters:**

- `fin`: `i64`
- `m`: `i64`
- `n`: `i64`
- `p`: `i64`
- `q`: `i64`
- `sys_max`: `i64`

**Returns:** `str`

*Defined at line 56*

---

## pll_solve

Find (M, N, P, Q) giving exactly `target` Hz from `fin` Hz.

```fides
pub fn pll_solve(fin: i64, target: i64, sys_max: i64) -> PllConfig {
```

**Parameters:**

- `fin`: `i64`
- `target`: `i64`
- `sys_max`: `i64`

**Returns:** `PllConfig`

*Defined at line 87*

---

## pll_cfgr

RCC_PLLCFGR value (STM32F4): M[5:0], N[14:6], P[17:16] ((P/2)-1),
PLLSRC[22] (1 = HSE), Q[27:24].

```fides
pub fn pll_cfgr(c: PllConfig, hse: bool) -> i64 {
```

**Parameters:**

- `c`: `PllConfig`
- `hse`: `bool`

**Returns:** `i64`

*Defined at line 130*

---

## ahb_prescaler_ok

```fides
pub fn ahb_prescaler_ok(d: i64) -> bool {
```

**Parameters:**

- `d`: `i64`

**Returns:** `bool`

*Defined at line 140*

---

## apb_prescaler_ok

```fides
pub fn apb_prescaler_ok(d: i64) -> bool {
```

**Parameters:**

- `d`: `i64`

**Returns:** `bool`

*Defined at line 144*

---

## BusClocks

```fides
pub struct BusClocks {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `why` | `str` |
| `hclk` | `i64` |
| `pclk1` | `i64` |
| `pclk2` | `i64` |
| `tim1` | `i64` |
| `tim2` | `i64` |
| `wait_states` | `i64` |

*Defined at line 148*

---

## flash_wait_states

Flash wait states for HCLK at a voltage range (STM32F4 table 10):
30 MHz per wait state at 2.7-3.6 V, 24 at 2.4-2.7, 22 at 2.1-2.4,
20 at 1.8-2.1 (mv = supply in millivolts).

```fides
pub fn flash_wait_states(hclk: i64, mv: i64) -> i64 {
```

**Parameters:**

- `hclk`: `i64`
- `mv`: `i64`

**Returns:** `i64`

*Defined at line 162*

---

## bus_clocks

The bus clocks from SYSCLK and prescalers, checked against the limits
(HCLK <= sys_max, PCLK1 <= apb1_max, PCLK2 <= apb2_max).

```fides
pub fn bus_clocks(sysclk: i64, ahb: i64, apb1: i64, apb2: i64, sys_max: i64, apb1_max: i64, apb2_max: i64, mv: i64) -> BusClocks {
```

**Parameters:**

- `sysclk`: `i64`
- `ahb`: `i64`
- `apb1`: `i64`
- `apb2`: `i64`
- `sys_max`: `i64`
- `apb1_max`: `i64`
- `apb2_max`: `i64`
- `mv`: `i64`

**Returns:** `BusClocks`

*Defined at line 179*

---

## apb_prescaler_for

The smallest APB prescaler keeping PCLK <= max.

```fides
pub fn apb_prescaler_for(hclk: i64, max: i64) -> i64 {
```

**Parameters:**

- `hclk`: `i64`
- `max`: `i64`

**Returns:** `i64`

*Defined at line 214*

---

## CLK_SOURCE:

```fides
pub const CLK_SOURCE: i64 = 0;
```

*Defined at line 227*

---

## CLK_MUL:

```fides
pub const CLK_MUL: i64 = 1;
```

*Defined at line 228*

---

## CLK_DIV:

```fides
pub const CLK_DIV: i64 = 2;
```

*Defined at line 229*

---

## CLK_MUX:

```fides
pub const CLK_MUX: i64 = 3;
```

*Defined at line 230*

---

## CLK_GATE:

```fides
pub const CLK_GATE: i64 = 4;
```

*Defined at line 231*

---

## ClkNode

```fides
pub struct ClkNode {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `kind` | `i64` |
| `parents` | `Vec<i64>` |
| `sel` | `i64` |
| `num` | `i64` |
| `den` | `i64` |
| `hz` | `i64` |
| `on` | `bool` |
| `max_hz` | `i64` |

*Defined at line 233*

---

## ClockTree

```fides
pub struct ClockTree {
```

**Fields:**

| Name | Type |
|------|------|
| `nodes` | `Vec<ClkNode>` |

*Defined at line 245*

---

## clktree_new

```fides
pub fn clktree_new() -> ClockTree {
```

**Returns:** `ClockTree`

*Defined at line 249*

---

## clk_source

```fides
pub fn clk_source(t: &mut ClockTree, name: str, hz: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut ClockTree`
- `name`: `str`
- `hz`: `i64`

**Returns:** `i64`

*Defined at line 266*

---

## clk_mul

A node multiplying its parent by num/den (a PLL).

```fides
pub fn clk_mul(t: &mut ClockTree, name: str, parent: i64, num: i64, den: i64, max_hz: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut ClockTree`
- `name`: `str`
- `parent`: `i64`
- `num`: `i64`
- `den`: `i64`
- `max_hz`: `i64`

**Returns:** `i64`

*Defined at line 273*

---

## clk_div

```fides
pub fn clk_div(t: &mut ClockTree, name: str, parent: i64, div: i64, max_hz: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut ClockTree`
- `name`: `str`
- `parent`: `i64`
- `div`: `i64`
- `max_hz`: `i64`

**Returns:** `i64`

*Defined at line 282*

---

## clk_mux

```fides
pub fn clk_mux(t: &mut ClockTree, name: str, parents: Vec<i64>, sel: i64, max_hz: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut ClockTree`
- `name`: `str`
- `parents`: `Vec<i64>`
- `sel`: `i64`
- `max_hz`: `i64`

**Returns:** `i64`

*Defined at line 290*

---

## clk_gate

```fides
pub fn clk_gate(t: &mut ClockTree, name: str, parent: i64, on: bool) -> i64 {
```

**Parameters:**

- `t`: `&mut ClockTree`
- `name`: `str`
- `parent`: `i64`
- `on`: `bool`

**Returns:** `i64`

*Defined at line 298*

---

## clk_set_ratio

Change a multiplier's num/den or a divider's den.

```fides
pub fn clk_set_ratio(t: &mut ClockTree, id: i64, num: i64, den: i64) {
```

**Parameters:**

- `t`: `&mut ClockTree`
- `id`: `i64`
- `num`: `i64`
- `den`: `i64`

*Defined at line 306*

---

## clk_set

```fides
pub fn clk_set(t: &mut ClockTree, id: i64, on: bool) {
```

**Parameters:**

- `t`: `&mut ClockTree`
- `id`: `i64`
- `on`: `bool`

*Defined at line 313*

---

## clk_select

```fides
pub fn clk_select(t: &mut ClockTree, id: i64, sel: i64) {
```

**Parameters:**

- `t`: `&mut ClockTree`
- `id`: `i64`
- `sel`: `i64`

*Defined at line 319*

---

## clk_freq

Frequency of a node (0 when gated off or a source is off).

```fides
pub fn clk_freq(t: &ClockTree, id: i64) -> i64 {
```

**Parameters:**

- `t`: `&ClockTree`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 326*

---

## clk_find

```fides
pub fn clk_find(t: &ClockTree, name: str) -> i64 {
```

**Parameters:**

- `t`: `&ClockTree`
- `name`: `str`

**Returns:** `i64`

*Defined at line 347*

---

## clk_check

Every node above its maximum: "name: f > max".

```fides
pub fn clk_check(t: &ClockTree) -> Vec<str> {
```

**Parameters:**

- `t`: `&ClockTree`

**Returns:** `Vec<str>`

*Defined at line 359*

---

## PM_RUN:

```fides
pub const PM_RUN: i64 = 0;
```

*Defined at line 375*

---

## PM_SLEEP:

```fides
pub const PM_SLEEP: i64 = 1;
```

*Defined at line 376*

---

## PM_STOP:

```fides
pub const PM_STOP: i64 = 2;
```

*Defined at line 377*

---

## PM_STANDBY:

```fides
pub const PM_STANDBY: i64 = 3;
```

*Defined at line 378*

---

## WAKE_IRQ:

```fides
pub const WAKE_IRQ: i64 = 1;
```

*Defined at line 380*

---

## WAKE_EXTI:

```fides
pub const WAKE_EXTI: i64 = 2;
```

*Defined at line 381*

---

## WAKE_RTC:

```fides
pub const WAKE_RTC: i64 = 4;
```

*Defined at line 382*

---

## WAKE_PIN:

```fides
pub const WAKE_PIN: i64 = 8;
```

*Defined at line 383*

---

## WAKE_IWDG:

```fides
pub const WAKE_IWDG: i64 = 16;
```

*Defined at line 384*

---

## WAKE_RESET:

```fides
pub const WAKE_RESET: i64 = 32;
```

*Defined at line 385*

---

## pm_name

```fides
pub fn pm_name(m: i64) -> str {
```

**Parameters:**

- `m`: `i64`

**Returns:** `str`

*Defined at line 387*

---

## PowerSim

```fides
pub struct PowerSim {
```

**Fields:**

| Name | Type |
|------|------|
| `mode` | `i64` |
| `sysclk_src` | `str` |
| `ram_ok` | `bool` |
| `irq_pending` | `bool` |
| `exti_enabled` | `bool` |
| `wake_sources` | `i64` |
| `wuf` | `bool` |
| `resets` | `i64` |
| `log` | `Vec<str>` |

*Defined at line 392*

---

## powersim_new

```fides
pub fn powersim_new() -> PowerSim {
```

**Returns:** `PowerSim`

*Defined at line 404*

---

## power_enter

Try to enter a mode; false (with the reason logged) when the rules
forbid it or WFI returns at once.

```fides
pub fn power_enter(s: &mut PowerSim, m: i64) -> bool {
```

**Parameters:**

- `s`: `&mut PowerSim`
- `m`: `i64`

**Returns:** `bool`

*Defined at line 412*

---

## power_wake

A wakeup event; false when it does not wake the current mode.

```fides
pub fn power_wake(s: &mut PowerSim, src: i64) -> bool {
```

**Parameters:**

- `s`: `&mut PowerSim`
- `src`: `i64`

**Returns:** `bool`

*Defined at line 443*

---

## power_restore_pll

After waking from STOP: switch back to the PLL.

```fides
pub fn power_restore_pll(s: &mut PowerSim) {
```

**Parameters:**

- `s`: `&mut PowerSim`

*Defined at line 483*

---

## power_average_na

Average current in nA of a cycle spending `durations[i]` microseconds in
`modes[i]`, given the current in nA of each mode (index by mode).

```fides
pub fn power_average_na(modes: Vec<i64>, durations: Vec<i64>, current_na: Vec<i64>) -> i64 {
```

**Parameters:**

- `modes`: `Vec<i64>`
- `durations`: `Vec<i64>`
- `current_na`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 490*

---

## power_battery_hours

Hours a battery of `mah` lasts at an average current of `avg_na`.

```fides
pub fn power_battery_hours(mah: i64, avg_na: i64) -> i64 {
```

**Parameters:**

- `mah`: `i64`
- `avg_na`: `i64`

**Returns:** `i64`

*Defined at line 506*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
