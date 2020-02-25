#include "json.h"

using namespace json;

json_data json_util::read(const char* filename) {
	return json_data::parse(readFile(filename));
}

bool json_util::write(const char* filename, json_data val) {
	return writeFile(filename, val.stringify());
}