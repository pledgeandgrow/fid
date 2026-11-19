# Safe Systems Programming with Fid — a university course

> Course material for one term (13 teaching weeks plus an exam period),
> built on the Fid book (`docs/book/`) and the language reference
> (`docs/reference/reference.md`). **Fid** is the language, **Fides** the
> compiler, `fid` the command. Every complete example in the lecture notes
> is run by `tests/course/check_course.py`; the notes were last checked
> against `fid` 0.8.0-unstable (Nov 16, 2026 build) on Nov 17, 2026.

## The course in one paragraph

Systems programming is the craft of writing software whose use of
memory, time and the machine you can account for. This course teaches it
through four contracts a language can make with its programmer, studied
in one compiler whose source is open and written in the language itself:
**memory safety** (who frees a value, and how the compiler knows nobody
still uses it), **effect systems** (what a function may do besides
compute its result), **capability security** (what a program, or a part
of it, may do at all) and **systems programming** proper (what values
look like in memory, and how they meet C and the machine). For each
contract we ask the same three questions: what does it say, where is it
enforced, and where does it stop?

## Audience and prerequisites

- **Audience:** third- or fourth-year undergraduates in computer science
  or computer engineering; first-year master's students. The course also
  works as a graduate seminar if the discussion questions and the
  project carry more weight.
- **Prerequisites:** a data structures course; one course with C or C++
  (pointers, `malloc`/`free`, the stack and the heap); an introduction to
  computer architecture (registers, caches, machine code). A compilers
  or programming-languages course helps for weeks 8–9 but is not
  required.
- **Not prerequisites:** Rust, functional programming, or type theory.
  Comparisons with Rust, C, Go and Java appear in the discussion
  questions and assume nothing beyond the C prerequisite.

## Learning outcomes

By the end of the course a student can:

1. state Fid's ownership rule, borrow rules and escape rules, and use
   them to explain why a given program is accepted or rejected;
2. write programs whose effects and safety properties (`!{}`,
   `@crashfree`, `@no-alloc`, `@deterministic`) are checked by the
   compiler, and explain the soundness and the limits of each check;
3. confine untrusted code with capabilities and `sandbox`, audit a
   program's `unsafe` code, and write a threat model for both;
4. reason about data layout, call C from Fid behind a safe interface, and
   build and compare a program on the interpreter, C, native and
   WebAssembly back ends;
5. read the part of a production compiler that enforces a rule, and
   propose, implement and test a change to it.

## Schedule

Two 90-minute lectures and one 2-hour lab per week. Each week's notes give
the reading to do *before* the first lecture, the concepts, complete
programs with the output they produce, exercises (the lab's material)
and a discussion question (the second lecture's last 20 minutes).

| Week | Notes | Theme | Book | Reference |
|---|---|---|---|---|
| 1 | [A language with a checked contract](week01-intro.md) | introduction | ch. 1–2 | §1, §3.1, §3.4 |
| 2 | [Types: structs, enums, generics and caps](week02-types.md) | foundations | ch. 3 | §2.2–2.5, §6, §7 |
| 3 | [Failure: panics and errors as values](week03-errors.md) | foundations | ch. 6 | §4.5, §5.5, §8.2 |
| 4 | [Collections, strings, closures and modules](week04-data.md) | foundations | ch. 3 | §2.9, §4.10, §13 |
| 5 | [Memory safety I: ownership](week05-ownership.md) | memory safety | ch. 4 | §8.1, §8.3, §8.7 |
| 6 | [Memory safety II: borrowing and second-class references](week06-borrowing.md) | memory safety | ch. 4 | §8.4 |
| 7 | [Memory safety III: regions, unsafe code and its tools](week07-regions-unsafe.md) | memory safety | ch. 4 | §8.5, §8.6 |
| 8 | [Effect systems I: inference, declarations, polymorphism](week08-effects.md) | effect systems | ch. 5 | §9.1, §9.4 |
| 9 | [Effect systems II: proving what a function cannot do](week09-safety-annotations.md) | effect systems | ch. 5 | §11 |
| 10 | [Capability security: what a program may do at all](week10-capabilities.md) | capability security | ch. 5 | §9.2, §9.3, §11 |
| 11 | [Systems programming I: data in memory](week11-layouts.md) | systems programming | — | §3, §11; proposal 0004 |
| 12 | [Systems programming II: C, machine code and the back ends](week12-ffi-backends.md) | systems programming | ch. 7, 10 | §2.10, §8.5 |
| 13 | [Concurrency: the four themes together, and the project](week13-concurrency-project.md) | synthesis | ch. 8, 9 | §10 |
| — | Exam period: project demos and the final exam | | | |

The midterm is in week 7, after the memory-safety block; the project
proposal is due at the end of week 9 and the project in the exam period.

## Grading

| Component | Weight | What |
|---|---|---|
| Weekly exercises | 30% | the best 10 of 12 weekly sets (weeks 1–12), graded on correctness and on the written explanations; programs must pass `fid check` and run |
| Midterm exam | 20% | week 7, written: weeks 1–6. Predict what the compiler accepts, rejects or prints for short programs, and explain why with the rules |
| Final project | 35% | week 13's brief: code with tests (20%), report (10%), demo (5%) |
| Final exam | 15% | written, cumulative, weighted to weeks 7–13 |

Late exercises lose 10% per day, up to three days. Grading answers that
involve compiler output use the course's pinned `fid` build (below); a
student who finds that the compiler disagrees with the notes or the
reference, and shows it with a minimal program, earns a bonus of up to
5% of the course grade per confirmed report (the project keeps such
reports in `docs/reference/known-bugs.md` and the reference's §15).

**Collaboration:** discuss ideas freely, write your code and answers
alone (projects: within the team). Cite every source, including AI
assistants, and say what you used them for.

## Tools

- Install `fid` (book chapter 1): the Windows installer, or a build from
  source with `scripts/bootstrap.sh`. Pin one build for the term and give
  every student the same one.
- A C compiler for `fid ccompile`, `--leaks` and `--checked-alloc`
  (weeks 5, 7, 12): `zig cc` works on every platform.
- For week 12's Linux targets on Windows: WSL (and `qemu-user` for
  AArch64 and RISC-V). For WebAssembly: Node.js.
- An editor with `fid lsp` (book chapter 7).

## Checking the notes

The notes follow the book's conventions: a ```` ```fides ```` block whose
first line is `// name.fd` is a complete file; a block whose first line
starts with `$ ` is a terminal session, and what follows each command is
what it printed (`...` stands for lines that depend on the build); a file
whose name ends in `_fail.fd` is rejected by the compiler.

```bash
python3 tests/course/check_course.py --fid release/fid.exe          # structure + every week
python3 tests/course/check_course.py --fid release/fid.exe --native docs/course/week11-layouts.md
python3 tests/course/check_course.py --structure-only               # no compiler needed
```

The script checks the course's structure (this syllabus links every week;
12–14 weeks; every week has exercises, a complete example and a session
that runs `fid`; the four themes are covered), then runs every week
through `tests/book/check_book.py`. `--native` also builds each `fid run`
example with `fid ccompile` and `fid emit` and requires the same output.
Run it whenever `fid` changes, before each term, and fix the notes, the
example or the compiler when it fails. The labs have their own check:

```bash
python3 tests/course/check_labs.py --fid release/fid.exe               # every lab's files and solution
python3 tests/course/check_labs.py --fid release/fid.exe --playground  # + the playground's compiler
python3 tests/course/check_labs.py --structure-only                    # links and layout only
```

## Notes for instructors

- **Where the notes come from.** Each week builds on a book chapter and
  the reference; the notes add the *why* (the design space, the
  trade-off), the course's own examples, and the edges where the
  implementation stops. When the reference changes, check the week that
  cites the section.
- **Known gaps, kept on purpose.** Several weeks show a limit of the
  compiler as it is (week 5: `drop` methods run at function exit; week 8:
  a nested `do` always resumes; week 9: `@deterministic` misses float
  casts; week 6: one borrow message prints `line 0`). They are good
  exercise and project material. When a limit is fixed, the session
  that shows it fails in `check_course.py`: update the text with the fix.
- **Labs.** Each week has a two-hour lab in [`labs/`](labs/README.md):
  a starter program with its tasks (the week's exercises 1–2, the rest
  are homework), a link that opens the starter in the browser playground
  and an embed for a course page, and a model solution whose output
  `tests/course/check_labs.py` checks (see [labs/README.md](labs/README.md)).
- **Shortening to 12 weeks.** Merge weeks 2 and 4 (cover modules and
  closures in week 2's second lecture), and keep the rest.
- **Lengthening to 14 weeks.** Split week 13: concurrency alone, then a
  week of project work with a lecture on the compiler's architecture
  (`docs/compiler-internals.md`, `docs/reference/architecture.md`).

## Further reading

- Proposals 0001 (value semantics), 0003 (memory model), 0004 (ownership
  and layout), 0005 (region arenas) in `docs/proposals/`.
- `docs/reference/safety-rules.md` (every safety rule with its negative
  test); `docs/verification/` (what is proved about the compiler).
- Background: Tofte and Talpin on region inference; Wadler, "Linear types
  can change the world!"; Lucassen and Gifford on effect systems;
  Plotkin and Pretnar on effect handlers; Miller, "Robust Composition"
  (object capabilities); Denning on information flow; Jung et al.,
  RustBelt (the semantics of ownership and unsafe code).
