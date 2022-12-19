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
		INSPECTOR_MESH, INSPECTOR_SPRITES, INSPECTOR_ALL
	};
};
int col_types[] = { COL_EMPTY, COL_ENT, COL_TRG, COL_OBJ, COL_SOLID, COL_CENTER };

int transform_type = CWEditor::TRANSFORM_MOVE;
int inspector_type = CWEditor::INSPECTOR_ALL;
int grid_size;
bool _col_show_rects, _col_show_lines{ 0 };

bool needs_loading = true;
std::string title;
void editor_update() {
	if (!Window::WindowInstance->scene) { return; }
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(Window::WindowInstance->border_size_x, io.DisplaySize.y));
	switch (inspector_type) {
		default: {title = "Inspector"; break; }
		case CWEditor::INSPECTOR_MESH: {title = "Meshes"; break; }
		case CWEditor::INSPECTOR_SPRITES: {title = "Sprites"; break; }
	}
	ImGui::Begin((title+"###InspectorWindow").c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
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

	if (ImGui::SmallButton("Assets") || inspector_type == CWEditor::INSPECTOR_ALL) {
		inspector_type = CWEditor::INSPECTOR_ALL;
		ImGui::UnderlineLast();
	}
	ImGui::SameLine();
	ImGui::PushFont(CWLGlobals::SymbolFont);
	if (ImGui::SmallButton("S")) {
		Window::WindowInstance->scene->Save();
	}
	ImGui::PopFont();
	ImGui::PopStyleVar(1);
	ImGui::BeginChild("INSPECTOR_CHILD", { 0, 0}, true);
	{
		if (inspector_type == CWEditor::INSPECTOR_SPRITES) {
			std::map<std::string, Sprite*> sprites = Window::WindowInstance->scene->sprite_assets;
			int _spriteoffset = 0;
			for (std::pair<const std::string, Sprite*> sprite : sprites) {
				_spriteoffset++;
				if (ImGui::CollapsingHeader(alib_strfmt("%s###%d", sprite.first.c_str(), _spriteoffset))) {
					if (ImGui::SmallButton(alib_strfmt("++###a_s%d", _spriteoffset))) {
						std::string id = alib_strfmt("%s_%d", sprite.first.c_str(), _spriteoffset);
						Sprite* s = sprite.second->copy()->withID(id);
						s->insert();
						Window::WindowInstance->scene->sprite_assets.insert({ id, s });
					} ImGui::SameLine();
					if (ImGui::SmallButton(alib_strfmt("--###r_s%d", _spriteoffset))) {
						if (Sprite::eraseElementByID(sprite.first)) {
							Window::WindowInstance->scene->sprite_assets.erase(sprite.first);
						}
					}
					if (ImGui::InputText(alib_strfmt("id###edit_sprite_id%d", _spriteoffset), sprite.second->identifier, 40)) {
						sprite.second->setID(sprite.second->identifier);
						auto node = Window::WindowInstance->scene->sprite_assets.extract(sprite.first);
						node.key() = sprite.second->getID();
						Window::WindowInstance->scene->sprite_assets.insert(std::move(node));
					};
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
					if (ImGui::CollapsingHeader(alib_strfmt("rect %d (%d, %d, %d, %d)###RECT%d",i, r->ws_rect->x, r->ws_rect->y, r->ws_rect->w, r->ws_rect->h, i))) {
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
						ImGui::InputText(alib_strfmt("id###rectid_editor%d",i), &r->id);
					}
				}
				for (int i = 0; i < CWEditor::mesh->lines.size() && _col_show_lines; i++) {
					MeshLine* l = CWEditor::mesh->lines.at(i);
					if (ImGui::CollapsingHeader(alib_strfmt("line %d (%.0f, %.0f, %.0f, %.0f) ###LINE%d", i, l->start.x, l->start.y, l->end.x, l->end.y, i))) {
						l->is_editing = true;
						if (ImGui::SmallButton(alib_strfmt("++###a_l%d",i))) {
							CWEditor::mesh->lines.push_back(new MeshLine(*l));
						} ImGui::SameLine();
						if (ImGui::SmallButton(alib_strfmt("--###r_l%d", i))) {
							CWEditor::mesh->lines.erase(CWEditor::mesh->lines.begin() + i);
						}
						if (ImGui::BeginCombo(alib_strfmt("###l_ty%d",i), coltohr(l->layer))) {
							for (int n = 0; n < IM_ARRAYSIZE(col_types); n++) {
								bool is_selected = (l->layer == col_types[n]);
								if (ImGui::Selectable(coltohr(col_types[n]), is_selected)) {
									l ->layer = col_types[n];
									if (is_selected) ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
						else {
							l->is_editing = false;
						}
						ImGui::PushItemWidth(100);
						ImGui::InputDouble(alib_strfmt("s###line_Startx%d",i), &l->start.x); ImGui::SameLine();
						ImGui::InputDouble(alib_strfmt("###line_Starty%d",i), &l->start.y);
						ImGui::InputDouble(alib_strfmt("e###line_Endx%d",i), &l->end.x); ImGui::SameLine();
						ImGui::InputDouble(alib_strfmt("###line_Endy%d",i), &l->end.y);
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