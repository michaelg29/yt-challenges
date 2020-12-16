#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

struct dnode
{
    void *val;
    struct dnode *next;
    struct dnode *prev;
};
typedef struct dnode dnode;

typedef struct
{
    unsigned int size;
    dnode *head;
    dnode *tail;
} dlinkedlist;

dlinkedlist dlist_allocate();
dnode *dlist_allocateNodePtr(void *val);

void dlist_addAtIdx(dlinkedlist *list, void *element, unsigned int idx);
void dlist_addFirst(dlinkedlist *list, void *element);
void dlist_addLast(dlinkedlist *list, void *element);

void *dlist_removeAtIdx(dlinkedlist *list, unsigned int idx);
void *dlist_removeFirst(dlinkedlist *list);
void *dlist_removeLast(dlinkedlist *list);

void *dlist_get(dlinkedlist *list, unsigned int idx);
dnode *dlist_getNode(dlinkedlist *list, unsigned int idx);

void dlist_iterate(dlinkedlist *list, void (*visit)(void *element));

#define dlist_createSpecificIterator(type)                                        \
    void dlist_##type##_iterate(dlinkedlist *list, void (*visit)(type * element)) \
    {                                                                             \
        for (dnode *cur = list->head->next; cur->next; cur = cur->next)           \
        {                                                                         \
            visit((type *)cur->val);                                              \
        }                                                                         \
    }

void dlist_clear(dlinkedlist *list);

void dlist_free(dlinkedlist *list);
void dlist_freeDeep(dlinkedlist *list);
void dlist_freeNode(dnode *node);
void dlist_freeNodeDeep(dnode *node);

#endif