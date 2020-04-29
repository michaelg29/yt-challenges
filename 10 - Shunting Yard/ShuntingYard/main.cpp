#include <iostream>

#include "shunting_yard.hpp"

int main() {
	std::cout << "Hello, Shunting Yard!" << std::endl;

	std::cout << "Input equation: ";

	std::string eqn;
	std::getline(std::cin, eqn);

	ShuntingYard::variables["x"] = 15;

	ShuntingYard::RPN rpn = ShuntingYard::reversePolishNotation(eqn.c_str());
	ShuntingYard::Node* tree = ShuntingYard::parse(rpn);
	std::cout << "= " << ShuntingYard::eval(tree) << std::endl;

	return 0;
}