/* LG15: for test_at_import_trusted.fd and test_at_import_untrusted_fail.fd.
   toupper is the C library's (ctype.h); the conditional below must drop the
   C++ branch and keep the C one. */
#ifdef __cplusplus
extern "C" {
int cupper_cpp_only(int c);
#else
int toupper(int c);
#endif
#if 0
int cupper_never(int c);
#endif
#ifdef __cplusplus
}
#endif
