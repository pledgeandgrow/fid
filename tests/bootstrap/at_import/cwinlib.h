/* LG15: for test_at_import_native_win.fd. A function of the C library
   (msvcrt.dll) and one of kernel32.dll, declared as the SDK's headers
   declare them (annotation macros not defined here) and with no library
   named: a native Windows build (fid emit, compiler/pe_ir.fd) imports each
   from the DLL that exports it (compiler/cparser.fd cparse_system_dll_of).
   Neither is in pe_ir's fixed import lists. */
_Check_return_ _ACRTIMP size_t __cdecl strspn(_In_z_ const char* _Str, _In_z_ const char* _Control);
WINBASEAPI int WINAPI lstrlenA(_In_opt_ const char* lpString);
