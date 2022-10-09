#include <sys/time.h>
#include <ncurses.h>

#include "timer.h"

void *timer(void *arguments)
{
    if (arguments == NULL)
    {
        return NULL;
    }

    TIMER_ARGS *args = (TIMER_ARGS *)arguments;
    if (args->delta_time == NULL || args->running == NULL || args->pressed_key == NULL)
    {
        return NULL;
    }

    struct timeval last_update, now;
    gettimeofday(&last_update, NULL);

    while (*args->running)
    {
        gettimeofday(&now, NULL);
        *args->pressed_key = getch();
        *args->delta_time += ((now.tv_sec - last_update.tv_sec) * 1000) + ((now.tv_usec - last_update.tv_usec) / 1000);
        last_update = now;
    }

    return NULL;
}
