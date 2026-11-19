/* LG15: includes itself under a longer path each time (cinc_sub/../). No
   guard on purpose: cparser.fd takes the path's `dir/..` out
   (cp_norm_path), so the include is this file again and is not read. */
#define CINC_DEEP 1
int cinc_deep_fn(int x);
#include "cinc_sub/../cinc_deep.h"
