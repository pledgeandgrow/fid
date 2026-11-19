/* A traffic light as an enum and a switch-based state machine. */
#include <stdio.h>

typedef enum { RED, RED_AMBER, GREEN, AMBER, OFF = 9 } Light;

enum event { TICK, FAULT, RESET };

static const char *light_name(Light l)
{
    switch (l) {
    case RED: return "red";
    case RED_AMBER: return "red+amber";
    case GREEN: return "green";
    case AMBER: return "amber";
    default: return "off";
    }
}

Light step(Light l, enum event e)
{
    if (e == FAULT)
        return OFF;
    switch (l) {
    case RED:
        l = RED_AMBER;
        break;
    case RED_AMBER:
        l = GREEN;
        break;
    case GREEN:
        l = AMBER;
        break;
    case AMBER:
        l = RED;
        break;
    case OFF:
        if (e == RESET)
            l = RED;
        break;
    }
    return l;
}

int main(void)
{
    enum event script[] = { TICK, TICK, TICK, TICK, TICK, FAULT, TICK, RESET, TICK };
    int n = sizeof(script) / sizeof(script[0]);
    Light l = RED;
    int greens = 0;
    printf("start: %s\n", light_name(l));
    for (int i = 0; i < n; i++) {
        l = step(l, script[i]);
        if (l == GREEN)
            greens++;
        printf("%d: %s (%d)\n", i, light_name(l), l);
    }
    printf("green %d times\n", greens);
    return 0;
}
