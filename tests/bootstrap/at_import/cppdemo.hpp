// LG16: for test_at_import_cpp.fd and test_at_import_cpp_fail.fd. A C++
// header: its extern "C" functions are bound directly (in namespace geo
// they are geo_area in Fides, the C symbol `area`); the class's methods,
// field and destructor and the function with C++ linkage through C++ shims
// (geo_Shape_sides, geo_Shape_get_id, geo_mangled); the abstract class has
// no constructor and the template no instantiation named. cppdemo.cpp
// beside it holds the definitions.
#pragma once
#include <cstdint>

namespace geo {

extern "C" {
int area(int w, int h);
int perimeter(int w, int h);
}

extern "C" int volume(int w, int h, int d);

class Shape {
public:
    virtual int sides() const = 0;
    int id;
};

template <typename T>
T biggest(T a, T b) { return a > b ? a : b; }

int mangled(int x);

}  // namespace geo
