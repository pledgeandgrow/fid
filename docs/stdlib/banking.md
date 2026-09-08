# banking — Fides Documentation

## Contents

- [Account](#account) (struct)
- [Account_open](#account_open) (function)
- [Account_deposit](#account_deposit) (function)
- [Account_withdraw](#account_withdraw) (function)
- [Account_transfer](#account_transfer) (function)
- [Account_accrue_interest](#account_accrue_interest) (function)
- [Account_balance_cents](#account_balance_cents) (function)
- [Account_id](#account_id) (function)
- [Account_print](#account_print) (function)
- [ACCOUNT_STATE_ACTIVE:](#account_state_active:) (const)
- [ACCOUNT_STATE_FROZEN:](#account_state_frozen:) (const)
- [ACCOUNT_STATE_CLOSED:](#account_state_closed:) (const)
- [AccountActive](#accountactive) (struct)
- [AccountFrozen](#accountfrozen) (struct)
- [AccountClosed](#accountclosed) (struct)
- [AccountActive_open](#accountactive_open) (function)
- [AccountActive_deposit](#accountactive_deposit) (function)
- [AccountActive_withdraw](#accountactive_withdraw) (function)
- [AccountActive_transfer](#accountactive_transfer) (function)
- [AccountActive_freeze](#accountactive_freeze) (function)
- [AccountActive_close](#accountactive_close) (function)
- [AccountActive_balance_cents](#accountactive_balance_cents) (function)
- [AccountActive_id](#accountactive_id) (function)
- [AccountFrozen_unfreeze](#accountfrozen_unfreeze) (function)
- [AccountFrozen_close](#accountfrozen_close) (function)
- [AccountFrozen_balance_cents](#accountfrozen_balance_cents) (function)
- [AccountFrozen_id](#accountfrozen_id) (function)
- [AccountClosed_balance_cents](#accountclosed_balance_cents) (function)
- [AccountClosed_id](#accountclosed_id) (function)
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

*Defined at line 30*

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

*Defined at line 39*

---

## Account_deposit

```fides
pub fn Account_deposit(acc: Account, amount: Money) -> Account {
```

**Parameters:**

- `acc`: `Account`
- `amount`: `Money`

**Returns:** `Account`

*Defined at line 49*

---

## Account_withdraw

```fides
pub fn Account_withdraw(acc: Account, withdraw_cents: i64) -> (Account, Money) {
```

**Parameters:**

- `acc`: `Account`
- `withdraw_cents`: `i64`

**Returns:** `(Account, Money)`

*Defined at line 59*

---

## Account_transfer

```fides
pub fn Account_transfer(from: Account, to: Account, amount_cents: i64) -> (Account, Account) {
```

**Parameters:**

- `from`: `Account`
- `to`: `Account`
- `amount_cents`: `i64`

**Returns:** `(Account, Account)`

*Defined at line 71*

---

## Account_accrue_interest

```fides
pub fn Account_accrue_interest(acc: Account, rate_bps: i64) -> Account {
```

**Parameters:**

- `acc`: `Account`
- `rate_bps`: `i64`

**Returns:** `Account`

*Defined at line 88*

---

## Account_balance_cents

```fides
pub fn Account_balance_cents(acc: Account) -> i64 {
```

**Parameters:**

- `acc`: `Account`

**Returns:** `i64`

*Defined at line 97*

---

## Account_id

```fides
pub fn Account_id(acc: Account) -> i64 {
```

**Parameters:**

- `acc`: `Account`

**Returns:** `i64`

*Defined at line 103*

---

## Account_print

Print account info. NOT @conserve (has IO effect).

```fides
pub fn Account_print(acc: Account) -> void {
```

**Parameters:**

- `acc`: `Account`

**Returns:** `void`

*Defined at line 108*

---

## ACCOUNT_STATE_ACTIVE:

Account state constants.

```fides
pub const ACCOUNT_STATE_ACTIVE: i64  = 0;
```

*Defined at line 142*

---

## ACCOUNT_STATE_FROZEN:

```fides
pub const ACCOUNT_STATE_FROZEN: i64  = 1;
```

*Defined at line 143*

---

## ACCOUNT_STATE_CLOSED:

```fides
pub const ACCOUNT_STATE_CLOSED: i64  = 2;
```

*Defined at line 144*

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

*Defined at line 148*

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

*Defined at line 155*

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

*Defined at line 162*

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

*Defined at line 170*

---

## AccountActive_deposit

```fides
pub fn AccountActive_deposit(acc: AccountActive, amount: Money) -> AccountActive {
```

**Parameters:**

- `acc`: `AccountActive`
- `amount`: `Money`

**Returns:** `AccountActive`

*Defined at line 176*

---

## AccountActive_withdraw

```fides
pub fn AccountActive_withdraw(acc: AccountActive, withdraw_cents: i64) -> (AccountActive, Money) {
```

**Parameters:**

- `acc`: `AccountActive`
- `withdraw_cents`: `i64`

**Returns:** `(AccountActive, Money)`

*Defined at line 183*

---

## AccountActive_transfer

```fides
pub fn AccountActive_transfer(from: AccountActive, to: AccountActive,
```

**Parameters:**

- `from`: `AccountActive`
- `to`: `AccountActive`

*Defined at line 192*

---

## AccountActive_freeze

```fides
pub fn AccountActive_freeze(acc: AccountActive) -> AccountFrozen {
```

**Parameters:**

- `acc`: `AccountActive`

**Returns:** `AccountFrozen`

*Defined at line 203*

---

## AccountActive_close

```fides
pub fn AccountActive_close(acc: AccountActive) -> AccountClosed {
```

**Parameters:**

- `acc`: `AccountActive`

**Returns:** `AccountClosed`

*Defined at line 209*

---

## AccountActive_balance_cents

```fides
pub fn AccountActive_balance_cents(acc: AccountActive) -> i64 {
```

**Parameters:**

- `acc`: `AccountActive`

**Returns:** `i64`

*Defined at line 215*

---

## AccountActive_id

```fides
pub fn AccountActive_id(acc: AccountActive) -> i64 {
```

**Parameters:**

- `acc`: `AccountActive`

**Returns:** `i64`

*Defined at line 221*

---

## AccountFrozen_unfreeze

```fides
pub fn AccountFrozen_unfreeze(acc: AccountFrozen) -> AccountActive {
```

**Parameters:**

- `acc`: `AccountFrozen`

**Returns:** `AccountActive`

*Defined at line 232*

---

## AccountFrozen_close

```fides
pub fn AccountFrozen_close(acc: AccountFrozen) -> AccountClosed {
```

**Parameters:**

- `acc`: `AccountFrozen`

**Returns:** `AccountClosed`

*Defined at line 238*

---

## AccountFrozen_balance_cents

```fides
pub fn AccountFrozen_balance_cents(acc: AccountFrozen) -> i64 {
```

**Parameters:**

- `acc`: `AccountFrozen`

**Returns:** `i64`

*Defined at line 244*

---

## AccountFrozen_id

```fides
pub fn AccountFrozen_id(acc: AccountFrozen) -> i64 {
```

**Parameters:**

- `acc`: `AccountFrozen`

**Returns:** `i64`

*Defined at line 250*

---

## AccountClosed_balance_cents

```fides
pub fn AccountClosed_balance_cents(acc: AccountClosed) -> i64 {
```

**Parameters:**

- `acc`: `AccountClosed`

**Returns:** `i64`

*Defined at line 261*

---

## AccountClosed_id

```fides
pub fn AccountClosed_id(acc: AccountClosed) -> i64 {
```

**Parameters:**

- `acc`: `AccountClosed`

**Returns:** `i64`

*Defined at line 267*

---

## account_state_name

```fides
pub fn account_state_name(state: i64) -> str {
```

**Parameters:**

- `state`: `i64`

**Returns:** `str`

*Defined at line 273*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
