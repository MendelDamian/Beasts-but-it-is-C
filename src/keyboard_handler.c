#include <ncurses.h>

#include "keyboard_handler.h"

void *keyboard_handler(void *arguments)
{
    if (arguments == NULL)
    {
        return NULL;
    }

    KEYBOARD_HANDLER_ARGS args = *(KEYBOARD_HANDLER_ARGS *)arguments;
    if (args.on_key_pressed == NULL || args.running == NULL)
    {
        return NULL;
    }

    while (*args.running)
    {
        char key = getch();
        args.on_key_pressed(key, args.on_key_pressed_args);
    }

    return NULL;
}
