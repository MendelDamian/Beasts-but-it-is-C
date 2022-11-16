#include <unistd.h>

#include "timer.h"

void update_timer(double *elapsed_time, struct timeval *last_update)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    double delta_time = ((now.tv_sec - last_update->tv_sec) * 1000.0) + ((now.tv_usec - last_update->tv_usec) / 1000.0);
    *elapsed_time += delta_time;
    *last_update = now;
}
