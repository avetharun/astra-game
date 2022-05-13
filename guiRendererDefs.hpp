#ifndef __gui_renderer_definitions__hpp__
#define __gui_renderer_definitions__hpp__

#include <SDL2/SDL_ttf.h> 
#include "renderer.hpp"

#include "GUIRenderer.h"
namespace UI {
	void GUIRenderer::Render()
	{
		_element_arr.shrink_to_fit();
		for (ui_elem_offset = 0; ui_elem_offset < _element_arr.size(); ui_elem_offset++) {
			std::any tx_elem = _element_arr.at(ui_elem_offset);
			printf("%s\n", tx_elem.type().name());
			if (alib_costr(tx_elem.type().name(), "UI::TextElement *")) {
				TextElement* _e = std::any_cast<TextElement*>(tx_elem);
				if (!_e->enabled) { continue; }
				_e->Render();
				continue;
			}
			if (alib_costr(tx_elem.type().name(), "UI::ButtonElement *")) {
				ButtonElement* _e = std::any_cast<ButtonElement*>(tx_elem);
				if (!_e->enabled) { continue; }
				_e->Render();
				continue;
			}
			if (alib_costr(tx_elem.type().name(), "UI::ImageElement *")) {
				ImageElement* _e = std::any_cast<ImageElement*>(tx_elem);
				if (!_e->enabled) { continue; }
				_e->Render();
				continue;
			}
		}
	}
	void TextElement::Render() {
		if (this->size <= 0) { this->size = 1.0f; }
		ImGui::SetNextWindowPos({(float)this->pos.x-8, (float)this->pos.y-8});

		ImGui::Begin(_fmt_name().c_str(), 0, GUIRenderer::__elementWinFlags);
		ImGui::SetWindowFontScale(this->size);
		ImGui::TextColouredUnformatted(this->text.c_str());
		ImGui::End();
	}
	TextElement::TextElement() {
		_element_arr.push_back(this);
	}
	TextElement::TextElement(std::string _txt) {
		_element_arr.push_back(this);
		this->text = _txt;
	}

	ButtonElement::ButtonElement() { _element_arr.push_back(this); }
	ButtonElement::ButtonElement(std::string _txt, ButtonElementFlags _flags = {}) {
		_element_arr.push_back(this);
		this->text = _txt;
		this->buttonFlags = _flags;
	}
	void ButtonElement::Render() {
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
		ImGui::TextColouredUnformatted(this->text.c_str());
		if (this->buttonFlags & ButtonElementFlags::NoBackground) {
			ImGui::PopStyleColor(3);
		}
		if (__pressed) { this->onClick(); }
		ImGui::End();
	}
}
#endif