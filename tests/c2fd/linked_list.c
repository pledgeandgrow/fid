/* A singly linked list of ints built with malloc and freed with free. */
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int value;
    struct Node *next;
} Node;

/* Add a value at the front; returns the new head. */
Node *push_front(Node *head, int value)
{
    Node *n = malloc(sizeof(Node));
    if (n == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    n->value = value;
    n->next = head;
    return n;
}

int length(const Node *head)
{
    int count = 0;
    while (head != NULL) {
        count++;
        head = head->next;
    }
    return count;
}

int sum(const Node *head)
{
    int total = 0;
    for (const Node *p = head; p != NULL; p = p->next)
        total += p->value;
    return total;
}

Node *reverse(Node *head)
{
    Node *prev = NULL;
    while (head) {
        Node *next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    return prev;
}

void print_list(const Node *head)
{
    printf("[");
    for (const Node *p = head; p; p = p->next) {
        printf("%d", p->value);
        if (p->next)
            printf(", ");
    }
    printf("]\n");
}

void free_list(Node *head)
{
    while (head != NULL) {
        Node *next = head->next;
        free(head);
        head = next;
    }
}

int main(void)
{
    Node *list = NULL;
    for (int i = 1; i <= 5; i++)
        list = push_front(list, i * i);
    print_list(list);
    printf("length %d, sum %d\n", length(list), sum(list));
    list = reverse(list);
    print_list(list);
    free_list(list);
    return 0;
}
