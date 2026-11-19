/* Count the lines, words and bytes of a file, like wc. */
#include <stdio.h>
#include <ctype.h>

int main(int argc, char **argv)
{
    const char *path = argc > 1 ? argv[1] : "words.txt";
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "cannot open %s\n", path);
        return 1;
    }
    int c;
    int in_word = 0;
    long lines = 0, words = 0, bytes = 0;
    long longest = 0, current = 0;
    while ((c = fgetc(f)) != EOF) {
        bytes++;
        if (c == '\n')
            lines++;
        if (isspace(c)) {
            if (current > longest)
                longest = current;
            current = 0;
            in_word = 0;
        } else {
            current++;
            if (!in_word) {
                in_word = 1;
                words++;
            }
        }
    }
    fclose(f);
    printf("%7ld %7ld %7ld %s\n", lines, words, bytes, path);
    printf("longest word: %ld\n", longest);
    return 0;
}
