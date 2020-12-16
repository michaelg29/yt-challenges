#include "dlinkedlist.h"

#include <stdlib.h>

dlinkedlist dlist_allocate()
{
    dlinkedlist ret;

    ret.size = 0;

    // setup dummy head and tail
    ret.head = dlist_allocateNodePtr(NULL);
    ret.tail = dlist_allocateNodePtr(NULL);
    ret.head->next = ret.tail;
    ret.tail->prev = ret.head;

    return ret;
}

dnode *dlist_allocateNodePtr(void *val)
{
    // allocate memory
    dnode *ret = malloc(sizeof(dnode));

    ret->next = NULL;
    ret->prev = NULL;
    ret->val = val;

    return ret;
}

void dlist_addAtIdx(dlinkedlist *list, void *element, unsigned int idx)
{
    // get current node at index
    dnode *cur = dlist_getNode(list, idx);
    if (!cur)
    {
        // boundary error
        return;
    }

    // create new node
    dnode *newNode = dlist_allocateNodePtr(element);

    // update pointers on left side
    newNode->prev = cur->prev;
    cur->prev->next = newNode;

    // update pointers on right size
    newNode->next = cur;
    cur->prev = newNode;

    // increase size
    list->size++;
}

void dlist_addFirst(dlinkedlist *list, void *element)
{
    // create node
    dnode *newNode = dlist_allocateNodePtr(element);

    // update pointers on right side
    list->head->next->prev = newNode;
    newNode->next = list->head->next;

    // update pointers on left side
    list->head->next = newNode;
    newNode->prev = list->head;

    // increase size
    list->size++;
}

void dlist_addLast(dlinkedlist *list, void *element)
{
    dnode *newNode = dlist_allocateNodePtr(element);

    // update pointers on left side
    list->tail->prev->next = newNode;
    newNode->prev = list->tail->prev;

    // update pointers on right side
    list->tail->prev = newNode;
    newNode->next = list->tail;

    // increase size
    list->size++;
}

void *dlist_removeAtIdx(dlinkedlist *list, unsigned int idx)
{
    if (list->size == 0)
    {
        // boundary error
        return NULL;
    }

    // get current node at index
    dnode *cur = dlist_getNode(list, idx);
    if (!cur)
    {
        // boundary error
        return NULL;
    }
    void *ret = cur->val;

    // update pointers to fix gap
    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;

    dlist_freeNode(cur);

    // decrease size
    list->size--;

    return ret;
}

void *dlist_removeFirst(dlinkedlist *list)
{
    if (list->size == 0)
    {
        // boundary error
        return NULL;
    }

    // get first node
    dnode *cur = list->head->next;
    void *ret = cur->val;

    // update pointers to fix gap
    cur->next->prev = cur->prev;
    cur->prev->next = cur->next;

    dlist_freeNode(cur);

    // decrease size
    list->size--;

    return ret;
}

void *dlist_removeLast(dlinkedlist *list)
{
    if (list->size == 0)
    {
        // boundary error
        return NULL;
    }

    // get last node
    dnode *cur = list->tail->prev;
    void *ret = cur->val;

    /// update pointers to fix gap
    cur->next->prev = cur->prev;
    cur->prev->next = cur->next;

    dlist_freeNode(cur);

    // decrease size
    list->size--;

    return ret;
}

void *dlist_get(dlinkedlist *list, unsigned int idx)
{
    dnode *cur = dlist_getNode(list, idx);

    // if current exists, return the value
    return cur ? cur->val : NULL;
}

dnode *dlist_getNode(dlinkedlist *list, unsigned int idx)
{
    if (idx >= list->size)
    {
        return NULL;
    }

    // iterate through the nodes
    dnode *cur = list->head->next;
    for (unsigned int i = 0; i < idx && cur->next; i++)
    {
        cur = cur->next;
    }
    return cur;
}

void dlist_iterate(dlinkedlist *list, void (*visit)(void *element))
{
    // iterate through all the nodes
    for (dnode *cur = list->head->next; cur->next; cur = cur->next)
    {
        // visit the value
        visit(cur->val);
    }
}

void dlist_clear(dlinkedlist *list)
{
    // start at first node
    dnode *cur = list->head->next;

    while (cur->next)
    {
        // advance pointer
        cur = cur->next;
        // free previous pointer
        dlist_freeNode(cur->prev);
    }

    list->head->next = list->tail;
    list->tail->prev = list->head;

    list->size = 0;
}

void dlist_free(dlinkedlist *list)
{
    // start at head
    dnode *cur = list->head;

    while (cur->next)
    {
        // advance pointer
        cur = cur->next;
        // free previous pointer
        dlist_freeNode(cur->prev);
    }

    // free tail
    dlist_freeNode(list->tail);

    free(list);
}

void dlist_freeDeep(dlinkedlist *list)
{
    // start at head
    dnode *cur = list->head;

    while (cur->next)
    {
        // advance pointer
        cur = cur->next;
        // free previous pointer and its element
        dlist_freeNodeDeep(cur->prev);
    }

    // free tail (deep not needed because value is NULL)
    dlist_freeNode(list->tail);

    free(list);
}

void dlist_freeNode(dnode *node)
{
    // free the pointer memory
    free(node);
}

void dlist_freeNodeDeep(dnode *node)
{
    // free the node pointer and its element
    free(node->val);
    free(node);
}