#ifndef JSON_HPP
#define JSON_HPP

#include <map>
#include <vector>
#include <string>
#include <initializer_list>
#include <fstream>
#include <sstream>

namespace json {
	// ================================ callbacks/reference
	class json_data;
	class json_string;
	class json_num;
	class json_bool;
	class json_list;
	class json_object;

	enum class json_type {
		json_string,
		json_num,
		json_bool,
		json_list,
		json_object
	};

	// ================================== utility methods
	bool firstAndLastMatch(std::string str, char first, char last) {
		return str[0] == first && str[str.length() - 1] == last;
	}

	bool strContains(std::string str, char c) {
		return str.find(c) != std::string::npos;
	}

	bool isNum(char c) {
		return strContains("0123456789.-", c);
	}

	bool isEmpty(char c) {
		return c == ' ' || c == '\n' || c == '\t';
	}

	bool writeFile(const char* filename, std::string content) {
		std::ofstream file(filename);

		if (file.is_open()) {
			file << content;
			file.close();
			return true;
		}

		return false;
	}

	std::string readFile(const char* filename) {
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

	class json_data {
	public:
		json_type type;

		// constructors
		json_data(json_type type = json_type::json_object)
			: type(type) {}
		json_data(std::string val)
			: type(json_type::json_string), s_val(val) {}
		json_data(const char* val)
			: type(json_type::json_string), s_val(val) {}
		json_data(double val)
			: type(json_type::json_num), n_val(val) {}
		json_data(int val)
			: type(json_type::json_num), n_val(val) {}
		json_data(float val)
			: type(json_type::json_num), n_val(val) {}
		json_data(bool val)
			: type(json_type::json_bool), b_val(val) {}
		json_data(std::vector<json_data> val)
			: type(json_type::json_list), l_val(val) {}
		json_data(std::initializer_list<json_data> val)
			: type(json_type::json_list), l_val(val) {}
		json_data(std::map<std::string, json_data> val)
			: type(json_type::json_object), o_val(val) {}
		json_data(std::initializer_list<std::pair<const std::string, json_data>> val)
			: type(json_type::json_object), o_val(val) {}

		// operators
		void operator=(std::string val) { type = json_type::json_string; s_val = val; }
		void operator=(const char* val) { type = json_type::json_string; s_val = val; }
		void operator=(double val) { type = json_type::json_num; n_val = val; }
		void operator=(int val) { type = json_type::json_num; n_val = val; }
		void operator=(float val) { type = json_type::json_num; n_val = val; }
		void operator=(bool val) { type = json_type::json_bool; b_val = val; }
		void operator=(std::vector<json_data> val) { type = json_type::json_list; l_val = val; }
		void operator=(std::initializer_list<json_data> val) { type = json_type::json_list; l_val = val; }
		void operator=(std::map<std::string, json_data> val) { type = json_type::json_object; o_val = val; }
		void operator=(std::initializer_list<std::pair<const std::string, json_data>> val) { type = json_type::json_object; o_val = val; }

		json_data& operator[](int idx) { return l_val[idx]; }
		json_data& operator[](std::string key) { return o_val[key]; }

		std::string stringify(int tabSpace = 4, int initTabPos = 0, bool isDictVal = false);
		std::string getType() {
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

	protected:
		std::string s_val;
		double n_val;
		bool b_val;
		std::vector<json_data> l_val;
		std::map<std::string, json_data> o_val;

	// parsing methods
	public:
		static std::vector<json_data> getObj(std::string str, int& i, bool breakAfterAdd = false, int last_i_offset = 1, bool endBracket = false) {
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
		static json_data parse(std::string str);
	};

	class json_string : public json_data {
	public:
		json_string(std::string val)
			: json_data(val) {}
		json_string(const char* val)
			: json_data(val) {}

		json_string(json_data d)
			: json_data(d) {}

		std::string val() { return s_val; }
	};

	class json_num : public json_data {
	public:
		json_num(double val)
			: json_data(val) {}
		json_num(int val)
			: json_data(val) {}
		json_num(float val)
			: json_data(val) {}

		json_num(json_data d)
			: json_data(d) {}

		double val() { return n_val; }
	};

	class json_bool : public json_data {
	public:
		static std::string TRUE;
		static std::string FALSE;

		json_bool(bool val)
			: json_data(val) {}

		json_bool(json_data d)
			: json_data(d) {}

		bool val() { return b_val; }
	};

	std::string json_bool::TRUE = "true";
	std::string json_bool::FALSE = "false";

	class json_list : public json_data {
	public:
		json_list(std::vector<json_data> val)
			: json_data(val) {}
		json_list(std::initializer_list<json_data> val)
			: json_data(val) {}

		json_list(json_data d)
			: json_data(d) {}

		std::vector<json_data> val() { return l_val; }

		void push(json_data val) { l_val.push_back(val); }
	};

	class json_object : public json_data {
	public:
		json_object(std::map<std::string, json_data> val)
			: json_data(val) {}
		json_object(std::initializer_list<std::pair<const std::string, json_data>> val)
			: json_data(val) {}

		json_object(json_data d)
			: json_data(d) {}

		std::map<std::string, json_data> val() { return o_val; }
	};

	// parse json object
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

			return json_data::getObj(val, i);
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

				std::vector<json_data> retv = json_data::getObj(val, i, true, 0, true);

				i++;

				ret[key] = retv[0];
				key = "";
				value = json_data();
			}

			return ret;
		}

		return json_data();
	}

	// stringify json object
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

	// read json object from file
	json_data read(const char* filename) {
		return json_data::parse(readFile(filename));
	}

	// write json object to file
	bool write(const char* filename, json_data val) {
		return writeFile(filename, val.stringify());
	}
}

#endif // JSON_HPP