#ifndef NODE_H
#define NODE_H

#include <vector>

typedef struct node {
	const char* name;
	std::vector<std::pair<struct node*, double>> neighbors;

	bool operator==(const struct node& n) const;
	bool operator!=(const struct node& n) const;

	double distance(node* n);

	static struct node* genNode(const char* name);
} node;

#endif