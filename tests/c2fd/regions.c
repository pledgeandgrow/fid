/* Memory malloc'd and freed in the same block: c2fd makes each lifetime a
 * Fid region (several freed together share one, like an arena). */
#include <stdio.h>
#include <stdlib.h>

/* A scratch array that lives for part of a function. */
int sum_of_squares(int n)
{
    int total = 0;
    int *sq = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        sq[i] = i * i;
    for (int i = 0; i < n; i++)
        total += sq[i];
    free(sq);
    return total;
}

/* Two buffers freed together; a result declared inside is used after. */
int dot_twice(int n)
{
    int *a = calloc(n, sizeof(int));
    int *b = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        a[i] = i + 1;
        b[i] = 2 * (i + 1);
    }
    int dot = 0;
    for (int i = 0; i < n; i++)
        dot += a[i] * b[i];
    free(a);
    free(b);
    return dot * 2;
}

/* A buffer per loop iteration, with an early exit from the loop. */
int first_with_sum(int limit)
{
    int found = -1;
    for (int k = 1; k < 20; k++) {
        int *row = malloc(k * sizeof(int));
        int s = 0;
        for (int i = 0; i < k; i++) {
            row[i] = i * k;
            s += row[i];
        }
        if (s > limit) {
            found = k;
            free(row);
            break;
        }
        free(row);
    }
    return found;
}

/* A char buffer filled and printed. */
void stars(int n)
{
    char *line = malloc(n + 1);
    for (int i = 0; i < n; i++)
        line[i] = '*';
    line[n] = '\0';
    printf("%s\n", line);
    free(line);
}

int main(void)
{
    printf("%d\n", sum_of_squares(10));
    printf("%d\n", dot_twice(4));
    printf("%d\n", first_with_sum(100));
    stars(5);
    stars(2);
    return 0;
}
