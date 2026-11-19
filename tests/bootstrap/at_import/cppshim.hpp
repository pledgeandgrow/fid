// LG16: for test_at_import_cpp_shim.fd (and _cpp_shim_fail.fd). A
// header-only C++ library: free functions with C++ linkage, an overload, a
// reference parameter, std::string in and out, a function that throws, a
// class with a constructor, methods, a static method and a public field, an
// abstract class, a function template (one instantiation named here, others
// by the program) and a class template (not bound). The definitions of
// cppshim.cpp beside it are compiled with the shims.
#pragma once
#include <string>
#include <stdexcept>

namespace shapes {

inline int scale(int x, int k) { return x * k; }
inline double half(double x) { return x / 2.0; }
inline void bump(int& x) { x = x + 1; }
inline int checked_div(int a, int b) {
    if (b == 0) {
        throw std::runtime_error("division by zero");
    }
    return a / b;
}
inline std::string greet(const std::string& who) { return "hello " + who; }
inline int twice(int x) { return 2 * x; }
inline double twice(double x) { return 2.0 * x; }
int from_cpp_file(int x);

class Counter {
public:
    explicit Counter(int start) : n(start), hidden(0) {}
    Counter(int start, int step) : n(start * step), hidden(0) {}
    ~Counter() {}
    int get() const { return n; }
    void add(int k) { n += k; }
    bool is_zero() const noexcept { return n == 0; }
    std::string label() const { return "counter " + std::to_string(n); }
    void fail() { throw std::logic_error("counter failed"); }
    static int zero() { return 0; }
    int n;
private:
    int hidden;
};

class Shape {
public:
    virtual ~Shape() {}
    virtual int sides() const = 0;
};

class Square : public Shape {
public:
    Square() {}
    int sides() const override { return 4; }
};

inline int count_sides(const Shape& s) { return s.sides(); }

template <typename T>
T biggest(T a, T b) { return a > b ? a : b; }

template int biggest<int>(int, int);

template <typename T>
class Box {
public:
    T value;
};

}  // namespace shapes
