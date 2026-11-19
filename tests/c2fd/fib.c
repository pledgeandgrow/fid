/* Recursive Fibonacci, with a memoised version to compare. */
#include <stdio.h>

static long long memo[64];

/* The naive definition: exponential time. */
long fib(int n)
{
    if (n < 2)
        return n;
    return fib(n - 1) + fib(n - 2);
}

long long fib_memo(int n)
{
    if (n < 2)
        return n;
    if (memo[n] != 0)
        return memo[n];
    memo[n] = fib_memo(n - 1) + fib_memo(n - 2);
    return memo[n];
}

int main(void)
{
    for (int i = 0; i <= 20; i++) {
        printf("fib(%d) = %ld\n", i, fib(i));
    }
    printf("fib_memo(50) = %lld\n", fib_memo(50));
    return 0;
}
