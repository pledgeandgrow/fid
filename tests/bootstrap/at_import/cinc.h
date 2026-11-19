/* LG15: for test_at_import_include.fd and ../test_cparser_include.fd.
   The includes below are followed (compiler/cparser.fd cp_include):
   quoted beside this header, angle-bracketed in the system directories and
   then beside this header, a header in a subdirectory, one included twice
   (read once: its include guard), one that includes itself under a longer
   path every time (read once: cinc_sub/../cinc_deep.h is cinc_deep.h), a
   chain 17 includes deep (cinc_chain/: the 17th is past the depth limit)
   and one that exists nowhere (noted, left out). The declarations carry
   MSVC's, glibc's and Apple's annotation macros, none of them defined
   here. */
#pragma once
#include "cinc_types.h"
#include <cinc_sys_lg15.h>
#include "cinc_types.h"
#include "cinc_sub/cinc_leaf.h"
#include "cinc_deep.h"
#include "cinc_chain/c01.h"
#include <cinc_no_such_header.h>

_ACRTIMP cinc_word __cdecl cinc_twice(_In_ cinc_word x);
extern int cinc_add(int a, int b) __THROW __wur __nonnull((1));
_Check_return_ _CRTIMP int __cdecl cinc_neg(_In_reads_(1) int x);
int cinc_mac(int * _Nonnull p) __API_AVAILABLE(macos(10.0));
_Check_return_ _ACRTIMP int __cdecl toupper(_In_ int c) __THROW __wur;
int select(int n);
