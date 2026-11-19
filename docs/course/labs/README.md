# Lab exercises

One two-hour lab per week of the [course](../README.md). Each lab is a
directory with a lab sheet, a starter program that opens in the browser
playground, and a model solution whose output is checked.

| Lab | Week | Topic | Starts as | Runs in the playground |
|---|---|---|---|---|
| [1 — The longest Collatz chain](lab01-collatz/README.md) | [1](../week01-intro.md) | integers, loops, overflow panics | runs | yes |
| [2 — An evaluator that cannot panic](lab02-expr/README.md) | [2](../week02-types.md) | enums, exhaustive `match`, `Result` | runs | yes |
| [3 — A calculator that reports overflow](lab03-rpn/README.md) | [3](../week03-errors.md) | errors as values, checked arithmetic | runs | yes |
| [4 — Word counts, the top n, and a closure](lab04-words/README.md) | [4](../week04-data.md) | `HashMap`, ordering, closures | runs | yes |
| [5 — A handle that must be closed exactly once](lab05-linear/README.md) | [5](../week05-ownership.md) | `@linear`, value semantics | rejected | no |
| [6 — A graph without references between nodes](lab06-graph/README.md) | [6](../week06-borrowing.md) | aliasing `&mut`, indexes as edges | rejected | yes |
| [7 — Taking the unsafe code out of a ring buffer](lab07-ring/README.md) | [7](../week07-regions-unsafe.md) | `unsafe`, `fid audit`, regions | runs | no |
| [8 — A pure core, an effectful shell, and closures](lab08-effects/README.md) | [8](../week08-effects.md) | `!{}`, effects of closures | runs | no |
| [9 — Functions the compiler proves cannot crash](lab09-crashfree/README.md) | [9](../week09-safety-annotations.md) | `@crashfree`, `@no-recursion` | rejected | yes |
| [10 — Running untrusted code in a sandbox](lab10-sandbox/README.md) | [10](../week10-capabilities.md) | `sandbox`, `@secret`, `@declassify` | rejected | yes |
| [11 — Layouts in memory and on the wire](lab11-wire/README.md) | [11](../week11-layouts.md) | `@repr(C)`, `@packed`, encoding | runs | yes |
| [12 — A safe wrapper around C](lab12-ffi/README.md) | [12](../week12-ffi-backends.md) | `extern "C"`, `unsafe`, null pointers | runs | no |
| [13 — Threads that share nothing, and one counter that is shared](lab13-threads/README.md) | [13](../week13-concurrency-project.md) | race check, mutex, `parallel` | rejected | no |

"Starts as *rejected*" means that the first task is to read a compiler
error: `fid check starter.fd` fails on purpose.

## For students

Open the lab sheet, then either click **Open the starter in the
playground** (nothing to install: the editor and compiler run in the
browser, and **Share** gives you a link to your version) or copy
`starter.fd` and use `fid`. The sheet lists the tasks, what the finished
program prints, and the questions to hand in.

The playground's compiler is the WebAssembly build of the bootstrap
compiler (`fidesc`), which is older than `fid`: it does not have every
check of the course (`@linear`, the effects of closures, the borrow and
race checks) and differs on a few run-time semantics. Every sheet says
whether the lab's solution runs there. Where it does not, use the
playground to read and edit, and `fid` to check and run; the sheet says
why. The compiler errors the sheets describe are `fid`'s.

## For instructors

Each lab directory holds:

| File | What it is |
|---|---|
| `README.md` | the lab sheet: tasks, playground link and embed, what the finished program prints, questions |
| `starter.fd` | what students start from |
| `solution.fd` | a model solution (give it out after the deadline) |
| `expected.txt` | what the solution prints |
| `*_fail.fd` | a program the lab shows being rejected (labs 8 and 12) |

The playground link is the playground's share form: everything after `#`
is `btoa(encodeURIComponent(code))` of `starter.fd`, so the code travels in
the link and no server stores it. The embed is the same hash on
`embed.html` (`docs/playground/README.md`), for an `<iframe>` in a course
page or a learning-management system. The embed works once `embed.html`
is served next to the playground's compiler on the website (the step left
in `docs/playground/README.md`); the full playground link works today.
`docs/playground/embed.js` can also turn a `<pre class="fides-playground">`
in your own page into a runnable editor.

Lines starting `//!` in a `.fd` file tell the check script how to check
it: `check: runs` or `check: rejected` (with `error:` text the message
must contain), `playground: yes|no`, `run: ccompile` (lab 12, which calls
C), `env: NAME=VALUE`, `forbid: <word>` (lab 7: no `unsafe` in the
solution).

## Checking the labs

```bash
python3 tests/course/check_labs.py --fid release/fid.exe               # every lab
python3 tests/course/check_labs.py --fid release/fid.exe lab05 lab06   # some labs
python3 tests/course/check_labs.py --fid release/fid.exe --playground  # + the playground's compiler
python3 tests/course/check_labs.py --structure-only                    # links and layout only
python3 tests/course/check_labs.py --links                             # print each starter's links
```

The script checks that there is one lab per week, that every lab sheet's
playground links decode to its `starter.fd` exactly, that each sheet shows
`expected.txt` and says correctly whether the playground runs the
solution; then that each starter compiles (and does not already print the
answer) or is rejected with the right error, that each `_fail.fd` is
rejected, and that each solution prints `expected.txt`. `--playground`
runs the solutions marked `playground: yes` through the playground's own
compiler (`tests/course/playground_run.mjs`: Node, and the website
checkout's `public/playground/fidesc.wasm` beside this repository). After
changing a starter, run `--links` and paste the new links into its sheet.
Run the check whenever `fid` changes and before each term.
