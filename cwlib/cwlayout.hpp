#pragma once
#ifndef cwlib_cwl_interpreter_hpp
#define cwlib_cwl_interpreter_hpp
#include "_cwincludes.h"
#include "cwerror.h"
#include "cwasset.hpp"
#include "cwabt.hpp"
#include <stdio.h>
#include <ctype.h>
#include "../renderer.hpp"



struct cwLayout : private ABT {
	bool isBinary;
	const std::string name;
	cwLayout(const char* filename) {
		const char* lbytes;
		size_t fsize;
		alib_file_read(filename, &lbytes, &fsize);
		std::stringstream cwlayoutfile(lbytes);
		cwError::sstate(cwError::CW_DEBUG);
		cwError::serrof("Loading CW layout %s\n", filename);
		this->decompile(filename);
		static_cast<std::string>(name) = filename;
		if (data.contains("layout")) {
			parse_scene_assets();
		}

		cwlayoutfile.clear();
	}

private:
	std::map<std::string, json> template_assets;
	void parse_scene_assets() {
		if (!this->data["layout"].is_array()) { return; }
		size_t assets_count = this->data["layout"].size();
		for (int i = 0; i < assets_count; i++) {
			try {
				json stub = this->data["layout"][i];
				if (stub.contains("type")) {
					if (alib_j_streq(stub["type"], "texture")) {
						if (alib_j_contkeys(stub, "asset_data\nfilename")) {
							Sprite* _s = new Sprite(alib_j_getstr(stub["asset_data"]["filename"]).c_str());
							_s->setID(alib_j_getstr(stub["id"]).c_str());
							json stub_ad = stub["asset_data"];
							json stub_tr = stub["asset_data"]["transform"];
							if (!alib_j_contkeys(stub_ad, "origin\n")) {
								stub_tr["origin"]["x"] = 0;
								stub_tr["origin"]["y"] = 0;
							}
							_s->SetTransform({
								{ alib_j_geti(stub_tr["x"]), alib_j_geti(stub_tr["x"])}, // pos
								{ alib_j_geti(stub_tr["w"]), alib_j_geti(stub_tr["h"])}, // scale
								{ alib_j_geti(stub_tr["origin"]["x"]), alib_j_geti(stub_tr["origin"]["y"])}, // origin
								0   // angle
							});
						}
					}
				}
			}
			catch (json::type_error e) {
				cwError::sstate(cwError::CW_ERROR);
				cwError::serrof("TypeError when reading CWL file: what():%s | id %d", e.what(), e.id);
			}
			catch (json::parse_error e) {
				cwError::sstate(cwError::CW_ERROR);
				cwError::serrof("ParseError when reading cwl file: what():%s | offset %zi", e.what(), e.byte);
			}
		}
	}
public:
	void Discard() {
		if (this == nullptr) { return; }
		this->invalidate();
	}
	void Save() {
		
	}
};
typedef cwLayout CWLScene;


#endif