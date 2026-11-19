/* C that c2fd cannot translate faithfully: each part needs a manual edit.
 * check.sh only checks the golden translation of this file. */
#include <stdio.h>
#include <stdlib.h>

/* 1. goto for error cleanup: Fid has no goto. */
int parse_digits(const char *s, int *out)
{
    int value = 0;
    if (*s == '\0')
        goto fail;
    while (*s) {
        if (*s < '0' || *s > '9')
            goto fail;
        value = value * 10 + (*s - '0');
        s++;
    }
    *out = value;
    return 1;
fail:
    *out = -1;
    return 0;
}

/* 2. A union read through another member (type punning). */
union bits {
    float f;
    unsigned int u;
};

unsigned int float_bits(float f)
{
    union bits b;
    b.f = f;
    return b.u;
}

/* 3. A pointer walking an int array (no longer a gap: c2fd writes it as an
 * index into the array, see cursors.c). */
int sum_until_zero(const int *p)
{
    int total = 0;
    while (*p != 0)
        total += *p++;
    return total;
}

int main(void)
{
    int v;
    printf("%d %d\n", parse_digits("1234", &v), v);
    printf("%d %d\n", parse_digits("12x4", &v), v);
    printf("%x\n", float_bits(1.0f));
    int data[] = { 3, 4, 5, 0, 9 };
    printf("%d\n", sum_until_zero(data));
    return 0;
}
