#include <ncurses.h>
#include <unistd.h>

#include "server.h"

int main(int argc, char *argv[])
{
    server_main_loop();
    return 0;
}
