#include "dynamicarray.h"

#include <stdlib.h>

#define DYNARR_DEFAULT_SIZE 4

dynamicarray dynarr_defaultAllocate()
{
    return dynarr_allocate(DYNARR_DEFAULT_SIZE);
}

dynamicarray dynarr_allocate(unsigned int capacity)
{
    dynamicarray ret;

    // allocate memory for the array
    ret.list = malloc(capacity * sizeof(void *));
    ret.size = 0;
    ret.capacity = ret.list ? capacity : 0;

    return ret;
}

void dynarr_addAtIdx(dynamicarray *list, void *element, unsigned int idx)
{
    if (idx > list->size)
    {
        // boundary error
        return;
    }

    if (list->size == list->capacity)
    {
        // resize if necessary
        dynarr_resize(list);
    }

    // right shift elements beyond index to make space
    for (unsigned int i = list->size; i > idx; i--)
    {
        list->list[i] = list->list[i - 1];
    }

    // set element at index
    list->list[idx] = element;
    list->size++;
}

void dynarr_addFirst(dynamicarray *list, void *element)
{
    dynarr_addAtIdx(list, element, 0);
}

void dynarr_addLast(dynamicarray *list, void *element)
{
    dynarr_addAtIdx(list, element, list->size);
}

void dynarr_set(dynamicarray *list, void *element, unsigned int idx)
{
    if (idx > list->size)
    {
        // boundary error
        return;
    }

    list->list[idx] = element;
}

void dynarr_resize(dynamicarray *list)
{
    // allocate memory (double capacity)
    void **newList = malloc((list->capacity * 2) * sizeof(void *));

    // copy elements
    for (unsigned int i = 0; i < list->size; i++)
    {
        newList[i] = list->list[i];
    }

    // update pointer
    free(list->list);
    list->list = newList;
    // double size
    list->capacity <<= 1;
}

void *dynarr_removeAtIdx(dynamicarray *list, unsigned int idx)
{
    // boundary check
    if (idx >= list->size)
    {
        return NULL;
    }

    // get the element to be removed
    void *ret = list->list[idx];

    // left shift elements
    for (unsigned int i = idx + 1; i < list->size; i++)
    {
        list->list[i - 1] = list->list[i];
    }
    // set last element to null
    list->list[list->size - 1] = NULL;

    // decrease size
    list->size--;

    return ret;
}

void *dynarr_removeFirst(dynamicarray *list)
{
    return dynarr_removeAtIdx(list, 0);
}

void *dynarr_removeLast(dynamicarray *list)
{
    return dynarr_removeAtIdx(list, list->size - 1);
}

void *dynarr_get(dynamicarray *list, unsigned int idx)
{
    // if in bounds, return element
    return (idx < list->size) ? list->list[idx] : NULL;
}

void dynarr_iterate(dynamicarray *list, void (*visit)(void *element))
{
    for (unsigned int i = 0; i < list->size; i++)
    {
        visit(list->list[i]);
    }
}

void dynarr_clear(dynamicarray *list)
{
    free(list->list);
    list->size = 0;
    list->capacity = DYNARR_DEFAULT_SIZE;
    list->list = malloc(DYNARR_DEFAULT_SIZE * sizeof(void *));
}

void dynarr_free(dynamicarray *list)
{
    free(list->list);
    free(list);
}

void dynarr_freeDeep(dynamicarray *list)
{
    free(list->list);
    for (unsigned int i = 0; i < list->size; i++)
    {
        free(list->list[i]);
    }
    free(list);
}