// LG16: the definitions of cppdemo.hpp (compiled with its C++ shims, so a
// `ccompile` of test_at_import_cpp.fd links).
#include "cppdemo.hpp"

namespace geo {

extern "C" int area(int w, int h) { return w * h; }
extern "C" int perimeter(int w, int h) { return 2 * (w + h); }
extern "C" int volume(int w, int h, int d) { return w * h * d; }

int mangled(int x) { return x * 3; }

}  // namespace geo
