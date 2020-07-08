#include <iostream>
#include <string>
#include <vector>
#include "btree.hpp"

struct data {
	unsigned int key;

	std::string name;
};

int main() {
	std::cout << "Hello, world!" << std::endl;

	btree::BTree<data> tree(4);

	std::vector<unsigned int> keys = { 10, 80, 30, 90, 85, 40, 50, 60, 70, 95, 100 };
	for (unsigned int i : keys) {
		data* d = new data;
		d->key = i;
		d->name = "name" + std::to_string(i);
		tree.insert(d);
	}

	tree.traverse([](data* element) -> void {
		std::cout << element->key << ' ' << element->name << std::endl;
	});

	tree.clear();

	std::cout << "Goodbye" << std::endl;

	return 0;
}