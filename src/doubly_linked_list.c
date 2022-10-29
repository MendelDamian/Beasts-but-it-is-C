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
    pthread_mutex_init(&dll->mutex, NULL);
    return dll;
}

void dll_destroy(DLL *dll)
{
    if (dll == NULL)
    {
        return;
    }

    dll_clear(dll);
    pthread_mutex_destroy(&dll->mutex);
    free(dll);
}

void *dll_push_back(DLL *dll)
{
    if (dll == NULL)
    {
        return NULL;
    }

    pthread_mutex_lock(&dll->mutex);
    NODE *node = calloc(1, sizeof(NODE));
    if (node == NULL)
    {
        pthread_mutex_unlock(&dll->mutex);
        return NULL;
    }

    node->item = calloc(1, dll->size_of_element);
    if (node->item == NULL)
    {
        pthread_mutex_unlock(&dll->mutex);
        free(node);
        return NULL;
    }

    if (dll->head == NULL)
    {
        dll->head = node;
        dll->tail = node;
    }
    else
    {
        dll->tail->next = node;
        node->prev = dll->tail;
        dll->tail = node;
    }

    pthread_mutex_unlock(&dll->mutex);
    return node->item;
}

int dll_size(DLL *dll)
{
    if (dll == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&dll->mutex);
    int size = 0;
    NODE *node = dll->head;
    while (node != NULL)
    {
        size++;
        node = node->next;
    }

    pthread_mutex_unlock(&dll->mutex);
    return size;
}

int dll_clear(DLL *dll)
{
    if (dll == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&dll->mutex);
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
    pthread_mutex_unlock(&dll->mutex);
    return 0;
}

void dll_remove(DLL *dll, void *item)
{
    if (dll == NULL || item == NULL)
    {
        return;
    }

    pthread_mutex_lock(&dll->mutex);

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
            break;
        }

        node = node->next;
    }

    pthread_mutex_unlock(&dll->mutex);
}

void *get_entity_at_coords(DLL *dll, COORDS coords, void *avoid)
{
    if (dll == NULL)
    {
        return NULL;
    }

    pthread_mutex_lock(&dll->mutex);

    NODE *node = dll->head;
    while (node != NULL)
    {
        ENTITY *entity = node->item;
        if (entity->position.x == coords.x && entity->position.y == coords.y && entity != avoid)
        {
            pthread_mutex_unlock(&dll->mutex);
            return node->item;
        }

        node = node->next;
    }

    pthread_mutex_unlock(&dll->mutex);

    return NULL;
}

void *get_entity_at_spawn_point(DLL *dll, COORDS coords)
{
    if (dll == NULL)
    {
        return NULL;
    }

    pthread_mutex_lock(&dll->mutex);

    NODE *node = dll->head;
    while (node != NULL)
    {
        ENTITY *entity = node->item;
        if (entity->spawn_point.x == coords.x && entity->spawn_point.y == coords.y)
        {
            pthread_mutex_unlock(&dll->mutex);
            return node->item;
        }

        node = node->next;
    }

    pthread_mutex_unlock(&dll->mutex);

    return NULL;
}

void *get_treasure_at_coords(DLL *dll, COORDS coords)
{
    if (dll == NULL)
    {
        return NULL;
    }

    pthread_mutex_lock(&dll->mutex);

    NODE *node = dll->head;
    while (node != NULL)
    {
        TREASURE *entity = node->item;
        if (entity->position.x == coords.x && entity->position.y == coords.y)
        {
            pthread_mutex_unlock(&dll->mutex);
            return node->item;
        }

        node = node->next;
    }

    pthread_mutex_unlock(&dll->mutex);

    return NULL;
}
