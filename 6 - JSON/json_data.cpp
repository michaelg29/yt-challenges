#include "json.h"
#include "util.h"

#include <sstream>
#include <iostream>

using namespace json;

std::string json_bool::TRUE = "true";
std::string json_bool::FALSE = "false";

std::string json_data::getType() {
	switch (type) {
	case json_type::json_string:
		return "string";
	case json_type::json_num:
		return "number";
	case json_type::json_bool:
		return "bool";
	case json_type::json_list:
		return "list";
	case json_type::json_object:
		return "json object";
	}
}

std::string json_data::stringify(int tabSpace, int initTabPos, bool isDictVal) {
	std::stringstream ret;

	std::string tab;
	// initialize tab string
	for (int i = 0; i < tabSpace; i++) { tab += ' '; }

	// indent line
	if (!isDictVal) {
		// don't want initial tab indent if dictionary value
		for (int i = 0; i < initTabPos; i++) { ret << tab; }
	}

	std::stringstream sec_stream;
	std::string sec_str;

	// input element
	switch (type) {
	case json_type::json_string:
		ret << '"' << s_val << '"';
		break;
	case json_type::json_num:
		ret << n_val;
		break;
	case json_type::json_bool:
		ret << (b_val ? json_bool::TRUE : json_bool::FALSE);
		break;
	case json_type::json_list:
		ret << '[' << std::endl;
		
		for (json_data data : l_val) {
			sec_stream << data.stringify(tabSpace, initTabPos + 1) << ',' << std::endl;
		}
		sec_str = sec_stream.str();
		sec_str = sec_str.substr(0, sec_str.length() - 2);
		ret << sec_str << std::endl;

		for (int i = 0; i < initTabPos; i++) { ret << tab; }
		ret << ']';
		break;
	case json_type::json_object:
		ret << '{' << std::endl;

		for (std::pair<std::string, json_data> pair : o_val) {
			for (int i = 0; i < initTabPos + 1; i++) { sec_stream << tab; }
			sec_stream << '"' << pair.first << "\": " << pair.second.stringify(tabSpace, initTabPos + 1, true) << ',' << std::endl;
		}
		sec_str = sec_stream.str();
		sec_str = sec_str.substr(0, sec_str.length() - 2);
		ret << sec_str << std::endl;

		for (int i = 0; i < initTabPos; i++) { ret << tab; }
		ret << '}';
		break;
	}

	return ret.str();
}

std::vector<json_data> json::getObj(std::string str, int& i, bool breakAfterAdd, int last_i_offset, bool endBracket) {
	std::vector<json_data> ret;

	char c;
	std::string val = "";
	int openCount = 0;
	bool quoteOpen = false;

	std::string openDelimeters = "[{";
	std::string closeDelimeters = "]}";

	for (; i < str.length() - last_i_offset; i++) {
		c = str[i];

		if (isEmpty(c)) {
			// ignore whitespace unless in string
			if (quoteOpen) {
				val += c;
			}
			continue;
		}

		// verify character (cannot be escape char)
		if (str[i - 1] != '\\') {
			if (c == '"') {
				openCount += (quoteOpen ? -1 : 1);

				quoteOpen = !quoteOpen;
			}

			if (strContains(openDelimeters, c)) {
				// open section
				openCount++;
			}
			else if (strContains(closeDelimeters, c)) {
				// close section
				openCount--;
			}
		}

		if (openCount <= 0 && val != "" && (c == ',' || (endBracket && c == '}'))) {
			ret.push_back(json_data::parse(val));
			if (breakAfterAdd) {
				return ret;
			}
			val = "";
		}
		else {
			val += c;
		}
	}

	ret.push_back(json_data::parse(val));

	return ret;
}

json_data json_data::parse(std::string val) {
	// identify type based on first character
	char c = val[0];

	if (firstAndLastMatch(val, '"', '"')) {
		// string
		return json_string(val.substr(1, val.length() - 2));
	}
	else if (isNum(c)) {
		// number
		return json_num(std::atof(val.c_str()));
	}
	else if (c == 't') {
		// might be true
		if (val == json_bool::TRUE) {
			return json_bool(true);
		}
	}
	else if (c == 'f') {
		// might be false
		if (val == json_bool::FALSE) {
			return json_bool(false);
		}
	}
	else if (firstAndLastMatch(val, '[', ']')) {
		// list
		int i = 1;
		
		return getObj(val, i);
	}
	else if (firstAndLastMatch(val, '{', '}')) {
		// json object

		int i = 1;

		std::string key = "";
		json_data value;

		std::map<std::string, json_data> ret;

		while (i < val.length()) {
			// GET KEY=====================
			c = val[i];

			// get first character
			while (isEmpty(c) && i < val.length()) {
				c = val[++i];
			}

			if (c != '"') {
				// invalid key
				break;
			}

			c = val[++i];

			// find end of string while c not valid quote
			for (; c != '"' && val[i - 1] != '\\'; c = val[++i]) {
				key += c;
			}

			c = val[++i];

			// skip to item
			while (isEmpty(c) || c == ':') {
				c = val[++i];
			}

			std::vector<json_data> retv = getObj(val, i, true, 0, true);

			i++;

			ret[key] = retv[0];
			key = "";
			value = json_data();
		}

		return ret;
	}

	return json_data();
}