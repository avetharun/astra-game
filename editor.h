#ifndef __cw_engine_editor_handler_hpp
#define __cw_engine_editor_handler_hpp

#include "engine.hpp"
#include "imgui/imgui_uielement.h"
#include "imgui/imgui_format.h"
#include "imgui/imgui_stdlib.h"
Window::Component editor_component;

struct CWEditor {
	static inline MeshCollider2d* mesh = nullptr;
	enum {
		TRANSFORM_MOVE, TRANSFORM_SCALE, TRANSFORM_ROTATE,
		INSPECTOR_MESH, INSPECTOR_SPRITES, INSPECTOR_ASSETS
	};
};
int col_types[] = { COL_EMPTY, COL_ENT, COL_TRG, COL_OBJ, COL_SOLID, COL_CENTER};

int transform_type = CWEditor::TRANSFORM_MOVE;
int inspector_type = CWEditor::INSPECTOR_ASSETS;
int grid_size;
bool _col_show_rects, _col_show_lines{ 0 };
std::string new_sprite_filename;
bool needs_loading = true;
std::string title;
inline void editor_update() {
	if (!Window::WindowInstance->scene) { return; }
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(Window::WindowInstance->border_size_x, io.DisplaySize.y));
	switch (inspector_type) {
	default: {title = "Inspector"; break; }
	case CWEditor::INSPECTOR_MESH: {title = "Meshes"; break; }
	case CWEditor::INSPECTOR_SPRITES: {title = "Sprites"; break; }
	}
	ImGui::Begin((title + "###InspectorWindow").c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	if (ImGui::SmallButton("Meshes") || inspector_type == CWEditor::INSPECTOR_MESH) {
		inspector_type = CWEditor::INSPECTOR_MESH;
		ImGui::UnderlineLast();
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Sprites") || inspector_type == CWEditor::INSPECTOR_SPRITES) {
		inspector_type = CWEditor::INSPECTOR_SPRITES;
		ImGui::UnderlineLast();
	}
	ImGui::SameLine();

	if (ImGui::SmallButton("Assets") || inspector_type == CWEditor::INSPECTOR_ASSETS) {
		inspector_type = CWEditor::INSPECTOR_ASSETS;
		ImGui::UnderlineLast();
	}
	ImGui::SameLine();
	ImGui::PushFont(CWLGlobals::SymbolFont);
	if (ImGui::SmallButton("S")) {
		Window::WindowInstance->scene->Save();
	}

	ImGui::PopFont();
	ImGui::PopStyleVar(1);
	ImGui::BeginChild("INSPECTOR_CHILD", { 0, 0 }, true);
	{
		if (inspector_type == CWEditor::INSPECTOR_ASSETS) {

			if (ImGui::Button("p+##addparticle")) {
				ParticleEffect* e = new ParticleEffect(ParticleEffects::SNOW, 12);
				ParticleEffect::m_particle_arr.push_back(e);
			};
			ImGui::InputText(alib_strfmt("###ADDITEM_SPRITE"), &new_sprite_filename);
			ImGui::SameLine();
			if (ImGui::Button("i+##additem_btn") && alib_file_exists(new_sprite_filename.c_str())) {
				Sprite* s = new Sprite(new_sprite_filename.c_str());
				CWItem* i = new CWItem(s);
				i->data.set_string("name", "An item");
				Window::WindowInstance->scene->scene_placed_items.push_back(i);
			};
			auto& s_particles = ParticleEffect::m_particle_arr;
			for (int i = 0; s_particles.size() != 0 && i < s_particles.size() / 2; i++) {
				auto& particle = s_particles.at(i);
				if (ImGui::CollapsingHeader(alib_strfmt("%s###m_edit_particle_header%d", particle->name.c_str(), i))) {
					ImGui::InputText(alib_strfmt("###edit_name_particle%d", i), &particle->name);
					if (ImGui::InputInt(alib_strfmt("count###m_edit_particle_count%d", i), &particle->amt)) {
						particle->refresh();
					}
					ImGui::InputInt4(alib_strfmt("uv###m_edit_particle_yuv%d", i), &particle->m_uv.x);
					ImGui::SliderAngle(alib_strfmt("dir###m_edit_particle_direction%d", i), &particle->m_dir_angle_rad, 0);
					ImGui::SliderFloat(alib_strfmt("vel###m_edit_particle_velocity%d", i), &particle->m_velocity, 0, 10);
					ImGui::InputDouble2(alib_strfmt("pos###m_edit_particle_start_pos%d", i), (double*)&particle->start_pos.x);
					ImGui::InputFloat(alib_strfmt("lifetime###m_edit_particle_lifetime%d", i), &particle->m_lifetime);
				}
			}
			auto& s_items = Window::WindowInstance->scene->scene_placed_items;
			for (int i = 0; i < s_items.size(); i++) {
				auto& m_item = s_items.at(i);
				Sprite* s = m_item->ItemSprite;
				std::string _name = m_item->data.get_string("name");
				std::string _tooltip = m_item->data.get_string("tooltip");
				std::string _id = m_item->data.get_string("id");
				if (ImGui::CollapsingHeader(alib_strfmt("%s : item###m_edit_item_header%d", _name.c_str(), i))) {
					if (ImGui::SmallButton(alib_strfmt("--###m_edit_item_resummon__remove_item_btn#d", i))) {
						s_items.erase(s_items.begin() + i);
					}
					if (ImGui::SmallButton(alib_strfmt("resummon###m_edit_item_resummon%d", i))) {
						m_item->ItemSprite->setEnabled(true);
					}
					ImGui::InputText(alib_strfmt("name###edit_name_item%d", i), &_name);
					ImGui::InputText(alib_strfmt("tooltip###edit_tooltip_item%d", i), &_tooltip);
					ImGui::InputText(alib_strfmt("id###edit_id_item%d", i), &_id);
					m_item->data.set_string("name", _name);
					m_item->data.set_string("tooltip", _tooltip);
					m_item->data.set_string("id", _id);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 32);
					float m_pos = ImGui::GetCursorPosX();
					if (ImGui::CollapsingHeader(alib_strfmt("Sprite###m_edit_item_sprite%d", i))) {
						ImGui::SetCursorPosX(m_pos);
						ImGui::BeginChild(alib_strfmt("m_edit_item_sprite_child"));
						ImGui::InputText(alib_strfmt("id###m_edit_item_sprite_id%d", i), &s->identifier);
						ImGui::InputInt4(alib_strfmt("UV###m_edit_item_sprite_uv%d", i), &s->uv.x);
						ImGui::InputDouble2(alib_strfmt("Size###m_edit_item_sprite_size%d", i), &s->transform.scale.x);
						ImGui::InputInt(alib_strfmt("MFrames###m_edit_item_sprite_frames%d", i), &s->meta.frames);
						ImGui::InputFloat(alib_strfmt("MDelay###m_edit_item_sprite_delay%d", i), &s->meta.delay);
						ImGui::InputDouble2(alib_strfmt("Pos### edit_item_sprite_pos%d", i), &s->transform.position.x);
						ImGui::InputDouble2(alib_strfmt("Size###edit_item_sprite_size%d", i), &s->transform.scale.x);
						ImGui::EndChild();
					}
				}
			}
		}
		if (inspector_type == CWEditor::INSPECTOR_SPRITES) {
			auto& sprites = Window::WindowInstance->scene->sprite_assets;
			int _spriteoffset = 0;
			ImGui::InputText("filename", &new_sprite_filename);
			ImGui::SameLine();
			if (ImGui::Button("s+##addsprite") && alib_file_exists(new_sprite_filename.c_str())) {
				Sprite* s = (new Sprite(new_sprite_filename.c_str()))->withID(new_sprite_filename);
				Window::WindowInstance->scene->sprite_assets.push_back({ new_sprite_filename, s });
				s->insert();
			};
			for (int i = 0; i < sprites.size(); i++) {
				std::pair<std::string, Sprite*>& sprite = sprites.at(i);
				_spriteoffset++;
				if (!sprite.second) {
					continue;
				}
				if (ImGui::CollapsingHeader(alib_strfmt("%s###edit_sprite_header%d", sprite.first.c_str(), _spriteoffset))) {
					if (ImGui::SmallButton(alib_strfmt("--###r_s%d", _spriteoffset))) {
						if (Sprite::eraseElementByID(sprite.first)) {
							sprites.erase(sprites.begin() + alib_FindOffsetOfPairInVector(sprites, sprite.first));
						}
					} ImGui::SameLine();
					if (ImGui::SmallButton(alib_strfmt("++###d_s%d", _spriteoffset))) {
						sprites.push_back(std::make_pair(sprite.first + " copy", sprite.second->copy()->withID(sprite.first + " copy")));
						continue;
					}
					if (ImGui::InputText(alib_strfmt("id###edit_sprite_id%d", _spriteoffset), &sprite.first)) {
						sprite.second->identifier = sprite.first;
					};
					ImGui::InputInt4(alib_strfmt("UV###edit_sprite_uv%d", _spriteoffset), &sprite.second->uv.x);
					ImGui::InputDouble2(alib_strfmt("Pos###edit_sprite_pos%d", _spriteoffset), &sprite.second->transform.position.x);
					ImGui::InputDouble2(alib_strfmt("Size###edit_sprite_size%d", _spriteoffset), &sprite.second->transform.scale.x);
					ImGui::InputInt(alib_strfmt("MFrames###edit_sprite_frames%d", _spriteoffset), &sprite.second->meta.frames);
					ImGui::InputFloat(alib_strfmt("MDelay###edit_sprite_delay%d", _spriteoffset), &sprite.second->meta.delay);
				}
			}
		}
		if (inspector_type == CWEditor::INSPECTOR_MESH) {
			CWEditor::mesh = Window::WindowInstance->scene->thisMesh;
			if (CWEditor::mesh) {
				if (ImGui::Button("l+##addlin") && _col_show_lines) {
					CWEditor::mesh->lines.push_back(new MeshLine());
				}; ImGui::SameLine(); ImGui::Checkbox("##shwlin", &_col_show_lines);
				if (ImGui::Button("b+##addbox") && _col_show_rects) {
					new RectCollider2d();
				}; ImGui::SameLine(); ImGui::Checkbox("##shwrct", &_col_show_rects);

				for (int i = 0; i < RectCollider2d::_mGlobalColArr.size() && _col_show_rects; i++) {
					RectCollider2d* r = RectCollider2d::_mGlobalColArr.at(i);
					if (ImGui::CollapsingHeader(alib_strfmt("rect %d (%d, %d, %d, %d)###RECT%d", i, r->ws_rect->x, r->ws_rect->y, r->ws_rect->w, r->ws_rect->h, i))) {
						if (ImGui::SmallButton(alib_strfmt("++###add_r%d", i))) {
							RectCollider2d* nr = new RectCollider2d(r->rect, r->layer);
							nr->id = "copy of " + r->id;
						} ImGui::SameLine();
						if (ImGui::SmallButton(alib_strfmt("--###rem_r%d", i))) {
							RectCollider2d::_mGlobalColArr.erase(RectCollider2d::_mGlobalColArr.begin() + i);
						}
						if (ImGui::BeginCombo(alib_strfmt("###c_ty%d", i), coltohr(r->layer))) {

							for (int n = 0; n < IM_ARRAYSIZE(col_types); n++) {
								bool is_selected = (r->layer == col_types[n]); // You can store your selection however you want, outside or inside your objects
								if (ImGui::Selectable(coltohr(col_types[n]), is_selected)) {
									r->layer = col_types[n];
									if (is_selected) ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
								}
							}
							ImGui::EndCombo();
						}
						ImGui::PushItemWidth(100);
						ImGui::InputInt4(alib_strfmt("###rect_editor%d", i), &r->ws_rect->x); ImGui::SameLine();
						ImGui::PopItemWidth();
						ImGui::InputText(alib_strfmt("id###rectid_editor%d", i), &r->id);
					}
				}
				for (int i = 0; i < CWEditor::mesh->lines.size() && _col_show_lines; i++) {
					MeshLine* l = CWEditor::mesh->lines.at(i);
					if (ImGui::CollapsingHeader(alib_strfmt("line %d (%.0f, %.0f, %.0f, %.0f) ###LINE%d", i, l->start.x, l->start.y, l->end.x, l->end.y, i))) {
						l->is_editing = true;
						if (ImGui::SmallButton(alib_strfmt("++###a_l%d", i))) {
							CWEditor::mesh->lines.push_back(new MeshLine(*l));
						} ImGui::SameLine();
						if (ImGui::SmallButton(alib_strfmt("--###r_l%d", i))) {
							CWEditor::mesh->lines.erase(CWEditor::mesh->lines.begin() + i);
						}
						if (ImGui::BeginCombo(alib_strfmt("###l_ty%d", i), coltohr(l->layer))) {
							for (int n = 0; n < IM_ARRAYSIZE(col_types); n++) {
								bool is_selected = (l->layer == col_types[n]);
								if (ImGui::Selectable(coltohr(col_types[n]), is_selected)) {
									l->layer = col_types[n];
									if (is_selected) ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
						else {
							l->is_editing = false;
						}
						ImGui::PushItemWidth(100);
						ImGui::InputDouble(alib_strfmt("s###line_Startx%d", i), &l->start.x); ImGui::SameLine();
						ImGui::InputDouble(alib_strfmt("###line_Starty%d", i), &l->start.y);
						ImGui::InputDouble(alib_strfmt("e###line_Endx%d", i), &l->end.x); ImGui::SameLine();
						ImGui::InputDouble(alib_strfmt("###line_Endy%d", i), &l->end.y);
						ImGui::PopItemWidth();

						ImGui::InputText(alib_strfmt("id###lineid_editor%d", i), &l->id);
					}
				}
			}
		}
	}
	ImGui::EndChild();

	ImGui::End();
	if (!Window::WindowInstance->scene->editor_loaded) {
		needs_loading = true;
	}
}
void editor_render() {

}

Window::Initializer _nn{
	[&]() {
		editor_component.Update = []() {
			if (getbitv(Window::WindowInstance->data, 8)) {
				editor_update();
			}
		};
		editor_component.Render = []() {
			if (getbitv(Window::WindowInstance->data, 8)) {
				editor_render();
			}
		};
		Window::WindowInstance->AddComponent(&editor_component);
	}
};

#endif