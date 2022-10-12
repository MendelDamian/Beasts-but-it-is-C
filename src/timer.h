#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>

void update_timer(double *delta_time, struct timeval *last_update);

#endif //TIMER_H
