/* BG72, BG95: for ../test_cparser_casts.fd. The #define constants cast
   their value to a narrower or unsigned type; the functions take wchar_t
   and long double, whose widths depend on the system. Nothing here is
   compiled or linked. */
#define BG_ALL_ONES ((uint32_t)-1)
#define BG_BYTE ((unsigned char)0x1FF)
#define BG_NEG_CHAR ((signed char)200)
#define BG_SHORT ((short)0x18000)
#define BG_U16 ((uint16_t)70000)
#define BG_WIDE ((int64_t)-1)
#define BG_PLAIN (-1)
int bg_wide(wchar_t c);
long double bg_ld(long double x);
double bg_d(double x);
