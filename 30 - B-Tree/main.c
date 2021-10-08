#include "btree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printBtree(btree_node *root)
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

int main()
{
    printf("Hello, world!\n");

    btree tree = btree_new(3);

    int keys[7] = {8, 9, 10, 11, 15, 20, 17};
    for (int i = 0; i < 7; i++)
    {
        btree_insert(&tree, keys[i], NULL);
        printf("+%04d: ", keys[i]);
        printBtree(tree.root);
        printf("\n");
    }

    btree_free(&tree);

    return 0;
}