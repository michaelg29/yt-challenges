#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <fstream>
#include <sstream>

namespace json {
	bool firstAndLastMatch(std::string str, char first, char last);

	bool strContains(std::string str, char c);

	bool isNum(char c);

	bool isEmpty(char c);

	bool writeFile(const char* filename, std::string content);

	std::string readFile(const char* filename);
}

#endif