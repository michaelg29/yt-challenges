#include <iostream>
#include "json.hpp"

int main() {
	std::cout << "Hello, JSON!" << std::endl;

	json::json_data obj = json::read("data.json");

	obj["names"] = { "Michael", "Stalin", "Lenin" };

	std::cout << obj.stringify() << std::endl;

	return 0;
}