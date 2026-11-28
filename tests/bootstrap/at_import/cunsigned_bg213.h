/* BG213: for ../test_bg213_cparser_unsigned.fd. #define constants whose
   value depends on C's integer types (unsigned operands, the type of a
   literal, the usual arithmetic conversions) and on operators the
   evaluator lacked (?:, comparisons, && and ||, 0b literals). Nothing here
   is compiled or linked. */
#define UDIV (-1u / 2)
#define UNEG (-1u)
#define MASK (~0u)
#define NESTED (MASK / 2)
#define COND (UDIV > 5 ? 1 : 2)
#define CMP (3 < 2)
#define BIN 0b1010
#define HEXWRAP (0xFFFFFFFF + 1)
#define DECBIG (4294967295 + 1)
#define ULLDIV (-1ULL / 2)
#define ULLSHR (~0ULL >> 1)
#define MIXED (-1 < 0u)
#define WIDEMIX (-1LL < 0u)
#define SHIFTU (0x80000000u >> 31)
#define SHIFTS (-8 >> 1)
#define LOGIC (1 && 0 || 2)
#define SKIPDIV (0 && (1 / 0))
#define REM (-7 % 3)
#define UREM (-7u % 10)
#define CHARS ('A' + 1)
#define TERNU (1 ? -1 : 0u)
#define NE (UNEG != 4294967295)
#if -1u > 0
#define PP_UNSIGNED 1
#else
#define PP_UNSIGNED 0
#endif
#if defined(BG213_NOT_DEFINED) && BG213_NOT_DEFINED > 3
#define PP_SKIP 0
#else
#define PP_SKIP 1
#endif
