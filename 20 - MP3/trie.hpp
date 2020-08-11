/*
	Modified version of Programming Challenge 15 - Tries

	Original source code: https://github.com/michaelg29/yt-challenges/blob/master/15%20-%20Tries/trie.hpp
*/

#ifndef TRIE_HPP
#define TRIE_HPP

#include <string>
#include <vector>
#include <stdexcept>

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

	typedef std::vector<Range> charset;

	/*
		trie node structure
	*/
	struct node {
		bool exists;				// if key exists
		struct node** children;		// array of children
	};

	/*
		trie class (simplified to only hold boolean as data)
	*/
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
			root = new node;
			root->exists = false;
			root->children = new node*[noChars];
			for (int i = 0; i < noChars; i++) {
				root->children[i] = NULL;
			}
		}

		/*
			modifiers
		*/

		// insertion (can also use to change data)
		bool insert(std::string key) {
			int idx;
			node* current = root;

			for (char c : key) {
				idx = getIdx(c);
				if (idx == -1) {
					return false;
				}
				if (!current->children[idx]) {
					// child doesn't exist yet
					current->children[idx] = new node;
					current->children[idx]->exists = false;
					current->children[idx]->children = new node*[noChars];
					for (int i = 0; i < noChars; i++) {
						current->children[idx]->children[i] = NULL;
					}
				}
				current = current->children[idx];
			}

			// set data
			current->exists = true;

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
			// placeholders
			int idx;
			node* current = root;
			for (char c : key) {
				idx = getIdx(c);

				if (idx == -1) {
					return false;
				}

				// update current
				current = current->children[idx];
				if (!current) {
					return false;
				}
			}

			return current->exists;
		}

	private:
		// character set
		charset chars;
		// length of set
		unsigned int noChars;

		// root node
		node* root;

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
		void unloadNode(node* top) {
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