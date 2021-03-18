#include <stdio.h>
#include <stdlib.h>

#include "dynamicarray.h"
#include "dlinkedlist.h"

void printChar(char *element)
{
    printf("%c, ", *element);
}

void printCharDynarr(dynamicarray *list)
{
    printf("{ ");

    dynarr_createSpecificIterator(char);
    dynarr_char_iterate(list, printChar);

    printf("}\n");
}

void printCharDlist(dlinkedlist *list)
{
    printf("{ ");

    dlist_createSpecificIterator(char);
    dlist_char_iterate(list, printChar);

    printf("}\n");
}

int main()
{
    printf("Hello, world!\n");

    char arr[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

    dynamicarray list = dynarr_defaultAllocate();

    for (unsigned int i = 0; i < 7; i++)
    {
        dynarr_addLast(&list, arr + i);
    }

    void *cur = NULL;
    dynarr_iterator it = dynarr_iterator_new(&list);

    while ((cur = dynarr_iterator_next(&it)))
    {
        printChar((char *)cur);
    }

    dynarr_free(&list);

    return 0;
}