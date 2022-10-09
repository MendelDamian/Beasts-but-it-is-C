#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>

typedef struct timer_args_t
{
    double *delta_time;
    bool *running;
    char *pressed_key;
} TIMER_ARGS;

void *timer(void *arguments);

#endif //TIMER_H
