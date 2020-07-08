#ifndef BTREE_HPP
#define BTREE_HPP

namespace btree {
    template <typename T>
    void shiftRight(T* arr, int i, int& size) {
        for (int idx = size - 1; idx >= i; idx--) {
            arr[idx + 1] = arr[idx];
        }
        size++;
    }

    template <typename T>
    void shiftLeft(T* arr, int i, int& size) {
        for (; i < size - 1; i++) {
            arr[i] = arr[i + 1];
        }
        size--;
    }

    template <typename T>
    class node {
    public:
        node<T>**   children;	// array of child pointers
        T**         data;       // array of data (has keys)

        int m;                  // tree degree
        int n;                  // current number of keys
        int min;                // min no keys = ceil(m / 2) - 1

        bool leaf;              // true if no children

        node(int m, bool leaf = true)
            : m(m), min(ceil(m / 2.) - 1), leaf(leaf), n(0) {
            // allocate memory for arrays
            children = new node<T> * [m + 1];
            data = new T * [m];
        }

        node<T>* split(T*& upshiftElement) {
            node<T>* right = new node<T>(m, leaf);

            upshiftElement = data[min];
            data[min] = nullptr;

            // data
            for (int idx = min + 1; idx < n; idx++) {
                right->data[idx - min - 1] = data[idx];
                data[idx] = nullptr;
                right->n++;
            }
            n -= right->n + 1;  // +1 to account for divisor

            // children
            if (!leaf) {
                for (int idx = min + 1; idx <= m; idx++) {
                    right->children[idx - min - 1] = children[idx];
                    children[idx] = nullptr;
                }
            }

            return right;
        }

        node<T>* insert(T* element, T*& upshiftElement) {
            /*
                if overflow, return new created child and update reference for new parent
            */

            // find ordered position for element
            int i = 0;
            while (i < n && element->key > data[i]->key) {
                i++;
            }

            if (leaf) {
                // allocate space and assign
                shiftRight<T*>(data, i, n);
                data[i] = element;

                if (n == m) {
                    // overflow
                    return split(upshiftElement);
                }
            }
            else {
                // pass down to child
                node<T>* right = children[i]->insert(element, upshiftElement);

                if (right) {
                    // overflow in child, returned min and right branch

                    // set data
                    shiftRight<T*>(data, i, n);
                    data[i] = upshiftElement;

                    // add child
                    int _n = n;
                    shiftRight<node<T>*>(children, i + 1, _n);
                    children[i + 1] = right;

                    if (n == m) {
                        // too many elements
                        return split(upshiftElement);
                    }
                }
            }

            upshiftElement = nullptr;
            return nullptr;
        }

        T*& search(int key) {
            int i = 0;
            for (; i < n && key >= data[i]->key; i++) {
                if (key == data[i]->key) {
                    // found element
                    return data[i];
                }
            }

            // if no children, no other places to look
            return leaf ? nullptr : children[i]->search(key);
        }

        void traverse(void(itemViewer)(T* data)) {
            for (int i = 0; i < n; i++) {
                if (!leaf) {
                    // has children
                    children[i]->traverse(itemViewer);
                }
                itemViewer(data[i]);
            }
            if (!leaf) {
                // get rightmost child
                children[n]->traverse(itemViewer);
            }
        }

        void clear() {
            for (int i = 0; i < n; i++) {
                if (!leaf) {
                    children[i]->clear();
                    children[i] = nullptr;
                }
                data[i] = nullptr;
            }
            if (!leaf) {
                children[n]->clear();
                children[n] = nullptr;
            }
            children = nullptr;
            data = nullptr;
        }
    };

    template <typename T>
    class BTree {
    public:
        int m;			// degree of tree

        node<T>* root;	// root node of tree

        BTree(int m)
            : m(m) {
            // allocate for root
            root = new node<T>(m, true);
            root->min = 1;
        }

        void traverse(void(itemViewer)(T* data)) {
            root->traverse(itemViewer);
        }

        void insert(std::vector<T*> elements) {
            for (T* element : elements) {
                insert(element);
            }
        }

        void insert(T* element) {
            T* upshiftElement = nullptr;
            node<T>* right = root->insert(element, upshiftElement);

            if (right) {
                // need to create new root
                node<T>* newRoot = new node<T>(m, false);

                // set data
                newRoot->data[0] = upshiftElement;
                newRoot->n = 1;

                // set children
                newRoot->children[0] = root;
                newRoot->children[1] = right;

                // reset root in class
                root = newRoot;
            }
        }

        T*& operator[](int key) {
            return root->search(key);
        }

        T*& search(int key) {
            return root->search(key);
        }

        void clear() {
            root->clear();
        }
    };
}

#endif