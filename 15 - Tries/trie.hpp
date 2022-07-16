#ifndef TRIE_HPP
#define TRIE_HPP

#include <string>
#include <vector>

namespace trie {
	struct Range {
		int lower;
		int upper;

		int calculateRange() {
			return upper - lower + 1;
		}

		bool contains(int i) {
			return i >= lower && i <= upper;
		}
	};

	// charsets for keys
	std::vector<Range> ascii_letters = { { 'A', 'Z' }, { 'a', 'z' } };
	std::vector<Range> ascii_lowercase = { { 'a', 'z' } };
	std::vector<Range> ascii_uppercase = { { 'A', 'Z' } };
	std::vector<Range> digits = { { '0', '9' } };
	std::vector<Range> alpha_numeric = { { '0', '9' }, { 'A', 'Z' }, { 'a', 'z' } };

	typedef std::vector<Range> charset;

	/*
		trie node structure
	*/
	template <typename T>
	struct node {
		bool exists;				// if data exists
		T data;						// data
		struct node<T>** children;	// array of children

		// traverse into this node and its children
		// send data to callback if data exists
		void traverse(void(*itemViewer)(T data), unsigned int noChildren) {
			if (exists) {
				itemViewer(data);
			}

			// iterate through children
			if (children) {
				for (int i = 0; i < noChildren; i++) {
					if (children[i] != NULL) {
						children[i]->traverse(itemViewer, noChildren);
					}
				}
			}
		}
	};

	/*
		trie class
	*/
	template <typename T>
	class Trie {
	public:
		/*
			constructor
		*/
		Trie(charset chars)
			: chars(chars), noChars(0), root(nullptr) {
			// set number of chars
			for (Range r : chars) {
				noChars += r.calculateRange();
			}

			// initialize root memory
			root = new node<T>;
			root->exists = false;
			root->children = new node<T>*[noChars];
			for (int i = 0; i < noChars; i++) {
				root->children[i] = NULL;
			}
		}

		/*
			modifiers
		*/

		// insertion (can also use to change data)
		bool insert(std::string key, T element) {
			int idx;
			node<T>* current = root;

			for (char c : key) {
				idx = getIdx(c);
				if (idx == -1) {
					return false;
				}
				if (!current->children[idx] || current->children[idx] == NULL) {
					// child doesn't exist yet
					current->children[idx] = new node<T>;
					current->children[idx]->exists = false;
					current->children[idx]->children = new node<T> * [noChars];
					for (int i = 0; i < noChars; i++) {
						current->children[idx]->children[i] = NULL;
					}
				}
				current = current->children[idx];
			}

			// set data
			current->data = element;
			current->exists = true;

			return true;
		}

		// deletion method
		bool erase(std::string key) {
			// TODO: do not clear node
			if (!root) {
				return false;
			}

			int idx;
			node<T>* current = root;

			for (char c : key) {
				idx = getIdx(c);
				if (idx == -1) {
					return false;
				}
				if (!current->children[idx] || current->children[idx] == NULL) {
					// child not found
					return false;
				}
				current = current->children[idx];
			}

			current->data = nullptr;
			current->exists = false;

			return true;
		}

		// release root note
		void cleanup() {
			unloadNode(root);
		}

		/*
			accessors
		*/

		// determine if key is contained in trie
		bool containsKey(std::string key) {
			// pass lambda function that obtains if found element exists
			return findKey<bool>(key, [](node<T>* element) -> bool {
				if (!element) {
					// element is nullptr
					return false;
				}

				return element->exists;
			});
		}

		// obtain data element
		T& operator[](std::string key) {
			// pass lambda function that returns data if element exists
			return findKey<T&>(key, [](node<T>* element) -> T& {
				if (!element || !element->exists) {
					// element is nullptr
					throw std::invalid_argument("key not found");
				}

				return element->data;
			});
		}

		// traverse through all keys
		void traverse(void(*itemViewer)(T data)) {
			if (root) {
				root->traverse(itemViewer, noChars);
			}
		}

	private:
		// character set
		charset chars;
		// length of set
		unsigned int noChars;

		// root node
		node<T>* root;

		// find element at key and process it
		template <typename V>
		V findKey(std::string key, V(*process)(node<T>* element)) {
			// placeholders
			int idx;
			node<T>* current = root;
			for (char c : key) {
				idx = getIdx(c);

				if (idx == -1) {
					// leave to parameter function to deal with nullptr
					return process(nullptr);
				}

				// update current
				current = current->children[idx];
				if (current == NULL) {
					// leave to parameter function to deal with nullptr
					return process(nullptr);
				}
			}
			return process(current);
		}

		// get index at specific character in character set
		// return -1 if not found
		int getIdx(char c) {
			int ret = 0;

			for (Range r : chars) {
				if (r.contains((int)c)) {
					ret += (int)c - r.lower;
					break;
				}
				else {
					ret += r.calculateRange();
				}
			}

			// went through all ranges and found nothing, return -1
			return ret == noChars ? -1 : ret;
		}

		// unload node and its children
		void unloadNode(node<T>* top) {
			if (!top) {
				return;
			}

			for (int i = 0; i < noChars; i++) {
				if (top->children[i]) {
					// child exists, deallocate it
					unloadNode(top->children[i]);
				}
			}

			top = nullptr;
		}
	};
}

#endif