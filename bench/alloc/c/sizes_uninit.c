/* alloc/sizes_uninit: alloc/sizes with blocks whose bytes are not zeroed in
 every variant (fid: __p_malloc_uninit, RT8), so fid compares with malloc and
 mi_malloc like for like; sizes itself keeps the zeroing (alloc.h). */
#define UNINIT 1
#include "sizes.c"
