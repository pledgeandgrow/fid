[All modules](index.md)

# banking_batch -- Fides Documentation

## Contents

- [BATCH_DEPOSIT:](#BATCH_DEPOSIT:) (const)
- [BATCH_WITHDRAW:](#BATCH_WITHDRAW:) (const)
- [BATCH_TRANSFER:](#BATCH_TRANSFER:) (const)
- [BatchTxn](#BatchTxn) (struct)
- [Ledger](#Ledger) (struct)
- [BatchReport](#BatchReport) (struct)
- [BatchFile](#BatchFile) (struct)
- [ledger_new](#ledger_new) (function)
- [ledger_open](#ledger_open) (function)
- [ledger_index](#ledger_index) (function)
- [ledger_balance](#ledger_balance) (function)
- [ledger_total](#ledger_total) (function)
- [batch_move](#batch_move) (function)
- [batch_credit](#batch_credit) (function)
- [batch_debit](#batch_debit) (function)
- [batch_run](#batch_run) (function)
- [batch_report_text](#batch_report_text) (function)
- [batch_txn_copybook](#batch_txn_copybook) (function)
- [batch_write_file](#batch_write_file) (function)
- [batch_read_file](#batch_read_file) (function)

## BATCH_DEPOSIT:

```fides
pub const BATCH_DEPOSIT: i64 = 1;
```

*Defined at line 37*

---

## BATCH_WITHDRAW:

```fides
pub const BATCH_WITHDRAW: i64 = 2;
```

*Defined at line 38*

---

## BATCH_TRANSFER:

```fides
pub const BATCH_TRANSFER: i64 = 3;
```

*Defined at line 39*

---

## BatchTxn

One transaction of a batch.

```fides
pub struct BatchTxn {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64,  // BATCH_DEPOSIT / WITHDRAW / TRANSFER` |
| `from` | `i64,  // the account (deposit/withdraw) or transfer source` |
| `to` | `i64,  // transfer target (0 otherwise)` |
| `cents` | `i64` |
| `ref_id` | `str` |

*Defined at line 42*

---

## Ledger

The accounts a batch posts to: id and balance in cents, in opening order.

```fides
pub struct Ledger {
```

**Fields:**

| Name | Type |
|------|------|
| `ids` | `Vec<i64>` |
| `cents` | `Vec<i64>` |

*Defined at line 51*

---

## BatchReport

What a batch run did.

```fides
pub struct BatchReport {
```

**Fields:**

| Name | Type |
|------|------|
| `applied` | `i64` |
| `rejected` | `i64` |
| `deposits` | `i64,  // cents posted in` |
| `withdrawals` | `i64,  // cents paid out` |
| `transfers` | `i64,  // cents moved between accounts` |
| `opening` | `i64,  // ledger total before` |
| `closing` | `i64,  // ledger total after` |
| `balanced` | `bool,  // closing == opening + deposits - withdrawals` |
| `errors` | `Vec<str>,  // one line per rejected transaction / file error` |

*Defined at line 57*

---

## BatchFile

The parsed transaction file: the transactions and file-level checks.

```fides
pub struct BatchFile {
```

**Fields:**

| Name | Type |
|------|------|
| `txns` | `Vec<BatchTxn>` |
| `errors` | `Vec<str>,  // header/trailer/count/total/data errors` |

*Defined at line 70*

---

## ledger_new

```fides
pub fn ledger_new() -> Ledger {
```

**Returns:** `Ledger`

*Defined at line 77*

---

## ledger_open

Open account `id` with `cents`; false when it is already open.

```fides
pub fn ledger_open(led: &mut Ledger, id: i64, cents: i64) -> bool {
```

**Parameters:**

- `led`: `&mut Ledger`
- `id`: `i64`
- `cents`: `i64`

**Returns:** `bool`

*Defined at line 84*

---

## ledger_index

Index of account `id`, or -1.

```fides
pub fn ledger_index(led: &Ledger, id: i64) -> i64 {
```

**Parameters:**

- `led`: `&Ledger`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 94*

---

## ledger_balance

Balance of account `id` in cents (0 when absent).

```fides
pub fn ledger_balance(led: &Ledger, id: i64) -> i64 {
```

**Parameters:**

- `led`: `&Ledger`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 106*

---

## ledger_total

Sum of all balances.

```fides
pub fn ledger_total(led: &Ledger) -> i64 {
```

**Parameters:**

- `led`: `&Ledger`

**Returns:** `i64`

*Defined at line 115*

---

## batch_move

```fides
pub fn batch_move(from: &mut Money, to: Money, cents: i64) -> Money {
```

**Parameters:**

- `from`: `&mut Money`
- `to`: `Money`
- `cents`: `i64`

**Returns:** `Money`

*Defined at line 130*

---

## batch_credit

```fides
pub fn batch_credit(balance: Money, deposit: Money) -> Money {
```

**Parameters:**

- `balance`: `Money`
- `deposit`: `Money`

**Returns:** `Money`

*Defined at line 138*

---

## batch_debit

```fides
pub fn batch_debit(balance: &mut Money, cents: i64) -> Money {
```

**Parameters:**

- `balance`: `&mut Money`
- `cents`: `i64`

**Returns:** `Money`

*Defined at line 144*

---

## batch_run

Post every transaction in order. A transaction that names an unknown
account, has a non-positive amount, or would overdraw is rejected and
changes nothing; the rest are applied. The report's `balanced` checks
the control totals afterwards.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var led = ledger_new();
assert(ledger_open(&mut led, 1, 1000));
assert(ledger_open(&mut led, 2, 0));
assert(!ledger_open(&mut led, 1, 5));
var txns: Vec<BatchTxn> = Vec::new();
txns.push(BatchTxn { kind: BATCH_TRANSFER, from: 1, to: 2, cents: 400, ref_id: "T1" });
txns.push(BatchTxn { kind: BATCH_WITHDRAW, from: 2, to: 0, cents: 900, ref_id: "T2" });
val rep = batch_run(&mut led, txns);
assert_eq(rep.applied, 1);
assert_eq(rep.rejected, 1);
assert(rep.balanced);
assert_eq(ledger_balance(&led, 2), 400);
assert_eq(ledger_total(&led), 1000);
```

```fides
pub fn batch_run(led: &mut Ledger, txns: Vec<BatchTxn>) -> BatchReport {
```

**Parameters:**

- `led`: `&mut Ledger`
- `txns`: `Vec<BatchTxn>`

**Returns:** `BatchReport`

*Defined at line 211*

---

## batch_report_text

The report as text, one fact a line, then the errors.

```fides
pub fn batch_report_text(rep: &BatchReport) -> str {
```

**Parameters:**

- `rep`: `&BatchReport`

**Returns:** `str`

*Defined at line 272*

---

## batch_txn_copybook

The default transaction record layout (see the top of this file).

```fides
pub fn batch_txn_copybook() -> Copybook {
```

**Returns:** `Copybook`

*Defined at line 290*

---

## batch_write_file

The transactions as a COBOL file: a header record, one record each,
and a trailer with the count and the amount total (no line ends:
a plain fixed-length sequential file).

```fides
pub fn batch_write_file(cb: &Copybook, txns: Vec<BatchTxn>) -> Vec<i64> {
```

**Parameters:**

- `cb`: `&Copybook`
- `txns`: `Vec<BatchTxn>`

**Returns:** `Vec<i64>`

*Defined at line 307*

---

## batch_read_file

Read a transaction file written with the layout `cb`. Checks that it
starts with a header, ends with a trailer whose count and total match,
and that every amount is valid packed decimal; problems go to
`.errors` (a bad record is left out).

```fides
pub fn batch_read_file(cb: &Copybook, data: Vec<i64>) -> BatchFile {
```

**Parameters:**

- `cb`: `&Copybook`
- `data`: `Vec<i64>`

**Returns:** `BatchFile`

*Defined at line 336*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
