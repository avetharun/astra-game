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


struct cwLayout : public ABT {
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
		if (data.contains("autoexec")) {}
		if (data.contains("layout")) {
			parse_scene_assets();
		}
		cwlayoutfile.clear();
	}
private:
	void parse_wh(Sprite* _s, std::string w_in, std::string h_in, int* w_out, int* h_out) {
		*w_out = alib_percents(_s->surf->w, w_in);
		*h_out = alib_percents(_s->surf->h, h_in);
	}
	Vector2 parse_vector2(json stub) {
		return { alib_j_getd(stub["x"]),alib_j_getd(stub["y"])};
	}
	Vector2 parse_vector2a(json stub) {
		return { alib_j_getd(stub[0]),alib_j_getd(stub[1]) };
	}
	std::map<std::string, json> template_assets;
	std::map<std::string, Sprite*> sprite_assets;
	std::map<std::string, UI::ImageElement*> ui_img_elem_assets;
	std::map<std::string, UI::TextElement*> ui_txt_elem_assets;
	MeshCollider2d* __parse_scene_mesh(json& stub) {
		json stub_ad = stub["asset_data"];
		json stub_lines = stub["asset_data"].at("lines");
		size_t line_amt = stub_lines.size();
		printf("Starting to create scene mesh.. Amount of lines: %zi\n", line_amt);
		
		MeshCollider2d* _mesh_ = new MeshCollider2d();
		int __layer = COL_EMPTY;
		if (stub_ad.contains("layer")) {
			std::string _s = alib_lowers(alib_j_getstr(stub_ad.at("layer")));
			if (alib_streq(_s, "solid")) {
				__layer = COL_SOLID;
			}
			if (alib_streq(_s, "empty")) {
				__layer = COL_EMPTY;
			}
		}
		//int __layer = stub_ad["layer"];
		for (int i = 0; i < line_amt; i++) {
			MeshLine* _m = new MeshLine();
			_m->layer = __layer;
			_m->start = parse_vector2a(stub_lines.at(i).at(0));
			_m->end = parse_vector2a(stub_lines.at(i).at(1));
			_mesh_->lines.push_back(_m);
			printf("Created line from (%f, %f) to (%f, %f)", _m->start.x, _m->start.y, _m->end.x, _m->end.y);
		}
		return _mesh_;
	}
	Sprite* parse_scene_sprite(json& stub) {
		if (alib_j_cokeys(stub, "asset_data\nfilename")) {
			json stub_ad = stub["asset_data"];
			json stub_tr = stub["asset_data"]["transform"];
			double ox, oy;
			double x = 0, y = 0, w, h;
			SDL_Rect __uv = {0,0,0,0};
			if (alib_j_cokeys(stub["asset_data"], "uv")) {
				if (stub["asset_data"]["uv"].is_array()) {
					json uvstub = stub["asset_data"]["uv"];
					__uv.x = uvstub[0][0].get<int>();
					__uv.y = uvstub[0][1].get<int>();
					__uv.w = uvstub[1][0].get<int>();
					__uv.h = uvstub[1][1].get<int>();
				}
			}
			Sprite* _s = new Sprite(alib_j_getstr(stub["asset_data"]["filename"]).c_str(), __uv);
			_s->setID(alib_j_getstr(stub["id"]));
			if (stub_tr.at("w").is_string()) { w = alib_percents(_s->uv.w, stub_tr.at("w").get<std::string>()); } else {w = stub_tr.at("w").get<int>();}
			if (stub_tr.at("h").is_string()) { h = alib_percents(_s->uv.h, stub_tr.at("h").get<std::string>()); } else {h = stub_tr.at("h").get<int>();}
			if (!alib_j_cokeys(stub_ad, "origin")) {
				ox = (int)(w * 0.5);
				oy = (int)(h * 0.5);
			}
			else {
				ox = stub_tr["origin"]["x"];
				oy = stub_tr["origin"]["y"];
			}
			if ((stub_tr.contains("x")) && stub_tr.at("x").is_number_integer()) { x = stub_tr.at("x").get<int>(); }
			if ((stub_tr.contains("y")) && stub_tr.at("y").is_number_integer()) { y = stub_tr.at("y").get<int>(); }
			printf("CW: Created world sprite with %f, %f at %f, %f using origin %f, %f\n", w, h, x, y, ox, oy);
			_s->transform = Transform{
				{x,y},
				{w,h},
				{ox, oy},
				0
			};
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
			
			sprite_assets.insert({ stub.at("id"), (_s) });
			return _s;
		}
		return nullptr;
	}
	void parse_scene_assets() {
		if (!this->data.contains("layout")) { return; }
		if (!this->data.at("layout").is_array()) { return; }
		size_t assets_count = this->data.at("layout").size();
		for (int i = 0; i < assets_count; i++) {
			try {
				json stub = this->data.at("layout").at(i);
				if (stub.contains("schema")) {
					if (alib_j_costr(stub["schema"], "mesh")) {
						MeshCollider2d* _mesh = __parse_scene_mesh(stub);
					}
					if (alib_j_costr(stub["schema"], "ui")) {
						if (alib_j_costr(stub["schema"], "image")) {
							UI::ImageElement* _imgel = new UI::ImageElement(alib_j_getstr(stub["asset_data"]["filename"]).c_str());
							ui_img_elem_assets.insert({stub["id"], _imgel});
							if (alib_j_cokeys(stub["asset_data"], "transform")) {
								json tstub = stub["asset_data"]["transform"];
								if (tstub.contains("x")) { _imgel->transform.position.x = tstub["x"].get<int>(); }
								if (tstub.contains("y")) { _imgel->transform.position.x = tstub["y"].get<int>(); }
								if (tstub.contains("w")) { _imgel->transform.scale.x = tstub["w"].get<int>(); }
								if (tstub.contains("h")) { _imgel->transform.scale.y = tstub["h"].get<int>(); }
							}
							if (alib_j_cokeys(stub["asset_data"], "uv")) {
								if (stub["asset_data"]["uv"].is_array()) {
									json uvstub = stub["asset_data"]["uv"];
									_imgel->uv.x = uvstub[0][0].get<int>();
									_imgel->uv.y = uvstub[0][1].get<int>();
									_imgel->uv.w = uvstub[1][0].get<int>();
									_imgel->uv.h = uvstub[1][1].get<int>();
								}
							}
							continue;
						}
						if (alib_j_costr(stub["schema"], "text")) {
							printf("Creating UI text\n");
							UI::TextElement* _t = new UI::TextElement();
							_t->text = stub["asset_data"]["text"].get<std::string>();
							_t->enabled = 1;
							if (alib_j_cokeys(stub["asset_data"], "pos")) {
								_t->pos = parse_vector2(stub["asset_data"]["pos"]);
							}
							ui_txt_elem_assets.insert({ stub.at("id"), _t});
							continue;
						}
						continue;
					}
					if (alib_j_costr(stub["schema"], "image") && !alib_j_costr(stub["schema"], "animated")) {
						parse_scene_sprite(stub);
						continue;
					}
				}
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

		for (const auto& kv : sprite_assets) {
			alib_remove_any_of<Sprite*>(Sprite::_mglobalspritearr, kv.second);
		}
		for (const auto& kv : ui_img_elem_assets) {
			delete kv.second;
		}
		sprite_assets.clear();
		ui_img_elem_assets.clear();
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