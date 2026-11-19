/* LG15: for test_at_import_native_libc.fd. Functions of every C library
   (strspn and strcspn of string.h), declared with glibc's attribute
   macros (not defined here) and no library named: a native build links
   them with no `link` (pe_ir: msvcrt.dll, cparser.fd
   cparse_system_dll_of; nat_ir on Linux: libc.so.6, the dynamic import of
   ng_ldyn). Neither is in pe_ir's fixed import lists. */
extern size_t strspn(const char* __s, const char* __accept) __THROW __attribute_pure__ __nonnull((1, 2));
extern size_t strcspn(const char* __s, const char* __reject) __THROW __attribute_pure__ __nonnull((1, 2));
