// LG16: the definitions of cppshim.hpp that are not in the header. The
// shims of a C++ header include <stem>.cpp beside it when there is one.
#include "cppshim.hpp"

namespace shapes {

int from_cpp_file(int x) { return x + 1000; }

}  // namespace shapes
