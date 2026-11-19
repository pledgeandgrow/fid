/* Bubble sort on a static array and on a malloc'd copy. */
#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void bubble_sort(int *a, int n)
{
    int swapped;
    do {
        swapped = 0;
        for (int i = 1; i < n; i++) {
            if (a[i - 1] > a[i]) {
                swap(&a[i - 1], &a[i]);
                swapped = 1;
            }
        }
        n--;
    } while (swapped);
}

void print_array(const int *a, int n)
{
    for (int i = 0; i < n; i++)
        printf(i == 0 ? "%d" : " %d", a[i]);
    printf("\n");
}

int main(void)
{
    int data[] = { 42, 7, 19, 3, 88, -5, 23, 0, 61, 14 };
    int n = sizeof(data) / sizeof(data[0]);

    int *copy = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        copy[i] = data[i] * 2;

    print_array(data, n);
    bubble_sort(data, n);
    print_array(data, n);

    bubble_sort(copy, n);
    print_array(copy, n);
    free(copy);
    return 0;
}
