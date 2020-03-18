#ifndef JSON_H
#define JSON_H

#include <map>
#include <vector>
#include <string>
#include <initializer_list>

#include "util.h"

namespace json {
	// class callback
	class json_data;

	// method to get object
	std::vector<json_data> getObj(std::string str, int& i, bool breakAfterAdd = false, int last_i_offset = 1, bool endBracket = false);

	enum class json_type {
		json_string,
		json_num,
		json_bool,
		json_list,
		json_object
	};

	class json_util {
	public:
		static json_data read(const char* filename);

		static bool write(const char* filename, json_data val);
	};

	class json_data {
	public:
		static json_data parse(std::string str);

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
		std::string getType();

	protected:
		std::string s_val;
		double n_val;
		bool b_val;
		std::vector<json_data> l_val;
		std::map<std::string, json_data> o_val;
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
}

#endif