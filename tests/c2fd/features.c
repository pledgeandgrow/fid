/* A tour of the C that c2fd translates: one function per feature. */
#include <stdio.h>
#include <string.h>

#define SQUARE(x) ((x) * (x))
#define N_PRIMES 8
#define GREETING "hello"
#define MASK 0x0F

typedef int (*binop)(int, int);

typedef struct {
    int x, y;
} Point;

typedef union {
    int i;
    unsigned u;
} Word;

enum flags { F_READ = 1, F_WRITE = 2, F_EXEC = 4, F_ALL = F_READ | F_WRITE | F_EXEC };

static const int primes[N_PRIMES] = { 2, 3, 5, 7, 11, 13, 17, 19 };
int grid[3][4];

static int add(int a, int b) { return a + b; }
static int mul(int a, int b) { return a * b; }

int apply(binop f, int a, int b)
{
    return f(a, b);
}

/* static locals keep their value between calls */
int counter(void)
{
    static int calls = 0;
    calls += 1;
    return calls;
}

void points(void)
{
    Point pts[3];
    for (int i = 0; i < 3; i++) {
        pts[i].x = i;
        pts[i].y = SQUARE(i + 1);
    }
    Point origin = { 0, 0 };
    Point far = { .y = 9, .x = 8 };
    int dist = 0;
    for (int i = 0; i < 3; i++)
        dist += (pts[i].x - origin.x) + (pts[i].y - origin.y);
    printf("points: %d (%d,%d)\n", dist, far.x, far.y);
}

void loops(void)
{
    int i = 0, odd = 0;
    do {
        i++;
        if (i % 2 == 0)
            continue;
        odd += i;
    } while (i < 9);
    printf("odd sum %d\n", odd);

    int found = -1;
    for (int k = 0; k < N_PRIMES; k++) {
        switch (primes[k] % 4) {
        case 1:
            if (primes[k] > 10) {
                found = primes[k];
                break;
            }
            printf("small 4k+1 prime %d\n", primes[k]);
            break;
        case 3:
            break;
        default:
            printf("even prime %d\n", primes[k]);
        }
        if (found > 0)
            break;
    }
    printf("first 4k+1 prime above 10: %d\n", found);

    int a = 0, b = 10;
    while (a < b)
        a += 2, b -= 1;
    printf("a=%d b=%d\n", a, b);

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 4; c++)
            grid[r][c] = r * 4 + c;
    int total = 0;
    for (int r = 2; r >= 0; r--)
        total += grid[r][3 - r];
    printf("anti-diagonal sum %d\n", total);
}

void strings(void)
{
    char buf[32];
    strcpy(buf, GREETING);
    strcat(buf, ", c2fd");
    printf("%s (%d chars)\n", buf, (int)strlen(buf));
    char line[64];
    sprintf(line, "%s|%5d|%-4s|%03d", "id", 42, "ab", 7);
    puts(line);
    const char *words[] = { "alpha", "beta", "gamma" };
    int longest = 0;
    for (int i = 0; i < 3; i++)
        if (strlen(words[i]) > strlen(words[longest]))
            longest = i;
    printf("longest: %s\n", words[longest]);
    putchar('O');
    putchar('K');
    putchar('\n');
}

void numbers(void)
{
    unsigned h = 5381;
    const char *s = "fides";
    while (*s)
        h = h * 33 + (unsigned char)*s++;
    printf("hash %u %x\n", h, h);
    int flags = F_READ | F_EXEC;
    printf("flags %d %d %d\n", flags & MASK, (flags & F_WRITE) != 0, F_ALL);
    double avg = (double)(1 + 2 + 4) / 3;
    printf("avg %.3f %.1f\n", avg, avg * 2);
    int n = 7;
    int m = n++ + 1;
    int q = --n * 2;
    printf("n=%d m=%d q=%d size=%d\n", n, m, q, (int)sizeof(int));
    int x = 3;
    int y = (x > 2 && x++ < 10) ? x : -x;
    printf("x=%d y=%d\n", x, y);
    Word w;
    w.u = 7u;
    printf("word %u\n", w.u);
    printf("apply %d %d\n", apply(add, 2, 3), apply(mul, 4, 5));
    printf("shift %d %d %d\n", 1 << 4, -16 >> 2, ~0 & 0xFF);
}

int main(void)
{
    points();
    loops();
    strings();
    numbers();
    counter();
    counter();
    printf("counter %d\n", counter());
    return 0;
}
