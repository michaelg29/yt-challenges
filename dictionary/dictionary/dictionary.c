#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "dictionary.h"

#define N 26

typedef struct node {
	bool isWord;
	struct node* children[N];
} node;

node* root;
int _size = 0;

bool load() {
	// initialize trie
	root = malloc(sizeof(node));
	if (root == NULL) {
		return false;
	}
	root->isWord = false;
	for (int i = 0; i < N; i++) {
		root->children[i] = NULL;
	}

	// open dictionary file
	FILE* file = fopen("words.txt", "r");
	if (file == NULL) {
		unload();
		return false;
	}

	// buffer for each word
	char word[LENGTH + 1];
	int idx;

	// insert each word into trie
	while (fscanf(file, "%s", word) != EOF) {
		node* child = root;

		for (int i = 0, len = strlen(word); i < len; i++) {
			idx = (int)word[i] - (int)'a';

			if (child->children[idx] == NULL) { // node doesn't exist
				child->children[idx] = malloc(sizeof(node));
				if (child->children[idx] == NULL) { // error with malloc
					return false;
				}
				child->children[idx]->isWord = false;
				for (int j = 0; j < N; j++) {
					child->children[idx]->children[j] = NULL;
				}
			}
			child = child->children[idx];
		}
		child->isWord = true;
		_size++;
	}

	fclose(file);

	return true;
}

int size(void) {
	return _size;
}

bool check(const char* word) {
	int idx;
	node* child = root;
	for (int i = 0, len = strlen(word); i < len; i++) {
		idx = (int)tolower(word[i]) - (int)'a';

		child = child->children[idx];
		if (child == NULL) {
			return false;
		}
	}
	return child->isWord;
}

void unloadNode(node* top) {
	if (top == NULL)
		return;

	for (int i = 0; i < N; i++) {
		if (top->children[i] != NULL) {
			unloadNode(top->children[i]);
		}
	}
	free(top);
}

void unload() {
	unloadNode(root);
}