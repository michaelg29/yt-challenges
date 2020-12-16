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

    for (unsigned int i = 0; i < 5; i++)
    {
        dynarr_addLast(&list, arr + i);
    }

    printCharDynarr(&list);
    printf("%c\n", *((char *)dynarr_removeLast(&list)));
    printCharDynarr(&list);

    dynarr_free(&list);

    return 0;
}