/* Reverse strings in place, and count vowels. */
#include <stdio.h>
#include <string.h>

void reverse(char *s)
{
    int n = strlen(s);
    for (int i = 0; i < n / 2; i++) {
        char tmp = s[i];
        s[i] = s[n - 1 - i];
        s[n - 1 - i] = tmp;
    }
}

int count_vowels(const char *s)
{
    int count = 0;
    while (*s) {
        switch (*s) {
        case 'a': case 'e': case 'i': case 'o': case 'u':
            count++;
            break;
        default:
            break;
        }
        s++;
    }
    return count;
}

int is_palindrome(const char *s)
{
    int i = 0;
    int j = strlen(s) - 1;
    while (i < j) {
        if (s[i] != s[j])
            return 0;
        i++;
        j--;
    }
    return 1;
}

int main(void)
{
    char word[] = "hello, world";
    printf("%s -> ", word);
    reverse(word);
    printf("%s\n", word);
    printf("vowels in \"%s\": %d\n", "programming", count_vowels("programming"));
    const char *candidates[] = { "level", "fides", "racecar", "ab" };
    for (int i = 0; i < 4; i++) {
        printf("%s: %s\n", candidates[i], is_palindrome(candidates[i]) ? "yes" : "no");
    }
    return 0;
}
