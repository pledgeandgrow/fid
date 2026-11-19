/* LG15: a header in a subdirectory of the header imported; its quoted
   include is looked for beside it first (../ reaches cinc_types.h). */
#include "../cinc_types.h"
#define CINC_LEAF 7
cinc_word cinc_leaf_fn(cinc_word w);
