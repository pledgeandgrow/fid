# Lab 9 — Functions the compiler proves cannot crash

**Unit:** [week 9, effect systems II: safety annotations](../../week09-safety-annotations.md)
(exercises 1–2). **Time:** 2 hours. **Runs in the playground:** yes
(the solution runs there; read the starter's errors with `fid check`,
whose messages are the ones this sheet describes).

`@crashfree` asks the compiler to prove that a function cannot panic:
no division by a possibly-zero value, no index that may be out of bounds,
no overflowing arithmetic, no call to a function without the same
guarantee. You turn three crashing functions into total ones.

## Start

- [Open the starter in the playground](https://fides-lang.org/playground#JTJGJTJGJTIwTGFiJTIwOSUyMHN0YXJ0ZXIlM0ElMjBmdW5jdGlvbnMlMjB0aGUlMjBjb21waWxlciUyMHByb3ZlcyUyMGNhbm5vdCUyMGNyYXNoLiUwQSUyRiUyRiElMjBsYWIlM0ElMjA5JTBBJTJGJTJGISUyMGNoZWNrJTNBJTIwcmVqZWN0ZWQlMEElMkYlMkYhJTIwZXJyb3IlM0ElMjBjcmFzaGZyZWUlMEElMkYlMkYlMEElMkYlMkYlMjAlNjBmaWQlMjBjaGVjayU2MCUyMHJlamVjdHMlMjB0aGlzJTIwZmlsZS4lMjBUYXNrcyUyMChzZWUlMjBSRUFETUUubWQpJTNBJTBBJTJGJTJGJTIwJTIwJTIwMS4lMjBNYWtlJTIwJTYwc2FmZV9kaXYlNjAlMjBwYXNzJTIwJTQwY3Jhc2hmcmVlJTNBJTIwaXQlMjByZXR1cm5zJTIwT3B0aW9uJTNDaTY0JTNFJTJDJTIwTm9uZSUyMGZvciUwQSUyRiUyRiUyMCUyMCUyMCUyMCUyMCUyMGElMjB6ZXJvJTIwZGl2aXNvci4lMEElMkYlMkYlMjAlMjAlMjAyLiUyMFdyaXRlJTIwJTYwYXZlcmFnZSh2JTNBJTIwJTI2VmVjJTNDaTY0JTNFKSUyMC0lM0UlMjBPcHRpb24lM0NpNjQlM0UlNjAlMjB0aGF0JTIwcGFzc2VzJTBBJTJGJTJGJTIwJTIwJTIwJTIwJTIwJTIwJTQwY3Jhc2hmcmVlJTIwKE5vbmUlMjBmb3IlMjBhbiUyMGVtcHR5JTIwVmVjKS4lMEElMkYlMkYlMjAlMjAlMjAzLiUyME1ha2UlMjAlNjBmYWN0JTYwJTIwcGFzcyUyMCU0MGNyYXNoZnJlZSUyMGFuZCUyMCU0MG5vLXJlY3Vyc2lvbiUyQyUyMHJldHVybmluZyUyMFNvbWUlMjBmb3IlMEElMkYlMkYlMjAlMjAlMjAlMjAlMjAlMjBuJTIwaW4lMjAwLi4lM0QyMCUyMGFuZCUyME5vbmUlMjBvdGhlcndpc2UuJTBBJTJGJTJGJTIwJTIwJTIwNC4lMjBQcmludCUyMHRoZSUyMHJlc3VsdHMlMjBsaXN0ZWQlMjBhdCUyMHRoZSUyMGVuZCUyMG9mJTIwJTYwbWFpbiU2MC4lMEElMEElNDBjcmFzaGZyZWUlMEFmbiUyMHNhZmVfZGl2KGElM0ElMjBpNjQlMkMlMjBiJTNBJTIwaTY0KSUyMC0lM0UlMjBpNjQlMjAlN0IlMEElMjAlMjAlMjAlMjByZXR1cm4lMjBhJTIwJTJGJTIwYiUzQiUwQSU3RCUwQSUwQSU0MGNyYXNoZnJlZSUyMCU0MG5vLXJlY3Vyc2lvbiUwQWZuJTIwZmFjdChuJTNBJTIwaTY0KSUyMC0lM0UlMjBpNjQlMjAlN0IlMEElMjAlMjAlMjAlMjBpZiUyMG4lMjAlM0MlM0QlMjAxJTIwJTdCJTIwcmV0dXJuJTIwMSUzQiUyMCU3RCUwQSUyMCUyMCUyMCUyMHJldHVybiUyMG4lMjAqJTIwZmFjdChuJTIwLSUyMDEpJTNCJTBBJTdEJTBBJTBBZm4lMjBzaG93KG8lM0ElMjBPcHRpb24lM0NpNjQlM0UpJTIwLSUzRSUyMHN0ciUyMCU3QiUwQSUyMCUyMCUyMCUyMG1hdGNoJTIwbyUyMCU3QiUwQSUyMCUyMCUyMCUyMCUyMCUyMCUyMCUyME9wdGlvbiUzQSUzQVNvbWUoeCklMjAlM0QlM0UlMjAlN0IlMjByZXR1cm4lMjBpbnRfdG9fc3RyKHgpJTNCJTIwJTdEJTBBJTIwJTIwJTIwJTIwJTIwJTIwJTIwJTIwT3B0aW9uJTNBJTNBTm9uZSUyMCUzRCUzRSUyMCU3QiUyMHJldHVybiUyMCUyMm5vbmUlMjIlM0IlMjAlN0QlMEElMjAlMjAlMjAlMjAlN0QlMEElN0QlMEElMEFmbiUyMG1haW4oKSUyMC0lM0UlMjBpNjQlMjAlN0IlMEElMjAlMjAlMjAlMjBwcmludGxuKGludF90b19zdHIoc2FmZV9kaXYoMTAlMkMlMjAzKSkpJTNCJTBBJTIwJTIwJTIwJTIwcHJpbnRsbihpbnRfdG9fc3RyKGZhY3QoNSkpKSUzQiUwQSUyMCUyMCUyMCUyMCUyRiUyRiUyMFRPRE8lMjA0JTNBJTIwc2FmZV9kaXYoNyUyQyUyMDApJTJDJTIwYXZlcmFnZSUyMG9mJTIwJTVCMyUyQyUyMDQlMkMlMjA4JTVEJTJDJTIwYXZlcmFnZSUyMG9mJTIwJTVCJTVEJTJDJTBBJTIwJTIwJTIwJTIwJTJGJTJGJTIwJTIwJTIwJTIwJTIwJTIwJTIwJTIwJTIwZmFjdCgyMCklMkMlMjBmYWN0KDIxKSUyQyUyMGZhY3QoLTEpJTBBJTIwJTIwJTIwJTIwcmV0dXJuJTIwMCUzQiUwQSU3RCUwQQ==) to read it, and
- copy [`starter.fd`](starter.fd) and run `fid check starter.fd`. It is
  rejected, with one error per rule the functions break.

Embed:

```html
<iframe src="https://fides-lang.org/playground/embed.html#JTJGJTJGJTIwTGFiJTIwOSUyMHN0YXJ0ZXIlM0ElMjBmdW5jdGlvbnMlMjB0aGUlMjBjb21waWxlciUyMHByb3ZlcyUyMGNhbm5vdCUyMGNyYXNoLiUwQSUyRiUyRiElMjBsYWIlM0ElMjA5JTBBJTJGJTJGISUyMGNoZWNrJTNBJTIwcmVqZWN0ZWQlMEElMkYlMkYhJTIwZXJyb3IlM0ElMjBjcmFzaGZyZWUlMEElMkYlMkYlMEElMkYlMkYlMjAlNjBmaWQlMjBjaGVjayU2MCUyMHJlamVjdHMlMjB0aGlzJTIwZmlsZS4lMjBUYXNrcyUyMChzZWUlMjBSRUFETUUubWQpJTNBJTBBJTJGJTJGJTIwJTIwJTIwMS4lMjBNYWtlJTIwJTYwc2FmZV9kaXYlNjAlMjBwYXNzJTIwJTQwY3Jhc2hmcmVlJTNBJTIwaXQlMjByZXR1cm5zJTIwT3B0aW9uJTNDaTY0JTNFJTJDJTIwTm9uZSUyMGZvciUwQSUyRiUyRiUyMCUyMCUyMCUyMCUyMCUyMGElMjB6ZXJvJTIwZGl2aXNvci4lMEElMkYlMkYlMjAlMjAlMjAyLiUyMFdyaXRlJTIwJTYwYXZlcmFnZSh2JTNBJTIwJTI2VmVjJTNDaTY0JTNFKSUyMC0lM0UlMjBPcHRpb24lM0NpNjQlM0UlNjAlMjB0aGF0JTIwcGFzc2VzJTBBJTJGJTJGJTIwJTIwJTIwJTIwJTIwJTIwJTQwY3Jhc2hmcmVlJTIwKE5vbmUlMjBmb3IlMjBhbiUyMGVtcHR5JTIwVmVjKS4lMEElMkYlMkYlMjAlMjAlMjAzLiUyME1ha2UlMjAlNjBmYWN0JTYwJTIwcGFzcyUyMCU0MGNyYXNoZnJlZSUyMGFuZCUyMCU0MG5vLXJlY3Vyc2lvbiUyQyUyMHJldHVybmluZyUyMFNvbWUlMjBmb3IlMEElMkYlMkYlMjAlMjAlMjAlMjAlMjAlMjBuJTIwaW4lMjAwLi4lM0QyMCUyMGFuZCUyME5vbmUlMjBvdGhlcndpc2UuJTBBJTJGJTJGJTIwJTIwJTIwNC4lMjBQcmludCUyMHRoZSUyMHJlc3VsdHMlMjBsaXN0ZWQlMjBhdCUyMHRoZSUyMGVuZCUyMG9mJTIwJTYwbWFpbiU2MC4lMEElMEElNDBjcmFzaGZyZWUlMEFmbiUyMHNhZmVfZGl2KGElM0ElMjBpNjQlMkMlMjBiJTNBJTIwaTY0KSUyMC0lM0UlMjBpNjQlMjAlN0IlMEElMjAlMjAlMjAlMjByZXR1cm4lMjBhJTIwJTJGJTIwYiUzQiUwQSU3RCUwQSUwQSU0MGNyYXNoZnJlZSUyMCU0MG5vLXJlY3Vyc2lvbiUwQWZuJTIwZmFjdChuJTNBJTIwaTY0KSUyMC0lM0UlMjBpNjQlMjAlN0IlMEElMjAlMjAlMjAlMjBpZiUyMG4lMjAlM0MlM0QlMjAxJTIwJTdCJTIwcmV0dXJuJTIwMSUzQiUyMCU3RCUwQSUyMCUyMCUyMCUyMHJldHVybiUyMG4lMjAqJTIwZmFjdChuJTIwLSUyMDEpJTNCJTBBJTdEJTBBJTBBZm4lMjBzaG93KG8lM0ElMjBPcHRpb24lM0NpNjQlM0UpJTIwLSUzRSUyMHN0ciUyMCU3QiUwQSUyMCUyMCUyMCUyMG1hdGNoJTIwbyUyMCU3QiUwQSUyMCUyMCUyMCUyMCUyMCUyMCUyMCUyME9wdGlvbiUzQSUzQVNvbWUoeCklMjAlM0QlM0UlMjAlN0IlMjByZXR1cm4lMjBpbnRfdG9fc3RyKHgpJTNCJTIwJTdEJTBBJTIwJTIwJTIwJTIwJTIwJTIwJTIwJTIwT3B0aW9uJTNBJTNBTm9uZSUyMCUzRCUzRSUyMCU3QiUyMHJldHVybiUyMCUyMm5vbmUlMjIlM0IlMjAlN0QlMEElMjAlMjAlMjAlMjAlN0QlMEElN0QlMEElMEFmbiUyMG1haW4oKSUyMC0lM0UlMjBpNjQlMjAlN0IlMEElMjAlMjAlMjAlMjBwcmludGxuKGludF90b19zdHIoc2FmZV9kaXYoMTAlMkMlMjAzKSkpJTNCJTBBJTIwJTIwJTIwJTIwcHJpbnRsbihpbnRfdG9fc3RyKGZhY3QoNSkpKSUzQiUwQSUyMCUyMCUyMCUyMCUyRiUyRiUyMFRPRE8lMjA0JTNBJTIwc2FmZV9kaXYoNyUyQyUyMDApJTJDJTIwYXZlcmFnZSUyMG9mJTIwJTVCMyUyQyUyMDQlMkMlMjA4JTVEJTJDJTIwYXZlcmFnZSUyMG9mJTIwJTVCJTVEJTJDJTBBJTIwJTIwJTIwJTIwJTJGJTJGJTIwJTIwJTIwJTIwJTIwJTIwJTIwJTIwJTIwZmFjdCgyMCklMkMlMjBmYWN0KDIxKSUyQyUyMGZhY3QoLTEpJTBBJTIwJTIwJTIwJTIwcmV0dXJuJTIwMCUzQiUwQSU3RCUwQQ=="
        width="100%" height="420" style="border:0" title="Lab 9 starter"></iframe>
```

## Tasks

1. **Division.** Make `safe_div(a, b) -> Option<i64>` pass `@crashfree`:
   `None` for `b == 0`. There is one more divisor for which `a / b` can
   panic; handle it too.
2. **Average.** Write `average(v: Vec<i64>) -> Option<i64>` that passes
   `@crashfree` (`None` for an empty `Vec`). Which `Vec` operations can
   you use? Try `v.get(i)` with `i` from a local `n`, and a `for` over the
   `Vec` itself.
3. **Factorial.** Make `fact(n) -> Option<i64>` pass `@crashfree` and
   `@no-recursion`: `Some(n!)` for `n` in `0..=20`, `None` otherwise. A
   `while` loop is rejected as unbounded: use a `for` over a range.
4. Print the results of `main` as below.

## Check your work

When you are done, `fid run starter.fd` prints:

```text
10 / 3 = 3
7 / 0 = none
average of 3 4 8 = 5
average of nothing = none
5! = 120
20! = 2432902008176640000
21! = none
(-1)! = none
```

## Questions to hand in

- Your `average` and `fact` need `@wrapping`. What does it change, and why
  is the result still right for every input `fact` accepts?
- Why must `average` return an `Option`? Could a `@crashfree` function
  return `i64` for the empty `Vec`?

## For instructors

[`solution.fd`](solution.fd) is a model answer and
[`expected.txt`](expected.txt) what it prints;
`python3 tests/course/check_labs.py --fid <fid> lab09` checks both. In
the Nov 17 compiler, `v.len()` inside a `@crashfree` function is reported
as a call to `StringBuilder_len`, which is why the solution's `average`
counts the elements itself; when that is fixed, students may use
`v.len()`.
