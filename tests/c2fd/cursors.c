/* Pointers walking arrays: c2fd writes each as an index into its array. */
#include <stdio.h>

/* A parameter moved along the array it points into. */
int sum_until_zero(const int *p)
{
    int total = 0;
    while (*p != 0)
        total += *p++;
    return total;
}

/* Callers point into the middle: the offset becomes a parameter. */
int sum_n(const int *a, int n)
{
    int s = 0;
    for (int i = 0; i < n; i++)
        s += *(a + i);
    return s;
}

/* Recursion on the rest of the array. */
int count_even(const int *a, int n)
{
    if (n == 0)
        return 0;
    return (a[0] % 2 == 0) + count_even(a + 1, n - 1);
}

/* A local pointer from start to end, compared with an end pointer. */
int max_of(const int *a, int n)
{
    const int *p = a;
    const int *end = a + n;
    int best = *p;
    while (p < end) {
        if (*p > best)
            best = *p;
        p++;
    }
    return best;
}

/* Writing through a moving pointer. */
void fill_squares(int *dst, int n)
{
    for (int i = 0; i < n; i++)
        *dst++ = i * i;
}

/* Two pointers from both ends, swapping. */
void reverse(int *a, int n)
{
    int *lo = a;
    int *hi = a + n - 1;
    while (lo < hi) {
        int t = *lo;
        *lo = *hi;
        *hi = t;
        lo++;
        hi--;
    }
}

/* Indexing a pointer that was moved, and a pointer difference. */
int find(const int *a, int n, int x)
{
    const int *p = a + 1;
    p--;
    for (; p < a + n; p++) {
        if (p[0] == x)
            return (int)(p - a);
    }
    return -1;
}

/* A char buffer written through a pointer. */
void upcase(char *s)
{
    while (*s) {
        if (*s >= 'a' && *s <= 'z')
            *s = *s - 'a' + 'A';
        s++;
    }
}

void print_array(const int *a, int n)
{
    for (int i = 0; i < n; i++)
        printf(i == 0 ? "%d" : " %d", a[i]);
    printf("\n");
}

int main(void)
{
    int data[] = { 3, 4, 5, 0, 9 };
    printf("%d\n", sum_until_zero(data));
    printf("%d\n", sum_until_zero(data + 1));

    int v[8] = { 7, 2, 9, 4, 11, 6, 1, 8 };
    printf("%d %d\n", sum_n(v, 8), sum_n(v + 2, 3));
    printf("%d\n", count_even(v, 8));
    printf("%d %d\n", max_of(v, 8), max_of(&v[5], 3));

    int sq[6];
    fill_squares(sq, 6);
    print_array(sq, 6);
    reverse(sq, 6);
    print_array(sq, 6);
    reverse(sq + 1, 4);
    print_array(sq, 6);

    printf("%d %d\n", find(v, 8, 11), find(v, 8, 5));

    int *q = v;
    q += 3;
    *q = 40;
    q[1] = 50;
    *(q + 2) = 60;
    print_array(v, 8);

    char word[] = "hello, world";
    upcase(word);
    printf("%s\n", word);

    /* a local char pointer writing into a buffer */
    char buf[16];
    char *w = buf;
    for (int i = 0; i < 3; i++)
        *w++ = 'x' + i;
    *w = '\0';
    printf("%s %d\n", buf, (int)(w - buf));

    /* a read-only string walk stays a str */
    const char *s = "abc";
    int len = 0;
    while (*s) {
        s++;
        len++;
    }
    printf("%d\n", len);
    return 0;
}
