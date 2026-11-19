/*
 * Real-world input for c2fd: four functions of musl libc, copied unchanged
 * (only the #defines below rename them, so that they do not clash with the
 * C library this file is linked with), and a small driver.
 *
 * Sources (musl git, master):
 *   https://git.musl-libc.org/cgit/musl/plain/src/stdlib/atoi.c
 *   https://git.musl-libc.org/cgit/musl/plain/src/string/strcmp.c
 *   https://git.musl-libc.org/cgit/musl/plain/src/string/strncmp.c
 *   https://git.musl-libc.org/cgit/musl/plain/src/prng/rand.c
 *
 * musl as a whole is licensed under the following standard MIT license:
 *
 * Copyright (c) 2005-2020 Rich Felker, et al.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#define atoi musl_atoi
#define strcmp musl_strcmp
#define strncmp musl_strncmp
#define srand musl_srand
#define rand musl_rand

#include <stdio.h>

/* ---- src/stdlib/atoi.c ---- */
#include <stdlib.h>
#include <ctype.h>

int atoi(const char *s)
{
	int n=0, neg=0;
	while (isspace(*s)) s++;
	switch (*s) {
	case '-': neg=1;
	case '+': s++;
	}
	/* Compute n as a negative number to avoid overflow on INT_MIN */
	while (isdigit(*s))
		n = 10*n - (*s++ - '0');
	return neg ? n : -n;
}

/* ---- src/string/strcmp.c ---- */
#include <string.h>

int strcmp(const char *l, const char *r)
{
	for (; *l==*r && *l; l++, r++);
	return *(unsigned char *)l - *(unsigned char *)r;
}

/* ---- src/string/strncmp.c ---- */
#include <string.h>

int strncmp(const char *_l, const char *_r, size_t n)
{
	const unsigned char *l=(void *)_l, *r=(void *)_r;
	if (!n--) return 0;
	for (; *l && *r && n && *l == *r ; l++, r++, n--);
	return *l - *r;
}

/* ---- src/prng/rand.c ---- */
#include <stdlib.h>
#include <stdint.h>

static uint64_t seed;

void srand(unsigned s)
{
	seed = s-1;
}

int rand(void)
{
	seed = 6364136223846793005ULL*seed + 1;
	return seed>>33;
}

/* ---- driver (not from musl) ---- */
int main(void)
{
	const char *numbers[] = { "42", "  -17", "+8x", "007", "", "-2147483648" };
	for (int i = 0; i < 6; i++)
		printf("atoi(\"%s\") = %d\n", numbers[i], atoi(numbers[i]));

	printf("strcmp: %d %d %d\n", strcmp("apple", "apple"),
	       strcmp("apple", "apply"), strcmp("b", "abc"));
	printf("strncmp: %d %d %d\n", strncmp("apple", "apply", 4),
	       strncmp("apple", "apply", 5), strncmp("x", "y", 0));

	srand(1);
	for (int i = 0; i < 5; i++)
		printf("rand %d: %d\n", i, rand());
	srand(2026);
	printf("rand after srand(2026): %d\n", rand());
	return 0;
}
