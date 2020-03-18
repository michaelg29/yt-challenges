#include "node.h"

bool node::operator==(const struct node& n) const {
	return name == n.name;
}

bool node::operator!=(const struct node& n) const {
	return name != n.name;
}

double node::distance(node* n) {
	for (std::pair<node*, double> pair : neighbors) {
		if (*(pair.first) == *n) {
			return pair.second;
		}
	}
}

node* node::genNode(const char* name) {
	node* ret = new node;
	ret->name = name;

	return ret;
}