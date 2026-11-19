/* A fixed-capacity stack of ints in a struct, used to check brackets. */
#include <stdio.h>

#define CAPACITY 64

typedef struct {
    int items[CAPACITY];
    int top;
} Stack;

void stack_init(Stack *s)
{
    s->top = 0;
}

int stack_empty(const Stack *s)
{
    return s->top == 0;
}

int stack_push(Stack *s, int value)
{
    if (s->top == CAPACITY)
        return 0;
    s->items[s->top++] = value;
    return 1;
}

int stack_pop(Stack *s)
{
    return s->items[--s->top];
}

int stack_peek(const Stack *s)
{
    return s->items[s->top - 1];
}

/* 1 when every bracket in text is closed in the right order. */
int balanced(const char *text)
{
    Stack s;
    stack_init(&s);
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        if (c == '(' || c == '[' || c == '{') {
            stack_push(&s, c);
        } else if (c == ')' || c == ']' || c == '}') {
            if (stack_empty(&s))
                return 0;
            char open = stack_pop(&s);
            if ((c == ')' && open != '(') || (c == ']' && open != '[') ||
                (c == '}' && open != '{'))
                return 0;
        }
    }
    return stack_empty(&s);
}

int main(void)
{
    Stack s;
    stack_init(&s);
    for (int i = 1; i <= 5; i++)
        stack_push(&s, i * 10);
    printf("peek: %d\n", stack_peek(&s));
    while (!stack_empty(&s))
        printf("pop: %d\n", stack_pop(&s));

    const char *tests[] = { "(a[b]{c})", "(]", "((x)", "{[()()]}" };
    for (int i = 0; i < 4; i++)
        printf("%-10s %s\n", tests[i], balanced(tests[i]) ? "balanced" : "unbalanced");
    return 0;
}
