#include <stdio.h>

#include "avl.h"

void print(avl *node)
{
    printf("{ %d: %d; %d; L: %d; R: %d; }\n",
           *((int *)node->key), *((int *)node->val),
           node->height,
           node->left ? *((int *)node->left->key) : -1,
           node->right ? *((int *)node->right->key) : -1);
}

int main()
{
    printf("Hello, world!\n");

    int vals[] = {44, 17, 62, 32, 50, 78, 48, 54, 88};

    avl *root = avl_createEmptyRoot(intkeycmp);
    for (int i = 0; i < 9; i++)
    {
        root = avl_insert(root, vals + i, vals + i);
    }

    int val = 32;
    root = avl_remove(root, &val);

    avl_preorderTraverse(root, print);

    printf("Freeing\n");
    avl_free(root);
    printf("Done\n");

    return 0;
}