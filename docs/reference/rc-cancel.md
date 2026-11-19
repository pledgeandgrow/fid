# Counts removed where ownership is known (LF15)

Under proposal 0004 a variable owns its value and drops it at the end of its
scope; keeping a value (`var t = s`) counts it again (`Str_dup`, `Vec_share`,
`HashMap_share`) so that each owner can drop its own count. Many of these
counts are undone later by a drop with nothing in between that needs them.
After lowering, `lw_rc_cancel_pairs` (compiler/lower.fd) removes them in
three steps.

## In one block, and along straight chains of blocks

A `Str_dup`/`Str_drop` or `Box_dup_t`/`Box_drop_t` pair on the same pointer
with nothing between that could free or keep it, and a Vec or map shared
for a temporary that is only read before its drop (`lw_rc_cancel_share`,
`LOWER_CANCEL_SHARES`), go. Since LOWER_CANCEL_ACROSS the scan also follows a
block into the blocks it runs straight on into: a block ending in a plain
`Jump` to a block that nothing else jumps to (`lw_rc_chain_next`,
`lw_rc_chain_insts`, at most `LW_CHAIN_MAX` = 4 blocks). Test:
tests/bootstrap/test_rc_cancel_across.fd.

## Across joins and branches: copies that only read

`lw_rc_cancel_param_slots` handles the case where the count is made in one
block and the drops are on every path out of the copy's scope: in either
arm of an `if`, at the join, on each `return`, at the end of each round of a
loop.

```fid
fn pick_len(s: str, c: bool) -> i64 {
    var t: str = s;       // Str_dup(s) ... removed
    var n: i64 = 0;
    if c {
        n = show(t);
    } else {
        n = 2;
    }
    return n;             // Str_drop(t) ... removed
}
```

The variable `t` then borrows its source instead of owning a count of it.
That is safe when:

* **The copy only reads.** `t`'s slot is an `Alloca` that is only loaded
  and stored (its address is never taken). Besides the store of the copy it
  is only stored literals (null, zero, a string literal), each either its
  first value or clearing it right after a drop (so it is never moved out).
  Every load of it is dropped (that drop being its one use) or only read
  (`lw_ps_use_ok`): a string by any call except a free or `p_str_append`
  (which grows its first argument in place), by a comparison or a print; a
  Vec or map by core's read-only calls (`lw_rc_read_only_call`), by a share
  for a keeper, or as a by-value Vec/map parameter of the program's own
  functions (which copy a parameter before changing it). A keeper of `t`
  (a field, an element) gets its own count as before.
* **The source outlives every read of the copy.**
  * A by-value parameter (`LOWER_CANCEL_PARAM_SLOTS`): its caller owns it
    for the whole call, and the function never drops it, frees it, grows it
    in place or stores through it (`lw_ps_param_untouched`).
  * A local variable (`LOWER_CANCEL_LOCAL_SLOTS`, `lw_ps_local_src_ok`):
    its slot is only loaded and stored, it owns its values (it has drops: a
    variable that borrows, such as a loop variable or a binding, has none),
    and no load of `t` can run after a *kill* of the source unless `t` was
    given a new value first (`lw_ps_reaches`, a walk over the blocks). A
    kill is a drop of the source, any store into it (a new value, or the
    literal that clears it after a move or a drop), and any use of one of
    its loads that does more than read (a push, an append in place, a
    hand-over). Changes of the source before the copy is made are fine:
    `var v = Vec::new(); v.push(5); ... val w = v` in a loop.
* **No borrower is borrowed from.** A slot that now borrows is not the
  source of another removal, and a slot that is borrowed from is not made a
  borrower (`roles` in `lw_rc_cancel_param_slots`): a borrower has no kills
  that say when its value goes.
* Not in a function that opens a region (`lw_rg_has_region`).

When all hold, the count goes, every drop of `t`'s slot goes, and a Vec or
map slot is stored the source itself instead of the share.

The lifetime a copy gets is the one its count had: from its store to its
drops. Whatever reads its value (a call, a task's frame that borrows an
argument) must be done by then, or it read a freed value before too. The one
difference is a path on which the copy is never dropped (a loop that never
ends, after a kill of a local source): its count kept the value alive there
for good, the borrow does not. Only a task that borrows an argument and is
never awaited on such a path could see it.

Copies that keep their count: one that is moved out (`return t`, pushed on
its last use), reassigned or appended to, changed (`w.push(9)`), or that can
be read after its source is dropped or reassigned (a copy declared before a
loop that copies a value made in each round).

## Checking it

* `FID_RC_SLOTS_OFF=1` turns the joins-and-branches step off (to compare).
* `tests/rc_cancel/check.sh $FID` generates the C of
  tests/bootstrap/test_rc_cancel_diamond.fd (copies of parameters) and
  test_rc_cancel_local.fd (copies of locals) with the step on and off and
  counts, per function, the count and drop calls: the copies that only read
  lose them, the others keep the same number. It then runs both programs
  with `--leaks`, on and off: the expected output and every block freed.
* `scripts/leak-check.sh $FID` (tests/leaks) and the suite must stay as they
  were.
