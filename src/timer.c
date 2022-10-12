#include <unistd.h>

#include "timer.h"

void update_timer(double *delta_time, struct timeval *last_update)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    *delta_time += ((now.tv_sec - last_update->tv_sec) * 1000) + ((now.tv_usec - last_update->tv_usec) / 1000);
    *last_update = now;
}
