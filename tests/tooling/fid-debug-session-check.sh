#!/bin/bash
# TL5 (docs/roadmap/competitive-roadmap.md): `fid debug` end to end. Pipes
# fixed command lists into `fid debug <file> --echo` (compiler/debugger.fd:
# commands from standard input, one per line; --echo prints each command
# after the prompt, so the transcript reads as a terminal session) and
# compares standard output (the session and the program's own output, in
# order) with the transcripts below:
#   1. break on a function, on a file:line; info breakpoints; list <func>;
#      run to the breakpoint; print a parameter; bt (three frames with
#      arguments and lines); finish (the returned value, back in the
#      caller); print / info locals / info args; next over a loop's lines;
#      delete; watch (stops where the value changes); continue to a
#      breakpoint in main; print a str, a struct, an i64, a bool; a name
#      that does not exist; next / step over println (the program's output
#      in order); continue to the end: exit code 0.
#   2. start (main's first line), step into a call, bt, continue into a
#      division by zero: the session stops in the panicking frame, its
#      variables readable; continue ends it with the panic.
#   3. quit at a breakpoint ends the session; the end of the input does
#      too (no hang: every run has a timeout).
#   FID=release/fid.exe bash tests/tooling/fid-debug-session-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
case "$FID" in /*|?:*) ;; *) FID="$PWD/$FID" ;; esac
export FID_HOME=${FID_HOME:-$PWD}
D=$(mktemp -d ./tests/_fid_debug_check.XXXXXX)
trap 'rm -rf "$D"' EXIT
fail=0
cat > "$D/session.fd" <<'EOF'
struct Point {
    x: i64,
    y: i64,
}

fn square(n: i64) -> i64 {
    var r: i64 = n * n;
    return r;
}

fn sum_to(limit: i64) -> i64 {
    var total: i64 = 0;
    var i: i64 = 1;
    while i <= limit {
        total = total + square(i);
        i = i + 1;
    }
    return total;
}

fn main() -> i64 {
    var name: str = "fides";
    var p: Point = Point { x: 3, y: 4 };
    var a: i64 = p.x;
    var b: i64 = sum_to(a);
    var ok: bool = b > 10;
    println(name);
    println(b);
    if ok {
        println("big");
    }
    return 0;
}
EOF
cat > "$D/panic.fd" <<'EOF'
fn divide(a: i64, b: i64) -> i64 {
    var q: i64 = a / b;
    return q;
}

fn main() -> i64 {
    var x: i64 = 10;
    var y: i64 = 0;
    var r: i64 = divide(x, y);
    println(r);
    return 0;
}
EOF

# run <name> <file> <want exit> <commands...>: the session's stdout in
# $D/<name>.out, compared with $D/<name>.want
run() {
    local name=$1 file=$2 want_rc=$3
    shift 3
    printf '%s\n' "$@" > "$D/$name.in"
    (cd "$D" && timeout 120 "$FID" debug "$file" --echo < "$name.in" > "$name.out" 2> "$name.err")
    local rc=$?
    if [ $rc -eq 124 ]; then echo "FAIL $name: timed out (the session waited for input)"; fail=1; return; fi
    if [ $rc -ne "$want_rc" ]; then echo "FAIL $name: exit $rc, want $want_rc"; fail=1; tail -5 "$D/$name.err"; fi
    if diff -u "$D/$name.want" "$D/$name.out" > "$D/$name.diff"; then
        echo "ok   $name: transcript as expected"
    else
        echo "FAIL $name: transcript differs:"; cat "$D/$name.diff"; fail=1
    fi
}

cat > "$D/session1.want" <<'EOF'
Fides debugger: session.fd
Type "help" for the commands, "run" to start the program.
(fid) break square
Breakpoint 1 at square: session.fd:7
(fid) break session.fd:27
Breakpoint 2 at main: session.fd:27
(fid) info breakpoints
  #1 square (hits: 0)
  #2 main:27 (hits: 0)
(fid) list sum_to
      8 |     return r;
      9 | }
     10 | 
     11 | fn sum_to(limit: i64) -> i64 {
->   12 |     var total: i64 = 0;
     13 |     var i: i64 = 1;
     14 |     while i <= limit {
     15 |         total = total + square(i);
     16 |         i = i + 1;
     17 |     }
     18 |     return total;
(fid) run

Breakpoint 1, square (n = 1) at session.fd:7
7	var r: i64 = n * n;
(fid) print n
n = 1
(fid) bt
#0  square (n = 1) at session.fd:7
#1  sum_to (limit = 3) at session.fd:15
#2  main () at session.fd:25
(fid) finish
Run till exit from square
square returned = 1
sum_to (limit = 3) at session.fd:15
15	total = total + square(i);
(fid) print total
total = 0
(fid) print i
i = 1
(fid) info locals
limit = 3
total = 0
i = 1
(fid) info args
limit = 3
(fid) next
sum_to (limit = 3) at session.fd:16
16	i = i + 1;
(fid) next
sum_to (limit = 3) at session.fd:14
14	while i <= limit {
(fid) delete 1
Breakpoint 1 deleted.
(fid) next
sum_to (limit = 3) at session.fd:15
15	total = total + square(i);
(fid) watch total
Watching total (now 1)
(fid) continue

Watch total: 1 -> 5
sum_to (limit = 3) at session.fd:16
16	i = i + 1;
(fid) continue

Watch total: 5 -> 14
sum_to (limit = 3) at session.fd:16
16	i = i + 1;
(fid) continue

Breakpoint 2, main () at session.fd:27
27	println(name);
(fid) print name
name = "fides"
(fid) print p
p = Point { x: 3, y: 4 }
(fid) print a
a = 3
(fid) print b
b = 14
(fid) print ok
ok = true
(fid) print nosuch
No symbol "nosuch" in the current frame.
(fid) next
fides
main () at session.fd:28
28	println(b);
(fid) step
14
main () at session.fd:29
29	if ok {
(fid) next
main () at session.fd:30
30	println("big");
(fid) continue
big
[program exited with code 0]
EOF
run session1 session.fd 0 "break square" "break session.fd:27" "info breakpoints" "list sum_to" run "print n" bt finish "print total" "print i" "info locals" "info args" next next "delete 1" next "watch total" continue continue continue "print name" "print p" "print a" "print b" "print ok" "print nosuch" next step next continue

cat > "$D/panic1.want" <<'EOF'
Fides debugger: panic.fd
Type "help" for the commands, "run" to start the program.
(fid) start
main () at panic.fd:7
7	var x: i64 = 10;
(fid) step
main () at panic.fd:8
8	var y: i64 = 0;
(fid) step
main () at panic.fd:9
9	var r: i64 = divide(x, y);
(fid) step
divide (a = 10, b = 0) at panic.fd:2
2	var q: i64 = a / b;
(fid) bt
#0  divide (a = 10, b = 0) at panic.fd:2
#1  main () at panic.fd:9
(fid) continue

Program panicked: Division by zero at line 2
divide (a = 10, b = 0) at panic.fd:2
2	var q: i64 = a / b;
(fid) bt
#0  divide (a = 10, b = 0) at panic.fd:2
#1  main () at panic.fd:9
(fid) print a
a = 10
(fid) print b
b = 0
(fid) continue
[program panicked: Division by zero at line 2]
EOF
run panic1 panic.fd 1 start step step step bt continue bt "print a" "print b" continue

cat > "$D/quit1.want" <<'EOF'
Fides debugger: panic.fd
Type "help" for the commands, "run" to start the program.
(fid) break divide
Breakpoint 1 at divide: panic.fd:2
(fid) break nosuch
Function "nosuch" not defined.
(fid) break 99
No code at or after line 99 in panic.fd.
(fid) run

Breakpoint 1, divide (a = 10, b = 0) at panic.fd:2
2	var q: i64 = a / b;
(fid) quit
[session ended]
EOF
run quit1 panic.fd 0 "break divide" "break nosuch" "break 99" run quit

# the end of the input at a stop ends the session (no command left)
cat > "$D/eof1.want" <<'EOF'
Fides debugger: panic.fd
Type "help" for the commands, "run" to start the program.
(fid) start
main () at panic.fd:7
7	var x: i64 = 10;
(fid) 
[session ended]
EOF
run eof1 panic.fd 0 start

[ $fail = 0 ] && echo "fid-debug-session-check: ok" || echo "fid-debug-session-check: FAILED"
exit $fail
