
#ifndef cwlib_cwabt_interpreter_hpp
#define cwlib_cwabt_interpreter_hpp

#include <iostream>
#include <variant>

#include <string>
#include <iomanip>
#include <utility>
#include <iostream>
#include <stdexcept>

#define ALIB_JSON_NLOHHMAN
#include <nlohmann/json/json.hpp>
#include "../utils.hpp"


#include "../Vectors.hpp"

using json = nlohmann::json;
#define JSONREF json&
struct ABTDataStub : public nlohmann::json {
	void deserialize_json(std::string d) {
		nlohmann::json j = nlohmann::json::parse(d);
		this->clear();
		std::istringstream(d) >> *this;
	}
	std::string serialize_json() {
		return this->dump();
	}

	std::string get_string(std::string key) {
		return this->contains(key) ? this->at(key) : "nil";
	}
	int get_int(std::string key) {
		return this->contains(key) ? this->at(key).get<int>(): 0;
	}
	float get_float(std::string key) {
		return this->contains(key) ? this->at(key).get < float > () : 0;
	}
	bool get_bool(std::string key) {
		return this->contains(key) ? this->at(key).get < bool >() : false;
	}
	std::string get_or_default_string(std::string key, std::string m_default) {
		return this->contains(key) ? this->at(key).get<std::string>() : m_default;
	}
	int get_or_default_int(std::string key, int m_default) {
		return this->contains(key) ? this->at(key).get<int>() : m_default;
	}
	float get_or_default_float(std::string key, float m_default) {
		return this->contains(key) ? this->at(key).get < float >() : m_default;
	}
	bool get_or_default_bool(std::string key, bool m_default) {
		return this->contains(key) ? this->at(key).get < bool >() : m_default;
	}
	// Not very efficient! Call once per init please!
	std::vector<std::string> get_keys() {
		std::vector<std::string> out;
		for (auto it = this->begin(); it != this->end(); ++it) {
			out.push_back(it.key());
		}
		return out;
	}
	bool m_contains(std::string key) {
		return this->contains(key);
	}
	void set_float(std::string k, float v) {
		(*this)[k] = v;
	}
	void set_int(std::string k, int v) {
		(*this)[k] = v;
	}
	void set_string(std::string k, std::string v) {
		(*this)[k] = v;
	}
	void set_bool(std::string k, bool v) {
		(*this)[k] = v;
	}

	void add_float(std::string k) {
		(*this)[k] = 0.0f;
	}
	void add_int(std::string k) {
		(*this)[k] = 0;
	}
	void add_string(std::string k) {
		(*this)[k] = "nil";
	}
	void add_bool(std::string k) {
		(*this)[k] = false;
	}

};

struct ABT {
	ABTDataStub data;
	auto & operator[](std::string k) {
		return data[k];
	}
	void invalidate() {
		if (data.type() == json::value_t::null) { return; }
		data.clear();
	}
	void decompile(const char* filen) {
		data.deserialize_json(alib_file_read(filen).c_str());
	}
	void compile(const char* filen) {
		alib_file_write(filen, data.serialize_json().c_str());
	}
};
int abt_geti(JSONREF j) {
	return j.get<int>();
}
std::string abt_getstr(JSONREF j) {
	return j.get<std::string>();
}
const char* abt__getchara(JSONREF j) {
	return abt_getstr(j).c_str();
}


#undef JSONREF


#endif	