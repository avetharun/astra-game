#ifndef __gui_renderer_definitions__hpp__
#define __gui_renderer_definitions__hpp__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h> 
#include "renderer.hpp"
#include "GUIRenderer.h"
namespace UI {
	void ButtonElement::Render() {
		if (!enabled) { return; }
		if (this->size == 0) { this->size = 1.0f; }
		ImGui::SetNextWindowPos({ (float)this->pos.x - 8, (float)this->pos.y - 12});
		ImGuiIO& __io = ImGui::GetIO();
		ImVec2 _v = ImGui::CalcTextSize(this->text.c_str());
		std::string _s = _fmt_name();
		ImGui::Begin(_s.c_str(), 0, GUIRenderer::__elementWinFlags);
		if (this->buttonFlags & ButtonElementFlags::NoBackground) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(0, 0, 0, 0)));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(ImColor(0, 0, 0, 0)));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(0, 0, 0, 0)));
		}

		bool __pressed= ImGui::Button(_fmt_name().c_str(), _v);
		ImGui::SameLine();
		ImGui::SetCursorPosX(this->pos.x + _v.x / 2);
		ImGui::SetWindowFontScale(this->size);
		ImGui::TextMulticolored(this->text.c_str());
		if (this->buttonFlags & ButtonElementFlags::NoBackground) {
			ImGui::PopStyleColor(3);
		}
		if (__pressed) { this->onClick(); }
		ImGui::End();
	}
}
#endif