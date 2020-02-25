#include "util.h"

bool json::firstAndLastMatch(std::string str, char first, char last) {
	return str[0] == first && str[str.length() - 1] == last;
}

bool json::strContains(std::string str, char c) {
	return str.find(c) != std::string::npos;
}

bool json::isNum(char c) {
	return strContains("0123456789.-", c);
}

bool json::isEmpty(char c) {
	return c == ' ' || c == '\n' || c == '\t';
}

bool json::writeFile(const char* filename, std::string content) {
	std::ofstream file(filename);

	if (file.is_open()) {
		file << content;
		file.close();
		return true;
	}

	return false;
}

std::string json::readFile(const char* filename) {
	std::ifstream file(filename);
	std::string ret = "";

	if (file.is_open()) {
		std::stringstream sstream;
		sstream << file.rdbuf();
		ret = sstream.str();
	}

	file.close();

	return ret;
}