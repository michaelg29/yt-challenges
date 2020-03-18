#include <iostream>

#include "json.h"

using namespace json;

int main() {
	std::cout << "Hello, JSON!" << std::endl;

	json_data d = json_util::read("data.json");

	std::cout << d.stringify() << std::endl;
}