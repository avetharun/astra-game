
#ifndef cwlib_cwabt_interpreter_hpp
#define cwlib_cwabt_interpreter_hpp

// type support
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/base_class.hpp>

// for doing the actual serialization
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
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
	void deserialize_json(const char* d) {
		nlohmann::json j = nlohmann::json::parse(d);
		this->clear();
		std::istringstream(d) >> *this;
	}
	std::string serialize_json() {
		return this->dump();
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