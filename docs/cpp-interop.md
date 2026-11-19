# Calling C++ from Fid (LG16)

`@import("lib.hpp")` (or `use lib` with `lib.hpp` beside the program) reads a
C++ header with compiler/cparser.fd and binds it in three ways:

- functions with C linkage (`extern "C" { }`, `extern "C" int f(..)`) are
  called directly, as from a C header; a function of `namespace a::b` is
  `a_b_<name>` in Fides (its C symbol unchanged);
- everything else the header itself declares (not the system headers it
  includes) goes through generated `extern "C"` shims: a C++ file written
  to the temporary directory (`fidcpp_<stem>_<hash>.cpp`, the hash of the
  shims and of the header's own files) that the bindings name as
  `extern "C" link "cpp:<file>" { .. }`;
- what cannot cross is listed in a comment of the bindings
  (`fid parse-header lib.hpp` prints them).

## What is bound

| C++ | Fides |
|-----|-------|
| `int a::f(int)` | `a_f(x: i32) -> i32` |
| an overload | `a_f_2`, `a_f_3`, ... (noted in the bindings) |
| `class a::C` | an opaque handle, `i64` |
| a public constructor | `a_C_new(..) -> i64` (`a_C_new_2` ...); a default one when the class declares none; none for an abstract class |
| the destructor | `a_C_delete(h)` (unless it is private or deleted) |
| a public method | `a_C_m(h, ..)`; a static one without `h` |
| a public field | `a_C_get_x(h)`, `a_C_set_x(h, v)` (no setter for a const field) |
| `template <typename T> T f(T)` | `a_f_i32`, `a_f_f64`, `a_f_str`, ...: the instantiations the header names (`template int f<int>(int);`) and the ones the program names |
| `T&` (T a scalar) | `*T` (`&mut v` at the call); `const T&` by value |
| `std::string`, `std::string_view` | `str` (a string result is a copy the program keeps) |
| a class by pointer, reference or value | a handle; a class returned by value is a new object (delete it) |
| a pointer to a struct or typedef | an address (`i64`) |

A program names a template instantiation by calling it: `a_f_` followed by
one Fides type name per template parameter (`i8` .. `u64`, `f32`, `f64`,
`bool`, `str`), so `shapes_biggest_f64(2.5, 1.5)` instantiates
`shapes::biggest<double>`.

## Exceptions

Every shim catches exceptions. A call that threw returns 0 (or `""`, or a
null handle), and until the next call `<stem>_cpp_failed()` is 1 and
`<stem>_cpp_error()` is the exception's `what()` (`<stem>` is the header's
file name without its extension):

```
@import("cppshim.hpp", trusted)

fn main() -> i64 {
    var q: i32 = shapes_checked_div(7, 0);
    if cppshim_cpp_failed() != 0 {
        println("error: " + cppshim_cpp_error());
    }
    return 0;
}
```

## Building

The shims are C++: build with `ccompile` and a C compiler that also
compiles C++, `zig cc` best (`CC="zig cc"`). The link step compiles each
`cpp:` file with the program (`-iquote .`: a header named by a relative path
is found from the directory the build runs in) and links the C++ library
(libc++ with zig and on macOS, libstdc++ otherwise). `<stem>.cpp` beside the
header, when there is one, is compiled with the shims: the definitions of a
library that is not header-only. For a compiled library, link it as usual.

A free function the header declares without a body is called through a weak
reference: a program that never calls it links without its library, and a
call is the error value `<name> is not defined: link the library that
defines it`.

The interpreter (`fid run`) cannot call C or C++; it type-checks a program
that imports a C++ header.

## Not bound (each noted in the bindings)

Class templates, member templates, operators, nested classes, scoped enums'
values, rvalue references, function pointers, variadic functions, standard
containers (`std::vector<T>` ..), a trailing result type (`auto f() -> T`),
templates with non-type parameters. A base class's methods are called on a
derived object through the base's shims (single inheritance: the handle is
the same address). Native backends (`fid emit`) do not compile the shims;
use `ccompile`.

Tests: tests/bootstrap/at_import/test_at_import_cpp*.fd (`cppshim.hpp`,
`cppshim.cpp`, `cppdemo.hpp`, `cppdemo.cpp`).
