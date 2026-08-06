// Hello, Pledge! — first test program

pf main() {
    print("Hello, Pledge!");
    pv x = 42;
    pv y = x + 8;
    print(y);
    if y > 40 {
        print("y is big");
    } else {
        print("y is small");
    }
}
