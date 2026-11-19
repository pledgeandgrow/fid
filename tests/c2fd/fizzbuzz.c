/* FizzBuzz: the numbers 1 to 30, with multiples of 3 and 5 replaced. */
#include <stdio.h>

#define LIMIT 30

int main(void)
{
    for (int i = 1; i <= LIMIT; i++) {
        if (i % 15 == 0)
            printf("FizzBuzz\n");
        else if (i % 3 == 0)
            printf("Fizz\n");
        else if (i % 5 == 0)
            printf("Buzz\n");
        else
            printf("%d\n", i);
    }
    return 0;
}
