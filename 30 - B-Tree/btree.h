#ifndef BTREE_H
#define BTREE_H

#define BTREE_MIN_M 3

typedef struct btree_node
{
    int n;                        // current number of values
    int noChildren;               // number of existing children (0 if leaf)
    int *keys;                    // array of keys
    void **vals;                  // parallel array of values
    struct btree_node **children; // children array
} btree_node;

typedef struct btree
{
    int m; // degree
    int t; // minimum degree

    btree_node *root; // pointer to root node
} btree;

// BTREE ===================================
btree btree_new(int m);

void *btree_search(btree tree, int key);

void btree_insert(btree *tree, int key, void *val);
void btree_delete(btree *tree, int key);

void btree_free(btree *tree);

// BTREE_NODE

btree_node *btree_newNode(btree tree);
btree_node *btree_newNodeItem(btree tree, int key, void *val);

void btree_moveKeyVal(btree_node *in, int inIdx, btree_node *out, int outIdx);

btree_node *btree_node_search(btree_node *root, int key, int *idx);

btree_node *btree_node_get_inorderPredecessor(btree_node *root, btree tree, int i);

btree_node *btree_node_split(btree_node *root, btree tree, btree_node *newNode, int i);
btree_node *btree_node_insert(btree_node *root, btree tree, int key, void *val);

void btree_node_rebalance(btree_node *root, btree tree, int key);
int btree_node_delete(btree_node *root, btree tree, int key);

void btree_node_freeShallow(btree_node *node);
void btree_node_free(btree_node *root, btree tree);

#endif