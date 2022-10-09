#ifndef KEYBOARD_HANDLER_H
#define KEYBOARD_HANDLER_H

#include <stdbool.h>

typedef struct keyboard_handler_args_t
{
    void (*on_key_pressed)(char key, void *args);
    void *on_key_pressed_args;
    bool *running;
} KEYBOARD_HANDLER_ARGS;

void *keyboard_handler(void *arguments);

#endif //KEYBOARD_HANDLER_H
