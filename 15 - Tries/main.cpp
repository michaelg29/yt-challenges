#include <iostream>
#include <string>

#include "trie.hpp"

struct data {
	unsigned int key;
	std::string name;
};

void printData(data* val) {
	std::cout << val->key << ' ' << val->name << std::endl;
}

int main() {
	std::cout << "Hello, world!" << std::endl;

	trie::Trie<data*> tree(trie::alpha_numeric);

	tree.insert("abcd", new data({ 5, "Mao" }));
	tree.insert("ABCD", new data({ 8, "Lenin" }));
	tree.insert("A0gg", new data({ 9, "Stalin" }));
	tree.insert("A0ggnore", new data({ 10, "Trotsky" }));

	std::cout << tree["A0gg"]->name << std::endl;

	tree.traverse(printData);

	tree.cleanup();

	std::cout << "goodbye" << std::endl;

	return 0;
}