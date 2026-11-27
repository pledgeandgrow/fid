[All modules](index.md)

# banking -- Fides Documentation

## Contents

- [Account](#Account) (struct)
- [Account_open](#Account_open) (function)
- [Account_deposit](#Account_deposit) (function)
- [Account_withdraw](#Account_withdraw) (function)
- [Account_transfer](#Account_transfer) (function)
- [Account_accrue_interest](#Account_accrue_interest) (function)
- [Account_balance_cents](#Account_balance_cents) (function)
- [Account_id](#Account_id) (function)
- [Account_print](#Account_print) (function)
- [ACCOUNT_STATE_ACTIVE:](#ACCOUNT_STATE_ACTIVE:) (const)
- [ACCOUNT_STATE_FROZEN:](#ACCOUNT_STATE_FROZEN:) (const)
- [ACCOUNT_STATE_CLOSED:](#ACCOUNT_STATE_CLOSED:) (const)
- [AccountActive](#AccountActive) (struct)
- [AccountFrozen](#AccountFrozen) (struct)
- [AccountClosed](#AccountClosed) (struct)
- [AccountActive_open](#AccountActive_open) (function)
- [AccountActive_deposit](#AccountActive_deposit) (function)
- [AccountActive_withdraw](#AccountActive_withdraw) (function)
- [AccountActive_transfer](#AccountActive_transfer) (function)
- [AccountActive_freeze](#AccountActive_freeze) (function)
- [AccountActive_close](#AccountActive_close) (function)
- [AccountActive_balance_cents](#AccountActive_balance_cents) (function)
- [AccountActive_id](#AccountActive_id) (function)
- [AccountFrozen_unfreeze](#AccountFrozen_unfreeze) (function)
- [AccountFrozen_close](#AccountFrozen_close) (function)
- [AccountFrozen_balance_cents](#AccountFrozen_balance_cents) (function)
- [AccountFrozen_id](#AccountFrozen_id) (function)
- [AccountClosed_balance_cents](#AccountClosed_balance_cents) (function)
- [AccountClosed_id](#AccountClosed_id) (function)
- [account_state_name](#account_state_name) (function)

## Account

```fides
pub struct Account {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `balance` | `Money` |

*Defined at line 29*

---

## Account_open

Open a new account with an initial balance.
NOT @conserve because it calls Money_new (minting).

```fides
pub fn Account_open(id: i64, initial_cents: i64) -> Account {
```

**Parameters:**

- `id`: `i64`
- `initial_cents`: `i64`

**Returns:** `Account`

*Defined at line 38*

---

## Account_deposit

```fides
pub fn Account_deposit(acc: Account, amount: Money) -> Account {
```

**Parameters:**

- `acc`: `Account`
- `amount`: `Money`

**Returns:** `Account`

*Defined at line 50*

---

## Account_withdraw

Withdraw money from an account. Consumes the account,
returns a new account with reduced balance AND the withdrawn cash.
Conservation: old_balance = new_balance + withdrawn.
NOT @conserve: it rebuilds balances from cents (Money_from_cents mints).
A conserving version needs Money_split, blocked by the tuple layout bug
(compiler/lower.fd names every 2-tuple layout `__tuple_2`).

```fides
pub fn Account_withdraw(acc: Account, withdraw_cents: i64) -> (Account, Money) {
```

**Parameters:**

- `acc`: `Account`
- `withdraw_cents`: `i64`

**Returns:** `(Account, Money)`

*Defined at line 64*

---

## Account_transfer

Transfer money from one account to another. Consumes both accounts,
returns both with updated balances.
Conservation: A loses X, B gains X. Total unchanged.
NOT @conserve: it rebuilds balances from cents (Money_from_cents mints).
A conserving version needs Money_split, blocked by the tuple layout bug
(compiler/lower.fd names every 2-tuple layout `__tuple_2`).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
use money;
val a = Account_open(1, 1000);
val b = Account_open(2, 0);
val (a2, b2) = Account_transfer(a, b, 300);
assert_eq(Account_balance_cents(a2), 700);
assert_eq(Account_balance_cents(b2), 300);
```

```fides
pub fn Account_transfer(from: Account, to: Account, amount_cents: i64) -> (Account, Account) {
```

**Parameters:**

- `from`: `Account`
- `to`: `Account`
- `amount_cents`: `i64`

**Returns:** `(Account, Account)`

*Defined at line 88*

---

## Account_accrue_interest

```fides
pub fn Account_accrue_interest(acc: Account, rate_bps: i64) -> Account {
```

**Parameters:**

- `acc`: `Account`
- `rate_bps`: `i64`

**Returns:** `Account`

*Defined at line 109*

---

## Account_balance_cents

Get the account balance in cents. Consumes the account.
Not @conserve: it consumes the account, so its money leaves the system.

```fides
pub fn Account_balance_cents(acc: Account) -> i64 {
```

**Parameters:**

- `acc`: `Account`

**Returns:** `i64`

*Defined at line 120*

---

## Account_id

Get the account ID. Consumes the account.
Not @conserve: it consumes the account, so its money leaves the system.

```fides
pub fn Account_id(acc: Account) -> i64 {
```

**Parameters:**

- `acc`: `Account`

**Returns:** `i64`

*Defined at line 126*

---

## Account_print

Print account info. NOT @conserve (has IO effect).

```fides
pub fn Account_print(acc: Account) -> void {
```

**Parameters:**

- `acc`: `Account`

**Returns:** `void`

*Defined at line 131*

---

## ACCOUNT_STATE_ACTIVE:

Account state constants.

```fides
pub const ACCOUNT_STATE_ACTIVE: i64 = 0;
```

*Defined at line 165*

---

## ACCOUNT_STATE_FROZEN:

```fides
pub const ACCOUNT_STATE_FROZEN: i64 = 1;
```

*Defined at line 166*

---

## ACCOUNT_STATE_CLOSED:

```fides
pub const ACCOUNT_STATE_CLOSED: i64 = 2;
```

*Defined at line 167*

---

## AccountActive

```fides
pub struct AccountActive {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `balance` | `Money` |

*Defined at line 170*

---

## AccountFrozen

```fides
pub struct AccountFrozen {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `balance` | `Money` |

*Defined at line 177*

---

## AccountClosed

```fides
pub struct AccountClosed {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `balance` | `Money` |

*Defined at line 184*

---

## AccountActive_open

Open a new active account with an initial balance.

```fides
pub fn AccountActive_open(id: i64, initial_cents: i64) -> AccountActive {
```

**Parameters:**

- `id`: `i64`
- `initial_cents`: `i64`

**Returns:** `AccountActive`

*Defined at line 192*

---

## AccountActive_deposit

```fides
pub fn AccountActive_deposit(acc: AccountActive, amount: Money) -> AccountActive {
```

**Parameters:**

- `acc`: `AccountActive`
- `amount`: `Money`

**Returns:** `AccountActive`

*Defined at line 200*

---

## AccountActive_withdraw

Withdraw money from an active account.
NOT @conserve: it rebuilds balances from cents (Money_from_cents mints).
A conserving version needs Money_split, blocked by the tuple layout bug
(compiler/lower.fd names every 2-tuple layout `__tuple_2`).

```fides
pub fn AccountActive_withdraw(acc: AccountActive, withdraw_cents: i64) -> (AccountActive, Money) {
```

**Parameters:**

- `acc`: `AccountActive`
- `withdraw_cents`: `i64`

**Returns:** `(AccountActive, Money)`

*Defined at line 211*

---

## AccountActive_transfer

Transfer money between two active accounts.
NOT @conserve: it rebuilds balances from cents (Money_from_cents mints).
A conserving version needs Money_split, blocked by the tuple layout bug
(compiler/lower.fd names every 2-tuple layout `__tuple_2`).

```fides
pub fn AccountActive_transfer(from: AccountActive, to: AccountActive, amount_cents: i64) -> (AccountActive, AccountActive) {
```

**Parameters:**

- `from`: `AccountActive`
- `to`: `AccountActive`
- `amount_cents`: `i64`

**Returns:** `(AccountActive, AccountActive)`

*Defined at line 224*

---

## AccountActive_freeze

```fides
pub fn AccountActive_freeze(acc: AccountActive) -> AccountFrozen {
```

**Parameters:**

- `acc`: `AccountActive`

**Returns:** `AccountFrozen`

*Defined at line 238*

---

## AccountActive_close

```fides
pub fn AccountActive_close(acc: AccountActive) -> AccountClosed {
```

**Parameters:**

- `acc`: `AccountActive`

**Returns:** `AccountClosed`

*Defined at line 246*

---

## AccountActive_balance_cents

Get balance of an active account.
Not @conserve: it consumes the account, so its money leaves the system.

```fides
pub fn AccountActive_balance_cents(acc: AccountActive) -> i64 {
```

**Parameters:**

- `acc`: `AccountActive`

**Returns:** `i64`

*Defined at line 254*

---

## AccountActive_id

Get ID of an active account.
Not @conserve: it consumes the account, so its money leaves the system.

```fides
pub fn AccountActive_id(acc: AccountActive) -> i64 {
```

**Parameters:**

- `acc`: `AccountActive`

**Returns:** `i64`

*Defined at line 260*

---

## AccountFrozen_unfreeze

```fides
pub fn AccountFrozen_unfreeze(acc: AccountFrozen) -> AccountActive {
```

**Parameters:**

- `acc`: `AccountFrozen`

**Returns:** `AccountActive`

*Defined at line 271*

---

## AccountFrozen_close

```fides
pub fn AccountFrozen_close(acc: AccountFrozen) -> AccountClosed {
```

**Parameters:**

- `acc`: `AccountFrozen`

**Returns:** `AccountClosed`

*Defined at line 279*

---

## AccountFrozen_balance_cents

Get balance of a frozen account (read-only).
Not @conserve: it consumes the account, so its money leaves the system.

```fides
pub fn AccountFrozen_balance_cents(acc: AccountFrozen) -> i64 {
```

**Parameters:**

- `acc`: `AccountFrozen`

**Returns:** `i64`

*Defined at line 287*

---

## AccountFrozen_id

Get ID of a frozen account.
Not @conserve: it consumes the account, so its money leaves the system.

```fides
pub fn AccountFrozen_id(acc: AccountFrozen) -> i64 {
```

**Parameters:**

- `acc`: `AccountFrozen`

**Returns:** `i64`

*Defined at line 293*

---

## AccountClosed_balance_cents

Get final balance of a closed account (for audit).
Not @conserve: it consumes the account, so its money leaves the system.

```fides
pub fn AccountClosed_balance_cents(acc: AccountClosed) -> i64 {
```

**Parameters:**

- `acc`: `AccountClosed`

**Returns:** `i64`

*Defined at line 304*

---

## AccountClosed_id

Get ID of a closed account.
Not @conserve: it consumes the account, so its money leaves the system.

```fides
pub fn AccountClosed_id(acc: AccountClosed) -> i64 {
```

**Parameters:**

- `acc`: `AccountClosed`

**Returns:** `i64`

*Defined at line 310*

---

## account_state_name

```fides
pub fn account_state_name(state: i64) -> str {
```

**Parameters:**

- `state`: `i64`

**Returns:** `str`

*Defined at line 316*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
