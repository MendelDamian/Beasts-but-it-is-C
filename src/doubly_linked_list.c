#include <stdlib.h>

#include "doubly_linked_list.h"
#include "entity.h"
#include "treasure.h"

DLL *dll_create(size_t size_of_element)
{
    DLL *dll = calloc(1, sizeof(DLL));
    if (dll == NULL)
    {
        return NULL;
    }

    dll->size_of_element = size_of_element;
    return dll;
}

void *dll_push_back(DLL *dll)
{
    if (dll == NULL)
    {
        return NULL;
    }

    NODE *node = malloc(sizeof(NODE));
    if (node == NULL)
    {
        return NULL;
    }

    node->item = calloc(1, dll->size_of_element);
    if (node->item == NULL)
    {
        free(node);
        return NULL;
    }

    node->next = NULL;
    node->prev = dll->tail;

    if (dll->tail != NULL)
    {
        dll->tail->next = node;
    }

    dll->tail = node;

    if (dll->head == NULL)
    {
        dll->head = node;
    }

    return node->item;
}

int dll_size(const DLL *dll)
{
    if (dll == NULL)
    {
        return -1;
    }

    int size = 0;
    NODE *node = dll->head;
    while (node != NULL)
    {
        size++;
        node = node->next;
    }

    return size;
}

int dll_clear(DLL *dll)
{
    if (dll == NULL)
    {
        return -1;
    }

    NODE *node = dll->head;
    while (node != NULL)
    {
        NODE *next = node->next;
        free(node->item);
        free(node);
        node = next;
    }

    dll->head = NULL;
    dll->tail = NULL;
    return 0;
}

void dll_remove(DLL *dll, void *item)
{
    if (dll == NULL || item == NULL)
    {
        return;
    }

    NODE *node = dll->head;
    while (node != NULL)
    {
        if (node->item == item)
        {
            if (node->prev != NULL)
            {
                node->prev->next = node->next;
            }
            else
            {
                dll->head = node->next;
            }

            if (node->next != NULL)
            {
                node->next->prev = node->prev;
            }
            else
            {
                dll->tail = node->prev;
            }

            free(node->item);
            free(node);
            return;
        }

        node = node->next;
    }
}

void *get_entity_at_coords(DLL *dll, COORDS coords)
{
    NODE *node = dll->head;
    while (node != NULL)
    {
        ENTITY *entity = node->item;
        if (entity->position.x == coords.x && entity->position.y == coords.y)
        {
            return node->item;
        }

        node = node->next;
    }

    return NULL;
}

void *get_entity_at_spawn_point(DLL *dll, COORDS coords)
{
    NODE *node = dll->head;
    while (node != NULL)
    {
        ENTITY *entity = node->item;
        if (entity->spawn_point.x == coords.x && entity->spawn_point.y == coords.y)
        {
            return node->item;
        }

        node = node->next;
    }

    return NULL;
}

void *get_treasure_at_coords(DLL *dll, COORDS coords)
{
    NODE *node = dll->head;
    while (node != NULL)
    {
        TREASURE *entity = node->item;
        if (entity->position.x == coords.x && entity->position.y == coords.y)
        {
            return node->item;
        }

        node = node->next;
    }

    return NULL;
}
