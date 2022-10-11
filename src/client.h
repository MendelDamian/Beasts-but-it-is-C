#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

#include "conf.h"

void client_main_loop(int sock_fd);

#endif //CLIENT_H
