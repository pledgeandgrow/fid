// Effect system test: effect handler syntax.
// handle { do Effect.op(args) } with { Effect.op(params) => { body } }
// This should parse and run (handler is a no-op for now).
pf main() -> i64 !IO {
    handle {
        do IO.print("hello from do");
    } with {
        IO.print(s) => {
            print("intercepted: ");
            print(s);
        }
    }
    print("after handle");
    ret 0;
}
4C: Peephole + ELF





