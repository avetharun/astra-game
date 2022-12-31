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
	std::vector<std::pair<std::string, Sprite*>> sprite_assets;
	std::map<std::string, std::vector<std::string>> asset_schemas;
	std::map<std::string, VectorRect> asset_image_size;
	std::map<std::string, Transform*> asset_transforms;
	std::map<std::string, UI::ImageElement*> ui_img_elem_assets;
	std::map<std::string, UI::TextElement*> ui_txt_elem_assets;


	nlohmann::json ParseLayerR(int layer) {
		if (layer == INT32_MIN) { return "bottom"; }
		if (layer == INT32_MAX) { return "top"; }
		return layer;
	}
	int ParseLayerS(nlohmann::json layer) {
		if (layer == "bottom") { return INT32_MIN; }
		if (layer == "top") { return INT32_MAX; }
		return layer;
	}
	nlohmann::json ParseParticleRectR(VectorRect r) {
		return {
			{"x", r.x},
			{"y", r.y},
			{"w", r.w},
			{"h", r.h}
		};
	}
	Vector2 ParseParticleVec2S(nlohmann::json r) {
		return { alib_j_getf(r["x"]), alib_j_getf(r["y"]) };
	}
	nlohmann::json ParseParticleVec2R(Vector2 r) {
		return { {"x", r.x}, {"y",r.y} };
	}
	VectorRect ParseParticleRectS(nlohmann::json r) {
		return VectorRect(alib_j_getf(r["x"]), alib_j_getf(r["y"]), alib_j_getf(r["w"]), alib_j_getf(r["h"]));
	}

	SDL_Rect ParseParticleSDLRectS(nlohmann::json r) {
		return SDL_Rect(alib_j_getf(r["x"]), alib_j_getf(r["y"]), alib_j_getf(r["w"]), alib_j_getf(r["h"]));
	}

	MeshCollider2d* thisMesh;
	MeshCollider2d* __parse_scene_mesh(json& stub) {
		json stub_ad = stub["asset_data"];
		json stub_lines = stub["asset_data"].at("lines");
		json stub_rects = stub["asset_data"].at("rects");
		size_t line_amt = stub_lines.size();
		size_t rect_amt = stub_rects.size();
		printf("Starting to create scene mesh.. Amount of lines: %zi\n", line_amt);
		thisMesh = new MeshCollider2d();
		//int __layer = stub_ad["layer"];
		for (int i = 0; i < line_amt; i++) {
			MeshLine* _m = new MeshLine();
			_m->start = parse_vector2a(stub_lines.at(i).at(0));
			_m->end = parse_vector2a(stub_lines.at(i).at(1));
			if (stub_lines.at(i).size() > 1) {
				_m->id = stub_lines.at(i).at(2);
				printf(_m->id.c_str());
			}
			if (stub_lines.at(i).size() > 2) {
				if (stub_lines.at(i).at(3).is_string()) {
					_m->layer = hrtocol(stub_lines.at(i).at(3));
				}
				else {
					_m->layer = stub_lines.at(i).at(3);
				}
			}
			thisMesh->lines.push_back(_m);
			printf("Created line from (%f, %f) to (%f, %f) with id %s\n", _m->start.x, _m->start.y, _m->end.x, _m->end.y, _m->id.c_str());
		}
		for (int i = 0; i < rect_amt; i++) {
			RectCollider2d* _m = new RectCollider2d();
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
			std::string id = stub["id"].get<std::string>();
			double ox, oy;
			double x = 0, y = 0, w, h;
			SDL_Rect __uv = {0,0,0,0};
			SpriteAnimationMeta _meta;
			if (alib_j_cokeys(stub["asset_data"], "uv")) {
				if (stub["asset_data"]["uv"].is_array()) {
					json uvstub = stub["asset_data"]["uv"];
					__uv.x = uvstub[0][0].get<int>();
					__uv.y = uvstub[0][1].get<int>();
					__uv.w = uvstub[1][0].get<int>();
					__uv.h = uvstub[1][1].get<int>();
				}
			}
			if (alib_j_cokeys(stub["asset_data"], "meta")) {
				auto& stub_meta = stub["asset_data"]["meta"];
				if (stub_meta.contains("delay")) { _meta.delay = stub_meta["delay"]; }
				if (stub_meta.contains("frames")) { _meta.frames= stub_meta["frames"]; }
				if (stub_meta.contains("animate")) { _meta.animate = stub_meta["animate"]; }
			}
			Sprite* _s = new Sprite(alib_j_getstr(stub["asset_data"]["filename"]).c_str(), __uv);
			_s->meta = _meta;
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
			printf("CW: Created world sprite %s with %f, %f at %f, %f using origin %f, %f\n", _s->identifier.c_str(), w, h, x, y, ox, oy);
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
			
			sprite_assets.push_back({ stub.at("id"), (_s) });
			return _s;
		}
		return nullptr;
	}
	/*
	
			asset["asset_data"] = {
				{"layer", ParseLayerI(pe.second->m_layer)},
				{"particle_type", "custom"},
				{"lifetime", pe.second->m_lifetime},
				{"count", pe.second->amt},
				{"rect", ParseParticleRectR(*pe.second->m_particle_sprite->rect)},
				{"dir", ParseParticleVec2R(pe.second->m_direction)},
				{"pos", ParseParticleVec2R(pe.second->start_pos)},
				{"randomness", ParseParticleVec2R(pe.second->m_randomness)},
				{"velocity", pe.second->m_velocity}
			};
	*/
	void parse_scene_particle(nlohmann::json a_stub) {
		ParticleEffects::stub _customstub;
		
		nlohmann::json ad_stub = a_stub["asset_data"];
		_customstub.size = parse_vector2a(ad_stub["size"]);
		nlohmann::json _u = ad_stub["uv"];
		_customstub.uv = { alib_j_geti(_u["x"]), _u["y"], _u["w"], _u["h"]};
		_customstub.layer = ParseLayerS(ad_stub["layer"]);
		_customstub.lifetime = ad_stub["lifetime"];
		_customstub.randomness = ParseParticleVec2S(ad_stub["randomness"]);
		_customstub.velocity = ad_stub["velocity"];
		_customstub.id = a_stub["id"];
		ParticleEffect* e = new ParticleEffect(_customstub, ad_stub["count"]);
		e->start_pos = ParseParticleVec2S(ad_stub["pos"]);
		e->m_dir_angle_rad = alib_deg2rad(ad_stub["dir"]);
		ParticleEffect::m_particle_arr.push_back(e);
		

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
					if (alib_j_costr(stub["schema"], "particle")) {
						parse_scene_particle(stub);
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
		for (int i = 0; i < sprite_assets.size(); i++) {
			if (Sprite::getElementByID(sprite_assets.at(i).first)) {
				Sprite::_mglobalspritearr.erase(Sprite::_mglobalspritearr.begin() + alib_FindValueInVector(Sprite::_mglobalspritearr, sprite_assets.at(i).second));
			}
		}
		for (int i = 0; i < ParticleEffect::m_particle_arr.size(); i++) {
			auto& ep = ParticleEffect::m_particle_arr.at(i);
			ep->operator~();
		}

		ParticleEffect::m_particle_arr.clear();
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
		for (auto& pe: ParticleEffect::m_particle_arr) {
			nlohmann::json asset{};
			asset["id"] = pe->name;
			asset["schema"] = { "particle" };
			asset["asset_data"] = {
				{"layer", ParseLayerR(pe->m_layer)},
				{"lifetime", pe->m_lifetime},
				{"count", pe->amt},
				{"size", {pe->effect.size.x, pe->effect.size.y}},
				{"dir", alib_rad2deg(pe->m_dir_angle_rad)},
				{"pos", ParseParticleVec2R(pe->start_pos)},
				{"randomness", ParseParticleVec2R(pe->m_randomness)},
				{"velocity", pe->m_velocity},
				{"uv", {
					{"x",pe->m_uv.x},
					{"y",pe->m_uv.y},
					{"w",pe->m_uv.w},
					{"h",pe->m_uv.h}
				}}
			};

			if (pe->m_particle_sprite->meta.frames > 0) {
				asset["asset_data"]["meta"] = {
					{"animate", pe->m_particle_sprite->meta.animate},
					{"frames", pe->m_particle_sprite->meta.frames},
					{"delay",  pe->m_particle_sprite->meta.delay}
				};
			}
			out["layout"].push_back(asset);
		}
		for (auto& pair : sprite_assets) {
			nlohmann::json asset{};
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
				{"layer", ParseLayerR(layer)},
				{"transform", {
						{"x", (int)t.position.x}, {"y", (int)t.position.y},
						{"w", (int)t.scale.x}, {"h", (int)t.scale.y}
					}
				},
				{"uv", { {u.x, u.y}, {u.w, u.h} }}
			};
			if (pair.second->meta.frames > 0) {
				asset["asset_data"]["meta"] = {
					{"animate", pair.second->meta.animate},
					{"frames", pair.second->meta.frames},
					{"delay", pair.second->meta.delay}
				};
			}
			asset["id"] = pair.first;
			asset["schema"].push_back("image");
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