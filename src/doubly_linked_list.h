#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "coordinate.h"

typedef struct doubly_linked_list_t
{
    struct node_t *head;
    struct node_t *tail;
    size_t size_of_element;
} DLL;

typedef struct node_t
{
    void *item;
    struct node_t *next;
    struct node_t *prev;
} NODE;

DLL *dll_create(size_t size_of_element);

void *dll_push_back(DLL *dll);
int dll_size(const DLL *dll);
int dll_clear(DLL *dll);
void dll_remove(DLL *dll, void *item);
void *get_entity_at_coords(DLL *dll, COORDS coords);
void *get_treasure_at_coords(DLL *dll, COORDS coords);

#endif //LINKED_LIST_H
