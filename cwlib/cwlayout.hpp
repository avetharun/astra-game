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
	enum AssetType : long long {
		MESH =			B32(00000000, 00000000, 00000000, 00000010),
		TRIGGER =		B32(00000000, 00000000, 00000000, 00000100),
		IMAGE	=		B32(00000000, 00000000, 00000000, 00001000),
		UI		=		B32(00000000, 00000000, 00000000, 00010000)
	};
	bool dirty, editor_loaded;
	std::string version;
	std::string autoexec;
	std::string name;
	cwLayout(const char* filename) {
		const char* lbytes = nullptr;
		size_t fsize;
		alib_file_read(filename, lbytes, &fsize);
		cwError::sstate(cwError::CW_DEBUG);
		cwError::serrof("Loading CW layout %s\n", filename);
		this->decompile(filename);
		name = filename;
		if (data.contains("autoexec")) {}
		if (data.contains("layout")) {
			parse_scene_assets();
		}
		free((void*)lbytes);
	}
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
	SDL_Rect parse_vector4as(json stub) {
		return { alib_j_geti(stub[0]),alib_j_geti(stub[1]),alib_j_geti(stub[2]), alib_j_geti(stub[3]) };
	}
	VectorRect parse_vector4a(json stub) {
		return { alib_j_getd(stub[0]),alib_j_getd(stub[1]),alib_j_getd(stub[2]), alib_j_getd(stub[3]) };
	}
	std::map<std::string, json> template_assets;
	std::map<std::string, Sprite*> sprite_assets;
	std::map<std::string, std::vector<std::string>> asset_schemas;
	std::map<std::string, VectorRect> asset_image_size;
	std::map<std::string, Transform*> asset_transforms;
	std::map<std::string, UI::ImageElement*> ui_img_elem_assets;
	std::map<std::string, UI::TextElement*> ui_txt_elem_assets;
	MeshCollider2d* thisMesh;
	MeshCollider2d* __parse_scene_mesh(json& stub) {
		json stub_ad = stub["asset_data"];
		json stub_lines = stub["asset_data"].at("lines");
		json stub_rects = stub["asset_data"].at("rects");
		size_t line_amt = stub_lines.size();
		size_t rect_amt = stub_rects.size();
		printf("Starting to create scene mesh.. Amount of lines: %zi\n", line_amt);
		thisMesh = new MeshCollider2d();
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
			if (stub_lines.at(i).array().size() > 1) {
				_m->id = stub_lines.at(i).at(2);
			}
			if (stub_lines.at(i).size() > 2) {
				if (stub_lines.at(i).at(2).is_string()) {
					_m->layer = hrtocol(stub_lines.at(i).at(2));
				}
				else {
					_m->layer = stub_lines.at(i).at(2);
				}
			}
			thisMesh->lines.push_back(_m);
			printf("Created line from (%f, %f) to (%f, %f) with id %s\n", _m->start.x, _m->start.y, _m->end.x, _m->end.y, _m->id.c_str());
		}
		for (int i = 0; i < rect_amt; i++) {
			RectCollider2d* _m = new RectCollider2d();
			_m->layer = __layer;
			*_m->ws_rect = parse_vector4as(stub_rects.at(i));
			
			if (stub_rects.at(i).size() > 3) {
				_m->id = stub_rects.at(i).at(4);
			}
			if (stub_rects.at(i).size() > 4) {
				if (stub_rects.at(i).at(5).is_string()) {
					_m->layer = hrtocol(stub_rects.at(i).at(5));
				}
				else {
					_m->layer = stub_rects.at(i).at(5);
				}
			}
			printf("Created rect from (%d, %d) to (%d, %d) with id %s\n", _m->ws_rect->x, _m->ws_rect->y, _m->ws_rect->x + _m->ws_rect->w, _m->ws_rect->h + _m->ws_rect->y, _m->id.c_str());
		}
		return thisMesh;
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
			std::string id = stub["id"].get<std::string>();
			Sprite* _s = new Sprite(alib_j_getstr(stub["asset_data"]["filename"]).c_str(), __uv);
			_s->setID(id);
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
			printf("CW: Created world sprite %s with %f, %f at %f, %f using origin %f, %f\n", _s->identifier, w, h, x, y, ox, oy);
			this->asset_image_size.insert({ stub["id"], {x,y,w,h} });
			_s->transform = Transform{
				{x,y},
				{w,h},
				{ox, oy},
				0
			};
			this->asset_transforms.insert({ stub.at("id"), &_s->transform });
			if (alib_j_cokeys(stub_ad, "layer")) {
				if (stub_ad["layer"].is_string()) {
					if (alib_j_streq(stub_ad["layer"], "top")) {
						_s->layer = INT32_MAX;
					}
					else if (alib_j_streq(stub_ad["layer"], "bottom")) {
						_s->layer = INT32_MIN;
					}
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
		if (this->data.contains("autoexec")) {
			this->autoexec = this->data.at("autoexec");
		}
		if (this->data.contains("version")) {
			this->version = this->data.at("version");
		}
		size_t assets_count = this->data.at("layout").size();
		for (int i = 0; i < assets_count; i++) {
			try {
				json stub = this->data.at("layout").at(i);
				if (stub.contains("schema") && stub.contains("id")) {
					std::vector<std::string> _s;
					if (!stub.at("schema").is_array()) {
						_s.push_back(stub.at("schema"));
					}
					for (int i = 0; i < stub.at("schema").size(); i++) {
						_s.push_back(stub.at("schema").at(i));
					}
					this->asset_schemas.insert({stub.at("id"), _s});
					if (alib_j_costr(stub["schema"], "mesh")) {
						MeshCollider2d* _mesh = __parse_scene_mesh(stub);
					}
					if (alib_j_costr(stub["schema"], "ui")) {
						if (alib_j_costr(stub["schema"], "image")) {
							UI::ImageElement* _imgel = new UI::ImageElement(alib_j_getstr(stub["asset_data"]["filename"]).c_str());
							if (alib_j_costr(stub["schema"], "centered")) {
								_imgel->centered = true;
							}
							ui_img_elem_assets.insert({stub["id"], _imgel});
							if (alib_j_cokeys(stub["asset_data"], "transform")) {
								json tstub = stub["asset_data"]["transform"];
								if (tstub.contains("x")) { _imgel->transform.position.x = tstub["x"].get<int>(); }
								if (tstub.contains("y")) { _imgel->transform.position.x = tstub["y"].get<int>(); }
								if (tstub.contains("w")) { _imgel->transform.scale.x = tstub["w"].get<int>(); }
								if (tstub.contains("h")) { _imgel->transform.scale.y = tstub["h"].get<int>(); }
								this->asset_image_size.insert({ stub["id"], {
									_imgel->transform.position.x,
									_imgel->transform.position.y,
									_imgel->transform.scale.x,
									_imgel->transform.scale.y
									}
								});
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
	std::function<void(void)> dtor = []() {};
	void Discard() {
		if (this == nullptr) { return; }
		this->invalidate();

		for (int i = 0; i < Sprite::_mglobalspritearr.size(); i++) {
			if (this->sprite_assets.contains(Sprite::_mglobalspritearr.at(i)->identifier)) {
				Sprite::_mglobalspritearr.erase(Sprite::_mglobalspritearr.begin() + i);
			}
		}
		MeshCollider2d::_mGlobalColArr.erase(std::find(MeshCollider2d::_mGlobalColArr.begin(), MeshCollider2d::_mGlobalColArr.end(), this->thisMesh));
		for (const auto& kv : ui_img_elem_assets) {
			try {
				kv.second->operator~();
			}
			catch (std::exception e) { printf("%s\n", e.what()); }
		}
		RectCollider2d::_mGlobalColArr.clear();

		sprite_assets.clear();
		ui_img_elem_assets.clear();
		dtor();
		this->operator~();
	}
	void Save() {
		nlohmann::json out;
		out["version"] = this->version;
		out["autoexec"] = this->autoexec;
		for (auto& pair : sprite_assets) {
			nlohmann::json asset{};
			asset["id"] = pair.first;
			if (asset_schemas.contains(pair.first)) {
				asset["schema"] = asset_schemas.at(pair.first);
			}
			Transform t = pair.second->transform;
			SDL_Rect u = pair.second->uv;
			json layer;
			if (pair.second->layer == INT32_MIN || pair.second->layer == INT32_MAX) {
				layer = pair.second->layer == INT32_MIN ? "bottom" : "top";
			}
			else {
				layer = pair.second->layer;
			}
			printf(pair.second->name.c_str());
			asset["asset_data"] = {
				{"filename", pair.second->name},
				{"layer", layer},
				{"transform", {
						{"x", (int)t.position.x}, {"y", (int)t.position.y},
						{"w", (int)t.scale.x}, {"h", (int)t.scale.y}
					}
				},
				{"uv", { {u.x, u.y}, {u.w, u.h} }}
			};
			out["layout"].push_back(asset);
		}

		{ // solid colliders
			nlohmann::json coll_asset{};
			nlohmann::json _coll_linedefs;
			nlohmann::json _coll_rectdefs;
			for (int i = 0; i < this->thisMesh->lines.size(); i++) {
				MeshLine l = *this->thisMesh->lines.at(i);
				_coll_linedefs.push_back({
					{l.start.x, l.start.y},
					{l.end.x, l.end.y},
					l.id, l.layer
					});
			}
			for (int i = 0; i < RectCollider2d::_mGlobalColArr.size(); i++) {
				RectCollider2d r = *RectCollider2d::_mGlobalColArr.at(i);
				_coll_rectdefs.push_back({
					r.ws_rect->x, r.ws_rect->y, r.ws_rect->w, r.ws_rect->h,
					r.id, r.layer
					});
			}
			coll_asset["id"] = "colliders";
			coll_asset["schema"] = { "mesh" };
			coll_asset["asset_data"] = {
				{"layer", "solid"},
				{"lines", _coll_linedefs},
				{"rects", _coll_rectdefs},
			};
			out["layout"].push_back(coll_asset);
		}
		for (int i = 0; i < ui_img_elem_assets.size(); i++) {
			UI::ImageElement _im = *std::next(ui_img_elem_assets.begin(),i)->second;
			out["layout"].push_back(
				{
					{"id", "overlay"},
					{"schema", {
						"ui", "image",
						_im.centered ? "centered" : "centered:nil"
					}},
					{"asset_data", {
						{"filename", _im.fname},
						{"transform", {
							{"x", _im.transform.position.x},
							{"y", _im.transform.position.y},
							{"w", _im.transform.scale.x},
							{"h", _im.transform.scale.y}
						}},
						{"uv", {
							{_im.uv.x, _im.uv.y},
							{_im.uv.w, _im.uv.h}
						}}
					}}
				}
			);
		}

		std::string sout = out.dump(1, '\t', true, nlohmann::detail::error_handler_t::ignore);
		alib_file_write(this->name.c_str(), sout.c_str(), sout.size());

	}
	void operator ~() {
		delete this;
	}
};
typedef cwLayout CWLScene;


#endif