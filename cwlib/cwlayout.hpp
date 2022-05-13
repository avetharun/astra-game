#ifndef cwlib_cwl_interpreter_hpp
#define cwlib_cwl_interpreter_hpp
#include "_cwincludes.h"
#include "cwerror.h"
#include "cwasset.hpp"
#include "cwabt.hpp"
#include <stdio.h>
#include <ctype.h>
#include "../renderer.hpp"
#include "../GUIRenderer.h"


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

	// extremely slow at the moment. Only use once when loading!
	std::array<int, 2> parse_wh_pair(json stub, Sprite* _s = nullptr) {
		int w = 0;
		int h = 0;
		std::string w_str = alib_j_getstr(stub["w"]);
		std::string h_str = alib_j_getstr(stub["h"]);
		if (stub["w"].is_string() && alib_endswith(w_str.c_str(), "%") && _s != nullptr) {
			if (!w_str.empty()) {
				w_str.pop_back();
			}
			w = alib_scale_percent(_s->surf->w, atoi(w_str.c_str()));
		}
		else {
			w = alib_j_geti(stub["w"]);
		}
		if (stub["h"].is_string() && alib_endswith(h_str.c_str(), "%") && _s != nullptr) {
			if (!h_str.empty()) {
				h_str.pop_back();
			}
			h = alib_scale_percent(_s->surf->h, atoi(h_str.c_str()));
		}
		else {
			w = alib_j_geti(stub["h"]);
		}
		return {w,h};
	}
	Vector2 parse_vector2(json stub) {
		return { alib_j_geti(stub["x"]),alib_j_geti(stub["y"])};
	}
	std::map<std::string, json> template_assets;
	// Needed to ::discard the sprite when unused by the scene.
	std::map<std::string, Sprite*> sprite_assets;
	std::map<std::string, UI::ImageElement*> ui_img_elem_assets;
	std::map<std::string, UI::TextElement*> ui_txt_elem_assets;
	Sprite* parse_scene_sprite(json& stub) {
		if (alib_j_cokeys(stub, "asset_data\nfilename")) {
			Sprite* _s = new Sprite(alib_j_getstr(stub["asset_data"]["filename"]).c_str());
			_s->setID(alib_j_getstr(stub["id"]).c_str());
			json stub_ad = stub["asset_data"];
			json stub_tr = stub["asset_data"]["transform"];
			int ox, oy;
			if (!alib_j_cokeys(stub_ad, "origin")) {
				ox = 0;
				oy = 0;
			}
			else {
				ox = stub_tr["origin"]["x"];
				oy = stub_tr["origin"]["y"];
			}
			std::array<int, 2> wh_pair = parse_wh_pair(stub_tr, _s);
			int w = wh_pair.at(0);
			int h = wh_pair.at(1);
			int x = alib_j_geti(stub_tr["x"]);
			int y = alib_j_geti(stub_tr["y"]);
			printf("Created sprite with %d, %d at %d, %d using origin %d, %d", w, h, x, y, ox, oy);
			_s->SetTransform({
				{ x, y}, // pos
				{ w, h}, // scale
				{ ox, oy}, // origin
				0   // angle
				});
			if (alib_j_cokeys(stub_ad, "layer")) {
				if (alib_j_streq(stub_ad["layer"], "top")) {
					_s->layer = INT32_MAX;
				}
				else if (alib_j_streq(stub_ad["layer"], "bottom")) {
					_s->layer = INT32_MIN;
				}
				else {
					_s->layer = alib_j_geti(stub_ad["layer"]);
				}
			}
			sprite_assets.insert({ stub["id"], _s});
			return _s;
		}
		return nullptr;
	}
	void parse_scene_assets() {
		if (!this->data["layout"].is_array()) { return; }
		size_t assets_count = this->data["layout"].size();
		for (int i = 0; i < assets_count; i++) {
			try {
				json stub = this->data["layout"][i];
				if (stub.contains("schema")) {
					if (alib_j_costr(stub["schema"], "image") && !alib_j_costr(stub["schema"], "animated") && !alib_j_costr(stub["schema"], "ui")) {
						parse_scene_sprite(stub);
						continue;
					}
					if (alib_j_costr(stub["schema"], "ui")) {
						if (alib_j_costr(stub["schema"], "image")) {
							printf("\nloading ui image\n");
							UI::ImageElement* _imgel = new UI::ImageElement(alib_j_getstr(stub["asset_data"]["filename"]).c_str());
							ui_img_elem_assets.insert({stub["id"], _imgel });
							if (alib_j_cokeys(stub["asset_data"], "uv")) {
								if (stub["asset_data"]["uv"].is_array()) {
									json uvstub = stub["asset_data"]["uv"];
									_imgel->uv.x = uvstub[0][0];
									_imgel->uv.y = uvstub[0][1];
									_imgel->uv.w = uvstub[1][0];
									_imgel->uv.h = uvstub[1][1];
								}
							}
							continue;
						}
						if (alib_j_costr(stub["schema"], "text")) {
							printf("Creating UI text\n");
							UI::TextElement _t = UI::TextElement();
							_t.text = stub["asset_data"]["text"].get<std::string>();
							_t.enabled = 1;
							if (alib_j_cokeys(stub["asset_data"], "pos")) {
								_t.pos = parse_vector2(stub["asset_data"]["pos"]);
							}
							continue;
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
		this->invalidate();
		alib_invalidatem(sprite_assets);
		alib_invalidatem(ui_img_elem_assets);
		this->operator~();
	}
	void Save() {
		
	}
	void operator ~() {
		delete this;
	}
};
typedef cwLayout CWLScene;


#endif