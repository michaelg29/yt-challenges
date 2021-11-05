#include "btree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printBtree(btree_node *root)
{
    if (root)
    {
        printf("( ");

        for (int i = 0; i < root->n; i++)
        {
            if (root->noChildren)
            {
                printBtree(root->children[i]);
            }
            printf(" %d ", root->keys[i]);
        }
        if (root->noChildren)
        {
            printBtree(root->children[root->n]);
        }

        printf(" )");
    }
    else
    {
        printf("<>");
    }
}

int main()
{
    printf("Hello, world!\n");

    btree tree = btree_new(3);

#define INSERTLEN 20
    int insertElements[INSERTLEN] = {25, 8, 37, 55, 95, 27, 88, 13, 29, 42, 51, 72, 100, 105, 90, 92, 50, 49, 45, 47};

    int i;
    for (i = 0; i < INSERTLEN; i++)
    {
        btree_insert(&tree, insertElements[i], NULL);
    }
    printBtree(tree.root);
    printf("\n");

#define DELETELEN 16
    int deleteElements[DELETELEN] = {27, 29, 88, 100, 51, 90, 37, 95, 72, 42, 92, 8, 55, 13, 105, 25};
    for (i = 0; i < DELETELEN - 1; i++)
    {
        btree_delete(&tree, deleteElements[i]);
        printf("-%04d: ", deleteElements[i]);
        printBtree(tree.root);
        printf("\n");
    }

    btree_delete(&tree, deleteElements[i]);
    printf("-%04d: ", deleteElements[i]);
    printBtree(tree.root);
    printf("\n");

    btree_free(&tree);

    return 0;
}