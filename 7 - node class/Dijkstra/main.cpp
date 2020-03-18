#include <iostream>
#include <vector>
#include <map>

#include "node.h"

template<typename T>
bool contains(std::vector<T> v, T x) {
	return std::find(v.begin(), v.end(), x) != v.end();
}

namespace dijkstra {
	node* findMinOnQueue(std::vector<std::pair<node*, double>> queue);
	int findOnQueue(std::vector<std::pair<node*, double>> queue, node* element);
	double calculateDistance(std::map<node*, double> weight, std::map<node*, node*> prevNode, node* element);
	double calculateDistance(node* shortestPathsTree, node* goal);
	node* generateTree(std::map<node*, double> weight, std::map<node*, node*> prevNode, node* n);

	node* algorithm(node* start);
}

int main() {
	std::cout << "Hello, Dijkstra!" << std::endl;

	node* A = node::genNode("A");
	node* B = node::genNode("B");
	node* C = node::genNode("C");
	node* D = node::genNode("D");
	node* E = node::genNode("E");
	node* F = node::genNode("F");
	node* G = node::genNode("G");

	A->neighbors = { { B, 5 }, { C, 3 } };
	B->neighbors = { { C, 2 }, { E, 3 }, { G, 1 } };
	C->neighbors = { { D, 7 }, { E, 7 } };
	D->neighbors = { { A, 2 }, { F, 6 } };
	E->neighbors = { { D, 2 }, { F, 1 } };
	G->neighbors = { { E, 1 } };

	node* tree = dijkstra::algorithm(A);
	std::cout << dijkstra::calculateDistance(tree, F) << std::endl;

	return 0;
}

node* dijkstra::findMinOnQueue(std::vector<std::pair<node*, double>> queue) {
	double min = queue.at(0).second;
	node* ret = queue.at(0).first;

	for (int i = 1, size = queue.size(); i < size; i++) {
		if (queue.at(i).second < min) {
			// found new min
			min = queue.at(i).second;
			ret = queue.at(i).first;
		}
	}

	return ret;
}

int dijkstra::findOnQueue(std::vector<std::pair<node*, double>> queue, node* element) {
	for (int i = 0, size = queue.size(); i < size; i++) {
		if (*(queue.at(i).first) == *element) {
			return i;
		}
	}

	return -1;
}

double dijkstra::calculateDistance(std::map<node*, double> weight, std::map<node*, node*> prevNode, node* element) {
	if (*(prevNode[element]) == *element) {
		// distance from element to self is 0
		return 0;
	}

	return weight[element] + calculateDistance(weight, prevNode, prevNode[element]);
}

double dijkstra::calculateDistance(node* shortestPathsTree, node* goal) {
	if (*shortestPathsTree == *goal) {
		return 0;
	}
	
	for (std::pair<node*, double> pair : shortestPathsTree->neighbors) {
		double w = calculateDistance(pair.first, goal);
		if (w != -1) {
			return shortestPathsTree->distance(pair.first) + w;
		}
	}

	return -1;
}

node* dijkstra::generateTree(std::map<node*, double> weight, std::map<node*, node*> prevNode, node* n) {
	node* ret = node::genNode(n->name);

	for (std::pair<node*, node*> pair : prevNode) {
		if (*(pair.second) == *ret && *(pair.first) != *ret) {
			node* neighbor = generateTree(weight, prevNode, pair.first);
			ret->neighbors.push_back({ neighbor, weight[pair.first] });
		}
	}

	return ret;
}

node* dijkstra::algorithm(node* start) {
	std::map<node*, double> weight;
	std::map<node*, node*> prevNode;

	weight[start] = 0;
	prevNode[start] = start;

	std::vector<node*> visited;

	std::vector<std::pair<node*, double>> queue;
	queue.push_back({ start, 0 });

	while (queue.size() != 0) {
		node* smallest = findMinOnQueue(queue);

		visited.push_back(smallest);
		queue.erase(queue.begin() + findOnQueue(queue, smallest));

		// go through neighbors
		for (std::pair<node*, double> neighbor : smallest->neighbors) {
			int loc = findOnQueue(queue, neighbor.first);

			if (contains<node*>(visited, neighbor.first)) {
				// already visited
				continue;
			}
			else if (loc != -1) {
				// in queue, find new path if necessary
				double currentDistance = calculateDistance(weight, prevNode, neighbor.first);

				double newDistance = calculateDistance(weight, prevNode, smallest) + smallest->distance(neighbor.first);
				if (newDistance < currentDistance) {
					// accept path
					queue.erase(queue.begin() + loc);
					queue.push_back({ neighbor.first, newDistance });
					weight[neighbor.first] = smallest->distance(neighbor.first);
					prevNode[neighbor.first] = smallest;
				}
			}
			else {
				// add to queue
				weight[neighbor.first] = smallest->distance(neighbor.first);
				prevNode[neighbor.first] = smallest;

				queue.push_back({ neighbor.first, calculateDistance(weight, prevNode, neighbor.first) });
			}
		}
	}

	// generate tree
	node* shortestPathsTree = generateTree(weight, prevNode, start);

	return shortestPathsTree;
}