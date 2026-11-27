[All modules](index.md)

# watchdog -- Fides Documentation

## Contents

- [IWDG_KR:](#IWDG_KR:) (const)
- [IWDG_PR:](#IWDG_PR:) (const)
- [IWDG_RLR:](#IWDG_RLR:) (const)
- [IWDG_SR:](#IWDG_SR:) (const)
- [IWDG_WINR:](#IWDG_WINR:) (const)
- [IWDG_KEY_START:](#IWDG_KEY_START:) (const)
- [IWDG_KEY_UNLOCK:](#IWDG_KEY_UNLOCK:) (const)
- [IWDG_KEY_RELOAD:](#IWDG_KEY_RELOAD:) (const)
- [IWDG_RL_MAX:](#IWDG_RL_MAX:) (const)
- [iwdg_divider](#iwdg_divider) (function)
- [iwdg_timeout_us](#iwdg_timeout_us) (function)
- [IwdgConfig](#IwdgConfig) (struct)
- [iwdg_config_for](#iwdg_config_for) (function)
- [iwdg_timeout_range](#iwdg_timeout_range) (function)
- [iwdg_write_sequence](#iwdg_write_sequence) (function)
- [iwdg_seq_to_str](#iwdg_seq_to_str) (function)
- [wwdg_tick_ns](#wwdg_tick_ns) (function)
- [wwdg_timeout_ns](#wwdg_timeout_ns) (function)
- [wwdg_window_ns](#wwdg_window_ns) (function)
- [WwdgSim](#WwdgSim) (struct)
- [wwdgsim_new](#wwdgsim_new) (function)
- [wwdgsim_tick](#wwdgsim_tick) (function)
- [wwdgsim_refresh](#wwdgsim_refresh) (function)
- [WdtSim](#WdtSim) (struct)
- [wdtsim_new](#wdtsim_new) (function)
- [wdtsim_advance](#wdtsim_advance) (function)
- [wdtsim_kick](#wdtsim_kick) (function)
- [KickReport](#KickReport) (struct)
- [wdg_check_kicks](#wdg_check_kicks) (function)
- [wdg_kick_period](#wdg_kick_period) (function)

## IWDG_KR:

```fides
pub const IWDG_KR: i64 = 0x00;
```

*Defined at line 30*

---

## IWDG_PR:

```fides
pub const IWDG_PR: i64 = 0x04;
```

*Defined at line 31*

---

## IWDG_RLR:

```fides
pub const IWDG_RLR: i64 = 0x08;
```

*Defined at line 32*

---

## IWDG_SR:

```fides
pub const IWDG_SR: i64 = 0x0C;
```

*Defined at line 33*

---

## IWDG_WINR:

```fides
pub const IWDG_WINR: i64 = 0x10;
```

*Defined at line 34*

---

## IWDG_KEY_START:

```fides
pub const IWDG_KEY_START: i64 = 0xCCCC;
```

*Defined at line 35*

---

## IWDG_KEY_UNLOCK:

```fides
pub const IWDG_KEY_UNLOCK: i64 = 0x5555;
```

*Defined at line 36*

---

## IWDG_KEY_RELOAD:

```fides
pub const IWDG_KEY_RELOAD: i64 = 0xAAAA;
```

*Defined at line 37*

---

## IWDG_RL_MAX:

```fides
pub const IWDG_RL_MAX: i64 = 0xFFF;
```

*Defined at line 38*

---

## iwdg_divider

```fides
pub fn iwdg_divider(pr: i64) -> i64 {
```

**Parameters:**

- `pr`: `i64`

**Returns:** `i64`

*Defined at line 40*

---

## iwdg_timeout_us

Timeout in microseconds for PR, RL at an LSI of `lsi_hz`.

```fides
pub fn iwdg_timeout_us(pr: i64, rl: i64, lsi_hz: i64) -> i64 {
```

**Parameters:**

- `pr`: `i64`
- `rl`: `i64`
- `lsi_hz`: `i64`

**Returns:** `i64`

*Defined at line 48*

---

## IwdgConfig

A configuration: prescaler code, reload, and the timeout it gives.

```fides
pub struct IwdgConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `pr` | `i64` |
| `rl` | `i64` |
| `timeout_us` | `i64` |

*Defined at line 53*

---

## iwdg_config_for

The smallest prescaler that reaches `timeout_ms` (the finest reload
step), reload rounded to the nearest count. ok = false when even PR 6
with the full reload is too short or 1 count too long.

```fides
pub fn iwdg_config_for(timeout_ms: i64, lsi_hz: i64) -> IwdgConfig {
```

**Parameters:**

- `timeout_ms`: `i64`
- `lsi_hz`: `i64`

**Returns:** `IwdgConfig`

*Defined at line 63*

---

## iwdg_timeout_range

Timeout range in microseconds over an LSI tolerance [lo_hz, hi_hz]:
[shortest (fastest clock), longest (slowest clock)].

```fides
pub fn iwdg_timeout_range(pr: i64, rl: i64, lo_hz: i64, hi_hz: i64) -> Vec<i64> {
```

**Parameters:**

- `pr`: `i64`
- `rl`: `i64`
- `lo_hz`: `i64`
- `hi_hz`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 83*

---

## iwdg_write_sequence

The register writes that start and configure the IWDG, as offset * 65536
+ value: start (the LSI starts too), unlock PR/RLR, PR, RLR, (WINR when
windowed: writing it also reloads), wait for SR == 0 (marked offset
0x0C, value 0 = "poll until"), then the first reload.

```fides
pub fn iwdg_write_sequence(c: IwdgConfig, window: i64) -> Vec<i64> {
```

**Parameters:**

- `c`: `IwdgConfig`
- `window`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 94*

---

## iwdg_seq_to_str

```fides
pub fn iwdg_seq_to_str(s: Vec<i64>) -> str {
```

**Parameters:**

- `s`: `Vec<i64>`

**Returns:** `str`

*Defined at line 109*

---

## wwdg_tick_ns

One WWDG counter tick in nanoseconds: 4096 * 2^WDGTB / PCLK.

```fides
pub fn wwdg_tick_ns(pclk_hz: i64, wdgtb: i64) -> i64 {
```

**Parameters:**

- `pclk_hz`: `i64`
- `wdgtb`: `i64`

**Returns:** `i64`

*Defined at line 155*

---

## wwdg_timeout_ns

Time from a refresh to T (0x40..0x7F) until the reset: T - 0x3F ticks.

```fides
pub fn wwdg_timeout_ns(pclk_hz: i64, wdgtb: i64, t: i64) -> i64 {
```

**Parameters:**

- `pclk_hz`: `i64`
- `wdgtb`: `i64`
- `t`: `i64`

**Returns:** `i64`

*Defined at line 160*

---

## wwdg_window_ns

The refresh window after reloading to T with window W: refreshing is
allowed once the counter is <= W, i.e. from (T - W) ticks, and must
happen before T - 0x3F ticks. [open_ns, close_ns].

```fides
pub fn wwdg_window_ns(pclk_hz: i64, wdgtb: i64, t: i64, w: i64) -> Vec<i64> {
```

**Parameters:**

- `pclk_hz`: `i64`
- `wdgtb`: `i64`
- `t`: `i64`
- `w`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 167*

---

## WwdgSim

WWDG state, simulated tick by tick.

```fides
pub struct WwdgSim {
```

**Fields:**

| Name | Type |
|------|------|
| `counter` | `i64` |
| `window` | `i64` |
| `reset` | `bool` |
| `reason` | `str` |
| `ticks` | `i64` |
| `ewi` | `bool` |

*Defined at line 180*

---

## wwdgsim_new

```fides
pub fn wwdgsim_new(t: i64, w: i64) -> WwdgSim {
```

**Parameters:**

- `t`: `i64`
- `w`: `i64`

**Returns:** `WwdgSim`

*Defined at line 189*

---

## wwdgsim_tick

One counter tick. The early wakeup flag rises at 0x40; going below it
resets.

```fides
pub fn wwdgsim_tick(s: &mut WwdgSim) {
```

**Parameters:**

- `s`: `&mut WwdgSim`

*Defined at line 200*

---

## wwdgsim_refresh

Refresh to `t`. Refreshing while the counter is above the window value
resets.

```fides
pub fn wwdgsim_refresh(s: &mut WwdgSim, t: i64) {
```

**Parameters:**

- `s`: `&mut WwdgSim`
- `t`: `i64`

*Defined at line 217*

---

## WdtSim

A down-counter reloaded by kicks. A kick sooner than `min_gap` ticks
after the previous one is refused with a reset (closed window); no kick
for `timeout` ticks resets.

```fides
pub struct WdtSim {
```

**Fields:**

| Name | Type |
|------|------|
| `timeout` | `i64` |
| `min_gap` | `i64` |
| `last_kick` | `i64` |
| `now` | `i64` |
| `reset_at` | `i64` |
| `reason` | `str` |
| `kicks` | `i64` |

*Defined at line 236*

---

## wdtsim_new

```fides
pub fn wdtsim_new(timeout: i64, min_gap: i64) -> WdtSim {
```

**Parameters:**

- `timeout`: `i64`
- `min_gap`: `i64`

**Returns:** `WdtSim`

*Defined at line 246*

---

## wdtsim_advance

Advance time to `t` (no kick in between).

```fides
pub fn wdtsim_advance(w: &mut WdtSim, t: i64) {
```

**Parameters:**

- `w`: `&mut WdtSim`
- `t`: `i64`

*Defined at line 253*

---

## wdtsim_kick

A kick at time `t`.

```fides
pub fn wdtsim_kick(w: &mut WdtSim, t: i64) {
```

**Parameters:**

- `w`: `&mut WdtSim`
- `t`: `i64`

*Defined at line 267*

---

## KickReport

```fides
pub struct KickReport {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `violations` | `Vec<str>` |
| `max_gap` | `i64` |
| `min_gap` | `i64` |
| `margin_pct` | `i64` |

*Defined at line 283*

---

## wdg_check_kicks

Check a log of kick times (sorted, the watchdog started at `start`)
against `timeout` and an optional window `min_gap` (0: none), up to
`end`. Each gap >= timeout is a reset ("late"), each gap < min_gap a
reset ("early"). margin_pct: how much of the timeout the worst gap
left unused (100 * (timeout - max_gap) / timeout).

```fides
pub fn wdg_check_kicks(kicks: Vec<i64>, start: i64, end: i64, timeout: i64, min_gap: i64) -> KickReport {
```

**Parameters:**

- `kicks`: `Vec<i64>`
- `start`: `i64`
- `end`: `i64`
- `timeout`: `i64`
- `min_gap`: `i64`

**Returns:** `KickReport`

*Defined at line 296*

---

## wdg_kick_period

Kick period that keeps `margin_pct` of the timeout in reserve and
stays inside the window: midpoint of [min_gap, timeout * (100 -
margin) / 100], or -1 when the window leaves no room.

```fides
pub fn wdg_kick_period(timeout: i64, min_gap: i64, margin_pct: i64) -> i64 {
```

**Parameters:**

- `timeout`: `i64`
- `min_gap`: `i64`
- `margin_pct`: `i64`

**Returns:** `i64`

*Defined at line 332*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
